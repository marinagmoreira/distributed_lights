#include "general.h"
#include <stdint.h>

float Kff=0;
float sum_Kff=0;


// Function PWM2LUX- conversion PWM to lux units	
float PWM2LUX(float PWM)
{
	return pow(10,(log10(((Vcc*max_PWM/( PWM *max_Volt))-1)*R1)-b)/a);
}


//Function Calibration 
void Calibration ()
{
	int not_calibrated=1;
	long int sum=0;
	float average;
	int counter_large=0;         //counter for output change
	unsigned long t0 = 0;
	unsigned long cur_time = 0;
	int is_first_sample = 1;
	int mat_counter = 0;
	float first_stair=0;
	float K;
	
	uint16_t response_values[intervals][1];



	while(not_calibrated)
	{

		//if interruption hits
		if (counter_small==1)
		{
      		sensorValue = analogRead(analogInPin); //  read  the analog  in  value 
      		response_values[mat_counter][0] = sensorValue;

	  		//inclements if we are getting to the end of the samples
      		if (counter_large >= (intervals-n_average))
      			sum=sum+sensorValue;

      		mat_counter++;
      		counter_small=0;
      		counter_large++;
      	}


		//Last count, be carefull with condition if n_increments is changed
      	if (counter_large==intervals && outputValue<=250)
      	{	


			//saves the output
      		LUX_mat[outputValue/n_increments][0]=outputValue;

			//computes average & converts to LUX
      		K = PWM2LUX((float)sum/n_average);

      		//Avoids division by zero
      		if(outputValue>=100)
      			sum_Kff+=(float)outputValue / K;


      		// Serial.print("mm");
      		//Compute TAU
      		for (int i = 0; i < intervals; i++)
      		{   	
      			if ( PWM2LUX(response_values[i][0]) >= (0.632*K + (1-0.632)*first_stair))
      			{
      				LUX_mat[outputValue/n_increments][1] = float(i)*0.001;
      				break;
      			}
      		}

      		//Saves last value for next iteration's first value
      		first_stair=K;

      		//Print Stuff
      		Serial.print("Kff:");
      		Serial.print(float(outputValue)/K,4);

      		Serial.print(" TAU:");
      		Serial.println(LUX_mat[outputValue/n_increments][1],4);
      		
			//increments output voltage only
      		outputValue=outputValue+n_increments;
      		if (outputValue<=250)
      		analogWrite(analogOutPin,  outputValue); //writes new voltage to LED


			//reset counters
      		counter_large=0;
      		mat_counter = 0;
      		sum=0;
      	}
      	//Reached the end of the calibration
      	if (counter_large==intervals && outputValue>250)
      	{

      		not_calibrated=0;

      		//Calculate Kff
      		Kff=sum_Kff/16;
      		
      		Serial.println("Kff:");
      		Serial.println(Kff);
      	}
      }

      Serial.println("Exited function");
  }

