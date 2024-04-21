/*
  XSpace V20 Library
  Autor     :  Pablo Cardenas
  This is an open source library but dont remeber to reference!
*/

#include <XSpaceV20.h>

volatile double TimerValue[2] = {0, 0};
volatile double Periodo[2] = {1000000, 1000000};
volatile double Tant[2] = {0, 0};
volatile double counter[2] = {0, 0};
volatile double Xval[2];
XSEncoder encoder[2];

void IRAM_ATTR ISR_encoder1(){
	detachInterrupt(encoder[E1].channelA);
	TimerValue[E1] = micros();
	Xval[E1] = (-1+digitalRead(encoder[E1].channelB)*2);
	Periodo[E1] = (TimerValue[E1] - Tant[E1])*Xval[E1];
	Tant[E1] = TimerValue[E1];
	counter[E1] = counter[E1]+Xval[E1];
	attachInterrupt(encoder[E1].channelA, ISR_encoder1, RISING);
}

void IRAM_ATTR ISR_encoder2(){
	detachInterrupt(encoder[E2].channelA);
	TimerValue[E2] = micros();
	Xval[E2] = (-1+digitalRead(encoder[E2].channelB)*2);
	Periodo[E2] = (TimerValue[E2] - Tant[E2])*Xval[E2];
	Tant[E2] = TimerValue[E2];
	counter[E2] = counter[E2]+Xval[E2];
	attachInterrupt(encoder[E2].channelA, ISR_encoder2, RISING);
}


/*****************************XSpaceV20Board******************************************/

void XSpaceV20Board::init(int frequency, double resolution, double Vm){
	this->DRV8837_init(DRV8837_IN1, 1, DRV8837_IN2, 2, DRV8837_nSLEEP, 20000, Vm);
	this->encoder1_init(encoder_CHA, encoder_CHB, resolution, RISING);
}

void XSpaceV20Board::encoder1_init(int enc_CHAx, int enc_CHBx, int resolutionx, int mode){	
	encoder[E1].channelA = enc_CHAx;
	encoder[E1].channelB = enc_CHBx;
	encoder[E1].resolution = resolutionx;

	pinMode(encoder[E1].channelA, INPUT_PULLUP);
	pinMode(encoder[E1].channelB, INPUT_PULLUP);
	attachInterrupt(encoder[E1].channelA, ISR_encoder1, mode);
}
void XSpaceV20Board::encoder2_init(int enc_CHAx, int enc_CHBx, int resolutionx, int mode){
	encoder[E2].channelA = enc_CHAx;
	encoder[E2].channelB = enc_CHBx;
	encoder[E2].resolution = resolutionx;

	pinMode(encoder[E2].channelA, INPUT_PULLUP);
	pinMode(encoder[E2].channelB, INPUT_PULLUP);
	attachInterrupt(encoder[E2].channelA, ISR_encoder2, mode);
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

double XSpaceV20Board::GetEncoderSpeed(int encx, int modo){
	double vel=0;

	vel = 360000000.0/(encoder[encx].resolution*Periodo[encx]);
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
double XSpaceV20Board::GetEncoderPosition(int encx, int modo){
	double pos=0;

	switch (modo){
		
	case DEGREES:
		pos = counter[encx]/(double)encoder[encx].resolution*360.0;
		break;
	case RADS:
		pos = counter[encx]/(double)encoder[encx].resolution*2*PI;
		break;
	
	default:
		break;
	}

	return pos;
}