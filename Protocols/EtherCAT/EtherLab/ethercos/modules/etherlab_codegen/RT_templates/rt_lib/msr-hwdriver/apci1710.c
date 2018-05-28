/*
  +----------------------------------------------------------------------------+
  | (C) ADDI-DATA GmbH         Dieselstrasse 3       D-77833 Ottersweier       |
  +----------------------------------------------------------------------------+
  | Tel : +49 (0) 7223/9493-0     | email    : info@addi-data.com              |
  | Fax : +49 (0) 7223/9493-92    | Internet : http://www.addi-data.com        |
  +-------------------------------+--------------------------------------------+
  | Project     : APCI-1710       |     Compiler   : GCC                       |
  | Module name : APCI1710.c      |     Version    : 2.96                      |
  +-------------------------------+--------------------------------------------+
  | Author      : E. Stolz        |     Date       : 12.02.2002                |
  +-------------------------------+--------------------------------------------+
  | Description :                                                              |
  |               Includes the following functions for the APCI-1710           |
  |                                                                            |
  |                 - i_APCI1710_InitCounter ()                                |
  |                 - i_APCI1710_ClearCounter ()                               |
  |                 - i_APCI1710_Read16BitCounterValue ()                      |
  |                 - i_APCI1710_Write16BitCounterValue ()                     |       
  |                 - i_APCI1710_Read32BitCounterValue ()                      |
  |                 - i_APCI1710_Write32BitCounterValue ()                     |
  |                 - i_APCI1710_InitDigitalIO ()                              |
  |                 - i_APCI1710_Read1DigitalInput ()                          |
  |                 - i_APCI1710_Read4DigitalInput ()                          |
  |                 - i_APCI1710_SetOutputMemoryOn ()                          |
  |                 - i_APCI1710_SetOutputMemoryOff ()                         |
  |                 - i_APCI1710_Set1DigitalOutput ()                          |
  |                 - i_APCI1710_SetAllDigitalOutput ()                        |
  |                 - i_APCI1710_InitSSI ()                                    |
  |                 - i_APCI1710_Read1SSIValue ()                              |
  |                 - i_APCI1710_ReadAllSSIValue ()                            |
  |                 - i_APCI1710_ReadAllSSIValueRaw ()Hm 2006.09.19            |
  |                 - i_APCI1710_ReadSSI1DigitalInput()  Hm 2008.01.14         |
  |                 - i_APCI1710_ReadSSIAllDigitalInput() Hm 2008.01.14        |
  |                 - i_APCI1710_SetSSIDigitalOutput() Hm 2008.01.14           |
  |                 - v_APCI1710_ReadAllModuleConfiguration ()                 |
  |                 - i_APCI1710_InitPWM ()                                    |
  |                 - i_APCI1710_SetNewPWMTiming ()                            |
  |                 - i_APCI1710_EnablePWM ()                                  |
  |                 - i_APCI1710_DisablePWM ()                                 |
  |                 - i_APCI1710_GetPWMInitialisation ()                       |
  |                 - i_APCI1710_GetPWMStatus ()                               |
  |                                                                            |
  |                 - i_APCI1710_InitChrono ()                                 |
  |                 - i_APCI1710_EnableChrono ()                               |
  |                 - i_APCI1710_DisableChrono ()                              |
  |                 - i_APCI1710_GetChronoProgressStatus  ()                   |
  |                 - i_APCI1710_ReadChronoValue ()                            |
  |                 - i_APCI1710_ConvertChronoValue ()                         |
  |                 - i_APCI1710_SetChronoChlOn ()                             |
  |                 - i_APCI1710_SetChronoChlOff ()                            |
  |                 - i_APCI1710_ReadChronoChlValue ()                         |
  |                 - i_APCI1710_ReadChronoPortValue ()                        |
  +----------------------------------------------------------------------------+
  |                             UPDATES                                        |
  +----------+-----------+--------------------------------------------+--------+
  |   Date   |   Author  |          Description of updates            |Version |
  +----------+-----------+--------------------------------------------+--------+
  | 21.06.04 | J. Krauth | - Append module configuration recognition. |        | 
  |          |           | - Append PWM functions.                    |01.00.01|
  |          |           | - Append i_APCI1710_Read16BitCounterValue  |        |
  |          |           |          i_APCI1710_Write16BitCounterValue |        |
  |          |           |          i_APCI1710_Write32BitCounterValue |        |
  |          |           | - Append some samples functions to test    |        |
  |          |           |   new functions.                           |        |         
  +----------+-----------+--------------------------------------------+--------+
  | 28.06.04 | J. Krauth | - Remove module function.                  |01.00.02|       
  |          |           | - Change structure names.                  |        |       
  +----------+-----------+--------------------------------------------+--------+
  
*/


#ifndef MODULE
   #define MODULE
#endif 

#ifndef __KERNEL__
   #define __KERNEL__
#endif  

/*
  +----------------------------------------------------------------------------+
  |                               Included files                               |
  +----------------------------------------------------------------------------+
*/

#include <apci1710.h>

#include <linux/fs.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/bitops.h>
#include <linux/sched.h>
#include <linux/tqueue.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/mm.h>

#include <msr_rcsinfo.h>

RCS_ID("$Header: /home/hm/projekte/msr_messen_steuern_regeln/linux/kernel_space/rt_lib_non_svn/rt_lib-2.8.3/RCS/apci1710.c,v 1.3 2008/01/14 16:57:04 hm Exp hm $");

str_APCI1710_DriverStruct s_APCI1710_DriverStruct; 

// BEGIN JK 08.09.03 : Implementation from CHRONO functionality
/*
+----------------------------------------------------------------------------+
| Function Name     :  INT      i_APCI1710_InitChrono                        |
|                                       (BYTE     b_BoardHandle,             |
|                                        BYTE     b_ModulNbr,                |
|                                        BYTE     b_ChronoMode,              |
|                                        BYTE     b_PCIInputClock,           |
|                                        BYTE     b_TimingUnit,              | 
|                                        ULONG   ul_TimingInterval,          |
|                                        PULONG pul_RealTimingInterval)      |
+----------------------------------------------------------------------------+
| Task              : Configure the chronometer operating mode (b_ChronoMode)|
|                     from selected module (b_ModulNbr).                     |
|                     The ul_TimingInterval and ul_TimingUnit determine the  |
|                     timing base for the measurement.                       |
|                     The pul_RealTimingInterval return the real timing      |
|                     value. You must calling this function be for you call  |
|                     any other function witch access of the chronometer.    |
|                                                                            |
|                     Witch this functionality from the APCI-1710 you have   |
|                     the possibility to measure the timing witch two event. |
|                                                                            |
|                     The mode 0 and 1 is appropriate for period measurement.|
|                     The mode 2 and 3 is appropriate for frequent           |
|                     measurement.                                           |
|                     The mode 4 to 7 is appropriate for measuring the timing|
|                     between  two event.                                    |
+----------------------------------------------------------------------------+
| Input Parameters  : BYTE    b_BoardHandle    : Handle of board APCI-1710   |
|                     BYTE    b_ModulNbr       : Module number to configure  |
|                                                (0 to 3)                    |
|                     BYTE    b_ChronoMode     : Chronometer action mode     |
|                                                (0 to 7).                   |
|                     BYTE    b_PCIInputClock  : Selection from PCI bus clock|
|                                                - APCI1710_30MHZ :          |
|                                                  The PC have a PCI bus     |
|                                                  clock from 30 MHz         |
|                                                - APCI1710_33MHZ :          |
|                                                  The PC have a PCI bus     |
|                                                  clock from 33 MHz         |
|                                                - APCI1710_40MHZ            |
|                                                  The APCI-1710 have a      |
|                                                  integrated 40Mhz          |
|                                                  quartz.                   |
|                     BYTE    b_TimingUnit    : Base timing unity (0 to 4)   |
|                                                 0 : ns                     |
|                                                 1 : µs                     |
|                                                 2 : ms                     |
|                                                 3 : s                      |
|                                                 4 : mn                     |
|                     ULONG  ul_TimingInterval : Base timing value.          |
+----------------------------------------------------------------------------+
| Output Parameters : PULONG   pul_RealTimingInterval : Real  base timing    |
|                                                       value.               |
+----------------------------------------------------------------------------+
| Return Value      :  0: No error                                           |
|                     -1: The handle parameter of the board is wrong         |
|                     -2: Module selection wrong                             |
|                     -3: The module is not a Chronometer module             |
|                     -4: Chronometer mode selection is wrong                |
|                     -5: The selected PCI input clock is wrong              |
|                     -6: Timing unity selection is wrong                    |
|                     -7: Base timing selection is wrong                     |
|                     -8: You can not used the 40MHz clock selection wich    |
|                         this board                                         |
|                     -9: You can not used the 40MHz clock selection wich    |
|                         this CHRONOS version                               |
+----------------------------------------------------------------------------+
*/

INT   i_APCI1710_InitChrono (BYTE     b_BoardHandle,
			     BYTE     b_ModuleNumber,
			     BYTE     b_ChronoMode,
			     BYTE     b_PCIInputClock,
			     BYTE     b_TimingUnit,
			     ULONG   ul_TimingInterval,
			     PULONG pul_RealTimingInterval)
	{
	INT    i_ReturnValue = 0;
	ULONG ul_TimerValue = 0UL;
	double d_RealTimingInterval = 0.0;
	DWORD dw_ModeArray [8] = {0x01, 0x05, 0x00, 0x04, 0x02, 0x0E, 0x0A, 0x06};

	/*******************************************************/
        /* Gets the pointer to the board information structure */
        /*******************************************************/
        str_APCI1710_BoardInformation *BoardInformation = NULL;

        BoardInformation = &(s_APCI1710_DriverStruct.
                             s_BoardInformation[b_BoardHandle]);

        if (b_BoardHandle >= s_APCI1710_DriverStruct.b_NumberOfBoard)
	   {
	   return (-1);
	   }

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModuleNumber < 4)
	   {
	   /***********************/
	   /* Test if chronometer */
	   /***********************/

            if (BoardInformation -> dw_MolduleConfiguration [b_ModuleNumber] == APCI1710_CHRONOMETER)
	      {
	      /*****************************/
	      /* Test the chronometer mode */
	      /*****************************/

	      if (b_ChronoMode >= 0 && b_ChronoMode <= 7)
		 {
		 /**************************/
		 /* Test the PCI bus clock */
		 /**************************/

		 if ((b_PCIInputClock == APCI1710_30MHZ) ||
		     (b_PCIInputClock == APCI1710_33MHZ) ||
		     (b_PCIInputClock == APCI1710_40MHZ))
		    {
		    /*************************/
		    /* Test the timing unity */
		    /*************************/

		    if ((b_TimingUnit >= 0) && (b_TimingUnit <= 4))
		       {
		       /**********************************/
		       /* Test the base timing selection */
		       /**********************************/

		       if (((b_PCIInputClock == APCI1710_30MHZ) && (b_TimingUnit == 0) && (ul_TimingInterval >= 66) && (ul_TimingInterval <= 0xFFFFFFFFUL)) ||
			   ((b_PCIInputClock == APCI1710_30MHZ) && (b_TimingUnit == 1) && (ul_TimingInterval >= 1)  && (ul_TimingInterval <= 143165576UL))  ||
			   ((b_PCIInputClock == APCI1710_30MHZ) && (b_TimingUnit == 2) && (ul_TimingInterval >= 1)  && (ul_TimingInterval <= 143165UL))     ||
			   ((b_PCIInputClock == APCI1710_30MHZ) && (b_TimingUnit == 3) && (ul_TimingInterval >= 1)  && (ul_TimingInterval <= 143UL))        ||
			   ((b_PCIInputClock == APCI1710_30MHZ) && (b_TimingUnit == 4) && (ul_TimingInterval >= 1)  && (ul_TimingInterval <= 2UL))          ||
			   ((b_PCIInputClock == APCI1710_33MHZ) && (b_TimingUnit == 0) && (ul_TimingInterval >= 60) && (ul_TimingInterval <= 0xFFFFFFFFUL)) ||
			   ((b_PCIInputClock == APCI1710_33MHZ) && (b_TimingUnit == 1) && (ul_TimingInterval >= 1)  && (ul_TimingInterval <= 130150240UL))  ||
			   ((b_PCIInputClock == APCI1710_33MHZ) && (b_TimingUnit == 2) && (ul_TimingInterval >= 1)  && (ul_TimingInterval <= 130150UL))     ||
			   ((b_PCIInputClock == APCI1710_33MHZ) && (b_TimingUnit == 3) && (ul_TimingInterval >= 1)  && (ul_TimingInterval <= 130UL))        ||
			   ((b_PCIInputClock == APCI1710_33MHZ) && (b_TimingUnit == 4) && (ul_TimingInterval >= 1)  && (ul_TimingInterval <= 2UL))          ||
			   ((b_PCIInputClock == APCI1710_40MHZ) && (b_TimingUnit == 0) && (ul_TimingInterval >= 50) && (ul_TimingInterval <= 0xFFFFFFFFUL)) ||
			   ((b_PCIInputClock == APCI1710_40MHZ) && (b_TimingUnit == 1) && (ul_TimingInterval >= 1)  && (ul_TimingInterval <= 107374182UL))  ||
			   ((b_PCIInputClock == APCI1710_40MHZ) && (b_TimingUnit == 2) && (ul_TimingInterval >= 1)  && (ul_TimingInterval <= 107374UL))     ||
			   ((b_PCIInputClock == APCI1710_40MHZ) && (b_TimingUnit == 3) && (ul_TimingInterval >= 1)  && (ul_TimingInterval <= 107UL))        ||
			   ((b_PCIInputClock == APCI1710_40MHZ) && (b_TimingUnit == 4) && (ul_TimingInterval >= 1)  && (ul_TimingInterval <= 1UL)))
			  {
			  /**************************/
			  /* Test the board version */
			  /**************************/
// BEGIN JK
/*
			  if ((b_PCIInputClock == APCI1710_40MHZ) && (s_APCI1710Variable->
								      s_Board [b_BoardHandle].
								      s_BoardInfos.
								      b_BoardVersion > 0) ||
			      (b_PCIInputClock != APCI1710_40MHZ))
*/
			  if (1)
// END JK
			     {
			     /************************/
			     /* Test the TOR version */
			     /************************/
// BEGIN JK 08.09.03
/*
			     if ((b_PCIInputClock == APCI1710_40MHZ) && ((ps_APCI1710Variable->
									  s_Board [b_BoardHandle].
									  s_BoardInfos.
									  dw_MolduleConfiguration [b_ModulNbr] & 0xFFFF) >= 0x3131) ||
				 (b_PCIInputClock != APCI1710_40MHZ))
*/
			     if (1)

// END JK 08.09.03
				{
				/****************************************/
				/* Calculate the timer 0 division fator */
				/****************************************/

				switch (b_TimingUnit)
				   {
				   /******/
				   /* ns */
				   /******/

				   case 0:
					   /******************/
					   /* Timer 0 factor */
					   /******************/

					   ul_TimerValue = (ULONG) (ul_TimingInterval * (0.001 * b_PCIInputClock));

					   /*******************/
					   /* Round the value */
					   /*******************/

					   if ((double) ((double) ul_TimingInterval * (0.001 * (double) b_PCIInputClock)) >= ((double) ((double) ul_TimerValue + 0.5)))
					      {
					      ul_TimerValue = ul_TimerValue + 1;
					      }

					   /*****************************/
					   /* Calculate the real timing */
					   /*****************************/

					   *pul_RealTimingInterval = (ULONG) (ul_TimerValue / (0.001 * (double) b_PCIInputClock));
					   d_RealTimingInterval = (double) ul_TimerValue / (0.001 * (double) b_PCIInputClock);

					   if ((double) ((double) ul_TimerValue / (0.001 * (double) b_PCIInputClock)) >= (double) ((double) *pul_RealTimingInterval + 0.5))
					      {
					      *pul_RealTimingInterval = *pul_RealTimingInterval + 1;
					      }

					   ul_TimingInterval = ul_TimingInterval - 1;
					   ul_TimerValue     = ul_TimerValue - 2;
					   if (b_PCIInputClock != APCI1710_40MHZ)
					      {
					      ul_TimerValue     = (ULONG) ((double) (ul_TimerValue) * 0.99392);
					      }
					break;

				   /******/
				   /* æs */
				   /******/

				   case 1:
					   /******************/
					   /* Timer 0 factor */
					   /******************/

					   ul_TimerValue = (ULONG) (ul_TimingInterval * (1.0 * b_PCIInputClock));

					   /*******************/
					   /* Round the value */
					   /*******************/

					   if ((double) ((double) ul_TimingInterval * (1.0 * (double) b_PCIInputClock)) >= ((double) ((double) ul_TimerValue + 0.5)))
					      {
					      ul_TimerValue = ul_TimerValue + 1;
					      }

					   /*****************************/
					   /* Calculate the real timing */
					   /*****************************/

					   *pul_RealTimingInterval = (ULONG) (ul_TimerValue / (1.0 * (double) b_PCIInputClock));
					   d_RealTimingInterval = (double) ul_TimerValue / ((double) 1.0 * (double) b_PCIInputClock);

					   if ((double) ((double) ul_TimerValue / (1.0 * (double) b_PCIInputClock)) >= (double) ((double) *pul_RealTimingInterval + 0.5))
					      {
					      *pul_RealTimingInterval = *pul_RealTimingInterval + 1;
					      }

					   ul_TimingInterval = ul_TimingInterval - 1;
					   ul_TimerValue     = ul_TimerValue - 2;
					   if (b_PCIInputClock != APCI1710_40MHZ)
					      {
					      ul_TimerValue     = (ULONG) ((double) (ul_TimerValue) * 0.99392);
					      }
					break;

				   /******/
				   /* ms */
				   /******/

				   case 2:
					   /******************/
					   /* Timer 0 factor */
					   /******************/

					   ul_TimerValue = ul_TimingInterval * (1000 * b_PCIInputClock);

					   /*******************/
					   /* Round the value */
					   /*******************/

					   if ((double) ((double) ul_TimingInterval * (1000.0 * (double) b_PCIInputClock)) >= ((double) ((double) ul_TimerValue + 0.5)))
					      {
					      ul_TimerValue = ul_TimerValue + 1;
					      }

					   /*****************************/
					   /* Calculate the real timing */
					   /*****************************/

					   *pul_RealTimingInterval = (ULONG) (ul_TimerValue / (1000.0 * (double) b_PCIInputClock));
					   d_RealTimingInterval = (double) ul_TimerValue / (1000.0 * (double) b_PCIInputClock);

					   if ((double) ((double) ul_TimerValue / (1000.0 * (double) b_PCIInputClock)) >= (double) ((double) *pul_RealTimingInterval + 0.5))
					      {
					      *pul_RealTimingInterval = *pul_RealTimingInterval + 1;
					      }

					   ul_TimingInterval = ul_TimingInterval - 1;
					   ul_TimerValue     = ul_TimerValue - 2;
					   if (b_PCIInputClock != APCI1710_40MHZ)
					      {
					      ul_TimerValue     = (ULONG) ((double) (ul_TimerValue) * 0.99392);
					      }
					break;

				   /*****/
				   /* s */
				   /*****/

				   case 3:
					   /******************/
					   /* Timer 0 factor */
					   /******************/

					   ul_TimerValue = (ULONG) (ul_TimingInterval * (1000000.0 * b_PCIInputClock));

					   /*******************/
					   /* Round the value */
					   /*******************/

					   if ((double) ((double) ul_TimingInterval * (1000000.0 * (double) b_PCIInputClock)) >= ((double) ((double) ul_TimerValue + 0.5)))
					      {
					      ul_TimerValue = ul_TimerValue + 1;
					      }

					   /*****************************/
					   /* Calculate the real timing */
					   /*****************************/

					   *pul_RealTimingInterval = (ULONG) (ul_TimerValue / (1000000.0 * (double) b_PCIInputClock));
					   d_RealTimingInterval = (double) ul_TimerValue / (1000000.0 * (double) b_PCIInputClock);

					   if ((double) ((double) ul_TimerValue / (1000000.0 * (double) b_PCIInputClock)) >= (double) ((double) *pul_RealTimingInterval + 0.5))
					      {
					      *pul_RealTimingInterval = *pul_RealTimingInterval + 1;
					      }

					   ul_TimingInterval = ul_TimingInterval - 1;
					   ul_TimerValue     = ul_TimerValue - 2;
					   if (b_PCIInputClock != APCI1710_40MHZ)
					      {
					      ul_TimerValue     = (ULONG) ((double) (ul_TimerValue) * 0.99392);
					      }
					break;

				   /******/
				   /* mn */
				   /******/

				   case 4:
					   /******************/
					   /* Timer 0 factor */
					   /******************/

					   ul_TimerValue = (ULONG) ((ul_TimingInterval * 60) * (1000000.0 * b_PCIInputClock));

					   /*******************/
					   /* Round the value */
					   /*******************/

					   if ((double) ((double) (ul_TimingInterval * 60.0) * (1000000.0 * (double) b_PCIInputClock)) >= ((double) ((double) ul_TimerValue + 0.5)))
					      {
					      ul_TimerValue = ul_TimerValue + 1;
					      }

					   /*****************************/
					   /* Calculate the real timing */
					   /*****************************/

					   *pul_RealTimingInterval = (ULONG) (ul_TimerValue / (1000000.0 * (double) b_PCIInputClock)) / 60;
					   d_RealTimingInterval = ((double) ul_TimerValue / (0.001 * (double) b_PCIInputClock)) / 60.0;

					   if ((double) (((double) ul_TimerValue / (1000000.0 * (double) b_PCIInputClock)) / 60.0) >= (double) ((double) *pul_RealTimingInterval + 0.5))
					      {
					      *pul_RealTimingInterval = *pul_RealTimingInterval + 1;
					      }

					   ul_TimingInterval = ul_TimingInterval - 1;
					   ul_TimerValue     = ul_TimerValue - 2;
					   if (b_PCIInputClock != APCI1710_40MHZ)
					      {
					      ul_TimerValue     = (ULONG) ((double) (ul_TimerValue) * 0.99392);
					      }
					break;
				   }

				/****************************/
				/* Save the PCI input clock */
				/****************************/

				s_APCI1710_DriverStruct.
		                s_BoardInformation[b_BoardHandle].
   	                        s_ModuleInformation[b_ModuleNumber].
				s_ChronoModuleInformation.
				b_PCIInputClock = b_PCIInputClock;

				/*************************/
				/* Save the timing unity */
				/*************************/

				s_APCI1710_DriverStruct.
		                s_BoardInformation[b_BoardHandle].
   	                        s_ModuleInformation[b_ModuleNumber].
				s_ChronoModuleInformation.
				b_TimingUnit = b_TimingUnit;

				/************************/
				/* Save the base timing */
				/************************/

				s_APCI1710_DriverStruct.
		                s_BoardInformation[b_BoardHandle].
   	                        s_ModuleInformation[b_ModuleNumber].
				s_ChronoModuleInformation.
				d_TimingInterval = d_RealTimingInterval;

				/****************************/
				/* Set the chronometer mode */
				/****************************/
				
				s_APCI1710_DriverStruct.
		                s_BoardInformation[b_BoardHandle].
   	                        s_ModuleInformation[b_ModuleNumber].
				s_ChronoModuleInformation.
				dw_ConfigReg = dw_ModeArray [b_ChronoMode];

				/***********************/
				/* Test if 40 MHz used */
				/***********************/

				if (b_PCIInputClock == APCI1710_40MHZ)
				   {
				   s_APCI1710_DriverStruct.
		                   s_BoardInformation[b_BoardHandle].
   	                           s_ModuleInformation[b_ModuleNumber].
				   s_ChronoModuleInformation.
				   dw_ConfigReg = s_APCI1710_DriverStruct.
		                                  s_BoardInformation[b_BoardHandle].
                  	                          s_ModuleInformation[b_ModuleNumber].
				                  s_ChronoModuleInformation.
				                  dw_ConfigReg | 0x80;
				   }

		                 outl (s_APCI1710_DriverStruct.
		                       s_BoardInformation[b_BoardHandle].
          	                       s_ModuleInformation[b_ModuleNumber].
				       s_ChronoModuleInformation.
				       dw_ConfigReg,
		                       BoardInformation ->
				       s_BaseInformation.
	 		               ui_BaseAddress[2]
				       + 4 + (64 * b_ModuleNumber));

				/***********************/
				/* Write timer 0 value */
				/***********************/

				 outl (ul_TimerValue,
		                       BoardInformation ->
				       s_BaseInformation.
	 		               ui_BaseAddress[2] + (64 * b_ModuleNumber));

				/*********************/
				/* Chronometer init. */
				/*********************/

				s_APCI1710_DriverStruct.
		                s_BoardInformation[b_BoardHandle].
          	                s_ModuleInformation[b_ModuleNumber].
				s_ChronoModuleInformation.
                                b_ChronoInit = 1;
				}
			     else
				{
				/***********************************************/
				/* TOR version error for 40MHz clock selection */
				/***********************************************/

				i_ReturnValue = -9;
				}
			     }
			  else
			     {
			     /**************************************************************/
			     /* You can not used the 40MHz clock selection wich this board */
			     /**************************************************************/

			     i_ReturnValue = -8;
			     }
			  }
		       else
			  {
			  /**********************************/
			  /* Base timing selection is wrong */
			  /**********************************/

			  i_ReturnValue = -7;
			  }
		       } // if ((b_TimingUnit >= 0) && (b_TimingUnit <= 4))
		    else
		       {
		       /***********************************/
		       /* Timing unity selection is wrong */
		       /***********************************/

		       i_ReturnValue = -6;
		       } // if ((b_TimingUnit >= 0) && (b_TimingUnit <= 4))
		    } // if ((b_PCIInputClock == APCI1710_30MHZ) || (b_PCIInputClock == APCI1710_33MHZ))
		 else
		    {
		    /*****************************************/
		    /* The selected PCI input clock is wrong */
		    /*****************************************/

		    i_ReturnValue = -5;
		    } // if ((b_PCIInputClock == APCI1710_30MHZ) || (b_PCIInputClock == APCI1710_33MHZ))
		 } // if (b_ChronoMode >= 0 && b_ChronoMode <= 7)
	      else
		 {
		 /***************************************/
		 /* Chronometer mode selection is wrong */
		 /***************************************/

		 i_ReturnValue = -4;
		 } // if (b_ChronoMode >= 0 && b_ChronoMode <= 7)
	      }
	   else
	      {
	      /******************************************/
	      /* The module is not a Chronometer module */
	      /******************************************/

	      i_ReturnValue = -3;
	      }
	   }
	else
	   {
	   /***********************/
	   /* Module number error */
	   /***********************/

	   i_ReturnValue = -2;
	   }

	return (i_ReturnValue);
	}


/*
+----------------------------------------------------------------------------+
| Function Name     :  INT  i_APCI1710_EnableChrono                          |
|                                               (BYTE b_BoardHandle,         |
|                                                BYTE b_ModulNbr,            |
|                                                BYTE b_CycleMode,           |
|                                                BYTE b_InterruptEnable)     |
+----------------------------------------------------------------------------+
| Task              : Enable the chronometer from selected module            |
|                     (b_ModulNbr). You must calling the                     |
|                     "i_APCI1710_InitChrono" function be for you call this  |
|                     function.                                              |
|                     If you enable the chronometer interrupt, the           |
|                     chronometer generate a interrupt after the stop signal.|
|                     See function "i_APCI1710_SetBoardIntRoutineX" and the  |
|                     Interrupt mask description chapter from this manual.   |
|                     The b_CycleMode parameter determine if you will        |
|                     measured a single or more cycle.                       |
+----------------------------------------------------------------------------+
| Input Parameters  : BYTE  b_BoardHandle  : Handle of board APCI-1710       |
|                     BYTE  b_ModulNbr     : Selected module number (0 to 3) |
|                     BYTE  b_CycleMode    : Selected the chronometer        |
|                                            acquisition mode                |
|                     BYTE  b_InterruptEnable : Enable or disable the        |
|                                               chronometer interrupt.       |
|                                               APCI1710_ENABLE:             |
|                                               Enable the chronometer       |
|                                               interrupt                    |
|                                               APCI1710_DISABLE:            |
|                                               Disable the chronometer      |
|                                               interrupt                    |
+----------------------------------------------------------------------------+
| Output Parameters : -                                                      |
+----------------------------------------------------------------------------+
| Return Value      :  0: No error                                           |
|                     -1: The handle parameter of the board is wrong         |
|                     -2: Module selection wrong                             |
|                     -3: The module is not a Chronometer module             |
|                     -4: Chronometer not initialised see function           |
|                         "i_APCI1710_InitChrono"                            |
|                     -5: Chronometer acquisition mode cycle is wrong        |
|                     -6: Interrupt parameter is wrong                       |
|                     -7: Interrupt function not initialised.                |
|                         See function "i_APCI1710_SetBoardIntRoutineX"      |
+----------------------------------------------------------------------------+
*/

INT   i_APCI1710_EnableChrono (BYTE b_BoardHandle,
			       BYTE b_ModuleNumber,
			       BYTE b_CycleMode,
			       BYTE b_InterruptEnable)
	{
	INT i_ReturnValue = 0;

        /*******************************************************/
        /* Gets the pointer to the board information structure */
        /*******************************************************/
        str_APCI1710_BoardInformation *BoardInformation = NULL;

        BoardInformation = &(s_APCI1710_DriverStruct.
                             s_BoardInformation[b_BoardHandle]);

	/***************************/
	/* Test if board handle OK */
	/***************************/

        if (b_BoardHandle >= s_APCI1710_DriverStruct.b_NumberOfBoard)
	   {
	   return (-1);
	   }

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModuleNumber < 4)
	   {
	   /***********************/
	   /* Test if chronometer */
	   /***********************/

            if (BoardInformation -> dw_MolduleConfiguration [b_ModuleNumber] == APCI1710_CHRONOMETER)
	      {
	      /***********************************/
	      /* Test if chronometer initialised */
	      /***********************************/

	      if (s_APCI1710_DriverStruct.
		  s_BoardInformation[b_BoardHandle].
          	  s_ModuleInformation[b_ModuleNumber].
		  s_ChronoModuleInformation.
                  b_ChronoInit == 1)
		 {
		 /*********************************/
		 /* Test the cycle mode parameter */
		 /*********************************/

		 if ((b_CycleMode == APCI1710_SINGLE) || (b_CycleMode == APCI1710_CONTINUOUS))
		    {
		    /***************************/
		    /* Test the interrupt flag */
		    /***************************/

		    if ((b_InterruptEnable == APCI1710_ENABLE) || (b_InterruptEnable == APCI1710_DISABLE))
		       {
		       /**************************/
		       /* Test if interrupt used */
		       /**************************/

		       if (b_InterruptEnable == APCI1710_ENABLE)
			  {
			  /******************************************/
			  /* Test if interrupt function initialised */
			  /******************************************/

			  if (s_APCI1710_DriverStruct.
			      s_BoardInformation[b_BoardHandle].
		 	      b_InterruptInitialized == APCI1710_DISABLE)
			     {
			     /**************************************/
			     /* Interrupt function not initialised */
			     /**************************************/

			     i_ReturnValue = -7;
			     }
			  }

		       /***********************/
		       /* Test if error occur */
		       /***********************/

		       if (!i_ReturnValue)
			  {
			  /***************************/
			  /* Save the interrupt flag */
			  /***************************/
			  
			  s_APCI1710_DriverStruct.
		          s_BoardInformation[b_BoardHandle].
                          s_ModuleInformation [b_ModuleNumber].
			  s_ChronoModuleInformation.
			  b_InterruptMask = b_InterruptEnable;
			  
			  /***********************/
			  /* Save the cycle mode */
			  /***********************/

			  s_APCI1710_DriverStruct.
		          s_BoardInformation[b_BoardHandle].
                          s_ModuleInformation [b_ModuleNumber].
			  s_ChronoModuleInformation.
			  b_CycleMode = b_CycleMode;

			  s_APCI1710_DriverStruct.
		          s_BoardInformation[b_BoardHandle].
                          s_ModuleInformation [b_ModuleNumber].
			  s_ChronoModuleInformation.
			  dw_ConfigReg = (s_APCI1710_DriverStruct.
		                          s_BoardInformation[b_BoardHandle].
                                          s_ModuleInformation [b_ModuleNumber].
			                  s_ChronoModuleInformation.dw_ConfigReg & 0x8F)           |
					  ((1 & b_InterruptEnable) << 5) |
					  ((1 & b_CycleMode) << 6)       |
					  0x10 ;

			  /*****************************/
			  /* Test if interrupt enabled */
			  /*****************************/

			  if (b_InterruptEnable == APCI1710_ENABLE)
			     {
			     /****************************/
			     /* Clear the interrupt flag */
			     /****************************/

			     outl (s_APCI1710_DriverStruct.
                                   s_BoardInformation[b_BoardHandle].
                                   s_ModuleInformation [b_ModuleNumber].
			           s_ChronoModuleInformation.
			           dw_ConfigReg,
                                   BoardInformation ->
				   s_BaseInformation.
	 		           ui_BaseAddress[2] + 32 + (64 * b_ModuleNumber));
			     }

			  /***********************************/
			  /* Enable or disable the interrupt */
			  /* Enable the chronometer          */
			  /***********************************/

			     outl (s_APCI1710_DriverStruct.
                                   s_BoardInformation[b_BoardHandle].
                                   s_ModuleInformation [b_ModuleNumber].
			           s_ChronoModuleInformation.
			           dw_ConfigReg,
                                   BoardInformation ->
				   s_BaseInformation.
	 		           ui_BaseAddress[2] + 16 + (64 * b_ModuleNumber));

			  /*************************/
			  /* Clear status register */
			  /*************************/
			     
			     outl (0,
                                   BoardInformation ->
				   s_BaseInformation.
	 		           ui_BaseAddress[2] + 36 + (64 * b_ModuleNumber));
			  }
		       } // if ((b_InterruptEnable == APCI1710_ENABLE) || (b_InterruptEnable == APCI1710_DISABLE))
		    else
		       {
		       /********************************/
		       /* Interrupt parameter is wrong */
		       /********************************/

		       i_ReturnValue = -6;
		       } // if ((b_InterruptEnable == APCI1710_ENABLE) || (b_InterruptEnable == APCI1710_DISABLE))
		    } // if ((b_CycleMode == APCI1710_SINGLE) || (b_CycleMode == APCI1710_CONTINUOUS))
		 else
		    {
		    /***********************************************/
		    /* Chronometer acquisition mode cycle is wrong */
		    /***********************************************/

		    i_ReturnValue = -5;
		    } // if ((b_CycleMode == APCI1710_SINGLE) || (b_CycleMode == APCI1710_CONTINUOUS))
		 }
	      else
		 {
		 /*******************************/
		 /* Chronometer not initialised */
		 /*******************************/

		 i_ReturnValue = -4;
		 }
	      }
	   else
	      {
	      /******************************************/
	      /* The module is not a Chronometer module */
	      /******************************************/

	      i_ReturnValue = -3;
	      }
	   }
	else
	   {
	   /***********************/
	   /* Module number error */
	   /***********************/

	   i_ReturnValue = -2;
	   }

	return (i_ReturnValue);
	}



