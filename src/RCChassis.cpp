#include "RCChassis.h"

const byte RCChassis::_address[6] = "00001";

// ── Constructor ─────────────────────────────────────────────────────────────

RCChassis::RCChassis(
    uint8_t channel,
    uint8_t cePin, uint8_t csnPin,
    uint8_t servoPin, uint8_t enaPin, uint8_t in1Pin, uint8_t in2Pin,
    uint8_t battPin, uint8_t ledPin)
    : _radio(cePin, csnPin),
      _channel(channel),
      _servoPin(servoPin), _enaPin(enaPin), _in1Pin(in1Pin), _in2Pin(in2Pin),
      _battPin(battPin), _ledPin(ledPin),
      _battLow(false), _connected(false),
      _missCount(0), _lastBattCheckMs(0), _lastFlashMs(0), _ledState(false)
{
    _packet = {90, 0, 0};  // safe defaults: centred steering, motor off
}

// ── begin() ─────────────────────────────────────────────────────────────────

void RCChassis::begin() {
#ifdef EVOLOCITY_DEBUG
    Serial.begin(9600);
    EVPRINT("EVolocityLib RCChassis starting on channel");
    EVPRINTLN(_channel);
#endif

    // Servo
    _servo.attach(_servoPin);
    _servo.write(90);  // centre

    // Motor driver
    pinMode(_enaPin, OUTPUT);
    pinMode(_in1Pin, OUTPUT);
    pinMode(_in2Pin, OUTPUT);
    analogWrite(_enaPin, 0);
    digitalWrite(_in1Pin, LOW);
    digitalWrite(_in2Pin, LOW);

    // Status LED
    pinMode(_ledPin, OUTPUT);
    digitalWrite(_ledPin, LOW);

    // Radio — flash LED rapidly on hardware fault so students know immediately.
    if (!_radio.begin()) {
        EVPRINTLN("ERROR: Radio failed to initialise. Check wiring.");
        while (true) {
            digitalWrite(_ledPin, HIGH); delay(100);
            digitalWrite(_ledPin, LOW);  delay(100);
        }
    }
    _radio.setChannel(_channel);
    _radio.openReadingPipe(0, _address);
    _radio.setPALevel(RF24_PA_LOW);
    _radio.enableAckPayload();
    _radio.enableDynamicPayloads();
    _radio.startListening();

    _status = {0, false};
    _loadAckPayload();

    EVPRINTLN("RCChassis ready.");
}

// ── update() ─────────────────────────────────────────────────────────────────

void RCChassis::update() {
    unsigned long start    = millis();
    bool          received = false;

    while ((millis() - start) < _packetTimeoutMs) {
        if (_radio.available()) {
            _radio.read(&_packet, sizeof(_packet));
            received = true;
            break;
        }
    }

    if (received) {
        _missCount = 0;
        _connected = true;
        EVPRINT("Packet received — steering: "); EVPRINT(_packet.servoPos);
        EVPRINT("  speed: ");                    EVPRINT(_packet.motorSpeed);
        EVPRINT("  dir: ");                      EVPRINTLN(_packet.motorDir);
        _loadAckPayload();
    } else {
        if (_missCount < _maxMisses) _missCount++;
        if (_missCount >= _maxMisses) _connected = false;
        EVPRINT("No packet (miss ");
        EVPRINT(_missCount);
        EVPRINT("/");
        EVPRINT(_maxMisses);
        EVPRINTLN(_connected ? ")" : ") — DISCONNECTED");
    }

    // Battery check — at most once per second to avoid slowing the loop.
    if ((millis() - _lastBattCheckMs) >= 1000UL) {
        _lastBattCheckMs  = millis();
        int reading       = analogRead(_battPin);
        _battLow          = (reading < _battThreshold);
        _status.batteryADC = reading;
        _status.batteryLow = _battLow;
        EVPRINT("Battery ADC: "); EVPRINT(reading);
        EVPRINTLN(_battLow ? " — LOW" : " — OK");
        EVPRINT("Status queued — ADC: "); EVPRINT(_status.batteryADC);
        EVPRINTLN(_status.batteryLow ? "  battLow: true" : "  battLow: false");
        _loadAckPayload();
    }

    _updateLED();
}

// ── LED logic ────────────────────────────────────────────────────────────────

void RCChassis::_updateLED() {
    // Battery low takes priority — flash regardless of connection state.
    if (_battLow) {
        if ((millis() - _lastFlashMs) >= _flashIntervalMs) {
            _lastFlashMs = millis();
            _ledState    = !_ledState;
            digitalWrite(_ledPin, _ledState ? HIGH : LOW);
        }
        return;
    }
    // Battery OK — solid ON when connected, OFF when not.
    if (_connected) {
        digitalWrite(_ledPin, HIGH);
        _ledState = true;
    } else {
        digitalWrite(_ledPin, LOW);
        _ledState = false;
    }
}

// ── ACK payload ──────────────────────────────────────────────────────────────

void RCChassis::_loadAckPayload() {
    _radio.writeAckPayload(0, &_status, sizeof(_status));
}

// ── Getters ──────────────────────────────────────────────────────────────────

int RCChassis::getSteeringAngle() {
    return constrain(_packet.servoPos, 0, 180);
}

int RCChassis::getMotorSpeed() {
    return constrain(_packet.motorSpeed, 0, 255);
}

int RCChassis::getMotorDirection() {
    return constrain(_packet.motorDir, -1, 1);
}

// ── Setters ──────────────────────────────────────────────────────────────────

void RCChassis::setSteering(int angle) {
    angle = constrain(angle, 0, 180);
    EVPRINT("setSteering: "); EVPRINTLN(angle);
    _servo.write(angle);
}

void RCChassis::setMotor(int speed, int direction) {
    speed     = constrain(speed, 0, 255);
    direction = constrain(direction, -1, 1);
    EVPRINT("setMotor: speed="); EVPRINT(speed);
    EVPRINT(" dir=");            EVPRINTLN(direction);

    if      (speed == 0)      stop();
    else if (direction == 0)  coast();
    else if (direction == 1)  forward(speed);
    else                      reverse(speed);
}

void RCChassis::forward(int speed) {
    speed = constrain(speed, 0, 255);
    EVPRINT("forward: "); EVPRINTLN(speed);
    digitalWrite(_in1Pin, HIGH);
    digitalWrite(_in2Pin, LOW);
    analogWrite(_enaPin, speed);
}

void RCChassis::reverse(int speed) {
    speed = constrain(speed, 0, 255);
    EVPRINT("reverse: "); EVPRINTLN(speed);
    digitalWrite(_in1Pin, LOW);
    digitalWrite(_in2Pin, HIGH);
    analogWrite(_enaPin, speed);
}

void RCChassis::coast() {
    EVPRINTLN("coast()");
    digitalWrite(_enaPin, LOW);
}

void RCChassis::stop() {
    EVPRINTLN("stop()");
    // Hard brake — both IN pins LOW with ENA HIGH shorts motor terminals to GND.
    digitalWrite(_in1Pin, LOW);
    digitalWrite(_in2Pin, LOW);
    digitalWrite(_enaPin, HIGH);
}

// ── Status ────────────────────────────────────────────────────────────────────

bool RCChassis::isBatteryLow() {
    return _battLow;
}

bool RCChassis::isControllerConnected() {
    return _connected;
}
