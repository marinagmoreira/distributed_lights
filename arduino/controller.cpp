#include "general.h"

/*FILTER BUTTERWORTH DMP */
static const float B1f=0.6389;
static const float B2f=1.2779;
static const float B3f=0.6389;
static const float A2f=1.1430;
static const float A3f=0.4128;


#define Kp 2.5
#define Ki 200
#define Kd 0
#define a 1
#define b 1
#define T 0.01

extern int sensorValue;                 // value read  from  the pot
extern long int outputValue;                 // value output  to  the PWM (analog out)
extern bool begin_filters;
extern int counter_small;
extern float LUX_mat[max_Output/n_increments+1][2];

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
float K_wind=0.5*K2;
double ff;


float y_ant = 0; 
float i_ant = 0; 
float d_ant = 0; 
float e_ant = 0;


int cool_counter;

void PID(float ref)
{
double e;
double p;
double i;
double d;
double y;
float windup=0;


sensorValue = analogRead(analogInPin); //  read  the analog  in  value 


sensorValue=PWM2LUX(sensorValue);

filter_butter(&sensorValue_filt, &y1_s, &y2_s, sensorValue, &x1_s, &x2_s);
//FILTER
//ANTYWIND

e = ref- sensorValue_filt;
p = K1*ref -Kp*y;
i = i_ant + K2*(e + e_ant);
d = K3*d_ant - K4*(y - y_ant);

outputValue = (long int) (p+i+d+ff);

//Saturator
if (outputValue>255) 
{
    windup=255-outputValue;
  outputValue=255;


}
if (outputValue<0)
{
  windup=255-outputValue;
  outputValue=0;

  
}

analogWrite(analogOutPin, outputValue); 

i_ant = i+windup;
y_ant = y;

d_ant = d;
e_ant = e;

  cool_counter++;
  if (cool_counter==100)
  {
            Serial.println("LUX:");
        Serial.println(sensorValue_filt);
        
        Serial.println("output:");
        Serial.println(outputValue);

        Serial.println("i:");
        Serial.println(sensorValue_filt);

        Serial.println("i:");
        Serial.println(sensorValue);  

        cool_counter=0;
  }




}


void filter_butter(float *y, float *y1, float *y2, float x, float *x1, float *x2)
{
	*y= (B1f*x + B2f*(*x1)  + B3f*(*x2))  - (A2f*(*y1) + A3f*(*y2));
	*y2=*y1;
	*y1=*y;
	*x2=*x1;
	*x1=x;
}


void Controller (float ref_PWM)
{
int refOutput;
if (counter_small==1)
{


	//FILTER INPUT
	if (begin_filters==true)
	{
		    sensorValue_filt=0;
    		y1_s=0;
    		y2_s=0;
    		x1_s=0;
    		x2_s=0;
		begin_filters=false;
    
LUX_mat[max_Output/n_increments+1][2];


    for (int i=0; i < max_Output/n_increments; i++){
      if (ref_PWM < LUX_mat[i][1]){
        float x1 = LUX_mat[i-1][0];
        float x2 = LUX_mat[i][0];
        float y1 = LUX_mat[i-1][1];
        float y2 = LUX_mat[i][1];

        float m = (y2-y1)/(x2-x1);
        float c = y1-m*x1;
        ff = (ref_PWM - c)/m;
      }
    }
        Serial.println("ffd:");
        Serial.println(refOutput);

	}



	PID(ref_PWM);
	counter_small=0;

  
}


}