/*
+----------------------------------------------------------------------------+
| Function Name     :  INT  i_APCI1710_DisableChrono                         |
|                                               (BYTE  b_BoardHandle,        |
|                                                BYTE  b_ModulNbr)           |
+----------------------------------------------------------------------------+
| Task              : Disable the chronometer from selected module           |
|                     (b_ModulNbr). If you disable the chronometer after a   |
|                     start signal occur and you restart the chronometer     |
|                     witch the " i_APCI1710_EnableChrono" function, if no   |
|                     stop signal occur this start signal is ignored.        |
+----------------------------------------------------------------------------+
| Input Parameters  : BYTE  b_BoardHandle  : Handle of board APCI-1710       |
|                     BYTE  b_ModulNbr     : Selected module number (0 to 3) |
+----------------------------------------------------------------------------+
| Output Parameters : -                                                      |
+----------------------------------------------------------------------------+
| Return Value      :  0: No error                                           |
|                     -1: The handle parameter of the board is wrong         |
|                     -2: Module selection wrong                             |
|                     -3: The module is not a Chronometer module             |
|                     -4: Chronometer not initialised see function           |
|                         "i_APCI1710_InitChrono"                            |
+----------------------------------------------------------------------------+
*/

INT   i_APCI1710_DisableChrono (BYTE b_BoardHandle,
                                BYTE b_ModuleNumber)
	{
	INT i_ReturnValue = 0;

        /*******************************************************/
        /* Gets the pointer to the board information structure */
        /*******************************************************/
        str_APCI1710_BoardInformation *BoardInformation = NULL;

        BoardInformation = &(s_APCI1710_DriverStruct.
                             s_BoardInformation[b_BoardHandle]);
	/***************************/
	/* Test if board handle OK */
	/***************************/
        
	if (b_BoardHandle >= s_APCI1710_DriverStruct.b_NumberOfBoard)
	   {
	   return (-1);
	   }

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModuleNumber < 4)
	   {
	   /***********************/
	   /* Test if chronometer */
	   /***********************/

            if (BoardInformation -> dw_MolduleConfiguration [b_ModuleNumber] == APCI1710_CHRONOMETER)
	      {
	      /***********************************/
	      /* Test if chronometer initialised */
	      /***********************************/

	      if (s_APCI1710_DriverStruct.
		  s_BoardInformation[b_BoardHandle].
		  s_ModuleInformation[b_ModuleNumber].
		  s_ChronoModuleInformation.
                  b_ChronoInit == 1)
		 {
		 s_APCI1710_DriverStruct.
		 s_BoardInformation[b_BoardHandle].
		 s_ModuleInformation[b_ModuleNumber].
		 s_ChronoModuleInformation.
		 b_InterruptMask = 0;

		 s_APCI1710_DriverStruct.
		 s_BoardInformation[b_BoardHandle].
		 s_ModuleInformation[b_ModuleNumber].
		 s_ChronoModuleInformation.
		 dw_ConfigReg = s_APCI1710_DriverStruct.
		 		s_BoardInformation[b_BoardHandle].
		 		s_ModuleInformation[b_ModuleNumber].
		 		s_ChronoModuleInformation.
				dw_ConfigReg & 0xAF;

		 /***************************/
		 /* Disable the interrupt   */
		 /* Disable the chronometer */
		 /***************************/

		outl (s_APCI1710_DriverStruct.
                      s_BoardInformation[b_BoardHandle].
                      s_ModuleInformation [b_ModuleNumber].
		      s_ChronoModuleInformation.
		      dw_ConfigReg,
                      BoardInformation ->
		      s_BaseInformation.
		      ui_BaseAddress[2] + 16 + (64 * b_ModuleNumber));

		 if (s_APCI1710_DriverStruct.
		     s_BoardInformation[b_BoardHandle].
		     s_ModuleInformation[b_ModuleNumber].
		     s_ChronoModuleInformation.
		     b_CycleMode == APCI1710_CONTINUOUS)
		    {
		    /*************************/
		    /* Clear status register */
		    /*************************/

		outl (0,
                      BoardInformation ->
		      s_BaseInformation.
		      ui_BaseAddress[2] + 36 + (64 * b_ModuleNumber));
		    }
		 }
	      else
		 {
		 /*******************************/
		 /* Chronometer not initialised */
		 /*******************************/

		 i_ReturnValue = -4;
		 }
	      }
	   else
	      {
	      /******************************************/
	      /* The module is not a Chronometer module */
	      /******************************************/

	      i_ReturnValue = -3;
	      }
	   }
	else
	   {
	   /***********************/
	   /* Module number error */
	   /***********************/

	   i_ReturnValue = -2;
	   }

	return (i_ReturnValue);
	}



/*
+----------------------------------------------------------------------------+
| Function Name     :  INT  i_APCI1710_GetChronoProgressStatus               |
|                               (BYTE    b_BoardHandle,                      |
|                                BYTE    b_ModulNbr,                         |
|                                PBYTE  pb_ChronoStatus)                     |
+----------------------------------------------------------------------------+
| Task              : Return the chronometer status (pb_ChronoStatus) from   |
|                     selected chronometer module (b_ModulNbr).              |
+----------------------------------------------------------------------------+
| Input Parameters  : BYTE  b_BoardHandle  : Handle of board APCI-1710       |
|                     BYTE  b_ModulNbr     : Selected module number (0 to 3) |
+----------------------------------------------------------------------------+
| Output Parameters : PULONG   pb_ChronoStatus : Return the chronometer      |
|                                                status.                     |
|                                                0 : Measurement not started.|
|                                                    No start signal occur.  |
|                                                1 : Measurement started.    |
|                                                    A start signal occur.   |
|                                                2 : Measurement stopped.    |
|                                                    A stop signal occur.    |
|                                                    The measurement is      |
|                                                    terminate.              |
|                                                3: A overflow occur. You    |
|                                                   must change the base     |
|                                                   timing witch the         |
|                                                   function                 |
|                                                   "i_APCI1710_InitChrono"  |
+----------------------------------------------------------------------------+
| Return Value      :  0: No error                                           |
|                     -1: The handle parameter of the board is wrong         |
|                     -2: Module selection wrong                             |
|                     -3: The module is not a Chronometer module             |
|                     -4: Chronometer not initialised see function           |
|                         "i_APCI1710_InitChrono"                            |
+----------------------------------------------------------------------------+
*/

INT   i_APCI1710_GetChronoProgressStatus (BYTE    b_BoardHandle,
					  BYTE    b_ModuleNumber,
					  PBYTE  pb_ChronoStatus)
	{
 	INT i_ReturnValue = 0;
	DWORD dw_Status;

        /*******************************************************/
        /* Gets the pointer to the board information structure */
        /*******************************************************/
        str_APCI1710_BoardInformation *BoardInformation = NULL;

        BoardInformation = &(s_APCI1710_DriverStruct.
                             s_BoardInformation[b_BoardHandle]);

	/***************************/
	/* Test if board handle OK */
	/***************************/
        
	if (b_BoardHandle >= s_APCI1710_DriverStruct.b_NumberOfBoard)
	   {
	   return (-1);
	   }

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModuleNumber < 4)
	   {
	   /***********************/
	   /* Test if chronometer */
	   /***********************/

            if (BoardInformation -> dw_MolduleConfiguration [b_ModuleNumber] == APCI1710_CHRONOMETER)
	      {
	      /***********************************/
	      /* Test if chronometer initialised */
	      /***********************************/

	      if (s_APCI1710_DriverStruct.
		  s_BoardInformation[b_BoardHandle].
		  s_ModuleInformation[b_ModuleNumber].
		  s_ChronoModuleInformation.
                  b_ChronoInit == 1)
		 {
	         dw_Status = inl (BoardInformation -> s_BaseInformation.
		                  ui_BaseAddress[2] + 8 +
		   		  (64 * b_ModuleNumber));

		 /********************/
		 /* Test if overflow */
		 /********************/

		 if ((dw_Status & 8) == 8)
		    {
		    /******************/
		    /* Overflow occur */
		    /******************/

		    *pb_ChronoStatus = 3;
		    } // if ((dw_Status & 8) == 8)
		 else
		    {
		    /*******************************/
		    /* Test if measurement stopped */
		    /*******************************/

		    if ((dw_Status & 2) == 2)
		       {
		       /***********************/
		       /* A stop signal occur */
		       /***********************/

		       *pb_ChronoStatus = 2;
		       } // if ((dw_Status & 2) == 2)
		    else
		       {
		       /*******************************/
		       /* Test if measurement started */
		       /*******************************/

		       if ((dw_Status & 1) == 1)
			  {
			  /************************/
			  /* A start signal occur */
			  /************************/

			  *pb_ChronoStatus = 1;
			  } // if ((dw_Status & 1) == 1)
		       else
			  {
			  /***************************/
			  /* Measurement not started */
			  /***************************/

			  *pb_ChronoStatus = 0;
			  } // if ((dw_Status & 1) == 1)
		       } // if ((dw_Status & 2) == 2)
		    } // if ((dw_Status & 8) == 8)
		 }
	      else
		 {
		 /*******************************/
		 /* Chronometer not initialised */
		 /*******************************/

		 i_ReturnValue = -4;
		 }
	      }
	   else
	      {
	      /******************************************/
	      /* The module is not a Chronometer module */
	      /******************************************/

	      i_ReturnValue = -3;
	      }
	   }
	else
	   {
	   /***********************/
	   /* Module number error */
	   /***********************/

	   i_ReturnValue = -2;
	   }

	return (i_ReturnValue);
	}
	
	
/*
+----------------------------------------------------------------------------+
| Function Name     :  INT  i_APCI1710_ReadChronoValue                       |
|                               (BYTE     b_BoardHandle,                     |
|                                BYTE     b_ModulNbr,                        |
|                                UINT    ui_TimeOut,                         |
|                                PBYTE   pb_ChronoStatus,                    |
|                                PULONG pul_ChronoValue)                     |
+----------------------------------------------------------------------------+
| Task              : Return the chronometer status (pb_ChronoStatus) and the|
|                     timing value (pul_ChronoValue) after a stop signal     |
|                     occur from selected chronometer module (b_ModulNbr).   |
|                     This function are only avaible if you have disabled    |
|                     the interrupt functionality. See function              |
|                     "i_APCI1710_EnableChrono" and the Interrupt mask       |
|                     description chapter.                                   |
|                     You can test the chronometer status witch the          |
|                     "i_APCI1710_GetChronoProgressStatus" function.         |
|                                                                            |
|                     The returned value from pul_ChronoValue parameter is   |
|                     not real measured timing.                              |
|                     You must used the "i_APCI1710_ConvertChronoValue"      |
|                     function or make this operation for calculate the      |
|                     timing:                                                |
|                                                                            |
|                     Timing = pul_ChronoValue * pul_RealTimingInterval.     |
|                                                                            |
|                     pul_RealTimingInterval is the returned parameter from  |
|                     "i_APCI1710_InitChrono" function and the time unity is |
|                     the b_TimingUnit from "i_APCI1710_InitChrono" function|
+----------------------------------------------------------------------------+
| Input Parameters  : BYTE  b_BoardHandle  : Handle of board APCI-1710       |
|                     BYTE  b_ModulNbr     : Selected module number (0 to 3) |
+----------------------------------------------------------------------------+
| Output Parameters : PULONG   pb_ChronoStatus : Return the chronometer      |
|                                                status.                     |
|                                                0 : Measurement not started.|
|                                                    No start signal occur.  |
|                                                1 : Measurement started.    |
|                                                    A start signal occur.   |
|                                                2 : Measurement stopped.    |
|                                                    A stop signal occur.    |
|                                                    The measurement is      |
|                                                    terminate.              |
|                                                3: A overflow occur. You    |
|                                                   must change the base     |
|                                                   timing witch the         |
|                                                   function                 |
|                                                   "i_APCI1710_InitChrono"  |
|                     PULONG  pul_ChronoValue  : Chronometer timing value.   |
+----------------------------------------------------------------------------+
| Return Value      :  0: No error                                           |
|                     -1: The handle parameter of the board is wrong         |
|                     -2: Module selection wrong                             |
|                     -3: The module is not a Chronometer module             |
|                     -4: Chronometer not initialised see function           |
|                         "i_APCI1710_InitChrono"                            |
|                     -5: Timeout parameter is wrong (0 to 65535)            |
|                     -6: Interrupt routine installed. You can not read      |
|                         directly the chronometer measured timing.          |
+----------------------------------------------------------------------------+
*/

INT   i_APCI1710_ReadChronoValue (BYTE     b_BoardHandle,
                                  BYTE     b_ModuleNumber,
                                  UINT    ui_TimeOut,
                                  PBYTE   pb_ChronoStatus,
                                  PULONG pul_ChronoValue)
	{
	INT    i_ReturnValue = 0;
	DWORD dw_Status;
	DWORD dw_TimeOut = 0;

        /*******************************************************/
        /* Gets the pointer to the board information structure */
        /*******************************************************/
        str_APCI1710_BoardInformation *BoardInformation = NULL;

        BoardInformation = &(s_APCI1710_DriverStruct.
                             s_BoardInformation[b_BoardHandle]);

	/***************************/
	/* Test if board handle OK */
	/***************************/

        if (b_BoardHandle >= s_APCI1710_DriverStruct.b_NumberOfBoard)
	   {
	   return (-1);
	   }

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModuleNumber < 4)
	   {
	   /***********************/
	   /* Test if chronometer */
	   /***********************/

            if (BoardInformation -> dw_MolduleConfiguration [b_ModuleNumber] == APCI1710_CHRONOMETER)

	      {
	      /***********************************/
	      /* Test if chronometer initialised */
	      /***********************************/

	      if (s_APCI1710_DriverStruct.
		  s_BoardInformation[b_BoardHandle].
		  s_ModuleInformation[b_ModuleNumber].
		  s_ChronoModuleInformation.
                  b_ChronoInit == 1)
		 {
		 /*****************************/
		 /* Test the timout parameter */
		 /*****************************/

		 if ((ui_TimeOut >= 0) && (ui_TimeOut <= 65535UL))
		    {
		    /***************************************/
		    /* Test if interrupt routine installed */
		    /***************************************/

		    if (s_APCI1710_DriverStruct.
			s_BoardInformation[b_BoardHandle].
			b_InterruptInitialized == APCI1710_DISABLE)
		       {
		       for (;;)
			  {
			  /*******************/
			  /* Read the status */
			  /*******************/

	         dw_Status = inl (BoardInformation -> s_BaseInformation.
		                  ui_BaseAddress[2] + 8 +
		   		  (64 * b_ModuleNumber));

			  /********************/
			  /* Test if overflow */
			  /********************/

			  if ((dw_Status & 8) == 8)
			     {
			     /******************/
			     /* Overflow occur */
			     /******************/

			     *pb_ChronoStatus = 3;

			     /***************************/
			     /* Test if continuous mode */
			     /***************************/

			     if (s_APCI1710_DriverStruct.
		                 s_BoardInformation[b_BoardHandle].
		                 s_ModuleInformation[b_ModuleNumber].
		                 s_ChronoModuleInformation.
		                 b_CycleMode == APCI1710_CONTINUOUS)
				{
				/*************************/
				/* Clear status register */
				/*************************/

				outl   (0,
					BoardInformation ->
					s_BaseInformation.
					ui_BaseAddress[2] + 36 + (64 * b_ModuleNumber));
				}

			     break;
			     } // if ((dw_Status & 8) == 8)
			  else
			     {
			     /*******************************/
			     /* Test if measurement stopped */
			     /*******************************/

			     if ((dw_Status & 2) == 2)
				{
				/***********************/
				/* A stop signal occur */
				/***********************/

				*pb_ChronoStatus = 2;

				/***************************/
				/* Test if continnous mode */
				/***************************/

			     if (s_APCI1710_DriverStruct.
		                 s_BoardInformation[b_BoardHandle].
		                 s_ModuleInformation[b_ModuleNumber].
		                 s_ChronoModuleInformation.
		                 b_CycleMode == APCI1710_CONTINUOUS)
				   {
				   /*************************/
				   /* Clear status register */
				   /*************************/

				outl   (0,
					BoardInformation ->
					s_BaseInformation.
					ui_BaseAddress[2] + 36 + (64 * b_ModuleNumber));
				   }
				break;
				} // if ((dw_Status & 2) == 2)
			     else
				{
				/*******************************/
				/* Test if measurement started */
				/*******************************/

				if ((dw_Status & 1) == 1)
				   {
				   /************************/
				   /* A start signal occur */
				   /************************/

				   *pb_ChronoStatus = 1;
				   } // if ((dw_Status & 1) == 1)
				else
				   {
				   /***************************/
				   /* Measurement not started */
				   /***************************/

				   *pb_ChronoStatus = 0;
				   } // if ((dw_Status & 1) == 1)
				} // if ((dw_Status & 2) == 2)
			     } // if ((dw_Status & 8) == 8)

			  if (dw_TimeOut == ui_TimeOut)
			     {
			     /*****************/
			     /* Timeout occur */
			     /*****************/

			     break;
			     }
			  else
			     {
			     /*************************/
			     /* Increment the timeout */
			     /*************************/

			     dw_TimeOut = dw_TimeOut + 1;
			     }
			  } // for (;;)

		       /*****************************/
		       /* Test if stop signal occur */
		       /*****************************/

		       if (*pb_ChronoStatus == 2)
			  {
			  /**********************************/
			  /* Read the measured timing value */
			  /**********************************/

			  *pul_ChronoValue = inl (BoardInformation -> s_BaseInformation.
						  ui_BaseAddress[2] + 4 + 64 * b_ModuleNumber);

			  if (*pul_ChronoValue != 0)
			     {
			     *pul_ChronoValue = *pul_ChronoValue - 1;
			     }
			  }
		       else
			  {
			  /*************************/
			  /* Test if timeout occur */
			  /*************************/

			  if ((*pb_ChronoStatus != 3) && (dw_TimeOut == ui_TimeOut) && (ui_TimeOut != 0))
			     {
			     /*****************/
			     /* Timeout occur */
			     /*****************/

			     *pb_ChronoStatus = 4;
			     }
			  }
		       }
		    else
		       {
		       /*******************************/
		       /* Interrupt routine installed */
		       /*******************************/

		       i_ReturnValue = -6;
		       }
		    }
		 else
		    {
		    /******************************/
		    /* Timeout parameter is wrong */
		    /******************************/

		    i_ReturnValue = -5;
		    }
		 }
	      else
		 {
		 /*******************************/
		 /* Chronometer not initialised */
		 /*******************************/

		 i_ReturnValue = -4;
		 }
	      }
	   else
	      {
	      /******************************************/
	      /* The module is not a Chronometer module */
	      /******************************************/

	      i_ReturnValue = -3;
	      }
	   }
	else
	   {
	   /***********************/
	   /* Module number error */
	   /***********************/

	   i_ReturnValue = -2;
	   }

	return (i_ReturnValue);
	}
	
	
/*
+----------------------------------------------------------------------------+
| Function Name     : INT i_APCI1710_ConvertChronoValue                      |
|                               (BYTE     b_BoardHandle,                     |
|                                BYTE     b_ModulNbr,                        |
|                                ULONG   ul_ChronoValue,                     |
|                                PULONG pul_Hour,                            |
|                                PBYTE   pb_Minute,                          |
|                                PBYTE   pb_Second,                          |
|                                PUINT  pui_MilliSecond,                     |
|                                PUINT  pui_MicroSecond,                     |
|                                PUINT  pui_NanoSecond)                      |
+----------------------------------------------------------------------------+
| Task              : Convert the chronometer measured timing                |
|                     (ul_ChronoValue) in to h, mn, s, ms, µs, ns.           |
+----------------------------------------------------------------------------+
| Input Parameters  : BYTE    b_BoardHandle : Handle of board APCI-1710      |
|                     BYTE    b_ModulNbr    : Selected module number (0 to 3)|
|                     ULONG  ul_ChronoValue : Measured chronometer timing    |
|                                             value.                         |
|                                             See"i_APCI1710_ReadChronoValue"|
+----------------------------------------------------------------------------+
| Output Parameters : PULONG    pul_Hour        : Chronometer timing hour    |
|                     PBYTE     pb_Minute      : Chronometer timing minute   |
|                     PBYTE     pb_Second      : Chronometer timing second   |
|                     PUINT    pui_MilliSecond  : Chronometer timing mini    |
|                                                second                      |
|                     PUINT    pui_MicroSecond : Chronometer timing micro    |
|                                                second                      |
|                     PUINT    pui_NanoSecond  : Chronometer timing nano     |
|                                                 second                     |
+----------------------------------------------------------------------------+
| Return Value      :  0: No error                                           |
|                     -1: The handle parameter of the board is wrong         |
|                     -2: Module selection wrong                             |
|                     -3: The module is not a Chronometer module             |
|                     -4: Chronometer not initialised see function           |
|                         "i_APCI1710_InitChrono"                            |
+----------------------------------------------------------------------------+
*/

INT i_APCI1710_ConvertChronoValue     (BYTE     b_BoardHandle,
				       BYTE     b_ModuleNumber,
				       ULONG   ul_ChronoValue,
				       PULONG pul_Hour,
				       PBYTE   pb_Minute,
				       PBYTE   pb_Second,
				       PUINT  pui_MilliSecond,
				       PUINT  pui_MicroSecond,
				       PUINT  pui_NanoSecond)
	{
	INT    i_ReturnValue = 0;
	double d_Time;

        /*******************************************************/
        /* Gets the pointer to the board information structure */
        /*******************************************************/
        str_APCI1710_BoardInformation *BoardInformation = NULL;

        BoardInformation = &(s_APCI1710_DriverStruct.
                             s_BoardInformation[b_BoardHandle]);

	/***************************/
	/* Test if board handle OK */
	/***************************/
        
	if (b_BoardHandle >= s_APCI1710_DriverStruct.b_NumberOfBoard)
	   {
	   return (-1);
	   }

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModuleNumber < 4)
	   {
	   /***********************/
	   /* Test if chronometer */
	   /***********************/

            if (BoardInformation -> dw_MolduleConfiguration [b_ModuleNumber] == APCI1710_CHRONOMETER)
	      {
	      /***********************************/
	      /* Test if chronometer initialised */
	      /***********************************/

	      if (s_APCI1710_DriverStruct.
		  s_BoardInformation[b_BoardHandle].
		  s_ModuleInformation[b_ModuleNumber].
		  s_ChronoModuleInformation.
                  b_ChronoInit == 1)
		 {
		    d_Time = (double) ul_ChronoValue * (double) s_APCI1710_DriverStruct.
		  						s_BoardInformation[b_BoardHandle].
		  						s_ModuleInformation[b_ModuleNumber].
		  						s_ChronoModuleInformation.
								d_TimingInterval;

		    switch (s_APCI1710_DriverStruct.
			    s_BoardInformation[b_BoardHandle].
		  	    s_ModuleInformation[b_ModuleNumber].
		  	    s_ChronoModuleInformation.
			    b_TimingUnit)
		       {
		       case 0:
               break;

		       case 1:
               // µs -> ns
			      d_Time = d_Time * (double) 1000.0;
               break;

		       case 2:
               // ms -> ns
			      d_Time = d_Time * (double) 1000.0;
			      d_Time = d_Time * (double) 1000.0;
               break;

               case 3:
               // s -> ns
			      d_Time = d_Time * (double) 1000.0;
				  d_Time = d_Time * (double) 1000.0;
			      d_Time = d_Time * (double) 1000.0;
               break;

			   case 4:
               // mn -> ns
			      d_Time = d_Time * (double) 1000.0;
				  d_Time = d_Time * (double) 1000.0;
			      d_Time = d_Time * (double) 1000.0;
				  d_Time = d_Time * (double) 60;
			   break;
		       }

		    /**********************/
	        /* Calculate the hour */
		    /**********************/


	        *pul_Hour = (ULONG) (d_Time / (double) 1000.0 / (double) 1000.0 / (double) 1000.0 / (double) 60.0 / (double) 60.0);

			if ((double) *pul_Hour > ((d_Time / (double) 1000.0 / (double) 1000.0 / (double) 1000.0 / (double) 60.0 / (double) 60.0)))
			   {
			   *pul_Hour = *pul_Hour - 1;
			   }

            d_Time    = d_Time - (double) (((double) *pul_Hour * 1000.0 * 1000.0 * 1000.0 *60.0 * 60.0));

	        /************************/
	        /* Calculate the minute */
	        /************************/


			*pb_Minute = (BYTE) (d_Time / (double) 1000.0 / (double) 1000.0 / (double) 1000.0 / (double) 60.0);
			if ((double) *pb_Minute > ((d_Time / (double) 1000.0 / (double) 1000.0 / (double) 1000.0 / (double) 60.0)))
			   {
			   *pb_Minute = *pb_Minute - 1;
			   }

            d_Time    = d_Time - (double) (((double) *pb_Minute * 1000.0 * 1000.0 * 1000.0 *60.0));

			if (*pb_Minute == 60)
			   {
			   *pb_Minute = 0;
			   *pul_Hour  = *pul_Hour + 1;
			   }

	        /************************/
	        /* Calculate the second */
	        /************************/


			*pb_Second = (BYTE) (d_Time / (double) 1000.0 / (double) 1000.0 / (double) 1000.0);
			if ((double) *pb_Second > ((d_Time / (double) 1000.0 / (double) 1000.0 / (double) 1000.0)))
			   {
			   *pb_Second = *pb_Second - 1;
			   }

            d_Time    = d_Time - (double) (((double) *pb_Second * 1000.0 * 1000.0 * 1000.0));

			if (*pb_Second == 60)
			   {
			   *pb_Second = 0;
			   *pb_Minute = *pb_Minute + 1;

			   if (*pb_Minute == 60)
			      {
				  *pb_Minute = 0;
				  *pul_Hour  = *pul_Hour + 1;
				  }
			   }


	        /******************************/
	        /* Calculate the milli second */
	        /******************************/


			*pui_MilliSecond = (UINT) (d_Time / (double) 1000.0 / (double) 1000.0);
			if ((double) *pui_MilliSecond > ((d_Time / (double) 1000.0 / (double) 1000.0)))
			   {
			   *pui_MilliSecond = *pui_MilliSecond - 1;
			   }

            d_Time    = d_Time - (double) (((double) *pui_MilliSecond * 1000.0 * 1000.0));

			if (*pui_MilliSecond == 1000)
			   {
			   *pui_MilliSecond = 0;
			   *pb_Second       = *pb_Second + 1;


			   if (*pb_Second == 60)
			      {
			      *pb_Second = 0;
			      *pb_Minute = *pb_Minute + 1;

			      if (*pb_Minute == 60)
			         {
				     *pb_Minute = 0;
				     *pul_Hour  = *pul_Hour + 1;
			 	     }
			      }
			   }

	        /******************************/
	        /* Calculate the micro second */
	        /******************************/


			*pui_MicroSecond = (UINT) (d_Time / (double) 1000.0);

			if ((double) *pui_MicroSecond > ((d_Time / (double) 1000.0)))
			   {
			   *pui_MicroSecond = *pui_MicroSecond - 1;
			   }


            d_Time    = d_Time - (double) (((double) *pui_MicroSecond * 1000.0));

            if (*pui_MicroSecond == 1000)
			   {
			   *pui_MicroSecond = 0;
			   *pui_MilliSecond = *pui_MilliSecond + 1;


			   if (*pui_MilliSecond == 1000)
			      {
			      *pui_MilliSecond = 0;
			      *pb_Second = *pb_Second + 1;

			      if (*pb_Second == 60)
			         {
			         *pb_Second = 0;
			         *pb_Minute = *pb_Minute + 1;

			         if (*pb_Minute == 60)
			            {
				        *pb_Minute = 0;
				        *pul_Hour  = *pul_Hour + 1;
				        }
			 	     }
			      }
			   }


	        /*****************************/
	        /* Calculate the nano second */
	        /*****************************/

		    *pui_NanoSecond = (UINT) d_Time;

			if (*pui_NanoSecond == 1000)
			   {
			   *pui_NanoSecond  = 0;
			   *pui_MicroSecond = *pui_MicroSecond + 1;

               if (*pui_MicroSecond == 1000)
			      {
			      *pui_MicroSecond = 0;
			      *pui_MilliSecond = *pui_MilliSecond + 1;

			      if (*pui_MilliSecond == 1000)
			         {
			         *pui_MilliSecond = 0;
			         *pb_Second       = *pb_Second + 1;

			         if (*pb_Second == 60)
			            {
			            *pb_Second = 0;
			            *pb_Minute = *pb_Minute + 1;

			            if (*pb_Minute == 60)
			               {
				           *pb_Minute = 0;
				           *pul_Hour  = *pul_Hour + 1;
						   }
				        }
			 	     }
			      }
			   }
		 }
	      else
		 {
		 /*******************************/
		 /* Chronometer not initialised */
		 /*******************************/

		 i_ReturnValue = -4;
		 }
	      }
	   else
	      {
	      /******************************************/
	      /* The module is not a Chronometer module */
	      /******************************************/

	      i_ReturnValue = -3;
	      }
	   }
	else
	   {
	   /***********************/
	   /* Module number error */
	   /***********************/

	   i_ReturnValue = -2;
	   }

	return (i_ReturnValue);
	}

/*
+----------------------------------------------------------------------------+
| Function Name     : INT i_APCI1710_SetChronoChlOn                          |
|                               (BYTE  b_BoardHandle,                        |
|                                BYTE  b_ModulNbr,                           |
|                                BYTE  b_OutputChannel)                      |
+----------------------------------------------------------------------------+
| Task              : Sets the output witch has been passed with the         |
|                     parameter b_Channel. Setting an output means setting an|
|                     output high.                                           |
+----------------------------------------------------------------------------+
| Input Parameters  : BYTE  b_BoardHandle   : Handle of board APCI-1710      |
|                     BYTE  b_ModulNbr      : Selected module number (0 to 3)|
|                     BYTE  b_OutputChannel : Selection from digital output  |
|                                             channel (0 to 2)               |
|                                              0 : Channel H                 |
|                                              1 : Channel A                 |
|                                              2 : Channel B                 |
+----------------------------------------------------------------------------+
| Output Parameters : -                                                      |
+----------------------------------------------------------------------------+
| Return Value      :  0: No error                                           |
|                     -1: The handle parameter of the board is wrong         |
|                     -2: Module selection wrong                             |
|                     -3: The module is not a Chronometer module             |
|                     -4: The selected digital output is wrong               |
|                     -5: Chronometer not initialised see function           |
|                         "i_APCI1710_InitChrono"                            |
+----------------------------------------------------------------------------+
*/

INT   i_APCI1710_SetChronoChlOn       (BYTE  b_BoardHandle,
					 BYTE  b_ModuleNumber,
					 BYTE  b_OutputChannel)
	{
	INT    i_ReturnValue = 0;

        /*******************************************************/
        /* Gets the pointer to the board information structure */
        /*******************************************************/
        str_APCI1710_BoardInformation *BoardInformation = NULL;

        BoardInformation = &(s_APCI1710_DriverStruct.
                             s_BoardInformation[b_BoardHandle]);

	/***************************/
	/* Test if board handle OK */
	/***************************/

        if (b_BoardHandle >= s_APCI1710_DriverStruct.b_NumberOfBoard)
	   {
	   return (-1);
	   }

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModuleNumber < 4)
	   {
	   /***********************/
	   /* Test if chronometer */
	   /***********************/

            if (BoardInformation -> dw_MolduleConfiguration [b_ModuleNumber] == APCI1710_CHRONOMETER)
	      {
	      /***********************************/
	      /* Test if chronometer initialised */
	      /***********************************/
	      
	      if (s_APCI1710_DriverStruct.
		  s_BoardInformation[b_BoardHandle].
		  s_ModuleInformation[b_ModuleNumber].
		  s_ChronoModuleInformation.
                  b_ChronoInit == 1)
		 {
		 /***********************************/
		 /* Test the digital output channel */
		 /***********************************/

		 if ((b_OutputChannel >= 0) && (b_OutputChannel <= 2))
		    {
		    outl   (1,
			    BoardInformation ->
			    s_BaseInformation.
			    ui_BaseAddress[2] + 20 + (b_OutputChannel * 4) + (64 * b_ModuleNumber));
		    } // if ((b_OutputChannel >= 0) && (b_OutputChannel <= 2))
		 else
		    {
		    /****************************************/
		    /* The selected digital output is wrong */
		    /****************************************/

		    i_ReturnValue = -4;

		    } // if ((b_OutputChannel >= 0) && (b_OutputChannel <= 2))
		 }
	      else
		 {
		 /*******************************/
		 /* Chronometer not initialised */
		 /*******************************/

		 i_ReturnValue = -5;
		 }
	      }
	   else
	      {
	      /******************************************/
	      /* The module is not a Chronometer module */
	      /******************************************/

	      i_ReturnValue = -3;
	      }
	   }
	else
	   {
	   /***********************/
	   /* Module number error */
	   /***********************/

	   i_ReturnValue = -2;
	   }

	return (i_ReturnValue);
	}

/*
+----------------------------------------------------------------------------+
| Function Name     :  INT i_APCI1710_SetChronoChlOff                        |
|                               (BYTE  b_BoardHandle,                        |
|                                BYTE  b_ModulNbr,                           |
|                                BYTE  b_OutputChannel)                      |
+----------------------------------------------------------------------------+
| Task              : Resets the output witch has been passed with the       |
|                     parameter b_Channel. Resetting an output means setting |
|                     an output low.                                         |
+----------------------------------------------------------------------------+
| Input Parameters  : BYTE b_BoardHandle   : Handle of board APCI-1710       |
|                     BYTE b_ModulNbr      : Selected module number (0 to 3) |
|                     BYTE b_OutputChannel : Selection from digital output   |
|                                             channel (0 to 2)               |
|                                              0 : Channel H                 |
|                                              1 : Channel A                 |
|                                              2 : Channel B                 |
+----------------------------------------------------------------------------+
| Output Parameters : -                                                      |
+----------------------------------------------------------------------------+
| Return Value      :  0: No error                                           |
|                     -1: The handle parameter of the board is wrong         |
|                     -2: Module selection wrong                             |
|                     -3: The module is not a Chronometer module             |
|                     -4: The selected digital output is wrong               |
|                     -5: Chronometer not initialised see function           |
|                         "i_APCI1710_InitChrono"                            |
+----------------------------------------------------------------------------+
*/

