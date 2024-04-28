# XSpaceV20 Library

Welcome to the `XSpaceV20` library, designed to simplify the development of projects using the XSpace V2.0 Boards. This library provides an easy-to-use interface for controlling motors, reading sensors, and managing connectivity, making it ideal for education, hobby projects, and rapid prototyping.

## Features

- Motor control support with the DRV8837 driver
- Enhanced sensor readings for a variety of inputs
- Easy integration with IoT services using PubSubClient for MQTT
- Support for ESP32-based XSpace V2.0 Boards

## Installation

### PlatformIO

To use `XSpaceV20` with PlatformIO, add the following line to your `platformio.ini` file under the `lib_deps` section:

```ini
lib_deps =
  https://github.com/TheXSpaceAcademy/XSpaceV20.git#main
```

### Arduino IDE

For Arduino IDE, you can clone this repository and place it in your Arduino libraries folder, or you can download the zip and import it directly into the Arduino IDE.

## Quick Start

Here's a simple example to get you started with the `XSpaceV20` library. This example initializes the board and blinks an onboard LED:

```cpp
#include <XSpaceV20.h>

XSpaceV20Board XSBoard; // Create an instance of the XSpaceV20Board for motor control.

// Constants for motor control
#define PWM_FREQUENCY 20000 // PWM frequency in Hz
#define ENCODER_RESOLUTION 960 // Encoder resolution
#define DRV8837_POWER_SUPPLY 5 // Power supply voltage for DRV8837 motor driver

void setup() {
  // Initialize the motor board with specified PWM frequency, encoder resolution, and power supply voltage
  XSBoard.init(PWM_FREQUENCY, ENCODER_RESOLUTION, DRV8837_POWER_SUPPLY);
}

void loop() {
  XSBoard.DRV8837_Wake(); // Activate the DRV8837 motor driver
  XSBoard.DRV8837_Voltage(2.5); //Apply 2.5 volts to DC Motor
}
```

## Documentation

For detailed documentation, including setup instructions, API reference, and examples, please visit our [GitHub repository](https://github.com/TheXSpaceAcademy/XSpaceV20).

## Contributing

We welcome contributions to the `XSpaceV20` library! If you have suggestions, bug reports, or contributions, please submit them as issues or pull requests on GitHub.

## License

`XSpaceV20` is licensed under the MIT License. See the [LICENSE](https://github.com/TheXSpaceAcademy/XSpaceV20/blob/main/LICENSE) file for more details.

## Acknowledgments

A big thank you to everyone who has contributed to this project. Your support helps make `XSpaceV20` better for everyone.
