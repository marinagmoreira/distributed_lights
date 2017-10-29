#include "general.h"

/*FILTER BUTTERWORTH DMP */
static const float B1f=0.6389;
static const float B2f=1.2779;
static const float B3f=0.6389;
static const float A2f=1.1430;
static const float A3f=0.4128;


#define Kp 2.5
#define Ki 100
#define Kd 0
#define a 1
#define b 1
#define T 0.01
#define deadzone 5
#define tau1 1


//For filter INPUT
float    sensorValue_filt;
float    y1_s;
float    y2_s;
float    x1_s;
float    x2_s;


float K1 = Kp*b;
float K2 = Kp*Ki*T/2;
float K3 = Kd/(Kd+a*T);
float K4 = Kp*Kd*a/(Kd+a*T);
float ff;

float m;
float c;
float y_ant = 0; 
float i_ant = 0; 
float d_ant = 0; 
float e_ant = 0;


//References
float ref_smooth=0;
float ref_PID=0;


int cool_counter;



void SmoothRef(float ref_LUX)
{
	ref_smooth=ref_smooth+T*(ref_LUX-ref_smooth)/tau1;	
}

//System Simulator
void SysPrime(float tau)
{
	ref_PID=ref_PID+T*(ref_smooth-ref_PID)/tau;
}


//Low pass filter at 50Hz
void filter_butter(float *y, float *y1, float *y2, float x, float *x1, float *x2)
{
	*y= (B1f*x + B2f*(*x1)  + B3f*(*x2))  - (A2f*(*y1) + A3f*(*y2));
	*y2=*y1;
	*y1=*y;
	*x2=*x1;
	*x1=x;
}


//Saturator
bool Saturator()
{

	if (outputValue>255) 
	{
		outputValue=255;
		return true;
	}
	if (outputValue<0)
	{
		outputValue=0;
		return true; 
	}

	return false;	
}

// Deadzone
void Deadzone1()
{
	if(outputValue<deadzone)
		outputValue=0;
}

// Deadzone
void Deadzone2()
{
	if(outputValue<deadzone)
		outputValue=0;
}

void PID(float ref_PID)
{
	double e;
	double p;
	double i;
	double d;
	double y;

	e = ref_PID- sensorValue_filt;
	p = K1*ref_PID -Kp*y;
	i = i_ant + K2*(e + e_ant);
	d = K3*d_ant - K4*(y - y_ant);

	outputValue = (long int) (p+i+d+ff);

	if (Saturator())
		i=i_ant;

	Deadzone1();

	//Write to the LED
	analogWrite(analogOutPin, outputValue); 

	i_ant = i;
	y_ant = y;

	d_ant = d;
	e_ant = e;
}


void UpdateUser(float tau)
{
	//For Status
	cool_counter++;
	if (cool_counter==100)
	{
		Serial.print("ff:");
		Serial.print(ff);		

		Serial.print(" ref_smooth:");
		Serial.print(ref_smooth);

		Serial.print(" ref_PID:");
		Serial.print(ref_PID);

		Serial.print(" tau:");
		Serial.print(tau);


		Serial.print(" LUX:");
		Serial.print(sensorValue_filt);

		Serial.print(" output:");
		Serial.println(outputValue);

    /*Serial.println("i:");
    Serial.println(sensorValue_filt);

    Serial.println("i:");
    Serial.println(sensorValue); */ 

		cool_counter=0;
	}

}

void Controller (float ref_LUX, float tau)
{
	int refOutput;


	if (counter_small==1)
	{

		if (begin_filters==true)
		{
			sensorValue_filt=0;
			y1_s=0;
			y2_s=0;
			x1_s=0;
			x2_s=0;
			begin_filters=false;
		}


		//Reference Smoother changes ref_smooth
		SmoothRef(ref_LUX);  

		//Calculates ff gain LUX2PWM
		ff = ref_smooth*Kff;
		//ff=ref_LUX*Kff;

		//Simulates real system, receives ff and changes ref_PID
		SysPrime(tau);
		//ref_PID=30;

  		//Read the sensor and convert it to LUX
  		sensorValue = analogRead(analogInPin); //  read  the analog  in  value 
  		sensorValue=PWM2LUX(sensorValue);

  		//Filter the sensor value
  		filter_butter(&sensorValue_filt, &y1_s, &y2_s, sensorValue, &x1_s, &x2_s);

  		PID(ref_PID);

  		//Only does this with 1Hz frequency
  		UpdateUser(tau);

  		counter_small=0;

}
}