INT   i_APCI1710_SetChronoChlOff      (BYTE  b_BoardHandle,
					 BYTE  b_ModuleNumber,
					 BYTE  b_OutputChannel)
	{
	INT    i_ReturnValue = 0;

        /*******************************************************/
        /* Gets the pointer to the board information structure */
        /*******************************************************/
        str_APCI1710_BoardInformation *BoardInformation = NULL;

        BoardInformation = &(s_APCI1710_DriverStruct.
                             s_BoardInformation[b_BoardHandle]);

	/***************************/
	/* Test if board handle OK */
	/***************************/

        if (b_BoardHandle >= s_APCI1710_DriverStruct.b_NumberOfBoard)
	   {
	   return (-1);
	   }

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModuleNumber < 4)
	   {
	   /***********************/
	   /* Test if chronometer */
	   /***********************/

            if (BoardInformation -> dw_MolduleConfiguration [b_ModuleNumber] == APCI1710_CHRONOMETER)
	      {
	      /***********************************/
	      /* Test if chronometer initialised */
	      /***********************************/

	      if (s_APCI1710_DriverStruct.
		  s_BoardInformation[b_BoardHandle].
		  s_ModuleInformation[b_ModuleNumber].
		  s_ChronoModuleInformation.
                  b_ChronoInit == 1)
// END JK 08.09.03 : Implementation from CHRONO functionality
		 {
		 /***********************************/
		 /* Test the digital output channel */
		 /***********************************/

		 if ((b_OutputChannel >= 0) && (b_OutputChannel <= 2))
		    {
		    outl   (0,
			    BoardInformation ->
			    s_BaseInformation.
			    ui_BaseAddress[2] + 20 + (b_OutputChannel * 4) + (64 * b_ModuleNumber));
		    } // if ((b_OutputChannel >= 0) && (b_OutputChannel <= 2))
		 else
		    {
		    /****************************************/
		    /* The selected digital output is wrong */
		    /****************************************/

		    i_ReturnValue = -4;

		    } // if ((b_OutputChannel >= 0) && (b_OutputChannel <= 2))
		 }
	      else
		 {
		 /*******************************/
		 /* Chronometer not initialised */
		 /*******************************/

		 i_ReturnValue = -5;
		 }
	      }
	   else
	      {
	      /******************************************/
	      /* The module is not a Chronometer module */
	      /******************************************/

	      i_ReturnValue = -3;
	      }
	   }
	else
	   {
	   /***********************/
	   /* Module number error */
	   /***********************/

	   i_ReturnValue = -2;
	   }

	return (i_ReturnValue);
	}

/*
+----------------------------------------------------------------------------+
| Function Name     :  INT  i_APCI1710_ReadChronoChlValue                    |
|                               (BYTE   b_BoardHandle,                       |
|                                BYTE   b_ModulNbr,                          |
|                                BYTE   b_InputChannel,                      |
|                                PBYTE pb_ChannelStatus)                     |
+----------------------------------------------------------------------------+
| Task              : Return the status from selected digital input          |
|                     (b_InputChannel) from selected chronometer             |
|                     module (b_ModulNbr).                                   |
+----------------------------------------------------------------------------+
| Input Parameters  : BYTE b_BoardHandle   : Handle of board APCI-1710       |
|                     BYTE b_ModulNbr      : Selected module number (0 to 3) |
|                     BYTE b_InputChannel  : Selection from digital input    |
|                                             channel (0 to 2)               |
|                                                0 : Channel E               |
|                                                1 : Channel F               |
|                                                2 : Channel G               |
+----------------------------------------------------------------------------+
| Output Parameters : PBYTE_ pb_ChannelStatus : Digital input channel status.|
|                                                0 : Channel is not active   |
|                                                1 : Channel is active       |
+----------------------------------------------------------------------------+
| Return Value      :  0: No error                                           |
|                     -1: The handle parameter of the board is wrong         |
|                     -2: Module selection wrong                             |
|                     -3: The module is not a Chronometer module             |
|                     -4: The selected digital input is wrong                |
|                     -5: Chronometer not initialised see function           |
|                         "i_APCI1710_InitChrono"                            |
+----------------------------------------------------------------------------+
*/

INT   i_APCI1710_ReadChronoChlValue   (BYTE   b_BoardHandle,
					 BYTE   b_ModuleNumber,
					 BYTE   b_InputChannel,
					 PBYTE pb_ChannelStatus)
	{
	INT    i_ReturnValue = 0;
	DWORD dw_Status;

        /*******************************************************/
        /* Gets the pointer to the board information structure */
        /*******************************************************/
        str_APCI1710_BoardInformation *BoardInformation = NULL;

        BoardInformation = &(s_APCI1710_DriverStruct.
                             s_BoardInformation[b_BoardHandle]);

	/***************************/
	/* Test if board handle OK */
	/***************************/

        if (b_BoardHandle >= s_APCI1710_DriverStruct.b_NumberOfBoard)
	   {
	   return (-1);
	   }

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModuleNumber < 4)
	   {
	   /***********************/
	   /* Test if chronometer */
	   /***********************/

            if (BoardInformation -> dw_MolduleConfiguration [b_ModuleNumber] == APCI1710_CHRONOMETER)
	      {
	      /***********************************/
	      /* Test if chronometer initialised */
	      /***********************************/

	      if (s_APCI1710_DriverStruct.
		  s_BoardInformation[b_BoardHandle].
		  s_ModuleInformation[b_ModuleNumber].
		  s_ChronoModuleInformation.
                  b_ChronoInit == 1)
		 {
		 /**********************************/
		 /* Test the digital input channel */
		 /**********************************/

		 if ((b_InputChannel >= 0) && (b_InputChannel <= 2))
		    {
			dw_Status = inl (BoardInformation -> s_BaseInformation.
		                         ui_BaseAddress[2] + 12 +
					(64 * b_ModuleNumber));

		    *pb_ChannelStatus = (BYTE) (((dw_Status >> b_InputChannel) & 1) ^ 1);
		    } // if ((b_InputChannel >= 0) && (b_InputChannel <= 2))
		 else
		    {
		    /***************************************/
		    /* The selected digital input is wrong */
		    /***************************************/

		    i_ReturnValue = -4;
		    } // if ((b_InputChannel >= 0) && (b_InputChannel <= 2))
		 }
	      else
		 {
		 /*******************************/
		 /* Chronometer not initialised */
		 /*******************************/

		 i_ReturnValue = -5;
		 }
	      }
	   else
	      {
	      /******************************************/
	      /* The module is not a Chronometer module */
	      /******************************************/

	      i_ReturnValue = -3;
	      }
	   }
	else
	   {
	   /***********************/
	   /* Module number error */
	   /***********************/

	   i_ReturnValue = -2;
	   }

	return (i_ReturnValue);
	}

/*
+----------------------------------------------------------------------------+
| Function Name     :  INT  i_APCI1710_ReadChronoPortValue                   |
|                               (BYTE   b_BoardHandle,                       |
|                                BYTE   b_ModulNbr,                          |
|                                PBYTE pb_PortValue)                         |
+----------------------------------------------------------------------------+
| Task              : Return the status from digital inputs port from        |
|                     selected  (b_ModulNbr) chronometer module.             |
+----------------------------------------------------------------------------+
| Input Parameters  : BYTE  b_BoardHandle   : Handle of board APCI-1710      |
|                     BYTE  b_ModulNbr      : Selected module number (0 to 3)|
+----------------------------------------------------------------------------+
| Output Parameters : PBYTE_ pb_PortValue   : Digital inputs port status.    |
+----------------------------------------------------------------------------+
| Return Value      :  0: No error                                           |
|                     -1: The handle parameter of the board is wrong         |
|                     -2: Module selection wrong                             |
|                     -3: The module is not a Chronometer module             |
|                     -4: Chronometer not initialised see function           |
|                         "i_APCI1710_InitChrono"                            |
+----------------------------------------------------------------------------+
*/

INT   i_APCI1710_ReadChronoPortValue  (BYTE   b_BoardHandle,
					 BYTE   b_ModuleNumber,
					 PBYTE pb_PortValue)
	{
	INT    i_ReturnValue = 0;
	DWORD dw_Status;

        /*******************************************************/
        /* Gets the pointer to the board information structure */
        /*******************************************************/
        str_APCI1710_BoardInformation *BoardInformation = NULL;

        BoardInformation = &(s_APCI1710_DriverStruct.
                             s_BoardInformation[b_BoardHandle]);

	/***************************/
	/* Test if board handle OK */
	/***************************/

        if (b_BoardHandle >= s_APCI1710_DriverStruct.b_NumberOfBoard)
	   {
	   return (-1);
	   }

	/**************************/
	/* Test the module number */
	/**************************/

	if (b_ModuleNumber < 4)
	   {
	   /***********************/
	   /* Test if chronometer */
	   /***********************/

            if (BoardInformation -> dw_MolduleConfiguration [b_ModuleNumber] == APCI1710_CHRONOMETER)
	      {
	      /***********************************/
	      /* Test if chronometer initialised */
	      /***********************************/

	      if (s_APCI1710_DriverStruct.
		  s_BoardInformation[b_BoardHandle].
		  s_ModuleInformation[b_ModuleNumber].
		  s_ChronoModuleInformation.
                  b_ChronoInit == 1)
		 {
			dw_Status = inl (BoardInformation -> s_BaseInformation.
		                         ui_BaseAddress[2] + 12 +
					(64 * b_ModuleNumber));

		 *pb_PortValue = (BYTE) ((dw_Status & 0x7) ^ 7);
		 }
	      else
		 {
		 /*******************************/
		 /* Chronometer not initialised */
		 /*******************************/

		 i_ReturnValue = -4;
		 }
	      }
	   else
	      {
	      /******************************************/
	      /* The module is not a Chronometer module */
	      /******************************************/

	      i_ReturnValue = -3;
	      }
	   }
	else
	   {
	   /***********************/
	   /* Module number error */
	   /***********************/

	   i_ReturnValue = -2;
	   }

	return (i_ReturnValue);
	}
// END JK 08.09.03 : Implementation from CHRONO functionality

/*
  +----------------------------------------------------------------------------+
  | Function Name     : INT i_APCI1710_Write16BitCounterValue                  |
  |                                               (BYTE  b_BoardHandle         |
  |                                                BYTE  b_ModulNbr,           |
  |                                                BYTE  b_SelectedCounter,    |
  |                                                UINT ui_WriteValue)         |
  +----------------------------------------------------------------------------+
  | Task              : Write a 16-Bit value (ui_WriteValue) in to the selected|
  |                     16-Bit counter (b_SelectedCounter) from selected module|
  |                     (b_ModulNbr).                                          |
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE  b_BoardHandle     : Handle of board APCI-1710    |
  |                     BYTE  b_ModulNbr        : Module number to configure   |
  |                                              (0 to 3)                      |
  |                     BYTE  b_SelectedCounter : Selected 16-Bit counter      |
  |                                               (0 or 1)                     |
  |                     UINT  ui_WriteValue     : 16-Bit write value           |
  +----------------------------------------------------------------------------+
  | Output Parameters : -                                                      |
  +----------------------------------------------------------------------------+
  | Return Value      :  0: No error                                           |
  |                     -1: The handle parameter of the board is wrong         |
  |                     -2: No counter module found                            |
  |                     -3: Counter not initialised see function               |
  |                         "i_APCI1710_InitCounter"                           |
  |                     -4: The selected 16-Bit counter parameter is wrong     |
  +----------------------------------------------------------------------------+
*/

INT   i_APCI1710_Write16BitCounterValue       (BYTE  b_BoardHandle,
					       BYTE  b_ModuleNumber,
					       BYTE  b_SelectedCounter,
					       UINT ui_WriteValue)
{
  INT i_ReturnValue = 0;
  /*******************************************************/
  /* Gets the pointer to the board information structure */
  /*******************************************************/
  str_APCI1710_BoardInformation *BoardInformation = NULL; 
  
  BoardInformation = &(s_APCI1710_DriverStruct.
		       s_BoardInformation[b_BoardHandle]);

  /****************************************/
  /* Checks if the b_BoardHandle is valid */
  /****************************************/
  if (b_BoardHandle < s_APCI1710_DriverStruct.b_NumberOfBoard)
    {  

      /**************************/
      /* Test the module number */
      /**************************/

      if (b_ModuleNumber < 4)
	{
	  /*******************************/
	  /* Test if counter initialised */
	  /*******************************/
	  if (s_APCI1710_DriverStruct.s_BoardInformation[b_BoardHandle].
	      s_ModuleInformation[b_ModuleNumber].
	      s_Inc_CptInformation.b_CounterInit == 1)
	    {
	      /******************************/
	      /* Test the counter selection */
	      /******************************/

	      if (b_SelectedCounter < 2)
		{
		  /*******************/
		  /* Write the value */
		  /*******************/

		  outl ((ULONG) ((ULONG) (ui_WriteValue) << (16 * b_SelectedCounter)),
			BoardInformation -> s_BaseInformation.
			ui_BaseAddress[2] + 
			8 + (b_SelectedCounter * 4) + (64 * b_ModuleNumber));
			
		}
	      else
		{
		  /**************************************************/
		  /* The selected 16-Bit counter parameter is wrong */
		  /**************************************************/

		  i_ReturnValue = -4;
		}
	    }
	  else
	    {
	      /****************************************/
	      /* Counter not initialised see function */
	      /* "i_APCI1710_InitCounter"             */
	      /****************************************/

	      i_ReturnValue = -3;
	    }
	}
      else
	{
	  /*************************************************/
	  /* The selected module number parameter is wrong */
	  /*************************************************/

	  i_ReturnValue = -2;
	}
    }
  else
    {
      /***************************************/
      /* The board handle parameter is wrong */
      /***************************************/

      i_ReturnValue = -1;
    }
  return (i_ReturnValue);
}

/*
  +----------------------------------------------------------------------------+
  | Function Name     :  INT i_APCI1710_Write32BitCounterValue                 |
  |                                               (BYTE   b_BoardHandle        |
  |                                                BYTE   b_ModulNbr,          |
  |                                                ULONG ul_WriteValue)        |
  +----------------------------------------------------------------------------+
  | Task              : Write a 32-Bit value (ui_WriteValue) in to the selected|
  |                     module (b_ModulNbr).                                   |
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE b_BoardHandle     : Handle of board APCI-1710     |
  |                     BYTE b_ModulNbr        : Module number to configure    |
  |                                              (0 to 3)                      |
  |                     ULONG ul_WriteValue    : 32-Bit write value            |
  +----------------------------------------------------------------------------+
  | Output Parameters : -                                                      |
  +----------------------------------------------------------------------------+
  | Return Value      :  0: No error                                           |
  |                     -1: The handle parameter of the board is wrong         |
  |                     -2: No counter module found                            |
  |                     -3: Counter not initialised see function               |
  |                         "i_APCI1710_InitCounter"                           |
  +----------------------------------------------------------------------------+
*/

INT   i_APCI1710_Write32BitCounterValue       (BYTE   b_BoardHandle,
					       BYTE   b_ModuleNumber,
					       ULONG ul_WriteValue)
{
  INT i_ReturnValue = 0;
  /*******************************************************/
  /* Gets the pointer to the board information structure */
  /*******************************************************/
  str_APCI1710_BoardInformation *BoardInformation = NULL; 
  
  BoardInformation = &(s_APCI1710_DriverStruct.
		       s_BoardInformation[b_BoardHandle]);

  /****************************************/
  /* Checks if the b_BoardHandle is valid */
  /****************************************/
  if (b_BoardHandle < s_APCI1710_DriverStruct.b_NumberOfBoard)
    {  

      /**************************/
      /* Test the module number */
      /**************************/

      if (b_ModuleNumber < 4)
	{
	  /*******************************/
	  /* Test if counter initialised */
	  /*******************************/
	  if (s_APCI1710_DriverStruct.s_BoardInformation[b_BoardHandle].
	      s_ModuleInformation[b_ModuleNumber].
	      s_Inc_CptInformation.b_CounterInit == 1)
	    {
	      /*******************/
	      /* Write the value */
	      /*******************/

	      outl (ul_WriteValue,
		    BoardInformation -> s_BaseInformation.
		    ui_BaseAddress[2] + 
		    4 + (64 * b_ModuleNumber));
	    }
	  else
	    {
	      /****************************************/
	      /* Counter not initialised see function */
	      /* "i_APCI1710_InitCounter"             */
	      /****************************************/

	      i_ReturnValue = -3;
	    }
	}
      else
	{
	  /*************************************************/
	  /* The selected module number parameter is wrong */
	  /*************************************************/

	  i_ReturnValue = -2;
	}
    }
  else
    {
      /***************************************/
      /* The board handle parameter is wrong */
      /***************************************/

      i_ReturnValue = -1;
    }

  return (i_ReturnValue);
}

/*
  +----------------------------------------------------------------------------+
  | Function Name     : VOID v_APCI1710_ReadAllModuleConfiguration             | 
  |                                   (BYTE b_BoardHandle)                     |
  +----------------------------------------------------------------------------+
  | Task              : Get module configuration. And fill the structure.      |
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE  b_BoardHandle : Handle of board APCI-1710        |
  +----------------------------------------------------------------------------+
  | Output Parameters : -                                                      |
  +----------------------------------------------------------------------------+
  | Return Value      :  -                                                     |
  +----------------------------------------------------------------------------+
*/

VOID v_APCI1710_ReadAllModuleConfiguration (BYTE b_BoardHandle)
{
  BYTE b_Module = 0;
  
  /*******************************************************/
  /* Gets the pointer to the board information structure */
  /*******************************************************/
  str_APCI1710_BoardInformation *BoardInformation = NULL; 
 
  BoardInformation = &(s_APCI1710_DriverStruct.
		       s_BoardInformation[b_BoardHandle]);

  /****************************************/
  /* Checks if the b_BoardHandle is valid */
  /****************************************/
  if (b_BoardHandle < s_APCI1710_DriverStruct.b_NumberOfBoard)
    { 
      /*********************************/ 
      /* Read all module configuration */ 
      /*********************************/ 

      BoardInformation -> dw_MolduleConfiguration [0] = 
        (inl(BoardInformation -> s_BaseInformation.
	     ui_BaseAddress[2]+60) & 0xFFFF0000UL);		
	     
      BoardInformation -> dw_MolduleConfiguration [1] = 
        (inl(BoardInformation -> s_BaseInformation.
	     ui_BaseAddress[2]+124) & 0xFFFF0000UL);	
	     
      BoardInformation -> dw_MolduleConfiguration [2] = 
        (inl(BoardInformation -> s_BaseInformation.
	     ui_BaseAddress[2]+188) & 0xFFFF0000UL);	
	     
      BoardInformation -> dw_MolduleConfiguration [3] = 
        (inl(BoardInformation -> s_BaseInformation.
	     ui_BaseAddress[2]+252) & 0xFFFF0000UL);	
                            
      printk ("\n+------------------------------------------+");
      printk ("\n+ Board %d module configuration is         +", b_BoardHandle);
      printk ("\n+------------------------------------------+");       
       
      for (b_Module = 0; b_Module < 4; b_Module ++)
	{       
          
          if (BoardInformation -> dw_MolduleConfiguration [b_Module] == 
              APCI1710_INCREMENTAL_COUNTER)
	    printk ("\n| Module %d: APCI1710_INCREMENTAL_COUNTER  |", b_Module);

          if (BoardInformation -> dw_MolduleConfiguration [b_Module] == 
              APCI1710_SSI_COUNTER)
	    printk ("\n| Module %d: APCI1710_SSI_COUNTER          |", b_Module);               

          if (BoardInformation -> dw_MolduleConfiguration [b_Module] == 
              APCI1710_TTL_IO)
	    printk ("\n| Module %d: APCI1710_TTL_IO               |", b_Module);  
              
          if (BoardInformation -> dw_MolduleConfiguration [b_Module] == 
              APCI1710_DIGITAL_IO)
	    printk ("\n| Module %d: APCI1710_DIGITAL_IO           |", b_Module);  
              
          if (BoardInformation -> dw_MolduleConfiguration [b_Module] == 
              APCI1710_82X54_TIMER)
	    printk ("\n| Module %d: APCI1710_82X54_TIMER          |", b_Module);                                                                 	
              
          if (BoardInformation -> dw_MolduleConfiguration [b_Module] == 
              APCI1710_CHRONOMETER)
	    printk ("\n| Module %d: APCI1710_CHRONOMETER          |", b_Module);                  
              
          if (BoardInformation -> dw_MolduleConfiguration [b_Module] == 
              APCI1710_PULSE_ENCODER)
	    printk ("\n| Module %d: APCI1710_PULSE_ENCODER        |", b_Module);                                
              
          if (BoardInformation -> dw_MolduleConfiguration [b_Module] == 
              APCI1710_PWM)
	    printk ("\n| Module %d: APCI1710_PWM                  |", b_Module);                                              
              
          if (BoardInformation -> dw_MolduleConfiguration [b_Module] == 
              APCI1710_ETM)
	    printk ("\n| Module %d: APCI1710_ETM                  |", b_Module);                                                            
              
          if (BoardInformation -> dw_MolduleConfiguration [b_Module] == 
              APCI1710_CDA)
	    printk ("\n| Module %d: APCI1710_CDA                  |", b_Module);                                                                          
	}  
                                    
      printk ("\n+------------------------------------------+\n");   
       
    }       
}




/*
  +----------------------------------------------------------------------------+
  | Function Name     : INT   i_APCI1710_InitCounter                           |
  |                               (BYTE           b_BoardHandle,               |
  |                                BYTE           b_ModulNbr,                  |
  |                                BYTE           b_CounterRange,              |
  |                                BYTE           b_FirstCounterModus,         |
  |                                BYTE           b_FirstCounterOption,        |
  |                                BYTE           b_SecondCounterModus,        |
  |                                BYTE           b_SecondCounterOption)       |
  +----------------------------------------------------------------------------+
  | Task              : Configure the counter operating mode from selected     |
  |                     module (b_ModulNbr). You must calling this function be |
  |                     for you call any other function witch access of        |
  |                     counters.                                              |
  |                                                                            |
  |                          Counter range                                     |
  |                          -------------                                     |
  | +------------------------------------+-----------------------------------+ |
  | | Parameter       Passed value       |        Description                | |
  | |------------------------------------+-----------------------------------| |
  | |b_ModulNbr   APCI1710_16BIT_COUNTER |  The module is configured for     | |
  | |                                    |  two 16-bit counter.              | |
  | |                                    |  - b_FirstCounterModus and        | |
  | |                                    |    b_FirstCounterOption           | |
  | |                                    |    configure the first 16 bit     | |
  | |                                    |    counter.                       | |
  | |                                    |  - b_SecondCounterModus and       | |
  | |                                    |    b_SecondCounterOption          | |
  | |                                    |    configure the second 16 bit    | |
  | |                                    |    counter.                       | |
  | |------------------------------------+-----------------------------------| |
  | |b_ModulNbr   APCI1710_32BIT_COUNTER |  The module is configured for one | |
  | |                                    |  32-bit counter.                  | |
  | |                                    |  - b_FirstCounterModus and        | |
  | |                                    |    b_FirstCounterOption           | |
  | |                                    |    configure the 32 bit counter.  | |
  | |                                    |  - b_SecondCounterModus and       | |
  | |                                    |    b_SecondCounterOption          | |
  | |                                    |    are not used and have no       | |
  | |                                    |    importance.                    | |
  | +------------------------------------+-----------------------------------+ |
  |                                                                            |
  |                      Counter operating mode                                |
  |                      ----------------------                                |
  |                                                                            |
  | +--------------------+-------------------------+-------------------------+ |
  | |    Parameter       |     Passed value        |    Description          | |
  | |--------------------+-------------------------+-------------------------| |
  | |b_FirstCounterModus | APCI1710_QUADRUPLE_MODE | In the quadruple mode,  | |
  | |       or           |                         | the edge analysis       | |
  | |b_SecondCounterModus|                         | circuit generates a     | |
  | |                    |                         | counting pulse from     | |
  | |                    |                         | each edge of 2 signals  | |
  | |                    |                         | which are phase shifted | |
  | |                    |                         | in relation to each     | |
  | |                    |                         | other.                  | |
  | |--------------------+-------------------------+-------------------------| |
  | |b_FirstCounterModus |   APCI1710_DOUBLE_MODE  | Functions in the same   | |
  | |       or           |                         | way as the quadruple    | |
  | |b_SecondCounterModus|                         | mode, except that only  | |
  | |                    |                         | two of the four edges   | |
  | |                    |                         | are analysed per        | |
  | |                    |                         | period                  | |
  | |--------------------+-------------------------+-------------------------| |
  | |b_FirstCounterModus |   APCI1710_SIMPLE_MODE  | Functions in the same   | |
  | |       or           |                         | way as the quadruple    | |
  | |b_SecondCounterModus|                         | mode, except that only  | |
  | |                    |                         | one of the four edges   | |
  | |                    |                         | is analysed per         | |
  | |                    |                         | period.                 | |
  | |--------------------+-------------------------+-------------------------| |
  | |b_FirstCounterModus |   APCI1710_DIRECT_MODE  | In the direct mode the  | |
  | |       or           |                         | both edge analysis      | |
  | |b_SecondCounterModus|                         | circuits are inactive.  | |
  | |                    |                         | The inputs A, B in the  | |
  | |                    |                         | 32-bit mode or A, B and | |
  | |                    |                         | C, D in the 16-bit mode | |
  | |                    |                         | represent, each, one    | |
  | |                    |                         | clock pulse gate circuit| |
  | |                    |                         | There by frequency and  | |
  | |                    |                         | pulse duration          | |
  | |                    |                         | measurements can be     | |
  | |                    |                         | performed.              | |
  | +--------------------+-------------------------+-------------------------+ |
  |                                                                            |
  |                                                                            |
  |       IMPORTANT!                                                           |
  |       If you have configured the module for two 16-bit counter, a mixed    |
  |       mode with a counter in quadruple/double/single mode                  |
  |       and the other counter in direct mode is not possible!                |
  |                                                                            |
  |                                                                            |
  |         Counter operating option for quadruple/double/simple mode          |
  |         ---------------------------------------------------------          |
  |                                                                            |
  | +----------------------+-------------------------+------------------------+|
  | |       Parameter      |     Passed value        |  Description           ||
  | |----------------------+-------------------------+------------------------||
  | |b_FirstCounterOption  | APCI1710_HYSTERESIS_ON  | In both edge analysis  ||
  | |        or            |                         | circuits is available  ||
  | |b_SecondCounterOption |                         | one hysteresis circuit.||
  | |                      |                         | It suppresses each     ||
  | |                      |                         | time the first counting||
  | |                      |                         | pulse after a change   ||
  | |                      |                         | of rotation.           ||
  | |----------------------+-------------------------+------------------------||
  | |b_FirstCounterOption  | APCI1710_HYSTERESIS_OFF | The first counting     ||
  | |       or             |                         | pulse is not suppress  ||
  | |b_SecondCounterOption |                         | after a change of      ||
  | |                      |                         | rotation.              ||
  | +----------------------+-------------------------+------------------------+|
  |                                                                            |
  |                                                                            |
  |       IMPORTANT!                                                           |
  |       This option are only avaible if you have selected the direct mode.   |
  |                                                                            |
  |                                                                            |
  |               Counter operating option for direct mode                     |
  |               ----------------------------------------                     |
  |                                                                            |
  | +----------------------+--------------------+----------------------------+ |
  | |      Parameter       |     Passed value   |       Description          | |
  | |----------------------+--------------------+----------------------------| |
  | |b_FirstCounterOption  | APCI1710_INCREMENT | The counter increment for  | |
  | |       or             |                    | each counting pulse        | |
  | |b_SecondCounterOption |                    |                            | |
  | |----------------------+--------------------+----------------------------| |
  | |b_FirstCounterOption  | APCI1710_DECREMENT | The counter decrement for  | |
  | |       or             |                    | each counting pulse        | |
  | |b_SecondCounterOption |                    |                            | |
  | +----------------------+--------------------+----------------------------+ |
  |                                                                            |
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE  b_BoardHandle         : Handle of board APCI-1710|
  |                     BYTE  b_ModulNbr            : Module number to         |
  |                                                   configure (0 to 3)       |
  |                     BYTE  b_CounterRange        : Selection form counter   |
  |                                                   range.                   |
  |                     BYTE  b_FirstCounterModus   : First counter operating  |
  |                                                   mode.                    |
  |                     BYTE  b_FirstCounterOption  : First counter  option.   |
  |                     BYTE  b_SecondCounterModus  : Second counter operating |
  |                                                   mode.                    |
  |                     BYTE  b_SecondCounterOption : Second counter  option.  |
  +----------------------------------------------------------------------------+
  | Output Parameters : -                                                      |
  +----------------------------------------------------------------------------+
  | Return Value      : 0: No error                                            |
  |                    -1: The handle parameter of the board is wrong          |
  |                    -2: The module is not a counter module                  |
  |                    -3: The selected counter range is wrong.                |
  |                    -4: The selected first counter operating mode is wrong. |
  |                    -5: The selected first counter operating option is wrong|
  |                    -6: The selected second counter operating mode is wrong.|
  |                    -7: The selected second counter operating option is     |
  |                        wrong.                                              |
  |                    -8: The parametered module number is wrong              | 
  +----------------------------------------------------------------------------+
*/

INT     i_APCI1710_InitCounter (BYTE           b_BoardHandle,
				BYTE           b_ModuleNumber,
				BYTE           b_CounterRange,
				BYTE           b_FirstCounterModus,
				BYTE           b_FirstCounterOption,
				BYTE           b_SecondCounterModus,
				BYTE           b_SecondCounterOption)
{
  int   i_ReturnValue         = 0;
   
  /*******************************************************/
  /* Gets the pointer to the board information structure */
  /*******************************************************/
  str_APCI1710_BoardInformation *BoardInformation = NULL; 
  
  BoardInformation = &(s_APCI1710_DriverStruct.
		       s_BoardInformation[b_BoardHandle]);

  /****************************************/
  /* Checks if the b_BoardHandle is valid */
  /****************************************/
  if (b_BoardHandle < s_APCI1710_DriverStruct.b_NumberOfBoard)
    {   
      /*************************************/
      /* Checks if the b_ModuleNumber is valid */
      /*************************************/
      if (b_ModuleNumber <= 3) 
	{
	  /******************************************/
	  /* Checks the configuration of the Module */
	  /******************************************/
	  if (BoardInformation -> dw_MolduleConfiguration [b_ModuleNumber] == APCI1710_INCREMENTAL_COUNTER)
	    {
	      /**************************/
	      /* Test the counter range */
	      /**************************/

	      if (b_CounterRange == APCI1710_16BIT_COUNTER || 
		  b_CounterRange == APCI1710_32BIT_COUNTER)
		{
		  /********************************/
		  /* Test the first counter modus */
		  /********************************/

		  if (b_FirstCounterModus == APCI1710_QUADRUPLE_MODE ||
		      b_FirstCounterModus == APCI1710_DOUBLE_MODE    ||
		      b_FirstCounterModus == APCI1710_SIMPLE_MODE    ||
		      b_FirstCounterModus == APCI1710_DIRECT_MODE)
		    {
		      /*********************************/
		      /* Test the first counter option */
		      /*********************************/

		      if ((b_FirstCounterModus   == APCI1710_DIRECT_MODE   &&
			   (b_FirstCounterOption == APCI1710_INCREMENT     ||
			    b_FirstCounterOption == APCI1710_DECREMENT))   ||
			  (b_FirstCounterModus   != APCI1710_DIRECT_MODE   &&
			   (b_FirstCounterOption == APCI1710_HYSTERESIS_ON ||
			    b_FirstCounterOption == APCI1710_HYSTERESIS_OFF)))
			{
			  /**************************/
			  /* Test if 16-bit counter */
			  /**************************/

			  if (b_CounterRange == APCI1710_16BIT_COUNTER)
			    {
			      /*********************************/
			      /* Test the second counter modus */
			      /*********************************/

			      if ((b_FirstCounterModus != APCI1710_DIRECT_MODE      &&
				   (b_SecondCounterModus == APCI1710_QUADRUPLE_MODE ||
				    b_SecondCounterModus == APCI1710_DOUBLE_MODE    ||
				    b_SecondCounterModus == APCI1710_SIMPLE_MODE))  ||
				  (b_FirstCounterModus == APCI1710_DIRECT_MODE &&
				   b_SecondCounterModus == APCI1710_DIRECT_MODE))
				{
				  /**********************************/
				  /* Test the second counter option */
				  /**********************************/

				  if ((b_SecondCounterModus   == APCI1710_DIRECT_MODE   &&
				       (b_SecondCounterOption == APCI1710_INCREMENT     ||
					b_SecondCounterOption == APCI1710_DECREMENT))   ||
				      (b_SecondCounterModus   != APCI1710_DIRECT_MODE   &&
				       (b_SecondCounterOption == APCI1710_HYSTERESIS_ON ||
					b_SecondCounterOption == APCI1710_HYSTERESIS_OFF)))
				    {
				      i_ReturnValue = 0;
				    }
				  else
				    {
				      /*********************************************************/
				      /* The selected second counter operating option is wrong */
				      /*********************************************************/

				      i_ReturnValue = -7;
				    }
				}
			      else
				{
				  /*******************************************************/
				  /* The selected second counter operating mode is wrong */
				  /*******************************************************/

				  i_ReturnValue = -6;
				}
			    }
			}
		      else
			{
			  /********************************************************/
			  /* The selected first counter operating option is wrong */
			  /********************************************************/

			  i_ReturnValue = -5;
			}
		    }
		  else
		    {
		      /******************************************************/
		      /* The selected first counter operating mode is wrong */
		      /******************************************************/

		      i_ReturnValue = -4;
		    }
		}
	      else
		{
		  /***************************************/
		  /* The selected counter range is wrong */
		  /***************************************/

		  i_ReturnValue = -3;
		}

	      /*************************/
	      /* Test if a error occur */
	      /*************************/

	      if (i_ReturnValue == 0)
		{
		  /**************************/
		  /* Test if 16-Bit counter */
		  /**************************/

		  if (b_CounterRange == APCI1710_32BIT_COUNTER)
		    {
		      s_APCI1710_DriverStruct.s_BoardInformation[b_BoardHandle].
			s_ModuleInformation[b_ModuleNumber].
			s_Inc_CptInformation.s_ModeRegister.
			s_ByteModeRegister.b_ModeRegister1 = 
			b_CounterRange      |
			b_FirstCounterModus |
			b_FirstCounterOption;
		    }
		  else
		    {
		      s_APCI1710_DriverStruct.s_BoardInformation[b_BoardHandle].
			s_ModuleInformation[b_ModuleNumber].
			s_Inc_CptInformation.s_ModeRegister.
			s_ByteModeRegister.b_ModeRegister1 = 
			b_CounterRange                |
			(b_FirstCounterModus   & 0x5)  |
			(b_FirstCounterOption  & 0x20) |
			(b_SecondCounterModus  & 0xA)  |
			(b_SecondCounterOption & 0x40);

		      /***********************/
		      /* Test if direct mode */
		      /***********************/

		      if (b_FirstCounterModus == APCI1710_DIRECT_MODE)
			{
			  s_APCI1710_DriverStruct.s_BoardInformation[b_BoardHandle].
			    s_ModuleInformation[b_ModuleNumber].
			    s_Inc_CptInformation.s_ModeRegister.
			    s_ByteModeRegister.b_ModeRegister1 = 
			    s_APCI1710_DriverStruct.s_BoardInformation[b_BoardHandle].
			    s_ModuleInformation[b_ModuleNumber].
			    s_Inc_CptInformation.s_ModeRegister.s_ByteModeRegister.
			    b_ModeRegister1 | APCI1710_DIRECT_MODE;
			}
		    }

		  /***************************/
		  /* Write the configuration */
		  /***************************/

		  outl (s_APCI1710_DriverStruct.s_BoardInformation[b_BoardHandle].
			s_ModuleInformation[b_ModuleNumber].
			s_Inc_CptInformation.s_ModeRegister.dw_ModeRegister1_2_3_4,
			BoardInformation -> s_BaseInformation.
			ui_BaseAddress[2] + 
			20 + (64 * b_ModuleNumber));
					 
					 
		  printk("Write config: %lX", s_APCI1710_DriverStruct.s_BoardInformation[b_BoardHandle].
			 s_ModuleInformation[b_ModuleNumber].
			 s_Inc_CptInformation.s_ModeRegister.dw_ModeRegister1_2_3_4);					 

		  s_APCI1710_DriverStruct.s_BoardInformation[b_BoardHandle].
		    s_ModuleInformation[b_ModuleNumber].
		    s_Inc_CptInformation.b_CounterInit = 1;
		}
	    }
	  else
	    {
	      /**************************************/
	      /* The module is not a counter module */
	      /**************************************/

	      i_ReturnValue = -2;
	    }
	}
      else
	{
	  /******************************************/
	  /* The parametered module number is wrong */
	  /******************************************/

	  i_ReturnValue = -8;
	}	 
    }
  else
    {
      /***********************/
      /* Wrong b_BoardHandle */
      /***********************/
      i_ReturnValue = -1;      
    }      
  return  i_ReturnValue;
}

