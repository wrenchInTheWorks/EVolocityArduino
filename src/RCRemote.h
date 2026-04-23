#ifndef RC_REMOTE_H
#define RC_REMOTE_H

#include <Arduino.h>
#include <RF24.h>
#include "RCChassis.h"  // ChassisStatus struct + EVPRINT macros

class RCRemote {
public:
    // channel   — RF24 radio channel (0–125). Must match the car's channel.
    //
    // All pin parameters default to the standard EVolocity remote board wiring.
    // Only change them if you have rewired your board.
    RCRemote(
        uint8_t channel  = 76,
        uint8_t cePin    = 7,
        uint8_t csnPin   = 9,
        uint8_t steerPin = A0,
        uint8_t drivePin = A1,
        uint8_t battPin  = A7,
        uint8_t ledPin   = 6
    );

    // Call once in setup()
    void begin();

    // ── Call at the top of every loop() ────────────────────────────────────
    // Reads the joystick pots, transmits a control packet to the car, reads
    // the car's battery status from the ACK payload, and updates the LED.
    //   LED solid ON      = connected, both batteries OK
    //   LED solid OFF     = no connection to car
    //   LED single flash  = remote battery low — please charge!
    //   LED double flash  = car battery low — please charge!
    //   LED rapid flash   = hardware fault — check radio wiring (begin() only)
    void update();

    // ── Status helpers ──────────────────────────────────────────────────────
    bool isCarConnected();       // true when the car is receiving packets
    bool isRemoteBatteryLow();   // true when the remote battery needs charging
    bool isCarBatteryLow();      // true when the car battery needs charging
    int  getCarBatteryADC();     // raw ADC reading from the car (0–1023)

private:
    struct ControlPacket {
        int servoPos;   // 0–180
        int motorSpeed; // 0–255
        int motorDir;   // -1, 0, +1
    };

    enum LEDMode : uint8_t { LED_OFF, LED_SOLID, LED_SINGLE, LED_DOUBLE };

    void _updateLED();
    int  _linearizePot(int raw);  // corrects voltage-divider nonlinearity

    RF24 _radio;

    uint8_t _channel;
    uint8_t _steerPin, _drivePin, _battPin, _ledPin;

    ControlPacket _outgoing;
    ChassisStatus _carStatus;

    bool    _connected;
    bool    _remoteBattLow;
    uint8_t _missCount;
    uint8_t _ledPhase;
    LEDMode _ledMode;
    bool    _ledState;

    unsigned long _lastBattCheckMs;
    unsigned long _lastFlashMs;

    static const byte     _address[6];
    static const uint8_t  _battThreshold  = 211;   // ADC ≈ 6.2 V
    static const uint8_t  _maxMisses      = 10;    // consecutive TX fails → disconnected
    static const uint8_t  _deadzone       = 10;    // pot centre dead band
    static const uint16_t _flashOnMs      = 150;   // LED ON pulse duration
    static const uint16_t _flashGapMs     = 150;   // gap between double-flash pulses
    static const uint16_t _flashPauseMs   = 700;   // OFF period after each pattern
};

#endif
