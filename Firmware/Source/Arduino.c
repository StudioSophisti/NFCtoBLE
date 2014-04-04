#include "Arduino.h"

void pinMode(cbPIO_Pin pin, cbPIO_Port port, cbPIO_Mode mode) {
  cbPIO_open(port, pin, NULL, mode, cbPIO_LOW); 
}

void digitalWrite(cbPIO_Pin pin, cbPIO_Port port, cbPIO_Value state) {
  cbPIO_write(port, pin, state);
}

cbPIO_Value digitalRead(cbPIO_Pin pin, cbPIO_Port port) {
  return cbPIO_read(port, pin);
}

void delayMicroseconds(int microSecs) {
   while(microSecs--)
  {
    /* 32 NOPs == 1 usecs */
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop");
  }  
}

void delay(int millis) {
  while(millis--)
  {
    delayMicroseconds(1000);
  }
}