/*
  +----------------------------------------------------------------------------+
  | Function Name     : INT   i_APCI1710_ClearCounterValue                     |
  |                               (BYTE       b_BoardHandle,                   |
  |                                BYTE        b_ModulNbr)                     |
  +----------------------------------------------------------------------------+
  | Task              : Clear the counter value from selected module           |
  |                     (b_ModulNbr).                                          |
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE  b_BoardHandle : Handle of board APCI-1710        |
  |                     BYTE  b_ModulNbr    : Module number to configure       |
  |                                           (0 to 3)                         |
  +----------------------------------------------------------------------------+
  | Output Parameters : -                                                      |
  +----------------------------------------------------------------------------+
  | Return Value      :  0: No error                                           |
  |                     -1: The handle parameter of the board is wrong         |
  |                     -2: The selected module number parameter is wrong      |
  |                     -3: Counter not initialised see function               |
  |                         "i_APCI1710_InitCounter"                           |
  |                     -4: Counter not initialised as Incremental encoder     |
  +----------------------------------------------------------------------------+
*/

INT     i_APCI1710_ClearCounterValue    (BYTE  b_BoardHandle,
					 BYTE  b_ModuleNumber)
{
  int   i_ReturnValue         = 0;
   
  /*******************************************************/
  /* Gets the pointer to the board information structure */
  /*******************************************************/
  str_APCI1710_BoardInformation *BoardInformation = NULL; 
  
  BoardInformation = &(s_APCI1710_DriverStruct.
		       s_BoardInformation[b_BoardHandle]);

  /****************************************/
  /* Checks if the b_BoardHandle is valid */
  /****************************************/
  if (b_BoardHandle < s_APCI1710_DriverStruct.b_NumberOfBoard)
    {   
      /*****************************************/
      /* Checks if the b_ModuleNumber is valid */
      /*****************************************/
      if (b_ModuleNumber <= 3) 
	{
	  /******************************************/
	  /* Checks the configuration of the Module */
	  /******************************************/
	  if (BoardInformation -> dw_MolduleConfiguration [b_ModuleNumber] == APCI1710_INCREMENTAL_COUNTER)
	    {
	      /*******************************/
	      /* Test if counter initialised */
	      /*******************************/

	      if (s_APCI1710_DriverStruct.s_BoardInformation[b_BoardHandle].
		  s_ModuleInformation[b_ModuleNumber].
		  s_Inc_CptInformation.b_CounterInit == 1)
		{
		  /*********************/
		  /* Clear the counter */
		  /*********************/


		  outl (1,
			BoardInformation -> s_BaseInformation.
			ui_BaseAddress[2] + 
			16 + (64 * b_ModuleNumber));
		}
	      else
		{
		  /****************************************/
		  /* Counter not initialised see function */
		  /* "i_APCI1710_InitCounter"             */
		  /****************************************/

		  i_ReturnValue = -3;
		}
	    }
	  else
	    {
	      /**************************************************/
	      /* The selected module not initialized as Inc_CPT */
	      /**************************************************/

	      i_ReturnValue = -4;
	    }
	}
      else
	{
	  /*************************************************/
	  /* The selected module number parameter is wrong */
	  /*************************************************/

	  i_ReturnValue = -2;
	}
    }
  else
    {
      /***********************/
      /* Wrong b_BoardHandle */
      /***********************/
      i_ReturnValue = -1;      
    }      
  return  i_ReturnValue;
}

/*
  +----------------------------------------------------------------------------+
  | Function Name     :  INT  i_APCI1710_Read16BitCounterValue                 |
  |                                       (BYTE     b_BoardHandle,             |
  |                                        BYTE     b_ModulNbr,                |
  |                                        BYTE     b_SelectedCounter,         |
  |                                        PUINT   pui_CounterValue)           |
  +----------------------------------------------------------------------------+
  | Task              : Latch the selected 16-Bit counter (b_SelectedCounter)  |
  |                     from selected module (b_ModulNbr) in to the first      |
  |                     latch register and return the latched value.           |
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE  b_BoardHandle     : Handle of board APCI-1710    |
  |                     BYTE  b_ModulNbr        : Module number to configure   |
  |                                              (0 to 3)                      |
  |                     BYTE  b_SelectedCounter : Selected 16-Bit counter      |
  |                                               (0 or 1)                     |
  +----------------------------------------------------------------------------+
  | Output Parameters : PUINT  pui_CounterValue : 16-Bit counter value         |
  +----------------------------------------------------------------------------+
  | Return Value      :  0: No error                                           |
  |                     -1: The handle parameter of the board is wrong         |
  |                     -2: No counter module found                            |
  |                     -3: Counter not initialised see function               |
  |                         "i_APCI1710_InitCounter"                           |
  |                     -4: The selected 16-Bit counter parameter is wrong     |
  +----------------------------------------------------------------------------+
*/

INT   i_APCI1710_Read16BitCounterValue        (BYTE    b_BoardHandle,
					       BYTE    b_ModuleNumber,
					       BYTE    b_SelectedCounter,
					       PUINT pui_CounterValue)
{
  INT    i_ReturnValue = 0;
  DWORD dw_LathchValue = 0;
  /*******************************************************/
  /* Gets the pointer to the board information structure */
  /*******************************************************/
  str_APCI1710_BoardInformation *BoardInformation = NULL; 
  
  BoardInformation = &(s_APCI1710_DriverStruct.
		       s_BoardInformation[b_BoardHandle]);

  /****************************************/
  /* Checks if the b_BoardHandle is valid */
  /****************************************/
  if (b_BoardHandle < s_APCI1710_DriverStruct.b_NumberOfBoard)
    {   
      /*****************************************/
      /* Checks if the b_ModuleNumber is valid */
      /*****************************************/
      if (b_ModuleNumber <= 3) 
	{
	  /******************************************/
	  /* Checks the configuration of the Module */
	  /******************************************/
	  if (BoardInformation -> dw_MolduleConfiguration [b_ModuleNumber] == APCI1710_INCREMENTAL_COUNTER)
	    {
	      /*******************************/
	      /* Test if counter initialised */
	      /*******************************/

	      if (s_APCI1710_DriverStruct.s_BoardInformation[b_BoardHandle].
		  s_ModuleInformation[b_ModuleNumber].
		  s_Inc_CptInformation.b_CounterInit == 1)
		{
		  /******************************/
		  /* Test the counter selection */
		  /******************************/

		  if (b_SelectedCounter < 2)
		    {
		      /*********************/
		      /* Latch the counter */
		      /*********************/
		      outl (1,
			    BoardInformation -> s_BaseInformation.
			    ui_BaseAddress[2] + 
			    (64 * b_ModuleNumber));

		      /************************/
		      /* Read the latch value */
		      /************************/
		
		      dw_LathchValue = inl (BoardInformation -> s_BaseInformation.
					    ui_BaseAddress[2] + 
					    4 + (64 * b_ModuleNumber));			

		      *pui_CounterValue = (UINT) ((dw_LathchValue >> (16 * b_SelectedCounter)) & 0xFFFFU);
		    }
		  else
		    {
		      /**************************************************/
		      /* The selected 16-Bit counter parameter is wrong */
		      /**************************************************/

		      i_ReturnValue = -4;
		    }
		}
	      else
		{
		  /****************************************/
		  /* Counter not initialised see function */
		  /* "i_APCI1710_InitCounter"             */
		  /****************************************/

		  i_ReturnValue = -3;
		}
	    }
	  else
	    {
	      /*************************************************/
	      /* The selected module number parameter is wrong */
	      /*************************************************/

	      i_ReturnValue = -2;
	    }
	}
    }
  else
    {
      /***************************************/
      /* The board handle parameter is wrong */
      /***************************************/

      i_ReturnValue = -1;
    }
  return (i_ReturnValue);
}

/*
  +----------------------------------------------------------------------------+
  | Function Name     : INT   i_APCI1710_Read32BitCounterValue                 |
  |                                       (BYTE        b_BoardHandle,          |
  |                                        BYTE        b_ModulNbr,             |
  |                                        PULONG_ pul_CounterValue)           |
  +----------------------------------------------------------------------------+
  | Task              : Latch the 32-Bit counter from selected module          |
  |                     (b_ModulNbr) in to the first latch register and return |
  |                     the latched value.                                     |
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE    b_BoardHandle     : Handle of board APCI-1710  |
  |                     BYTE    b_ModulNbr        : Module number to configure |
  |                                              (0 to 3)                      |
  +----------------------------------------------------------------------------+
  | Output Parameters : PULONG   pul_CounterValue : 32-Bit counter value       |
  +----------------------------------------------------------------------------+
  | Return Value      :  0: No error                                           |
  |                     -1: The handle parameter of the board is wrong         |
  |                     -2: The modul number is wrong                          |
  |                     -3: No counter module found                            |
  |                     -4: Counter not initialised see function               |
  |                         "i_APCI1710_InitCounter"                           |
  +----------------------------------------------------------------------------+
*/

INT     i_APCI1710_Read32BitCounterValue        (BYTE      b_BoardHandle,
						 BYTE      b_ModuleNumber,
						 PULONG  pul_CounterValue)
{
  int   i_ReturnValue         = 0;
   
  /*******************************************************/
  /* Gets the pointer to the board information structure */
  /*******************************************************/
  str_APCI1710_BoardInformation *BoardInformation = NULL; 
  
  BoardInformation = &(s_APCI1710_DriverStruct.
		       s_BoardInformation[b_BoardHandle]);

  /****************************************/
  /* Checks if the b_BoardHandle is valid */
  /****************************************/
  if (b_BoardHandle < s_APCI1710_DriverStruct.b_NumberOfBoard)
    {   
      /*****************************************/
      /* Checks if the b_ModuleNumber is valid */
      /*****************************************/
      if (b_ModuleNumber <= 3) 
	{
	  /******************************************/
	  /* Checks the configuration of the Module */
	  /******************************************/
	  if (BoardInformation -> dw_MolduleConfiguration [b_ModuleNumber] == APCI1710_INCREMENTAL_COUNTER)
	    {
	      /*******************************/
	      /* Test if counter initialised */
	      /*******************************/

	      if (s_APCI1710_DriverStruct.s_BoardInformation[b_BoardHandle].
		  s_ModuleInformation[b_ModuleNumber].
		  s_Inc_CptInformation.b_CounterInit == 1)
		{
		  /*********************/
		  /* Tatch the counter */
		  /*********************/

		  outl (1,
			BoardInformation -> s_BaseInformation.
			ui_BaseAddress[2] + 
			(64 * b_ModuleNumber));

		  /************************/
		  /* Read the latch value */
		  /************************/
		                 
		  *pul_CounterValue = inl (BoardInformation -> s_BaseInformation.
					   ui_BaseAddress[2] + 4 +
					   (64 * b_ModuleNumber));
		}
	      else
		{
		  /****************************************/
		  /* Counter not initialised see function */
		  /* "i_APCI1710_InitCounter"             */
		  /****************************************/

		  i_ReturnValue = -4;
		}
	    }
	  else
	    {
	      /*************************************/
	      /* The selected module is no Inc_Cpt */
	      /*************************************/

	      i_ReturnValue = -3;
	    }
	}
      else
	{
	  /*************************************************/
	  /* The selected module number parameter is wrong */
	  /*************************************************/

	  i_ReturnValue = -2;
	}
    }
  else
    {
      /***********************/
      /* Wrong b_BoardHandle */
      /***********************/
      i_ReturnValue = -1;      
    }      
  return  i_ReturnValue;
}

/*
  +----------------------------------------------------------------------------+
  | Function   Name   : INT     i_APCI1710_InitDigitalIO                       |
  |                                               (BYTE     b_BoardHandle,     |
  |                                                BYTE     b_ModuleNumber,        |
  |                                                BYTE     b_ChannelAMode,    |
  |                                                BYTE     b_ChannelBMode)    |
  +----------------------------------------------------------------------------+
  | Task              : Configurated the digital I/O operating mode from the   | 
  |                     selected module. You must call this function before you| 
  |                     call other functions which acesses the digital I/O port| 
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE   b_BoardHandle  : Handle of the APCI-1710        |
  |                     BYTE   b_ModuleNumber     : Module number to configure     |
  |                                             (0 - 3)                        |
  |                     BYTE   b_ChannelAMode : Channel A Mode selection:      |
  |                                             0: Channel is used for digital |
  |                                                input                       |
  |                                             1: Channel is used for digital |
  |                                                output                      |
  |                     BYTE   b_ChannelBMode : Channel B Mode selection:      |
  |                                             0: Channel is used for digital |
  |                                                input                       |
  |                                             1: Channel is used for digital |
  |                                                output                      |
  +----------------------------------------------------------------------------+
  | Output Parameters : ---                                                    |
  +----------------------------------------------------------------------------+
  | Return Value      :  0 : No error                                          |
  |                     -1 : The handle parameter of the board is wrong        |
  |                     -2 : The parametered module number is wrong            | 
  |                     -3 : The module is not a digital I/O module            | 
  |                     -4 : Bi-directional channel A configuration error      | 
  |                     -5 : Bi-directional channel B configuration error      | 
  +----------------------------------------------------------------------------+
*/
INT i_APCI1710_InitDigitalIO (BYTE b_BoardHandle,
                              BYTE b_ModuleNumber,
                              BYTE b_ChannelAMode,
                              BYTE b_ChannelBMode)
{
  int   i_ReturnValue        = 0;
  UINT ui_WriteConfig        = 0; 
   
  /*******************************************************/
  /* Gets the pointer to the board information structure */
  /*******************************************************/
  str_APCI1710_BoardInformation *BoardInformation = NULL; 
  
  BoardInformation = &(s_APCI1710_DriverStruct.
		       s_BoardInformation[b_BoardHandle]);

  /****************************************/
  /* Checks if the b_BoardHandle is valid */
  /****************************************/
  if (b_BoardHandle < s_APCI1710_DriverStruct.b_NumberOfBoard)
    {   
      /*************************************/
      /* Checks if the b_ModuleNumber is valid */
      /*************************************/
      if (b_ModuleNumber <= 3) 
	{
	  /******************************************/
	  /* Checks the configuration of the Module */
	  /******************************************/
	  if (BoardInformation -> dw_MolduleConfiguration [b_ModuleNumber] == APCI1710_DIGITAL_IO)
	    {
	      /************************************************************/
	      /* Checks the configuration of the bi-directional channel A */
	      /************************************************************/
	      if ((b_ChannelAMode == 0) || (b_ChannelAMode ==1))
		{
		  /************************************************************/
		  /* Checks the configuration of the bi-directional channel B */
		  /************************************************************/
		  if ((b_ChannelBMode == 0) || (b_ChannelBMode ==1))
		    {
	       
		      /****************************************/
		      /* Flag for the Init of the digital I/O */
		      /****************************************/
		      s_APCI1710_DriverStruct.s_BoardInformation[b_BoardHandle].
			s_ModuleInformation[b_ModuleNumber].
			s_DigitalIOInformation.b_DigitalInit = 1;

		      /*************************************/
		      /* Saves the channel A configuration */
		      /*************************************/
		      s_APCI1710_DriverStruct.s_BoardInformation[b_BoardHandle].
			s_ModuleInformation[b_ModuleNumber].
			s_DigitalIOInformation.b_ChannelAMode = b_ChannelAMode;
		  
		      /*************************************/
		      /* Saves the channel B configuration */
		      /*************************************/
		      s_APCI1710_DriverStruct.s_BoardInformation[b_BoardHandle].
			s_ModuleInformation[b_ModuleNumber].
			s_DigitalIOInformation.b_ChannelBMode = b_ChannelBMode;
		  
		      /******************************************/
		      /* Sets the channel A and B configuration */
		      /******************************************/
		      ui_WriteConfig = (UINT) (b_ChannelAMode | 
					       (b_ChannelBMode * 2));		  
                  
		      /****************************/
		      /* Writes the configuration */
		      /****************************/
		      outl (ui_WriteConfig,
			    BoardInformation -> s_BaseInformation.
			    ui_BaseAddress[2] + 
			    4 + (64 * b_ModuleNumber));
	          
		    }
		  else
		    {
		      /******************************************************/
		      /* Bi-directional configuration of channel B is wrong */
		      /******************************************************/
		      i_ReturnValue = -5;
		    }
		}
	      else
		{
		  /******************************************************/
		  /* Bi-directional configuration of channel A is wrong */               
		  /******************************************************/
		  i_ReturnValue = -4;
		}
	    }
	  else
	    {
	      /*****************************************/
	      /* The Module is not a digitalI/O Module */               
	      /*****************************************/
	      i_ReturnValue = -3;
	    }
	}
      else
	{
	  /******************************************/
	  /* The Module is not a digital I/O Module */
	  /******************************************/
	  i_ReturnValue = -2;
	}
    }
  else
    {
      /***********************/
      /* Wrong b_BoardHandle */
      /***********************/
      i_ReturnValue = -1;      
    }      
  return  i_ReturnValue;
}

/*
  +----------------------------------------------------------------------------+
  | Function   Name   : INT     i_APCI1710_Read1DigitalInput                   |
  |                                               (BYTE     b_BoardHandle,     |
  |                                                BYTE     b_ChannelNumber,   |
  |                                                PBYTE   pb_InputValue)      |
  +----------------------------------------------------------------------------+
  | Task              : Reads the status of the digital input                  | 
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE   b_BoardHandle  : Handle of the APCI-1710        |
  |                     BYTE   b_ChannelNumber: Digital channel to read        |
  +----------------------------------------------------------------------------+
  | Output Parameters : PBYTE pb_InputValue   : Status of the digital input:   |
  |                                             "0" => low; "1" => high        |
  +----------------------------------------------------------------------------+
  | Return Value      :  0 : No error                                          |
  |                     -1 : The handle parameter of the board is wrong        |
  |                     -2 : The parametered channel number is wrong           |
  +----------------------------------------------------------------------------+
*/
INT i_APCI1710_Read1DigitalInput ( BYTE  b_BoardHandle,
                                   BYTE  b_ModuleNumber,
                                   BYTE  b_InputChannel,
				   PBYTE pb_ChannelStatus)
{
  int   i_ReturnValue         = 0;
  UINT ui_StatusRegister      = 0;
   
  /*******************************************************/
  /* Gets the pointer to the board information structure */
  /*******************************************************/
  str_APCI1710_BoardInformation *BoardInformation = NULL; 
  
  BoardInformation = &(s_APCI1710_DriverStruct.
		       s_BoardInformation[b_BoardHandle]);

  /****************************************/
  /* Checks if the b_BoardHandle is valid */
  /****************************************/
  if (b_BoardHandle < s_APCI1710_DriverStruct.b_NumberOfBoard)
    {   
      /*************************************/
      /* Checks if the b_ModuleNumber is valid */
      /*************************************/
      if (b_ModuleNumber <= 3) 
	{
	  /******************************************/
	  /* Checks the configuration of the Module */
	  /******************************************/
	  if (BoardInformation -> dw_MolduleConfiguration [b_ModuleNumber] == APCI1710_DIGITAL_IO)
	    {
	      /*********************************/
	      /* Checks the given input number */
	      /*********************************/
	      if (b_InputChannel <= 6)
		{
		  /***************************************************/
		  /* Checks if the digital IO Module are initialised */
		  /***************************************************/
		  if (s_APCI1710_DriverStruct.s_BoardInformation[b_BoardHandle].
		      s_ModuleInformation[b_ModuleNumber].
		      s_DigitalIOInformation.b_DigitalInit == 1)
		    {
		      /******************************************************/
		      /* Checks if the bi directional I/O channels are used */
		      /******************************************************/
		      if (b_InputChannel > 4)
			{
			  /******************************************************/
			  /* Checks if the bi directional I/O channel 5 is used */
			  /******************************************************/
			  if (b_InputChannel == 5)
			    {
			      /**************************************/
			      /* Checks the bi-directional I/O mode */
			      /**************************************/
			      if (s_APCI1710_DriverStruct.
				  s_BoardInformation[b_BoardHandle].
				  s_ModuleInformation[b_ModuleNumber].
				  s_DigitalIOInformation.b_ChannelAMode != 0)
				{
				  /*******************************************/
				  /* The bi-directional I/O mode is a output */
				  /*******************************************/
				  i_ReturnValue = -6;
				}    
			    }
			  /********************************************/
			  /* The bi directional I/O channel 6 is used */
			  /********************************************/
			  else
			    {
			      /**************************************/
			      /* Checks the bi-directional I/O mode */
			      /**************************************/
			      if (s_APCI1710_DriverStruct.
				  s_BoardInformation[b_BoardHandle].
				  s_ModuleInformation[b_ModuleNumber].
				  s_DigitalIOInformation.b_ChannelBMode != 0)
				{
				  /*******************************************/
				  /* The bi-directional I/O mode is a output */
				  /*******************************************/
				  i_ReturnValue = -7;
				}    
			    }
			}
		      /**********************************************/
		      /* If no error ocurrs, the input will be read */
		      /**********************************************/
		      if (i_ReturnValue == 0)
			{
			  ui_StatusRegister = inl (BoardInformation -> 
						   s_BaseInformation.
						   ui_BaseAddress[2] +
						   (64 * b_ModuleNumber));
					      
			  /*************************************************/
			  /* Returns the digital value of the wished input */
			  /*************************************************/
			  *pb_ChannelStatus = (BYTE) ((ui_StatusRegister ^ 0x1C)
						      >> b_InputChannel) & 0x1;
			}
		    }
		  else
		    {
		      /*****************************************/
		      /* Digital I/O Module is not initialised */
		      /*****************************************/
		      i_ReturnValue = -5;
		    }	   
	    
		}
	      else
		{
		  /*************************************/
		  /* The selected I/O channel is wrong */
		  /*************************************/
		  i_ReturnValue = -4;
		} 
	    }
	  else
	    {
	      /**********************************/
	      /* The Module is not a I/O Module */
	      /**********************************/
	      i_ReturnValue = -3;
	    }    
	}
      else
	{
	  /*********************************/
	  /* The Module parameter is wrong */
	  /*********************************/
	  i_ReturnValue = -2;
	}
    }
  else
    {
      /***********************/
      /* Wrong b_BoardHandle */
      /***********************/
      i_ReturnValue = -1;      
    }      
  return  i_ReturnValue;
}

/*
  +----------------------------------------------------------------------------+
  | Function   Name   : INT     i_APCI1710_Read4DigitalInput                   |
  |                                               (BYTE     b_BoardHandle,     |
  |                                                PBYTE   pb_PortValue)       |
  +----------------------------------------------------------------------------+
  | Task              : Reads the status of all digital inputs                 | 
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE   b_BoardHandle  : Handle of the APCI-1710        |
  |                                                                            |
  +----------------------------------------------------------------------------+
  | Output Parameters : PBYTE pb_InputValue   : Status of the digital input:   |
  |                                             "0"  => Port status : 0000     |
  |                                             "10" => Port status : 1010     |
  |                                             "15" => Port status : 1111     |
  +----------------------------------------------------------------------------+
  | Return Value      :  0 : No error                                          |
  |                     -1 : The handle parameter of the board is wrong        |
  +----------------------------------------------------------------------------+
*/
INT i_APCI1710_ReadAllDigitalInput (BYTE b_BoardHandle,
                                    BYTE b_ModuleNumber,
                                    PBYTE pb_PortValue)
{
  int   i_ReturnValue         = 0;
  UINT ui_StatusRegister      = 0;
   
  /*******************************************************/
  /* Gets the pointer to the board information structure */
  /*******************************************************/
  str_APCI1710_BoardInformation *BoardInformation = NULL; 
  
  BoardInformation = &(s_APCI1710_DriverStruct.
		       s_BoardInformation[b_BoardHandle]);

  /****************************************/
  /* Checks if the b_BoardHandle is valid */
  /****************************************/
  if (b_BoardHandle < s_APCI1710_DriverStruct.b_NumberOfBoard)
    {   
      /*************************************/
      /* Checks if the b_ModuleNumber is valid */
      /*************************************/
      if (b_ModuleNumber <= 3) 
	{
	  /******************************************/
	  /* Checks the configuration of the Module */
	  /******************************************/
	  if (BoardInformation -> dw_MolduleConfiguration [b_ModuleNumber] == APCI1710_DIGITAL_IO)
	    {
	      /***************************************************/
	      /* Checks if the digital IO Module are initialised */
	      /***************************************************/
	      if (s_APCI1710_DriverStruct.s_BoardInformation[b_BoardHandle].
		  s_ModuleInformation[b_ModuleNumber].
		  s_DigitalIOInformation.b_DigitalInit == 1)
		{
		  ui_StatusRegister = inl (BoardInformation -> 
					   s_BaseInformation.
					   ui_BaseAddress[2] +
					   (64 * b_ModuleNumber));
					      
		  /*****************************************/
		  /* Returns the digital value of the port */
		  /*****************************************/
		  *pb_PortValue = (BYTE) (ui_StatusRegister ^ 0x1C);
		}
	      else
		{
		  /*****************************************/
		  /* Digital I/O Module is not initialised */
		  /*****************************************/
		  i_ReturnValue = -4;
		}	   
	    }
	  else
	    {
	      /**********************************/
	      /* The Module is not a I/O Module */
	      /**********************************/
	      i_ReturnValue = -3;
	    } 
	}
      else
	{
	  /*********************************/
	  /* The Module parameter is wrong */
	  /*********************************/
	  i_ReturnValue = -2;
	}
    }
  else
    {
      /***********************/
      /* Wrong b_BoardHandle */
      /***********************/
      i_ReturnValue = -1;      
    }      
  return  i_ReturnValue;
}

/*
  +----------------------------------------------------------------------------+
  | Function   Name   : INT     i_APCI1710_SetOutputMemoryOn                   |
  |                                               (BYTE     b_BoardHandle,     |
  |                                                BYTE     b_ModuleNumber)     |
  +----------------------------------------------------------------------------+
  | Task              : Activates the output memory of the digital outputs     | 
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE   b_BoardHandle  : Handle of the APCI-1710        |
  |                     BYTE   b_ModuleNumber  : Selected Module number (0 - 3) |
  +----------------------------------------------------------------------------+
  | Output Parameters : ----                                                   |
  +----------------------------------------------------------------------------+
  | Return Value      :  0 : No error                                          |
  |                     -1 : The handle parameter of the board is wrong        |
  |                     -2 : The Module parameter is wrong                     |
  |                     -3 : The Module is not a digital I/O Module            |
  |                     -4 : Digital I/O is not initialised                    |
  +----------------------------------------------------------------------------+
*/
INT i_APCI1710_SetOutputMemoryOn (BYTE b_BoardHandle,
                                  BYTE b_ModuleNumber)
{
  int   i_ReturnValue      = 0;
   
  /*******************************************************/
  /* Gets the pointer to the board information structure */
  /*******************************************************/
  str_APCI1710_BoardInformation *BoardInformation = NULL; 
  
  BoardInformation = &(s_APCI1710_DriverStruct.
		       s_BoardInformation[b_BoardHandle]);

  /****************************************/
  /* Checks if the b_BoardHandle is valid */
  /****************************************/
  if (b_BoardHandle < s_APCI1710_DriverStruct.b_NumberOfBoard)
    {   
      /*************************************/
      /* Checks if the b_ModuleNumber is valid */
      /*************************************/
      if (b_ModuleNumber <= 3) 
	{
	  /******************************************/
	  /* Checks the configuration of the Module */
	  /******************************************/
	  if (BoardInformation -> dw_MolduleConfiguration [b_ModuleNumber] == APCI1710_DIGITAL_IO)
	    {
	      /***************************************************/
	      /* Checks if the digital IO Module are initialised */
	      /***************************************************/
	      if (s_APCI1710_DriverStruct.s_BoardInformation[b_BoardHandle].
		  s_ModuleInformation[b_ModuleNumber].
		  s_DigitalIOInformation.b_DigitalInit == 1)
		{
		  /*************************************/
		  /* Sets the digital output memory on */
		  /*************************************/
		  s_APCI1710_DriverStruct.s_BoardInformation[b_BoardHandle].
		    s_ModuleInformation[b_ModuleNumber].
		    s_DigitalIOInformation.b_DigitalOutputMemory = APCI1710_ENABLE;

		  /************************************/
		  /* Clears the digital output status */
		  /************************************/
		  s_APCI1710_DriverStruct.s_BoardInformation[b_BoardHandle].
		    s_ModuleInformation[b_ModuleNumber].
		    s_DigitalIOInformation.b_DigitalOutputStatus = APCI1710_DISABLE;
		}
	      else
		{
		  /*****************************************/
		  /* Digital I/O Module is not initialised */
		  /*****************************************/
		  i_ReturnValue = -4;
		}	   
	    }
	  else
	    {
	      /**********************************/
	      /* The Module is not a I/O Module */
	      /**********************************/
	      i_ReturnValue = -3;
	    } 
	}
      else
	{
	  /*********************************/
	  /* The Module parameter is wrong */
	  /*********************************/
	  i_ReturnValue = -2;
	}
    }
  else
    {
      /***********************/
      /* Wrong b_BoardHandle */
      /***********************/
      i_ReturnValue = -1;      
    }      
  return  i_ReturnValue;
}

/*
  +----------------------------------------------------------------------------+
  | Function   Name   : INT     i_APCI1710_SetOutputMemoryOff                  |
  |                                               (BYTE     b_BoardHandle,     |
  |                                                BYTE     b_ModuleNumber)     |
  +----------------------------------------------------------------------------+
  | Task              : Deactivates the output memory of the digital outputs   | 
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE   b_BoardHandle  : Handle of the APCI-1710        |
  |                     BYTE   b_ModuleNumber  : Selected Module number (0 - 3) |
  +----------------------------------------------------------------------------+
  | Output Parameters : ----                                                   |
  +----------------------------------------------------------------------------+
  | Return Value      :  0 : No error                                          |
  |                     -1 : The handle parameter of the board is wrong        |
  |                     -2 : The Module parameter is wrong                     |
  |                     -3 : The Module is not a digital I/O Module            |
  |                     -4 : Digital I/O is not initialised                    |
  +----------------------------------------------------------------------------+
*/
INT i_APCI1710_SetOutputMemoryOff (BYTE b_BoardHandle,
                                   BYTE b_ModuleNumber)
{
  int   i_ReturnValue      = 0;
  
  /*******************************************************/
  /* Gets the pointer to the board information structure */
  /*******************************************************/
  str_APCI1710_BoardInformation *BoardInformation = NULL; 
   
  BoardInformation = &(s_APCI1710_DriverStruct.
		       s_BoardInformation[b_BoardHandle]);

  /****************************************/
  /* Checks if the b_BoardHandle is valid */
  /****************************************/
  if (b_BoardHandle < s_APCI1710_DriverStruct.b_NumberOfBoard)
    {   
      /*************************************/
      /* Checks if the b_ModuleNumber is valid */
      /*************************************/
      if (b_ModuleNumber <= 3) 
	{
	  /******************************************/
	  /* Checks the configuration of the Module */
	  /******************************************/
          if (BoardInformation -> dw_MolduleConfiguration [b_ModuleNumber] == APCI1710_DIGITAL_IO)
	    {
	      /***************************************************/
	      /* Checks if the digital IO Module are initialised */
	      /***************************************************/
	      if (s_APCI1710_DriverStruct.s_BoardInformation[b_BoardHandle].
		  s_ModuleInformation[b_ModuleNumber].
		  s_DigitalIOInformation.b_DigitalInit == 1)
		{
		  /*************************************/
		  /* Sets the digital output memory on */
		  /*************************************/
		  s_APCI1710_DriverStruct.s_BoardInformation[b_BoardHandle].
		    s_ModuleInformation[b_ModuleNumber].
		    s_DigitalIOInformation.b_DigitalOutputMemory = APCI1710_DISABLE;

		  /************************************/
		  /* Clears the digital output status */
		  /************************************/
		  s_APCI1710_DriverStruct.s_BoardInformation[b_BoardHandle].
		    s_ModuleInformation[b_ModuleNumber].
		    s_DigitalIOInformation.b_DigitalOutputStatus = APCI1710_DISABLE;
		}
	      else
		{
		  /*****************************************/
		  /* Digital I/O Module is not initialised */
		  /*****************************************/
		  i_ReturnValue = -4;
		}	   
	    }
	  else
	    {
	      /**********************************/
	      /* The Module is not a I/O Module */
	      /**********************************/
	      i_ReturnValue = -3;
	    } 
	}
      else
	{
	  /*********************************/
	  /* The Module parameter is wrong */
	  /*********************************/
	  i_ReturnValue = -2;
	}
    }
  else
    {
      /***********************/
      /* Wrong b_BoardHandle */
      /***********************/
      i_ReturnValue = -1;      
    }      
  return  i_ReturnValue;
}

