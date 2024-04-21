# Changelog

This file documents all significant changes made to the XSpaceV20 library.

## [Version 1.0.8]

#### Fixed
- General update

## [Version 1.0.7] - 2024-04-14

#### Added
- Examples to obtain Static and Dynamic curve of DC Motors

#### Improved
- Optimization of encoder interrupts for more accurate and efficient period and event counter calculations.

## [Version 1.0.2] - 2024-04-10

#### Added
- Implementation of DRV8837 motor controllers, including methods to sleep and wake the device, as well as to configure the operating voltage.
- Full support for encoders, allowing initialization and speed and position readings in different units (degrees, radians, degrees per second, radians per second).
- Functions for WiFi and MQTT connectivity, enabling the board to connect to WiFi networks, publish and subscribe to MQTT topics, and maintain an active connection.

#### Improved
- Optimization of encoder interrupts for more accurate and efficient period and event counter calculations.
- Refactoring of WiFi and MQTT initialization and handling to improve connection stability and automatic reconnection to MQTT brokers.

#### Fixed
- Correction in the management of negative voltages in `DRV8837_Voltage` to ensure proper application of reverse PWM.

#### Internal Changes
- Reduction in the use of global variables by encapsulating them within more cohesive methods and data structures.
- Improved code documentation to enhance readability and facilitate long-term maintenance.

## [Version 1.0.0] - 2024-04-10

#### Added
- Initial release of the XSpace V20 library implemented for XSpace v2.0 board
- Basic functions for handling encoders and motor controllers.

#### Notes
- This is the first public implementation, designed for use in robotics and automation projects that require precise encoder readings and motor control via standard protocols like WiFi and MQTT.