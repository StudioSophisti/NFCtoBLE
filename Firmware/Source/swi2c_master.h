#ifndef _SWI2C_MASTER_H_
#define _SWI2C_MASTER_H_

/*---------------------------------------------------------------------------
 * Copyright (c) 2000, 2001 connectBlue AB, Sweden.
 * Any reproduction without written permission is prohibited by law.
 *
 * Component   : Tracer
 * File        : cb_swi2c_master.h
 *
 * Description : See swi2c_master.c
 *-------------------------------------------------------------------------*/


/*===========================================================================
 * DEFINES
 *=========================================================================*/


/*===========================================================================
 * TYPES
 *=========================================================================*/


/*===========================================================================
 * FUNCTIONS
 *=========================================================================*/

void SWI2C_init(void);
void SWI2C_start(void);
void SWI2C_txByte(unsigned char data);
unsigned char SWI2C_rxByte(bool ack);
void SWI2C_ack(void);
void SWI2C_stop(void);
void SWI2C_readBlock(unsigned char SlaveAddress, unsigned int numBytes, void* RxData);
void SWI2C_writeBlock(unsigned char SlaveAddress, unsigned int numBytes, void* TxData);

#endif