/*
  +----------------------------------------------------------------------------+
  | Function   Name   : INT     i_APCI1710_Set1DigitalOutput                   |
  |                                               (BYTE     b_BoardHandle,     |
  |                                                BYTE     b_ModuleNumber,    |
  |                                                BYTE     b_OutputValue,     |
  |                                                BYTE     b_OutputChannel)   |
  +----------------------------------------------------------------------------+
  | Task              : (Re)sets the output which has been parametered with the| 
  |                     variable b_OutputChannel. Setting an Output means      | 
  |                     setting the output high.                               | 
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE   b_BoardHandle  : Handle of the APCI-1710        |
  |                     BYTE   b_ModuleNumber  : Selected Module number (0 - 3)|
  |                     BYTE   b_OutputValue  : Selection if the digital output|
  |                                             should be set or reset:        |
  |                                                0: Channel will be reseted  |
  |                                                1: Channel will be seted    |
  |                     BYTE   b_OutputChannel: Selection of the digital output|
  |                                                0: Channel H                |
  |                                                1: Channel A                |
  |                                                2: Channel B                |
  +----------------------------------------------------------------------------+
  | Output Parameters : ----                                                   |
  +----------------------------------------------------------------------------+
  | Return Value      :  0 : No error                                          |
  |                     -1 : The handle parameter of the board is wrong        |
  |                     -2 : The Module parameter is wrong                     |
  |                     -3 : The Module is not a digital I/O Module            |
  |                     -4 : The selected output channel is wrong              |
  |                     -5 : Digital I/O is not initialised                    |
  |                     -6 : The digital I/O channel A is used as input        |
  |                     -7 : The digital I/O channel B is used as input        |
  |                     -8 : The parametered output value is wrong             |
  |                     -9 : The digital output memory is off                  |
  +----------------------------------------------------------------------------+
*/
INT i_APCI1710_Set1DigitalOutput (BYTE b_BoardHandle,
                                  BYTE b_ModuleNumber,
                                  BYTE b_OutputValue,
                                  BYTE b_OutputChannel)
{
  int   i_ReturnValue         = 0;
  UINT ui_WriteValue          = 0;
   
  /*******************************************************/
  /* Gets the pointer to the board information structure */
  /*******************************************************/
  str_APCI1710_BoardInformation *BoardInformation = NULL; 
  
  BoardInformation = &(s_APCI1710_DriverStruct.
		       s_BoardInformation[b_BoardHandle]);

  /****************************************/
  /* Checks if the b_BoardHandle is valid */
  /****************************************/
  if (b_BoardHandle < s_APCI1710_DriverStruct.b_NumberOfBoard)
    {   
      /*************************************/
      /* Checks if the b_ModuleNumber is valid */
      /*************************************/
      if (b_ModuleNumber <= 3) 
	{
	  /******************************************/
	  /* Checks the configuration of the Module */
	  /******************************************/
	  if (BoardInformation -> dw_MolduleConfiguration [b_ModuleNumber] == APCI1710_DIGITAL_IO)
	    {
	      /***************************************************/
	      /* Checks if the digital IO Module are initialised */
	      /***************************************************/
	      if (s_APCI1710_DriverStruct.s_BoardInformation[b_BoardHandle].
		  s_ModuleInformation[b_ModuleNumber].
		  s_DigitalIOInformation.b_DigitalInit == 1)
		{
		  /*******************************************/
		  /* Tests the digital output channel number */
		  /*******************************************/
		  switch (b_OutputChannel)
		    {
		      /************************************************/
		      /* Channel H is used (no configuration needed)  */
		      /************************************************/
		    case 0:
		      {
			break;
		      }

		      /*********************************************/
		      /* Channel A is used (configuration needed)  */
		      /*********************************************/
		    case 1:
		      {
			if (s_APCI1710_DriverStruct.
			    s_BoardInformation[b_BoardHandle].
			    s_ModuleInformation[b_ModuleNumber].
			    s_DigitalIOInformation.b_ChannelAMode != 1)
			  {
			    /*******************************/
			    /* Channel A is used for input */
			    /*******************************/
			    i_ReturnValue = -6;
			  } 	  
			break;
		      }

		      /*********************************************/
		      /* Channel B is used (configuration needed)  */
		      /*********************************************/
		    case 2:
		      {
			if (s_APCI1710_DriverStruct.
			    s_BoardInformation[b_BoardHandle].
			    s_ModuleInformation[b_ModuleNumber].
			    s_DigitalIOInformation.b_ChannelBMode != 1)
			  {
			    /*******************************/
			    /* Channel B is used for input */
			    /*******************************/
			    i_ReturnValue = -7;
			  } 	  
			break;
		      }
		  
		    default:
		      {
			/********************************/
			/* The selected output is wrong */ 
			/********************************/
			i_ReturnValue = -4;
			break;
		      }
		    }
		  /********************************/
		  /* The selected output is wrong */ 
		  /********************************/
		  if (!i_ReturnValue)
		    {
		      /***************************************/
		      /* Checks if the output memory is used */ 
		      /***************************************/
		      if (s_APCI1710_DriverStruct.
			  s_BoardInformation[b_BoardHandle].
			  s_ModuleInformation[b_ModuleNumber].
			  s_DigitalIOInformation.b_DigitalOutputMemory 
			  == APCI1710_ENABLE)
			{
			  /***************************************/
			  /* Checks the parametered output value */ 
			  /***************************************/
			  if (b_OutputValue <= 1)
			    {
			      /**************************************/
			      /* Checks if the output should be set */ 
			      /**************************************/
			      if (b_OutputValue == 1)
				{
				  /***************************************/
				  /* Saves the old stauts of the outputs */ 
				  /***************************************/
				  ui_WriteValue = s_APCI1710_DriverStruct.
				    s_BoardInformation[b_BoardHandle].
				    s_ModuleInformation[b_ModuleNumber].
				    s_DigitalIOInformation.
				    b_DigitalOutputStatus
				    | (1 << b_OutputChannel);
				  
				  /****************************************/
				  /* Writes the new status of the outputs */ 
				  /****************************************/
				  s_APCI1710_DriverStruct.
				    s_BoardInformation[b_BoardHandle].
				    s_ModuleInformation[b_ModuleNumber].
				    s_DigitalIOInformation.b_DigitalOutputStatus = 
				    ui_WriteValue;
				}
			      else
				{
				  /******************************/
				  /* The output should be reset */ 
				  /******************************/

				  /***************************************/
				  /* Saves the old status of the outputs */ 
				  /***************************************/
				  ui_WriteValue = s_APCI1710_DriverStruct.
				    s_BoardInformation[b_BoardHandle].
				    s_ModuleInformation[b_ModuleNumber].
				    s_DigitalIOInformation.
				    b_DigitalOutputStatus &
				    (0xFFFFFFFFUL - (1 << b_OutputChannel));
				  
				  /****************************************/
				  /* Writes the new status of the outputs */ 
				  /****************************************/
				  s_APCI1710_DriverStruct.
				    s_BoardInformation[b_BoardHandle].
				    s_ModuleInformation[b_ModuleNumber].
				    s_DigitalIOInformation.b_DigitalOutputStatus = 
				    ui_WriteValue;
			
				}
			    }
			  else 
			    {
			      /*****************************/
			      /* The output value is wrong */ 
			      /*****************************/
			      i_ReturnValue = -8;
			    }
			}  
		      /*********************************/
		      /* The output memory is not used */ 
		      /*********************************/
		      else
			{
			  /***************************************/
			  /* Checks the parametered output value */ 
			  /***************************************/
			  if (b_OutputValue <= 1)
			    {
			      /**************************************/
			      /* Checks if the output should be set */ 
			      /**************************************/
			      if (b_OutputValue == 1)
				{
				  /************************************/
				  /* Writes the status of the outputs */ 
				  /************************************/
				  ui_WriteValue = 1 << b_OutputChannel;
				}
			      else
				{
				  /************************************/
				  /* The output memory is not enabled */ 
				  /************************************/
				  i_ReturnValue = -9;
				}
			    }
			  else 
			    {
			      /*****************************/
			      /* The output value is wrong */ 
			      /*****************************/
			      i_ReturnValue = -8;
			    }
			}
		  
		      /******************************************/
		      /* Writes the digital outputs to the port */ 
		      /******************************************/
		      outl (ui_WriteValue,
			    BoardInformation -> s_BaseInformation.
			    ui_BaseAddress[2] + (64 * b_ModuleNumber));    
		    }	       
		}
	      else
		{
		  /*****************************************/
		  /* Digital I/O Module is not initialised */
		  /*****************************************/
		  i_ReturnValue = -5;
		}	   
	    }
	  else
	    {
	      /**********************************/
	      /* The Module is not a I/O Module */
	      /**********************************/
	      i_ReturnValue = -3;
	    } 
	}
      else
	{
	  /*********************************/
	  /* The Module parameter is wrong */
	  /*********************************/
	  i_ReturnValue = -2;
	}
    }
  else
    {
      /***********************/
      /* Wrong b_BoardHandle */
      /***********************/
      i_ReturnValue = -1;      
    }      
  return  i_ReturnValue;
}

/*
  +----------------------------------------------------------------------------+
  | Function   Name   : INT     i_APCI1710_SetAllDigitalOutput                 |
  |                                               (BYTE     b_BoardHandle,     |
  |                                                BYTE     b_ModuleNumber,    |
  |                                                BYTE     b_OutputValue,     |
  |                                                BYTE     b_PortValue)       |
  +----------------------------------------------------------------------------+
  | Task              : Sets or resets the outputs which are parametered       | 
  |                     with the variable b_PortValue. Setting an Output       | 
  |                     means setting the output high.                         | 
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE   b_BoardHandle  : Handle of the APCI-1710        |
  |                     BYTE   b_ModuleNumber  : Selected Module number (0 - 3)|
  |                     BYTE   b_OutputValue  : Selection if the digital output|
  |                                             should be set or reset:        |
  |                                                0: Channel will be reseted  |
  |                                                1: Channel will be seted    |
  |                     BYTE   b_PortValue    : Output Value ( 0 - 7 )         |
  +----------------------------------------------------------------------------+
  | Output Parameters : ----                                                   |
  +----------------------------------------------------------------------------+
  | Return Value      :  0 : No error                                          |
  |                     -1 : The handle parameter of the board is wrong        |
  |                     -2 : The Module parameter is wrong                     |
  |                     -3 : The Module is not a digital I/O Module            |
  |                     -4 : The selected port value is wrong                  |
 
  |                     -5 : Digital I/O is not initialised                    |
  |                     -6 : The digital I/O channel A is used as input        |
  |                     -7 : The digital I/O channel B is used as input        |
  |                     -8 : The parametered output value is wrong             |
  |                     -9 : The digital output memory is off                  |
  +----------------------------------------------------------------------------+
*/
INT i_APCI1710_SetAllDigitalOutput (BYTE b_BoardHandle,
                                    BYTE b_ModuleNumber,
                                    BYTE b_OutputValue,
                                    BYTE b_PortValue)
{
  int   i_ReturnValue         = 0;
  UINT ui_WriteValue          = 0;
   
  /*******************************************************/
  /* Gets the pointer to the board information structure */
  /*******************************************************/
  str_APCI1710_BoardInformation *BoardInformation = NULL; 
   
  BoardInformation = &(s_APCI1710_DriverStruct.
		       s_BoardInformation[b_BoardHandle]);

  /****************************************/
  /* Checks if the b_BoardHandle is valid */
  /****************************************/
  if (b_BoardHandle < s_APCI1710_DriverStruct.b_NumberOfBoard)
    {   
      /*************************************/
      /* Checks if the b_ModuleNumber is valid */
      /*************************************/
      if (b_ModuleNumber <= 3) 
	{
	  /******************************************/
	  /* Checks the configuration of the Module */
	  /******************************************/
	  if (BoardInformation -> dw_MolduleConfiguration [b_ModuleNumber] == APCI1710_DIGITAL_IO)
	    {
	      /***************************************************/
	      /* Checks if the digital IO Module are initialised */
	      /***************************************************/
	      if (s_APCI1710_DriverStruct.s_BoardInformation[b_BoardHandle].
		  s_ModuleInformation[b_ModuleNumber].
		  s_DigitalIOInformation.b_DigitalInit == 1)
		{
		  /************************/
		  /* Tests the port value */
		  /************************/
		  if (b_PortValue <= 7 )
		    {
		      /***********************************/
		      /* Test the digital output channel */
		      /***********************************/

		      /*****************************/
		      /* Test if channel A is used */
		      /*****************************/
		  
		      if ((b_PortValue & 2) == 2)
			{
			  if (s_APCI1710_DriverStruct.
			      s_BoardInformation[b_BoardHandle].
			      s_ModuleInformation[b_ModuleNumber].
			      s_DigitalIOInformation.b_ChannelAMode != 1)
			    {
			      /*******************************************/
			      /* The channel A is used for digital input */
			      /*******************************************/
			      i_ReturnValue = -6;
			    } 
			}
		  
		      /*****************************/
		      /* Test if channel B is used */
		      /*****************************/
		      if ((b_PortValue & 4) == 4)
			{
			  if (s_APCI1710_DriverStruct.
			      s_BoardInformation[b_BoardHandle].
			      s_ModuleInformation[b_ModuleNumber].
			      s_DigitalIOInformation.b_ChannelBMode != 1)
			    {
			      /*******************************************/
			      /* The channel B is used for digital input */
			      /*******************************************/
			      i_ReturnValue = -7;
			    } 
			}
		  
		      if (!i_ReturnValue)
			{
			  /***************************************/
			  /* Checks if the output memory is used */ 
			  /***************************************/
			  if (s_APCI1710_DriverStruct.
			      s_BoardInformation[b_BoardHandle].
			      s_ModuleInformation[b_ModuleNumber].
			      s_DigitalIOInformation.b_DigitalOutputMemory 
			      == APCI1710_ENABLE)
			    {
			      /***************************************/
			      /* Checks the parametered output value */ 
			      /***************************************/
			      if (b_OutputValue <= 1)
				{
				  /**************************************/
				  /* Checks if the output should be set */ 
				  /**************************************/
				  if (b_OutputValue == 1)
				    {
				      /***************************************/
				      /* Saves the old stauts of the outputs */ 
				      /***************************************/
				      ui_WriteValue = s_APCI1710_DriverStruct.
					s_BoardInformation[b_BoardHandle].
					s_ModuleInformation[b_ModuleNumber].
					s_DigitalIOInformation.
					b_DigitalOutputStatus
					| b_PortValue;
				  
				      /****************************************/
				      /* Writes the new status of the outputs */ 
				      /****************************************/
				      s_APCI1710_DriverStruct.
					s_BoardInformation[b_BoardHandle].
					s_ModuleInformation[b_ModuleNumber].
					s_DigitalIOInformation.b_DigitalOutputStatus = 
					ui_WriteValue;
				    }
				  else
				    {
				      /******************************/
				      /* The output should be reset */ 
				      /******************************/

				      /***************************************/
				      /* Saves the old status of the outputs */ 
				      /***************************************/
				      ui_WriteValue = s_APCI1710_DriverStruct.
					s_BoardInformation[b_BoardHandle].
					s_ModuleInformation[b_ModuleNumber].
					s_DigitalIOInformation.
					b_DigitalOutputStatus &
					(0xFFFFFFFFUL - b_PortValue);
				  
				      /****************************************/
				      /* Writes the new status of the outputs */ 
				      /****************************************/
				      s_APCI1710_DriverStruct.
					s_BoardInformation[b_BoardHandle].
					s_ModuleInformation[b_ModuleNumber].
					s_DigitalIOInformation.b_DigitalOutputStatus = 
					ui_WriteValue;
			
				    }
				}
			      else 
				{
				  /*****************************/
				  /* The output value is wrong */ 
				  /*****************************/
				  i_ReturnValue = -8;
				}
			    }  
			  /*********************************/
			  /* The output memory is not used */ 
			  /*********************************/
			  else
			    {
			      /***************************************/
			      /* Checks the parametered output value */ 
			      /***************************************/
			      if (b_OutputValue <= 1)
				{
				  /**************************************/
				  /* Checks if the output should be set */ 
				  /**************************************/
				  if (b_OutputValue == 1)
				    {
				      /************************************/
				      /* Writes the status of the outputs */ 
				      /************************************/
				      ui_WriteValue = b_PortValue;
				    }
				  else
				    {
				      /************************************/
				      /* The output memory is not enabled */ 
				      /************************************/
				      i_ReturnValue = -9;
				    }
				}
			      else 
				{
				  /*****************************/
				  /* The output value is wrong */ 
				  /*****************************/
				  i_ReturnValue = -8;
				}
			    }
		  
			  /******************************************/
			  /* Writes the digital outputs to the port */ 
			  /******************************************/
			  outl (ui_WriteValue,
				BoardInformation -> s_BaseInformation.
				ui_BaseAddress[2] + (64 * b_ModuleNumber));    
			}
		    }
		  else
		    {
		      /***************************/
		      /* The port value is wrong */ 
		      /***************************/
		      i_ReturnValue = -4;
		    }	  	       
		}
	      else
		{
		  /*****************************************/
		  /* Digital I/O Module is not initialised */
		  /*****************************************/
		  i_ReturnValue = -5;
		}	   
	    }
	  else
	    {
	      /**********************************/
	      /* The Module is not a I/O Module */
	      /**********************************/
	      i_ReturnValue = -3;
	    } 
	}
      else
	{
	  /*********************************/
	  /* The Module parameter is wrong */
	  /*********************************/
	  i_ReturnValue = -2;
	}
    }
  else
    {
      /***********************/
      /* Wrong b_BoardHandle */
      /***********************/
      i_ReturnValue = -1;      
    }      
  return  i_ReturnValue;
}


/*
  +----------------------------------------------------------------------------+
  | Function Name     :  INT  i_APCI1710_InitSSI                               |
  |                               (BYTE     b_BoardHandle,                     |
  |                                BYTE     b_ModulNbr,                        |
  |                                BYTE     b_SSIProfile,                      |
  |                                BYTE     b_PositionTurnLength,              |
  |                                BYTE     b_TurnCptLength,                   |
  |                                BYTE     b_PCIInputClock,                   |
  |                                ULONG   ul_SSIOutputClock,                  |
  |                                BYTE     b_SSICountingMode)                 |
  +----------------------------------------------------------------------------+
  | Task              : Configure the SSI operating mode from selected module  |
  |                     (b_ModulNbr). You must calling this function be for you|
  |                     call any other function witch access of SSI.           |
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE  b_BoardHandle         : Handle of board APCI-1710|
  |                     BYTE  b_ModulNbr            : Module number to         |
  |                                                   configure (0 to 3)       |
  |                     BYTE   b_SSIProfile         : Selection from SSI       |
  |                                                   profile length (2 to 32).|
  |                     BYTE   b_PositionTurnLength : Selection from SSI       |
  |                                                   position data length     |
  |                                                   (1 to 31).               |
  |                     BYTE   b_TurnCptLength      : Selection from SSI turn  |
  |                                                   counter data length      |
  |                                                   (1 to 31).               |
  |                     BYTE   b_PCIInputClock      : Selection from PCI bus   |
  |                                                   clock                    |
  |                                                 - APCI1710_30MHZ :         |
  |                                                   The PC have a PCI bus    |
  |                                                   clock from 30 MHz        |
  |                                                 - APCI1710_33MHZ :         |
  |                                                   The PC have a PCI bus    |
  |                                                   clock from 33 MHz        |
  |                     ULONG  ul_SSIOutputClock    : Selection from SSI output|
  |                                                   clock.                   |
  |                                                   From  229 to 5 000 000 Hz|
  |                                                   for 30 MHz selection.    |
  |                                                   From  252 to 5 000 000 Hz|
  |                                                   for 33 MHz selection.    |
  |                     BYTE   b_SSICountingMode    : SSI counting mode        |
  |                                                   selection                |
  |                                                 - APCI1710_BINARY_MODE :   |
  |                                                    Binary counting mode.   |
  |                                                 - APCI1710_GRAY_MODE :     |
  |                                                    Gray counting mode.     |
  +----------------------------------------------------------------------------+
  | Output Parameters : -                                                      |
  +----------------------------------------------------------------------------+
  | Return Value      : 0: No error                                            |
  |                    -1: The handle parameter of the board is wrong          |
  |                    -2: The module parameter is wrong                       |
  |                    -3: The module is not a SSI module                      |
  |                    -4: The selected SSI profile length is wrong            |
  |                    -5: The selected SSI position data length is wrong      |
  |                    -6: The selected SSI turn counter data length is wrong  |
  |                    -7: The selected PCI input clock is wrong               |
  |                    -8: The selected SSI output clock is wrong              |
  |                    -9: The selected SSI counting mode parameter is wrong   |
  +----------------------------------------------------------------------------+
*/
INT i_APCI1710_InitSSI (BYTE   b_BoardHandle,
			BYTE   b_ModuleNumber,
			BYTE   b_SSIProfile,
			BYTE   b_PositionTurnLength,
			BYTE   b_TurnCptLength,
			BYTE   b_PCIInputClock,
			ULONG ul_SSIOutputClock,
			BYTE   b_SSICountingMode)
{
  int   i_ReturnValue         = 0;
  UINT ui_TimerValue          = 0; 
   
  /*******************************************************/
  /* Gets the pointer to the board information structure */
  /*******************************************************/
  str_APCI1710_BoardInformation *BoardInformation = NULL; 
   
  BoardInformation = &(s_APCI1710_DriverStruct.
		       s_BoardInformation[b_BoardHandle]);

  /****************************************/
  /* Checks if the b_BoardHandle is valid */
  /****************************************/
  if (b_BoardHandle < s_APCI1710_DriverStruct.b_NumberOfBoard)
    {   
      /*****************************************/
      /* Checks if the b_ModuleNumber is valid */
      /*****************************************/
      if (b_ModuleNumber <= 3) 
	{
	  /******************************************/
	  /* Checks the configuration of the Module */
	  /******************************************/
	  if (BoardInformation -> dw_MolduleConfiguration [b_ModuleNumber] == APCI1710_SSI_COUNTER)
	    {
	      /*******************************/
	      /* Test the SSI profile lenght */
	      /*******************************/
	      if ((b_SSIProfile >= 2) && (b_SSIProfile < 33))
		{
		  /*************************************/
		  /* Test the SSI position data lenght */
		  /*************************************/
		  if ((b_PositionTurnLength > 0) && (b_PositionTurnLength < 32))
		    {
		      /*****************************************/
		      /* Test the SSI turn counter data lenght */
		      /*****************************************/
			if ((b_TurnCptLength > 0) && (b_TurnCptLength < 32)) 
			{
			  /***************************/
			  /* Test the profile length */
			  /***************************/
			  if ((b_TurnCptLength + b_PositionTurnLength) 
			      <= b_SSIProfile)
			    {
			      /****************************/
			      /* Test the PCI input Clock */
			      /****************************/
			      if ((b_PCIInputClock == APCI1710_30MHZ) || 
				  (b_PCIInputClock == APCI1710_33MHZ))
				{
				  /*************************/
				  /* Test the output clock */
				  /*************************/
				  if ((b_PCIInputClock == APCI1710_30MHZ &&
				       (ul_SSIOutputClock > 228 && ul_SSIOutputClock
					<= 5000000UL)) || 
				      (b_PCIInputClock == APCI1710_33MHZ &&
				       (ul_SSIOutputClock > 251 && ul_SSIOutputClock
					<= 5000000UL)))
				    {
				      /******************************/
				      /* Test the SSI counting mode */
				      /******************************/
				      if ((b_SSICountingMode == APCI1710_GRAY_MODE) ||
					  (b_SSICountingMode == APCI1710_BINARY_MODE))
					{
					  /**************************/
					  /* Save the configuration */
					  /**************************/

					  s_APCI1710_DriverStruct.
					    s_BoardInformation[b_BoardHandle].
					    s_ModuleInformation[b_ModuleNumber].
					    s_SSIInformation.b_SSIProfile = 
					    b_SSIProfile;
			         
					  s_APCI1710_DriverStruct.
					    s_BoardInformation[b_BoardHandle].
					    s_ModuleInformation[b_ModuleNumber].
					    s_SSIInformation.b_PositionTurnLength = 
					    b_PositionTurnLength;
			         
					  s_APCI1710_DriverStruct.
					    s_BoardInformation[b_BoardHandle].
					    s_ModuleInformation[b_ModuleNumber].
					    s_SSIInformation.b_TurnCptLength = 
					    b_TurnCptLength;
			         
					  /*********************************/
					  /* Initialise the profile lenght */
					  /*********************************/
					  if (b_SSICountingMode == APCI1710_BINARY_MODE)
					    {
					      outl ((b_SSIProfile + 1),
						    BoardInformation -> 
						    s_BaseInformation.
						    ui_BaseAddress[2]  
						    + 4 + (64 * b_ModuleNumber));    
					    }
					  else
					    {
					      outl (b_SSIProfile,
						    BoardInformation -> 
						    s_BaseInformation.
						    ui_BaseAddress[2]  
						    + 4 + (64 * b_ModuleNumber));    
					    }
					  /*********************************/
					  /* Initialise the profile lenght */
					  /*********************************/
					  ui_TimerValue = (UINT) (((ULONG) 
								   (b_PCIInputClock) * 500000UL)
								  / ul_SSIOutputClock);

					  /************************/
					  /* Initialise the timer */
					  /************************/
					  outl (ui_TimerValue,
						BoardInformation -> 
						s_BaseInformation.
						ui_BaseAddress[2]  
						+ (64 * b_ModuleNumber));    
					  /********************************/
					  /* Initialise the counting mode */
					  /********************************/
					  outl ((7 * b_SSICountingMode),
						BoardInformation -> 
						s_BaseInformation.
						ui_BaseAddress[2]  
						+ 12 + (64 * b_ModuleNumber));    

					  s_APCI1710_DriverStruct.
					    s_BoardInformation[b_BoardHandle].
					    s_ModuleInformation[b_ModuleNumber].
					    s_SSIInformation.b_SSIInit = 1;				    
					}
				      else
					{
					  /***************************************/
					  /* The selected counting mode is wrong */
					  /***************************************/
					  i_ReturnValue = -9; 			       
					} 
				    }
				  else
				    {	
				      /**************************************/
				      /* The selected output clock is wrong */
				      /**************************************/
				      i_ReturnValue = -8;  
				    }	 
				}
			      else
				{
				  /*****************************************/
				  /* The selected PCI input clock is wrong */
				  /*****************************************/
				  i_ReturnValue = -7;  
				}    
			    }
			  else
			    {
			      /********************************************/
			      /* The selected SSI profile length is wrong */
			      /********************************************/
			      i_ReturnValue = -4;  
			    }
			}
		      else
			{
			  /******************************************************/
			  /* The selected SSI turn counter data length is wrong */
			  /******************************************************/
			  i_ReturnValue = -6;  
			}
		    }
		  else
		    {
		      /**************************************************/
		      /* The selected SSI position data length is wrong */
		      /**************************************************/
		      i_ReturnValue = -5;  
		    }
		}
	      else
		{
		  /********************************************/
		  /* The selected SSI profile length is wrong */
		  /********************************************/
		  i_ReturnValue = -4;  
		}
	    }
	  else
	    {
	      /*******************************************/
	      /* The selected module is not a SSI module */
	      /*******************************************/
	      i_ReturnValue = -3;  
	    }
	}
      else
	{
	  /*****************************************/ 
	  /* The parametered modul number is wrong */
	  /*****************************************/
	  i_ReturnValue = -2;  
	}
    }
  else
    {
      /*****************************/ 
      /* The board handle is wrong */
      /*****************************/
      i_ReturnValue = -1;  
    }

  return  i_ReturnValue;

}

/*
  +----------------------------------------------------------------------------+
  | Function Name     : INT     i_APCI1710_Read1SSIValue                       |
  |                               (BYTE      b_BoardHandle,                    |
  |                                BYTE      b_ModulNumber,                    |
  |                                BYTE      b_SelectedSSI,                    |
  |                                PULONG  pul_Position,                       |
  |                                PULONG  pul_TurnCpt)                        |
  +----------------------------------------------------------------------------+
  | Task              : Read the selected SSI counter (b_SelectedSSI) from     |
  |                     selected module (b_ModulNbr).                          |
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE  b_BoardHandle         : Handle of board APCI-1710|
  |                     BYTE  b_ModulNumber         : Module number to         |
  |                                                   configure (0 to 3)       |
  |                     BYTE  b_SelectedSSI         : Selection from SSI       |
  |                                                   counter (0 to 2)         |
  +----------------------------------------------------------------------------+
  | Output Parameters : PULONG   pul_Position       : SSI position in the turn |
  |                     PULONG   pul_TurnCpt        : Number of turns          |
  +----------------------------------------------------------------------------+
  | Return Value      : 0: No error                                            |
  |                    -1: The handle parameter of the board is wrong          |
  |                    -2: The module parameter is wrong                       |
  |                    -3: The module is not a SSI module                      |
  |                    -4: SSI not initialised see function                    |
  |                        "i_APCI1710_InitSSI"                                |
  |                    -5: The selected SSI is wrong                           |
  +----------------------------------------------------------------------------+
*/
INT i_APCI1710_Read1SSIValue (BYTE     b_BoardHandle,
			      BYTE     b_ModuleNumber,
			      BYTE     b_SelectedSSI,
			      PULONG pul_Position,
			      PULONG pul_TurnCpt)
{
  INT    i_ReturnValue         = 0;
  BYTE   b_Cpt                 = 0;
  BYTE   b_Length              = 0;
  BYTE   b_Shift               = 0;
  ULONG ul_And                 = 0; 
  ULONG ul_StatusReg           = 0; 
  ULONG ul_CounterValue        = 0;

  /*******************************************************/
  /* Gets the pointer to the board information structure */
  /*******************************************************/
  str_APCI1710_BoardInformation *BoardInformation = NULL; 
   
  BoardInformation = &(s_APCI1710_DriverStruct.
		       s_BoardInformation[b_BoardHandle]);

  /****************************************/
  /* Checks if the b_BoardHandle is valid */
  /****************************************/
  if (b_BoardHandle < s_APCI1710_DriverStruct.b_NumberOfBoard)
    {   
      /*****************************************/
      /* Checks if the b_ModuleNumber is valid */
      /*****************************************/
      if (b_ModuleNumber <= 3) 
	{
	  /******************************************/
	  /* Checks the configuration of the Module */
	  /******************************************/
	  if (BoardInformation -> dw_MolduleConfiguration [b_ModuleNumber] == APCI1710_SSI_COUNTER)
	    {
	      /***************************/
	      /* Test if SSI initialised */
	      /***************************/

	      if (s_APCI1710_DriverStruct.
		  s_BoardInformation[b_BoardHandle].
		  s_ModuleInformation[b_ModuleNumber].
		  s_SSIInformation.b_SSIInit == 1)
		{ 
		  /****************************************/
		  /* Test the selected SSI counter number */
		  /****************************************/
		  if (b_SelectedSSI < 3)
		    {
		      /************************/
		      /* Start the conversion */
		      /************************/

		      outl (0,
			    BoardInformation -> 
			    s_BaseInformation.
			    ui_BaseAddress[2]  
			    + 8 + (64 * b_ModuleNumber));
		      do
			{
			  /*******************/
			  /* Read the status */
			  /*******************/

			  ul_StatusReg = inl (BoardInformation -> 
					      s_BaseInformation.
					      ui_BaseAddress[2]  
					      + (64 * b_ModuleNumber));
			}
		      while ((ul_StatusReg & 0x1) != 0);

		      /******************************/
		      /* Read the SSI counter value */
		      /******************************/

		      ul_CounterValue = inl (BoardInformation -> 
					     s_BaseInformation.
					     ui_BaseAddress[2] 
					     + 4 + (b_SelectedSSI * 4) 
					     + (64 * b_ModuleNumber));

		      /**************************************/
		      /* Gets the middle of the SSI profile */
		      /**************************************/
		      b_Length = s_APCI1710_DriverStruct.
			s_BoardInformation[b_BoardHandle].
			s_ModuleInformation[b_ModuleNumber].
			s_SSIInformation.b_SSIProfile / 2;

		      /*****************************************/
		      /* Enlarges the profil to an even number */
		      /*****************************************/
		      if ((b_Length * 2) != s_APCI1710_DriverStruct.
			  s_BoardInformation[b_BoardHandle].
			  s_ModuleInformation[b_ModuleNumber].
			  s_SSIInformation.b_SSIProfile)
			{
			  b_Length ++;
			}

		      b_Shift = b_Length - s_APCI1710_DriverStruct.
			s_BoardInformation[b_BoardHandle].
			s_ModuleInformation[b_ModuleNumber].
			s_SSIInformation.
b_PositionTurnLength;


		      *pul_Position = ul_CounterValue >> b_Shift;

		      ul_And = 1;

		      for (b_Cpt = 0; 
			   b_Cpt < s_APCI1710_DriverStruct.
			     s_BoardInformation[b_BoardHandle].
			     s_ModuleInformation[b_ModuleNumber].
			     s_SSIInformation.
b_PositionTurnLength; 
			   b_Cpt ++)
			{
			  ul_And = ul_And * 2;
			}

		      *pul_Position = *pul_Position & ((ul_And) - 1);

		      *pul_TurnCpt = ul_CounterValue >> b_Length;

		      ul_And = 1;

		      for (b_Cpt = 0; 
			   b_Cpt < s_APCI1710_DriverStruct.
			     s_BoardInformation[b_BoardHandle].
			     s_ModuleInformation[b_ModuleNumber].
			     s_SSIInformation.
b_TurnCptLength; 
			   b_Cpt ++)
			{
			  ul_And = ul_And * 2;
			}

		      *pul_TurnCpt = *pul_TurnCpt & ((ul_And) - 1);
		    }
		  else
		    {
		      /*****************************/
		      /* The selected SSI is wrong */
		      /*****************************/

		      i_ReturnValue = -5;
		    }
		}
	      else
		{
		  /***********************/
		  /* SSI not initialised */
		  /***********************/
		  i_ReturnValue = -4;
		}
	    }
	  else
	    {
	      /**********************************/
	      /* The module is not a SSI module */
	      /**********************************/
	      i_ReturnValue = -3;
	    }
	}
      else
	{
	  /***********************/
	  /* Module number error */
	  /***********************/
	  i_ReturnValue = -2;
	}
      
   }
  else
    {
      /**********************/
      /* Board Handle error */
      /**********************/
      i_ReturnValue = -1;
    }
  return (i_ReturnValue);
}


