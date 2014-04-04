/*---------------------------------------------------------------------------
 * Copyright (c) 2000, 2001 connectBlue AB, Sweden.
 * Any reproduction without written permission is prohibited by law.
 *
 * Component   : swi2c_master
 * File        : cb_swi2c_master.c
 *
 * Description : I2C implemented using SWI for the TL1 (cB-0950) PCB.
 *               Based on the application note slva302 available for
 *               download at www.ti.com.
 *
 *                  Master                   
 *                  CC2540        
 *             -----------------              
 *            |         SDA/P1.6|-------> [I2C SLAVE SDA]
 *            |         SCL/P1.5|-------> [I2C SLAVE SCL]
 *            |                 |
 *             -----------------
 *
 *               TBD - rewrite the all operations using macros 
 *               with clearifying names. 
 *
 *-------------------------------------------------------------------------*/

#include "hal_mcu.h"
#include "swi2c_master.h"

/*===========================================================================
 * DEFINES
 *=========================================================================*/
#define BIT0  0x01
#define BIT1  0x02
#define BIT2  0x04
#define BIT3  0x08
#define BIT4  0x10
#define BIT5  0x20
#define BIT6  0x40
#define BIT7  0x80

#define PxSEL P1SEL  // Port selection
#define PxDIR P1DIR  // Port direction
#define Px    P1     // Port input/output
#define PxIN  P1INP  // Port input configuration

#define SDA   BIT6 // Controls SDA line (pull-up used for logic 1)
#define SCL   BIT5 // Controls SCL line (pull-up used for logic 1)

/*===========================================================================
 * TYPES
 *=========================================================================*/


/*===========================================================================
 * DECLARATIONS
 *=========================================================================*/

/*===========================================================================
 * DEFINITIONS
 *=========================================================================*/
static void _NOP(void){asm("NOP");}

/*===========================================================================
 * FUNCTIONS
 *=========================================================================*/


