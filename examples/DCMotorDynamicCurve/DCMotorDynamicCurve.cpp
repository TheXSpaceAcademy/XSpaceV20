/*
This example demonstrates how to obtain the dynamic curve of a DC motor.
The code generates MATLAB-compatible commands for plotting the dynamic performance curve of the motor.

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

// Definition of methods for the board and filter
XSpaceV20Board XSBoard;
XSFilter Filter;

// Constants for motor control
#define PWM_FREQUENCY 20000 // PWM frequency in Hz
#define ENCODER_RESOLUTION 1280 // Encoder resolution
#define DRV8837_POWER_SUPPLY 5 // Power supply voltage for DRV8837 motor driver

// Variables to store speed measurements
double speed;
double filtered_speed;

// Task for filtering the motor encoder's speed measurement
void SpeedFilter(void *pv){
  while(1){
    // Measure speed in degrees per second
    speed = XSBoard.GetEncoderSpeed(E1,DEGREES_PER_SECOND);
    // Filter the speed using a second-order low-pass filter
    filtered_speed = Filter.SecondOrderLPF(speed, 20, 0.001);
    vTaskDelay(1); // 1ms delay between measurements
  }
  vTaskDelete(NULL); // Delete the task if the loop ends
}

void setup() {
  Serial.begin(1000000); // Start serial communication at 115200 baud
  // Initialize the motor board with specified PWM frequency, encoder resolution, and power supply voltage
  XSBoard.init(PWM_FREQUENCY, ENCODER_RESOLUTION, DRV8837_POWER_SUPPLY);
 
  // Create a real-time operating system task for speed filtering
  xTaskCreate(SpeedFilter, "Filter", 2000, NULL, 1, NULL);
}

void loop() {
  XSBoard.DRV8837_Wake(); // Wake the DRV8837 motor driver from low power mode

  // Commands to clear, close, and prepare the MATLAB plotting environment
  Serial.println("");
  Serial.println("clc; clear; close all;");

  // Preparing the time array and initializing the speed array for MATLAB
  Serial.println("time = 0:0.001:3.999;");
  Serial.println("voltage = [1.5*ones(1,1000) -2*ones(1,1000) -3*ones(1,1000) 2.5*ones(1,1000)];");
  Serial.print("speed = [");
  
  // Apply different voltages to the motor at time intervals and log the filtered speed
  for(int i = 0; i < 4000; i++){
    if(i == 0) XSBoard.DRV8837_Voltage(1.5);
    if(i == 1000) XSBoard.DRV8837_Voltage(-2);
    if(i == 2000) XSBoard.DRV8837_Voltage(-3);
    if(i == 3000) XSBoard.DRV8837_Voltage(2.5);
    
    Serial.print(filtered_speed);
    Serial.print(" ");
    delay(1);
  }

  // Conclude MATLAB commands for plotting
  Serial.println("];");
  Serial.println("plot(time, speed);");
  Serial.println("xlabel('Time (seconds)', 'Interpreter', 'latex', 'FontSize', 14);");
  Serial.println("ylabel('Speed (degrees/sec)', 'Interpreter', 'latex', 'FontSize', 14);");
  Serial.println("title('Dynamic Curve of DC Motor', 'Interpreter', 'latex', 'FontSize', 14);");
  
  XSBoard.DRV8837_Sleep(); // Put the DRV8837 motor driver into low power mode

  while(1); // Infinite loop to prevent re-execution of the code
}