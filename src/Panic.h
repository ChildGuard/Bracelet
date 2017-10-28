#ifndef PANIC_H
#define PANIC_H

#define PIN_PANIC_BTN 9 // the actual panic button
#define PIN_PANIC_LED 13 // the led which shows if panic mode is on or not; on-board LED

int triggerInt = 5; // trigger interval for activating panic mode (in seconds)
int panicMode = 0; // tracks the current state of panic mode; false by default (0)

void panicButtonPressed();
void panicHandling();

#endif