void delayUs(int microSecs) {
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

/*---------------------------------------------------------------------------
* Description of function. Optional verbose description.
*-------------------------------------------------------------------------*/
void SWI2C_init(void)
{
  PxSEL &= ~(SCL | SDA);                    // Set GPIO function
  PxDIR &= ~(SCL | SDA);                    // Set output direction to input
  Px    &= ~(SCL | SDA);                    // SCL & SDA = 0, low when = outputs 

  PxIN  &= ~(SCL | SDA);                    // Pull up or down
  P2INP &= ~0x40;                           // Configure port 1 to pull up
}

/*---------------------------------------------------------------------------
* Description of function. Optional verbose description.
*-------------------------------------------------------------------------*/
void SWI2C_start(void)            // Set up start condition for I2C
{
  PxDIR &= ~SCL;                           // Set to input, SCL = 1 via pull-up
  _NOP();                                   // Quick delay
  _NOP();                                   // Quick delay
  PxDIR |= SDA;                             // Set to output, data low [SDA = 0]
  _NOP();                                   // Quick delay
  _NOP();                                   // Quick delay
  PxDIR |= SCL;                             // Set to output, data low [SCL = 0]
}

/*---------------------------------------------------------------------------
* Description of function. Optional verbose description.
*-------------------------------------------------------------------------*/
void SWI2C_wait_ack(void)
{
  PxDIR &= ~SDA;                            // Set to input, SDA = 1 via pull-up    
  PxDIR &= ~SCL;                            // Set to input, SCL = 1 via pull-up
  _NOP();
  _NOP();

  // TBD break if ack is not received within a certain time
  while (PxIN & SDA == SDA)
  {
    _NOP();
  }

  PxDIR |= SCL;                            // Set to output, data low [SCL = 0]  
}

/*---------------------------------------------------------------------------
* Description of function. Optional verbose description.
*-------------------------------------------------------------------------*/
void SWI2C_txByte(unsigned char data)
{
  unsigned char bits, temp;

  temp = data;
  bits = 0x08;                              // Load I2C bit counter
  while (bits != 0x00)                      // Loop until all bits are shifted
  {
    if (temp & BIT7)                        // Test data bit
      PxDIR &= ~SDA;                        // Set to input, SDA = 1 via pull-up
    else
      PxDIR |= SDA;                         // Set to output, data low [SDA = 0]
    PxDIR &= ~SCL;                          // Set to output, data low [SCL = 0]
    temp = (temp << 1);                     // Shift bits 1 place to the left
    _NOP();                                 // Quick delay
    PxDIR |= SCL;                           // Set to output, data low [SCL = 0]
    _NOP();                                 // Quick delay
    bits = (bits - 1);                      // Loop until 8 bits are sent
  }

  SWI2C_wait_ack();               // wait for acknowledge
}

/*---------------------------------------------------------------------------
* Description of function. Optional verbose description.
*-------------------------------------------------------------------------*/
void SWI2C_ack(void)              // Set up for I2C acknowledge
{
  PxDIR |= SCL;                            // Set to output, data low [SCL = 0]
  PxDIR |= SDA;                         // Set to output, data low [SDA = 0]

  PxDIR &= ~SCL;                           // Set to input, SCL = 1 via pull-up
  _NOP();                                  // delay to meet I2C spec
  _NOP();                                  //   "        "        "
  PxDIR |= SCL;                            // Set to output, data low [SCL = 0]
  PxDIR &= ~SDA;                        // Set to input, SDA = 1 via pull-up
}

/*---------------------------------------------------------------------------
* Description of function. Optional verbose description.
*-------------------------------------------------------------------------*/
unsigned char SWI2C_rxByte(bool ack)  // Read 8 bits of I2C data
{
  unsigned char bits, temp = 0;            // I2C bit counter

  PxDIR &= ~SDA;                            // Set to input, SDA = 1 via pull-up  

  bits = 0x08;                             // Load I2C bit counter
  while (bits > 0)                         // Loop until all bits are read
  {
    PxDIR &= ~SCL;                        // Set to input, SDL = 1 via pull-up
    _NOP();                                 // Quick delay
    temp = (temp << 1);                   // Shift bits 1 place to the left
    if (Px & SDA)                         // Check digital input
      temp = (temp + 1);                  // If input is 'H' store a '1'
    _NOP();                               // Quick delay
    PxDIR |= SCL;                         // Set to output, data low [SCL = 0]
    _NOP();                               // Quick delay      
    bits = (bits - 1);                    // Decrement I2C bit counter
  }
  if (ack == TRUE)
  {
    SWI2C_ack();                   // Send acknowledge
  }
  return (temp);                           // Return 8-bit data byte
}   



/*---------------------------------------------------------------------------
* Description of function. Optional verbose description.
*-------------------------------------------------------------------------*/
void SWI2C_stop(void)             // Send I2C stop command
{
  PxDIR |= SDA;                             // Set to output, data low [SCA = 0]
  _NOP();                                   // Quick delay
  _NOP();                                   // Quick delay
  PxDIR &= ~SCL;                            // Set to input, SCL = 1 via pull-up
  _NOP();                                   // Quick delay
  _NOP();                                   // Quick delay
  PxDIR &= ~SDA;                            // Set to input, SDA = 1 via pull-up
} 

/*---------------------------------------------------------------------------
* Description of function. Optional verbose description.
*-------------------------------------------------------------------------*/
void SWI2C_readBlock(unsigned char SlaveAddress,
                     unsigned int numBytes, 
                     void* RxData)
{
  unsigned char* temp;
  temp = (unsigned char *)RxData;           // Initialize array pointer

  SWI2C_start();                  // Send Start condition
  SWI2C_txByte((SlaveAddress << 1) | BIT0); // [ADDR] + R/W bit = 1
  delayUs(10);
  for (unsigned int i = 0; i < numBytes; i++) {
    if (i == numBytes -1)
      *(temp) = SWI2C_rxByte(FALSE);     // Read 8 bits of data and send nack
    else 
      *(temp) = SWI2C_rxByte(TRUE);     // Read 8 bits of data and send acks
    temp++;                                 // Increment pointer to next element
  }
  SWI2C_stop();                   // Send Stop condition
}

/*---------------------------------------------------------------------------
* Description of function. Optional verbose description.
*-------------------------------------------------------------------------*/
void SWI2C_writeBlock(unsigned char SlaveAddress,
                      unsigned int numBytes, void* TxData)
{        
  unsigned char *temp;

  temp = (unsigned char *)TxData;          // Initialize array pointer
  SWI2C_start();                 // Send Start condition
  SWI2C_txByte((SlaveAddress << 1) & ~BIT0); // [ADDR] + R/W bit = 0
  delayUs(150);
  for (unsigned int i = 0; i < numBytes; i++) {
    SWI2C_txByte(*(temp));       // Send data and ack
    temp++;                                // Increment pointer to next element
  }
  SWI2C_stop();                  // Send Stop condition
}

/*===========================================================================
 * STATIC FUNCTIONS
 *=========================================================================*/







