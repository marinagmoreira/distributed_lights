#include "general.h"

/****************************************************************************/
/* Global variables                                                         */
/****************************************************************************/

extern int sensorValue;                       // value read  from  the LDR
extern long int outputValue;                  // value output  to  the PWM (analog out)

extern int counter_small;                     // flag to detect interruptions
extern float LUX_mat[max_Output/n_increments+1][2];  //Matrix containing the LUX value when applying a given PWM is applied to the LED


// Function PWM2LUX- conversion PWM to LUX units	

float PWM2LUX(float PWM)
{
	return pow(10,(log10(((Vcc*max_PWM/( PWM *max_Volt))-1)*R1)-b)/a);
}


//The calibration function computes the values to insert in the LUX-mat matrix. The PWM output of the LED is incremented of n_increments and the steady state response
//is collected. The response is then converted to LUX and saved in the matrix next to the PWM value that generated it.

void Calibration ()
{
	int not_calibrated=1;
	long int sum=0;
	float average;
	int counter_large=0;         //counter for output change

  while(not_calibrated){

    if (counter_small==1){
      sensorValue = analogRead(analogInPin); //  read  the analog  in  value 

			//write the readings value
			//Serial.print(sensorValue); 
			//Serial.print("\n");      
			counter_small=0;
			counter_large++;

			//inclements if we are getting to the end of the samples
			if (counter_large>(intervals-n_average))
				sum=sum+sensorValue;	
	  }

	//Last count, be carefull with condition if n_increments is changed
  	if (counter_large==intervals && outputValue<=250){	

			//saves the output
			LUX_mat[outputValue/n_increments][0]=outputValue;
			//computes average & converts to LUX
			LUX_mat[outputValue/n_increments][1]=PWM2LUX((float)sum/n_average);

			Serial.println("output:");
			Serial.println(LUX_mat[outputValue/n_increments][0]);
			Serial.println(outputValue);

			Serial.println("input PWM:");
			Serial.println((float)sum/n_average);

			Serial.println("LUX:");
			Serial.println(LUX_mat[outputValue/n_increments][1]);
			Serial.println("\n");
			sum=0;
		}

		if (counter_large==intervals && outputValue<250)
		{
			//increments output voltage
			
			if(outputValue+n_increments<=255)
			outputValue=outputValue+n_increments;

			//writes new voltage to LED
			analogWrite(analogOutPin,  outputValue); 

			//write the new output value 
			Serial.println(outputValue); 
			Serial.print("---\n"); 

			//reset counters
			counter_large=0;
		}
		else if (counter_large==intervals && outputValue>=250)
			not_calibrated=0;

  }

  Serial.println("Exited function");
}

