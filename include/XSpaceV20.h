/*
  XSpace V20 Library

  Version   :  1.0.2
  Autor     :  Pablo Cardenas
  Fecha     :  09/04/2024

  This is an open source library but dont remeber to reference!

*/

#ifndef XSPACEV20_H
#define XSPACEV20_H

#include <Arduino.h>
#include <stdint.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define IN1 32
#define IN2 33
#define nSLEEP 25

#define encoder_CHA 34
#define encoder_CHB 35

#define DEGREES_PER_SECOND 1
#define RADS_PER_SECOND 2

#define DEGREES 1
#define RADS 2

class XSpaceV20Board{
    private:
        double _vel_ant = 0;
        bool _xspace_info = false;
        double _resolution;
        double _vm;

    public:
        /*init method
        frequency  => PWM frequency for drv8837 driver (Hz)
        resolution => encoder pulses/rev * gearmotor reduction 
        vm => DRV8837 Driver Power Supply voltage (Min=0v Max=11v)*/
        void init(int frequency, double resolution, double vm);
        void DRV8837_Sleep();
        void DRV8837_Wake();
        void DRV8837_Voltage(double vp);
        
        double GetEncoderSpeed(int modo);
        double GetEncoderPosition(int modo);

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