/*
  +----------------------------------------------------------------------------+
  | Function Name     :     INT  i_APCI1710_StartAllSSIScan                    |
  |                               (BYTE      b_BoardHandle,                    |
  |                                BYTE      b_ModulNumber)                    |
  +----------------------------------------------------------------------------+
  | Task              : Start des scan of SSI Module                           |
  |                     selected module (b_ModulNbr).                          |
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE  b_BoardHandle         : Handle of board APCI-1710|
  |                     BYTE  b_ModulNbr            : Module number to         |
  |                                                   configure (0 to 3)       |
  +----------------------------------------------------------------------------+
  | Output Parameters :                                                        |
  |                                                                            |
  +----------------------------------------------------------------------------+
  | Return Value      : 0: No error                                            |
  |                    -1: The handle parameter of the board is wrong          |
  |                    -2: The module parameter is wrong                       |
  |                    -3: The module is not a SSI module                      |
  |                    -4: SSI not initialised see function                    |
  |                        "i_APCI1710_InitSSI"                                |
  +----------------------------------------------------------------------------+
*/

int i_APCI1710_StartAllSSIScan (BYTE     b_BoardHandle,
				BYTE     b_ModuleNumber)
{
  INT    i_ReturnValue         = 0;
  BYTE   b_Cpt                 = 0;
  ULONG ul_And1                = 0; 
  ULONG ul_And2                = 0; 
/*  BYTE   b_SSICpt              = 0;
  BYTE   b_Length              = 0;
  BYTE   b_Shift               = 0;
  ULONG ul_StatusReg           = 0; 
  ULONG ul_CounterValue        = 0;
*/
  /*******************************************************/
  /* Gets the pointer to the board information structure */
  /*******************************************************/
  str_APCI1710_BoardInformation *BoardInformation = NULL; 
  
  BoardInformation = &(s_APCI1710_DriverStruct.
		       s_BoardInformation[b_BoardHandle]);

  /****************************************/
  /* Checks if the b_BoardHandle is valid */
  /****************************************/
  if (b_BoardHandle < s_APCI1710_DriverStruct.b_NumberOfBoard)
    {   
      /*****************************************/
      /* Checks if the b_ModuleNumber is valid */
      /*****************************************/
      if (b_ModuleNumber <= 3) 
	{
	  /******************************************/
	  /* Checks the configuration of the Module */
	  /******************************************/
	  if (BoardInformation -> dw_MolduleConfiguration [b_ModuleNumber] == APCI1710_SSI_COUNTER)
	    {
	      /***************************/
	      /* Test if SSI initialised */
	      /***************************/

	      if (s_APCI1710_DriverStruct.
		  s_BoardInformation[b_BoardHandle].
		  s_ModuleInformation[b_ModuleNumber].
		  s_SSIInformation.b_SSIInit == 1)
		{ 
		  ul_And1 = 1;
		  
		  for (b_Cpt = 0;b_Cpt < s_APCI1710_DriverStruct.
			   s_BoardInformation[b_BoardHandle].
			   s_ModuleInformation[b_ModuleNumber].
			   s_SSIInformation.
			   b_PositionTurnLength; 
		       b_Cpt ++)
		  { 
		      ul_And1 = ul_And1 * 2;
		  }

		  ul_And2 = 1;

		  for (b_Cpt = 0; 
		       b_Cpt < s_APCI1710_DriverStruct.
			 s_BoardInformation[b_BoardHandle].
			 s_ModuleInformation[b_ModuleNumber].
			 s_SSIInformation.b_TurnCptLength; 
		       b_Cpt ++)
		    {
		      ul_And2 = ul_And2 * 2;
		    }

		  /************************/
		  /* Start the conversion */
		  /************************/

		  outl (0,
			BoardInformation -> 
			s_BaseInformation.
			ui_BaseAddress[2]  
			+ 8 + (64 * b_ModuleNumber));

		}
	      else
		{
		  /***********************/
		  /* SSI not initialised */
		  /***********************/
		  i_ReturnValue = -4;
		}
	    }
	  else
	    {
	      /**********************************/
	      /* The module is not a SSI module */
	      /**********************************/
	      i_ReturnValue = -3;
	    }
	}
      else
	{
	  /***********************/
	  /* Module number error */
	  /***********************/
	  i_ReturnValue = -2;
	}
    }
  else
    {
      /*********************/
      /* BoardHandle error */
      /*********************/
      i_ReturnValue = -1;
    }
  
   return (i_ReturnValue);
}


/*
  +----------------------------------------------------------------------------+
  | Function Name     :     INT  i_APCI1710_ReadAllSSIValue                    |
  |                               (BYTE      b_BoardHandle,                    |
  |                                BYTE      b_ModulNumber,                    |
  |                                PULONG  pul_Position,                       |
  |                                PULONG  pul_TurnCpt)                        |
  +----------------------------------------------------------------------------+
  | Task              : Read all SSI counter (b_SelectedSSI) from              |
  |                     selected module (b_ModulNbr).                          |
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE  b_BoardHandle         : Handle of board APCI-1710|
  |                     BYTE  b_ModulNbr            : Module number to         |
  |                                                   configure (0 to 3)       |
  +----------------------------------------------------------------------------+
  | Output Parameters : PULONG   pul_Position       : SSI position in the turn |
  |                     PULONG   pul_TurnCpt        : Number of turns          |
  +----------------------------------------------------------------------------+
  | Return Value      : 0: No error                                            |
  |                    -1: The handle parameter of the board is wrong          |
  |                    -2: The module parameter is wrong                       |
  |                    -3: The module is not a SSI module                      |
  |                    -4: SSI not initialised see function                    |
  |                        "i_APCI1710_InitSSI"                                |
  +----------------------------------------------------------------------------+
*/

INT i_APCI1710_ReadAllSSIValue (BYTE     b_BoardHandle,
				BYTE     b_ModuleNumber,
				PULONG pul_Position,
				PULONG pul_TurnCpt)
{
  INT    i_ReturnValue         = 0;
  BYTE   b_Cpt                 = 0;
  BYTE   b_SSICpt              = 0;
  BYTE   b_Length              = 0;
  BYTE   b_Shift               = 0;
  ULONG ul_And1                = 0; 
  ULONG ul_And2                = 0; 
  ULONG ul_StatusReg           = 0; 
  ULONG ul_CounterValue        = 0;

  /*******************************************************/
  /* Gets the pointer to the board information structure */
  /*******************************************************/
  str_APCI1710_BoardInformation *BoardInformation = NULL; 
  
  BoardInformation = &(s_APCI1710_DriverStruct.
		       s_BoardInformation[b_BoardHandle]);

  /****************************************/
  /* Checks if the b_BoardHandle is valid */
  /****************************************/
  if (b_BoardHandle < s_APCI1710_DriverStruct.b_NumberOfBoard)
    {   
      /*****************************************/
      /* Checks if the b_ModuleNumber is valid */
      /*****************************************/
      if (b_ModuleNumber <= 3) 
	{
	  /******************************************/
	  /* Checks the configuration of the Module */
	  /******************************************/
	  if (BoardInformation -> dw_MolduleConfiguration [b_ModuleNumber] == APCI1710_SSI_COUNTER)
	    {
	      /***************************/
	      /* Test if SSI initialised */
	      /***************************/

	      if (s_APCI1710_DriverStruct.
		  s_BoardInformation[b_BoardHandle].
		  s_ModuleInformation[b_ModuleNumber].
		  s_SSIInformation.b_SSIInit == 1)
		{ 
		  ul_And1 = 1;
		  
  	       for (b_Cpt = 0; 
		    b_Cpt < s_APCI1710_DriverStruct.
			s_BoardInformation[b_BoardHandle].
			s_ModuleInformation[b_ModuleNumber].
			s_SSIInformation.
			b_PositionTurnLength; 
				    b_Cpt ++)
				 { 
				   ul_And1 = ul_And1 * 2;
				 }

		  ul_And2 = 1;

		  for (b_Cpt = 0; 
		       b_Cpt < s_APCI1710_DriverStruct.
			 s_BoardInformation[b_BoardHandle].
			 s_ModuleInformation[b_ModuleNumber].
			 s_SSIInformation.b_TurnCptLength; 
		       b_Cpt ++)
		    {
		      ul_And2 = ul_And2 * 2;
		    }

		  /************************/
		  /* Start the conversion */
		  /************************/
		  /* Start der Wandlung schon passiert, siehe oben */
/*		  outl (0,
			BoardInformation -> 
			s_BaseInformation.
			ui_BaseAddress[2]  
			+ 8 + (64 * b_ModuleNumber));   */

		  do
		    {
		      /*******************/
		      /* Read the status */
		      /*******************/

		      ul_StatusReg = inl(BoardInformation -> 
					 s_BaseInformation.
					 ui_BaseAddress[2]  
					 + (64 * b_ModuleNumber));
		    }
		  while ((ul_StatusReg & 0x1) != 0);

		  for (b_SSICpt = 0; b_SSICpt < 3; b_SSICpt ++)
		    {
		      /******************************/
		      /* Read the SSI counter value */
		      /******************************/

		      ul_CounterValue = inl (BoardInformation -> 
					     s_BaseInformation.
					     ui_BaseAddress[2]
					     + 4 + (b_SSICpt * 4) 
					     + (64 * b_ModuleNumber));

		      b_Length = s_APCI1710_DriverStruct.
			s_BoardInformation[b_BoardHandle].
			s_ModuleInformation[b_ModuleNumber].
			s_SSIInformation.b_SSIProfile / 2;

		      if ((b_Length * 2) != s_APCI1710_DriverStruct.
			  s_BoardInformation[b_BoardHandle].
			  s_ModuleInformation[b_ModuleNumber].
			  s_SSIInformation.
			  b_SSIProfile)
			{
			  b_Length ++;
			}

		      b_Shift = b_Length - s_APCI1710_DriverStruct.
			s_BoardInformation[b_BoardHandle]. 
			s_ModuleInformation[b_ModuleNumber].
			s_SSIInformation
			  .b_PositionTurnLength;


		      pul_Position[b_SSICpt] = ul_CounterValue >> b_Shift;
		      pul_Position[b_SSICpt] = pul_Position [b_SSICpt] & 
			((ul_And1) - 1);

		      pul_TurnCpt [b_SSICpt] = ul_CounterValue >> b_Length;
		      pul_TurnCpt [b_SSICpt] = pul_TurnCpt [b_SSICpt] & 
			((ul_And2) - 1);
		    }
		}
	      else
		{
		  /***********************/
		  /* SSI not initialised */
		  /***********************/
		  i_ReturnValue = -4;
		}
	    }
	  else
	    {
	      /**********************************/
	      /* The module is not a SSI module */
	      /**********************************/
	      i_ReturnValue = -3;
	    }
	}
      else
	{
	  /***********************/
	  /* Module number error */
	  /***********************/
	  i_ReturnValue = -2;
	}
    }
  else
    {
      /*********************/
      /* BoardHandle error */
      /*********************/
      i_ReturnValue = -1;
    }
  
   return (i_ReturnValue);
}

/*
  +----------------------------------------------------------------------------+
  | Function Name     :     INT  i_APCI1710_ReadAllSSIValue                    |
  |                               (BYTE      b_BoardHandle,                    |
  |                                BYTE      b_ModulNumber,                    |
  |                                PULONG  pul_Position,                       |
  |                                PULONG  pul_TurnCpt)                        |
  +----------------------------------------------------------------------------+
  | Task              : Read all SSI counter (b_SelectedSSI) from              |
  |                     selected module (b_ModulNbr).                          |
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE  b_BoardHandle         : Handle of board APCI-1710|
  |                     BYTE  b_ModulNbr            : Module number to         |
  |                                                   configure (0 to 3)       |
  +----------------------------------------------------------------------------+
  | Output Parameters : PULONG   pul_Position       : SSI position             |
  +----------------------------------------------------------------------------+
  | Return Value      : 0: No error                                            |
  |                    -1: The handle parameter of the board is wrong          |
  |                    -2: The module parameter is wrong                       |
  |                    -3: The module is not a SSI module                      |
  |                    -4: SSI not initialised see function                    |
  |                        "i_APCI1710_InitSSI"                                |
  +----------------------------------------------------------------------------+
*/

INT i_APCI1710_ReadAllSSIValueRaw (BYTE     b_BoardHandle,
				BYTE     b_ModuleNumber,
				   PULONG pul_Position)

{
  INT    i_ReturnValue         = 0;
  BYTE   b_Cpt                 = 0;
  BYTE   b_SSICpt              = 0;
  BYTE   b_Length              = 0;
  BYTE   b_Shift               = 0;
  ULONG ul_And1                = 0; 
  ULONG ul_And2                = 0; 
  ULONG ul_StatusReg           = 0; 
  ULONG ul_CounterValue        = 0;

  /*******************************************************/
  /* Gets the pointer to the board information structure */
  /*******************************************************/
  str_APCI1710_BoardInformation *BoardInformation = NULL; 
  
  BoardInformation = &(s_APCI1710_DriverStruct.
		       s_BoardInformation[b_BoardHandle]);

  /****************************************/
  /* Checks if the b_BoardHandle is valid */
  /****************************************/
  if (b_BoardHandle < s_APCI1710_DriverStruct.b_NumberOfBoard)
    {   
      /*****************************************/
      /* Checks if the b_ModuleNumber is valid */
      /*****************************************/
      if (b_ModuleNumber <= 3) 
	{
	  /******************************************/
	  /* Checks the configuration of the Module */
	  /******************************************/
	  if (BoardInformation -> dw_MolduleConfiguration [b_ModuleNumber] == APCI1710_SSI_COUNTER)
	    {
	      /***************************/
	      /* Test if SSI initialised */
	      /***************************/

	      if (s_APCI1710_DriverStruct.
		  s_BoardInformation[b_BoardHandle].
		  s_ModuleInformation[b_ModuleNumber].
		  s_SSIInformation.b_SSIInit == 1)
		{ 
		  ul_And1 = 1;
		  
  	       for (b_Cpt = 0; 
		    b_Cpt < s_APCI1710_DriverStruct.
			s_BoardInformation[b_BoardHandle].
			s_ModuleInformation[b_ModuleNumber].
			s_SSIInformation.
			b_PositionTurnLength; 
				    b_Cpt ++)
				 { 
				   ul_And1 = ul_And1 * 2;
				 }

		  ul_And2 = 1;

		  for (b_Cpt = 0; 
		       b_Cpt < s_APCI1710_DriverStruct.
			 s_BoardInformation[b_BoardHandle].
			 s_ModuleInformation[b_ModuleNumber].
			 s_SSIInformation.b_TurnCptLength; 
		       b_Cpt ++)
		    {
		      ul_And2 = ul_And2 * 2;
		    }

		  /************************/
		  /* Start the conversion */
		  /************************/
		  /* Start der Wandlung schon passiert, siehe oben */
/*		  outl (0,
			BoardInformation -> 
			s_BaseInformation.
			ui_BaseAddress[2]  
			+ 8 + (64 * b_ModuleNumber));   */

		  do
		    {
		      /*******************/
		      /* Read the status */
		      /*******************/

		      ul_StatusReg = inl(BoardInformation -> 
					 s_BaseInformation.
					 ui_BaseAddress[2]  
					 + (64 * b_ModuleNumber));
		    }
		  while ((ul_StatusReg & 0x1) != 0);

		  for (b_SSICpt = 0; b_SSICpt < 3; b_SSICpt ++)
		    {
		      /******************************/
		      /* Read the SSI counter value */
		      /******************************/

		      ul_CounterValue = inl (BoardInformation -> 
					     s_BaseInformation.
					     ui_BaseAddress[2]
					     + 4 + (b_SSICpt * 4) 
					     + (64 * b_ModuleNumber));

		      pul_Position[b_SSICpt] = ul_CounterValue; //FIXME rohwert rausgeben

		    }
		}
	      else
		{
		  /***********************/
		  /* SSI not initialised */
		  /***********************/
		  i_ReturnValue = -4;
		}
	    }
	  else
	    {
	      /**********************************/
	      /* The module is not a SSI module */
	      /**********************************/
	      i_ReturnValue = -3;
	    }
	}
      else
	{
	  /***********************/
	  /* Module number error */
	  /***********************/
	  i_ReturnValue = -2;
	}
    }
  else
    {
      /*********************/
      /* BoardHandle error */
      /*********************/
      i_ReturnValue = -1;
    }
  
   return (i_ReturnValue);
}



/*
+----------------------------------------------------------------------------+
|                               Included files                               |
+----------------------------------------------------------------------------+
*/

/*
+----------------------------------------------------------------------------+
| Function Name     : int     i_APCI1710_ReadSSI1DigitalInput                |
|                                       (BYTE      b_BoardHandle,            |
|                                        BYTE      b_ModulNbr,               |
|                                        BYTE      b_InputChannel,           |
|                                        PBYTE    pb_ChannelStatus)          |
+----------------------------------------------------------------------------+
| Task              : Read the status from selected SSI digital input        |
|                     (b_InputChannel)                                       |
+----------------------------------------------------------------------------+
| Input Parameters  : BYTE  b_BoardHandle         : Handle of board APCI-1710|
|                     BYTE  b_ModulNbr            : Module number to         |
|                                                   configure (0 to 3)       |
|                     BYTE  b_InputChannel        : Selection from digital   |
|                                                   input ( 0 to 2)          |
+----------------------------------------------------------------------------+
| Output Parameters : PBYTE   pb_ChannelStatus    : Digital input channel    |
|                                                   status                   |
|                                                   0 : Channel is not active|
|                                                   1 : Channel is active    |
+----------------------------------------------------------------------------+
| Return Value      : 0: No error                                            |
|                    -1: The handle parameter of the board is wrong          |
|                    -2: The module parameter is wrong                       |
|                    -3: The module is not a SSI module                      |
|                    -4: The selected SSI digital input is wrong             |
+----------------------------------------------------------------------------+
*/

int i_APCI1710_ReadSSI1DigitalInput  (BYTE    b_BoardHandle,
				      BYTE    b_ModuleNumber,
				      BYTE    b_InputChannel,
				      PBYTE  pb_ChannelStatus)
{
   INT    i_ReturnValue         = 0;
   ULONG ul_StatusRegister      = 0;


   /*******************************************************/
   /* Gets the pointer to the board information structure */
   /*******************************************************/
   str_APCI1710_BoardInformation *BoardInformation = NULL; 

   BoardInformation = &(s_APCI1710_DriverStruct.
                        s_BoardInformation[b_BoardHandle]);

   /****************************************/
   /* Checks if the b_BoardHandle is valid */
   /****************************************/
   if (b_BoardHandle < s_APCI1710_DriverStruct.b_NumberOfBoard)
   {   
      /*****************************************/
      /* Checks if the b_ModuleNumber is valid */
      /*****************************************/
      if (b_ModuleNumber <= 3) 
      {
         /******************************************/
         /* Checks the configuration of the Module */
         /******************************************/


         if (BoardInformation -> dw_MolduleConfiguration [b_ModuleNumber] == APCI1710_SSI_COUNTER)
	 {
            /******************************************/
	    /* Test the digital imnput channel number */
	    /******************************************/

	    if (b_InputChannel <= 2)
	    {
	       /**************************/
	       /* Read all digital input */
	       /**************************/

		 ul_StatusRegister = inl (BoardInformation -> 
		                          s_BaseInformation.
	 	                          ui_BaseAddress[2] 
					  + (64 * b_ModuleNumber));

		 *pb_ChannelStatus = (BYTE) (((~ul_StatusRegister) 
		                     >> (4 + b_InputChannel)) & 1);
  	    }
	    else
	    {
	       /********************************/
	       /* Selected digital input error */
	       /********************************/
	       i_ReturnValue = -4;
	    }
	 }
	 else
	 {
	    /**********************************/
	    /* The module is not a SSI module */
	    /**********************************/

            i_ReturnValue = -3;
         }
      }
      else
      {
         /***********************/
         /* Module number error */
	 /***********************/

         i_ReturnValue = -2;
      }
   }
   else
   {
      /************************/
      /* Boardhandle is wrong */
      /************************/

      i_ReturnValue = -1;
   
   }
   return (i_ReturnValue);
}

/*
+----------------------------------------------------------------------------+
| Function Name     : int     i_APCI1710_ReadSSIAllDigitalInput              |
|                                       (BYTE      b_BoardHandle,            |
|                                        BYTE      b_ModulNbr,               |
|                                        PBYTE    pb_InputStatus)            |
+----------------------------------------------------------------------------+
| Task              : Read the status from all SSI digital inputs from       |
|                     selected SSI module (b_ModulNbr)                       |
+----------------------------------------------------------------------------+
| Input Parameters  : BYTE  b_BoardHandle         : Handle of board APCI-1710|
|                     BYTE  b_ModulNbr            : Module number to         |
|                                                   configure (0 to 3)       |
+----------------------------------------------------------------------------+
| Output Parameters : PBYTE   pb_InputStatus      : Digital inputs channel   |
|                                                   status                   |
+----------------------------------------------------------------------------+
| Return Value      : 0: No error                                            |
|                    -1: The handle parameter of the board is wrong          |
|                    -2: The module parameter is wrong                       |
|                    -3: The module is not a SSI module                      |
+----------------------------------------------------------------------------+
*/

int i_APCI1710_ReadSSIAllDigitalInput  (BYTE   b_BoardHandle,
				        BYTE   b_ModuleNumber,
				        PBYTE pb_InputStatus)
{
   INT    i_ReturnValue         = 0;
   ULONG ul_StatusRegister      = 0;

   /*******************************************************/
   /* Gets the pointer to the board information structure */
   /*******************************************************/
   str_APCI1710_BoardInformation *BoardInformation = NULL; 

   
   BoardInformation = &(s_APCI1710_DriverStruct.
                        s_BoardInformation[b_BoardHandle]);

   /****************************************/
   /* Checks if the b_BoardHandle is valid */
   /****************************************/
   if (b_BoardHandle < s_APCI1710_DriverStruct.b_NumberOfBoard)
   {   
      /*****************************************/
      /* Checks if the b_ModuleNumber is valid */
      /*****************************************/
      if (b_ModuleNumber <= 3) 
      {
         /******************************************/
         /* Checks the configuration of the Module */
         /******************************************/
         if (BoardInformation -> dw_MolduleConfiguration [b_ModuleNumber] == APCI1710_SSI_COUNTER)
	 {

	    /**************************/
	    /* Read all digital input */
	    /**************************/

	    ul_StatusRegister = inl (BoardInformation -> 
		                     s_BaseInformation.
	 	                     ui_BaseAddress[2]
				     + (64 * b_ModuleNumber));

	    *pb_InputStatus = (BYTE) (((~ul_StatusRegister) >> 4) & 7);
         }
	 else
	 {
	    /**********************************/
	    /* The module is not a SSI module */
	    /**********************************/

	    i_ReturnValue = -3;

         }
      }
      else
      {
         /***********************/
         /* Module number error */
         /***********************/

         i_ReturnValue = -2;
      }
   }
   else
   {
      /*********************/
      /* BoardHandle Error */
      /*********************/

      i_ReturnValue = -1;
   
   }
   return (i_ReturnValue);
}

/*
+----------------------------------------------------------------------------+
| Function Name     : int     i_APCI1710_SetSSIDigitalOutput                 |
|                                       (BYTE      b_BoardHandle,            |
|                                        BYTE      b_ModulNbr)               |
+----------------------------------------------------------------------------+
| Task              : Set the digital output from selected SSI moule         |
|                     (b_ModuleNbr) ON                                       |
+----------------------------------------------------------------------------+
| Input Parameters  : BYTE  b_BoardHandle   : Handle of board APCI-1710      |
|                     BYTE  b_ModulNbr      : Module number to configure     |
|                                             (0 to 3)                       |
|                     BYTE   b_OutputValue  : Selection if the digital output|
|                                             should be set or reset:        |
|                                                0: Channel will be reseted  |
|                                                1: Channel will be seted    |
+----------------------------------------------------------------------------+
| Output Parameters : -                                                      |
+----------------------------------------------------------------------------+
| Return Value      : 0: No error                                            |
|                    -1: The handle parameter of the board is wrong          |
|                    -2: The module parameter is wrong                       |
|                    -3: The module is not a SSI module                      |
|                    -4: The output value is wrong                           |
+----------------------------------------------------------------------------+
*/

int i_APCI1710_SetSSIDigitalOutput  (BYTE b_BoardHandle,
				     BYTE b_ModuleNumber,
				     BYTE b_OutputValue)
{

   INT    i_ReturnValue         = 0;


   /*******************************************************/
   /* Gets the pointer to the board information structure */
   /*******************************************************/
   str_APCI1710_BoardInformation *BoardInformation = NULL; 

   
   BoardInformation = &(s_APCI1710_DriverStruct.
                        s_BoardInformation[b_BoardHandle]);

   /****************************************/
   /* Checks if the b_BoardHandle is valid */
   /****************************************/
   if (b_BoardHandle < s_APCI1710_DriverStruct.b_NumberOfBoard)
   {   
      /*****************************************/
      /* Checks if the b_ModuleNumber is valid */
      /*****************************************/
      if (b_ModuleNumber <= 3) 
      {
         /******************************************/
         /* Checks the configuration of the Module */
         /******************************************/
         if (BoardInformation -> dw_MolduleConfiguration [b_ModuleNumber] == APCI1710_SSI_COUNTER)
	 {
            /**************************************/
            /* Checks the b_OutputValue parameter */
            /**************************************/
	    if (b_OutputValue <= 1)
	    {
               /***************************************/
               /* Writes the digital output ( 0 or 1) */
               /***************************************/
	       outl (b_OutputValue,
	             BoardInformation -> s_BaseInformation.
	   	     ui_BaseAddress[2] + 16 + (64 * b_ModuleNumber));
		     
	    }
	    else
	    {
               /**************************/
               /* b_OutputValue is wrong */
               /**************************/
	       i_ReturnValue = -4;
	    } 
         } 
	 else
	 {
	    /**********************************/
	    /* The module is not a SSI module */
	    /**********************************/

            i_ReturnValue = -3;
         }
      }
      else
      {
         /***********************/
         /* Module number error */
         /***********************/

         i_ReturnValue = -2;
      }
   }
   else
   {
      /*********************/
      /* BoardHandle error */
      /*********************/

      i_ReturnValue = -1;
  
   }

   return (i_ReturnValue);
}



/*
  +----------------------------------------------------------------------------+
  | Function Name     :  INT  i_APCI1710_InitPWM                               |
  |                                       (BYTE      b_BoardHandle,            |
  |                                        BYTE      b_ModulNbr,               |
  |                                        BYTE      b_PWM,                    |
  |                                        BYTE      b_ClockSelection,         |
  |                                        BYTE      b_TimingUnit,             |
  |                                        ULONG    ul_LowTiming,              |
  |                                        ULONG    ul_HighTiming,             |
  |                                        PULONG  pul_RealLowTiming,          |
  |                                        PULONG  pul_RealHighTiming)         |
  +----------------------------------------------------------------------------+
  | Task              : Configure the selected PWM (b_PWM) from selected module|
  |                     (b_ModulNbr). The ul_LowTiming, ul_HighTiming and      |
  |                     ul_TimingUnit determine the low/high timing base for   |
  |                     the period. pul_RealLowTiming, pul_RealHighTiming      |
  |                     return the real timing value.                          |
  |                     You must calling this function be for you call any     |
  |                     other function witch access of the PWM.                |
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE      b_BoardHandle    : Handle of board APCI-1710 |
  |                     BYTE      b_ModulNbr       : Module number to configure|
  |                                                  (0 to 3)                  |
  |                     BYTE      b_PWM            : Selected PWM (0 or 1).    |
  |                     BYTE      b_ClockSelection : Selection from PCI bus    |
  |                                                  clock                     |
  |                                                   - APCI1710_30MHZ :       |
  |                                                     The PC have a 30 MHz   |
  |                                                     PCI bus clock          |
  |                                                   - APCI1710_33MHZ :       |
  |                                                     The PC have a 33 MHz   |
  |                                                     PCI bus clock          |
  |                                                   - APCI1710_40MHZ         |
  |                                                     The APCI-1710 have a   |
  |                                                     integrated 40Mhz       |
  |                                                     quartz.                |
  |                     BYTE      b_TimingUnit     : Base timing Unit (0 to 4) |
  |                                                       0 : ns               |
  |                                                       1 : æs               |
  |                                                       2 : ms               |
  |                                                       3 : s                |
  |                                                       4 : mn               |
  |                     ULONG     ul_LowTiming     : Low base timing value.    |
  |                     ULONG     ul_HighTiming    : High base timing value.   |
  +----------------------------------------------------------------------------+
  | Output Parameters : PULONG   pul_RealLowTiming  : Real low base timing     |
  |                                                   value.                   |
  |                     PULONG   pul_RealHighTiming : Real high base timing    |
  |                                                   value.                   |
  +----------------------------------------------------------------------------+
  | Return Value      : 0: No error                                            |
  |                    -1: The handle parameter of the board is wrong          |
  |                    -2: Module selection wrong                              |
  |                    -3: The module is not a PWM module                      |
  |                    -4: PWM selection is wrong                              |
  |                    -5: The selected input clock is wrong                   |
  |                    -6: Timing Unit selection is wrong                      |
  |                    -7: Low base timing selection is wrong                  |
  |                    -8: High base timing selection is wrong                 |
  |                    -9: You can not used the 40MHz clock selection with     |
  |                        this board                                          |
  +----------------------------------------------------------------------------+
*/

