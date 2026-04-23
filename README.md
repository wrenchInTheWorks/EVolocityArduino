# EVolocityLib

The official Arduino library for the [EVolocity](https://evolocity.co.nz/) solar car student challenge.

EVolocity runs competitive engineering events where students design, build, and race solar-powered vehicles. This library provides the Arduino building blocks for those vehicles — one install covers all EVolocity hardware projects so students never need to manage multiple libraries.

## Projects

| Project | Description |
|---------|-------------|
| RC Car Challenge | Students build and program a radio-controlled car using the `RCChassis` class |

---

## Getting Started

### 1. Install dependencies

Open the Arduino IDE, go to **Sketch → Include Library → Manage Libraries** and install:

| Library | Version |
|---------|---------|
| RF24 | latest |

### 2. Install EVolocityLib

Download the latest release ZIP from the [Releases page](../../releases/latest).

Then in the Arduino IDE go to **Sketch → Include Library → Add .ZIP Library** and select the downloaded file.

### 3. Open an example

Go to **File → Examples → EVolocityLib** and choose the example for your project. Each example has comments explaining every line.

---

## Documentation & Help

Full documentation, wiring guides, and project walkthroughs are on the **[GitHub Wiki](../../wiki)**.  
That's the best place to start if you're a student trying to understand how something works.

---

## Contributing

If you're making changes to the library — fixing a bug, adding a class, updating CI — please read **[CONTRIBUTING.md](CONTRIBUTING.md)** first.  
It covers the branching workflow, PR requirements, versioning, and how the CI checks work.

---

## License

MIT — see [LICENSE](LICENSE)
