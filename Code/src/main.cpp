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
#include "ArduPID.h" // PID Controller
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h" //Provide the token generation process info.
#include "addons/RTDBHelper.h"  //Provide the RTDB payload printing info and other helper functions.
/************************************************/


/*************** Macro Defentions Section ***************/

/************ Network & Firebase Section ************/
// Insert your network credentials(ssid : Name)
#define WIFI_SSID       "Omar"
#define WIFI_PASSWORD   "0123456789"  

// Insert Firebase project API Key
#define API_KEY       "AIzaSyBuqRJ65YOWkPKruUfNekaekYqsH174HNw"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL  "https://learn-iot-esp32-default-rtdb.firebaseio.com/" 

/************ Interfacing Section ************/
/* System Constraints */
#define INITIAL_HORIZONTAL_ANGLE    (90)
#define INITIAL_VERTICAL_ANGLE      (90)
#define PID_OUTPUT_LIMITS           (5)
#define PID_KP                      (0.01)
#define PID_KI                      (0)
#define PID_KD                      (0)

/* 4 LDRs Sensors */
#define LDR_TOP             (36)
#define LDR_LEFT            (39)
#define LDR_BOTTOM          (34)
#define LDR_RIGHT           (35)


/* 2 Servo Motors */
#define SERVO_HORIZONTAL        (19)
#define SERVO_VERTICAL          (18)

/* Voltage and Current of the cell  */
#define CELL_VOLTAGE            (32)
#define CELL_CURRENT            (33)
/********************************************************/



/*************** Function Decleration Section ***************/
void manualMode(void); // using IOT : Flutter & Firebase
void automaticMode(void); // using LDRs
/************************************************************/



/*************** Global Decleration Section ***************/
// Define Motors and their angle
Servo ServoHorizontal; 
Servo ServoVertical;
int HorizontalAngle = INITIAL_HORIZONTAL_ANGLE; 
int VerticalAngle = INITIAL_VERTICAL_ANGLE; 

//Define Firebase Data object
FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;
bool signupOK = false;
unsigned long sendDataPrevMillis = 0;

// PID constants
double InputVertical, InputHorizontal;
double SetPoint;
double VerticalOutput, HorizontalOutput;

// PID Controllers
ArduPID VerticalPID;
ArduPID HorizontalPID;

// Cell Volatge and Current
double CellVoltage = 0;
double CellCurrent = 0;
/**********************************************************/



/*************** Setup Application Section ***************/
void setup() {
    /********** Setup Serial **********/
    Serial.begin(115200);

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
    // Assign the api key (required)
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

    /********** Setup Servo Motors **********/
    ServoHorizontal.attach(SERVO_HORIZONTAL);
    ServoVertical.attach(SERVO_VERTICAL);

    ServoHorizontal.write(INITIAL_HORIZONTAL_ANGLE);
    ServoVertical.write(INITIAL_VERTICAL_ANGLE);
    

    /* Initializing The PID Controllers and Constants */
    
    VerticalPID.begin(&InputVertical, &VerticalOutput, &SetPoint, PID_KP, PID_KI, PID_KD);
    VerticalPID.setOutputLimits(-PID_OUTPUT_LIMITS, PID_OUTPUT_LIMITS);

    HorizontalPID.begin(&InputHorizontal, &HorizontalOutput, &SetPoint, PID_KP, PID_KI, PID_KD);
    HorizontalPID.setOutputLimits(-PID_OUTPUT_LIMITS, PID_OUTPUT_LIMITS);
}
/*********************************************************/



/*************** Start Application Section ***************/
void loop() {
    if ((Firebase.ready()) && (signupOK) && ((millis() - sendDataPrevMillis > 1500) || (sendDataPrevMillis == 0))) 
    {
        sendDataPrevMillis = millis();
        
        if (Firebase.RTDB.getBool(&firebaseData, "/ESP/Mode")) 
        {
            if (firebaseData.boolData() == false) 
            {
                Serial.println("Mode : Manual mode");
                manualMode();
            }
            else 
            {
                Serial.println("Mode : Automatic mode");
                automaticMode();
            }
        }

        if (Firebase.RTDB.setInt(&firebaseData, "/ESP/CellVoltage", CellVoltage)) 
        {
        }else {Serial.println(firebaseData.errorReason());}

        if (Firebase.RTDB.setInt(&firebaseData, "/ESP/CellCurrent", CellCurrent)) 
        {
        }else {Serial.println(firebaseData.errorReason());}
    }
    // Power of Cell = 1.5 W (V * I)
    CellVoltage = random(100, 481) / (100.0);
    CellCurrent = (random(10, 15) / 10.0) / (CellVoltage);
}
/*********************************************************/



/*************** Function Defintions Section ***************/
void manualMode(void){
        if (Firebase.RTDB.getInt(&firebaseData, "/ESP/HorizontalAngle")) {
            if (firebaseData.dataType() == "int") {
                HorizontalAngle = firebaseData.intData();
                Serial.print("HorizontalAngle = ");
                Serial.println(HorizontalAngle);
                ServoHorizontal.write(HorizontalAngle);
            }   
        }
        else {
            Serial.println(firebaseData.errorReason());
        }

        if (Firebase.RTDB.getInt(&firebaseData, "/ESP/VerticalAngle")) {
            if (firebaseData.dataType() == "int") {
                VerticalAngle = firebaseData.intData();
                Serial.print("VerticalAngle = ");
                Serial.println(VerticalAngle);
                ServoVertical.write(VerticalAngle);
            }
        }
        else {
            Serial.println(firebaseData.errorReason());
        }
}


void automaticMode(void){
    // Reading LDRs
    int top     = analogRead(LDR_TOP);
    int bottom  = analogRead(LDR_BOTTOM);
    int right   = analogRead(LDR_RIGHT); 
    int left    = analogRead(LDR_LEFT);

    // Calculate the output of Vertical Servo 
    SetPoint = 0;

    InputVertical = bottom - top;
    VerticalPID.compute();
    ServoVertical.write(ServoVertical.read() + VerticalOutput);

    /* Calculate the output of Horizontal Servo */ 
    InputHorizontal = left - right;
    HorizontalPID.compute();
    ServoHorizontal.write(ServoHorizontal.read()+ HorizontalOutput);

    delay(50);
}
/***********************************************************/
