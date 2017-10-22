#define PIN_PANIC_BTN 9 // the actual panic button
#define PIN_PANIC_LED 13 // the led which shows if panic mode is on or not; on-board LED
#define SERIAL_GSM_RX 5 // GSM RX pin,
#define SERIAL_GSM_TX 6 // GSM TX pin,
#define SERIAL_GPS_RX 2 // GPS RX pin, blue cable
#define SERIAL_GPS_TX 3 // GPS TX pin, yellow cable
#define SERIAL_GPS_BAUD 9600
#define SERIAL_BAUD 115200

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

void panicButtonPressed();
void panicHandling();
void displayGPSInfo();
void blinkLED(int, int, int, int);

// Global Variables
int triggerInt = 5; // trigger interval for activating panic mode (in seconds)
int panicMode = 0; // tracks the current state of panic mode; false by default (0)
float gps_lat = 0.0;
float gps_lng = 0.0;

// Global Object Declarations
// SoftwareSerial SerialGSM(SERIAL_GSM_RX, SERIAL_GSM_TX); // RX, TX
SoftwareSerial SerialGPS(SERIAL_GPS_RX, SERIAL_GPS_TX); // RX, TX
TinyGPSPlus gps;

void setup() {
    // put your setup code here, to run once:

  	Serial.begin(SERIAL_BAUD); // sets up the Serial library for debugging @ 9600 baud
    delay(200);
    SerialGPS.begin(SERIAL_GPS_BAUD); // sets up the GPS serial connection @ 9600 baud
    delay(1000);

    Serial.println("Serial devices are setup.");
    blinkLED(13, 5, 250, 500);

    // SerialGPS.listen();
    // GPS.sendCommand("$PGCMD,33,0*6D"); // Turn Off GPS Antenna Update
    // GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA); // Tell GPS we want only $GPRMC and $GPGGA NMEA sentences
    // GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
    // delay(1000);  // Pause

  	// pin setup
  	pinMode(PIN_PANIC_BTN, INPUT);
  	pinMode(PIN_PANIC_LED, OUTPUT);
}

void loop() {
    // put your main code here, to run repeatedly:
    if(digitalRead(PIN_PANIC_BTN) && !panicMode){
        panicButtonPressed();
    }
	  if(panicMode){
        panicHandling();
    }
    if(SerialGPS.available() > 0 ){
      if(gps.encode(SerialGPS.read())) displayGPSInfo();
    }

    delay(500);
}

// gets called when the panic button is pressed
void panicButtonPressed(){
  int seconds = 0;

	while(digitalRead(PIN_PANIC_BTN) && seconds <= triggerInt){
		delay(500);
		digitalWrite(PIN_PANIC_LED, HIGH);
		delay(500);
		digitalWrite(PIN_PANIC_LED, LOW);
		seconds++;
		Serial.print("Button held for ");
		Serial.print(seconds);
		Serial.println(" seconds");

		if(seconds == triggerInt) panicMode = 1;
	}
}

// gets called when the panic mode is activated
void panicHandling(){
	Serial.println("Panic mode active!");
	digitalWrite(PIN_PANIC_LED, HIGH);
	delay(5000);

  Serial.println("Panic mode off!");
	panicMode = 0;
	digitalWrite(PIN_PANIC_LED, LOW);
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
