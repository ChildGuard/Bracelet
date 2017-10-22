#define PIN_PANIC_BTN 9 // the actual panic button
#define PIN_PANIC_LED 13 // the led which shows if panic mode is on or not; on-board LED
#define SERIAL_GSM_RX 5 // GSM RX pin,
#define SERIAL_GSM_TX 6 // GSM TX pin,
#define SERIAL_GPS_RX 2 // GPS RX pin, blue cable
#define SERIAL_GPS_TX 3 // GPS TX pin, yellow cable
#define SERIAL_GPS_BAUD 9600
#define SERIAL_GSM_BAUD 115200
#define SERIAL_BAUD 115200

#define TINY_GSM_MODEM_SIM900

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <TinyGsmClient.h>

void panicButtonPressed();
void panicHandling();
void displayGPSInfo();
void blinkLED(int, int, int, int);

// Global Variables
int triggerInt = 5; // trigger interval for activating panic mode (in seconds)
int panicMode = 0; // tracks the current state of panic mode; false by default (0)
float gps_lat = 0.0;
float gps_lng = 0.0;

const char APN[] = "wholesale";
const char USER[] = "";
const char PASS[] = "";

// Global Object Declarations
SoftwareSerial SerialGSM(SERIAL_GSM_RX, SERIAL_GSM_TX); // RX, TX
SoftwareSerial SerialGPS(SERIAL_GPS_RX, SERIAL_GPS_TX); // RX, TX
TinyGPSPlus gps;
TinyGsm modem(SerialGSM);
TinyGsmClient client(modem);

const char SERVER[] = "rpi.anthony-nunez.me";
const int TCP_PORT = 5911;

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
    blinkLED(13, 5, 250, 500);

    // Restart takes quite some time
    // To skip it, call init() instead of restart()
    Serial.println(F("Initializing modem..."));
    modem.restart();

    // String modemInfo = modem.getModemInfo();
    // Serial.print("Modem: ");
    // Serial.println(modemInfo);

  	// pin setup
  	pinMode(PIN_PANIC_BTN, INPUT);
  	pinMode(PIN_PANIC_LED, OUTPUT);
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

    Serial.print(F("Waiting for network..."));
    if (!modem.waitForNetwork()) {
      Serial.println(" fail");
      delay(10000);
      return;
    }
    Serial.println(" OK");

    Serial.print(F("Connecting to "));
    Serial.print(APN);
    if (!modem.gprsConnect(APN, USER, PASS)) {
      Serial.println(" fail");
      delay(10000);
      return;
    }
    Serial.println(" OK");

    Serial.print(F("Connecting to "));
    Serial.print(SERVER);
    if (!client.connect(SERVER, TCP_PORT)) {
      Serial.println(" fail");
      delay(10000);
      return;
    }
    Serial.println(" OK");

    // Make a HTTP GET request:
    // client.print(String("GET ") + resource + " HTTP/1.0\r\n");
    client.print(String("Host: ") + SERVER + "\r\n");
    client.print("Connection: close\r\n\r\n");

    // unsigned long timeout = millis();
    // while (client.connected() && millis() - timeout < 10000L) {
    //   // Print available data
    //   while (client.available()) {
    //     char c = client.read();
    //     Serial.print(c);
    //     timeout = millis();
    //   }
    // }
    Serial.println();

    client.stop();
    Serial.println("Server disconnected");

    modem.gprsDisconnect();
    Serial.println("GPRS disconnected");

    blinkLED(13, 10, 1000, 1000);
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
