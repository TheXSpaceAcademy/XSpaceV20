/*
  XSpace V20 Library

  Version   :  1.0.2
  Autor     :  Pablo Cardenas
  Fecha     :  09/04/2024

  This is an open source library but dont remeber to reference!

*/

#include <XSpaceV20.h>

volatile double TimerValue = 0;
volatile double Periodo = 1000000;
volatile double Tant = 0;
volatile double counter = 0;
volatile double Xval;

WiFiClient XSpaceV2WifiClient;
PubSubClient XSpaceV2MQTT(XSpaceV2WifiClient);
void Mqtt_Callback(char* topic, byte* payload, unsigned int length);

void IRAM_ATTR ISR_encoder(){
    TimerValue = micros();
    Xval = (-1+digitalRead(encoder_CHB)*2);
    Periodo = (TimerValue - Tant)*Xval;
    Tant = TimerValue;
    counter = counter+Xval;
}

/*****************************XSpaceV2Board******************************************/

void XSpaceV20Board::init(int frequency, double resolution, double vm){
    pinMode(nSLEEP,OUTPUT);

    ledcSetup(1, frequency, 10);
    ledcSetup(2, frequency, 10);

    ledcAttachPin(IN1, 1);
    ledcAttachPin(IN2, 2);

    pinMode(encoder_CHA,INPUT_PULLUP);
    pinMode(encoder_CHB,INPUT_PULLUP);
    attachInterrupt(encoder_CHA, ISR_encoder, HIGH);

    this->_resolution = resolution;
    this->_vm = vm;
}

void XSpaceV20Board::DRV8837_Sleep(){
    digitalWrite(nSLEEP,LOW);
}

void XSpaceV20Board::DRV8837_Wake(){
    digitalWrite(nSLEEP,HIGH);
}

void XSpaceV20Board::DRV8837_Voltage(double vp){

    if(vp>_vm) vp = _vm;
    if(vp<-_vm) vp = -_vm;
    uint32_t Duty = (uint32_t) ( (1 - abs(vp)/_vm) * 1024.0);

    if(vp<0){
        ledcWrite(1, Duty);
        ledcWrite(2, 1024);
    }else{
        ledcWrite(1, 1024);
        ledcWrite(2, Duty);
    }
}

double XSpaceV20Board::GetEncoderSpeed(int modo){
    double vel=0;

    vel = 360000000.0/(_resolution*Periodo);
    if(abs(vel)>600)vel=_vel_ant;
    if(abs(vel-_vel_ant)>600)vel=_vel_ant;
    _vel_ant = vel;

    switch (modo)
    {
    case DEGREES_PER_SECOND:
        return vel;
        break;
    case RADS_PER_SECOND:
        return vel*PI/180;
        break;
    
    default:
        break;
    }

    return vel;
}
double XSpaceV20Board::GetEncoderPosition(int modo){
    double pos=0;

    switch (modo){
        
    case DEGREES:
        pos = counter/this->_resolution*360.0;
        break;
    case RADS:
        pos = counter/this->_resolution*2*PI;
        break;
    
    default:
        break;
    }

    return pos;
}

/* Metodos reescritos para un mejor entendimiento */

void XSpaceV20Board::Wifi_init(const char* ssid, const char* password){
	
    if(this->_xspace_info){
        Serial.println();
	    Serial.print("Conectando a ssid: ");
	    Serial.println(ssid);
    }

	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		if(this->_xspace_info) Serial.print(".");
	}

    if(this->_xspace_info){
	    Serial.println("");
	    Serial.println("Conectado!!");
	    Serial.println("Dirección IP: ");
	    Serial.println(WiFi.localIP());
    }
    WiFi.setSleep(false);

}

void XSpaceV20Board::Mqtt_Connect(const char *clientId, const char *mqtt_user, const char *mqtt_pass){

	while (!XSpaceV2MQTT.connected()) {
		
		
		// Intentamos conectar
		if (XSpaceV2MQTT.connect(clientId,mqtt_user,mqtt_pass)) {
				if(this->_xspace_info) Serial.println("Conectado al Broker!");
                
		}else {
			if(this->_xspace_info){
                Serial.print("falló :( con error -> ");
			    Serial.print(XSpaceV2MQTT.state());
			    Serial.println(" Intentamos de nuevo en 5 segundos");
			    delay(5000);
            }
		}
	}

}

void XSpaceV20Board::Mqtt_init(const char *mqtt_server, uint16_t mqtt_port){
    XSpaceV2MQTT.setServer(mqtt_server, mqtt_port);
    XSpaceV2MQTT.setCallback([this] (char* topic, byte* payload, unsigned int length) { Mqtt_Callback(topic, payload, length); });
}

bool XSpaceV20Board::Mqtt_IsConnected(){
    return XSpaceV2MQTT.connected();
}

void XSpaceV20Board::Mqtt_Publish(const char* topic, const char* payload){
    XSpaceV2MQTT.publish(topic,payload);
}

void XSpaceV20Board::Mqtt_Suscribe(const char* topic){
    if(this->Mqtt_IsConnected()){
         XSpaceV2MQTT.subscribe(topic);
         if(this->_xspace_info){
             Serial.print("Suscrito a: ");
             Serial.println(topic);
         }else{
             Serial.println("Subscripcion fallida, no se tiene conexion con el broker");
         }
    }
   

}

void XSpaceV20Board::Mqtt_KeepAlive(){
    XSpaceV2MQTT.loop();
}

void XSpaceV20Board::SerialInfo(bool mode){
    this->_xspace_info = mode;
}