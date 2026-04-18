#ifndef RC_CHASSIS_H
#define RC_CHASSIS_H

#include <Arduino.h>
#include <RF24.h>
#include <Servo.h>

class RCChassis {
public:
    // All pins default to the standard EVolocity motor board wiring.
    // Only change them if you have rewired your board.
    RCChassis(
        uint8_t cePin    = 7,
        uint8_t csnPin   = 9,
        uint8_t servoPin = 8,
        uint8_t enaPin   = 3,   // L298N PWM speed
        uint8_t in1Pin   = 5,
        uint8_t in2Pin   = 6,
        uint8_t battPin  = A0,
        uint8_t ledPin   = A3
    );

    // Call once in setup()
    void begin();

    // ── Call at the top of every loop() ────────────────────────────────────
    // Waits up to 500 ms for a packet from the handheld controller.
    // Also checks the battery and updates the status LED automatically.
    //   LED solid ON  = connected, battery OK
    //   LED solid OFF = no signal (disconnected)
    //   LED flashing  = connected, battery low — please charge!
    void waitForPacket();

    // ── Read values sent from the handheld controller ───────────────────────
    int  getSteeringAngle();     // steering angle in degrees (0 – 180)
    int  getMotorSpeed();        // motor speed (0 – 255)
    int  getMotorDirection();    // -1 = reverse, 0 = stop, 1 = forward

    // ── Send commands to the car ────────────────────────────────────────────
    void setSteering(int angle);              // 0 – 180 degrees
    void setMotor(int speed, int direction);  // speed 0–255, direction -1/0/1
    void stop();                              // cut motor instantly

    // ── Status helpers ──────────────────────────────────────────────────────
    bool isBatteryLow();           // true when battery needs charging
    bool isControllerConnected();  // true when radio packets are arriving

private:
    struct Packet {
        int servoPos;   // 0 – 180
        int motorSpeed; // 0 – 255
        int motorDir;   // -1, 0, +1
    };

    void _updateLED();

    RF24  _radio;
    Servo _servo;

    uint8_t _servoPin, _enaPin, _in1Pin, _in2Pin, _battPin, _ledPin;

    Packet        _packet;
    bool          _battLow;
    bool          _connected;
    uint8_t       _missCount;
    unsigned long _lastBattCheckMs;
    unsigned long _lastFlashMs;
    bool          _ledState;

    static const byte     _address[6];
    static const uint8_t  _battThreshold  = 211;  // ADC value ≈ 6.2 V
    static const uint16_t _packetTimeoutMs = 500;  // ms to wait per call
    static const uint8_t  _maxMisses      = 10;   // consecutive timeouts → disconnected
    static const uint16_t _flashIntervalMs = 250;  // low-batt LED flash rate
};

#endif
