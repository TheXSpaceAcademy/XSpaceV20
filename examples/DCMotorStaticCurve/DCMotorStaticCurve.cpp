/*
This example demonstrates obtaining the static curve of a DC motor, useful for assessing its linear performance range.
The generated output is formatted for direct use within a MATLAB script, facilitating the plotting of the motor's static curve.

Author: PabloC
Date: 09/04/2024
Dependencies: <XSControl.h>

Install the library dependencies
- XSControl

To use the VSCode Serial Monitor with this project, update your platformio.ini file to include the following line:
monitor_speed = 1000000

This sets the baud rate to 1,000,000 bits per second, matching the configuration in the sketch.
*/

#include <Arduino.h>
#include <XSpaceV20.h>
#include <XSControl.h>

XSpaceV20Board XSBoard; // Create an instance of the XSpaceV20Board for motor control.
XSFilter Filter; // Create an instance of XSFilter for processing the motor's speed data.

// Constants for motor control
#define PWM_FREQUENCY 20000 // PWM frequency in Hz
#define ENCODER_RESOLUTION 1280 // Encoder resolution
#define DRV8837_POWER_SUPPLY 5 // Power supply voltage for DRV8837 motor driver

double speed; // Variable to hold the raw speed reading from the motor encoder.
double filtered_speed; // Variable to hold the filtered speed, providing a stable reading.

// Task dedicated to continuously filtering the motor speed.
void SpeedFilter(void *pv) {
  while(1) {
    // Fetch the current speed of the motor in degrees per second.
    speed = XSBoard.GetEncoderSpeed(E1,DEGREES_PER_SECOND);
    // Apply a second-order low-pass filter to smooth out the speed readings.
    filtered_speed = Filter.SecondOrderLPF(speed, 20, 0.001);
    vTaskDelay(1); // Pause for a short period before the next reading.
  }
  vTaskDelete(NULL); // Clean up the task if it ever exits the while loop.
}

void setup() {
  Serial.begin(1000000); // Start serial communication at 1000000 baud
  // Initialize the motor board with specified PWM frequency, encoder resolution, and power supply voltage
  XSBoard.init(PWM_FREQUENCY, ENCODER_RESOLUTION, DRV8837_POWER_SUPPLY);

  // Create a real-time task for filtering speed, ensuring it runs on core 1.
  xTaskCreate(SpeedFilter, "", 2000, NULL, 1, NULL);
}

void loop() {
  XSBoard.DRV8837_Wake(); // Activate the DRV8837 motor driver.

  // Prepare MATLAB for receiving data by clearing the workspace and figures.
  Serial.println("clc; clear; close all;");

  // Define a range of voltages to apply to the motor and initialize the speed array in MATLAB syntax.
  Serial.println("voltage = -4:0.5:4;");
  Serial.print("speed = [");
  
  // Sequentially apply each voltage in the defined range to the motor and log the filtered speed.
  for(double v = -4; v <= 4; v += 0.5) {
    XSBoard.DRV8837_Voltage(v); // Set motor voltage.
    delay(2000); // Wait for the speed to stabilize before reading.
    Serial.print(filtered_speed); // Output the filtered speed for MATLAB.
    Serial.print(" ");
  }

  // Finalize the MATLAB array and generate the plot command.
  Serial.println("];");
  Serial.println("plot(voltage, speed);");
  Serial.println("xlabel('Voltage (volts)', 'Interpreter', 'latex', 'FontSize', 14);");
  Serial.println("ylabel('Speed (degrees/sec)', 'Interpreter', 'latex', 'FontSize', 14);");
  Serial.println("title('Static Curve of DC Motor', 'Interpreter', 'latex', 'FontSize', 14);");
  
  XSBoard.DRV8837_Sleep(); // Deactivate the DRV8837 to conserve power.

  while(1); // Infinite loop to prevent re-execution of the code
}