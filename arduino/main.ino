#include "general.h"
/****************************************************************************/
/* Global variables                                                         */
/****************************************************************************/
int sensorValue;                 // value read  from  the pot
long int outputValue;                 // value output  to  the PWM (analog out)

int counter_small;  //counter for measurements 

float LUX_mat[max_Output/n_increments+1][2];

long int microseconds=10000;     //tempo interrupção

float ref_PWM=30;

bool begin_filters;

//interruption should not have alot of stuff inside
void tempo() 
{
	counter_small=1;
}



/****************************************************************************/
/* Setup routine - runs once when reset is pressed                          */
/****************************************************************************/
void setup() 
{
  	// initialize serial communication at 9600 bits per second:
	Serial.begin(38400);
    // initialize interrupt
	Timer1.initialize(microseconds) ;
  	Timer1.attachInterrupt(tempo); //insert the function the interrupt goes to
  	//Serial.println(outputValue); 
  	//Serial.print("---\n"); 

  	counter_small=0; 
  
  	sensorValue = 0; 
  	outputValue = 0; 


  	Calibration();
  	Serial.println("Exited setup");
  	begin_filters=true;


}

void loop() 
{   
	Controller(ref_PWM);
}
