/*
  XSpace V20 Library

  Version   :  1.0.3
  Autor     :  Pablo Cardenas
  Fecha     :  10/04/2024

  This is an open source library but dont remeber to reference!

*/

#include <XSpaceV20.h>

volatile double TimerValue[2] = {0, 0};
volatile double Periodo[2] = {1000000, 1000000};
volatile double Tant[2] = {0, 0};
volatile double counter[2] = {0, 0};
volatile double Xval[2];
XSEncoder encoder[2];

WiFiClient XSpaceV2WifiClient;
PubSubClient XSpaceV2MQTT(XSpaceV2WifiClient);
void Mqtt_Callback(char* topic, byte* payload, unsigned int length);

void IRAM_ATTR ISR_encoder1(){
    int idx = 0; // Índice para el primer encoder
    TimerValue[idx] = micros();
    Xval[idx] = (-1+digitalRead(encoder[idx].channelB)*2); // Asumiendo que encoder_CHB es global o estática
    Periodo[idx] = (TimerValue[idx] - Tant[idx])*Xval[idx];
    Tant[idx] = TimerValue[idx];
    counter[idx] = counter[idx]+Xval[idx];
}

void IRAM_ATTR ISR_encoder2(){
    int idx = 1; // Índice para el primer encoder
    TimerValue[idx] = micros();
    Xval[idx] = (-1+digitalRead(encoder[idx].channelB)*2); // Asumiendo que encoder_CHB es global o estática
    Periodo[idx] = (TimerValue[idx] - Tant[idx])*Xval[idx];
    Tant[idx] = TimerValue[idx];
    counter[idx] = counter[idx]+Xval[idx];
}


/*****************************XSpaceV20Board******************************************/

void XSpaceV20Board::init(int frequency, double resolution, double Vm){
    this->DRV8837_init(DRV8837_IN1, 1, DRV8837_IN2, 2, DRV8837_nSLEEP, 20000, Vm);
    this->encoder1_init(encoder_CHA, encoder_CHB, resolution, RISING);
    this->_encoder_idx = 0;
}

void XSpaceV20Board::encoder1_init(int enc_CHAx, int enc_CHBx, int resolutionx, int mode){
    
    encoder[0].channelA = enc_CHAx;
    encoder[0].channelB = enc_CHBx;
    encoder[0].resolution = resolutionx;

    pinMode(encoder[0].channelA, INPUT_PULLUP);
    pinMode(encoder[0].channelB, INPUT_PULLUP);
    attachInterrupt(encoder[0].channelA, ISR_encoder1, mode);
}
void XSpaceV20Board::encoder2_init(int enc_CHAx, int enc_CHBx, int resolutionx, int mode){
    encoder[1].channelA = enc_CHAx;
    encoder[1].channelB = enc_CHBx;
    encoder[1].resolution = resolutionx;

    pinMode(encoder[1].channelA, INPUT_PULLUP);
    pinMode(encoder[1].channelB, INPUT_PULLUP);
    attachInterrupt(encoder[1].channelA, ISR_encoder2, mode);
}

void XSpaceV20Board::DRV8837_init(int IN1x, int CH_IN1x, int IN2x, int CH_IN2x, int nSLEEPx,int frequencyx, int Vmx){
    DRV8837.IN1 = IN1x;
    DRV8837.IN2 = IN2x;
    DRV8837.nSLEEP = nSLEEPx;
    DRV8837.PWM_CH_IN1 = CH_IN1x;
    DRV8837.PWM_CH_IN2 = CH_IN2x;
    DRV8837.Vm = Vmx;

    pinMode(DRV8837.nSLEEP,OUTPUT);
    this->DRV8837_Sleep();

    ledcSetup(DRV8837.PWM_CH_IN1, frequencyx, 10);
    ledcSetup(DRV8837.PWM_CH_IN2, frequencyx, 10);

    ledcAttachPin(DRV8837.IN1, DRV8837.PWM_CH_IN1);
    ledcAttachPin(DRV8837.IN2, DRV8837.PWM_CH_IN2);

}

void XSpaceV20Board::DRV8837_Sleep(){
    digitalWrite(DRV8837.nSLEEP,LOW);
}

void XSpaceV20Board::DRV8837_Wake(){
    digitalWrite(DRV8837.nSLEEP,HIGH);
}

void XSpaceV20Board::DRV8837_Voltage(double Vp){

    if(Vp>DRV8837.Vm) Vp = DRV8837.Vm;
    if(Vp<-DRV8837.Vm) Vp = -DRV8837.Vm;
    uint32_t Duty = (uint32_t) ( (1 - abs(Vp)/DRV8837.Vm) * 1023.0);

    if(Vp<0){
        ledcWrite(DRV8837.PWM_CH_IN1, Duty);
        ledcWrite(DRV8837.PWM_CH_IN2, 1023);
    }else{
        ledcWrite(DRV8837.PWM_CH_IN1, 1023);
        ledcWrite(DRV8837.PWM_CH_IN2, Duty);
    }
}

double XSpaceV20Board::GetEncoderSpeed(int modo){
    double vel=0;

    vel = 360000000.0/(encoder[_encoder_idx].resolution*Periodo[_encoder_idx]);
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
        pos = counter[_encoder_idx]/encoder[_encoder_idx].resolution*360.0;
        break;
    case RADS:
        pos = counter[_encoder_idx]/encoder[_encoder_idx].resolution*2*PI;
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