INT i_APCI1710_InitPWM (BYTE     b_BoardHandle,            
                        BYTE     b_ModuleNumber,               
                        BYTE     b_PWM,                    
                        BYTE     b_ClockSelection,         
                        BYTE     b_TimingUnit,             
                        ULONG   ul_LowTiming,              
                        ULONG   ul_HighTiming,             
                        PULONG pul_RealLowTiming,           
                        PULONG pul_RealHighTiming) 
{
  INT    i_ReturnValue = 0;
  ULONG ul_LowTimerValue = 0;
  ULONG ul_HighTimerValue = 0;
  DWORD dw_Command;

  /*******************************************************/
  /* Gets the pointer to the board information structure */
  /*******************************************************/
  str_APCI1710_BoardInformation *BoardInformation = NULL; 	
	    
  BoardInformation = &(s_APCI1710_DriverStruct.
		       s_BoardInformation[b_BoardHandle]);
	
  /****************************************/
  /* Checks if the b_BoardHandle is valid */
  /****************************************/
  
  if (b_BoardHandle >= s_APCI1710_DriverStruct.b_NumberOfBoard)
    {   
      return -1;
    }	
     	
  /**************************/
  /* Test the module number */
  /**************************/

  if (b_ModuleNumber < 4)
    {
      /***************/
      /* Test if PWM */
      /***************/

      if (BoardInformation -> dw_MolduleConfiguration [b_ModuleNumber] == APCI1710_PWM)
	{
	  /**************************/
	  /* Test the PWM selection */
	  /**************************/

	  if (b_PWM >= 0 && b_PWM <= 1)
	    {
	      /******************/
	      /* Test the clock */
	      /******************/

	      if ((b_ClockSelection == APCI1710_30MHZ) ||
		  (b_ClockSelection == APCI1710_33MHZ) ||
		  (b_ClockSelection == APCI1710_40MHZ))
		{
		  /************************/
		  /* Test the timing unit */
		  /************************/

		  if ((b_TimingUnit >= 0) && (b_TimingUnit <= 4))
		    {
		      /*********************************/
		      /* Test the low timing selection */
		      /*********************************/

		      if (((b_ClockSelection == APCI1710_30MHZ) && (b_TimingUnit == 0) && (ul_LowTiming >= 266) && (ul_LowTiming <= 0xFFFFFFFFUL)) ||
			  ((b_ClockSelection == APCI1710_30MHZ) && (b_TimingUnit == 1) && (ul_LowTiming >= 1)   && (ul_LowTiming <= 571230650UL))  ||
			  ((b_ClockSelection == APCI1710_30MHZ) && (b_TimingUnit == 2) && (ul_LowTiming >= 1)   && (ul_LowTiming <= 571230UL))     ||
			  ((b_ClockSelection == APCI1710_30MHZ) && (b_TimingUnit == 3) && (ul_LowTiming >= 1)   && (ul_LowTiming <= 571UL))        ||
			  ((b_ClockSelection == APCI1710_30MHZ) && (b_TimingUnit == 4) && (ul_LowTiming >= 1)   && (ul_LowTiming <= 9UL))          ||
			  ((b_ClockSelection == APCI1710_33MHZ) && (b_TimingUnit == 0) && (ul_LowTiming >= 242) && (ul_LowTiming <= 0xFFFFFFFFUL)) ||
			  ((b_ClockSelection == APCI1710_33MHZ) && (b_TimingUnit == 1) && (ul_LowTiming >= 1)   && (ul_LowTiming <= 519691043UL))  ||
			  ((b_ClockSelection == APCI1710_33MHZ) && (b_TimingUnit == 2) && (ul_LowTiming >= 1)   && (ul_LowTiming <= 519691UL))     ||
			  ((b_ClockSelection == APCI1710_33MHZ) && (b_TimingUnit == 3) && (ul_LowTiming >= 1)   && (ul_LowTiming <= 520UL))        ||
			  ((b_ClockSelection == APCI1710_33MHZ) && (b_TimingUnit == 4) && (ul_LowTiming >= 1)   && (ul_LowTiming <= 8UL))          ||
			  ((b_ClockSelection == APCI1710_40MHZ) && (b_TimingUnit == 0) && (ul_LowTiming >= 200) && (ul_LowTiming <= 0xFFFFFFFFUL)) ||
			  ((b_ClockSelection == APCI1710_40MHZ) && (b_TimingUnit == 1) && (ul_LowTiming >= 1)   && (ul_LowTiming <= 429496729UL))  ||
			  ((b_ClockSelection == APCI1710_40MHZ) && (b_TimingUnit == 2) && (ul_LowTiming >= 1)   && (ul_LowTiming <= 429496UL))     ||
			  ((b_ClockSelection == APCI1710_40MHZ) && (b_TimingUnit == 3) && (ul_LowTiming >= 1)   && (ul_LowTiming <= 429UL))        ||
			  ((b_ClockSelection == APCI1710_40MHZ) && (b_TimingUnit == 4) && (ul_LowTiming >= 1)   && (ul_LowTiming <= 7UL)))
			{
			  /**********************************/
			  /* Test the High timing selection */
			  /**********************************/

			  if (((b_ClockSelection == APCI1710_30MHZ) && (b_TimingUnit == 0) && (ul_HighTiming >= 266) && (ul_HighTiming <= 0xFFFFFFFFUL)) ||
			      ((b_ClockSelection == APCI1710_30MHZ) && (b_TimingUnit == 1) && (ul_HighTiming >= 1)   && (ul_HighTiming <= 571230650UL))  ||
			      ((b_ClockSelection == APCI1710_30MHZ) && (b_TimingUnit == 2) && (ul_HighTiming >= 1)   && (ul_HighTiming <= 571230UL))     ||
			      ((b_ClockSelection == APCI1710_30MHZ) && (b_TimingUnit == 3) && (ul_HighTiming >= 1)   && (ul_HighTiming <= 571UL))        ||
			      ((b_ClockSelection == APCI1710_30MHZ) && (b_TimingUnit == 4) && (ul_HighTiming >= 1)   && (ul_HighTiming <= 9UL))          ||
			      ((b_ClockSelection == APCI1710_33MHZ) && (b_TimingUnit == 0) && (ul_HighTiming >= 242) && (ul_HighTiming <= 0xFFFFFFFFUL)) ||
			      ((b_ClockSelection == APCI1710_33MHZ) && (b_TimingUnit == 1) && (ul_HighTiming >= 1)   && (ul_HighTiming <= 519691043UL))  ||
			      ((b_ClockSelection == APCI1710_33MHZ) && (b_TimingUnit == 2) && (ul_HighTiming >= 1)   && (ul_HighTiming <= 519691UL))     ||
			      ((b_ClockSelection == APCI1710_33MHZ) && (b_TimingUnit == 3) && (ul_HighTiming >= 1)   && (ul_HighTiming <= 520UL))        ||
			      ((b_ClockSelection == APCI1710_33MHZ) && (b_TimingUnit == 4) && (ul_HighTiming >= 1)   && (ul_HighTiming <= 8UL))          ||
			      ((b_ClockSelection == APCI1710_40MHZ) && (b_TimingUnit == 0) && (ul_HighTiming >= 200) && (ul_HighTiming <= 0xFFFFFFFFUL)) ||
			      ((b_ClockSelection == APCI1710_40MHZ) && (b_TimingUnit == 1) && (ul_HighTiming >= 1)   && (ul_HighTiming <= 429496729UL))  ||
			      ((b_ClockSelection == APCI1710_40MHZ) && (b_TimingUnit == 2) && (ul_HighTiming >= 1)   && (ul_HighTiming <= 429496UL))     ||
			      ((b_ClockSelection == APCI1710_40MHZ) && (b_TimingUnit == 3) && (ul_HighTiming >= 1)   && (ul_HighTiming <= 429UL))        ||
			      ((b_ClockSelection == APCI1710_40MHZ) && (b_TimingUnit == 4) && (ul_HighTiming >= 1)   && (ul_HighTiming <= 7UL)))
			    {
			      /**************************/
			      /* Test the board version */
			      /**************************/

			      if ((b_ClockSelection == APCI1710_40MHZ) && 
			          (BoardInformation-> b_BoardVersion > 0) ||
				  (b_ClockSelection != APCI1710_40MHZ))
				{

				  /************************************/
				  /* Calculate the low division fator */
				  /************************************/

				  switch (b_TimingUnit)
				    {
				      /******/
				      /* ns */
				      /******/

				    case 0:
				      /******************/
				      /* Timer 0 factor */
				      /******************/

				      ul_LowTimerValue = (ULONG) (ul_LowTiming / 4000);

				      ul_LowTiming     = ul_LowTiming - 1;
				      ul_LowTimerValue = ul_LowTimerValue - 2;

				      //				      if (b_ClockSelection != APCI1710_40MHZ)
				      //					{
				      //					  ul_LowTimerValue = (ULONG) (((ul_LowTimerValue) * 1007752288) / 1000000000);					    
				      //					}
					       
				      break;

				      /******/
				      /* æs */
				      /******/

				    case 1:
				      /******************/
				      /* Timer 0 factor */
				      /******************/

				      ul_LowTimerValue = (ULONG) ((ul_LowTiming * b_ClockSelection) / 4);

				      ul_LowTiming     = ul_LowTiming - 1;
				      ul_LowTimerValue = ul_LowTimerValue - 2;

				      //				      if (b_ClockSelection != APCI1710_40MHZ)
				      //					{					   
				      //					  ul_LowTimerValue = (ULONG) (((ul_LowTimerValue) * 1007752288) / 1000000000);
				      //					}


				      break;

				      /******/
				      /* ms */
				      /******/

				    case 2:

				      /******************/
				      /* Timer 0 factor */
				      /******************/

				      ul_LowTimerValue = ul_LowTiming * (250 * b_ClockSelection);

				      ul_LowTiming     = ul_LowTiming - 1;
				      ul_LowTimerValue = ul_LowTimerValue - 2;

				      //				      if (b_ClockSelection != APCI1710_40MHZ)
				      //					{
				      //					  ul_LowTimerValue = (ULONG) (((ul_LowTimerValue) * 1007752288) / 1000000000);					      
				      //					}
				      break;

				      /*****/
				      /* s */
				      /*****/

				    case 3:

				      /******************/
				      /* Timer 0 factor */
				      /******************/

				      ul_LowTimerValue = (ULONG) (ul_LowTiming * (250000 * b_ClockSelection));

				      ul_LowTiming     = ul_LowTiming - 1;
				      ul_LowTimerValue = ul_LowTimerValue - 2;

				      //				      if (b_ClockSelection != APCI1710_40MHZ)
				      //					{
				      //					  ul_LowTimerValue = (ULONG) (((ul_LowTimerValue) * 1007752288) / 1000000000);
				      //					}
				      break;

				      /******/
				      /* mn */
				      /******/

				    case 4:

				      /******************/
				      /* Timer 0 factor */
				      /******************/

				      ul_LowTimerValue = (ULONG) ((ul_LowTiming * 60) * (250000 * b_ClockSelection));

				      ul_LowTiming     = ul_LowTiming - 1;
				      ul_LowTimerValue = ul_LowTimerValue - 2;

				      //				      if (b_ClockSelection != APCI1710_40MHZ)
				      //					{
				      //					  ul_LowTimerValue = (ULONG) (((ul_LowTimerValue) * 1007752288) / 1000000000);					      
				      //					}

				      break;
				    }

				  /*************************************/
				  /* Calculate the high division fator */
				  /*************************************/

				  switch (b_TimingUnit)
				    {
				      /******/
				      /* ns */
				      /******/

				    case 0:
				      /******************/
				      /* Timer 0 factor */
				      /******************/

				      ul_HighTimerValue = (ULONG) ((ul_HighTiming * b_ClockSelection) / 4000);

				      ul_HighTiming     = ul_HighTiming - 1;
				      ul_HighTimerValue = ul_HighTimerValue - 2;

				      //				      if (b_ClockSelection != APCI1710_40MHZ)
				      //					{
				      //					  ul_HighTimerValue = (ULONG) (((ul_LowTimerValue) * 1007752288) / 1000000000);
				      //					}

				      break;

				      /******/
				      /* æs */
				      /******/

				    case 1:
				      /******************/
				      /* Timer 0 factor */
				      /******************/

				      /******************/
				      /* Timer 0 factor */
				      /******************/

				      ul_HighTimerValue = (ULONG) ((ul_HighTiming * b_ClockSelection) / 4);

				      ul_HighTiming     = ul_HighTiming - 1;
				      ul_HighTimerValue = ul_HighTimerValue - 2;

				      //				      if (b_ClockSelection != APCI1710_40MHZ)
				      //					{
				      //					  ul_HighTimerValue = (ULONG) (((ul_LowTimerValue) * 1007752288) / 1000000000);
				      //					}

				      break;

				      /******/
				      /* ms */
				      /******/

				    case 2:

				      /******************/
				      /* Timer 0 factor */
				      /******************/

				      ul_HighTimerValue = ul_HighTiming * (250 * b_ClockSelection);

				      ul_HighTiming     = ul_HighTiming - 1;
				      ul_HighTimerValue = ul_HighTimerValue - 2;

				      //				      if (b_ClockSelection != APCI1710_40MHZ)
				      //					{
				      //					  ul_HighTimerValue = (ULONG) (((ul_LowTimerValue) * 1007752288) / 1000000000);
				      //					}

				      break;

				      /*****/
				      /* s */
				      /*****/

				    case 3:

				      /******************/
				      /* Timer 0 factor */
				      /******************/

				      ul_HighTimerValue = (ULONG) (ul_HighTiming * (250000UL * b_ClockSelection));

				      ul_HighTiming     = ul_HighTiming - 1;
				      ul_HighTimerValue = ul_HighTimerValue - 2;

				      //				      if (b_ClockSelection != APCI1710_40MHZ)
				      //					{
				      //					  ul_HighTimerValue = (ULONG) (((ul_HighTimerValue) * 1007752288) / 1000000000);					      
				      //					}

				      break;

				      /******/
				      /* mn */
				      /******/

				    case 4:
				      /******************/
				      /* Timer 0 factor */
				      /******************/

				      ul_HighTimerValue = (ULONG) ((ul_HighTiming * 60) * (250000UL * b_ClockSelection));

				      ul_HighTiming     = ul_HighTiming - 1;
				      ul_HighTimerValue = ul_HighTimerValue - 2;

				      //				      if (b_ClockSelection != APCI1710_40MHZ)
				      //					{
				      //					  ul_HighTimerValue = (ULONG) (((ul_HighTimerValue) * 1007752288) / 1000000000);					      
				      //					}
				      break;
				    }
				    
				  /****************************/
				  /* Save the clock selection */
				  /****************************/
								
				  s_APCI1710_DriverStruct.
				    s_BoardInformation[b_BoardHandle].
				    s_ModuleInformation[b_ModuleNumber].
				    s_PWMModuleInfo.
				    b_ClockSelection = b_ClockSelection;

				  /************************/
				  /* Save the timing unit */
				  /************************/

				  s_APCI1710_DriverStruct.
				    s_BoardInformation[b_BoardHandle].
				    s_ModuleInformation[b_ModuleNumber].
				    s_PWMModuleInfo.
				    s_PWMInfo [b_PWM].
				    b_TimingUnit = b_TimingUnit;

				  /****************************/
				  /* Save the low base timing */
				  /****************************/
				  
				  //For linux Kernel Mode
				  *pul_RealLowTiming = ul_LowTimerValue;

				  s_APCI1710_DriverStruct.
				    s_BoardInformation[b_BoardHandle].
				    s_ModuleInformation[b_ModuleNumber].
				    s_PWMModuleInfo.
				    s_PWMInfo [b_PWM].
				    ul_RealLowTiming = *pul_RealLowTiming;

				  /*****************************/
				  /* Save the high base timing */
				  /*****************************/
				  
				  //For linux Kernel Mode
				  *pul_RealHighTiming = ul_HighTimerValue;
				  	
				  s_APCI1710_DriverStruct.
				    s_BoardInformation[b_BoardHandle].
				    s_ModuleInformation[b_ModuleNumber].
				    s_PWMModuleInfo.
				    s_PWMInfo [b_PWM].
				    ul_RealHighTiming = *pul_RealHighTiming;

				  /************************/
				  /* Write the low timing */
				  /************************/
											
				  outl (ul_LowTimerValue,
					BoardInformation -> 
					s_BaseInformation.
					ui_BaseAddress[2] + 
					(20 * b_PWM) + (64 * b_ModuleNumber));

				  /*************************/
				  /* Write the high timing */
				  /*************************/

				  outl (ul_HighTimerValue,
					BoardInformation -> 
					s_BaseInformation.
					ui_BaseAddress[2] + 
					4 + (20 * b_PWM) + (64 * b_ModuleNumber));

				  /***************************/
				  /* Set the clock selection */
				  /***************************/
				
				  dw_Command = inl (BoardInformation -> 
						    s_BaseInformation.
						    ui_BaseAddress[2] + 
						    8 + (20 * b_PWM) + (64 * b_ModuleNumber));

				  dw_Command = dw_Command & 0x7F;

				  if (b_ClockSelection == APCI1710_40MHZ)
				    {
				      dw_Command = dw_Command | 0x80;
				    }

				  /***************************/
				  /* Set the clock selection */
				  /***************************/

				  outl (dw_Command,
					BoardInformation -> 
					s_BaseInformation.
					ui_BaseAddress[2] + 
					8 + (20 * b_PWM) + (64 * b_ModuleNumber));	 		          

				  /*************/
				  /* PWM init. */
				  /*************/

				  s_APCI1710_DriverStruct.
				    s_BoardInformation[b_BoardHandle].
				    s_ModuleInformation[b_ModuleNumber].
				    s_PWMModuleInfo.
				    s_PWMInfo [b_PWM].
				    b_PWMInit = 1;
				}
			      else
				{
				  /***************************************************/
				  /* You can not used the 40MHz clock selection with */
				  /* this board                                      */
				  /***************************************************/

				  i_ReturnValue = -9;
				}
			    }
			  else
			    {
			      /***************************************/
			      /* High base timing selection is wrong */
			      /***************************************/

			      i_ReturnValue = -8;
			    }
			}
		      else
			{
			  /**************************************/
			  /* Low base timing selection is wrong */
			  /**************************************/

			  i_ReturnValue = -7;
			}
		    } // if ((b_TimingUnit >= 0) && (b_TimingUnit <= 4))
		  else
		    {
		      /**********************************/
		      /* Timing unit selection is wrong */
		      /**********************************/

		      i_ReturnValue = -6;
		    } // if ((b_TimingUnit >= 0) && (b_TimingUnit <= 4))
		} // if ((b_ClockSelection == APCI1710_30MHZ) || (b_ClockSelection == APCI1710_33MHZ) || (b_ClockSelection == APCI1710_40MHZ))
	      else
		{
		  /*******************************/
		  /* The selected clock is wrong */
		  /*******************************/

		  i_ReturnValue = -5;
		} // if ((b_ClockSelection == APCI1710_30MHZ) || (b_ClockSelection == APCI1710_33MHZ) || (b_ClockSelection == APCI1710_40MHZ))
	    } // if (b_PWM >= 0 && b_PWM <= 1)
	  else
	    {
	      /******************************/
	      /* Tor PWM selection is wrong */
	      /******************************/

	      i_ReturnValue = -4;
	    } // if (b_PWM >= 0 && b_PWM <= 1)
	}
      else
	{
	  /**********************************/
	  /* The module is not a PWM module */
	  /**********************************/

	  i_ReturnValue = -3;
	}
    }
  else
    {
      /***********************/
      /* Module number error */
      /***********************/

      i_ReturnValue = -2;
    }

  return (i_ReturnValue);
}

/*
  +----------------------------------------------------------------------------+
  | Function Name     : INT _ i_APCI1710_SetNewPWMTiming                       |
  |                                       (BYTE      b_BoardHandle,            |
  |                                        BYTE      b_ModulNbr,               |
  |                                        BYTE      b_PWM,                    |
  |                                        BYTE      b_ClockSelection,         |
  |                                        BYTE      b_TimingUnit,             |
  |                                        ULONG    ul_LowTiming,              |
  |                                        ULONG    ul_HighTiming)             |
  +----------------------------------------------------------------------------+
  | Task              : Set a new timing. The ul_LowTiming, ul_HighTiming and  |
  |                     ul_TimingUnit determine the low/high timing base for   |
  |                     the period.                                            |
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE     b_BoardHandle    : Handle of board APCI-1710  |
  |                     BYTE     b_ModulNbr       : Module number to configure |
  |                                                  (0 to 3)                  |
  |                     BYTE     b_PWM            : Selected PWM (0 or 1).     |
  |                     BYTE     b_TimingUnit     : Base timing Unit (0 to 4)  |
  |                                                       0 : ns               |
  |                                                       1 : æs               |
  |                                                       2 : ms               |
  |                                                       3 : s                |
  |                                                       4 : mn               |
  |                     ULONG    ul_LowTiming     : Low base timing value.     |
  |                     ULONG    ul_HighTiming    : High base timing value.    |
  +----------------------------------------------------------------------------+
  | Output Parameters : -                                                      |
  +----------------------------------------------------------------------------+
  | Return Value      : 0: No error                                            |
  |                    -1: The handle parameter of the board is wrong          |
  |                    -2: Module selection wrong                              |
  |                    -3: The module is not a PWM module                      |
  |                    -4: PWM selection is wrong                              |
  |                    -5: PWM not initialised                                 |
  |                    -6: Timing Unit selection is wrong                      |
  |                    -7: Low base timing selection is wrong                  |
  |                    -8: High base timing selection is wrong                 |
  +----------------------------------------------------------------------------+
*/

INT i_APCI1710_SetNewPWMTiming (BYTE     b_BoardHandle,            
                                BYTE     b_ModuleNumber,               
                                BYTE     b_PWM,                    
                                BYTE     b_ClockSelection,         
                                BYTE     b_TimingUnit,             
                                ULONG   ul_LowTiming,              
                                ULONG   ul_HighTiming)
{
  INT    i_ReturnValue = 0;
  ULONG ul_LowTimerValue = 0;
  ULONG ul_HighTimerValue = 0;
  ULONG ul_RealLowTiming = 0;
  ULONG ul_RealHighTiming = 0;
  DWORD dw_Status = 0;
  DWORD dw_Command = 0;

  /*******************************************************/
  /* Gets the pointer to the board information structure */
  /*******************************************************/
  str_APCI1710_BoardInformation *BoardInformation = NULL; 
		   
  BoardInformation = &(s_APCI1710_DriverStruct.
		       s_BoardInformation[b_BoardHandle]);
	
  /****************************************/
  /* Checks if the b_BoardHandle is valid */
  /****************************************/
  if (b_BoardHandle >= s_APCI1710_DriverStruct.b_NumberOfBoard)
    {   
      return -1;
    }	
   
  /**************************/
  /* Test the module number */
  /**************************/

  if (b_ModuleNumber < 4)
    {
      /***************/
      /* Test if PWM */
      /***************/

      if (BoardInformation -> dw_MolduleConfiguration [b_ModuleNumber] == APCI1710_PWM)
	{
	  /**************************/
	  /* Test the PWM selection */
	  /**************************/

	  if (b_PWM >= 0 && b_PWM <= 1)
	    {
	      /***************************/
	      /* Test if PWM initialised */
	      /***************************/

	      dw_Status = inl (BoardInformation -> 
			       s_BaseInformation.
			       ui_BaseAddress[2] + 
			       12 + (20 * b_PWM) + (64 * b_ModuleNumber));


	      if (dw_Status & 0x10)
		{
		  b_ClockSelection = s_APCI1710_DriverStruct.
		    s_BoardInformation[b_BoardHandle].
   	            s_ModuleInformation[b_ModuleNumber].
		    s_PWMModuleInfo.
		    b_ClockSelection;

		  /************************/
		  /* Test the timing unit */
		  /************************/

		  if ((b_TimingUnit >= 0) && (b_TimingUnit <= 4))
		    {
		      /*********************************/
		      /* Test the low timing selection */
		      /*********************************/

		      if (((b_ClockSelection == APCI1710_30MHZ) && (b_TimingUnit == 0) && (ul_LowTiming >= 266) && (ul_LowTiming <= 0xFFFFFFFFUL)) ||
			  ((b_ClockSelection == APCI1710_30MHZ) && (b_TimingUnit == 1) && (ul_LowTiming >= 1)   && (ul_LowTiming <= 571230650UL))  ||
			  ((b_ClockSelection == APCI1710_30MHZ) && (b_TimingUnit == 2) && (ul_LowTiming >= 1)   && (ul_LowTiming <= 571230UL))     ||
			  ((b_ClockSelection == APCI1710_30MHZ) && (b_TimingUnit == 3) && (ul_LowTiming >= 1)   && (ul_LowTiming <= 571UL))        ||
			  ((b_ClockSelection == APCI1710_30MHZ) && (b_TimingUnit == 4) && (ul_LowTiming >= 1)   && (ul_LowTiming <= 9UL))          ||
			  ((b_ClockSelection == APCI1710_33MHZ) && (b_TimingUnit == 0) && (ul_LowTiming >= 242) && (ul_LowTiming <= 0xFFFFFFFFUL)) ||
			  ((b_ClockSelection == APCI1710_33MHZ) && (b_TimingUnit == 1) && (ul_LowTiming >= 1)   && (ul_LowTiming <= 519691043UL))  ||
			  ((b_ClockSelection == APCI1710_33MHZ) && (b_TimingUnit == 2) && (ul_LowTiming >= 1)   && (ul_LowTiming <= 519691UL))     ||
			  ((b_ClockSelection == APCI1710_33MHZ) && (b_TimingUnit == 3) && (ul_LowTiming >= 1)   && (ul_LowTiming <= 520UL))        ||
			  ((b_ClockSelection == APCI1710_33MHZ) && (b_TimingUnit == 4) && (ul_LowTiming >= 1)   && (ul_LowTiming <= 8UL))          ||
			  ((b_ClockSelection == APCI1710_40MHZ) && (b_TimingUnit == 0) && (ul_LowTiming >= 200) && (ul_LowTiming <= 0xFFFFFFFFUL)) ||
			  ((b_ClockSelection == APCI1710_40MHZ) && (b_TimingUnit == 1) && (ul_LowTiming >= 1)   && (ul_LowTiming <= 429496729UL))  ||
			  ((b_ClockSelection == APCI1710_40MHZ) && (b_TimingUnit == 2) && (ul_LowTiming >= 1)   && (ul_LowTiming <= 429496UL))     ||
			  ((b_ClockSelection == APCI1710_40MHZ) && (b_TimingUnit == 3) && (ul_LowTiming >= 1)   && (ul_LowTiming <= 429UL))        ||
			  ((b_ClockSelection == APCI1710_40MHZ) && (b_TimingUnit == 4) && (ul_LowTiming >= 1)   && (ul_LowTiming <= 7UL)))
			{
			  /**********************************/
			  /* Test the High timing selection */
			  /**********************************/

			  if (((b_ClockSelection == APCI1710_30MHZ) && (b_TimingUnit == 0) && (ul_HighTiming >= 266) && (ul_HighTiming <= 0xFFFFFFFFUL)) ||
			      ((b_ClockSelection == APCI1710_30MHZ) && (b_TimingUnit == 1) && (ul_HighTiming >= 1)   && (ul_HighTiming <= 571230650UL))  ||
			      ((b_ClockSelection == APCI1710_30MHZ) && (b_TimingUnit == 2) && (ul_HighTiming >= 1)   && (ul_HighTiming <= 571230UL))     ||
			      ((b_ClockSelection == APCI1710_30MHZ) && (b_TimingUnit == 3) && (ul_HighTiming >= 1)   && (ul_HighTiming <= 571UL))        ||
			      ((b_ClockSelection == APCI1710_30MHZ) && (b_TimingUnit == 4) && (ul_HighTiming >= 1)   && (ul_HighTiming <= 9UL))          ||
			      ((b_ClockSelection == APCI1710_33MHZ) && (b_TimingUnit == 0) && (ul_HighTiming >= 242) && (ul_HighTiming <= 0xFFFFFFFFUL)) ||
			      ((b_ClockSelection == APCI1710_33MHZ) && (b_TimingUnit == 1) && (ul_HighTiming >= 1)   && (ul_HighTiming <= 519691043UL))  ||
			      ((b_ClockSelection == APCI1710_33MHZ) && (b_TimingUnit == 2) && (ul_HighTiming >= 1)   && (ul_HighTiming <= 519691UL))     ||
			      ((b_ClockSelection == APCI1710_33MHZ) && (b_TimingUnit == 3) && (ul_HighTiming >= 1)   && (ul_HighTiming <= 520UL))        ||
			      ((b_ClockSelection == APCI1710_33MHZ) && (b_TimingUnit == 4) && (ul_HighTiming >= 1)   && (ul_HighTiming <= 8UL))          ||
			      ((b_ClockSelection == APCI1710_40MHZ) && (b_TimingUnit == 0) && (ul_HighTiming >= 200) && (ul_HighTiming <= 0xFFFFFFFFUL)) ||
			      ((b_ClockSelection == APCI1710_40MHZ) && (b_TimingUnit == 1) && (ul_HighTiming >= 1)   && (ul_HighTiming <= 429496729UL))  ||
			      ((b_ClockSelection == APCI1710_40MHZ) && (b_TimingUnit == 2) && (ul_HighTiming >= 1)   && (ul_HighTiming <= 429496UL))     ||
			      ((b_ClockSelection == APCI1710_40MHZ) && (b_TimingUnit == 3) && (ul_HighTiming >= 1)   && (ul_HighTiming <= 429UL))        ||
			      ((b_ClockSelection == APCI1710_40MHZ) && (b_TimingUnit == 4) && (ul_HighTiming >= 1)   && (ul_HighTiming <= 7UL)))
			    {

			      /************************************/
			      /* Calculate the low division fator */
			      /************************************/

			      switch (b_TimingUnit)
				{
				  /******/
				  /* ns */
				  /******/

				case 0:
				  /******************/
				  /* Timer 0 factor */
				  /******************/

				  ul_LowTimerValue = (ULONG) (ul_LowTiming / 4000);

				  ul_LowTiming     = ul_LowTiming - 1;
				  ul_LowTimerValue = ul_LowTimerValue - 2;

				  //				  if (b_ClockSelection != APCI1710_40MHZ)
				  //				    {
				  //				      ul_LowTimerValue = (ULONG) (((ul_LowTimerValue) * 1007752288) / 1000000000);					    
				  //				    }
					       
				  break;

				  /******/
				  /* æs */
				  /******/

				case 1:
				  /******************/
				  /* Timer 0 factor */
				  /******************/

				  ul_LowTimerValue = (ULONG) ((ul_LowTiming * b_ClockSelection) / 4);

				  ul_LowTiming     = ul_LowTiming - 1;
				  ul_LowTimerValue = ul_LowTimerValue - 2;

				  //				  if (b_ClockSelection != APCI1710_40MHZ)
				  //				    {					   
				  //				      ul_LowTimerValue = (ULONG) (((ul_LowTimerValue) * 1007752288) / 1000000000);
				  //				    }

				  break;

				  /******/
				  /* ms */
				  /******/

				case 2:

				  /******************/
				  /* Timer 0 factor */
				  /******************/

				  ul_LowTimerValue = ul_LowTiming * (250 * b_ClockSelection);

				  ul_LowTiming     = ul_LowTiming - 1;
				  ul_LowTimerValue = ul_LowTimerValue - 2;

				  //				  if (b_ClockSelection != APCI1710_40MHZ)
				  //				    {
				  //				      ul_LowTimerValue = (ULONG) (((ul_LowTimerValue) * 1007752288) / 1000000000);					      
				  //				    }
				  break;

				  /*****/
				  /* s */
				  /*****/

				case 3:

				  /******************/
				  /* Timer 0 factor */
				  /******************/

				  ul_LowTimerValue = (ULONG) (ul_LowTiming * (250000 * b_ClockSelection));

				  ul_LowTiming     = ul_LowTiming - 1;
				  ul_LowTimerValue = ul_LowTimerValue - 2;

				  //				  if (b_ClockSelection != APCI1710_40MHZ)
				  //				    {
				  //				      ul_LowTimerValue = (ULONG) (((ul_LowTimerValue) * 1007752288) / 1000000000);
				  //				    }
				  break;

				  /******/
				  /* mn */
				  /******/

				case 4:

				  /******************/
				  /* Timer 0 factor */
				  /******************/

				  ul_LowTimerValue = (ULONG) ((ul_LowTiming * 60) * (250000 * b_ClockSelection));

				  ul_LowTiming     = ul_LowTiming - 1;
				  ul_LowTimerValue = ul_LowTimerValue - 2;

				  //				  if (b_ClockSelection != APCI1710_40MHZ)
				  //				    {
				  //				      ul_LowTimerValue = (ULONG) (((ul_LowTimerValue) * 1007752288) / 1000000000);					      
				  //				    }

				  break;
				}

			      /*************************************/
			      /* Calculate the high division fator */
			      /*************************************/

			      switch (b_TimingUnit)
				{
				  /******/
				  /* ns */
				  /******/

				case 0:
				  /******************/
				  /* Timer 0 factor */
				  /******************/

				  ul_HighTimerValue = (ULONG) ((ul_HighTiming * b_ClockSelection) / 4000);

				  ul_HighTiming     = ul_HighTiming - 1;
				  ul_HighTimerValue = ul_HighTimerValue - 2;

				  //				  if (b_ClockSelection != APCI1710_40MHZ)
				  //				    {
				  //				      ul_HighTimerValue = (ULONG) (((ul_LowTimerValue) * 1007752288) / 1000000000);
				  //				    }

				  break;

				  /******/
				  /* æs */
				  /******/

				case 1:
				  /******************/
				  /* Timer 0 factor */
				  /******************/

				  /******************/
				  /* Timer 0 factor */
				  /******************/

				  ul_HighTimerValue = (ULONG) ((ul_HighTiming * b_ClockSelection) / 4);

				  ul_HighTiming     = ul_HighTiming - 1;
				  ul_HighTimerValue = ul_HighTimerValue - 2;

				  //				  if (b_ClockSelection != APCI1710_40MHZ)
				  //				    {
				  //				      ul_HighTimerValue = (ULONG) (((ul_LowTimerValue) * 1007752288) / 1000000000);
				  //				    }

				  break;

				  /******/
				  /* ms */
				  /******/

				case 2:

				  /******************/
				  /* Timer 0 factor */
				  /******************/

				  ul_HighTimerValue = ul_HighTiming * (250 * b_ClockSelection);

				  ul_HighTiming     = ul_HighTiming - 1;
				  ul_HighTimerValue = ul_HighTimerValue - 2;

				  //				  if (b_ClockSelection != APCI1710_40MHZ)
				  //				    {
				  //				      ul_HighTimerValue = (ULONG) (((ul_LowTimerValue) * 1007752288) / 1000000000);
				  //				    }

				  break;

				  /*****/
				  /* s */
				  /*****/

				case 3:

				  /******************/
				  /* Timer 0 factor */
				  /******************/

				  ul_HighTimerValue = (ULONG) (ul_HighTiming * (250000UL * b_ClockSelection));

				  ul_HighTiming     = ul_HighTiming - 1;
				  ul_HighTimerValue = ul_HighTimerValue - 2;

				  //				  if (b_ClockSelection != APCI1710_40MHZ)
				  //				    {
				  //				      ul_HighTimerValue = (ULONG) (((ul_HighTimerValue) * 1007752288) / 1000000000);					      
				  //				    }

				  break;

				  /******/
				  /* mn */
				  /******/

				case 4:
				  /******************/
				  /* Timer 0 factor */
				  /******************/

				  ul_HighTimerValue = (ULONG) ((ul_HighTiming * 60) * (250000UL * b_ClockSelection));

				  ul_HighTiming     = ul_HighTiming - 1;
				  ul_HighTimerValue = ul_HighTimerValue - 2;

				  //				  if (b_ClockSelection != APCI1710_40MHZ)
				  //				    {
				  //				      ul_HighTimerValue = (ULONG) ((ul_HighTimerValue * 1007752288) / 1000000000);					      
				  //				    }
				  break;
				}

			      /************************/
			      /* Save the timing unit */
			      /************************/

			      s_APCI1710_DriverStruct.
				s_BoardInformation[b_BoardHandle].
				s_ModuleInformation[b_ModuleNumber].
				s_PWMModuleInfo.
				s_PWMInfo [b_PWM].
				b_TimingUnit = b_TimingUnit;

			      /****************************/
			      /* Save the low base timing */
			      /****************************/
 
 			      //For linux Kernel Mode
			      ul_RealLowTiming = ul_LowTimerValue;
				  
			      s_APCI1710_DriverStruct.
				s_BoardInformation[b_BoardHandle].
				s_ModuleInformation[b_ModuleNumber].
				s_PWMModuleInfo.
				s_PWMInfo [b_PWM].
				ul_RealLowTiming = ul_RealLowTiming;

			      /****************************/
			      /* Save the high base timing */
			      /****************************/

 			      //For linux Kernel Mode
			      ul_RealHighTiming = ul_HighTimerValue;
			      
			      s_APCI1710_DriverStruct.
				s_BoardInformation[b_BoardHandle].
				s_ModuleInformation[b_ModuleNumber].
				s_PWMModuleInfo.
				s_PWMInfo [b_PWM].
				ul_RealHighTiming = ul_RealHighTiming;

			      /************************/
			      /* Write the low timing */
			      /************************/

			      outl (ul_LowTimerValue,
				    BoardInformation -> 
				    s_BaseInformation.
				    ui_BaseAddress[2] + 
				    (20 * b_PWM) + (64 * b_ModuleNumber));

			      /*************************/
			      /* Write the high timing */
			      /*************************/

			      outl (ul_HighTimerValue,
				    BoardInformation -> 
				    s_BaseInformation.
				    ui_BaseAddress[2] + 
				    4 + (20 * b_PWM) + (64 * b_ModuleNumber));

			      /***************************/
			      /* Set the clock selection */
			      /***************************/

			      dw_Command = inl (BoardInformation -> 
						s_BaseInformation.
						ui_BaseAddress[2] + 
						8 + (20 * b_PWM) + (64 * b_ModuleNumber));

			      dw_Command = dw_Command & 0x7F;

			      if (b_ClockSelection == APCI1710_40MHZ)
				{
				  dw_Command = dw_Command | 0x80;
				}

			      /***************************/
			      /* Set the clock selection */
			      /***************************/

			      outl (dw_Command,
				    BoardInformation -> 
				    s_BaseInformation.
				    ui_BaseAddress[2] + 
				    8 + (20 * b_PWM) + (64 * b_ModuleNumber));
			    }
			  else
			    {
			      /***************************************/
			      /* High base timing selection is wrong */
			      /***************************************/

			      i_ReturnValue = -8;
			    }
			}
		      else
			{
			  /**************************************/
			  /* Low base timing selection is wrong */
			  /**************************************/

			  i_ReturnValue = -7;
			}
		    } // if ((b_TimingUnit >= 0) && (b_TimingUnit <= 4))
		  else
		    {
		      /**********************************/
		      /* Timing unit selection is wrong */
		      /**********************************/

		      i_ReturnValue = -6;
		    } // if ((b_TimingUnit >= 0) && (b_TimingUnit <= 4))
		} // if (dw_Status & 0x10)
	      else
		{
		  /***********************/
		  /* PWM not initialised */
		  /***********************/

		  i_ReturnValue = -5;
		} // if (dw_Status & 0x10)
	    } // if (b_PWM >= 0 && b_PWM <= 1)
	  else
	    {
	      /******************************/
	      /* Tor PWM selection is wrong */
	      /******************************/

	      i_ReturnValue = -4;
	    } // if (b_PWM >= 0 && b_PWM <= 1)
	}
      else
	{
	  /**********************************/
	  /* The module is not a PWM module */
	  /**********************************/

	  i_ReturnValue = -3;
	}
    }
  else
    {
      /***********************/
      /* Module number error */
      /***********************/

      i_ReturnValue = -2;
    }

  return (i_ReturnValue);
}

