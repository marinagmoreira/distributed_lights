#ifndef General_h
#define General_h

#include <Arduino.h>
#include <TimerOne.h>
#include <math.h>


//From Calibration-----------------------------------

//LUX characteristic
#define a -0.6901
#define b 4.7782
#define Vcc 5.0
#define R1 10000
#define max_PWM 1024
#define max_Volt 5
#define max_Output 255

#define n_average 5
#define n_increments 10

//numero de interrupções até mudança de output
#define intervals 10

/****************************************************************************/
/* Global constant variables                                                         */
/****************************************************************************/


const int analogInPin =  A0;   //  Analog  input pin that  the potentiometer is  attached  to
const int analogOutPin =  5;  //  Analog  output  pin that  the LED is  attached  to

//From Calibration-----------------------------------

float PWM2LUX(float PWM);
void Calibration ();
void tempo();
void filter_butter(float *y, float *y1, float *y2, float x, float *x1, float *x2);
void Controller (float ref_PWM);

#endif 
