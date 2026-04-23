#include "RCRemote.h"

const byte RCRemote::_address[6] = "00001";

// ── Constructor ─────────────────────────────────────────────────────────────

RCRemote::RCRemote(
    uint8_t channel,
    uint8_t cePin, uint8_t csnPin,
    uint8_t steerPin, uint8_t drivePin, uint8_t battPin, uint8_t ledPin)
    : _radio(cePin, csnPin),
      _channel(channel),
      _steerPin(steerPin), _drivePin(drivePin), _battPin(battPin), _ledPin(ledPin),
      _connected(false), _remoteBattLow(false),
      _missCount(0), _ledPhase(0), _ledMode(LED_OFF), _ledState(false),
      _lastBattCheckMs(0), _lastFlashMs(0)
{
    _outgoing = {90, 0, 0};
    _carStatus = {0, false};
}

// ── begin() ─────────────────────────────────────────────────────────────────

void RCRemote::begin() {
#ifdef EVOLOCITY_DEBUG
    Serial.begin(9600);
    EVPRINT("RCRemote starting on channel ");
    EVPRINTLN(_channel);
#endif

    pinMode(_ledPin, OUTPUT);
    digitalWrite(_ledPin, LOW);

    if (!_radio.begin()) {
        EVPRINTLN("ERROR: Radio failed to initialise. Check wiring.");
        while (true) {
            digitalWrite(_ledPin, HIGH); delay(100);
            digitalWrite(_ledPin, LOW);  delay(100);
        }
    }
    _radio.setChannel(_channel);
    _radio.openWritingPipe(_address);
    _radio.setPALevel(RF24_PA_LOW);
    _radio.enableAckPayload();
    _radio.enableDynamicPayloads();
    _radio.stopListening();

    EVPRINTLN("RCRemote ready.");
}

// ── update() ─────────────────────────────────────────────────────────────────

void RCRemote::update() {
    // Read and linearise pots (compensates for voltage-divider nonlinearity)
    int steerLinear = _linearizePot(analogRead(_steerPin));  // 0–511
    int driveLinear = _linearizePot(analogRead(_drivePin));  // 0–511

    // Steering: full pot travel → 0–180°
    _outgoing.servoPos = map(steerLinear, 0, 511, 0, 180);

    // Drive: pot centre (256) = stop, with dead band either side
    int drive = driveLinear - 256;
    if (abs(drive) < _deadzone) {
        _outgoing.motorDir   = 0;
        _outgoing.motorSpeed = 0;
    } else if (drive < 0) {
        _outgoing.motorDir   = -1;
        _outgoing.motorSpeed = constrain(map(-drive, _deadzone, 256, 0, 255), 0, 255);
    } else {
        _outgoing.motorDir   = 1;
        _outgoing.motorSpeed = constrain(map(drive, _deadzone, 255, 0, 255), 0, 255);
    }

    EVPRINT("TX — servo: "); EVPRINT(_outgoing.servoPos);
    EVPRINT("  speed: ");    EVPRINT(_outgoing.motorSpeed);
    EVPRINT("  dir: ");      EVPRINTLN(_outgoing.motorDir);

    // Transmit and read ACK payload from car
    bool ack = _radio.write(&_outgoing, sizeof(_outgoing));
    if (ack) {
        _missCount = 0;
        _connected = true;
        if (_radio.isAckPayloadAvailable()) {
            _radio.read(&_carStatus, sizeof(_carStatus));
            EVPRINT("Car status — ADC: "); EVPRINT(_carStatus.batteryADC);
            EVPRINTLN(_carStatus.batteryLow ? "  battLow: true" : "  battLow: false");
        }
    } else {
        if (_missCount < _maxMisses) _missCount++;
        if (_missCount >= _maxMisses) _connected = false;
        EVPRINT("TX fail (miss "); EVPRINT(_missCount);
        EVPRINT("/");              EVPRINT(_maxMisses);
        EVPRINTLN(_connected ? ")" : ") — DISCONNECTED");
    }

    // Remote battery check — at most once per second
    if ((millis() - _lastBattCheckMs) >= 1000UL) {
        _lastBattCheckMs = millis();
        int reading      = analogRead(_battPin);
        _remoteBattLow   = (reading < _battThreshold);
        EVPRINT("Remote battery ADC: "); EVPRINT(reading);
        EVPRINTLN(_remoteBattLow ? " — LOW" : " — OK");
    }

    _updateLED();
}

// ── LED logic ────────────────────────────────────────────────────────────────

void RCRemote::_updateLED() {
    // Priority: remote batt > car batt > connected > disconnected
    LEDMode target;
    if      (_remoteBattLow)        target = LED_SINGLE;
    else if (_carStatus.batteryLow) target = LED_DOUBLE;
    else if (_connected)            target = LED_SOLID;
    else                            target = LED_OFF;

    // Reset phase on mode change
    if (target != _ledMode) {
        _ledMode     = target;
        _ledPhase    = 0;
        _lastFlashMs = millis();
    }

    if (_ledMode == LED_OFF) {
        digitalWrite(_ledPin, LOW);
        _ledState = false;
        return;
    }
    if (_ledMode == LED_SOLID) {
        digitalWrite(_ledPin, HIGH);
        _ledState = true;
        return;
    }

    // Flashing modes — advance phase when its duration has elapsed
    uint16_t phaseDuration;
    if (_ledMode == LED_SINGLE) {
        phaseDuration = (_ledPhase == 0) ? _flashOnMs : _flashPauseMs;
    } else {
        switch (_ledPhase) {
            case 0:  phaseDuration = _flashOnMs;   break;
            case 1:  phaseDuration = _flashGapMs;  break;
            case 2:  phaseDuration = _flashOnMs;   break;
            default: phaseDuration = _flashPauseMs; break;
        }
    }

    if ((millis() - _lastFlashMs) >= phaseDuration) {
        _lastFlashMs = millis();
        _ledPhase    = (_ledPhase + 1) % (_ledMode == LED_SINGLE ? 2 : 4);
    }

    // LED is ON during phase 0 only (single) or phases 0 and 2 (double)
    bool on = (_ledPhase == 0) || (_ledMode == LED_DOUBLE && _ledPhase == 2);
    if (on != _ledState) {
        _ledState = on;
        digitalWrite(_ledPin, on ? HIGH : LOW);
    }
}

// ── Pot linearisation ────────────────────────────────────────────────────────

int RCRemote::_linearizePot(int raw) {
    // The remote board has a voltage divider: R17 (10 K fixed) in series with
    // RV2 (10 K linear pot, wiper→GND, far end floating). This means the ADC
    // reading is nonlinear: ADC = 1023 × R_pot / (10000 + R_pot).
    // Inverting gives true pot position ∝ raw / (1023 − raw), scaled to 0–511.
    if (raw <= 0)    return 0;
    if (raw >= 1023) return 511;
    return (int)((long)raw * 511 / (1023 - raw));
}

// ── Status ───────────────────────────────────────────────────────────────────

bool RCRemote::isCarConnected()     { return _connected; }
bool RCRemote::isRemoteBatteryLow() { return _remoteBattLow; }
bool RCRemote::isCarBatteryLow()    { return _carStatus.batteryLow; }
int  RCRemote::getCarBatteryADC()   { return _carStatus.batteryADC; }
