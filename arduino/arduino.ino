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



//Control Variables- CONTROL IN READSERIAL MONITOR
bool FF_ON=true;    //If FeedForward is ON
bool Filter_ON=true;  //If Filter is ON
bool PID_ON=true;   //If PID is ON
bool AW_ON=true;    //If Anti-Windup ON
bool Deadzone_ON=true;  //If Deadzone ON



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
int i, start=0, n_input=0;
// Read messages with the following sintax:
// "LUX(L/H) FF(O/1) Filter(O/1) PID(O/1) AW(0/1) Deadzone(0/1)" 

//To read imputs from the serial monitor!
  if (Serial.available() > 0) 
  {    // is a character available?
    String  inputs = Serial.readString();       // get string

    for(i=0;inputs[i]!='\n';i++){
      if(inputs[i]==' '){
        n_input+=1;
        start=i+1;
        
        if(n_input==1){
          String LUX=inputs.substring(start,i-1);
          if (LUX=="H")
            ref_LUX=LUX_mat[max_Output/n_increments][0]*(2/3);
          if (LUX=="L")
            ref_LUX=LUX_mat[max_Output/n_increments][0]*(1/3);            
        }  
        if(n_input==2)
          FF_ON=inputs.substring(start,i-1).toInt();
        if(n_input==3)
          Filter_ON=inputs.substring(start,i-1).toInt();  
        if(n_input==4)
          PID_ON=inputs.substring(start,i-1).toInt();                    
        if(n_input==5)
          AW_ON=inputs.substring(start,i-1).toInt(); 
        if(n_input==6)
          Deadzone_ON=inputs.substring(start,i-1).toInt();           
            
      }
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
