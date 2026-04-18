/*
  EVolocityLib — RC Car Basic Usage
  ---------------------------------------
  This is all the code you need to drive the car!

  waitForPacket() does the hard work for you every loop:
    • Listens for a signal from the handheld controller
    • Checks the battery level
    • Updates the status LED:
        Solid ON  = connected, battery OK
        Solid OFF = no signal from controller
        Flashing  = connected, but battery needs charging
*/

// Uncomment the next line to print debug info to the Serial Monitor
// #define EVOLOCITY_DEBUG
#include <EVolocityLib.h>

// Set this number to match your handheld controller.
// Every car on the track must use a different channel (0–125).
RCChassis chassis(76);

void setup() {
  chassis.begin();
}

void loop() {
  // Wait for the latest command from the handheld controller.
  // This also handles the battery check and status LED.
  chassis.waitForPacket();

  // Read what the controller is asking for...
  int angle     = chassis.getSteeringAngle();   // 0 to 180 degrees
  int speed     = chassis.getMotorSpeed();       // 0 to 255
  int direction = chassis.getMotorDirection();   // -1, 0, or 1

  // ...and make the car do it.
  chassis.setSteering(angle);
  chassis.setMotor(speed, direction);
}
