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
Chip type           : ATmega1281
Clock frequency     : 18,4320000 MHz
*****************************************************/

#define F_CPU 18432000UL
#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>
#include "cpuinit.h"
#include "utypes.h"
#include "esc.h"

#define wd_reset 100

_ESCvar         ESCvar;
_MBX            MBX[MBXBUFFERS];
_MBXcontrol     MBXcontrol[MBXBUFFERS];
uint8           MBXrun=0;
uint16          SM2_sml,SM3_sml;
_Rbuffer        Rb;
_Wbuffer        Wb;
_eedat EEMEM	eedat;
uint8           TXPDOsize,RXPDOsize;
uint8           wd_ok = 1, wd_cnt = wd_reset;
volatile uint8	digoutput;
volatile uint8	diginput;

/** void ESC_objecthandler(uint16 index, uint8 subindex)
    \brief Object handler, declared from esc.h, as extern function
    \param index
    \param subindex
*/
void ESC_objecthandler(uint16 index, uint8 subindex)
{
  switch (index)
    {
    case 0x7000:
      switch (subindex)
        {
        case 0x01:
          eeprom_write_byte(&eedat.setting8, Wb.setting8);
          break;
        case 0x02:
          eeprom_write_word(&eedat.setting16, Wb.setting16);
          break;
        }
      break;
    }
}

void TXPDO_update(void)
{
// Reference is second item, Rb.state is used internally
  ESC_write(SM3_sma, &Rb.status, TXPDOsize, &ESCvar.ALevent);
}

void RXPDO_update(void)
{
  ESC_read(SM2_sma, &Wb.control, RXPDOsize, &ESCvar.ALevent);
}

void DIG_process(void)
{
  if (Rb.state & APPSTATE_OUTPUT)
    {
      if (ESCvar.ALevent & ESCREG_ALEVENT_SM2) // SM2 trigger ?
        {
          RXPDO_update();
          // dummy output point
          digoutput = Wb.digoutput;
          wd_cnt = wd_reset;
        }
      if (!wd_ok)
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
      wd_ok = 1;
      wd_cnt = wd_reset;
    }
  if (Rb.state)
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
  cpuinit();

// Global enable interrupts
  sei();
  TXPDOsize = sizeTXPDO();
  RXPDOsize = sizeRXPDO();

  _delay_ms(200);
// wait until ESC is started up
  while ((ESCvar.DLstatus & 0x0001) == 0)
    ESC_read(ESCREG_DLSTATUS, &ESCvar.DLstatus, sizeof(ESCvar.DLstatus), &ESCvar.ALevent);

// reset ESC to init state
  ESC_ALstatus(ESCinit);
  ESC_ALerror(ALERR_NONE);
  ESC_stopmbx();
  ESC_stopinput();
  ESC_stopoutput();
// read persistent data from eeprom
  Wb.setting8 = eeprom_read_byte(&eedat.setting8);
  Wb.setting16 = eeprom_read_word(&eedat.setting16);

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
    };
}
