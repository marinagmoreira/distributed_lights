#include "general.h"

/****************************************************************************/
/* Global variables                                                         */
/****************************************************************************/

//General
int sensorValue;                      // value read  from  the pot
long int outputValue;                 // value output  to  the PWM (analog out)

int counter_small;                    //counter for measurements 


float LUX_mat[max_Output/n_increments+1][2];
float tau=1;

long int microseconds=1000;          //interruption time for calibration
bool begin_filters;               

float ref_LUX=30;                     // Reference 

char rx_byte = 0;
String rx_str = "";
boolean not_number = false;


//Control Variables- CONTROL IN READSERIAL MONITOR
bool FF_ON=true;		//If FeedForward is ON
bool Filter_ON=true;	//If Filter is ON
bool PID_ON=true;		//If PID is ON
bool AW_ON=true;		//If Anti-Windup ON
bool Deadzone_ON=true;	//If Deadzone ON



/****************************************************************************/
/* Time Interruption                                                        */
/****************************************************************************/
void tempo() 
{
	counter_small=1;
}

/****************************************************************************/
/* Read from Serial Monitor                                                 */
/****************************************************************************/
bool ReadSerialMonitor()
{
//To read imputs from the serial monitor!
	if (Serial.available() > 0) 
  	{    // is a character available?
    	rx_byte = Serial.read();       // get the character

    	if ((rx_byte >= '0') && (rx_byte <= '9')) 
    	{
    		rx_str += rx_byte;
    	}
    	// end of string
    	else if (rx_byte == '\n') 
    	{
      		
    		if (not_number) 
    		{
    			Serial.println("Not a number");
    		}
    		else 
    		{
    			Serial.println("ref_PWM:");
    			ref_LUX = rx_str.toInt();
    			Serial.println(ref_LUX);
    			return true;
    		}
      		not_number = false;         // reset flag
      		rx_str = "";                // clear the string for reuse
      	}
      	else 
      	{
      		// non-number character received
      		not_number = true;    // flag a non-number
      	}
    }

    return false; 
}


/****************************************************************************/
/* Computes Tau for specific reference                                      */
/****************************************************************************/
float ComputeTau(float ref_LUX)
{
	for (int i = 0; i < 25; i++)
  	{
  		if(LUX_mat[i][0]>ref_LUX*Kff)
  		{
  			tau=LUX_mat[i][1];
  			Serial.print("Tau chosen:");
  			Serial.println(tau,4);
  			return tau;
  		}

  	}
  	return -1;
}



/****************************************************************************/
/* Setup routine - runs once when reset is pressed                          */
/****************************************************************************/
  void setup() 
  {
  // initialize serial communication at 9600 bits per second:
  	Serial.begin(115200); //REVIEW THIS

  // initialize interrupt
  	Timer1.initialize(microseconds) ;
  	Timer1.attachInterrupt(tempo); 

	//Serial.println(outputValue); 
	//Serial.print("---\n"); 

  	counter_small=0; 

  	sensorValue = 0; 
  	outputValue = 0; 

  	Calibration();
  	Serial.println("Exited setup");

  	//Change interuption time for control
  	microseconds=10000;
   // initialize interrupt
  	Timer1.initialize(microseconds) ;
  	Timer1.attachInterrupt(tempo); 	

  	//Begin filters for control
  	begin_filters=true;

  	tau=ComputeTau(ref_LUX);

  }

/****************************************************************************/
/* Loop routine                          */
/****************************************************************************/
  void loop() 
  {   


  	if (ReadSerialMonitor())
  		tau=ComputeTau(ref_LUX);


  	Controller(ref_LUX, tau);
  }
