#ifndef GSM_h
#define GSM_h

#define SERIAL_GSM_RX 5 // GSM RX pin, blue
#define SERIAL_GSM_TX 6 // GSM TX pin, yellow
#define SERIAL_GSM_BAUD 9600
#define SERVER_HOST "childguard.anthony-nunez.me"
#define SERVER_PORT 5916

#include "Arduino.h"
#include "SoftwareSerial.h"

SoftwareSerial SerialGSM(SERIAL_GSM_RX, SERIAL_GSM_TX); // RX, TX

bool setupGSM(); // returns true if successful, false if unsuccessful
String sendAT(String);

#endif
