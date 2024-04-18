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
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h" //Provide the token generation process info.
#include "addons/RTDBHelper.h"  //Provide the RTDB payload printing info and other helper functions.
/************************************************/


/*************** Macro Defentions Section ***************/

/************ System constraints  ************/
#define MAX_HORIZONTAL_ANGLE    (175)
#define MIN_HORIZONTAL_ANGLE    (5)
#define MAX_VERTICAL_ANGLE      (60)
#define MIN_VERTICAL_ANGLE      (1)

/* Time Period of each Cycle in milliseconds, 
the lower time the more responding */
#define TIME_PERIOD             (10)

/* If the angle = 90(Tolerance) this mean the light is perpendicular to the cell,
if not we have to change the direction to to perpendicular again*/
#define TOLERANCE               (90)

/************ Network & Firebase Section ************/
// Insert your network credentials(ssid : Name)
#define WIFI_SSID       "asus"
#define WIFI_PASSWORD   "Abo Bakr 2468"

// Insert Firebase project API Key
#define API_KEY       "AIzaSyBuqRJ65YOWkPKruUfNekaekYqsH174HNw"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL  "https://learn-iot-esp32-default-rtdb.firebaseio.com/" 

/************ Interfacing Section ************/
/* 4 LDRs Sensors */
#define LDR_TOP_RIGHT           (4)
#define LDR_TOP_LEFT            (0)
#define LDR_DOWN_RIGHT          (2)
#define LDR_DOWN_LEFT           (15)

/* 2 Servo Motors */
#define SERVO_HORIZONTAL        (32)
#define SERVO_VERTICAL          (33)

/* Indication LEDS */
#define MANUAL_MODE_LED         (23)
#define AUTOMATIC_MODE_LED      (22)
#define STOP_MODE_LED           (1)
/********************************************************/



/*************** Function Decleration Section ***************/
void manualMode(void); // using IOT : Flutter & Firebase
void automaticMode(void); // using LDRs
void stopMode(void);
/************************************************************/



/*************** Global Decleration Section ***************/
// Define Motors and their angle
Servo Servo_Horizontal; 
Servo Servo_Vertical;
int HorizontalAngle = 180; 
int VerticalAngle = 45; 

//Define Firebase Data object
FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;
bool signupOK = false;
/**********************************************************/



/*************** Setup Application Section ***************/
void setup() {
    /********** Setup Serial **********/
    Serial.begin(115200);

    /********** Indication LEDS **********/
    pinMode(MANUAL_MODE_LED, OUTPUT);
    pinMode(AUTOMATIC_MODE_LED, OUTPUT);
    pinMode(STOP_MODE_LED, OUTPUT);

    /********** Setup Servo Motors with initial angle **********/
    Servo_Horizontal.attach(SERVO_HORIZONTAL);
    Servo_Vertical.attach(SERVO_VERTICAL);
    Servo_Horizontal.write(180);
    Servo_Vertical.write(45);

    /********** Connecting to Wi-Fi **********/
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    /********** Connecting to Firebase **********/
    //Assign the api key (required)
    config.api_key = API_KEY;
    // Assign the RTDB URL (required)
    config.database_url = DATABASE_URL;

    // Signup for an anonymous user 
    if (Firebase.signUp(&config, &auth, "", "")){
        Serial.println("Signup To Firebase ");
        signupOK = true;
    }
    else{
        Serial.printf("%s\n", config.signer.signupError.message.c_str());
    }

    // Assign the callback function for the long running token generation task 
    config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
    
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
}
/*********************************************************/



/*************** Start Application Section ***************/
void loop() {
    if ((Firebase.ready()) && (signupOK)) 
    {
        if (Firebase.RTDB.getInt(&firebaseData, "ESP/Mode")) 
        {
            if (firebaseData.intData() == 1) 
            {
                manualMode();
            }
            else if (firebaseData.intData() == 2) 
            {
                automaticMode();
            }
            else 
            {
                stopMode();
            }
        }
    }
    delay(TIME_PERIOD);
}
/*********************************************************/



/*************** Function Defintions Section ***************/
void manualMode(void){
    /* Turn On Indication LED and Turn off others*/
    digitalWrite(MANUAL_MODE_LED, HIGH);
    digitalWrite(AUTOMATIC_MODE_LED, LOW);
    digitalWrite(STOP_MODE_LED, LOW);

    if (Firebase.RTDB.getInt(&firebaseData, "ESP/HorizontalAngle")) {
        HorizontalAngle = firebaseData.intData();
        Serial.print("HorizontalAngle = ");
        Serial.println(HorizontalAngle);
        Servo_Horizontal.write(HorizontalAngle);
    }
    else {
        Serial.println(firebaseData.errorReason());
    }

    delay(100);

    if (Firebase.RTDB.getInt(&firebaseData, "ESP/VerticalAngle")) {
        VerticalAngle = firebaseData.intData();
        Serial.print("VerticalAngle = ");
        Serial.println(VerticalAngle);
        Servo_Vertical.write(VerticalAngle);
    }
    else {
        Serial.println(firebaseData.errorReason());
    }
}


void automaticMode(void){
    /* Turn On Indication LED and Turn off others*/
    digitalWrite(MANUAL_MODE_LED, LOW);
    digitalWrite(AUTOMATIC_MODE_LED, HIGH);
    digitalWrite(STOP_MODE_LED, LOW);

    int topRight    = analogRead(LDR_TOP_RIGHT);
    int topLeft     = analogRead(LDR_TOP_LEFT); 
    int downRight   = analogRead(LDR_DOWN_RIGHT); 
    int downLeft    = analogRead(LDR_DOWN_LEFT);

    /* Get the average value for all directions */
    int averageTop      = (topLeft  + topRight)   / 2; 
    int averageDown     = (downLeft + downRight)  / 2; 
    int averageLeft     = (topLeft  + downLeft)   / 2; 
    int averageRight    = (topRight + downRight)  / 2; 

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
}


void stopMode(void){
    /* Set Motors to the initial values */
    Servo_Horizontal.write(180);
    Servo_Vertical.write(45);

    /* Turn On Indication LED and Turn off others*/
    digitalWrite(MANUAL_MODE_LED, LOW);
    digitalWrite(AUTOMATIC_MODE_LED, LOW);
    digitalWrite(STOP_MODE_LED, HIGH);    
}
/***********************************************************/