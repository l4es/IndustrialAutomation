/*
 * SOES Simple Open EtherCAT Slave
 *
 * File    : esc_hw.c
 * Version : 0.9.2
 * Date    : 22-02-2010
 * Copyright (C) 2007-2010 Arthur Ketels
 *
 * SOES is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License version 2 as published by the Free
 * Software Foundation.
 *
 * SOES is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * As a special exception, if other files instantiate templates or use macros
 * or inline functions from this file, or you compile this file and link it
 * with other works to produce a work based on this file, this file does not
 * by itself cause the resulting work to be covered by the GNU General Public
 * License. However the source code for this file must still be made available
 * in accordance with section (3) of the GNU General Public License.
 *
 * This exception does not invalidate any other reasons why a work based on
 * this file might be covered by the GNU General Public License.
 *
 * The EtherCAT Technology, the trade name and logo "EtherCAT" are the intellectual
 * property of, and protected by Beckhoff Automation GmbH.
 */
#include "stm32f0xx.h"
#include "utypes.h"

#define ESC_CMD_READ    0x02
#define ESC_CMD_READWS  0x03
#define ESC_CMD_WRITE   0x04
#define ESC_CMD_NOP     0x00
#define ESC_TERM        0xff
#define ESC_NEXT        0x00

#ifndef WSREAD

/** \fn uint8 ESC_read(uint16 address, void *buf, uint8 len, void *tALevent)
    \brief Read a value from the ESC.
    \param address ESC address
    \param *buf pointer to buffer of uint8s in RAM
    \param len length of buffer in RAM
    \param *tALevent pointer to Application Layer event
*/
uint8 ESC_read(uint16 address, void *buf, uint8 len, void *tALevent)
{
    uint8 pstat;
    uint8 count;
    uint8 *ptr;
    uint16union adr;
    //SPI_Cmd(SPI1, ENABLE); //pull nSS low
    GPIO_ResetBits(GPIOA, GPIO_Pin_4);
    adr.w = address << 3;
    pstat = !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6); //last cmd result; read MISO pin state and invert
    SPI_SendData8(SPI1,adr.b[1]);  //write first address byte
    ptr = tALevent;
    while ( SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) != SET);    //wait until SPI transaction (r+w) is complete
    *(ptr) = SPI_ReceiveData8(SPI1);                              //write received byte to tAlevent
    SPI_SendData8(SPI1,adr.b[0] + ESC_CMD_READ);                  //write second address byte, with read command
    ptr++;                                                        //increase write pointer
    count = len;                                                  //
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) != SET);    //wait until SPI transaction (r+w) is complete
    *(ptr) = SPI_ReceiveData8(SPI1);                              //read second received byte, write to tAtlevent+1
    ptr = buf;                                                    //let ptr point to the buffer location
    while (count > 1)                                             //while number of received bytes is smaller than len
    {
        SPI_SendData8(SPI1,ESC_NEXT);                                 //write dummy byte to start transaction
        count--;                                                      //decrease bytecounter
        while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) != SET);    //wait until SPI transaction is complete
        *(ptr) = SPI_ReceiveData8(SPI1);                              //save data to next place in buffer
        ptr++;                                                        //increase buffer pointer
    }
    SPI_SendData8(SPI1, ESC_TERM);                                //write last byte
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) != SET);    //wait until transaction is complete

    *(ptr) = SPI_ReceiveData8(SPI1);                              //save last byte to last buffer location
    while(SPI_GetTransmissionFIFOStatus(SPI1) !=0);
    while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_BSY)== SET);
    while(SPI_GetReceptionFIFOStatus(SPI1) != 0)
    {
        SPI_ReceiveData8(SPI1);
    }
    //SPI_Cmd(SPI1, DISABLE);                                       //set SS high
    GPIO_SetBits(GPIOA, GPIO_Pin_4);
    return pstat;      //return inverted MISO state from previous transaction
}
#endif

