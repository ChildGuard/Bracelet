#include "Panic.h"
#include "Arduino.h"

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
