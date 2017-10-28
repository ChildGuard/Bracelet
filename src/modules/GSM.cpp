#include "Arduino.h"
#include "GSM.h"

bool setupGSM(){
  String serialIn = "";

  if(sendAT("AT") != "OK") return false;

  return true;
}

String sendAT(String command){
  String serialIn = "";

  if(!SerialGSM.isListening()) SerialGSM.listen();

  SerialGSM.println(command);
  delay(250);

  while(SerialGSM.available() > 0) serialIn += (char)SerialGSM.read();

  serialIn = serialIn.substring(2, serialIn.length() - 2);

  return serialIn;
}
