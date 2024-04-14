/*
	XSpace V20 Library
	Autor     :  Pablo Cardenas
	This is an open source library but dont remeber to reference!
*/

#ifndef XSPACEV20_H
#define XSPACEV20_H

#include <Arduino.h>
#include <stdint.h>
#include <WiFi.h>
#include <PubSubClient.h>

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
	int PWM_CH_IN1;
	int PWM_CH_IN2;
	int nSLEEP;
	double Vm;
};

class XSpaceV20Board{
		private:
				double _vel_ant = 0;
				bool   _xspace_info = false;

				XSDRV8837 DRV8837;

		public:
				/*init method
				frequency  => PWM frequency for drv8837 driver (Hz)
				resolution => encoder pulses/rev * gearmotor reduction 
				VM => DRV8837 Driver Power Supply voltage (Min=0v Max=11v)*/
				void init(int frequency, double resolution, double VM);

				void encoder1_init(int enc_CHAx, int enc_CHBx, int resolutionx, int mode);
				void encoder2_init(int enc_CHAx, int enc_CHBx, int resolutionx, int mode);

				void DRV8837_init(int IN1x, int CH_IN1x, int IN2x, int CH_IN2x, int nSLEEPx,int frequencyx, int Vmx);
				void DRV8837_Sleep();
				void DRV8837_Wake();
				void DRV8837_Voltage(double vp);
				
				double GetEncoderSpeed(int encoder, int modo);
				double GetEncoderPosition(int encoder, int modo);

				void SerialInfo(bool mode);
				void Wifi_init(const char* ssid, const char* password);
				void Mqtt_init(const char* mqtt_server, uint16_t mqtt_port);
				void Mqtt_Connect(const char *clientId, const char *mqtt_user, const char *mqtt_pass);
				void Mqtt_Publish(const char* topic, const char* payload);
				void Mqtt_Suscribe(const char* topic);
				void Mqtt_KeepAlive();
				
				bool Mqtt_IsConnected();
};



#endif