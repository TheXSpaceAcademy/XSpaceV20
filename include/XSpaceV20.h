/*
	XSpace V20 Library
	Author: Pablo Cardenas
	Description: An open-source library designed for advanced robotics projects. Please remember to reference this library in your projects!
*/

#ifndef XSPACEV20_H
#define XSPACEV20_H

#include <Arduino.h>

#define DRV8837_IN1 32
#define DRV8837_IN2 33
#define DRV8837_nSLEEP 25

#define encoder_CHA 34
#define encoder_CHB 35
#define E1 0
#define E2 1

#define DEGREES_PER_SECOND 1
#define RADS_PER_SECOND 2

#define DEGREES 1
#define RADS 2

struct XSEncoder{
	int resolution;
	int channelA;
	int channelB;
};

struct XSDRV8837{
	int IN1;
	int IN2;
	int nSLEEP;

	int PWM_CH_IN1;
	int PWM_CH_IN2;
	double Vm;
};

class XSpaceV20Board{
	private:
		double _vel_ant = 0;
		bool   _xspace_info = false;

		XSDRV8837 DRV8837;

	public:
		/* Initializes the board and motor driver
			@param frequency PWM frequency for the DRV8837 driver (in Hz)
			@param resolution Encoder pulses per revolution * gearmotor reduction
			@param VM DRV8837 driver power supply voltage (range 0v to 11v)
		*/
		void init(int frequency, double resolution, double VM);

		/* Initializes encoder1 settings
			@param enc_CHAx Channel A pin number
			@param enc_CHBx Channel B pin number
			@param resolutionx Encoder resolution
			@param mode Encoding mode (RISING, FALLING)
		*/
		void encoder1_init(int enc_CHAx, int enc_CHBx, int resolutionx, int mode);

		/* Initializes encoder2 settings
			@param enc_CHAx Channel A pin number
			@param enc_CHBx Channel B pin number
			@param resolutionx Encoder resolution
			@param mode Encoding mode (RISING, FALLING)
		*/
		void encoder2_init(int enc_CHAx, int enc_CHBx, int resolutionx, int mode);

		/* Initializes DRV8837 motor driver
			@param IN1x Pin connected to IN1 on DRV8837
			@param CH_IN1x PWM channel for IN1
			@param IN2x Pin connected to IN2 on DRV8837
			@param CH_IN2x PWM channel for IN2
			@param nSLEEPx Pin connected to nSLEEP on DRV8837
			@param frequencyx PWM frequency
			@param Vmx Operating voltage for the DRV8837
		*/
		void DRV8837_init(int IN1x, int CH_IN1x, int IN2x, int CH_IN2x, int nSLEEPx,int frequencyx, int Vmx);
		
		/* Puts the DRV8837 motor driver into sleep mode, reducing power consumption.
		*/
		void DRV8837_Sleep();

		/* Wakes the DRV8837 motor driver from sleep mode, enabling normal operation.
		*/
		void DRV8837_Wake();

		/* Sets the operating voltage for the DRV8837 motor driver.
		   @param vp The voltage to be set, which controls the motor speed and power (range 0v to 11v).
		*/
		void DRV8837_Voltage(double vp);
		
		/* Retrieves the speed of the specified encoder
			@param encoder Encoder number (E1 or E2)
			@param mode Unit of measurement (DEGREES_PER_SECOND, RADS_PER_SECOND)
			@return Current speed of the encoder
		*/
		double GetEncoderSpeed(int encoder, int modo);

		/* Retrieves the position of the specified encoder
			@param encoder Encoder number (E1 or E2)
			@param mode Unit of measurement (DEGREES, RADS)
			@return Current position of the encoder
		*/
		double GetEncoderPosition(int encoder, int modo);

};

#endif