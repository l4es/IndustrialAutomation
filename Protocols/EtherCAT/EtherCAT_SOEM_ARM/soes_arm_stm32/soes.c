/*
 * SOES Simple Open EtherCAT Slave
 *
 * File    : soes.c
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

/****************************************************
Chip type           : STM32F051R8
Clock frequency     : 48 MHz
*****************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "cpuinit.h"
#include "utypes.h"
#include "esc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define wd_reset 1000

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
_ESCvar         ESCvar;
uint8			APPstate;
_MBX            MBX[MBXBUFFERS];
_MBXcontrol     MBXcontrol[MBXBUFFERS];
uint8           MBXrun=0;
uint16          SM2_sml,SM3_sml;
_Rbuffer        Rb;
_Wbuffer        Wb;
_Ebuffer        Eb; //EEprom
uint8           TXPDOsize,RXPDOsize;
uint16          wd_ok = 1, wd_cnt = wd_reset;
volatile uint8	digoutput;
volatile uint8	diginput;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** void ESC_objecthandler(uint16 index, uint8 subindex)
    \brief Object handler, declared from esc.h, as extern function
    \param index
    \param subindex
*/
void ESC_objecthandler(uint16 index, uint8 subindex);
void TXPDO_update(void);
void RXPDO_update(void);
void DIG_process(void);

void _delay_ms(uint16 number_of_ms)
{
    volatile uint16_t cpu_counter = 0;
    uint16_t ms_counter;
    for(ms_counter = 0 ; ms_counter < number_of_ms ; ms_counter++)
    {
        cpu_counter = 48000; //48MHz processor
        while(cpu_counter >0)
            cpu_counter--;
    }
}

void ESC_objecthandler(uint16 index, uint8 subindex)
{
    uint8 dummy8;
    uint16 dummy16;
    switch (index)
    {
    case 0x8000:
        switch (subindex)
        {
        case 0x01:
            dummy8  = Eb.setting8;//Write value to EEPROM; eeprom_write_byte(&eedat.setting8, Wb.setting8);
            break;
        case 0x02:
            dummy16 = Eb.setting16;//Write value to EEPROM; eeprom_write_word(&eedat.setting16, Wb.setting16);
            break;
        }
        break;
    }
}

void TXPDO_update(void)
{
  ESC_write(SM3_sma, &Rb, TXPDOsize, &ESCvar.ALevent);
}

void RXPDO_update(void)
{
  ESC_read(SM2_sma, &Wb, RXPDOsize, &ESCvar.ALevent);
}

void APP_safeoutput(void)
{
  asm("nop");
  //Wb.dout = 0;
  //DOUTPORT = (Wb.dout >> 4) & 0xf0;
}

void DIG_process(void)
{
  if (APPstate & APPSTATE_OUTPUT) //output enabled
    {
        if (ESCvar.ALevent & ESCREG_ALEVENT_SM2) // SM2 trigger ?
        {
          ESCvar.ALevent &= ~ESCREG_ALEVENT_SM2;
            RXPDO_update();
            // dummy output point
            digoutput = Wb.digoutput;
            if(digoutput & 0x01)
                GPIO_SetBits(GPIOC, GPIO_Pin_8);
            else
                GPIO_ResetBits(GPIOC, GPIO_Pin_8);
            wd_cnt = wd_reset;
        }

      if (!wd_cnt)
        {
            ESC_stopoutput();
            // watchdog, invalid outputs
            ESC_ALerror(ALERR_WATCHDOG);
            // goto safe-op with error bit set
            ESC_ALstatus(ESCsafeop | ESCerror);
        }
    }
    else
    {
        //wd_ok = 1;
        wd_cnt = wd_reset;
    }
  if (APPstate) //input or output enabled
    {
        Rb.timestamp = ESCvar.Time;
        //just some dummy data to test
        Rb.counter++;
        Rb.diginput = diginput;
        Rb.analog[0] = 1;
        Rb.analog[1] = 2;
        Rb.analog[2] = 3;
        Rb.analog[3] = 4;
        Rb.analog[4] = 5;
        Rb.analog[5] = 6;
        Rb.analog[6] = 7;
        Rb.analog[7] = 8;

        TXPDO_update();
    }
}


int main(void)
{
    /*!< At this stage the microcontroller clock setting is already configured,
         this is done through SystemInit() function which is called from startup
         file (startup_stm32f0xx.s) before to branch to application main.
         To reconfigure the default setting of SystemInit() function, refer to
         system_stm32f0xx.c file
       */
    cpuinit();
	TXPDOsize = sizeTXPDO();
	RXPDOsize = sizeRXPDO();
    _delay_ms(200);
    /*initialize configuration*/
    Eb.setting16 = 0xABCD;
    Eb.setting8  = 111;
    // wait until ESC is started up
    while ((ESCvar.DLstatus & 0x0001) == 0)
        ESC_read(ESCREG_DLSTATUS, &ESCvar.DLstatus, sizeof(ESCvar.DLstatus), &ESCvar.ALevent);

// reset ESC to init state
    ESC_ALstatus(ESCinit);
    ESC_ALerror(ALERR_NONE);
    ESC_stopmbx();
    ESC_stopinput();
    ESC_stopoutput();

// application run loop
    while (1)
    {
        ESC_read(ESCREG_LOCALTIME, &ESCvar.Time, sizeof(ESCvar.Time), &ESCvar.ALevent);
        ESC_ALevent();
        ESC_state();
        if (ESC_mbxprocess())
        {
            ESC_coeprocess();
            ESC_xoeprocess();
        }
        DIG_process();
    }
}

/** void ESC_objecthandler(uint16 index, uint8 subindex)
    \brief Object handler, declared from esc.h, as extern function
    \param index
    \param subindex
*/


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {
    }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
