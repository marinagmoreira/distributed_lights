#include "general.h"

/****************************************************************************/
/* Global variables                                                         */
/****************************************************************************/

int sensorValue;                      // value read  from  the pot
long int outputValue;                 // value output  to  the PWM (analog out)

int counter_small;                    //counter for measurements 

float LUX_mat[max_Output/n_increments+1][2];

long int microseconds=10000;          //interruption time
bool begin_filters;               

float ref_PWM=30;                     // Reference   
char rx_byte = 0;
String rx_str = "";
boolean not_number = false;

/****************************************************************************/
/* Time Interruption                                                        */
/****************************************************************************/
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
  Timer1.attachInterrupt(tempo); 
  
  //Serial.println(outputValue); 
  //Serial.print("---\n"); 

  counter_small=0; 

  sensorValue = 0; 
  outputValue = 0; 

  Calibration();
  Serial.println("Exited setup");
  begin_filters=true;

}

/****************************************************************************/
/* Loop routine - runs once when reset is pressed                          */
/****************************************************************************/
void loop() 
{   
	recvWithEndMarker();
    
	Controller(ref_PWM);
}

void recvWithEndMarker() {
	
  //To read imputs from the serial monitor!
  if (Serial.available() > 0) {    // is a character available?
    rx_byte = Serial.read();       // get the character
    
    if ((rx_byte >= '0') && (rx_byte <= '9')) {
      rx_str += rx_byte;
    }
	
    else if (rx_byte == '\n') {
      // end of string
      if (not_number) {
        Serial.println("Not a number");
      }
      else {
        Serial.println("ref_PWM:");
        Serial.println(ref_PWM);
        ref_PWM = rx_str.toInt();
      }
      not_number = false;         // reset flag
      rx_str = "";                // clear the string for reuse
    }
    else {
      // non-number character received
      not_number = true;    // flag a non-number
    }
  } 
  
}
