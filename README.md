# EVolocityChassisController

An Arduino library for controlling the EVolocity educational electric-vehicle chassis.

## Features

- Throttle, brake, and steering control
- Speed feedback (placeholder — wire to your hardware)
- Compatible with all Arduino-compatible boards (`architectures=*`)

## Installation

Install via the Arduino Library Manager: search for **EVolocityChassisController**.

Or manually: download this repository as a ZIP and use  
**Sketch → Include Library → Add .ZIP Library…**

## Usage

```cpp
#include <EVolocityChassisController.h>

EVolocityChassisController chassis;

void setup() {
    chassis.begin();
}

void loop() {
    chassis.setThrottle(50);   // 50 % throttle
    chassis.setSteering(0);    // straight
    delay(2000);
    chassis.stop();
    delay(2000);
}
```

## API

| Method | Description |
|--------|-------------|
| `begin()` | Initialise hardware interfaces |
| `setThrottle(int 0–100)` | Set drive power percentage |
| `setBrake(int 0–100)` | Set braking percentage |
| `setSteering(int -100–100)` | Set steering (-100 = full left, +100 = full right) |
| `stop()` | Cut throttle and apply full brake |
| `getSpeed()` | Returns current speed in km/h |

## License

MIT — see [LICENSE](LICENSE)
