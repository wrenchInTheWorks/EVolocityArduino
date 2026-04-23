/*
  EVolocityLib — RC Remote Basic Usage
  --------------------------------------
  This is the code for the HANDHELD CONTROLLER. You also need the car
  code loaded on the chassis board.

  remote.update() does the hard work for you every loop:
    • Reads the steering and drive joystick potentiometers
    • Transmits a control packet to the car
    • Reads the car's battery status sent back automatically
    • Updates the status LED:
        Solid ON      = connected, both batteries OK
        Solid OFF     = no connection to car
        Single flash  = remote battery low — please charge!
        Double flash  = car battery low — please charge!
        Rapid flash   = hardware fault — check radio wiring
*/

// Uncomment the next line to print debug info to the Serial Monitor
// #define EVOLOCITY_DEBUG
#include <EVolocityLib.h>

// Set this number to match your car.
// Every car on the track must use a different channel (0–125).
RCRemote remote(76);

void setup() {
  remote.begin();
}

void loop() {
  // Update the remote — reads pots, transmits to the car,
  // receives car battery status, and updates the LED automatically.
  remote.update();

  // Want to know what's happening? Try these:
  //   remote.isCarConnected()      — true when the car is receiving
  //   remote.isRemoteBatteryLow()  — true when the remote needs charging
  //   remote.isCarBatteryLow()     — true when the car needs charging
  //   remote.getCarBatteryADC()    — raw battery reading from the car (0–1023)
}
