#ifndef EVOLOCITY_CHASSIS_CONTROLLER_H
#define EVOLOCITY_CHASSIS_CONTROLLER_H

#include <Arduino.h>

class EVolocityChassisController {
public:
    EVolocityChassisController();

    void begin();
    void setThrottle(int value);   // 0–100 %
    void setBrake(int value);      // 0–100 %
    void setSteering(int value);   // -100 (full left) to +100 (full right)
    void stop();

    int getSpeed();                // returns current speed in km/h (placeholder)

private:
    int _throttle;
    int _brake;
    int _steering;
};

#endif
