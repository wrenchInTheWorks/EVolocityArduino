/*
  EVolocityLib — RC Chassis Basic Usage
  ---------------------------------------
  This is the code for the CAR. You also need the remote control
  code loaded on the handheld controller.

  chassis.update() does the hard work for you every loop:
    • Listens for a signal from the handheld controller
    • Checks the battery level
    • Updates the status LED:
        Solid ON     = connected, battery OK
        Solid OFF    = no signal from controller
        Slow flash   = battery needs charging
        Fast flash   = hardware fault — check radio wiring
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
  // Update the chassis — checks for new commands, reads the battery,
  // and updates the status LED automatically.
  chassis.update();

  // Read what the controller is asking for...
  int angle     = chassis.getSteeringAngle();   // 0 to 180 degrees
  int speed     = chassis.getMotorSpeed();       // 0 to 255
  int direction = chassis.getMotorDirection();   // -1, 0, or 1

  // ...and make the car do it.
  chassis.setSteering(angle);
  chassis.setMotor(speed, direction);

  // setMotor() already handles these automatically:
  //   speed == 0     → hard brake  (motor stops immediately)
  //   direction == 0 → coast       (motor spins down on its own)
  //
  // Want more control? Call these directly instead of setMotor():
  //   chassis.forward(speed)  — drive forward at speed (0–255)
  //   chassis.reverse(speed)  — drive in reverse at speed (0–255)
  //   chassis.stop()          — hard brake
  //   chassis.coast()         — free wheel
}
