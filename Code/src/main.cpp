/**
 * @brief   Dual-Axis Solar Tracker implemented using the PID controller concept. 
 *          The project aims to efficiently orient solar panels towards the sun 
 *          throughout the day, maximizing energy generation.
 * 
 * @author  DUA AXIS ☀️↗️⚡
 * @date    Apirl 4, 2024 
*/



/*************** Includes Section ***************/
#include <Arduino.h>
#include <ESP32Servo.h>
/************************************************/



/*************** Macro Defentions Section ***************/
/* Time Period of each Cycle in milliseconds, 
the more lower the more high detection */
#define TIME_PERIOD             (10)


/* If the angle = 90(Tolerance) this mean the light is perpendicular to the cell,
if not we have to change the direction to to perpendicular again*/
#define TOLERANCE               (90)


/* System constraints  */
#define MAX_HORIZONTAL_ANGLE    (175)
#define MIN_HORIZONTAL_ANGLE    (5)
#define MAX_VERTICAL_ANGLE      (60)
#define MIN_VERTICAL_ANGLE      (1)


/************ Interfacing ************/
/* 4 LDRs Sensors */
#define LDR_TOP_RIGHT           (36)
#define LDR_TOP_LEFT            (39)
#define LDR_DOWN_RIGHT          (34)
#define LDR_DOWN_LEFT           (35)

/* 2 Servo Motors */
#define SERVO_HORIZONTAL        (32)
#define SERVO_VERTICAL          (33)
/********************************************************/



/*************** Function Decleration Section ***************/

/************************************************************/



/*************** Global Decleration Section ***************/
Servo Servo_Horizontal; 
int HorizontalAngle = 180; 


Servo Servo_Vertical;
int VerticalAngle = 45; 
/**********************************************************/



/*************** Setup Application Section ***************/
void setup() {
    Servo_Horizontal.attach(SERVO_HORIZONTAL);
    Servo_Vertical.attach(SERVO_VERTICAL);

    Servo_Horizontal.write(180);
    Servo_Vertical.write(45);
    delay(2500); 
}
/*********************************************************/



/*************** Start Application Section ***************/
void loop() {
    int topRight    = analogRead(LDR_TOP_RIGHT);
    int topLeft     = analogRead(LDR_TOP_LEFT); 
    int downRight   = analogRead(LDR_DOWN_RIGHT); 
    int downLeft    = analogRead(LDR_DOWN_LEFT);


    /* Get the average value for all directions */
    int averageTop      = (topLeft + topRight)   / 2; 
    int averageDown     = (downLeft + downRight) / 2; 
    int averageLeft     = (topLeft + downLeft)   / 2; 
    int averageRight    = (topRight + downRight) / 2; 


    int diffirenceVertical   = averageTop  - averageDown; 
    int diffirenceHorizontal = averageLeft - averageRight;


    // check if the diffirence is in the tolerance else change Vertical angle
    if ((-1*TOLERANCE > diffirenceVertical) || (diffirenceVertical > TOLERANCE)) 
    {
        /* If the light at top part */
        if (averageTop >= averageDown)
        {   /* Move Vertical Servo one degree(upward) until reach it's limit
            if it's angle more than limit stop at limit */
            
            VerticalAngle = ++VerticalAngle;
            if (VerticalAngle > MAX_VERTICAL_ANGLE)
            {
                VerticalAngle = MAX_VERTICAL_ANGLE;
            }
        }

        /* If the light at down part */
        else if (averageTop < averageDown)
        {   /* Move Vertical Servo one degree(downward) until reach it's limit
            if it's angle more than limit stop at limit */

            VerticalAngle = --VerticalAngle;
            if (VerticalAngle < MIN_VERTICAL_ANGLE)
            { 
                VerticalAngle = MIN_VERTICAL_ANGLE;
            }
        }

        Servo_Vertical.write(VerticalAngle);
    }

    // check if the diffirence is in the tolerance else change horizontal angle
    if ((-1*TOLERANCE > diffirenceHorizontal) || (diffirenceHorizontal > TOLERANCE)) 
    {
        /* If the light at right part */
        if (averageLeft >= averageRight)
        {   /* Move Horizontal Servo one degree(rightward) until reach it's limit
            if it's angle more than limit stop at limit */

            HorizontalAngle = --HorizontalAngle;
            if (HorizontalAngle < MIN_HORIZONTAL_ANGLE)
            {
                HorizontalAngle = MIN_HORIZONTAL_ANGLE;
            }
        }

        /* If the light at left part */
        else if (averageLeft < averageRight)
        {   /* Move Horizontal Servo one degree(leftward) until reach it's limit
            if it's angle more than limit stop at limit */
            
            HorizontalAngle = ++HorizontalAngle;
            if (HorizontalAngle > MAX_HORIZONTAL_ANGLE)
            {
                HorizontalAngle = MAX_HORIZONTAL_ANGLE;
            }
        }

        Servo_Horizontal.write(HorizontalAngle);
    }

    delay(TIME_PERIOD);
}
/*********************************************************/



/*************** Function Defintions Section ***************/

/***********************************************************/