/*
  +----------------------------------------------------------------------------+
  | Function Name     :  INT      i_APCI1710_EnablePWM                         |
  |                                       (BYTE   b_BoardHandle,               |
  |                                        BYTE   b_ModulNbr,                  |
  |                                        BYTE   b_PWM,                       |
  |                                        BYTE   b_StartLevel,                |
  |                                        BYTE   b_StopMode,                  |
  |                                        BYTE   b_StopLevel,                 |
  |                                        BYTE   b_ExternGate,                |
  |                                        BYTE   b_InterruptEnable)           |
  +----------------------------------------------------------------------------+
  | Task              : Enable the selected PWM (b_PWM) from selected module   |
  |                     (b_ModulNbr). You must calling the "i_APCI1710_InitPWM"|
  |                     function be for you call this function.                |
  |                     If you enable the PWM interrupt, the PWM generate a    |
  |                     interrupt after each period.                           |
  |                     See function "i_APCI1710_SetBoardIntRoutineX" and the  |
  |                     Interrupt mask description chapter.                    |
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE  b_BoardHandle     : Handle of board APCI-1710    |
  |                     BYTE  b_ModulNbr        : Selected module number       |
  |                                               (0 to 3)                     |
  |                     BYTE  b_PWM             : Selected PWM (0 or 1)        |
  |                     BYTE  b_StartLevel      : Start period level selection |
  |                                                0 : The period start with a |
  |                                                    low level               |
  |                                                1 : The period start with a |
  |                                                    high level              |
  |                     BYTE  b_StopMode        : Stop mode selection          |
  |                                                0 : The PWM is stopped      |
  |                                                    directly after the      |
  |                                                    "i_APCI1710_DisablePWM" |
  |                                                    function and break the  |
  |                                                    last period             |
  |                                                1 : After the               |
  |                                                    "i_APCI1710_DisablePWM" |
  |                                                     function the PWM is    |
  |                                                     stopped at the end from|
  |                                                     last period cycle.     |
  |                     BYTE  b_StopLevel       : Stop PWM level selection     |
  |                                                0 : The output signal keep  |
  |                                                    the level after the     |
  |                                                    "i_APCI1710_DisablePWM" |
  |                                                    function                |
  |                                                1 : The output signal is set|
  |                                                    to low after the        |
  |                                                    "i_APCI1710_DisablePWM" |
  |                                                    function                |
  |                                                2 : The output signal is set|
  |                                                    to high after the       |
  |                                                    "i_APCI1710_DisablePWM" |
  |                                                    function                |
  |                     BYTE  b_ExternGate      : Extern gate action selection |
  |                                                0 : Extern gate signal not  |
  |                                                    used.                   |
  |                                                1 : Extern gate signal used.|
  |                     BYTE  b_InterruptEnable : Enable or disable the PWM    |
  |                                               interrupt.                   |
  |                                               - APCI1710_ENABLE :          |
  |                                                 Enable the PWM interrupt   |
  |                                                 A interrupt occur after    |
  |                                                 each period                |
  |                                               - APCI1710_DISABLE :         |
  |                                                 Disable the PWM interrupt  |
  +----------------------------------------------------------------------------+
  | Output Parameters : -                                                      |
  +----------------------------------------------------------------------------+
  | Return Value      : 0:  No error                                           |
  |                    -1:  The handle parameter of the board is wrong         |
  |                    -2:  Module selection wrong                             |
  |                    -3:  The module is not a PWM module                     |
  |                    -4:  PWM selection is wrong                             |
  |                    -5:  PWM not initialised see function                   |
  |                         "i_APCI1710_InitPWM"                               |
  |                    -6:  PWM start level selection is wrong                 |
  |                    -7:  PWM stop mode selection is wrong                   |
  |                    -8:  PWM stop level selection is wrong                  |
  |                    -9:  Extern gate signal selection is wrong              |
  |                    -10: Interrupt parameter is wrong                       |
  |                    -11: Interrupt function not initialised.                |
  |                         See function "i_APCI1710_SetBoardIntRoutineX"      |
  +----------------------------------------------------------------------------+
*/

INT   i_APCI1710_EnablePWM    (BYTE  b_BoardHandle,
			       BYTE  b_ModuleNumber,
			       BYTE  b_PWM,
			       BYTE  b_StartLevel,
			       BYTE  b_StopMode,
			       BYTE  b_StopLevel,
			       BYTE  b_ExternGate,
			       BYTE  b_InterruptEnable)
{
  INT    i_ReturnValue = 0;
  DWORD dw_Status;
  DWORD dw_Command = 0;
  
  /*******************************************************/
  /* Gets the pointer to the board information structure */
  /*******************************************************/
  str_APCI1710_BoardInformation *BoardInformation = NULL; 
  
  BoardInformation = &(s_APCI1710_DriverStruct.
		       s_BoardInformation[b_BoardHandle]);

  /****************************************/
  /* Checks if the b_BoardHandle is valid */
  /****************************************/
  if (b_BoardHandle >= s_APCI1710_DriverStruct.b_NumberOfBoard)
    {   
      return -1;
    }	
   
  /**************************/
  /* Test the module number */
  /**************************/

  if (b_ModuleNumber < 4)
    {
      /***************/
      /* Test if PWM */
      /***************/

      if (BoardInformation -> dw_MolduleConfiguration [b_ModuleNumber] == APCI1710_PWM)
	{
	  /**************************/
	  /* Test the PWM selection */
	  /**************************/

	  if (b_PWM >= 0 && b_PWM <= 1)
	    {
	      /***************************/
	      /* Test if PWM initialised */
	      /***************************/

	      dw_Status = inl (BoardInformation -> 
			       s_BaseInformation.
			       ui_BaseAddress[2] + 
			       12 + (20 * b_PWM) + (64 * b_ModuleNumber));

	      if (dw_Status & 0x10)
		{
		  /**********************************/
		  /* Test the start level selection */
		  /**********************************/

		  if (b_StartLevel >= 0 && b_StartLevel <= 1)
		    {
		      /**********************/
		      /* Test the stop mode */
		      /**********************/

		      if (b_StopMode >= 0 && b_StopMode <= 1)
			{
			  /***********************/
			  /* Test the stop level */
			  /***********************/

			  if (b_StopLevel >= 0 && b_StopLevel <= 2)
			    {
			      /*****************************/
			      /* Test the extern gate mode */
			      /*****************************/

			      if (b_ExternGate >= 0 && b_ExternGate <= 1)
				{
				  /*****************************/
				  /* Test the interrupt action */
				  /*****************************/

				  if (b_InterruptEnable == APCI1710_ENABLE || b_InterruptEnable == APCI1710_DISABLE)
				    {
				      /******************************************/
				      /* Test if interrupt function initialised */
				      /******************************************/

				      if (((s_APCI1710_DriverStruct.
					    s_BoardInformation[b_BoardHandle].
					    b_InterruptInitialized) != APCI1710_DISABLE) ||
					  (b_InterruptEnable == APCI1710_DISABLE))
					{
					  /********************/
					  /* Read the command */
					  /********************/
					  outl (dw_Command,
						BoardInformation -> 
						s_BaseInformation.
						ui_BaseAddress[2] + 
						8 + (20 * b_PWM) + (64 * b_ModuleNumber));
	 		          
					  dw_Command = inl (BoardInformation -> 
							    s_BaseInformation.
							    ui_BaseAddress[2] + 
							    (8 + (20 * b_PWM) + (64 * b_ModuleNumber)));
	 		          
	 		          
					  dw_Command = inl (BoardInformation -> 
							    s_BaseInformation.
							    ui_BaseAddress[2] + 
							    8 + (20 * b_PWM) + (64 * b_ModuleNumber));

					  dw_Command = dw_Command & 0x80;

					  /********************/
					  /* Make the command */
					  /********************/

					  dw_Command = dw_Command | b_StopMode | (b_InterruptEnable << 3) | (b_ExternGate << 4) | (b_StartLevel << 5);

					  if (b_StopLevel & 3)
					    {
					      dw_Command = dw_Command | 2;

					      if (b_StopLevel & 2)
						{
						  dw_Command = dw_Command | 4;
						}
					    }

					  s_APCI1710_DriverStruct.
					    s_BoardInformation[b_BoardHandle].
					    s_ModuleInformation[b_ModuleNumber].
					    s_PWMModuleInfo.
					    s_PWMInfo [b_PWM].
					    b_InterruptEnable = b_InterruptEnable;

					  /*******************/
					  /* Set the command */
					  /*******************/
					      
					  outl (dw_Command,
						BoardInformation -> 
						s_BaseInformation.
						ui_BaseAddress[2] + 
						8 + (20 * b_PWM) + (64 * b_ModuleNumber));
	 		          
	 		          					      

					  /******************/
					  /* Enable the PWM */
					  /******************/

					  outl (1,
						BoardInformation -> 
						s_BaseInformation.
						ui_BaseAddress[2] + 
						12 + (20 * b_PWM) + (64 * b_ModuleNumber));

					}
				      else
					{
					  /**************************************/
					  /* Interrupt function not initialised */
					  /**************************************/

					  i_ReturnValue = -11;
					}
				    } // if (b_InterruptEnable == APCI1710_ENABLE || b_InterruptEnable == APCI1710_DISABLE)
				  else
				    {
				      /********************************/
				      /* Interrupt parameter is wrong */
				      /********************************/

				      i_ReturnValue = -10;
				    } // if (b_InterruptEnable == APCI1710_ENABLE || b_InterruptEnable == APCI1710_DISABLE)
				} // if (b_ExternGate >= 0 && b_ExternGate <= 1)
			      else
				{
				  /*****************************************/
				  /* Extern gate signal selection is wrong */
				  /*****************************************/

				  i_ReturnValue = -9;
				} // if (b_ExternGate >= 0 && b_ExternGate <= 1)
			    } // if (b_StopLevel >= 0 && b_StopLevel <= 2)
			  else
			    {
			      /*************************************/
			      /* PWM stop level selection is wrong */
			      /*************************************/

			      i_ReturnValue = -8;
			    } // if (b_StopLevel >= 0 && b_StopLevel <= 2)
			} // if (b_StopMode >= 0 && b_StopMode <= 1)
		      else
			{
			  /************************************/
			  /* PWM stop mode selection is wrong */
			  /************************************/

			  i_ReturnValue = -7;
			} // if (b_StopMode >= 0 && b_StopMode <= 1)
		    } // if (b_StartLevel >= 0 && b_StartLevel <= 1)
		  else
		    {
		      /**************************************/
		      /* PWM start level selection is wrong */
		      /**************************************/

		      i_ReturnValue = -6;
		    } // if (b_StartLevel >= 0 && b_StartLevel <= 1)
		} // if (dw_Status & 0x10)
	      else
		{
		  /***********************/
		  /* PWM not initialised */
		  /***********************/

		  i_ReturnValue = -5;
		} // if (dw_Status & 0x10)
	    } // if (b_PWM >= 0 && b_PWM <= 1)
	  else
	    {
	      /******************************/
	      /* Tor PWM selection is wrong */
	      /******************************/

	      i_ReturnValue = -4;
	    } // if (b_PWM >= 0 && b_PWM <= 1)
	}
      else
	{
	  /**********************************/
	  /* The module is not a PWM module */
	  /**********************************/

	  i_ReturnValue = -3;
	}
    }
  else
    {
      /***********************/
      /* Module number error */
      /***********************/

      i_ReturnValue = -2;
    }

  return (i_ReturnValue);
}

/*
  +----------------------------------------------------------------------------+
  | Function Name     :  INT  i_APCI1710_DisablePWM (BYTE   b_BoardHandle,     |
  |                                                  BYTE   b_ModulNbr,        |
  |                                                  BYTE   b_PWM)             |
  +----------------------------------------------------------------------------+
  | Task              : Disable the selected PWM (b_PWM) from selected module  |
  |                     (b_ModulNbr). The output signal level depend of the    |
  |                     initialisation by the "i_APCI1710_EnablePWM".          |
  |                     See the b_StartLevel, b_StopMode and b_StopLevel       |
  |                     parameters from this function.                         |
  +----------------------------------------------------------------------------+
  | Input Parameters  :BYTE  b_BoardHandle : Handle of board APCI-1710         |
  |                    BYTE  b_ModulNbr    : Selected module number (0 to 3)   |
  |                    BYTE  b_PWM         : Selected PWM (0 or 1)             |
  +----------------------------------------------------------------------------+
  | Output Parameters : -                                                      |
  +----------------------------------------------------------------------------+
  | Return Value      :  0: No error                                           |
  |                     -1: The handle parameter of the board is wrong         |
  |                     -2: Module selection wrong                             |
  |                     -3: The module is not a PWM module                     |
  |                     -4: PWM selection is wrong                             |
  |                     -5: PWM not initialised see function                   |
  |                         "i_APCI1710_InitPWM"                               |
  |                     -6: PWM not enabled see function                       |
  |                         "i_APCI1710_EnablePWM"                             |
  +----------------------------------------------------------------------------+
*/

INT   i_APCI1710_DisablePWM   (BYTE  b_BoardHandle,
			       BYTE  b_ModuleNumber,
			       BYTE  b_PWM)
{
  INT    i_ReturnValue = 0;
  DWORD dw_Status;
  
  /*******************************************************/
  /* Gets the pointer to the board information structure */
  /*******************************************************/
  str_APCI1710_BoardInformation *BoardInformation = NULL; 
   
  BoardInformation = &(s_APCI1710_DriverStruct.
		       s_BoardInformation[b_BoardHandle]);

  /****************************************/
  /* Checks if the b_BoardHandle is valid */
  /****************************************/
  if (b_BoardHandle >= s_APCI1710_DriverStruct.b_NumberOfBoard)
    {   
      return -1;
    }	
   
  /**************************/
  /* Test the module number */
  /**************************/

  if (b_ModuleNumber < 4)
    {
      /***************/
      /* Test if PWM */
      /***************/

      if (BoardInformation -> dw_MolduleConfiguration [b_ModuleNumber] == APCI1710_PWM)
	{
	  /**************************/
	  /* Test the PWM selection */
	  /**************************/

	  if (b_PWM >= 0 && b_PWM <= 1)
	    {
	      /***************************/
	      /* Test if PWM initialised */
	      /***************************/

              dw_Status = inl (BoardInformation -> 
		               s_BaseInformation.
		               ui_BaseAddress[2] + 
			       12 + (20 * b_PWM) + (64 * b_ModuleNumber));

	      if (dw_Status & 0x10)
		{
		  /***********************/
		  /* Test if PWM enabled */
		  /***********************/

		  if (dw_Status & 0x1)
		    {
		      /*******************/
		      /* Disable the PWM */
		      /*******************/

                      outl (0,
                            BoardInformation -> 
		            s_BaseInformation.
		            ui_BaseAddress[2] + 
		            12 + (20 * b_PWM) + (64 * b_ModuleNumber));
		    } // if (dw_Status & 0x1)
		  else
		    {
		      /*******************/
		      /* PWM not enabled */
		      /*******************/

		      i_ReturnValue = -6;
		    } // if (dw_Status & 0x1)
		} // if (dw_Status & 0x10)
	      else
		{
		  /***********************/
		  /* PWM not initialised */
		  /***********************/

		  i_ReturnValue = -5;
		} // if (dw_Status & 0x10)
	    } // if (b_PWM >= 0 && b_PWM <= 1)
	  else
	    {
	      /******************************/
	      /* Tor PWM selection is wrong */
	      /******************************/

	      i_ReturnValue = -4;
	    } // if (b_PWM >= 0 && b_PWM <= 1)
	}
      else
	{
	  /**********************************/
	  /* The module is not a PWM module */
	  /**********************************/

	  i_ReturnValue = -3;
	}
    }
  else
    {
      /***********************/
      /* Module number error */
      /***********************/

      i_ReturnValue = -2;
    }

  return (i_ReturnValue);
}

/*
  +----------------------------------------------------------------------------+
  | Function Name     :  INT  i_APCI1710_GetPWMInitialisation                  |
  |                                       (BYTE      b_BoardHandle,            |
  |                                        BYTE      b_ModulNbr,               |
  |                                        BYTE      b_PWM,                    |
  |                                        PBYTE    pb_TimingUnit,             |
  |                                        PULONG  pul_LowTiming,              |
  |                                        PULONG  pul_HighTiming,             |
  |                                        PBYTE    pb_StartLevel,             |
  |                                        PBYTE    pb_StopMode,               |
  |                                        PBYTE    pb_StopLevel,              |
  |                                        PBYTE    pb_ExternGate,             |
  |                                        PBYTE    pb_InterruptEnable,        |
  |                                        PBYTE    pb_Enable)                 |
  +----------------------------------------------------------------------------+
  | Task              : Return the PWM (b_PWM) initialisation from selected    |
  |                     module (b_ModulNbr). You must calling the              |
  |                     "i_APCI1710_InitPWM" function be for you call this     |
  |                     function.                                              |
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE  b_BoardHandle : Handle of board APCI-1710        |
  |                     BYTE  b_ModulNbr    : Selected module number (0 to 3)  |
  |                     BYTE  b_PWM         : Selected PWM (0 or 1)            |
  +----------------------------------------------------------------------------+
  | Output Parameters : PBYTE   pb_TimingUnit      : Base timing Unit (0 to 4) |
  |                                                       0 : ns               |
  |                                                       1 : æs               |
  |                                                       2 : ms               |
  |                                                       3 : s                |
  |                                                       4 : mn               |
  |                     PULONG  pul_LowTiming      : Low base timing value.    |
  |                     PULONG  pul_HighTiming     : High base timing value.   |
  |                     PBYTE   pb_StartLevel      : Start period level        |
  |                                                  selection                 |
  |                                                       0 : The period start |
  |                                                           with a low level |
  |                                                       1 : The period start |
  |                                                           with a high level|
  |                     PBYTE   pb_StopMode        : Stop mode selection       |
  |                                                  0 : The PWM is stopped    |
  |                                                      directly after the    |
  |                                                     "i_APCI1710_DisablePWM"|
  |                                                      function and break the|
  |                                                      last period           |
  |                                                  1 : After the             |
  |                                                     "i_APCI1710_DisablePWM"|
  |                                                      function the PWM is   |
  |                                                      stopped at the end    |
  |                                                      from last period cycle|
  |                     PBYTE   pb_StopLevel        : Stop PWM level selection |
  |                                                    0 : The output signal   |
  |                                                        keep the level after|
  |                                                        the                 |
  |                                                     "i_APCI1710_DisablePWM"|
  |                                                        function            |
  |                                                    1 : The output signal is|
  |                                                        set to low after the|
  |                                                     "i_APCI1710_DisablePWM"|
  |                                                        function            |
  |                                                    2 : The output signal is|
  |                                                        set to high after   |
  |                                                        the                 |
  |                                                     "i_APCI1710_DisablePWM"|
  |                                                        function            |
  |                     PBYTE   pb_ExternGate      : Extern gate action        |
  |                                                  selection                 |
  |                                                   0 : Extern gate signal   |
  |                                                       not used.            |
  |                                                   1 : Extern gate signal   |
  |                                                       used.                |
  |                     PBYTE   pb_InterruptEnable : Enable or disable the PWM |
  |                                                  interrupt.                |
  |                                                  - APCI1710_ENABLE :       |
  |                                                    Enable the PWM interrupt|
  |                                                    A interrupt occur after |
  |                                                    each period             |
  |                                                  - APCI1710_DISABLE :      |
  |                                                    Disable the PWM         |
  |                                                    interrupt               |
  |                     PBYTE   pb_Enable          : Indicate if the PWM is    |
  |                                                  enabled or no             |
  |                                                       0 : PWM not enabled  |
  |                                                       1 : PWM enabled      |
  +----------------------------------------------------------------------------+
  | Return Value      :  0: No error                                           |
  |                     -1: The handle parameter of the board is wrong         |
  |                     -2: Module selection wrong                             |
  |                     -3: The module is not a PWM module                     |
  |                     -4: PWM selection is wrong                             |
  |                     -5: PWM not initialised see function                   |
  |                         "i_APCI1710_InitPWM"                               |
  +----------------------------------------------------------------------------+
*/

INT   i_APCI1710_GetPWMInitialisation (BYTE     b_BoardHandle,
				       BYTE     b_ModuleNumber,
				       BYTE     b_PWM,
				       PBYTE   pb_TimingUnit,
				       PULONG pul_LowTiming,
				       PULONG pul_HighTiming,
				       PBYTE   pb_StartLevel,
				       PBYTE   pb_StopMode,
				       PBYTE   pb_StopLevel,
				       PBYTE   pb_ExternGate,
				       PBYTE   pb_InterruptEnable,
				       PBYTE   pb_Enable)
{
  INT    i_ReturnValue = 0;
  DWORD dw_Status;
  DWORD dw_Command;
  
  /*******************************************************/
  /* Gets the pointer to the board information structure */
  /*******************************************************/
  str_APCI1710_BoardInformation *BoardInformation = NULL; 
   
  BoardInformation = &(s_APCI1710_DriverStruct.
		       s_BoardInformation[b_BoardHandle]);

  /****************************************/
  /* Checks if the b_BoardHandle is valid */
  /****************************************/
  if (b_BoardHandle >= s_APCI1710_DriverStruct.b_NumberOfBoard)
    {   
      return -1;
    }	

  /**************************/
  /* Test the module number */
  /**************************/

  if (b_ModuleNumber < 4)
    {
      /***************/
      /* Test if PWM */
      /***************/

      if (BoardInformation -> dw_MolduleConfiguration [b_ModuleNumber] == APCI1710_PWM)
	{
	  /**************************/
	  /* Test the PWM selection */
	  /**************************/

	  if (b_PWM >= 0 && b_PWM <= 1)
	    {
	      /***************************/
	      /* Test if PWM initialised */
	      /***************************/

              dw_Status = inl (BoardInformation -> 
		               s_BaseInformation.
		               ui_BaseAddress[2] + 
			       12 + (20 * b_PWM) + (64 * b_ModuleNumber));

	      if (dw_Status & 0x10)
		{
		  /***********************/
		  /* Read the low timing */
		  /***********************/
                  
                  *pul_LowTiming = inl (BoardInformation -> 
					s_BaseInformation.
					ui_BaseAddress[2] + 
					(20 * b_PWM) + (64 * b_ModuleNumber));

		  /************************/
		  /* Read the high timing */
		  /************************/

                  *pul_HighTiming = inl (BoardInformation -> 
					 s_BaseInformation.
					 ui_BaseAddress[2] + 
					 4 + (20 * b_PWM) + (64 * b_ModuleNumber));

		  /********************/
		  /* Read the command */
		  /********************/

                  dw_Command = inl (BoardInformation -> 
				    s_BaseInformation.
				    ui_BaseAddress[2] + 
				    8 + (20 * b_PWM) + (64 * b_ModuleNumber));

		  *pb_StartLevel      = (BYTE) ((dw_Command >> 5) & 1);
		  *pb_StopMode        = (BYTE) ((dw_Command >> 0) & 1);
		  *pb_StopLevel       = (BYTE) ((dw_Command >> 1) & 1);
		  *pb_ExternGate      = (BYTE) ((dw_Command >> 4) & 1);
		  *pb_InterruptEnable = (BYTE) ((dw_Command >> 3) & 1);

		  if (*pb_StopLevel)
		    {
		      *pb_StopLevel = *pb_StopLevel + (BYTE) ((dw_Command >> 2) & 1);
		    }

		  /********************/
		  /* Read the command */
		  /********************/

                  dw_Command = inl (BoardInformation -> 
				    s_BaseInformation.
				    ui_BaseAddress[2] + 
				    8 + (20 * b_PWM) + (64 * b_ModuleNumber));
		  

		  *pb_Enable = (BYTE) ((dw_Command >> 0) & 1);

		  *pb_TimingUnit = s_APCI1710_DriverStruct.
		    s_BoardInformation[b_BoardHandle].
		    s_ModuleInformation[b_ModuleNumber].
		    s_PWMModuleInfo.
		    s_PWMInfo [b_PWM].
		    b_TimingUnit;
		} // if (dw_Status & 0x10)
	      else
		{
		  /***********************/
		  /* PWM not initialised */
		  /***********************/

		  i_ReturnValue = -5;
		} // if (dw_Status & 0x10)
	    } // if (b_PWM >= 0 && b_PWM <= 1)
	  else
	    {
	      /******************************/
	      /* Tor PWM selection is wrong */
	      /******************************/

	      i_ReturnValue = -4;
	    } // if (b_PWM >= 0 && b_PWM <= 1)
	}
      else
	{
	  /**********************************/
	  /* The module is not a PWM module */
	  /**********************************/

	  i_ReturnValue = -3;
	}
    }
  else
    {
      /***********************/
      /* Module number error */
      /***********************/

      i_ReturnValue = -2;
    }

  return (i_ReturnValue);
}

/*
  +----------------------------------------------------------------------------+
  | Function Name     :  INT  i_APCI1710_GetPWMStatus                          |
  |                               (BYTE     b_BoardHandle,                     |
  |                                BYTE     b_ModulNbr,                        |
  |                                BYTE     b_PWM,                             |
  |                                PBYTE   pb_PWMOutputStatus,                 |
  |                                PBYTE   pb_ExternGateStatus)                |
  +----------------------------------------------------------------------------+
  | Task              : Return the status from selected PWM (b_PWM) from       |
  |                     selected module (b_ModulNbr).                          |
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE   b_BoardHandle : Handle of board APCI-1710       |
  |                     BYTE   b_PWM         : Selected PWM (0 or 1)           |
  |                     BYTE   b_ModulNbr    : Selected module number (0 to 3) |
  +----------------------------------------------------------------------------+
  | Output Parameters : PBYTE   pb_PWMOutputStatus  : Return the PWM output    |
  |                                                   level status.            |
  |                                                    0 : The PWM output level|
  |                                                        is low.             |
  |                                                    1 : The PWM output level|
  |                                                        is high.            |
  |                     PBYTE   pb_ExternGateStatus : Return the extern gate   |
  |                                                   level status.            |
  |                                                    0 : The extern gate is  |
  |                                                        low.                |
  |                                                    1 : The extern gate is  |
  |                                                        high.               |
  +----------------------------------------------------------------------------+
  | Return Value      :  0: No error                                           |
  |                     -1: The handle parameter of the board is wrong         |
  |                     -2: Module selection wrong                             |
  |                     -3: The module is not a PWM module                     |
  |                     -4: PWM selection is wrong                             |
  |                     -5: PWM not initialised see function                   |
  |                         "i_APCI1710_InitPWM"                               |
  |                     -6: PWM not enabled see function "i_APCI1710_EnablePWM"|
  +----------------------------------------------------------------------------+
*/

INT   i_APCI1710_GetPWMStatus (BYTE    b_BoardHandle,
			       BYTE    b_ModuleNumber,
			       BYTE    b_PWM,
			       PBYTE  pb_PWMOutputStatus,
			       PBYTE  pb_ExternGateStatus)
{
  INT    i_ReturnValue = 0;
  DWORD dw_Status;
    
  /*******************************************************/
  /* Gets the pointer to the board information structure */
  /*******************************************************/
  str_APCI1710_BoardInformation *BoardInformation = NULL; 
   
  BoardInformation = &(s_APCI1710_DriverStruct.
		       s_BoardInformation[b_BoardHandle]);

  /****************************************/
  /* Checks if the b_BoardHandle is valid */
  /****************************************/
  if (b_BoardHandle >= s_APCI1710_DriverStruct.b_NumberOfBoard)
    {   
      return -1;
    }	
   
  /**************************/
  /* Test the module number */
  /**************************/

  if (b_ModuleNumber < 4)
    {
      /***************/
      /* Test if PWM */
      /***************/

      if (BoardInformation -> dw_MolduleConfiguration [b_ModuleNumber] == APCI1710_PWM)
	{
	  /**************************/
	  /* Test the PWM selection */
	  /**************************/

	  if (b_PWM >= 0 && b_PWM <= 1)
	    {
	      /***************************/
	      /* Test if PWM initialised */
	      /***************************/

	      dw_Status = inl (BoardInformation -> 
		               s_BaseInformation.
		               ui_BaseAddress[2] + 
			       12 + (20 * b_PWM) + (64 * b_ModuleNumber));

	      if (dw_Status & 0x10)
		{
		  /***********************/
		  /* Test if PWM enabled */
		  /***********************/

		  if (dw_Status & 0x1)
		    {
		      *pb_PWMOutputStatus  = (BYTE) ((dw_Status >> 7) & 1);
		      *pb_ExternGateStatus = (BYTE) ((dw_Status >> 6) & 1);
		    } // if (dw_Status & 0x1)
		  else
		    {
		      /*******************/
		      /* PWM not enabled */
		      /*******************/

		      i_ReturnValue = -6;
		    } // if (dw_Status & 0x1)
		} // if (dw_Status & 0x10)
	      else
		{
		  /***********************/
		  /* PWM not initialised */
		  /***********************/

		  i_ReturnValue = -5;
		} // if (dw_Status & 0x10)
	    } // if (b_PWM >= 0 && b_PWM <= 1)
	  else
	    {
	      /******************************/
	      /* Tor PWM selection is wrong */
	      /******************************/

	      i_ReturnValue = -4;
	    } // if (b_PWM >= 0 && b_PWM <= 1)
	}
      else
	{
	  /**********************************/
	  /* The module is not a PWM module */
	  /**********************************/

	  i_ReturnValue = -3;
	}
    }
  else
    {
      /***********************/
      /* Module number error */
      /***********************/

      i_ReturnValue = -2;
    }

  return (i_ReturnValue);
}

/*
  +----------------------------------------------------------------------------+
  | Function Name     :  INT i_APCI1710_SearchAllAPCI1710 (VOID)               |
  +----------------------------------------------------------------------------+
  | Task              : Search all APCI-1710 on the PCI bus                    |
  +----------------------------------------------------------------------------+
  | Input Parameters  : ---                                                    |
  +----------------------------------------------------------------------------+
  | Output Parameters : ---                                                    |
  +----------------------------------------------------------------------------+
  | Return Value      :  Return the number of APCI-1710 found, else 0          |
  +----------------------------------------------------------------------------+
*/

INT i_APCI1710_SearchAllAPCI1710 (VOID)
{
  int i                                   = 0;
  struct pci_dev *pPCILinuxStruct         = NULL;

  printk("\n +------------------------------+\n");
  printk(" | ADDI-DATA GmbH -+- APCI-1710 |\n");
  printk(" +------------------------------+\n");
  printk(" |      Module installation     |\n");
  printk(" +------------------------------+\n");
	
  s_APCI1710_DriverStruct.b_NumberOfBoard = 0;


  if(!pci_present())
    {
      printk("<1> NO PCI BUS;\n");
    }      
  else
    {
      //Filling str_PCI_Information stucture
      while ((pPCILinuxStruct = pci_find_device (APCI1710_BOARD_VENDOR_ID, 
						 APCI1710_BOARD_DEVICE_ID, 
						 pPCILinuxStruct)))
	{
	  for (i = 0; i < 5; i++)
	    {
	      s_APCI1710_DriverStruct.s_BoardInformation
		[s_APCI1710_DriverStruct.b_NumberOfBoard]
		.s_BaseInformation.ui_BaseAddress[i] = 
		(UINT) PCI_BASE_ADDRESS_IO_MASK & 
		pPCILinuxStruct->resource[i].start;
	    }
	  s_APCI1710_DriverStruct.s_BoardInformation
	    [s_APCI1710_DriverStruct.b_NumberOfBoard]
	    .s_BaseInformation.b_Interrupt =
	    (BYTE) pPCILinuxStruct->irq;
	  s_APCI1710_DriverStruct.s_BoardInformation
	    [s_APCI1710_DriverStruct.b_NumberOfBoard]
	    .s_BaseInformation.b_SlotNumber = 
	    (BYTE)  PCI_SLOT(pPCILinuxStruct->devfn);	
	    
	  s_APCI1710_DriverStruct.s_BoardInformation
	    [s_APCI1710_DriverStruct.b_NumberOfBoard].b_BoardVersion = 1;    	    

	  s_APCI1710_DriverStruct.b_NumberOfBoard++;
	  printk(" |      NumberOfBoard: %2d       |\n",s_APCI1710_DriverStruct.b_NumberOfBoard);
	  printk(" +------------------------------+\n");
	  printk(" | Baseaddress[0,1,2]:          |\n");
	  printk(" |      %4lX, %4lX, %4lX        |\n", pPCILinuxStruct->resource[0].start, pPCILinuxStruct->resource[1].start, pPCILinuxStruct->resource[2].start);   
	  printk(" | Interrupt No.:               |\n");
	  printk(" |      %2d                      |\n",pPCILinuxStruct->irq);   
	  printk(" +------06.03.2002---15:42------+\n");
	  
	  /****************************/
	  /* Get module configuration */
	  /****************************/	  
	  v_APCI1710_ReadAllModuleConfiguration (s_APCI1710_DriverStruct.b_NumberOfBoard-1);	  
		
	}
    }
    
  return (s_APCI1710_DriverStruct.b_NumberOfBoard);	
}


