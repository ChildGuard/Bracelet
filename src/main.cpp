#include "./modules/GSM.h"
#include "./modules/GPS.h"
#include "Panic.h"
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

#define SERIAL_BAUD 9600

void displayGPSInfo();
void blinkLED(int, int, int, int);

// Global Variables

float gps_lat = 0.0;
float gps_lng = 0.0;

const char APN[] = "wholesale";
const char USER[] = "";
const char PASS[] = "";

// Global Object Declarations
SoftwareSerial SerialGPS(SERIAL_GPS_RX, SERIAL_GPS_TX); // RX, TX
TinyGPSPlus gps;

const char SERVER[] = "73.244.68.162";
const int TCP_PORT = 5916;

void setup() {
    // put your setup code here, to run once:

  	Serial.begin(SERIAL_BAUD); // sets up the Serial library for debugging @ 9600 baud
    delay(200);
    SerialGPS.begin(SERIAL_GPS_BAUD); // sets up the GPS serial connection @ 9600 baud
    delay(200);
    SerialGSM.begin(SERIAL_GSM_BAUD);
    delay(1000);

    SerialGSM.listen();

    Serial.println("Serial devices are setup.");
    //blinkLED(13, 5, 250, 500);

  	// pin setup
  	pinMode(PIN_PANIC_BTN, INPUT);
  	pinMode(PIN_PANIC_LED, OUTPUT);

    if(setupGSM()) Serial.println("GSM serial is connected! :)");
    else Serial.println("GSM serial failed to connect. :(");

    delay(1000);
}

void loop() {
    // // put your main code here, to run repeatedly:
    // if(digitalRead(PIN_PANIC_BTN) && !panicMode){
    //     panicButtonPressed();
    // }
	  // if(panicMode){
    //     panicHandling();
    // }
    // if(SerialGPS.available() > 0 ){
    //   if(gps.encode(SerialGPS.read())) displayGPSInfo();
    // }
    //
    // delay(500);

    Serial.println("Done.");
    while(true); // Do nothing forever more...
}



void displayGPSInfo(){
    Serial.print("Location: ");
    if(gps.location.isValid()){
        gps_lat = gps.location.lat();
        gps_lng = gps.location.lng();

        Serial.print(gps_lat, 8);
        Serial.print(',');
        Serial.print(gps_lng, 8);
    }
    else{
        Serial.print("INVALID");
    }

    Serial.print(F("  Date/Time: "));
    if (gps.date.isValid())
    {
      Serial.print(gps.date.month());
      Serial.print(F("/"));
      Serial.print(gps.date.day());
      Serial.print(F("/"));
      Serial.print(gps.date.year());
      Serial.print(F("; "));
      Serial.print(gps.time.hour());
      Serial.print(F(":"));
      Serial.print(gps.time.minute());
      Serial.print(F(":"));
      Serial.print(gps.time.second());
    }

    Serial.println();
    delay(3000);
}

void blinkLED(int pin, int numBlinks, int durationOn, int durationOff){
  int i = 0;
  for(i = 0; i < numBlinks; i++){
    digitalWrite(pin, HIGH);
    delay(durationOn);
    digitalWrite(pin, LOW);
    delay(durationOff);
  }
}
