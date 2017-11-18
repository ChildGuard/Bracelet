#define PIN_PANIC_BTN 9 // the actual panic button
#define PIN_PANIC_LED 13 // the led which shows if panic mode is on or not; on-board LED
#define SERIAL_GSM_RX 5 // GSM RX pin,
#define SERIAL_GSM_TX 6 // GSM TX pin,
#define SERIAL_GPS_RX 2 // GPS RX pin, blue cable
#define SERIAL_GPS_TX 3 // GPS TX pin, yellow cable
#define SERIAL_GPS_BAUD 9600
#define SERIAL_GSM_BAUD 9600
#define SERIAL_BAUD 9600
// #define SERVER_HOST "childguard.anthony-nunez.me"
// #define SERVER_PORT 5905
#define READGSM while(SerialGSM.available() > 0) Serial.write(SerialGSM.read())

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

void panicButtonPressed();
void panicHandling();
bool setupGSM(); // returns true if successful, false if unsuccessful
void connectTCP();
void disconnectTCP();
void sendMessage(String);
String sendAT(String);
static void smartDelay(unsigned long);
void getGPSInfo();
void blinkLED(int, int, int, int);

// Global Variables
int triggerInt = 5; // trigger interval for activating panic mode (in seconds)
int panicMode = 0; // tracks the current state of panic mode; false by default (0)
float gps_lat = 0.0;
float gps_lng = 0.0;

// Global Object Declarations
SoftwareSerial SerialGSM(SERIAL_GSM_RX, SERIAL_GSM_TX); // RX, TX
SoftwareSerial SerialGPS(SERIAL_GPS_RX, SERIAL_GPS_TX); // RX, TX
TinyGPSPlus gps;

const char SERVER[] = "childguard.anthony-nunez.me";
const int TCP_PORT = 5916;

char charRead = ' ';

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

    connectTCP();
    delay(5000);

    // int i;
    // SerialGPS.listen();
    // delay(3000);
    // for(i = 0; i < 5; i++){
    //   Serial.print("Iteration ");
    //   Serial.println(i);
    //   while(SerialGPS.available() > 0){
    //     if(gps.encode(SerialGPS.read())) getGPSInfo();
    //   }
    //
    //   delay(3000);
    // }
    //
    // delay(3000);
    // Serial.println("Disconnecting from TCP...");
    // disconnectTCP();

    delay(1000);
    Serial.println("Going into loop...");
}

void loop() {
    // // put your main code here, to run repeatedly:
    if(digitalRead(PIN_PANIC_BTN) && !panicMode){
        panicButtonPressed();
    }
	  if(panicMode){
        panicHandling();
    }
    // getGPSInfo();
    //
    // delay(500);

    // if(!SerialGPS.isListening()) SerialGPS.listen();
    // smartDelay(10000);
    // getGPSInfo();
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

  smartDelay(5 * 1000);
  getGPSInfo();

  String locMsg = "$GPSLOC=";
  locMsg += "{\"lat\":";
  locMsg += String(gps_lat,6);
  locMsg += ",\"lng\":";
  locMsg += String(gps_lng,6);
  locMsg += "}!";

  Serial.print("Sending message: ");
  Serial.println(locMsg);
  sendMessage(locMsg);

  // delay(5000);

  Serial.println("Panic mode off!");
	panicMode = 0;
	digitalWrite(PIN_PANIC_LED, LOW);
}

bool setupGSM(){
  String returnString = "";
  String test = "";

  if(sendAT("AT") == "\r\nERROR\r\n") return false; // Check if module is up

  // EDIT THESE TO CONSIDER THAT THE SETTINGS MIGHT ALREADY BE SET IN MEMORY!!!
  test = "+CSTT: \"wholesale\"";
  returnString = sendAT("AT+CSTT?");
  Serial.print("Comparison check: ");
  Serial.println(returnString.startsWith(test, 2));
  // if(!returnString.startsWith(test, 2)){
  //   if(sendAT("AT+CSTT=\"wholesale\"") == "\r\nERROR\r\n") return false;
  // }
  sendAT("AT+CSTT=\"wholesale\"");
  // if(sendAT("AT+CIICR=?") != "\r\nOK\r\n"){
    sendAT("AT+CIICR");
  // }
  //if(sendAT("AT+CIFSR") == "\r\nERROR\r\n") return false;
  sendAT("AT+CIFSR");
  return true;
}

void connectTCP(){
  String command = "AT+CIPSTART=\"TCP\",\"";
  command += SERVER;
  command += "\",\"";
  command += TCP_PORT;
  command += "\"";

  Serial.println(sendAT(command));
}

void disconnectTCP(){
  sendAT("AT+CIPCLOSE");
}

void sendMessage(String message){
  sendAT("AT+CIPSEND");
  delay(1000);
  while(SerialGSM.available() > 0) Serial.write(SerialGSM.read());
  delay(250);
  SerialGSM.print(message);
  SerialGSM.print((char)0x1a);

  while(SerialGSM.available() < 1);

  READGSM;
  delay(500);
}

String sendAT(String command){
  String serialIn = "";

  if(!SerialGSM.isListening()) SerialGSM.listen();

  Serial.print(command);
  Serial.print("... ");
  SerialGSM.println(command);
  delay(250);

  while(SerialGSM.available() > 0) serialIn += (char)SerialGSM.read();

  //serialIn = serialIn.substring(2, serialIn.length() - 2);
  //Serial.println(serialIn);

  return serialIn;
}

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    if(!SerialGPS.isListening()) SerialGPS.listen();
    delay(100);
    while (SerialGPS.available())
      gps.encode(SerialGPS.read());
  } while (millis() - start < ms);
}

void getGPSInfo(){
    if(gps.location.isValid()){
        gps_lat = gps.location.lat();
        gps_lng = gps.location.lng();

        Serial.print("Location: ");
        Serial.print(gps_lat, 8);
        Serial.print(',');
        Serial.println(gps_lng, 8);
    }
    else{
        Serial.println("INVALID");
    }

    // Serial.print(F("  Date/Time: "));
    // if (gps.date.isValid())
    // {
    //   Serial.print(gps.date.month());
    //   Serial.print(F("/"));
    //   Serial.print(gps.date.day());
    //   Serial.print(F("/"));
    //   Serial.print(gps.date.year());
    //   Serial.print(F("; "));
    //   Serial.print(gps.time.hour());
    //   Serial.print(F(":"));
    //   Serial.print(gps.time.minute());
    //   Serial.print(F(":"));
    //   Serial.print(gps.time.second());
    // }

    // Serial.println();
    // delay(3000);
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
