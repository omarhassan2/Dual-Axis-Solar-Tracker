/**
 * @brief   Dual-Axis Solar Tracker implemented using the PID controller concept. 
 *          The project aims to efficiently orient solar panels towards the sun 
 *          throughout the day, maximizing energy generation.
 * 
 * @author  DUA AXIS ☀️↗️⚡
 * 
 * @date    Apirl 4, 2024 
*/



/*************** Includes Section ***************/
#include <Arduino.h>
#include <Servo.h>
/************************************************/



/*************** Macro Defentions Section ***************/
/* 4 LDRs Sensors */
#define LDR_TOP_RIGHT           (A0)
#define LDR_TOP_LEFT            (A1)
#define LDR_BOTTOM_RIGHT        (A2)
#define LDR_BOTTOM_LEFT         (A3)

/* 2 Servo Motors */
#define MOTOR_TOP_BOTTOM        (2)
#define MOTOR_RIGHT_LEFT        (3)


/********************************************************/



/*************** Function Decleration Section ***************/

/************************************************************/



/*************** Global Decleration Section ***************/
Servo horizontal; // horizontal servo
int servoh = 180; 
int servohLimitHigh = 175;
int servohLimitLow = 5;
// 65 degrees MAX

Servo vertical; // vertical servo
int servov = 45; 
int servovLimitHigh = 60;
int servovLimitLow = 1;

// LDR pin connections
// name = analogpin;
int ldrlt = A0; //LDR top left - BOTTOM LEFT <--- BDG
int ldrrt = A3; //LDR top rigt - BOTTOM RIGHT
int ldrld = A1; //LDR down left - TOP LEFT
int ldrrd = A3; //ldr down rigt - TOP RIGHT
/**********************************************************/



/*************** Setup Application Section ***************/
void setup() {
horizontal.attach(2);
vertical.attach(3);
horizontal.write(180);
vertical.write(45);
delay(2500); 
}
/*********************************************************/



/*************** Start Application Section ***************/
void loop() {
int lt = analogRead(ldrlt); // top left
int rt = analogRead(ldrrt); // top right
int ld = analogRead(ldrld); // down left
int rd = analogRead(ldrrd); // down right
int dtime = 10; int tol = 90; // dtime=diffirence time, tol=toleransi
int avt = (lt + rt) / 2; // average value top
int avd = (ld + rd) / 2; // average value down
int avl = (lt + ld) / 2; // average value left
int avr = (rt + rd) / 2; // average value right
int dvert = avt - avd; // check the diffirence of up and down
int dhoriz = avl - avr;// check the diffirence og left and rigt

if (-1*tol > dvert || dvert > tol) 
 {
 if (avt > avd)
 {
 servov = ++servov;
 if (servov > servovLimitHigh)
 {servov = servovLimitHigh;}
 }
 else if (avt < avd)
 {servov= --servov;
 if (servov < servovLimitLow)
 { servov = servovLimitLow;}
 }
 vertical.write(servov);
 }
if (-1*tol > dhoriz || dhoriz > tol) // check if the diffirence is in the tolerance else change horizontal angle
 {
 if (avl > avr)
 {
 servoh = --servoh;
 if (servoh < servohLimitLow)
 {
 servoh = servohLimitLow;
 }
 }
 else if (avl < avr)
 {
 servoh = ++servoh;
 if (servoh > servohLimitHigh)
 {
 servoh = servohLimitHigh;
 }
 }
 else if (avl = avr)
 {
 delay(5000);
 }
 horizontal.write(servoh);
 }
 
 delay(dtime);
   
}
/*********************************************************/



/*************** Function Defintions Section ***************/

/***********************************************************/