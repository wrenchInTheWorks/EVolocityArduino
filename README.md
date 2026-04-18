# EVolocityChassisController

An Arduino library that makes controlling the EVolocity RC car chassis as simple as possible for students aged 10–12.

## What it does for you automatically

Call `chassis.waitForPacket()` at the top of `loop()` and the library handles:

- **Receiving** the latest steering and speed packet from the handheld controller
- **Checking the battery** level once per second
- **Updating the status LED** on the board:

| LED | Meaning |
|-----|---------|
| Solid ON | Connected to controller, battery OK |
| Solid OFF | No signal from controller |
| Flashing | Connected, but battery needs charging |

Connection is considered lost after 10 consecutive gaps of more than 500 ms between packets.

## Installation

Search for **EVolocityChassisController** in the Arduino Library Manager.

> **Requires the [RF24](https://github.com/nRF24/RF24) library** — install it from the Library Manager too.

## The simplest possible program

```cpp
#include <EVolocityChassisController.h>

EVolocityChassisController chassis;

void setup() {
  chassis.begin();
}

void loop() {
  chassis.setSteering(chassis.getSteeringAngle());
  chassis.setMotor(chassis.getMotorSpeed(), chassis.getMotorDirection());
}
```

That's it. The car will follow the controller.

## Full API

### Setup & background tasks
| Function | What it does |
|----------|--------------|
| `chassis.begin()` | Start everything up. Call once in `setup()`. |
| `chassis.waitForPacket()` | Wait for a packet from the controller (up to 500 ms). Also checks battery and updates the status LED. Call at the top of `loop()`. |

### Reading the controller
| Function | Returns | Description |
|----------|---------|-------------|
| `chassis.getSteeringAngle()` | `int` 0 – 180 | Steering angle the controller is asking for |
| `chassis.getMotorSpeed()` | `int` 0 – 255 | Motor speed the controller is asking for |
| `chassis.getMotorDirection()` | `int` -1 / 0 / 1 | -1 = reverse, 0 = stop, 1 = forward |

### Controlling the car
| Function | Description |
|----------|-------------|
| `chassis.setSteering(angle)` | Turn the wheels to the given angle (0 – 180) |
| `chassis.setMotor(speed, direction)` | Set the motor speed (0 – 255) and direction (-1 / 0 / 1) |
| `chassis.stop()` | Cut the motor immediately |

### Status
| Function | Returns | Description |
|----------|---------|-------------|
| `chassis.isBatteryLow()` | `bool` | `true` when the battery needs charging |
| `chassis.isControllerConnected()` | `bool` | `true` when radio packets are arriving |

## Wiring (default pins)

| Signal | Arduino Pin |
|--------|-------------|
| RF24 CE | 7 |
| RF24 CSN | 9 |
| Servo | 8 |
| L298N ENA (PWM) | 3 |
| L298N IN1 | 5 |
| L298N IN2 | 6 |
| Battery voltage divider | A0 |
| Low-battery LED | A3 |

Pin assignments can be changed by passing them to the constructor — see the header file for details.

## Compatibility

Compatible with any Arduino board (`architectures=*`). Tested on Uno and Nano.

## License

MIT — see [LICENSE](LICENSE)
