#include <EVolocityChassisController.h>

EVolocityChassisController chassis;

void setup() {
    Serial.begin(9600);
    chassis.begin();
    Serial.println("EVolocity Chassis Controller ready");
}

void loop() {
    // Accelerate to 50% throttle
    chassis.setThrottle(50);
    chassis.setSteering(0);     // straight ahead
    delay(3000);

    // Steer right
    chassis.setSteering(30);
    delay(1000);

    // Brake to a stop
    chassis.stop();
    delay(2000);
}