#ifdef WSREAD
// use read with wait state byte, needed if SPI>12Mhz or>16KB addressing
/** void ESC_read(uint16 address, void *buf, uint8 len, void *tALevent)
    \brief read function for SPI ESC interface ("PDI")
    \param address - address of register in ESC
    \param *buf pointer to read buffer in microcontroller
    \param len  length of memory section to read, from start of 'address'
    \param *tAlevent microcontroller status register to write response to
*/
uint8 ESC_read(uint16 address, void *buf, uint8 len, void *tALevent)
{
    uint8 pstat;
    uint8 count;
    uint8 *ptr;
    uint16union adr;
    SPI_Cmd(SPI1, ENABLE); //pull nSS low
    adr.w = address << 3;
    pstat = !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);
    SPI_SendData8(SPI1, adr.b[1]);                                //write first address byte
    ptr = tALevent;
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) != SET);    //wait until SPI transaction (r+w) is complete
    *(ptr) = SPI_ReceiveData8(SPI1);                              //write received byte to tAlevent
    SPI_SendData8(SPI1, adr.b[0] + ESC_CMD_READWS);               //write second address byte, with readws command
    ptr++;                                                        //increase write pointer
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) != SET);    //wait until SPI transaction (r+w) is complete
    *(ptr) = SPI_ReceiveData8(SPI1);	                            //write received byte to tAlevent+1
    SPI_SendData8(SPI1, ESC_TERM);		                        //write byte to start transaction //COMMENT: Why not ESC_NEXT?
    ptr = buf;			                                        //change ptr to point to buffer
    count = len;                                                  //
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) != SET);    //wait until SPI transaction (r+w) is complete
    while (count > 1)                                             //while number of received bytes is smaller than len
    {
        SPI_SendData8(SPI1, ESC_NEXT);                                //write dummy byte to start transaction
        count--;                                                      //decrease bytecounter
        while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) != SET);    //wait until SPI transaction is complete
        *(ptr) = SPI_ReceiveData8(SPI1);                              //save data to next place in buffer
        ptr++;                                                        //increase buffer pointer
    }
    SPI_SendData8(SPI1, ESC_TERM);                                //write last byte
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) != SET);    //wait until transaction is complete
    *(ptr) = SPI_ReceiveData8(SPI1);                              //save last byte to last buffer location
    SPI_Cmd(SPI1, DISABLE);                               //set SS high
    return pstat;                                                 //return inverted MISO state from previous transaction
}
#endif

/** void ESC_write(uint16 address, void *buf, uint8 len, void *tALevent)
    \brief write function for SPI ESC interface ("PDI")
    \param address - address of register in ESC
    \param *buf pointer to data buffer in microcontroller, buffer contains 'len' bytes to write to location 'address'
    \param len  length of memory section to write, from start of 'address'
    \param *tAlevent microcontroller status register to write response to
*/
uint8 ESC_write(uint16 address, void *buf, uint8 len, void *tALevent)
{
    uint8 pstat;
    uint8 count;
    uint8 *ptr;
    uint16union adr;
    adr.w = address << 3;                                      //shift 16-bit address value 3 places, then write to adr (union of uint16 and uint8[2])
    pstat = !3;                                        //last cmd result   ///////COMMENT: Shouldn't this be  pstat = !(PINB & (1 << i_spimiso));? Otherwise just return 0
    //SPI_Cmd(SPI1, ENABLE); //pull nSS low
    GPIO_ResetBits(GPIOA, GPIO_Pin_4);
    SPI_SendData8(SPI1, adr.b[1]);                                   //write first address byte
    ptr = tALevent;                                            //set pointer to tAlevent
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) != SET); //wait until SPI byte transaction is complete
    *(ptr) = SPI_ReceiveData8(SPI1);                           //read received byte, writ to tAlevent location
    SPI_SendData8(SPI1, adr.b[0] + ESC_CMD_WRITE);                   //write second address byte with write command
    ptr++;                                                     //increase pointer to second byte of tAlevent
    count = len;                                               //
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) != SET); //wait until SPI byte transaction is complete
    *(ptr) = SPI_ReceiveData8(SPI1);                           //write received byte into tAlevent second location
    ptr = buf;                                                 //set pointer to buffer location
    while (count > 0)                                          //while number of received bytes is smaller than len
    {
        SPI_SendData8(SPI1, *(ptr));                                        //write byte from buffer
        ptr++;                                                        //increase buffer pointer
        count--;                                                      //decrease counter
        while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) != SET);    //wait until transaction complete
    }
    while(SPI_GetTransmissionFIFOStatus(SPI1) !=0);
    while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_BSY)== SET);
    while(SPI_GetReceptionFIFOStatus(SPI1) != 0)
    {
        SPI_ReceiveData8(SPI1);
    }
    //SPI_Cmd(SPI1, DISABLE);                              //set SS high
    GPIO_SetBits(GPIOA, GPIO_Pin_4);
    return pstat;
}

