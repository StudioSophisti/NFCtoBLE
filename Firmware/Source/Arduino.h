#ifndef Arduino_h
#define Arduino_h

#include "cb_pio.h"
#include "hal_types.h"

#define HIGH cbPIO_HIGH
#define LOW cbPIO_LOW
#define INPUT cbPIO_INPUT_PD
#define OUTPUT cbPIO_OUTPUT

typedef uint8 uint8_t;
typedef uint16 uint16_t;
typedef uint32 uint32_t;
typedef bool boolean;
typedef uint8 byte;

void pinMode(cbPIO_Pin pin, cbPIO_Port port, cbPIO_Mode mode);
void digitalWrite(cbPIO_Pin pin, cbPIO_Port port, cbPIO_Value state);
cbPIO_Value digitalRead(cbPIO_Pin pin, cbPIO_Port port);
void delayMicroseconds(int ms);
void delay(int ms);

#endif