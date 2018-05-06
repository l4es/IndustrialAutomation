/*
 * SOES Simple Open EtherCAT Slave
 *
 * File    : cpuinit.c
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
//#include <avr/io.h>
#include "cpuinit.h"
#include "stm32f0xx.h"

void cpuinit(void)
{
    GPIO_InitTypeDef        GPIO_InitStructure;
    SPI_InitTypeDef         SPI_InitStructure;
    //DMA_InitTypeDef         DMA_InitStructure;

        /* SPI1 Clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    /* GPIOC Periph clock enable */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
    /* GPIOA Periph clock enable */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    /* DMA Perpiph clock Enable */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    //GPIO_PinAFConfig(GPIOA, GPIO_Pin_4, GPIO_AF_0);
    GPIO_PinAFConfig(GPIOA, GPIO_Pin_5, GPIO_AF_0); //SCK
    GPIO_PinAFConfig(GPIOA, GPIO_Pin_6, GPIO_AF_0); //MISO
    GPIO_PinAFConfig(GPIOA, GPIO_Pin_7, GPIO_AF_0); //MOSI

    /* Configure PC8 and PC9 in output pushpull mode */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    //Set SCK, MISO, MOSI pins as alternate function
    GPIO_InitStructure.GPIO_Pin =  /*GPIO_Pin_4 |*/ GPIO_Pin_5 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
/*NSS pin*/
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
/*MISO pin*/
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    SPI_InitStructure.SPI_BaudRatePrescaler    = SPI_BaudRatePrescaler_32;
    SPI_InitStructure.SPI_CPHA                 = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_CPOL                 = SPI_CPOL_High;
    SPI_InitStructure.SPI_DataSize             = SPI_DataSize_8b;
    SPI_InitStructure.SPI_Direction            = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_FirstBit             = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_Mode                 = SPI_Mode_Master;
    SPI_InitStructure.SPI_NSS                  = SPI_NSS_Soft;
    SPI_Init(SPI1, &SPI_InitStructure);
    SPI_RxFIFOThresholdConfig(SPI1,SPI_RxFIFOThreshold_QF);
    //SPI_SSOutputCmd(SPI1,ENABLE);
    GPIO_SetBits(GPIOA, GPIO_Pin_4);
    SPI_Cmd(SPI1,ENABLE);

//    SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Tx, ENABLE);
//
//    /*Init DMA*/
//    DMA_InitStructure.DMA_BufferSize    = sizeof(u8_message);
//    DMA_InitStructure.DMA_DIR           = DMA_DIR_PeripheralDST;
//    DMA_InitStructure.DMA_M2M           = DMA_M2M_Disable;
//    DMA_InitStructure.DMA_MemoryBaseAddr= (uint32_t)(&u8_message);
//    DMA_InitStructure.DMA_MemoryDataSize= DMA_MemoryDataSize_Byte;
//    DMA_InitStructure.DMA_MemoryInc     = DMA_MemoryInc_Enable;
//    DMA_InitStructure.DMA_Mode          = DMA_Mode_Normal;
//    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(SPI1->DR));
//    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
//    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//    DMA_InitStructure.DMA_Priority      = DMA_Priority_Low;
//
//    DMA_ITConfig(DMA1_Channel3,DMA_IT_TC, ENABLE);


//    NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
//    NVIC_SetPriority(DMA1_Channel2_3_IRQn,0);
//    DMA_Init(DMA1_Channel3, &DMA_InitStructure);

}
