/*
  +-----------------------------------------------------------------------+
  | (C) ADDI-DATA GmbH         Dieselstrasse 3       D-77833 Ottersweier  |
  +-----------------------------------------------------------------------+
  | Tel : +49 (0) 7223/9493-0     | email    : info@addi-data.com         |
  | Fax : +49 (0) 7223/9493-92    | Internet : http://www.addi-data.com   |
  +-------------------------------+---------------------------------------+
  | Project     : APCI-3120       |     Compiler   : GCC                  |
  | Module name : apci3120.c      |     Version    : 2.96                 |
  +-------------------------------+---------------------------------------+
  | Author      : J. Krauth       |     Date       : 18.06.2004           |
  +-------------------------------+---------------------------------------+
  | Description :                                                         |
  |               Includes the following functions for the APCI-3120      |
  |                                                                       |
  |                 - i_APCI3120_InitAnalogInput ()                       |
  |                 - i_APCI3120_StartAnalogInput ()                      |
  |                 - i_APCI3120_ReadAnalogInputBit ()                    |
  |                 - i_APCI3120_ReadAnalogInputValue ()                  |
  |                 - i_APCI3120_InitAnalogInputScan ()                   |
  |                 - i_APCI3120_StartAnalogInputScan ()                  |
  |                 - i_APCI3120_ReadAnalogInputScanBit ()                |
  |                 - i_APCI3120_ReadAnalogInputScanValue ()              |
  |                 - i_APCI3120_InitAnalogOutput ()                      |
  |                 - i_APCI3120_ReadAnalogOutputBit ()                   |
  |                 - i_APCI3120_Write1AnalogOutput ()                    |
  |                 - i_APCI3120_WriteMoreAnalogOutput ()                 |
  |                 - i_APCI3120_Read1DigitalInput ()                     |
  |                 - i_APCI3120_Read4DigitalInput ()                     |
  |                 - i_APCI3120_SetOutputMemoryOn ()                     |
  |                 - i_APCI3120_SetOutputMemoryOff ()                    |
  |                 - i_APCI3120_Set1DigitalOutputOn ()                   |
  |                 - i_APCI3120_Set1DigitalOutputOff ()                  |
  |                 - i_APCI3120_Set4DigitalOutputOn ()                   |
  |                 - i_APCI3120_Set4DigitalOutputOff ()                  |
  |                 - i_APCI3120_InitTimerWatchdog ()                     |
  |                 - i_APCI3120_StartTimerWatchdog ()                    |
  |                 - i_APCI3120_StopTimerWatchdog ()                     |
  |                 - i_APCI3120_ReadTimer ()                             |
  |                 - i_APCI3120_SetBoardInterruptRoutine ()              |
  |                 - i_APCI3120_InitAnalogInputAcquisition ()            |
  |                 - i_APCI3120_StartAnalogInputAcquisition ()           |
  |                 - i_APCI3120_StopAnalogInputAcquisition ()            |
  |                 - i_APCI3120_ClearAnalogInputAcquisition ()           |
  |                                                                       |
  +-----------------------------------------------------------------------+
  |                             UPDATES                                   |
  +----------+-----------+------------------------------------+-----------+
  |   Date   |   Author  |  Description of updates            | Version   |
  +----------+-----------+------------------------------------+-----------+
  | 18.06.04 | J. Krauth |  Creation                          | 01.00.00  |
  +----------*-----------*------------------------------------*-----------+
  | 21.06.04 | J. Krauth |  Append a test on allocated memory | 01.00.01  |
  +----------*-----------*------------------------------------*-----------+  
*/

//#include <Standard.h>
//#include <xPCI3120.h>

#include <apci3120.h>
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

RCS_ID("$Header: /home/hm/projekte/msr_messen_steuern_regeln/linux/kernel_space/simulator/rt/rt_lib/msr-hwdriver/RCS/apci3120.c,v 1.1 2005/06/14 12:34:23 hm Exp $");

#define NomDRV "APCI3120"

//********************************* Modul information *********************************
//MODULE_LICENSE("GPL");
//MODULE_AUTHOR("ADDI-DATA GmbH <info@addi-data.com>");
//MODULE_DESCRIPTION("MSX-Box APCI-3120 RTAI Module");
//*************************************************************************************

  // This value can be changed (if needed)
#define MAX_NUMBER_OF_VALUE 5000

typedef struct
{
  uint8 ReceiveInterrupt;
  uint8 handle;
  uint8 intMask;
  uint16 ui_SaveArray [MAX_NUMBER_OF_VALUE];
  uint16 ui_TimerIntCpt;  
	
} str_InterruptValue;


/*-------------------------------------------------------------------------*\
** DATA
\*-------------------------------------------------------------------------*/

str_InterruptValue APCI3120_InterruptValue;
xPCI3120_DEV    xPci3120_data[MAX_NO_3120];	/* Information array for all modules found */
int             xPci3120_nrDev = 0;	/* The number of xPCI-3120 modules found */

/*
**	The following describes the sequence table that is used to tell the board how it shall 
** work on the different channels.
** Each value represents one channel and is structured as follows:
**
**	Bit # 0..3  - channel number
** Bit # 4..6  - gain, adjustable values are: 0=1, 1=2, 2=5, 4=10
** Bit # 7     - 0=bipolar, 1 unipolar mode
** Bit # 8..15	- address of the sequence table value
**
** Per default, we treat each board and channel in the same way: bipolar mode, gain=1.
*/
uint16 SequenceTable[8] =
  {
    0x0000,
    0x0101,
    0x0202,
    0x0303,
    0x0404,
    0x0505,
    0x0606,
    0x0707
  };

/*
  +----------------------------------------------------------------------------+
  | Function   Name   : INT     i_APCI3120_InitAnalogInput                     |
  |                                               (BYTE     b_BoardHandle,     |
  |                                                BYTE     b_ChannelNbr,      |
  |                                                BYTE     b_Gain,            |
  |                                                BYTE     b_Polarity,        |
  |                                                UINT    ui_ConvertTiming)   |
  +----------------------------------------------------------------------------+
  | Task              : Initialises the the Reading of one analog input channel|
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE   b_BoardHandle  : Handle of the APCI-3120        |
  |                     BYTE   b_ChannelNbr   : Number of the input to be read:|
  |                                              (1 to 16)                     |
  |                     BYTE   b_Gain         : Gain factor of the analog input|
  |                                             - APCI3120_1_GAIN : Gain =  1  |
  |                                             - APCI3120_2_GAIN : Gain =  2  |
  |                                             - APCI3120_5_GAIN : Gain =  5  |
  |                                             - APCI3120_10_GAIN: Gain = 10  |
  |                     BYTE   b_Polarity     : Polarity of the analog input:  |
  |                                             - APCI3120_UNIPOLAR:  0V - +10V|
  |                                             - APCI3120_BIPOLAR: -10V - +10V|
  |                     UINT  ui_ConvertTiming: Sets the convert timing of the |
  |                                             analog input:                  |
  |                                             from 10us  to 32767us          |
  +----------------------------------------------------------------------------+
  | Output Parameters : ---                                                    |
  |                                                                            |
  +----------------------------------------------------------------------------+
  | Return Value      :  0 : No error                                          |
  |                     -1 : The handle parameter of the board is wrong        |
  |                     -2 : Channel number is not between 1 and 16            |
  |                     -3 : The parametered gain factor is wrong              |
  |                     -4 : The parametered polarity factor is wrong          |
  |                     -5 : The parametered convert timing is wrong           |
  +----------------------------------------------------------------------------+
*/
INT i_APCI3120_InitAnalogInput (BYTE b_BoardHandle,
                                BYTE    b_ChannelNumber,
                                BYTE    b_Gain, 
                                BYTE    b_Polarity,
		                UINT   ui_ConvertTiming)
{
  uint8  b_DummyRead      = 0;
  uint16 ui_DummyRead = 0;
  uint b_InterruptFlag;
  int i_ReturnValue = 0;
  xPCI3120_DEV   *pDev = xPci3120_data;   
  pDev = &xPci3120_data[b_BoardHandle];	/* set up a pointer to the relevant info-structure */

  // Let the possibility to use the interrupt is APCI3120_ENABLE, but was not tested
  b_InterruptFlag = APCI3120_DISABLE;
   
  if (b_BoardHandle < xPci3120_nrDev)
    { 					            
      /* Checks the channel number */
      if (b_ChannelNumber < pDev->nrAnaIn)
	{
	  pDev->lastChannel = b_ChannelNumber;
            
	  /* Tests the gain */
	  if (b_Gain == APCI3120_1_GAIN  || b_Gain == APCI3120_2_GAIN  || b_Gain == APCI3120_5_GAIN  || b_Gain == APCI3120_10_GAIN)
	    {
	      /* Tests the polarity */
	      if (b_Polarity == APCI3120_UNIPOLAR || b_Polarity == APCI3120_BIPOLAR)
		{
		  if ( (10 <= ui_ConvertTiming) && (ui_ConvertTiming <= 32767) )
		    {
                     
		      if (((b_InterruptFlag == APCI3120_DISABLE) || (b_InterruptFlag == APCI3120_ENABLE)) && (pDev->interruptInitialized == APCI3120_ENABLE))
                        {
			  if (b_InterruptFlag == APCI3120_ENABLE)
			    pDev->intMode = APCI3120_EOC_INT_MODE;
			  else
			    pDev->intMode = 0;
                        }
		      /* Initialises the sequence array */
		      outw ((0 << 8) + (b_ChannelNumber | b_Gain | b_Polarity), pDev->addr[1] + APCI3120_FIFO_ADDRESS);
			   
		      /* Clears the FIFO of the board */
		      b_DummyRead = inb (pDev->addr[1] + APCI3120_RESET_FIFO);
					       
		      /* Initializes the timer 0 mode 5 (Start by trigger) */
		      pDev->crt1 = (pDev->crt1 & 0xFC) | APCI3120_TIMER_0_MODE_5;
			     
		      outb (pDev->crt1, pDev->addr[1] + APCI3120_TIMER_CRT1);

		      /* Writes the interrupt select register */
		      if (b_InterruptFlag == APCI3120_ENABLE)
			pDev->modeSelect = ((pDev->modeSelect & 0x34) | APCI3120_ENABLE_EOC_INT);
		      else
			pDev->modeSelect = pDev->modeSelect & 0x34;
                           
		      outb (pDev->modeSelect, (pDev->addr[1] + APCI3120_WRITE_MODE_SELECT));

		      ui_DummyRead = inw(pDev->addr[1] + APCI3120_RD_STATUS);

		      /* Sets gate 0 */
		      pDev->nWrAddr = (pDev->nWrAddr & APCI3120_CLEAR_PA_PR) |  APCI3120_ENABLE_TIMER0;
		      outw (pDev->nWrAddr, pDev->addr[1] + APCI3120_WR_ADDRESS);
				  
		      /* Converts the Convertion Time to the right format */
		      ui_ConvertTiming = (uint16) (((uint32) ((uint32) (ui_ConvertTiming) * 2L)) - 2);

		      pDev->crt0 = (pDev->crt0 & 0xF0) | APCI3120_SELECT_TIMER_0_WORD;
		      outb (pDev->crt0, pDev->addr[1] +  APCI3120_TIMER_CRT0);

		      /* Writes the conversion timing */
		      outw (ui_ConvertTiming, pDev->addr[1] + APCI3120_TIMER_VALUE);
		    }
		  else
		    {
		      /* Wrong convert timing selection */
		      i_ReturnValue = -5;      	 
		    }		
		}  
	      else
		{
		  /* Wrong polarity selection */
		  i_ReturnValue = -4;      	 
		}
	    }
	  else
	    {
	      /* Wrong gain selection */
	      i_ReturnValue = -3;      
	    }
	}
      else
	{
	  /* Wrong channel number */
	  i_ReturnValue = -2;
	}
    }	
  else
    {
      /* The boardhandle is wrong */
      i_ReturnValue = -1;
    }
      
  return (i_ReturnValue);
}

/*
  +----------------------------------------------------------------------------+
  | Function   Name   : INT     i_APCI3120_StartAnalogInput                    |
  |                                               (BYTE     b_BoardHandle)     |
  +----------------------------------------------------------------------------+
  | Task              : Starts the reading of one analog input channel         | 
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE   b_BoardHandle  : Handle of the APCI-3120        |
  +----------------------------------------------------------------------------+
  | Output Parameters : ---                                                    |
  |                                                                            |
  +----------------------------------------------------------------------------+
  | Return Value      :  0 : No error                                          |
  |                     -1 : The handle parameter of the board is wrong        |
  +----------------------------------------------------------------------------+
*/
INT i_APCI3120_StartAnalogInput (BYTE b_BoardHandle)
{
  uint16 ui_DummyWrite  = 0;
  int i_ReturnValue = 0;
  xPCI3120_DEV   *pDev = xPci3120_data;   
  pDev = &xPci3120_data[b_BoardHandle];	/* set up a pointer to the relevant info-structure */
  
  if (b_BoardHandle < xPci3120_nrDev)
    { 	
    			   
      /* Dummy write for the software trigger to start the conversation */
      outw (ui_DummyWrite, pDev->addr[1] + APCI3120_RD_STATUS);
    }	
  else
    {
      /* The boardhandle is wrong */
      i_ReturnValue = -1;
    }
  return (i_ReturnValue);
}

/*
  +----------------------------------------------------------------------------+
  | Function   Name   : INT     i_APCI3120_ReadAnalogInputBit                  |
  |                                               (BYTE     b_BoardHandle)     |
  +----------------------------------------------------------------------------+
  | Task              : Reads the EOC Bit of the analog input conversion       | 
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE   b_BoardHandle  : Handle of the APCI-3120        |
  +----------------------------------------------------------------------------+
  | Output Parameters : ---                                                    |
  |                                                                            |
  +----------------------------------------------------------------------------+
  | Return Value      :  0 : No error                                          |
  |                     -1 : The handle parameter of the board is wrong        |
  |                     -2 : The EOC Bit was not set since the last start      |
  +----------------------------------------------------------------------------+
*/
INT i_APCI3120_ReadAnalogInputBit (BYTE b_BoardHandle)
{
  uint16 ui_StatusValue = 0;
  int i_ReturnValue = 0;
  xPCI3120_DEV   *pDev = xPci3120_data;   
  pDev = &xPci3120_data[b_BoardHandle];	/* set up a pointer to the relevant info-structure */
  
  if (b_BoardHandle < xPci3120_nrDev)
    { 	

      /* Reads the EOC (end of conversion) flag (low active) */
      ui_StatusValue = inw (pDev->addr[1] + APCI3120_RD_STATUS);
		    
      /* Returns -1 if the EOC flag is not ready */
      if ((ui_StatusValue & APCI3120_EOC) == APCI3120_EOC)
	{
	  i_ReturnValue = -2;
	}
    }	
  else
    {
      /* The boardhandle is wrong */
      i_ReturnValue = -1;
    }

  return (i_ReturnValue);
}

/*
  +----------------------------------------------------------------------------+
  | Function   Name   : INT     i_APCI3120_InitAnalogInputScan                 |
  |                                               (BYTE     b_BoardHandle,     |
  |                                                PBYTE    b_GainArray,       |
  |                                                PBYTE    b_PolarityArray,   |
  |                                                UINT    ui_ConvertTiming)   |
  +----------------------------------------------------------------------------+
  | Task              : Initialises the the Reading of one analog input channel|
  |                     in Scan mode. This reads all 16 inputs of the board.   |
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE   b_BoardHandle  : Handle of the APCI-3120        |
  |                     PBYTE pb_GainArray    : Array[16] for the Gain factor  |
  |                                             of the analog inputs           |
  |                                             - APCI3120_1_GAIN : Gain =  1  |
  |                                             - APCI3120_2_GAIN : Gain =  2  |
  |                                             - APCI3120_5_GAIN : Gain =  5  |
  |                                             - APCI3120_10_GAIN: Gain = 10  |
  |                     PBYTE pb_PolarityArray: Array[16] for the polarity of  |
  |                                             the analog inputs:             |
  |                                             - APCI3120_UNIPOLAR:  0V - +10V|
  |                                             - APCI3120_BIPOLAR: -10V - +10V|
  |                     UINT  ui_ConvertTiming: Sets the convert timing of the |
  |                                             analog inputs:                 |
  |                                             from 10us  to 32767us          |
  +----------------------------------------------------------------------------+
  | Output Parameters : ---                                                    |
  |                                                                            |
  +----------------------------------------------------------------------------+
  | Return Value      :  0 : No error                                          |
  |                     -1 : The handle parameter of the board is wrong        |
  |                     -2 : The parametered gain factor is wrong              |
  |                     -3 : The parametered polarity factor is wrong          |
  |                     -4 : The parametered convert timing is wrong           |
  |                     -5 : The parametered channel array size is wrong       |
  |                     -6 : The parametered channels are wrong                |
  +----------------------------------------------------------------------------+
*/

INT i_APCI3120_InitAnalogInputScan ( BYTE    b_BoardHandle,
                                     BYTE    b_ChannelArraySize,
				     PBYTE   pb_ChannelArray,
				     PBYTE   pb_GainArray, 
				     PBYTE   pb_PolarityArray,
				     UINT    ui_ConvertTiming)
{
  uint8  b_Cpt        = 0;
  uint16 ui_DummyRead = 0;		   
  uint8  b_DummyRead  = 0;
  uint b_InterruptFlag;
  uint16 ui_ConvertTiming_tmp ;
  uint16 seqTable[16];	/* holds the sequence table according How-To, pg. 12 */	
  int i_ReturnValue = 0;
  xPCI3120_DEV   *pDev = xPci3120_data;   
  pDev = &xPci3120_data[b_BoardHandle];	/* set up a pointer to the relevant info-structure */

  b_InterruptFlag = APCI3120_DISABLE;
  
  if (b_BoardHandle < xPci3120_nrDev)
    { 	

      /* Tests the gain */
      if (b_ChannelArraySize <= pDev->nrAnaIn)
	{
	  pDev->dma.nrAcq = b_ChannelArraySize;
	  for (b_Cpt = 0; b_Cpt < pDev->dma.nrAcq; b_Cpt ++)
	    {
	      if (pb_GainArray[b_Cpt] == APCI3120_1_GAIN  || pb_GainArray[b_Cpt] == APCI3120_2_GAIN  ||  pb_GainArray[b_Cpt] == APCI3120_5_GAIN  ||  pb_GainArray[b_Cpt] == APCI3120_10_GAIN)
		{
		  /* Tests the polarity */
		  if ((pb_PolarityArray[b_Cpt] == APCI3120_UNIPOLAR) || (pb_PolarityArray[b_Cpt] == APCI3120_BIPOLAR))
		    {
		      /* Tests the conversion timing */
		      if ((ui_ConvertTiming >= 10) && (ui_ConvertTiming < 32767U))
			{
			  seqTable[b_Cpt] = ((uint16) b_Cpt << 8) | (pb_GainArray[b_Cpt] & 0xFF) | (pb_PolarityArray [b_Cpt] & 0xFF) | (b_Cpt & 0xFF );

			  if ((b_InterruptFlag == APCI3120_DISABLE) || ((b_InterruptFlag == APCI3120_ENABLE) && (pDev->interruptInitialized == APCI3120_ENABLE)))
			    {
			      if (b_InterruptFlag == APCI3120_ENABLE)
				pDev->intMode = APCI3120_EOS_INT_MODE;
			      else
				pDev->intMode = 0;
			    }


			  /* Write the sequence table */								 
			  outw (seqTable[b_Cpt], pDev->addr[1] + APCI3120_FIFO_ADDRESS);

			  /* Clears the FIFO of the board */
			  b_DummyRead = inb (pDev->addr[1] + APCI3120_RESET_FIFO);

			  /* Initializes the timer 0 mode 2 (Start by command) */
			  pDev->crt1	= (pDev->crt1 & 0xFC) | APCI3120_TIMER_0_MODE_2;

			  outb (pDev->crt1, pDev->addr[1] + APCI3120_TIMER_CRT1);

			  /* Selects timer 0 */
			  pDev->crt0 = (pDev->crt0 & 0xf0) | APCI3120_SELECT_TIMER_0_WORD;
			  outb (pDev->crt0, pDev->addr[1] + APCI3120_TIMER_CRT0);    

			  /* Sets the conversion time */
			  ui_ConvertTiming_tmp = (uint16) ((uint32) (((uint32) ((((uint32) (ui_ConvertTiming)) * 2L))))) - 2;
			  outw (ui_ConvertTiming_tmp, pDev->addr[1] + APCI3120_TIMER_VALUE);

			  /* Writes the interrupt select register */
			  if (b_InterruptFlag == APCI3120_ENABLE)
			    pDev->modeSelect = ((pDev->modeSelect & 0x34) | APCI3120_ENABLE_SCAN |APCI3120_ENABLE_EOS_INT);
			  else
			    pDev->modeSelect = (pDev->modeSelect & 0x34) | APCI3120_ENABLE_SCAN;
		                     
			  outb (pDev->modeSelect, pDev->addr[1] + APCI3120_WRITE_MODE_SELECT);
								
			  ui_DummyRead = inw (pDev->addr[1] + APCI3120_RD_STATUS);
			}   
		      else
			{
			  /* Wrong conversion timing selection */
			  i_ReturnValue = -4;	       
			}
		    }  
		  else
		    {
		      /* Wrong polarity selection */
		      i_ReturnValue = -3;      	 
		    }
		}
	      else
		{
		  /* Wrong gain selection */
		  i_ReturnValue = -2;      
		}
	    }
	}
      else
	i_ReturnValue = -5;
    }
  else
    {
      /* The boardhandle is wrong */
      i_ReturnValue = -1;
    }

  return (i_ReturnValue);
}

/*
  +----------------------------------------------------------------------------+
  | Function   Name   : INT     i_APCI3120_StartAnalogInputScan                |
  |                                               (BYTE     b_BoardHandle)     |
  +----------------------------------------------------------------------------+
  | Task              : Starts the reading of the analog input scan            | 
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE   b_BoardHandle  : Handle of the APCI-3120        |
  +----------------------------------------------------------------------------+
  | Output Parameters : ---                                                    |
  |                                                                            |
  +----------------------------------------------------------------------------+
  | Return Value      :  0 : No error                                          |
  |                     -1 : The handle parameter of the board is wrong        |
  +----------------------------------------------------------------------------+
*/

INT i_APCI3120_StartAnalogInputScan (BYTE b_BoardHandle)
{
  uint16 ui_DummyWrite  = 0;
  int i_ReturnValue = 0;
  xPCI3120_DEV   *pDev = xPci3120_data;   
  pDev = &xPci3120_data[b_BoardHandle];	/* set up a pointer to the relevant info-structure */
  
  if (b_BoardHandle < xPci3120_nrDev)
    { 


      /* Writes the new Output Register */
      pDev->nWrAddr = (pDev->nWrAddr & APCI3120_CLEAR_PA_PR ) | APCI3120_ENABLE_TIMER0 |((uint16) (pDev->dma.nrAcq-1) << 8);  /*  No. Channels -1 --> aus Struktur nehmen!*/
      outw (pDev->nWrAddr, pDev->addr[1] + APCI3120_WR_ADDRESS);
			   
      /* Dummy write to start the conversation */
      outw (ui_DummyWrite, pDev->addr[1] + APCI3120_START_CONVERSION);
    }
  else
    {
      /* The boardhandle is wrong */
      i_ReturnValue = -1;
    }

  return (i_ReturnValue);
}

/*
  +----------------------------------------------------------------------------+
  | Function   Name   : INT     i_APCI3120_ReadAnalogInputScanBit              |
  |                                               (BYTE     b_BoardHandle)     |
  +----------------------------------------------------------------------------+
  | Task              : Reads the EOS Bit of the analog input scan             | 
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE   b_BoardHandle  : Handle of the APCI-3120        |
  +----------------------------------------------------------------------------+
  | Output Parameters : ---                                                    |
  |                                                                            |
  +----------------------------------------------------------------------------+
  | Return Value      :  0 : No error                                          |
  |                     -1 : The handle parameter of the board is wrong        |
  |                     -2 : The EOS Bit was not set since the last scan start |
  +----------------------------------------------------------------------------+
*/
int i_APCI3120_ReadAnalogInputScanBit (BYTE b_BoardHandle)
{
  uint16 ui_StatusValue; 
  int i_ReturnValue = 0;
  xPCI3120_DEV   *pDev = xPci3120_data;   
  pDev = &xPci3120_data[b_BoardHandle];	/* set up a pointer to the relevant info-structure */
  
  if (b_BoardHandle < xPci3120_nrDev)
    { 
				
      /* Reads the EOS (end of conversion) flag (high active) */
      ui_StatusValue = inw (pDev->addr[1] + APCI3120_RD_STATUS);
				
      /* Returns -2 if the conversion isn't complete */
      if ((ui_StatusValue & APCI3120_EOS) != APCI3120_EOS)
	{
	  i_ReturnValue = -2;
	}
    }
  else
    {
      /* The boardhandle is wrong */
      i_ReturnValue = -1;
    }

  return (i_ReturnValue);
}

/*
  +----------------------------------------------------------------------------+
  | Function   Name   : INT     i_APCI3120_ReadAnalogInputScanValue            |
  |                                         (BYTE     b_BoardHandle,           |
  |                                          PUINT  pui_AnalogInputValueArray) |
  +----------------------------------------------------------------------------+
  | Task              : Reads the values of the analog input channels          | 
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE   b_BoardHandle  : Handle of the APCI-3120        |
  +----------------------------------------------------------------------------+
  | Output Parameters : PUINT pui_AnalogInputValueArray: Array for the analog  |
  |                                                      input values          |
  |                                                                            |
  +----------------------------------------------------------------------------+
  | Return Value      :  0 : No error                                          |
  |                     -1 : The handle parameter of the board is wrong        |
  +----------------------------------------------------------------------------+
*/
int i_APCI3120_ReadAnalogInputScanValue (BYTE b_BoardHandle,
                                         PUINT pui_AnalogInputValueArray)
{
  uint8  b_Cpt         = 0;
  int i_ReturnValue = 0;
  xPCI3120_DEV   *pDev = xPci3120_data;   
  pDev = &xPci3120_data[b_BoardHandle];	/* set up a pointer to the relevant info-structure */
  
  if (b_BoardHandle < xPci3120_nrDev)
    { 	
      /* Reads the analog input value */
      for (b_Cpt = 0; b_Cpt < pDev->nrAnaIn; b_Cpt ++) /* Anzahl Channels!*/
	{
	  pui_AnalogInputValueArray[b_Cpt] = inw (pDev->addr[1]);
	}
    }
  else
    {
      /* The boardhandle is wrong */
      i_ReturnValue = -1;
    }

  return (i_ReturnValue);
}

/*
  +----------------------------------------------------------------------------+
  | Function   Name   : INT     i_APCI3120_InitAnalogOutput                    |
  |                                               (BYTE     b_BoardHandle,     |
  |                                                BYTE     b_ChannelNbr,      |
  |                                                BYTE     b_Polarity,        |
  +----------------------------------------------------------------------------+
  | Task              : Configures the analog outputs with the wished Polarity |
  |                     this is neccessary for each channel which is set.      |
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE   b_BoardHandle  : Handle of the APCI-3120        |
  |                     BYTE   b_ChannelNbr.  : Sets the channel number to     |
  |                                             configure (0 - 7)SE; (0 - 3)Dif|
  |                     BYTE   b_Polarity     : Sets the poalirty for the      |
  |                                             analog output:                 |
  |                                             - APCI3120_UNIPOLAR:  0V - +10V|
  |                                             - APCI3120_BIPOLAR: -10V - +10V|
  +----------------------------------------------------------------------------+
  | Output Parameters : ---                                                    |
  |                                                                            |
  +----------------------------------------------------------------------------+
  | Return Value      :  0 : No error                                          |
  |                     -1 : The handle parameter of the board is wrong        |
  |                     -2 : The parametered channel is wrong                  | 
  |                     -3 : The parametered polarity factor is wrong          |
  +----------------------------------------------------------------------------+
*/

int i_APCI3120_InitAnalogOutput (BYTE b_BoardHandle,
                                 BYTE b_ChannelNbr, 
				 BYTE b_Polarity)
{
  int i_ReturnValue = 0;
  xPCI3120_DEV   *pDev = xPci3120_data;   
  pDev = &xPci3120_data[b_BoardHandle];	/* set up a pointer to the relevant info-structure */
  
  if (b_BoardHandle < xPci3120_nrDev)
    { 					

      if (b_ChannelNbr <= pDev->nrAnaOut)
	{
	  /* Tests the polarity */
	  if ((b_Polarity == APCI3120_UNIPOLAR) || (b_Polarity == APCI3120_BIPOLAR))
	    {
	      pDev->outputPolarity[b_ChannelNbr] = b_Polarity;
	    }
	  else
	    {
	      /* Wrong polarity selection */
	      i_ReturnValue = -3;
	    }	  
	}
      else
	{
	  /* Wrong channel number */
	  i_ReturnValue = -2;
	}
    }
  else
    {
      /* The boardhandle is wrong */
      i_ReturnValue = -1;
    }

  return (i_ReturnValue);
}

/*
  +----------------------------------------------------------------------------+
  | Function   Name   : INT     i_APCI3120_ReadAnalogOutputBit                 |
  |                                               (BYTE     b_BoardHandle)     |
  +----------------------------------------------------------------------------+
  | Task              : Reads the Ready bit of the analog output               | 
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE   b_BoardHandle  : Handle of the APCI-3120        |
  +----------------------------------------------------------------------------+
  | Output Parameters : ---                                                    |
  |                                                                            |
  +----------------------------------------------------------------------------+
  | Return Value      :  0 : No error                                          |
  |                     -1 : The handle parameter of the board is wrong        |
  |                     -2 : The Ready bit was not set since the last start    |
  +----------------------------------------------------------------------------+
*/
int i_APCI3120_ReadAnalogOutputBit (BYTE b_BoardHandle)
{
  uint16 ui_Status = 0;
  int i_ReturnValue = 0;
  xPCI3120_DEV   *pDev = xPci3120_data;   
  pDev = &xPci3120_data[b_BoardHandle];	/* set up a pointer to the relevant info-structure */
  
  if (b_BoardHandle < xPci3120_nrDev)
    { 
      /* Reads the Ready bit of the DAC */
      ui_Status = inw(pDev->addr[1] + APCI3120_RD_STATUS);
      if ((ui_Status & 0x1) != 0x1)
	{
	  /* Returns -2 if the DAC is not ready */
	  i_ReturnValue = -2;
	}
    }
  else
    {
      /* The boardhandle is wrong */
      i_ReturnValue = -1;
    }

  return (i_ReturnValue);
}

/*
  +----------------------------------------------------------------------------+
  | Function   Name   : INT     i_APCI3120_Write1AnalogOutputValue             |
  |                                               (BYTE     b_BoardHandle,     |
  |                                                BYTE     b_ChannelNbr,      |
  |                                                UINT    ui_ValueToWrite)    |
  +----------------------------------------------------------------------------+
  | Task              : Writes the analog output value to the decided channel  | 
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE   b_BoardHandle  : Handle of the APCI-3120        |
  |                     BYTE   b_ChannelNbr   : Output channel number to write |
  |                                             ( 0 - 7)                       |
  |                     UINT  ui_ValueToWrite : Value to write on the analog   |
  |                                             output:                        |
  |                                             UNIPOLAR: 0 - 8192             |
  |                                             BIPOLAR : 0 - 16383            |
  +----------------------------------------------------------------------------+
  | Output Parameters : ---                                                    |
  |                                                                            |
  +----------------------------------------------------------------------------+
  | Return Value      :  0 : No error                                          |
  |                     -1 : The handle parameter of the board is wrong        |
  +----------------------------------------------------------------------------+
*/
int i_APCI3120_Write1AnalogOutput (BYTE b_BoardHandle,
                                   BYTE b_ChannelNumber, 
				   UINT ui_ValueToWrite)
{
  uint16 ui_TmpValueToWrite = 0;	
  int i_ReturnValue = 0;
  xPCI3120_DEV   *pDev = xPci3120_data;   
  pDev = &xPci3120_data[b_BoardHandle];	/* set up a pointer to the relevant info-structure */
  
  if (b_BoardHandle < xPci3120_nrDev)
    { 	            
      ui_TmpValueToWrite = (((uint16) (b_ChannelNumber & 0x03) << 14) & 0xC000);

      if ( (( pDev->outputPolarity[b_ChannelNumber] == APCI3120_UNIPOLAR) && (ui_ValueToWrite <= 8192)) ||
	   (( pDev->outputPolarity[b_ChannelNumber] == APCI3120_BIPOLAR)  && (ui_ValueToWrite <= 16383)) )
	{
	  if (pDev->outputPolarity[b_ChannelNumber] == APCI3120_UNIPOLAR)
	    {
	      if (ui_ValueToWrite != 0)
		{
		  ui_ValueToWrite = ui_TmpValueToWrite | (1 << 13) | (ui_ValueToWrite + 8191);
		}
	      else
		{
		  /* pArg->uiAnalogOutputValue == 0 */
		  ui_ValueToWrite = ui_TmpValueToWrite | (1 << 13) | 8192;        
		}	 
	    }	
	  else
	    {
	      /* b_Polarity == APCI3120_BIPOLAR */
	      ui_ValueToWrite = ui_TmpValueToWrite | (0 << 13) | ui_ValueToWrite;      
	    } 
	}
      /* Writes the pArg->uiAnalogOutputValue to the parametered output */
      outw (ui_ValueToWrite, pDev->addr[1] +((b_ChannelNumber / 4) * 2) + 0x08);
            
    }
  else
    {
      /* The boardhandle is wrong */
      i_ReturnValue = -1;
    }

  return (i_ReturnValue);
}


/*
  +----------------------------------------------------------------------------+
  | Function   Name   : INT     i_APCI3120_WriteMoreAnalogOutputValue          |
  |                                               (BYTE   b_BoardHandle,       |
  |                                                UINT  ui_ValueToWriteArray) |
  +----------------------------------------------------------------------------+
  | Task              : Writes the analog output values to all channels        | 
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE   b_BoardHandle  : Handle of the APCI-3120        |
  |                     UINT  ui_ValueToWriteArray : Array[8] includes the     |
  |                                                  values to write on the    |
  |                                                  analog outputs:           |
  |                                             UNIPOLAR: 0 - 8192             |
  |                                             BIPOLAR : 0 - 16383            |
  +----------------------------------------------------------------------------+
  | Output Parameters : ---                                                    |
  |                                                                            |
  +----------------------------------------------------------------------------+
  | Return Value      :  0 : No error                                          |
  |                     -1 : The handle parameter of the board is wrong        |
  +----------------------------------------------------------------------------+
*/

int i_APCI3120_WriteMoreAnalogOutput (BYTE b_BoardHandle,
                                      UINT *ui_ValueToWriteArray)
{    
  BYTE  b_ChannelNbr         = 0;
  UINT ui_Status             = 0;
  UINT ui_TmpValueToWrite[8];
  UINT ui_SaveValueToWriteArray[8];
  int i_ReturnValue = 0;
  xPCI3120_DEV   *pDev = xPci3120_data;   
  pDev = &xPci3120_data[b_BoardHandle];	/* set up a pointer to the relevant info-structure */
  
  if (b_BoardHandle < xPci3120_nrDev)
    { 	
      for (b_ChannelNbr = 0; b_ChannelNbr < pDev->nrAnaOut; b_ChannelNbr ++)
	{
	  if ( (( pDev->outputPolarity[b_ChannelNbr] == APCI3120_UNIPOLAR) 
		&& (ui_ValueToWriteArray [b_ChannelNbr] <= 8192)) ||
	       (( pDev->outputPolarity[b_ChannelNbr] == APCI3120_BIPOLAR)  
		&& (ui_ValueToWriteArray[b_ChannelNbr] <= 16383)) )
	    {  
	      ui_TmpValueToWrite[b_ChannelNbr] = (((UINT) (b_ChannelNbr & 0x03)
						   << 14) & 0xC000);

	      if (pDev->outputPolarity[b_ChannelNbr] == APCI3120_UNIPOLAR)
		{
		  if (ui_ValueToWriteArray[b_ChannelNbr] != 0)
		    {
		      ui_SaveValueToWriteArray[b_ChannelNbr] = ui_ValueToWriteArray[b_ChannelNbr] | (1 << 13) | (ui_ValueToWriteArray[b_ChannelNbr] + 8191);
		    }
		}
	      else
		{
  
                  ui_SaveValueToWriteArray[b_ChannelNbr] = 
		    ui_TmpValueToWrite[b_ChannelNbr] | 
		    (1 << 13) | 8192;        
		}	 
            }	
	  else
            { 
	      /**********************************/
	      /* b_Polarity == APCI3120_BIPOLAR */
	      /**********************************/      
	      ui_SaveValueToWriteArray[b_ChannelNbr] = 
		ui_TmpValueToWrite[b_ChannelNbr] | 
		(0 << 13) | 
		ui_ValueToWriteArray[b_ChannelNbr];      
            } 
	}
      
      for (b_ChannelNbr = 0; b_ChannelNbr < pDev->nrAnaOut; b_ChannelNbr ++)
	{
	  do
	    {
	 

	      ui_Status = inw(pDev->addr[1] + APCI3120_RD_STATUS);
	    }
	  while ((ui_Status & 0x1) != 0x1);
   
	  outw (ui_SaveValueToWriteArray[b_ChannelNbr], pDev->addr[1] +((b_ChannelNbr / 4) * 2) + 0x08);
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
  | Function   Name   : INT     i_APCI3120_ReadAnalogInputValue                |
  |                                               (BYTE     b_BoardHandle,     |
  |                                                PUINT  pui_ReadValue)       |
  +----------------------------------------------------------------------------+
  | Task              : Reads the value of the analog input channel            | 
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE   b_BoardHandle  : Handle of the APCI-3120        |
  +----------------------------------------------------------------------------+
  | Output Parameters : PUINT pui_ReadValue   : Value of the analog input      |
  |                                                                            |
  +----------------------------------------------------------------------------+
  | Return Value      :  0 : No error                                          |
  |                     -1 : The handle parameter of the board is wrong        |
  +----------------------------------------------------------------------------+
*/

INT i_APCI3120_ReadAnalogInputValue (BYTE b_BoardHandle,
                                     PUINT pui_ReadValue)
{
  int i_ReturnValue = 0;
  xPCI3120_DEV   *pDev = xPci3120_data;   
  pDev = &xPci3120_data[b_BoardHandle];	/* set up a pointer to the relevant info-structure */
  
  if (b_BoardHandle < xPci3120_nrDev)
    { 	
      /* Reads the analog input value */
      *pui_ReadValue = inw (pDev->addr[1]);
    }	
  else
    {
      /* The boardhandle is wrong */
      i_ReturnValue = -1;
    }

  return (i_ReturnValue);
}

/*
  +----------------------------------------------------------------------------+
  | Function name     : _VOID_ v_InterruptRoutine ( BYTE_ b_BoardHandle,       |
  |                                                 BYTE_ b_InterruptMask,     |
  |                                                 PUINT_ pui_ValueArray )    |
  +----------------------------------------------------------------------------+
  | Task              : This function is an interrupt routine .                |
  +----------------------------------------------------------------------------+
*/

static void v_InterruptRoutine (uint8 b_BoardHandle, uint8 b_InterruptMask, uint16* pui_ValueArray)
{
  unsigned int ui_Cpt;

  printk("irq from apci3120: mask %d\n",b_InterruptMask);
  switch(b_InterruptMask)
    {
    case 1:
      /* EOC interrupt */
      APCI3120_InterruptValue.ui_SaveArray[0] = pui_ValueArray[1];
      break;

    case 2:
      /* EOS interrupt Acquisition */
      for (ui_Cpt=0;ui_Cpt<pui_ValueArray [0];ui_Cpt++)
	APCI3120_InterruptValue.ui_SaveArray [ui_Cpt] = pui_ValueArray [1+ui_Cpt];
      break;

    case 4:
      /* EOS interrupt Read More*/
      for (ui_Cpt=0;ui_Cpt<pui_ValueArray [0];ui_Cpt++)
	APCI3120_InterruptValue.ui_SaveArray [ui_Cpt] = pui_ValueArray [1+ui_Cpt];
      break;

    case 8:
      /* DMA completed */
      for (ui_Cpt=0;ui_Cpt<16;ui_Cpt++)
	APCI3120_InterruptValue.ui_SaveArray [ui_Cpt] = pui_ValueArray [ui_Cpt];
      break;
    case 16:
      /* Timer 2 has run down */
      APCI3120_InterruptValue.ui_TimerIntCpt = APCI3120_InterruptValue.ui_TimerIntCpt + 1;
      break;
    default :
      break;
    }
  APCI3120_InterruptValue.ReceiveInterrupt = 1;
}


/* Rückgabe 1 wenn Interrupt von ACPI3120 */

int APCI3120_Handle_Interrupt(uint8 b_BoardHandle,uint8* rb_InterruptMask)
{
         		               
//static void APCI3120_Interrupt(int irq, void *dev_id, struct pt_regs *regs) HM
//{
  xPCI3120_DEV   *pDev = NULL;

  uint8   b_InterruptMask;
  uint8   b_Cpt;
  uint8   b_DummyRead;
  uint16  ui_BoardAddress;
  uint16  ui_Array[17];       /* holds the values read */
  uint16  ui_StatusValue;
  uint16  ui_Status;
  uint32 dmaBufAddr;			/* temporary holder for the active DMA buffer's address */
  uint16 *pData = NULL;   
  int returnvalue = 0; //HM
  pDev = &xPci3120_data[b_BoardHandle];	/* set up a pointer to the relevant info-structure */ //HM


//  pDev = dev_id;  HM 2004.09.19
     
  /* Test if a interrupt occur from a APCI-3120 */
  if (pDev->created)
    {
      /* Reads the board address */
      b_InterruptMask   = 0;
      ui_BoardAddress = pDev->addr[1];

      ui_StatusValue = inw(ui_BoardAddress + APCI3120_RD_STATUS);	/* get the interrupt status of the board */

      ui_StatusValue = (ui_StatusValue >> 12) & 0xF;					/* mask the possible interrupt reasons */

      /****************************************************************************************************/
      /* Test if timer interrupt */
      if (ui_StatusValue & 0x1 )
	{
	  b_InterruptMask = b_InterruptMask | 0x10;
	  if (pDev->t2Interrupt == APCI3120_ENABLE)
	    {
	      if (pDev->timerMode == APCI3120_WATCHDOG)
		{
		  /* Watchdog 2 interrupt */
		  b_InterruptMask = b_InterruptMask | 0x20;
		}
	      else /* if (ps_APCI3120Variable ... b_TimerWatchdog == APCI3120_WATCHDOG)*/
		{
		  if (pDev->timerMode == APCI3120_TIMER)
		    {
		      /* Timer 2 interrupt */
		      b_InterruptMask = b_InterruptMask | 0x10;
		    }
		  else /* if (ps_APCI3120Variable ... b_TimerWatchdog == APCI3120_TIMER) */
		    {
		      if (pDev->dma.acqEnable == APCI3120_ENABLE) 
			{
			  /* Clears gate 0, 1, 2 */
			  pDev->nWrAddr = pDev->nWrAddr & APCI3120_DISABLE_ALL_TIMER;
			  outw (pDev->nWrAddr, ui_BoardAddress + APCI3120_WR_ADDRESS);

			  ui_Array[0] = pDev->dma.nrAcq;

			  for (b_Cpt = 1; b_Cpt <= pDev->dma.nrAcq; b_Cpt ++)
			    {
			      ui_Array[b_Cpt] = inw(ui_BoardAddress);
			    }

			  b_InterruptMask = b_InterruptMask | 2;
			  pDev->intMode = 0;
			} /* if (ps_APCI3120Variable ->...b_InputAcquisitionEnable == APCI3120_ENABLE)*/
		    } /* else if (ps_APCI3120Variable ... b_TimerWatchdog == APCI3120_TIMER)*/
		} /* else if (ps_APCI3120Variable ... b_TimerWatchdog == APCI3120_WATCHDOG)*/
	    } /* if (ps_APCI3120Variable ... b_Timer2Interrupt == APCI3120_ENABLE)*/
	  else
	    {
	      /* Disable the timer interrupt */
	      pDev->modeSelect = pDev->modeSelect & APCI3120_DISABLE_TIMER_INT;
	      outb(pDev->modeSelect, ui_BoardAddress + APCI3120_WRITE_MODE_SELECT);
	    }
	 
	  /* CLEAR FC_TIMER INTERRUPT */
	  b_DummyRead = inb(ui_BoardAddress + APCI3120_TIMER_STATUS_REGISTER);
	} /* if (ui_StatusValue & 0x1 )*/

      /****************************************************************************************************/
      /* Test if EOC interrupt */
      if ( ((ui_StatusValue & 0x8) == 0) && (pDev->intMode == APCI3120_EOC_INT_MODE)) 
	{
	  ui_Array[1] = inw(ui_BoardAddress);

	  b_InterruptMask = b_InterruptMask | 1;
	  ui_Array[0] = pDev->lastChannel;

	  pDev->intMode = 0;
	} /* if (((ui_StatusValue & 0x8) == 0) && (ps_APCI3120Variable ... b_InterruptMode == APCI3120_EOC_INT_MODE))*/


      /****************************************************************************************************/
      /* Test if EOS interrupt */
      if ((ui_StatusValue & 0x2) && (pDev->intMode == APCI3120_EOS_INT_MODE))
	{
        
	  ui_Array[0] = pDev->dma.nrAcq;
	  ui_Status = inw(ui_BoardAddress + APCI3120_RD_STATUS);
	 
	  /* Test if the fifo is not empty */
	  if ((ui_Status & 0x02) != 0x02)
	    {
	      for (b_Cpt = 1; b_Cpt <= pDev->dma.nrAcq; b_Cpt ++)
		{
		  ui_Array[b_Cpt] = inw(ui_BoardAddress);
		}
	    }/* if (ui_StatusValue & 0x02 == 0x0)*/

	  if (pDev->dma.acqEnable == APCI3120_ENABLE)
	    {
	      if ((ui_Status & 0x02) != 0x02)
		b_InterruptMask = b_InterruptMask | 2;
	    }
	  else /* if (ps_APCI3120Variable ... b_InputAcquisitionEnable == APCI3120_ENABLE)*/
	    {
	      b_InterruptMask = b_InterruptMask | 4;
	      pDev->intMode = 0;
	    } /* else if (ps_APCI3120Variable ... b_InputAcquisitionEnable == APCI3120_ENABLE)*/

	  /* Test if interrupt enabled */
	  if (!b_InterruptMask)
	    {
	      /* Disable the EOS interrupt */
	      pDev->modeSelect = pDev->modeSelect & APCI3120_DISABLE_EOS_INT;
	      outb(pDev->modeSelect, ui_BoardAddress + APCI3120_WRITE_MODE_SELECT);
	    }
	} /* if ((ui_StatusValue & 0x2) && (ps_APCI3120Variable ... b_InterruptMode == APCI3120_EOS_INT_MODE))*/


      /****************************************************************************************************/
      /**** Test if DMA interrupt ****/
      if ((ui_StatusValue & 0x4) && (pDev->intMode == APCI3120_DMA_INT_MODE))
	{
	  /* Clear Timer Write TC INT */
	  outl(APCI3120_CLEAR_WRITE_TC_INT, pDev->addr[0] + APCI3120_AMCC_OP_REG_INTCSR);
	      
	  /* Clear Timer int */
	  b_DummyRead = inb(ui_BoardAddress + APCI3120_TIMER_STATUS_REGISTER);

	  if (pDev->dma.acqEnable == APCI3120_ENABLE)
	    {
	      b_InterruptMask = b_InterruptMask | 0x8;

	      if (pDev->dma.acqCycle == APCI3120_SINGLE)
		{
		  /* Stops the Timer */
		  outw(pDev->nWrAddr & APCI3120_DISABLE_TIMER0 & APCI3120_DISABLE_TIMER1, ui_BoardAddress + APCI3120_WR_ADDRESS);
	            
		  pDev->nWrAddr  = pDev->nWrAddr & APCI3120_DISABLE_TIMER0 & APCI3120_DISABLE_TIMER1;
		  pData = (uint16 *) pDev->dma.dmaBuffer;
		  pDev->dma.bufIndex = 0;
	       
		  /* Resets the FIFO */
		  b_DummyRead = inb(pDev->addr[1] + APCI3120_RESET_FIFO);
		}
	      else /* if (ps_APCI3120Variable ... AcquisitionCycle == APCI3120_SINGLE)*/
		{
		  /* Continuous DMA mode */
		  /* Initializes the DMA    */
		  /* ENABLE BUS MASTER */
		  outw(APCI3120_ADD_ON_AGCSTS_LOW, pDev->addr[2]);
		  outw(APCI3120_ENABLE_TRANSFER_ADD_ON_LOW, pDev->addr[2] + 2);

		  outw(APCI3120_ADD_ON_AGCSTS_HIGH, pDev->addr[2]);
		  outw(0x1000, pDev->addr[2] + 2);

		  /* set active DMA buffer */
		  if (pDev->dma.bufIndex)
		    dmaBufAddr = (uint32)pDev->dma.oldDmaBufferHw;
		  else
		    dmaBufAddr = (uint32)pDev->dma.dmaBufferHw;
                     
               
		  /* DMA Start Adress Low */
		  outw(APCI3120_ADD_ON_MWAR_LOW, pDev->addr[2]);
		  outw((dmaBufAddr & 0x0000FFFFUL),  pDev->addr[2] + 2);
	       
		  /* DMA Start Adress High */
		  outw(APCI3120_ADD_ON_MWAR_HIGH, pDev->addr[2]);
		  outw(((dmaBufAddr & 0xFFFF0000UL)>>16),  pDev->addr[2] + 2);
                     
		  pDev->dma.bufIndex = (pDev->dma.bufIndex ^ 1);	/* toggle index 0 <-> 1 */
	       
		  /* Nbr of acquisition LOW */
		  outw(APCI3120_ADD_ON_MWTC_LOW, pDev->addr[2]);
		  outw((uint16) (pDev->dma.nrAcq * 2) & 0xFFFF, pDev->addr[2] + 2);

		  /* Nbr of acquisition HIGH */
		  outw(APCI3120_ADD_ON_MWTC_HIGH, pDev->addr[2]);
		  outw((uint16)((pDev->dma.nrAcq * 2) / 65536UL), pDev->addr[2] + 2); /*pK: interessante Rechenoperation...*/
	       
		  /* A2P FIFO CONFIGURATE, END OF DMA INTERRUPT INIT */
		  outl((APCI3120_FIFO_ADVANCE_ON_BYTE_2 | APCI3120_ENABLE_WRITE_TC_INT), pDev->addr[0] + APCI3120_AMCC_OP_REG_INTCSR);
	       
		  /* ENABLE A2P FIFO WRITE AND ENABLE AMWEN */
		  outw(3, pDev->addr[2] + 4);
		}  /* else if (ps_APCI3120Variable ... AcquisitionCycle == APCI3120_SINGLE)*/
	    }     /* if (ps_APCI3120Variable ...b_InputAcquisitionEnable == APCI3120_ENABLE)*/
	  else
	    {
	      /* Stops the Timer */
	      outw(pDev->nWrAddr & APCI3120_DISABLE_TIMER0 & APCI3120_DISABLE_TIMER1, ui_BoardAddress + APCI3120_WR_ADDRESS);
	      pDev->nWrAddr  = pDev->nWrAddr & APCI3120_DISABLE_TIMER0 & APCI3120_DISABLE_TIMER1;
	    }
	}     /* if ((ui_StatusValue & 0x4) && (ps_APCI3120Variable ... b_InterruptMode == APCI3120_DMA_INT_MODE))*/


      /**** Test if interrupt occur ****/
      if (b_InterruptMask)
	{
	    returnvalue = 1; //Hm ok Interrupt von APCI3120
	  if (pDev->intOccur == 0)
	    {
	      pDev->intOccur = 1;
	      pDev->oldIntMask = b_InterruptMask;

	      if (b_InterruptMask & 0x7)
		{
		  memcpy(pDev->oldArray, ui_Array, sizeof(ui_Array));
		}

	      if (pDev->dma.bufIndex == 0)
		{
		  pData = (uint16 *) pDev->dma.dmaBuffer;
		}
	      else
		{
		  pData = (uint16 *) pDev->dma.oldDmaBuffer;
		}
	    } /* if (ps_APCI3120Variable ... b_InterruptOccur == 0)*/
    
	  if (pDev->oldIntMask & 0x8)  //mit DMA ? Hm
	    {
		if (pDev->usrHdl) {
		    *rb_InterruptMask = pDev->oldIntMask; //Hm
/*		    *pui_ValueArray = pData; //Hm
		    //Hm -------------------------
		    if (pDev->dma.bufIndex == 0)
			pui_ValueArray = &((uint16 *) pDev->dma.dmaBuffer);
		    else
			pui_ValueArray = &((uint16 *) pDev->dma.oldDmaBuffer);
		    //Hm -------------------------
		    */

		    // HMpDev->usrHdl(pDev->BoardHandle, pDev->oldIntMask, pData);

		}
	    }
          else   //ohne DMA ? Hm
	    {
		if (pDev->usrHdl) {
		    *rb_InterruptMask = pDev->oldIntMask; //Hm
//		    pui_ValueArray = &pDev->oldArray;  //Hm
		    // pDev->usrHdl(pDev->BoardHandle, pDev->oldIntMask, pDev->oldArray); 
		}
	    }

	  pDev->intOccur = 0;

	  b_InterruptMask = 0;
	} /* if (b_InterruptMask)*/
    }
  return returnvalue;
}



/*
  +----------------------------------------------------------------------------+
  | Function's Name   : _INT_ i_APCI3120_StopAnalogInputAcquisition            |
  |                               (BYTE_    b_BoardHandle)                     |
  +----------------------------------------------------------------------------+
  | Task              : Stops the cyclic conversion . It has been started      |
  |                     previously with function                               |
  |                     "i_APCI3120_StartAnalogInputAcquisition".              |
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE      b_BoardHandle     : Handle of board APCI3120 |
  +----------------------------------------------------------------------------+
  | Return Value      :  0: No error                                           |
  |                     -1: The handle parameter of the board is wrong         |
  |                     -2: The cyclic conversion has not been started.        |
  |                         "i_APCI3120_StartAnalogInputAcquisition"           |
  +----------------------------------------------------------------------------+
*/

INT   i_APCI3120_StopAnalogInputAcquisition     (BYTE    b_BoardHandle)
{
  int i_ReturnValue = 0;	  	
  uint8   	b_DummyRead;
  xPCI3120_DEV   *pDev = xPci3120_data;
   
  pDev = &xPci3120_data[b_BoardHandle];	/* set up a pointer to the relevant info-structure */
   
  if (b_BoardHandle < xPci3120_nrDev)
    { 
      /* Tests if acquisition is enable */
      if (pDev->dma.acqEnable == APCI3120_ENABLE)
	{		
	  /* Init FC Timer IT */
	  b_DummyRead = inb(pDev->addr[1] + APCI3120_TIMER_STATUS_REGISTER);	  			  

	  /* Test if DELAY mode */
	  if ((pDev->dma.acqMode == APCI3120_DELAY_MODUS) || (pDev->dma.acqMode == APCI3120_DELAY_1_MODUS))
	    {
	      pDev->nWrAddr = pDev->nWrAddr & APCI3120_DISABLE_TIMER0 & APCI3120_DISABLE_TIMER1;
	    }
	  else
	    {
	      pDev->nWrAddr = pDev->nWrAddr & APCI3120_DISABLE_TIMER0;
	    }/* else if (ps_APCI3120Variable ...b_AcquisitionMode == APCI3120_DELAY_MODUS)*/

	  /* Extern Trigger */
	  if (pDev->dma.extTrigger == APCI3120_ENABLE)
	    {
	      pDev->nWrAddr = (pDev->nWrAddr & APCI3120_DISABLE_EXT_TRIGGER);
	    }

	  /* Stops timer */
	  outw(pDev->nWrAddr, pDev->addr[1] + APCI3120_WR_ADDRESS);

	  /* Writes the interrupt select register */
	  outb(pDev->modeSelect, pDev->addr[1] + APCI3120_WRITE_MODE_SELECT);
	  b_DummyRead= inb(pDev->addr[1] + APCI3120_TIMER_STATUS_REGISTER);
	   
	  /* Test if DMA is used */
	  if (pDev->dma.dmaSelect == APCI3120_ENABLE)
	    {
	      /* Clear Timer Write TC int16 */
	      outl(APCI3120_CLEAR_WRITE_TC_INT, pDev->addr[0] + APCI3120_AMCC_OP_REG_INTCSR);

	      /* DISABLE END OF DMA INTERRUPT */
	      outl(0, pDev->addr[0] + APCI3120_AMCC_OP_REG_INTCSR);

	      /* Init FC Timer IT */
	      b_DummyRead = inb(pDev->addr[1] + APCI3120_TIMER_STATUS_REGISTER);

	      /* DISABLE A2P WRITE AND AMWEN */
	      outw(APCI3120_DISABLE_AMWEN_AND_A2P_FIFO_WRITE, pDev->addr[2] + 4);

	      /* DISABLE BUS MASTER */
	      outw(APCI3120_ADD_ON_AGCSTS_LOW, pDev->addr[2]);
	      outw(APCI3120_DISABLE_BUS_MASTER_ADD_ON, pDev->addr[2] + 2);
	      outw(APCI3120_ADD_ON_AGCSTS_HIGH, pDev->addr[2]);
	      outw(APCI3120_DISABLE_BUS_MASTER_ADD_ON, pDev->addr[2] + 2);

	      /* DISABLE BUS MASTER PCI */
	      outl(APCI3120_DISABLE_BUS_MASTER_PCI, pDev->addr[0] + APCI3120_AMCC_OP_MCSR);
	    }

	  /*pK2	(Passiert bei cmd_APCI3120_ClearAnalogInputAcquisition)		pDev->dma.acqEnable = APCI3120_DISABLE;*/
	  pDev->dma.dmaSelect = APCI3120_DISABLE;
	}
      else
	{
	  /* The cyclic conversion has not been started */
	  i_ReturnValue = -2;
	}
	
    }
  else
    {
      /* The boardhandle is wrong */
      i_ReturnValue = -1;
    }
		
  return i_ReturnValue;
	
}

/*
  +----------------------------------------------------------------------------+
  | Function   Name   : INT     i_APCI3120_Read4DigitalInput                   |
  |                                               (BYTE     b_BoardHandle,     |
  |                                                PBYTE   pb_PortValue)       |
  +----------------------------------------------------------------------------+
  | Task              : Reads the status of all digital inputs                 | 
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE   b_BoardHandle  : Handle of the APCI-3120        |
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
int i_APCI3120_Read4DigitalInput (BYTE b_BoardHandle,
                                  PBYTE pb_PortValue)
{
  int i_ReturnValue = 0;	  	
  uint16 ui_TmpRegisterValue = 0;
  xPCI3120_DEV   *pDev = xPci3120_data;

  pDev = &xPci3120_data[b_BoardHandle];	/* set up a pointer to the relevant info-structure */
   
  if (b_BoardHandle < xPci3120_nrDev)
    {

      /* Reads the register from address Base[1] + 0x02 */
      ui_TmpRegisterValue = inw (pDev->addr[1] + APCI3120_DigitalInput);
				
      /* Selects the input port */
      ui_TmpRegisterValue = (ui_TmpRegisterValue >> (8)) & 0x0F;
					  
      /* Returns the port status */
      *pb_PortValue = (uint8) ui_TmpRegisterValue;        
    }
  else
    {
      i_ReturnValue = -1;
    }	   
   
  return (i_ReturnValue);
}


/*
  +----------------------------------------------------------------------------+
  | Function   Name   : INT     i_APCI3120_SetOutputMemoryOn                   |
  |                                               (BYTE     b_BoardHandle)     |
  +----------------------------------------------------------------------------+
  | Task              : Activates the output memory of the digital outputs     | 
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE   b_BoardHandle  : Handle of the APCI-3120        |
  |                                                                            |
  +----------------------------------------------------------------------------+
  | Output Parameters : ----                                                   |
  +----------------------------------------------------------------------------+
  | Return Value      :  0 : No error                                          |
  |                     -1 : The handle parameter of the board is wrong        |
  +----------------------------------------------------------------------------+
*/
int i_APCI3120_SetOutputMemoryOn (BYTE b_BoardHandle)
{
  int i_ReturnValue = 0;	  	
  xPCI3120_DEV   *pDev = xPci3120_data;

  pDev = &xPci3120_data[b_BoardHandle];	/* set up a pointer to the relevant info-structure */
   
  if (b_BoardHandle < xPci3120_nrDev)
    {
      /* Sets the Output Memory On */
      pDev->digOutMem = APCI3120_ENABLE;
    
    }
  else
    {
      i_ReturnValue = -1;
    }	   
   
  return (i_ReturnValue);
}

/*
  +----------------------------------------------------------------------------+
  | Function   Name   : INT     i_APCI3120_SetOutputMemoryOff                  |
  |                                               (BYTE     b_BoardHandle)     |
  +----------------------------------------------------------------------------+
  | Task              : Deactivates the output memory of the digital outputs   | 
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE   b_BoardHandle  : Handle of the APCI-3120        |
  |                                                                            |
  +----------------------------------------------------------------------------+
  | Output Parameters : ----                                                   |
  +----------------------------------------------------------------------------+
  | Return Value      :  0 : No error                                          |
  |                     -1 : The handle parameter of the board is wrong        |
  +----------------------------------------------------------------------------+
*/
int i_APCI3120_SetOutputMemoryOff (BYTE b_BoardHandle)
{
  int i_ReturnValue = 0;	  	
  xPCI3120_DEV   *pDev = xPci3120_data;

  pDev = &xPci3120_data[b_BoardHandle];	/* set up a pointer to the relevant info-structure */
   
  if (b_BoardHandle < xPci3120_nrDev)
    {
      /* Sets the Output Memory On */
      pDev->digOutMem = APCI3120_DISABLE; 
    }
  else
    {
      i_ReturnValue = -1;
    }	   
   
  return (i_ReturnValue);
}


/*
  +----------------------------------------------------------------------------+
  | Function   Name   : INT     i_APCI3120_Set1DigitalOutputOn                 |
  |                                               (BYTE     b_BoardHandle,     |
  |                                                BYTE     b_ChannelNumber)   |
  +----------------------------------------------------------------------------+
  | Task              : Sets one channel of the digital outputs                | 
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE   b_BoardHandle  : Handle of the APCI-3120        |
  |                     BYTE   b_ChannelNumber: Number of the channel to set   |
  |                                                                            |
  +----------------------------------------------------------------------------+
  | Output Parameters : ----                                                   |
  +----------------------------------------------------------------------------+
  | Return Value      :  0 : No error                                          |
  |                     -1 : The handle parameter of the board is wrong        |
  |                     -2 : The parametered channel number is wrong           |
  +----------------------------------------------------------------------------+
*/
int i_APCI3120_Set1DigitalOutputOn (BYTE b_BoardHandle,
                                    BYTE b_ChannelNumber)
{
  int i_ReturnValue = 0;	  	
  xPCI3120_DEV   *pDev = xPci3120_data;

  pDev = &xPci3120_data[b_BoardHandle];	/* set up a pointer to the relevant info-structure */
   
  if (b_BoardHandle < xPci3120_nrDev)
    {
      if (b_ChannelNumber <= 3)
	{
	  /* If Outp. Mem. is used, all outputs are saved and kept*/
	  if (pDev->digOutMem == APCI3120_ENABLE)
	    {
	      pDev->digOutReg =   (pDev->digOutReg | (1 << b_ChannelNumber));
	    }
	  /* If Outp. Mem. is not used, all outputs are overwritten */
	  else
	    {
	      pDev->digOutReg  = 1 << (b_ChannelNumber);
	    }
	  /* Writes the digital output register to the board */
	  pDev->crt0 = (pDev->digOutReg << 4);
	  outb (pDev->digOutReg  << 4, pDev->addr[1] + APCI3120_DigitalOutput);
	}
      else 
	{ 
	  i_ReturnValue = -2;
	}         
    }
  else
    {
      i_ReturnValue = -1;
    }	   
   
  return (i_ReturnValue);
}


/*
  +----------------------------------------------------------------------------+
  | Function   Name   : INT     i_APCI3120_Set1DigitalOutputOff                |
  |                                               (BYTE     b_BoardHandle,     |
  |                                                BYTE     b_ChannelNumber)   |
  +----------------------------------------------------------------------------+
  | Task              : Resets one channel of the digital outputs              | 
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE   b_BoardHandle  : Handle of the APCI-3120        |
  |                     BYTE   b_ChannelNumber: Number of the channel to reset |
  |                                                                            |
  +----------------------------------------------------------------------------+
  | Output Parameters : ----                                                   |
  +----------------------------------------------------------------------------+
  | Return Value      :  0 : No error                                          |
  |                     -1 : The handle parameter of the board is wrong        |
  |                     -2 : The parametered channel number is wrong           |
  |                     -3 : The Output memory is not enabled                  |
  +----------------------------------------------------------------------------+
*/
int i_APCI3120_Set1DigitalOutputOff (BYTE b_BoardHandle,
                                     BYTE b_ChannelNumber)
{
  int i_ReturnValue = 0;	  	
  xPCI3120_DEV   *pDev = xPci3120_data;

  pDev = &xPci3120_data[b_BoardHandle];	/* set up a pointer to the relevant info-structure */
   
  if (b_BoardHandle < xPci3120_nrDev)
    {
      if (b_ChannelNumber <= 3)
	{
	  /* If Outp. Mem. is used, the wished output will be reseted */
	  if ((pDev->digOutMem) == APCI3120_ENABLE)
	    {
	      pDev->digOutReg = (pDev->digOutReg & (0xF - (1 << b_ChannelNumber)));

	      pDev->crt0 = (pDev->digOutReg << 4);
	      outb (pDev->digOutReg << 4, pDev->addr[1] + APCI3120_DigitalOutput);
	    }
	  else
	    {
	      /* If Outp. Mem. is not used, the function returns with an error */
	      i_ReturnValue = -3;
	    }
	}
      else
	{
	  /* The parametered channel number is wrong */
	  i_ReturnValue = -2;
	}        
    }
  else
    {
      i_ReturnValue = -1;
    }	   
   
  return (i_ReturnValue);
}



/*
  +----------------------------------------------------------------------------+
  | Function   Name   : INT     i_APCI3120_Set4DigitalOutputOn                 |
  |                                               (BYTE     b_BoardHandle,     |
  |                                                BYTE     b_PortValue)       |
  +----------------------------------------------------------------------------+
  | Task              : Sets all channels of the digital outputs               |  
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE   b_BoardHandle  : Handle of the APCI-3120        |
  |                     BYTE   b_PortValue    : Value which is written to the  |
  |                                             port:                          |
  |                                                 "1" : Output 1 is set      |
  |                                                 "3" : Output 1 and 2 is set|
  |                                                 "15": All Outputs are set  |
  +----------------------------------------------------------------------------+
  | Output Parameters : ----                                                   |
  +----------------------------------------------------------------------------+
  | Return Value      :  0 : No error                                          |
  |                     -1 : The handle parameter of the board is wrong        |
  |                     -2 : The parametered port value is wrong               |
  +----------------------------------------------------------------------------+
*/
int i_APCI3120_Set4DigitalOutputOn (BYTE b_BoardHandle,
                                    BYTE b_PortValue)
{
  int i_ReturnValue = 0;	  	
  xPCI3120_DEV   *pDev = xPci3120_data;

  pDev = &xPci3120_data[b_BoardHandle];	/* set up a pointer to the relevant info-structure */
   
  if (b_BoardHandle < xPci3120_nrDev)
    {
      if (b_PortValue <= 0xF)
	{
	  /* If Outp. Mem. is used, all outputs are saved and kept*/
	  if ((pDev->digOutMem) == APCI3120_ENABLE)
	    {
	      pDev->digOutReg = pDev->digOutReg | b_PortValue;
	    }
	  /* If Outp. Mem. is not used, all outputs are overwritten */
	  else
	    {
	      pDev->digOutReg = b_PortValue;
	    }
	  /* Writes the digital output register to the board */
	  pDev->crt0 = ( pDev->digOutReg << 4);
	  outb ((pDev->digOutReg) << 4, pDev->addr[1] + APCI3120_DigitalOutput);
	}
      else
	{
	  /* The parametered port value is wrong */
	  i_ReturnValue = -2;
	}	         
    }
  else
    {
      i_ReturnValue = -1;
    }	   
   
  return (i_ReturnValue);
}

/*
  +----------------------------------------------------------------------------+
  | Function   Name   : INT     i_APCI3120_Set4DigitalOutputOff                |
  |                                               (BYTE     b_BoardHandle,     |
  |                                                BYTE     b_PortValue)       |
  +----------------------------------------------------------------------------+
  | Task              : Resets all channels of the digital outputs             |  
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE   b_BoardHandle  : Handle of the APCI-3120        |
  |                     BYTE   b_PortValue    : Value which is reset on the    |
  |                                             port:                          |
  |                                              "1" : Output 1 is reset       |
  |                                              "3" : Output 1 and 2 are reset|
  |                                              "15": All Outputs are reset   |
  +----------------------------------------------------------------------------+
  | Output Parameters : ----                                                   |
  +----------------------------------------------------------------------------+
  | Return Value      :  0 : No error                                          |
  |                     -1 : The handle parameter of the board is wrong        |
  |                     -2 : The parametered port value is wrong               |
  |                     -3 : The output memory is not enabled                  |
  +----------------------------------------------------------------------------+
*/
int i_APCI3120_Set4DigitalOutputOff (BYTE b_BoardHandle,
                                     BYTE b_PortValue)
{
  int i_ReturnValue = 0;	  	
  xPCI3120_DEV   *pDev = xPci3120_data;

  pDev = &xPci3120_data[b_BoardHandle];	/* set up a pointer to the relevant info-structure */
   
  if (b_BoardHandle < xPci3120_nrDev)
    {
      if (b_PortValue <= 0xF)
	{
	  /* If Outp. Mem. is used, all outputs are saved and kept*/
	  if ((pDev->digOutMem) == APCI3120_ENABLE)
	    {
	      pDev->digOutReg = (pDev->digOutReg & (0xF - b_PortValue));
	      /* Writes the digital output register to the board */
	      pDev->crt0 = (pDev->digOutReg << 4);
	      outb((pDev->digOutReg ) << 4, pDev->addr[1] + APCI3120_DigitalOutput);
	    }
	  else
	    {
	      /* If Outp. Mem. is not used, the function is not available */
	      i_ReturnValue = -3;
	    } 
	}
      else
	{
	  /* the parametered port value is wrong */
	  i_ReturnValue = -2;
	}        
    }
  else
    {
      i_ReturnValue = -1;
    }	   
   
  return (i_ReturnValue);
}


/*
  +----------------------------------------------------------------------------+
  | Function's Name   : INT   i_APCI3120_InitTimerWatchdog                     |
  |                               (BYTE   b_BoardHandle,                       |
  |                                BYTE   b_TimerMode,                         |
  |                                LONG   l_DelayValue,                        |
  |                                BYTE   b_InterruptFlag)                     |
  +----------------------------------------------------------------------------+
  | Task              : Initializes the timer as an edge generator or watchdog.|
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE      b_BoardHandle   : Handle of board APCI 3120  |
  |                     LONG      l_DelayValue    : Time interval of timer     |
  |                                                 from 70 us to 4587400 us   |
  |                     BYTE      b_InterruptFlag : APCI3120_ENABLE :          |
  |                                                 Timer : an interrupt is    |
  |                                                 generated at the end of    |
  |                                                 each time interval .       |
  |                                                 Watchdog : an interrupt is |
  |                                                 generated when the watchdog|
  |                                                 has run down .             |
  |                                                 apci3120_DISABLE : No      |
  |                                                 interrupt is generated .   |
  |                                                 interval .                 |
  +----------------------------------------------------------------------------+
  | Return Value      :  0: No error.                                          |
  |                     -1: The handle parameter of the board is wrong.        |
  |                     -2: Timer mode error                                   |
  |                     -3: The user interrupt routine is not installed        |
  |                         See function "i_APCI3120_SetBoardIntRoutine"       |
  |                     -4: The interrupt parameter is wrong                   |
  |                     -5: The time interval selected for timer is wrong      |
  +----------------------------------------------------------------------------+
*/

INT     i_APCI3120_InitTimerWatchdog     (BYTE   b_BoardHandle,
					  BYTE   b_TimerMode,
					  LONG   l_DelayTime,
					  BYTE   b_InterruptFlag)
{
  uint32 ul_TimerValue;
  uint16  ui_TimerValueLow  = 0;
  uint16  ui_TimerValueHigh = 0;
  int i_ReturnValue = 0;	  	
  xPCI3120_DEV   *pDev = xPci3120_data;

  pDev = &xPci3120_data[b_BoardHandle];	/* set up a pointer to the relevant info-structure */
   
  if (b_BoardHandle < xPci3120_nrDev)
    {

      /* Tests the mode */
      if (b_TimerMode == APCI3120_TIMER ||  b_TimerMode == APCI3120_WATCHDOG)
	{
	  /* Tests if interrupt used */
	  if (b_InterruptFlag == APCI3120_ENABLE ||  b_InterruptFlag == APCI3120_DISABLE)
	    {
	      /* Tests the interrupt */
	      if ( ( (pDev->interruptInitialized == APCI3120_ENABLE) && (b_InterruptFlag == APCI3120_ENABLE)) || b_InterruptFlag == APCI3120_DISABLE)
		{
		  /* Tests and calculate the time interval */
		  if (b_TimerMode == APCI3120_TIMER)
		    {
		      if (l_DelayTime < 100 || l_DelayTime > 838860800L)
			i_ReturnValue = -5;
		    }
		  else
		    {
		      if (l_DelayTime < 50 || l_DelayTime > 838860800L)
			i_ReturnValue = -5;
		    }

		  if (i_ReturnValue == 0)
		    {
		      /* Save Delay Value */
		      pDev->t2Delay = l_DelayTime;

		      ul_TimerValue    = (int32) (l_DelayTime / 50);
		      if (ul_TimerValue >= 2)
			ul_TimerValue = ul_TimerValue - 2;
		      ui_TimerValueLow  = (uint16) (ul_TimerValue & 0xFFFF);
		      ui_TimerValueHigh = (uint8) (ul_TimerValue >> 16);

		      /* Resets gate 2 */
		      pDev->nWrAddr = pDev->nWrAddr & APCI3120_DISABLE_TIMER2;
		      outw(pDev->nWrAddr, pDev->addr[1] + APCI3120_WR_ADDRESS);
		                
		      /* DISABLE TIMER INTERRUPT */
		      pDev->modeSelect = pDev->modeSelect & APCI3120_DISABLE_TIMER_INT & 0xEF;
		      outb(pDev->modeSelect, pDev->addr[1] + APCI3120_WRITE_MODE_SELECT);

		      switch (b_TimerMode)
			{
			case APCI3120_TIMER:
			  /* initializes timer 2 in mode 2 */
			  pDev->crt1  = (pDev->crt1 & 0x0F) | APCI3120_TIMER_2_MODE_2;
			  break;

			case APCI3120_WATCHDOG:
			  /* initialize timer 2 in mode 5 */
			  pDev->crt1  = (pDev->crt1 & 0x0F) | APCI3120_TIMER_2_MODE_5;
			  break;
                            
			default:
			    break;
                        }

		      /* Writes timer mode */
		      outb(pDev->crt1, pDev->addr[1] + APCI3120_TIMER_CRT1 );
		      pDev->crt0 = ((pDev->crt0  & 0xF0) | APCI3120_SELECT_TIMER_2_LOW_WORD);
		                
		      /* Writes the low word */
		      outb(pDev->crt0, pDev->addr[1] + APCI3120_TIMER_CRT0);
		      outw(ui_TimerValueLow, pDev->addr[1] + APCI3120_TIMER_VALUE);
		      pDev->crt0 = ((pDev->crt0 & 0xF0) | APCI3120_SELECT_TIMER_2_HIGH_WORD);
		                
		      /* Writes the high word */
		      outb(pDev->crt0, pDev->addr[1] + APCI3120_TIMER_CRT0);
		      outw(ui_TimerValueHigh, pDev->addr[1] + APCI3120_TIMER_VALUE);
		                
		      /* Saves informations */
		      pDev->t2Init = APCI3120_ENABLE;
		      pDev->t2Started = APCI3120_DISABLE;
		                
		      /* Timer interrupt */
		      pDev->t2Interrupt = (uint8) b_InterruptFlag;
		                
		      /* Timer mode */
		      pDev->timerMode = b_TimerMode;
		    }
		  else
		    {
		      /* Time interval error */
		      i_ReturnValue = -5;
		    }
		}
	      else
		{
		  /* Interrupt error */
		  i_ReturnValue = -3;
		}
	    }
	  else
	    {
	      /* Interrupt parameter error */
	      i_ReturnValue = -4;
	    }
	}
      else
	{
	  /* Mode error */
	  i_ReturnValue = -2;
	}        
    }
  else
    {
      i_ReturnValue = -1;
    }	   
   
  return (i_ReturnValue);
}


/*
  +----------------------------------------------------------------------------+
  | Function's Name   : INT       i_APCI3120_StartTimerWatchdog                  |
  |                                                  (BYTE  b_BoardHandle)     |
  +----------------------------------------------------------------------------+
  | Task              : Start timer/watchdog                                   |
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE      b_BoardHandle   : Handle of board APCI 3120  |
  +----------------------------------------------------------------------------+
  | Return Value      :  0: No error.                                          |
  |                     -1: The handle parameter of the board is wrong.        |
  |                     -2: Timer/watchdog has not been initialized.           |
  +----------------------------------------------------------------------------+
*/

INT       i_APCI3120_StartTimerWatchdog (BYTE  b_BoardHandle)

{
  uint8   b_DummyRead;
  int i_ReturnValue = 0;	  	
  xPCI3120_DEV   *pDev = xPci3120_data;

  pDev = &xPci3120_data[b_BoardHandle];	/* set up a pointer to the relevant info-structure */
   
  if (b_BoardHandle < xPci3120_nrDev)
    {


	
      /* Tests if timer initialized */
		
      if (pDev->t2Init == APCI3120_ENABLE)
	{	   
	  /* RESET FC_TIMER BIT */
	  b_DummyRead = inb(pDev->addr[1] + APCI3120_TIMER_STATUS_REGISTER);
		   
	  /* Tests if watchdog initialized */
	  if (pDev->timerMode == APCI3120_WATCHDOG)
	    {
	      pDev->modeSelect = (pDev->modeSelect &0x0B) | APCI3120_ENABLE_WATCHDOG;
	    }
	  else
	    {
	      pDev->modeSelect = (pDev->modeSelect & 0x0B);
	    }                                      
		   
	  /* ENABLE WATCHDOG or ENABLE TIMER COUNTER */
	  outb(pDev->modeSelect, pDev->addr[1] + APCI3120_WRITE_MODE_SELECT);

	  if (pDev->t2Interrupt == APCI3120_ENABLE)
	    {
	      pDev->modeSelect = pDev->modeSelect | APCI3120_ENABLE_TIMER_INT;
	    }
	  else
	    {
	      pDev->modeSelect = pDev->modeSelect & APCI3120_DISABLE_TIMER_INT;
	    }

	  /* ENABLE or DISABLE TIMER INTERRUPT */
	  outb(pDev->modeSelect, pDev->addr[1] + APCI3120_WRITE_MODE_SELECT);

	  /* Tests if timer */
	  if (pDev->timerMode == APCI3120_TIMER)
	    {
	      /* Sets gate 2 */
	      pDev->nWrAddr = pDev->nWrAddr | APCI3120_ENABLE_TIMER2;
	      outw(pDev->nWrAddr, pDev->addr[1] + APCI3120_WR_ADDRESS);
	    }

	  pDev->t2Started = APCI3120_ENABLE;
	}
      else
	{
	  /* Timer has not been initialized */
	  i_ReturnValue = -2;
	}        
    }
  else
    {
      i_ReturnValue = -1;
    }	   
   
  return (i_ReturnValue);
}



/*
  +----------------------------------------------------------------------------+
  | Function's Name   : INT     i_APCI3120_StopTimerWatchdog(BYTE  b_BoardHandle)|
  +----------------------------------------------------------------------------+
  | Task              : Stops timer/watchdog                                   |
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE      b_BoardHandle   : Handle of board PCI 3120   |
  +----------------------------------------------------------------------------+
  | Return Value      :  0: No error.                                          |
  |                     -1: The handle parameter of the board is wrong.        |
  |                     -2: Timer/watchdog has not been initialized.           |
  |                     -3: Timer/watchdog has not been started.               |
  +----------------------------------------------------------------------------+
*/

INT       i_APCI3120_StopTimerWatchdog (BYTE  b_BoardHandle)
{
  int i_ReturnValue = 0;	  	
  uint8   b_DummyRead;
  xPCI3120_DEV   *pDev = xPci3120_data;

  pDev = &xPci3120_data[b_BoardHandle];	/* set up a pointer to the relevant info-structure */
   
  if (b_BoardHandle < xPci3120_nrDev)
    {

			
      /* Tests if timer 2 is initialized */
			
      if (pDev->t2Init == APCI3120_ENABLE)
	{
	  /* Tests if timer 2 is started     */
	  if (pDev->t2Started == APCI3120_ENABLE)
	    {
	      /* Tests if watchdog is initialized */
	      if (pDev->timerMode == APCI3120_WATCHDOG)
		{
		  pDev->modeSelect = pDev->modeSelect & APCI3120_DISABLE_WATCHDOG;
		}
	      else
		{
		  pDev->modeSelect = pDev->modeSelect & APCI3120_DISABLE_TIMER_COUNTER;
		}
			      
	      if (pDev->t2Interrupt == APCI3120_ENABLE)
		{
		  pDev->modeSelect = pDev->modeSelect & APCI3120_DISABLE_TIMER_INT;
		}
		
	      outb(pDev->modeSelect, pDev->addr[1] + APCI3120_WRITE_MODE_SELECT);
			      
	      /* Resets gate 2 */
	      pDev->nWrAddr = pDev->nWrAddr & APCI3120_DISABLE_TIMER2;
	      outw(pDev->nWrAddr, pDev->addr[1] + APCI3120_WR_ADDRESS);
			      
	      /* RESET FC_TIMER BIT */
	      b_DummyRead = inb(pDev->addr[1] + APCI3120_TIMER_STATUS_REGISTER);
	      pDev->t2Started = APCI3120_DISABLE;
	    }
	  else
	    {
	      /* Timer 2 has not been started     */
	      i_ReturnValue = -3;
	    }
	}
      else
	{
	  /* Timer 2 has not been initialized */
	  i_ReturnValue = -2;
	}        
    }
  else
    {
      i_ReturnValue = -1;
    }	   
   
  return (i_ReturnValue);
}

/*
  +----------------------------------------------------------------------------+
  | Function's Name   : INT       i_APCI3120_ReadTimer                         |
  |                               (BYTE     b_BoardHandle,                     |
  |                                PLONG    pl_ReadValue)                      |
  +----------------------------------------------------------------------------+
  | Task              : Reads the current value of timer 2.                    |
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE      b_BoardHandle   : Handle of board APCI 3120  |
  |                     PLONG     pl_ReadValue    : Current timer value        |
  +----------------------------------------------------------------------------+
  | Return Value      :  0: No error.                                          |
  |                     -1: The handle parameter of the board is wrong.        |
  |                     -2: Timer has not been initialized.                    |
  |                     -3: Watchdog initialized                               |
  +----------------------------------------------------------------------------+
*/

INT       i_APCI3120_ReadTimer     (BYTE    b_BoardHandle,
                                    PLONG  pl_ReadValue)
{
  int i_ReturnValue = 0;	  	
  uint16 ui_TimerValueLow;
  uint16 ui_TimerValueHigh;
  xPCI3120_DEV   *pDev = xPci3120_data;

  pDev = &xPci3120_data[b_BoardHandle];	/* set up a pointer to the relevant info-structure */
   
  if (b_BoardHandle < xPci3120_nrDev)
    {


      /* Tests if timer is initialized */
      if (pDev->t2Init == APCI3120_ENABLE)
	{
	  {
	    pDev->crt0 = ((pDev->crt0 & 0xF0) | APCI3120_SELECT_TIMER_2_LOW_WORD);
		      
	    /* Reads the low word */
	    outb(pDev->crt0, pDev->addr[1] + APCI3120_TIMER_CRT0);
	    ui_TimerValueLow= inw(pDev->addr[1] + APCI3120_TIMER_VALUE);
	    pDev->crt0 = ((pDev->crt0 & 0xF0) | APCI3120_SELECT_TIMER_2_HIGH_WORD);

	    /* Reads the high word */
	    outb(pDev->crt0, pDev->addr[1] + APCI3120_TIMER_CRT0);
	    ui_TimerValueHigh = inw(pDev->addr[1] + APCI3120_TIMER_VALUE);
	    *pl_ReadValue = ui_TimerValueLow | ((uint32) ui_TimerValueHigh << 16);
	  }
	}
      else
	{
	  /* Timer has not been initialized */
	  i_ReturnValue = -2;
	}        
    }
  else
    {
      i_ReturnValue = -1;
    }	   
   
  return (i_ReturnValue);
}


INT       i_APCI3120_ReadWatchdogStatus     (BYTE    b_BoardHandle,
					     PBYTE  pb_watchdogStatus)
{
  int i_ReturnValue = 0;	  	
  uint16 ui_StatusValue = 0;
  uint8   b_DummyRead;
  xPCI3120_DEV   *pDev = xPci3120_data;

  pDev = &xPci3120_data[b_BoardHandle];	/* set up a pointer to the relevant info-structure */
   
  if (b_BoardHandle < xPci3120_nrDev)
    {


      /* Tests if timer 2 initialized */
      if (pDev->t2Init == APCI3120_ENABLE)
	{
	  /* Tests if watchdog */
	  if (pDev->timerMode == APCI3120_WATCHDOG)
	    {
	      ui_StatusValue= inw(pDev->addr[1] + APCI3120_RD_STATUS);
	      *pb_watchdogStatus = ((ui_StatusValue >> 12) & 1);
	      if (*pb_watchdogStatus)
		{
		  /* RESET FC_TIMER BIT */
		  b_DummyRead = inb(pDev->addr[1] + APCI3120_TIMER_STATUS_REGISTER);
		}
	    }
	  else
	    {
	      /* Watchdog initialized */
	      i_ReturnValue = -3;
	    }
	}
      else
	{
	  /* Timer has not been initialized */
	  i_ReturnValue = -2;
	}        
    }
  else
    {
      i_ReturnValue = -1;
    }	   
   
  return (i_ReturnValue);
}


/*
  +----------------------------------------------------------------------------+
  | Function   Name   : INT     i_APCI3120_Read1DigitalInput                   |
  |                                               (BYTE     b_BoardHandle,     |
  |                                                BYTE     b_ChannelNumber,   |
  |                                                PBYTE   pb_InputValue)      |
  +----------------------------------------------------------------------------+
  | Task              : Reads the status of the digital input                  | 
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE   b_BoardHandle  : Handle of the APCI-3120        |
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
INT i_APCI3120_Read1DigitalInput (BYTE b_BoardHandle,
				  BYTE b_ChannelNumber,
				  PBYTE pb_InputValue)
{
  int i_ReturnValue = 0;	  	
  uint16 ui_TmpRegisterValue = 0;
  xPCI3120_DEV   *pDev = xPci3120_data;
   
  pDev = &xPci3120_data[b_BoardHandle];	/* set up a pointer to the relevant info-structure */
   
  if (b_BoardHandle < xPci3120_nrDev)
    {    
      if (b_ChannelNumber <= 3) 
	{
	  /* Reads the register from address Base[1] + 0x02 */
	  ui_TmpRegisterValue = inw (pDev->addr[1] + APCI3120_DigitalInput);
		
	  /* Selects the one bit given by pArg->b_ChannelNumber */
	  ui_TmpRegisterValue = (ui_TmpRegisterValue >> (8 + (b_ChannelNumber))) & 0x01;
			  
	  /****************************/
	  /* Returns the input status */
	  /****************************/
	  *pb_InputValue = (uint8) ui_TmpRegisterValue;
	}
      else
	{
	  i_ReturnValue = -2;
	}
    }
  else
    {
      i_ReturnValue = -1;
    }	   
   
  return (i_ReturnValue);
}

/*
  +----------------------------------------------------------------------------+
  | Function's Name   : _INT_ i_APCI3120_InitAnalogInputAcquisition            |
  |                               (BYTE_    b_BoardHandle,                     |
  |                                BYTE_    b_SequenzArraySize,                |
  |                                PBYTE_  pb_ChannelArray,                    |
  |                                PBYTE_  pb_GainArray,                       |
  |                                PBYTE_  pb_PolarityArray,                   |
  |                                BYTE_    b_AcquisitionMode,                 |
  |                                BYTE_    b_ExternTrigger,                   |
  |                                UINT_   ui_AcquisitionTiming,               |
  |                                LONG_    l_DelayTiming,                     |
  |                                ULONG_   ul_NumberOfAcquisition,            |
  |                                BYTE_    b_DMAMode,                         |
  |                                BYTE_    b_AcquisitionCycle)                |
  +----------------------------------------------------------------------------+
  | Task              :This function initializes a cyclic conversion.          |
  |                    The priority of the analog inputs is set with the       |
  |                    sequence table.                                         |
  |                    The sequence table allows to determine the input voltage|
  |                    range and the gain for each analog input.               |
  |                    The DMA option (APCI3120_DMA_USED) allows to acquire in |
  |                    the background analog values of a high frequence.       |
  |                    An interupt is generated at the end of the conversion.  |
  |                    In your interrupt routine a "2" is passed through the   |
  |                    parameter b_InterruptMaske. The DMA buffer is returned  |
  |                    through the parameter pui_AnalogInputValue.             |
  |                    See function "i_APCI3120_SetBoardIntRoutine".           |
  |                                                                            |
  |                    You have to :                                           |
  |                       - set the priority of the analog inputs through the  |
  |                         sequence table.                                    |
  |                       - enter the mode through the parameter               |
  |                         b_AcquisitionMode.                                 |
  |                       - enter the time between two conversions through the |
  |                         parameter ui_AcquisitionTiming .                   |
  |                       - enter the waiting time between two conversion      |
  |                         cycles through the parameter l_DelayTiming         |
  |                         (if you use the mode APCI3120_DELAY_MODUS).        |
  |                       - determine if DMA must be used (parameter b_DMAUsed)|
  |                       - enter the number of acquisitions through the       |
  |                         parameter ul_NumberOfAcquisition (if DMA is used)  |
  |                       - enter the DMA conversion cycle through the         |
  |                         parameter b_AcquisitionCycle (if DMA is used)      |
  |                                                                            |
  |                     Table 1: Selecting the analog inputs                   |
  |                                                                            |
  |                    +---------------------+-------------------------+       |
  |                    |     b_Channel       |       Analog input      |       |
  |                    +=====================+=========================+       |
  |                    |  APCI3120_CHANNEL_0 |           0             |       |
  |                    +---------------------+-------------------------+       |
  |                    |  APCI3120_CHANNEL_1 |           1             |       |
  |                    +---------------------+-------------------------+       |
  |                    |  APCI3120_CHANNEL_2 |           2             |       |
  |                    +---------------------+-------------------------+       |
  |                    |  APCI3120_CHANNEL_3 |           3             |       |
  |                    +---------------------+-------------------------+       |
  |                    |  APCI3120_CHANNEL_4 |           4             |       |
  |                    +---------------------+-------------------------+       |
  |                    |  APCI3120_CHANNEL_5 |           5             |       |
  |                    +---------------------+-------------------------+       |
  |                    |  APCI3120_CHANNEL_6 |           6             |       |
  |                    +---------------------+-------------------------+       |
  |                    |  APCI3120_CHANNEL_7 |           7             |       |
  |                    +---------------------+-------------------------+       |
  |                    |  APCI3120_CHANNEL_8 |           8             |       |
  |                    +---------------------+-------------------------+       |
  |                    |  APCI3120_CHANNEL_9 |           9             |       |
  |                    +---------------------+-------------------------+       |
  |                    |  APCI3120_CHANNEL_10|           10            |       |
  |                    +---------------------+-------------------------+       |
  |                    |  APCI3120_CHANNEL_11|           11            |       |
  |                    +---------------------+-------------------------+       |
  |                    |  APCI3120_CHANNEL_12|           12            |       |
  |                    +---------------------+-------------------------+       |
  |                    |  APCI3120_CHANNEL_13|           13            |       |
  |                    +---------------------+-------------------------+       |
  |                    |  APCI3120_CHANNEL_14|           14            |       |
  |                    +---------------------+-------------------------+       |
  |                    |  APCI3120_CHANNEL_15|           15            |       |
  |                    +---------------------+-------------------------+       |
  |                                                                            |
  |                     Table 2: Gain selection                                |
  |                                                                            |
  |                    +---------------------+-------------------------+       |
  |                    |       b_Gain        |          gain           |       |
  |                    +=====================+=========================+       |
  |                    |   APCI3120_1_GAIN   |           1             |       |
  |                    +---------------------+-------------------------+       |
  |                    |   APCI3120_2_GAIN   |           2             |       |
  |                    +---------------------+-------------------------+       |
  |                    |   APCI3120_5_GAIN   |           5             |       |
  |                    +---------------------+-------------------------+       |
  |                    |   APCI3120_10_GAIN  |           10            |       |
  |                    +---------------------+-------------------------+       |
  |                                                                            |
  |                     Table 3: Input voltage range selection                 |
  |                                                                            |
  |                  +-----------------------+----------------------------+    |
  |                  |     b_Polarity        |        Voltage range       |    |
  |                  +=======================+============================+    |
  |                  |   APCI3120_UNIPOLAR   |   0-10V at amplification 1 |    |
  |                  +-----------------------+----------------------------+    |
  |                  |   APCI3120_BIPOLAR    |   ñ10V at amplification 1  |    |
  |                  +-----------------------+----------------------------+    |
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE      b_BoardHandle      : Handle of board APCI3120|
  |                     BYTE      b_SequenzArraySize  : Size of the sequence   |
  |                                                     tables                 |
  |                                                     (1 to 16 elements)     |
  |                     PBYTE     pb_ChannelArray     : Sequence table of the  |
  |                                                     analog inputs.         |
  |                                                     See table 1.           |
  |                     PBYTE     pb_GainArray        : Sequence table of      |
  |                                                     the gain See table 2.  |
  |                     PBYTE     pb_PolarityArray      Sequence table of the  |
  |                                                     input voltage range    |
  |                                                     See table 3.           |
  |                     BYTE      b_AcquisitionMode   : Two conversion cycles  |
  |                                                     are possible:          |
  |                                                   - APCI3120_SIMPLE_MODUS: |
  |                                                     A conversion occurs    |
  |                                                     every                  |
  |                                                     ui_AcquisitionTiming   |
  |                                                     (time interval)        |
  |                                                   - APCI3120_DELAY_MODUS:  |
  |                                                     The both times are used|
  |                                                     in this mode:          |
  |                                                     ui_AcquisitionTiming   |
  |                                                     and l_DelayTiming.     |
  |                                                     Conversions occur every|
  |                                                     ui_AcquisitionTiming   |
  |                                                     (time interval) until  |
  |                                                     all the analog inputs  |
  |                                                     have been acquired     |
  |                                                     (determined            |
  |                                                     by b_SequenzArraySize) |
  |                                                     Step 1.                |
  |                                                     Afterwards there is a  |
  |                                                     waiting time of        |
  |                                                     l_DelayTiming. Step 2  |
  |                                                     The two steps are      |
  |                                                     repeated.              |
  |                     BYTE_       b_ExternTrigger:  - APCI3120_ENABLE :      |
  |                                                                            |
  |                                                                            |
  |                                                   - APCI3120_DISABLE :     |
  |                                                                            |
  |                                                                            |
  |                                                                            |
  |                     UINT      ui_AcquisitionTiming: Time in æs between 2   |
  |                                                     conversions of         |
  |                                                     successive inputs      |
  |                                                     - from 7 *s to 45874 *s|
  |                                                       if you use the Option|
  |                                                       APCI3120_DMA_USED.   |
  |                                                     - from 1500 *s to      |
  |                                                       45874 *s, if you use |
  |                                                       the option           |
  |                                                       APCI3120_NOT_DMA_USED|
  |                     LONG      l_DelayTiming       : Waiting time in æs     |
  |                                                     between two            |
  |                                                     conversion cycles      |
  |                                                     (from 70 *s to         |
  |                                                     4587400 *s).           |
  |                                                     This parameter has only|
  |                                                     one meaning            |
  |                                                     if you use the mode    |
  |                                                     APCI3120_DELAY_MODUS.  |
  |                     ULONG     ul_NumberOfAcquisition : This parameter      |
  |                                                        determines how      |
  |                                                        many conversions    |
  |                                                        have to happen      |
  |                                                        (1 to 32767).       |
  |                     BYTE      b_DMAUsed           : Determines if DMA must |
  |                                                     be used or not.        |
  |                                                   - APCI3120_DMA_USED :    |
  |                                                     All the conversion     |
  |                                                     values are saved and   |
  |                                                     returned to the user.  |
  |                                                     An interrupt is        |
  |                                                     generated when the     |
  |                                                     conversions            |
  |                                                     (set by                |
  |                                                     ui_NumberOfAcquisition)|
  |                                                     are completed.         |
  |                                                                            |
  |                                                   - APCI3120_DMA_NOT_USED :|
  |                                                     An interrupt is        |
  |                                                     generated at the end of|
  |                                                     each conversion and the|
  |                                                     analog value is        |
  |                                                     returned to the user   |
  |                                                     through the interrupt  |
  |                                                     routine.               |
  |                                                                            |
  |                     BYTE      b_AcquisitionCycle  : Determines the type of |
  |                                                     DMA conversion.        |
  |                                                     - APCI3120_CONTINUOUS: |
  |                                                     An interrupt is        |
  |                                                     generated each time a  |
  |                                                     DMA conversion cycle is|
  |                                                     completed .A new DMA   |
  |                                                     conversion cycle is    |
  |                                                     started.               |
  |                                                     - APCI3120_SINGLE:     |
  |                                                     The DMA conversion     |
  |                                                     cycle is only carried  |
  |                                                     out once: i.e.         |
  |                                                     you receive            |
  |                                                     one single interrupt   |
  |                                                     at the end of the      |
  |                                                     first DMA conversion   |
  |                                                     cycle.                 |
  +----------------------------------------------------------------------------+
  | Return Value      :  0  : No error                                         |
  |                     -1  : The handle parameter of the board is wrong       |
  |                     -2  : The user interrupt routine is not installed.     |
  |                           See function "i_APCI3120_SetBoardIntRoutine"     |
  |                     -3  : The size of the sequence table is wrong          |
  |                     -4  : Wrong parameter found in table "pb_ChannelArray" |
  |                     -5  : Wrong parameter found in table "pb_GainArray"    |
  |                     -6  : Wrong parameter found in table "pb_PolarityArray"|
  |                     -7  : The waiting time between two conversion cycles is|
  |                           too high                                         |
  |                     -8  : The selected time for ui_AcquisitionTiming or    |
  |                           l_DelayTiming is wrong                           |
  |                     -9  : The parameter b_DMAUsed is wrong                 |
  |                     -10 : The parametered running time of the DMA          |
  |                           conversion cycle is wrong                        |
  |                           (APCI3120_CONTINUOUS or APCI3120_SINGLE)         |
  |                     -11 : The parametered conversion cycle is wrong        |
  |                           (APCI3120_SIMPLE_MODUS or APCI3120_DELAY_MODUS   |
  |                     -12: Not enough memory available.                      |
  |                     -13: Extern Trigger is wrong                           |
  |                     -14: ul_NumberOfAcquisition is wrong                   |
  |                     -16: The Delay time is too small.                      |
  +----------------------------------------------------------------------------+
  | ATTENTION: In this version of the driver the DMA space allocation is done  |
  |            by allocationg pages.                                           |
  +----------------------------------------------------------------------------+
*/

INT   i_APCI3120_InitAnalogInputAcquisition (BYTE    b_BoardHandle,
					     BYTE    b_SequenzArraySize,
					     PBYTE  pb_ChannelArray,
					     PBYTE  pb_GainArray,
					     PBYTE  pb_PolarityArray,
					     BYTE    b_AcquisitionMode,
					     BYTE    b_ExternTrigger,
					     UINT   ui_AcquisitionTiming,
					     LONG    l_DelayTiming,
					     ULONG   ul_NumberOfAcquisition,
					     BYTE    b_DMAMode,
					     BYTE    b_AcquisitionCycle)
{
  uint8   b_Cpt            = 0;
  uint8   b_DummyRead;
  uint16  ui_StatusValue;
  uint32  ul_DMABufferSize = 0;
  uint16  ui_TimerValue0   = 0;
  uint16  ui_TimerValue1   = 0;
  uint16   uif_DelayValue     = 0;
  uint16   uif_Timer1Value    = 0;
  int16 i_ReturnValue = 0;
  int16 pages;
    
  xPCI3120_DEV   *pDev = xPci3120_data;   
  pDev = &xPci3120_data[b_BoardHandle];	/* set up a pointer to the relevant info-structure */
   
  if (b_BoardHandle < xPci3120_nrDev)
    {    
      pDev->dma.nrAcq = (uint32)(ul_NumberOfAcquisition); /* controller size */
            
      ul_DMABufferSize = pDev->dma.nrAcq + 24;                               /* malloc size */
            
      /* Tests the array size */
      if (b_SequenzArraySize >= 1 && b_SequenzArraySize <= 16)
	{
	  for (b_Cpt = 0; b_Cpt < b_SequenzArraySize; b_Cpt ++)
	    {
	      /* Tests the channel number */
	      if (pb_ChannelArray [b_Cpt] >= pDev->nrAnaIn)
		{
		  /* Channel number error */
		  i_ReturnValue = -4;
		  break;
		}

	      /* Tests the gain */
	      if (pb_GainArray [b_Cpt] != APCI3120_1_GAIN &&  pb_GainArray [b_Cpt] != APCI3120_2_GAIN &&  pb_GainArray [b_Cpt] != APCI3120_5_GAIN &&  pb_GainArray [b_Cpt] != APCI3120_10_GAIN)
		{
		  /* Gain error */
		  i_ReturnValue = -5;
		  break;
		}

	      /* Tests the polarity */
	      if (pb_PolarityArray [b_Cpt] != APCI3120_UNIPOLAR &&  pb_PolarityArray [b_Cpt] != APCI3120_BIPOLAR)
		{
		  /* Polarity error */
		  i_ReturnValue = -6;
		  break;
		}
	      /* Save in Ram Array */
	      pDev->dma.seqArray[b_Cpt] = ((uint16) b_Cpt << 8)| pb_GainArray[b_Cpt] | pb_PolarityArray[b_Cpt] | pb_ChannelArray[b_Cpt];
	    } 

	  if ( (ul_NumberOfAcquisition == 0) || (ul_NumberOfAcquisition > 33554432UL))
	    i_ReturnValue = -14;

	  if ( ((ul_NumberOfAcquisition %2) !=0)  && (b_DMAMode == APCI3120_DMA_USED))
	    i_ReturnValue = -14;

	  if (i_ReturnValue == OK)
	    {
	      /* Tests if an interrupt is installed */
		if (1 == 1) //pDev->interruptInitialized == APCI3120_ENABLE)  HM Test später hier noch nicht
		{
		  /* Tests extern Trigger */
		  if (b_ExternTrigger == APCI3120_ENABLE || b_ExternTrigger == APCI3120_DISABLE)
		    {
		      /* Tests the mode */
		      if (b_AcquisitionMode == APCI3120_SIMPLE_MODUS || b_AcquisitionMode == APCI3120_DELAY_MODUS || b_AcquisitionMode == APCI3120_DELAY_1_MODUS)
			{
			  ui_StatusValue = inw(pDev->addr[1] + APCI3120_RD_STATUS);
			  if ( ( ((ui_AcquisitionTiming >= 10) && (ui_AcquisitionTiming <= 32767U)) && ((l_DelayTiming >= 100) &&  (l_DelayTiming <= 3276750UL) && (b_AcquisitionMode != APCI3120_SIMPLE_MODUS)) ) || b_AcquisitionMode == APCI3120_SIMPLE_MODUS)
			    {
			      i_ReturnValue = 0;
			    }
			  else
			    {
			      i_ReturnValue = -8;
			    }

			  if (i_ReturnValue == 0)
			    {
			      if (b_AcquisitionMode == APCI3120_DELAY_MODUS)
				{
				  if ((uint32)l_DelayTiming + ((uint32)ui_AcquisitionTiming * (uint32)b_SequenzArraySize) > 3276750UL)
				    {
				      i_ReturnValue = -7;
				    }
				}
			    } 
			  /* End 05/03/02 CG 0102/0235 -> 0302/0236*/

			  /* Tests the acquisition time */				    
			  if (i_ReturnValue == 0)
			    {
			      ui_TimerValue0 =  (uint16)((uint32)(((uint32)((((uint32)(ui_AcquisitionTiming)) * 2))))) - 2;
			      if (b_AcquisitionMode == APCI3120_DELAY_MODUS)
				{
				  uif_DelayValue     = (((l_DelayTiming * 2) / 100) - 2);
				  ui_TimerValue1   = (ui_AcquisitionTiming * (uint16) b_SequenzArraySize);
				  uif_Timer1Value   = ((ui_TimerValue1 * 2) / 100);										   
				  ui_TimerValue1  = (uint16) uif_Timer1Value +  (uint16) uif_DelayValue;
				}

			      if (b_AcquisitionMode == APCI3120_DELAY_1_MODUS)
				{
				  uif_DelayValue     = ((l_DelayTiming * 2) / 100 - 2);
				  ui_TimerValue1  = (uint16) uif_DelayValue;
				}

			      /* Save Timer 0 interval */
			      pDev->dma.t0interval  = ui_TimerValue0;		

			      if ((b_AcquisitionMode == APCI3120_DELAY_MODUS) || (b_AcquisitionMode == APCI3120_DELAY_1_MODUS))
				{
				  if ((ui_TimerValue1 > 65535UL) || (((ui_TimerValue1+2) * 100) <= (ui_TimerValue0+2)))
				    {
				      i_ReturnValue =-7;
				    }
							   				 
				  /* Test if the delay time is smaller that the conversion time */ 
				  if ((((ui_TimerValue1+2) * 100) <= ((ui_TimerValue0+2) * (unsigned int)(b_SequenzArraySize))))
				    {
				      i_ReturnValue =-16;
				    }
				
				  if (i_ReturnValue == 0)
				    {
				      pDev->dma.t1interval  = ui_TimerValue1;
				    }
				}

			      /* Tests the DMA */
			      if ((b_DMAMode == APCI3120_DMA_USED) || (b_DMAMode == APCI3120_DMA_NOT_USED))
				{
				  if (b_DMAMode == APCI3120_DMA_USED)
				    {
				      /* Tests the mode */
				      if (b_AcquisitionCycle ==  APCI3120_CONTINUOUS ||  b_AcquisitionCycle == APCI3120_SINGLE)
					{
					  /* Tests if the DMA is already used */
					  if (pDev->dma.acqEnable == APCI3120_ENABLE)
					    {
					      i_ReturnValue = i_APCI3120_StopAnalogInputAcquisition(b_BoardHandle);
					      switch (pDev->dma.acqCycle)
						{
						case APCI3120_SINGLE:
						  /* Free DMA buffer */
						  if (pDev->dma.dmaBuffer != NULL)
						    free_pages ((uint32)pDev->dma.dmaBuffer, pDev->dma.dmaBufferPages);
						  
						  pDev->dma.dmaBuffer = NULL;   
						  break;
                        
						case APCI3120_CONTINUOUS:
						  /* Free DMA buffer */
						  if (pDev->dma.dmaBuffer != NULL)
						    free_pages ((uint32)pDev->dma.dmaBuffer, pDev->dma.dmaBufferPages);
						  if (pDev->dma.oldDmaBuffer != NULL)
						    free_pages ((uint32)pDev->dma.oldDmaBuffer, pDev->dma.oldDmaBufferPages);
						  pDev->dma.dmaBuffer = NULL;   
						  pDev->dma.oldDmaBuffer = NULL;   
						  break;
						}
					    }
                                       
					  /* Clears DMA buffer index */
					  if (b_AcquisitionCycle == APCI3120_CONTINUOUS)
					    {
					      /* Malloc DMA buffer */
					      for (pages=4; pages>=0; pages--)
						{
						  if((pDev->dma.dmaBuffer = (void *) __get_free_pages(__GFP_DMA,pages)))
						    break;                
						} 
					      printk("pagealloc dma apci3120 %d\n",pages); //HM
                                       	
					      //BEGIN JK 21.06.2004: Test if allocated memory is enough			
//					      if ((pDev->dma.oldDmaBuffer == NULL) || (ul_DMABufferSize > PAGE_SIZE*pages))
//FIXME falscher Vergleich
					      if ((pDev->dma.dmaBuffer != NULL) && (ul_DMABufferSize <= PAGE_SIZE*pages)) 
						//if (pDev->dma.dmaBuffer == NULL)
						//END JK 21.06.2004: Test if allocated memory is enough
						{
						  pDev->dma.dmaBufferPages=pages;
						  pDev->dma.dmaBufferSize=PAGE_SIZE*pages;																														
						  pDev->dma.dmaBufferHw = (void *) virt_to_bus(pDev->dma.dmaBuffer);
					            							
						  for (pages=4; pages>=0; pages--)
						    {
						      if((pDev->dma.oldDmaBuffer = (void *) __get_free_pages(__GFP_DMA,pages)))
							break;                
						    }
						  printk("pagealloc olddma apci3120 %d\n",pages); //HM
						  //BEGIN JK 21.06.2004: Test if allocated memory is enough			
						  if ((pDev->dma.oldDmaBuffer == NULL) || (ul_DMABufferSize > PAGE_SIZE*pages))
						    {
						      //if (pDev->dma.oldDmaBuffer == NULL)
						      //END JK 21.06.2004: Test if allocated memory is enough 
						      /* Not enough memory available */
						      i_ReturnValue = -12;
						    }
						  else
						    {
						      pDev->dma.oldDmaBufferPages=pages;
						      pDev->dma.oldDmaBufferSize=PAGE_SIZE*pages;
						      pDev->dma.oldDmaBufferHw = (void *) virt_to_bus(pDev->dma.oldDmaBuffer);	
						    }
                                             
						}
					      else
						{
						  /* Not enough memory available */
						  i_ReturnValue = -12;
						}
					    }
					  else
					    {
					      /* Allocate DMA buffer */
	
					      for (pages=4; pages>=0; pages--)
						{
						  if((pDev->dma.dmaBuffer = (void *) __get_free_pages(__GFP_DMA,pages)))
						    break;                
						}                       			     					     					      					      					     
					      printk("pagealloc single dma apci3120 %d\n",pages); //HM
						
					      //BEGIN JK 21.06.2004: Test if allocated memory is enough			
					      if ((pDev->dma.dmaBuffer == NULL) || (ul_DMABufferSize > PAGE_SIZE*pages))
						//if (pDev->dma.dmaBuffer == NULL)
						//END JK 21.06.2004: Test if allocated memory is enough
						{
						  i_ReturnValue = -12;
						}
					      else
						{
						  pDev->dma.dmaBufferPages=pages;
						  pDev->dma.dmaBufferSize=PAGE_SIZE*pages;																														
						  pDev->dma.dmaBufferHw = (void *) virt_to_bus(pDev->dma.dmaBuffer);
						}

                                          
					    }
					}
				      else
					{
					  /* DMA mode error */
					  i_ReturnValue = -10;
					}
				    } 
				}
			      else
				{
				  /* DMA error */
				  i_ReturnValue = -9;
				}

			      if (!i_ReturnValue)
				{
				  /* Resets the FIFO */
				  b_DummyRead = inb(pDev->addr[1] + APCI3120_RESET_FIFO);

				  /* DMA Acquisition initialized */
				  pDev->dma.acqEnable = APCI3120_ENABLE;

				  /* Saves informations */
				  pDev->dma.acqCycle = (uint8) b_AcquisitionCycle;
				  pDev->dma.acqMode = (uint8) b_AcquisitionMode;
				  pDev->dma.nrAcq = (uint32) ul_NumberOfAcquisition;
				  pDev->dma.seqArraySize = (uint8) b_SequenzArraySize;
				  pDev->dma.extTrigger = (uint8) b_ExternTrigger;
				  pDev->dma.dmaUsed = b_DMAMode;
				  pDev->dma.bufIndex = 0;
				} 
			    }
			  else
			    {
			      /* pArg->ui_AcquisitionTiming error  */
			      i_ReturnValue = -8;
			    }
			}
		      else
			{
			  /* SIMPLE OR DELAY ERROR */
			  i_ReturnValue = -11;
			}
		    }
		  else
		    {
		      /* Extern Trigger Error */
		      i_ReturnValue = -13;
		    }
		}
	      else
		{
		  /* Interrupt error */
		  i_ReturnValue = -2;
		}
	    }
	}
      else
	{
	  /* The size of the sequence array is wrong */
	  i_ReturnValue = -3;
	}

    }
  else
    {
      /* The boardhandle is wrong */
      i_ReturnValue = -1;
    }

  return (i_ReturnValue);
}


/*
  +----------------------------------------------------------------------------+
  | Function's Name   : _INT_ i_APCI3120_StartAnalogInputAcquisition           |
  |                               (BYTE_    b_BoardHandle)                     |
  +----------------------------------------------------------------------------+
  | Task              : Starts the cyclic conversion. It has been previously   |
  |                     initialized with function                              |
  |                     "i_APCI3120_InitAnalogInputAcquisition".               |
  |                                                                            |
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE      b_BoardHandle     : Handle of board APCI3120 |
  +----------------------------------------------------------------------------+
  | Return Value      :  0: No error                                           |
  |                     -1: The handle parameter of the board is wrong         |
  |                     -2: The cyclic conversion has not been initialized.    |
  |                         "i_APCI3120_InitAnalogInputAcquisition"            |
  |                     -3: Interrupt routine has not been installed. See      |
  |                         function "i_APCI3120_SetBoardIntRoutine            |
  +----------------------------------------------------------------------------+
*/

INT   i_APCI3120_StartAnalogInputAcquisition  (BYTE b_BoardHandle)
{
  int i_ReturnValue = 0;
  uint16 ui_Timer2LowWord;
  uint16 ui_Timer2HighWord;
  uint8  b_Cpt;
  uint8   b_DummyRead;
  uint32 ul_DMABufferSize = 0;
				
  xPCI3120_DEV   *pDev = xPci3120_data;   
  pDev = &xPci3120_data[b_BoardHandle];	/* set up a pointer to the relevant info-structure */
   
  if (b_BoardHandle < xPci3120_nrDev)
    {   

      /* Tests if acquisition is initialised */
      if (pDev->dma.acqEnable == APCI3120_ENABLE)
	{
	  	
	  /* Clear Timer Write TC int16 */
	  outl(APCI3120_CLEAR_WRITE_TC_INT, pDev->addr[0] + APCI3120_AMCC_OP_REG_INTCSR);

  	
	  /* Clears the timer status register */
	  b_DummyRead = inb(pDev->addr[1] + APCI3120_TIMER_STATUS_REGISTER);
	    
	  /* Disables All Timer     */
	  /* Sets PR and PA to 0    */
	  pDev->nWrAddr = pDev->nWrAddr & APCI3120_DISABLE_TIMER0 & APCI3120_DISABLE_TIMER1 & APCI3120_CLEAR_PA_PR ;

  	
	  if (pDev->dma.dmaUsed == APCI3120_DMA_NOT_USED)
	    {
	      /* Disables Timer 2       */
	      pDev->nWrAddr = pDev->nWrAddr & APCI3120_DISABLE_TIMER2;
		
	    }
	  outw(pDev->nWrAddr, pDev->addr[1] + APCI3120_WR_ADDRESS);
	  /* Resets the FIFO */
	  b_DummyRead = inb(pDev->addr[1] + APCI3120_RESET_FIFO);
	    

	  /* Initializes the sequence array */
	  for (b_Cpt = 0; b_Cpt < pDev->dma.seqArraySize; b_Cpt ++)
	    {
	      outw(pDev->dma.seqArray[b_Cpt], pDev->addr[1] + APCI3120_FIFO_ADDRESS);
	    }

	  /* Initializes timer 0 in mode 2 */ /* crt1 */
	  pDev->crt1 = (pDev->crt1 & 0xFC) | APCI3120_TIMER_0_MODE_2;

  					   
	  /* Tests if delay modus */
				   
	  if ((pDev->dma.acqMode == APCI3120_DELAY_MODUS) || (pDev->dma.acqMode == APCI3120_DELAY_1_MODUS))
	    {
	      /* Initializes Timer 1 in mode 2 */
	      pDev->crt1 = (pDev->crt1 & 0xF3) | APCI3120_TIMER_1_MODE_2;
		
	    }

	  if (pDev->dma.dmaUsed == APCI3120_DMA_NOT_USED)
	    {
	      /* Initializes timer 2 in mode 0 */
	      pDev->crt1 = (pDev->crt1 & 0x0F) | APCI3120_TIMER_2_MODE_0;
		
	    }
				   
	  /* Writes timer mode */
	  outb(pDev->crt1, pDev->addr[1] + APCI3120_TIMER_CRT1);

  	
	  if (pDev->dma.dmaUsed == APCI3120_DMA_NOT_USED)
	    {
	      pDev->crt0 = ((pDev->crt0 & 0xF0) | APCI3120_SELECT_TIMER_2_LOW_WORD);
	      /* Writes timer 2 low word */
	      outb(pDev->crt0, pDev->addr[1] + APCI3120_TIMER_CRT0);
	      /* CG Version -> 0999/0220.*/

		
	      ul_DMABufferSize = (pDev->dma.nrAcq - 2);
			
	      if (ul_DMABufferSize <=  0)
		ul_DMABufferSize = 1;
			
	      ui_Timer2LowWord = (uint16)ul_DMABufferSize;
	      ui_Timer2HighWord = (uint16)(ul_DMABufferSize >> 16);
			
	      outw(ui_Timer2LowWord, pDev->addr[1] + APCI3120_TIMER_VALUE);

	      pDev->crt0 = ((pDev->crt0 & 0xF0) | APCI3120_SELECT_TIMER_2_HIGH_WORD);
	      /* Writes timer 2 high word */
	      outb(pDev->crt0, pDev->addr[1] + APCI3120_TIMER_CRT0);

	  				
	      /* CG Version -> 0999/0220.*/
	      outw(ui_Timer2HighWord, pDev->addr[1] + APCI3120_TIMER_VALUE);
		
	    }

	  pDev->crt0 = ((pDev->crt0 & 0xF0) | APCI3120_SELECT_TIMER_0_WORD);

	  /* Selects timer 0 */
	  outb(pDev->crt0, pDev->addr[1] + APCI3120_TIMER_CRT0);

		  	
	  /* Sets the conversion time */
	  outw(pDev->dma.t0interval, pDev->addr [1] + APCI3120_TIMER_VALUE);
		  	
	  /* Tests if delay modus */

	  if ((pDev->dma.acqMode == APCI3120_DELAY_MODUS) || (pDev->dma.acqMode == APCI3120_DELAY_1_MODUS))
	    {
				
	      pDev->crt0 = ((pDev->crt0 & 0xF0) | APCI3120_SELECT_TIMER_1_WORD);
	      /* Selects timer 1 */
	      outb(pDev->crt0, pDev->addr[1] + APCI3120_TIMER_CRT0);
		

		
	      /* Sets the delay time */
	      outw(pDev->dma.t1interval, pDev->addr[1] + APCI3120_TIMER_VALUE);
		

	    } 

	  if (pDev->dma.dmaUsed == APCI3120_DMA_NOT_USED)
	    {
	      /* Clears the timer status register */
	      b_DummyRead = inb(pDev->addr[1] + APCI3120_TIMER_STATUS_REGISTER);
				         
	      /* Enables the timer counter */
	      pDev->modeSelect = pDev->modeSelect | APCI3120_ENABLE_TIMER_COUNTER;
					         
	      /* Selects EOS clock input for timer 2 */
	      pDev->modeSelect = pDev->modeSelect | APCI3120_TIMER2_SELECT_EOS;
	      pDev->t2lastInterrupt = pDev->t2Interrupt;
	      pDev->t2Interrupt = APCI3120_ENABLE;
			
	      pDev->t2lastMode = pDev->timerMode;
	      pDev->timerMode = APCI3120_COUNTER;
                	      
	    }

	  /* Clears the SCAN bit */
	  pDev->modeSelect = pDev->modeSelect	& APCI3120_DISABLE_SCAN;
	    


	  /* Tests if interrupt installed */

	  if (pDev->interruptInitialized == APCI3120_ENABLE)
	    {
	      /* Tests if DMA used */

	      if (pDev->dma.dmaUsed == APCI3120_DMA_USED)
		{
		  /* Disables the EOC, EOS interrupt  */
		  pDev->modeSelect = (pDev->modeSelect & APCI3120_DISABLE_EOC_INT & APCI3120_DISABLE_EOS_INT);
		  pDev->intMode = APCI3120_DMA_INT_MODE;

		}
	      else /*if (ps_APCI3120Variable ...b_DMAUsed == APCI3120_DMA_USED)*/
		{
		  /* Disables the EOC and enables the EOS interrupt */
		  pDev->modeSelect = (pDev->modeSelect & APCI3120_DISABLE_EOC_INT ) | APCI3120_ENABLE_EOS_INT;

		
		  pDev->intMode =  APCI3120_EOS_INT_MODE;
		}/* else if (ps_APCI3120Variable ...b_DMAUsed == APCI3120_DMA_USED)*/
		
	      /* Tests if the timer 2 interrupt is enabled */

	      if (pDev->t2Interrupt == APCI3120_ENABLE)
		{
		  pDev->modeSelect = pDev->modeSelect | APCI3120_ENABLE_TIMER_INT;
		}
	    }
	  else /* Test if interrupt is installed*/
	    {
	      pDev->modeSelect = pDev->modeSelect & APCI3120_DISABLE_ALL_INTERRUPT;
	      /* Interrupt routine has not been installed */

	      i_ReturnValue = -3;
	    }

	  /* Writes the interrupt select register */
	  outb(pDev->modeSelect, pDev->addr[1] + APCI3120_WRITE_MODE_SELECT);

					
	  if (i_ReturnValue == 0)
	    {
	      /* Test if DMA is used */

	      if (pDev->dma.dmaUsed == APCI3120_DMA_USED)
		{
		  pDev->dma.dmaSelect = APCI3120_ENABLE;
		  pDev->lastChannel = APCI3120_MAX_INPUT_CHANNEL + 1;

		  /* ENABLE BUS MASTER */
		  outw(APCI3120_ADD_ON_AGCSTS_LOW, pDev->addr[2]);
		  outw(APCI3120_ENABLE_TRANSFER_ADD_ON_LOW, pDev->addr[2] + 2);
		  outw(APCI3120_ADD_ON_AGCSTS_HIGH, pDev->addr[2]);
		  outw(0x1000, pDev->addr[2] + 2);

		  /* A2P FIFO MANAGEMENT */
		  outl(APCI3120_A2P_FIFO_MANAGEMENT, pDev->addr[0] + APCI3120_AMCC_OP_MCSR);
							
		  /* DMA Start Address Low */
		  outw(APCI3120_ADD_ON_MWAR_LOW, pDev->addr[2]);
		  outw((uint16)((uint32)pDev->dma.dmaBufferHw & 0x0000FFFFUL), pDev->addr[2] + 2);



		  /* DMA Start Adress High */
		  outw(APCI3120_ADD_ON_MWAR_HIGH, pDev->addr[2]);
		  outw((uint16)(((uint32)pDev->dma.dmaBufferHw & 0xFFFF0000UL) >> 16), pDev->addr[2] + 2);

                           
		  /* Inc Buffer Index */
		  pDev->dma.bufIndex = (pDev->dma.bufIndex ^ 1);  /* toggle index from 0 to 1 */



		  /* Nbr of acquisition LOW */
		  outw(APCI3120_ADD_ON_MWTC_LOW, pDev->addr[2]);
		  outw((uint16)((pDev->dma.nrAcq * 2) & 0xFFFF), pDev->addr[2] + 2);

							
		  /* Nbr of acquisition HIGH */
		  outw(APCI3120_ADD_ON_MWTC_HIGH, pDev->addr[2]);
		  outw((uint16)((pDev->dma.nrAcq * 2 ) / 65536UL), pDev->addr[2] + 2);


		  /* A2P FIFO RESET */
		  outl(0x04000000, pDev->addr[0] + APCI3120_AMCC_OP_MCSR);


		  /* A2P FIFO CONFIGURATE, END OF DMA INTERRUPT INIT */
		  outl((APCI3120_FIFO_ADVANCE_ON_BYTE_2 | APCI3120_ENABLE_WRITE_TC_INT), pDev->addr[0] + APCI3120_AMCC_OP_REG_INTCSR);


		} /* dma used*/

	      /* Clears PA, sets PR */
	      pDev->nWrAddr = ((pDev->nWrAddr & APCI3120_CLEAR_PA_PR)| (((uint16)pDev->dma.seqArraySize - 1) << 8));



	      /* Extern Trigger */

	      if (pDev->dma.extTrigger == APCI3120_ENABLE)
		{
		  pDev->nWrAddr = (pDev->nWrAddr | APCI3120_ENABLE_EXT_TRIGGER);
		}
	      else
		{
		  pDev->nWrAddr = (pDev->nWrAddr & APCI3120_DISABLE_EXT_TRIGGER);
		}


	      if (pDev->dma.dmaUsed == APCI3120_DMA_NOT_USED)
		{
		  /* Sets gate 2 */
		  pDev->nWrAddr = pDev->nWrAddr | APCI3120_ENABLE_TIMER2;
		}

	      /* Tests if delay modus */
	      if ((pDev->dma.acqMode == APCI3120_DELAY_MODUS) ||	 (pDev->dma.acqMode == APCI3120_DELAY_1_MODUS))
		{
		  /* Set Gate 1 */
		  pDev->nWrAddr = (pDev->nWrAddr | APCI3120_ENABLE_TIMER1);
		}

	      /* Set Gate 0 */
	      pDev->nWrAddr = (pDev->nWrAddr | APCI3120_ENABLE_TIMER0);

	      if (pDev->dma.dmaUsed == APCI3120_DMA_USED)
		{
		  /* ENABLE A2P FIFO WRITE AND ENABLE AMWEN */
		  outw(3, pDev->addr[2] + 4);
		  outl(0x04000000, pDev->addr[0] + APCI3120_AMCC_OP_MCSR);


		}
	      /* Starts timer */
	      outw(pDev->nWrAddr, pDev->addr[1] + APCI3120_WR_ADDRESS);

	      pDev->lastChannel = 0;
	    }
	}
      else
	{
	  /* The cyclic conversion has not been initialized */
	  i_ReturnValue = -2;
	}
    }
  else
    {
      /* The boardhandle is wrong */
      i_ReturnValue = -1;
    }

  return (i_ReturnValue);
}

/*
  +----------------------------------------------------------------------------+
  | Function's Name   : _INT_ i_APCI3120_ClearAnalogInputAcquisition           |
  |                               (BYTE_    b_BoardHandle)                     |
  +----------------------------------------------------------------------------+
  | Task              : Deinstalls the DMA buffer.                             |
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE      b_BoardHandle     : Handle of board APCI3120 |
  +----------------------------------------------------------------------------+
  | Return Value      :  0: No error.                                          |
  |                     -1: The handle parameter of the board is wrong.        |
  |                     -2: The cyclic conversion has not been initialized.    |
  |                         "i_APCI3120_InitAnalogInputAcquisition"            |
  +----------------------------------------------------------------------------+
*/

INT   i_APCI3120_ClearAnalogInputAcquisition     (BYTE b_BoardHandle)
{
  int i_ReturnValue = 0;		
  xPCI3120_DEV   *pDev = xPci3120_data;   
  pDev = &xPci3120_data[b_BoardHandle];	/* set up a pointer to the relevant info-structure */
   
  if (b_BoardHandle < xPci3120_nrDev)
    { 
      i_ReturnValue = i_APCI3120_StopAnalogInputAcquisition(b_BoardHandle);

      /* Test if Acquisition initialised */
      if (pDev->dma.acqEnable == APCI3120_ENABLE)
	{
	  /* Test if DMA Used */
	  if (pDev->dma.dmaUsed == APCI3120_ENABLE)
	    {
	      switch (pDev->dma.acqCycle)
		{
		case APCI3120_SINGLE:
		  /* Free DMA buffer */
		  if (pDev->dma.dmaBuffer != NULL)
		    free_pages ((uint32)&pDev->dma.dmaBuffer, pDev->dma.dmaBufferPages);
		  pDev->dma.dmaBuffer = NULL;   
		  break;

		case APCI3120_CONTINUOUS:
		  /* Free DMA buffer */
		  if (pDev->dma.dmaBuffer != NULL)
		    free_pages ((uint32)pDev->dma.dmaBuffer, pDev->dma.dmaBufferPages);
		  if (pDev->dma.oldDmaBuffer != NULL)
		    free_pages ((uint32)pDev->dma.oldDmaBuffer, pDev->dma.oldDmaBufferPages);
		  pDev->dma.dmaBuffer = NULL;   
		  pDev->dma.oldDmaBuffer = NULL;   
		  break;
		}
	    }
	  pDev->dma.acqEnable = APCI3120_DISABLE;
	  pDev->dma.extTrigger = APCI3120_DISABLE;
	}
      else
	{
	  /* Cyclic conversion has not been initialized */
	  i_ReturnValue = -2;
	}
    }
  else
    {
      /* The boardhandle is wrong */
      i_ReturnValue = -1;
    }

  return (i_ReturnValue);
}
  
/*
  +----------------------------------------------------------------------------+
  | Function name   : INT        i_APCI3120_SetBoardInterruptRoutine           |
  |                                              (BYTE     b_BoardHandle)      |
  +----------------------------------------------------------------------------+
  | Task              : Initialises the interruptroutine for the APCI-3120     |
  +----------------------------------------------------------------------------+
  | Input parameters  : BYTE   b_BoardHandle :Handle of the APCI-3120          |
  +----------------------------------------------------------------------------+
  | Output Parameters : ---                                                    |
  +----------------------------------------------------------------------------+
  | Return Value      :  0 : No error                                          |
  |                     -1 : The handle parameter of the board is wrong        |
  |                     -2 : Interrupt routine already installed               |
  |                     -3 : Interrupt routine not installed                   |
  +----------------------------------------------------------------------------+
*/

/* INT      i_APCI3120_SetBoardInterruptRoutine (BYTE    b_BoardHandle, */
/*                                               void (*usrHdl) (uint8 handle, uint8 intMask, uint16 *aiv)) */
/* { */
/*   int i_ReturnValue = 0;		 */
/*   xPCI3120_DEV   *pDev = xPci3120_data;    */
/*   pDev = &xPci3120_data[b_BoardHandle];	/\* set up a pointer to the relevant info-structure *\/ */
   
/*   if (b_BoardHandle < xPci3120_nrDev) */
/*     {  */
/*       /\****************************************\/ */
/*       /\* Test if interrupt allready installed *\/ */
/*       /\****************************************\/ */

/*       if (pDev->interruptInitialized == APCI3120_DISABLE) */
/* 	{ */
/* 	  // Get the interrupt function to use	  */
/* 	  pDev->usrHdl = usrHdl; */
		
/* 	  if (request_irq(pDev->intrpt,  */
/* 			  APCI3120_Interrupt,  */
/* 			  SA_SHIRQ,  */
/* 			  "APCI3120",  */
/* 			  pDev) == 0) */
/* 	    {			 */
/* 	      pDev->interruptInitialized = APCI3120_ENABLE;      */
/* 	    } */
/* 	  else */
/* 	    { */
/* 	      i_ReturnValue = -3;	      */
/* 	      pDev->interruptInitialized = APCI3120_DISABLE;  */
/* 	    } */
/* 	} */
/*       else */
/* 	{ */
/* 	  /\***************************************\/ */
/* 	  /\* Interrupt routine already installed *\/ */
/* 	  /\***************************************\/ */

/* 	  i_ReturnValue = -2; */
/* 	}  */

/*     } */
/*   else */
/*     { */
/*       /\*****************************\/ */
/*       /\* The board handle is wrong *\/ */
/*       /\*****************************\/ */

/*       i_ReturnValue = -1; */
/*     } */
/*   return (i_ReturnValue); */
/* }   */


/*
  +----------------------------------------------------------------------------+
  | Function Name     : _INT_ i_APCI3120_ResetBoardIntRoutine                  |
  |                                                      (BYTE_ b_BoardHandle) |
  +----------------------------------------------------------------------------+
  | Task              : Stops the interrupt management of board APCI-3120.     |
  |                     Deinstalls the user interrupt routine if the management|
  |                     of interrupts of all boards APCI-3120 is stopped.      |
  +----------------------------------------------------------------------------+
  | Input Parameters  : BYTE_ b_BoardHandle : Handle of board APCI-3120        |
  +----------------------------------------------------------------------------+
  | Output Parameters : -                                                      |
  +----------------------------------------------------------------------------+
  | Return Value      :  0: No error                                           |
  |                     -1: The handle parameter of the board is wrong         |
  |                     -2: No Interrupt function initialised with function    |
  |                         "i_APCI3120_SetBoardIntRoutine"                    |
  +----------------------------------------------------------------------------+
*/

INT   i_APCI3120_ResetBoardIntRoutine     (BYTE b_BoardHandle)
{
  int i_ReturnValue = 0;		
  xPCI3120_DEV   *pDev = xPci3120_data;   
  pDev = &xPci3120_data[b_BoardHandle];	/* set up a pointer to the relevant info-structure */
   
  if (b_BoardHandle < xPci3120_nrDev)
    { 
      free_irq (pDev->intrpt, pDev);
      pDev->interruptInitialized = APCI3120_DISABLE;
    }
  else
    {
      /*****************************/
      /* The board handle is wrong */
      /*****************************/

      i_ReturnValue = -1;
    }
  return (i_ReturnValue);
}


/* gibt die Interruptnummer für die Karte zurück */
  
int i_APCI3120_Get_irq(BYTE b_BoardHandle)
{
  xPCI3120_DEV   *pDev = xPci3120_data;   
  pDev = &xPci3120_data[b_BoardHandle];	/* set up a pointer to the relevant info-structure */
   
  if (b_BoardHandle < xPci3120_nrDev)
      return pDev->intrpt;
  else
      return -1;
}



INT      i_APCI3120_FakeBoardInterruptRoutine (BYTE    b_BoardHandle)
{
  xPCI3120_DEV   *pDev = xPci3120_data;   
  pDev = &xPci3120_data[b_BoardHandle];	/* set up a pointer to the relevant info-structure */
   
  if (b_BoardHandle < xPci3120_nrDev)
    { 
	pDev->interruptInitialized = APCI3120_ENABLE;     
    }
  return 0;
}

uint16 *i_APCI3120_GetDMABufferP (uint8 b_BoardHandle)
{
  xPCI3120_DEV   *pDev = xPci3120_data;   
  pDev = &xPci3120_data[b_BoardHandle];	/* set up a pointer to the relevant info-structure */
   
  if (b_BoardHandle < xPci3120_nrDev)
    { 
	if (pDev->dma.bufIndex == 0)
	{
	    return ( (uint16 *) pDev->dma.dmaBuffer);
	}
	else
	{
	    return ( (uint16 *) pDev->dma.oldDmaBuffer);
	}
    }
  else 
      return NULL;
}


/*
  +----------------------------------------------------------------------------+
  | Function name   : int sample01(void)                                       |
  +----------------------------------------------------------------------------+
  | Task              : Test driver function without DMA                       |
  +----------------------------------------------------------------------------+
  | Input parameters  : -                                                      |
  +----------------------------------------------------------------------------+
  | Output Parameters : -                                                      |
  +----------------------------------------------------------------------------+
  | Return Value      :  0                                                     |
  +----------------------------------------------------------------------------+
*/

/* int sample01(void) */
/* { */
/*   int   i_ReturnValue                     = 0; */
/*   LONG  l_ReadValue                       = 0; */
/*   UINT ui_ReadValue                       = 0; */
    
/*   BYTE b_ChannelArray[16]; */
/*   BYTE b_GainArray[16]; */
/*   BYTE b_PolarityArray[16]; */
/*   UINT ui_AnalogValue[16]; */
/*   BYTE b_ChannelArraySize                 = 12; */
/*   BYTE b_BoardHandle = 0; */
    

/*   b_ChannelArray[0] = 0; */
/*   b_ChannelArray[1] = 1; */
/*   b_ChannelArray[2] = 2; */
/*   b_ChannelArray[3] = 3; */
/*   b_ChannelArray[4] = 3; */
/*   b_ChannelArray[5] = 2; */
/*   b_ChannelArray[6] = 1; */
/*   b_ChannelArray[7] = 0; */
/*   b_ChannelArray[8] = 1; */
/*   b_ChannelArray[9] = 2; */
/*   b_ChannelArray[10] = 2; */
/*   b_ChannelArray[11] = 3; */

/*   b_GainArray[0] = APCI3120_1_GAIN; */
/*   b_GainArray[1] = APCI3120_1_GAIN; */
/*   b_GainArray[2] = APCI3120_1_GAIN; */
/*   b_GainArray[3] = APCI3120_1_GAIN; */
/*   b_GainArray[4] = APCI3120_1_GAIN; */
/*   b_GainArray[5] = APCI3120_1_GAIN; */
/*   b_GainArray[6] = APCI3120_1_GAIN; */
/*   b_GainArray[7] = APCI3120_1_GAIN; */
/*   b_GainArray[8] = APCI3120_1_GAIN; */
/*   b_GainArray[9] = APCI3120_1_GAIN; */
/*   b_GainArray[10] = APCI3120_1_GAIN; */
/*   b_GainArray[11] = APCI3120_1_GAIN; */

/*   b_PolarityArray[0] = APCI3120_UNIPOLAR; */
/*   b_PolarityArray[1] = APCI3120_UNIPOLAR; */
/*   b_PolarityArray[2] = APCI3120_UNIPOLAR; */
/*   b_PolarityArray[3] = APCI3120_UNIPOLAR; */
/*   b_PolarityArray[4] = APCI3120_UNIPOLAR; */
/*   b_PolarityArray[5] = APCI3120_UNIPOLAR; */
/*   b_PolarityArray[6] = APCI3120_UNIPOLAR; */
/*   b_PolarityArray[7] = APCI3120_UNIPOLAR; */
/*   b_PolarityArray[8] = APCI3120_UNIPOLAR; */
/*   b_PolarityArray[9] = APCI3120_UNIPOLAR; */
/*   b_PolarityArray[10] = APCI3120_UNIPOLAR; */
/*   b_PolarityArray[11] = APCI3120_UNIPOLAR; */

/*   i_ReturnValue = i_APCI3120_SetBoardInterruptRoutine (b_BoardHandle, v_InterruptRoutine); */
/*   printk("SetBoardInterruptRoutine: %d\n", i_ReturnValue); */
    
/*   i_ReturnValue = i_APCI3120_InitTimerWatchdog     (b_BoardHandle, */
/* 						    APCI3120_TIMER, */
/* 						    5000, */
/* 						    APCI3120_ENABLE); */
/*   printk("InitTimerWatchdog: %d\n", i_ReturnValue); */
    
/*   i_ReturnValue = i_APCI3120_StartTimerWatchdog (b_BoardHandle); */
/*   printk("StartTimerWatchdog: %d\n", i_ReturnValue); */
    
/*   i_ReturnValue = i_APCI3120_ReadTimer (b_BoardHandle, &l_ReadValue);     */
/*   printk("ReadTimer: %d\n", i_ReturnValue); */
    
/*   printk("Timer Value: %lX\n", l_ReadValue); */
    
/*   i_ReturnValue = i_APCI3120_InitAnalogInput (b_BoardHandle, */
/* 					      1, */
/* 					      APCI3120_1_GAIN,  */
/* 					      APCI3120_UNIPOLAR, */
/* 					      150); */
/*   printk("Init Analog Input: %d\n", i_ReturnValue); */

/*   i_ReturnValue = i_APCI3120_StartAnalogInput (b_BoardHandle);     */
/*   printk("Start Analog Input: %d\n", i_ReturnValue); */
    
/*   i_ReturnValue = i_APCI3120_ReadAnalogInputBit (b_BoardHandle); */
/*   printk("Read Analog Input Bit: %d\n", i_ReturnValue); */

/*   do */
/*     { */
/*       i_ReturnValue = i_APCI3120_ReadAnalogInputBit (b_BoardHandle); */
/*     } */
/*   while (i_ReturnValue == -2); */

/*   i_ReturnValue = i_APCI3120_ReadAnalogInputBit (b_BoardHandle); */
/*   printk("Read Analog Input Bit: %d\n", i_ReturnValue); */

/*   i_ReturnValue = i_APCI3120_ReadAnalogInputValue (b_BoardHandle, */
/* 						   &ui_ReadValue); */
/*   printk("Read Analog Input: %d\n", i_ReturnValue); */
/*   printk("Analog Input: %d\n", ui_ReadValue); */



/*   i_ReturnValue = i_APCI3120_InitAnalogInputScan (b_BoardHandle, */
/* 						  b_ChannelArraySize, */
/* 						  b_ChannelArray,  */
/* 						  b_GainArray,  */
/* 						  b_PolarityArray, */
/* 						  150); */
/*   printk("Init Analog Input Scan: %d\n", i_ReturnValue); */
		  
/*   i_ReturnValue = i_APCI3120_StartAnalogInputScan (b_BoardHandle); */
/*   printk("Start Analog Input Scan: %d\n", i_ReturnValue); */

/*   i_ReturnValue = i_APCI3120_ReadAnalogInputScanBit (b_BoardHandle); */
/*   printk("Read Analog Input Scan Bit: %d\n", i_ReturnValue); */

/*   do */
/*     { */
/*       i_ReturnValue = i_APCI3120_ReadAnalogInputScanBit (b_BoardHandle); */
/*     } */
/*   while (i_ReturnValue == -2); */

/*   i_ReturnValue = i_APCI3120_ReadAnalogInputScanBit (b_BoardHandle); */
/*   printk("Read Analog Input Bit: %d\n", i_ReturnValue); */

/*   i_ReturnValue = i_APCI3120_ReadAnalogInputScanValue (b_BoardHandle, */
/* 						       ui_AnalogValue); */
							 
/*   printk("Value 0 : %d\n", ui_AnalogValue[0]); */
/*   printk("Value 1 : %d\n", ui_AnalogValue[1]); */
/*   printk("Value 2 : %d\n", ui_AnalogValue[2]); */
/*   printk("Value 3 : %d\n", ui_AnalogValue[3]); */
/*   printk("Value 4 : %d\n", ui_AnalogValue[4]); */
/*   printk("Value 5 : %d\n", ui_AnalogValue[5]); */
/*   printk("Value 6 : %d\n", ui_AnalogValue[6]); */
/*   printk("Value 7 : %d\n", ui_AnalogValue[7]); */
/*   printk("Value 8 : %d\n", ui_AnalogValue[8]); */
/*   printk("Value 9 : %d\n", ui_AnalogValue[9]); */
/*   printk("Value 10: %d\n", ui_AnalogValue[10]); */
/*   printk("Value 11: %d\n", ui_AnalogValue[11]); */
/*   printk("Value 12: %d\n", ui_AnalogValue[12]); */
/*   printk("Value 13: %d\n", ui_AnalogValue[13]); */
/*   printk("Value 14: %d\n", ui_AnalogValue[14]); */
/*   printk("Value 15: %d\n", ui_AnalogValue[15]); */

/*   i_ReturnValue = i_APCI3120_ReadTimer (b_BoardHandle, */
/* 					&l_ReadValue);     */
/*   printk("Timer Value: %lX\n", l_ReadValue); */

/*   i_APCI3120_ResetBoardIntRoutine     (b_BoardHandle); */
    
/*   return 0; */
/* } */

/* /\* */
/*   +----------------------------------------------------------------------------+ */
/*   | Function name   : int sample02 (void)                                      | */
/*   +----------------------------------------------------------------------------+ */
/*   | Task              : Test driver DMA function                               | */
/*   +----------------------------------------------------------------------------+ */
/*   | Input parameters  : -                                                      | */
/*   +----------------------------------------------------------------------------+ */
/*   | Output Parameters : -                                                      | */
/*   +----------------------------------------------------------------------------+ */
/*   | Return Value      :  0                                                     | */
/*   +----------------------------------------------------------------------------+ */
/* *\/ */

/* int sample02(void) */
/* { */
/*   int i = 0, i_InterruptCounter = 0; */
/*   INT i_ReturnValue = 0; */
/*   BYTE b_Channel [17], b_Gain [17], b_Polar [17]; */
/*   BYTE b_BoardHandle = 0; */
/*   BYTE b_AcquisitionCycle = APCI3120_CONTINUOUS; */
/*   BYTE b_SequenzArraySize = 8; */
    
/*   i_ReturnValue = i_APCI3120_SetBoardInterruptRoutine (b_BoardHandle, v_InterruptRoutine); */
  
      
/*   switch (i_ReturnValue) */
/*     { */
/*     case 0: */
/*       printk ("\ni_APCI3120_SetBoardInterruptRoutine OK"); */
/*       break; */
		     
/*     case -1:         	 */
/*       printk ("\ni_APCI3120_SetBoardInterruptRoutine error"); */
/*       printk ("\nError = %d. The handle of the board is wrong", i_ReturnValue); */
/*       break;		      */
		     
/*     case -2:         	 */
/*       printk ("\ni_APCI3120_SetBoardInterruptRoutine error"); */
/*       printk ("\nError = %d. Interrupt routine already installed", i_ReturnValue); */
/*       break;	 */
      
/*     case -3:         	 */
/*       printk ("\ni_APCI3120_SetBoardInterruptRoutine error"); */
/*       printk ("\nError = %d. Interrupt routine can't be installed", i_ReturnValue); */
/*       break;      	 */
		          
/*     default: */
/*       printk ("\ni_APCI3120_SetBoardInterruptRoutine error"); */
/*       printk ("\nError = %d. ", i_ReturnValue); */
/*       break; */
/*     } // switch (i_ReturnValue)	      */


/*   if(i_ReturnValue == 0) */
/*     { */
/*       for (i=0; i<b_SequenzArraySize; i++) */
/*         {  */
/* 	  b_Channel[i] = (APCI3120_CHANNEL_0 + i); */
/* 	  b_Gain[i] = APCI3120_1_GAIN; */
/* 	  b_Polar[i] = APCI3120_UNIPOLAR; */
/*         }        */

/*       i_ReturnValue = i_APCI3120_InitAnalogInputAcquisition (b_BoardHandle, */
/* 							     b_SequenzArraySize, //b_SequenzArraySize */
/* 							     b_Channel, //pb_ChannelArray */
/* 							     b_Gain,//pb_GainArray */
/* 							     b_Polar,//pb_PolarityArray */
/* 							     APCI3120_DELAY_1_MODUS,//b_AcquisitionMode */
/* 							     APCI3120_DISABLE,//b_ExternTrigger */
/* 							     10,//ui_AcquisitionTiming */
/* 							     100,//l_DelayTiming */
/* 							     10,//ul_NumberOfAcquisition */
/* 							     APCI3120_DMA_USED,//b_DMAMode */
/* 							     b_AcquisitionCycle);//b_AcquisitionCycle */

/*       /\*************************\/ */
/*       /\* Test the return value *\/ */
/*       /\*************************\/ */

/*       switch (i_ReturnValue) */
/* 	{ */
/* 	case 0: */
/* 	  printk ("\ni_APCI3120_InitAnalogInputAcquisition OK"); */
/* 	  break; */

/* 	case -1: */
/* 	  printk ("\ni_APCI3120_InitAnalogInputAcquisition error"); */
/* 	  printk ("\nError = %d. The handle of the board is wrong", i_ReturnValue); */
/* 	  break; */

/* 	case -2: */
/* 	  printk ("\ni_APCI3120_InitAnalogInputAcquisition error"); */
/* 	  printk ("\nError = %d. User interrupt Routine has not been installed", i_ReturnValue); */
/* 	  break; */

/* 	case -3: */
/* 	  printk ("\ni_APCI3120_InitAnalogInputAcquisition error"); */
/* 	  printk ("\nError = %d. Size of scan list is wrong", i_ReturnValue); */
/* 	  break; */

/* 	case -4: */
/* 	  printk ("\ni_APCI3120_InitAnalogInputAcquisition error"); */
/* 	  printk ("\nError = %d. Wrong parameter detected in table pb_ChannelArray", i_ReturnValue); */
/* 	  break; */

/* 	case -5: */
/* 	  printk ("\ni_APCI3120_InitAnalogInputAcquisition error"); */
/* 	  printk ("\nError = %d. Wrong parameter detected in table pb_GainArray", i_ReturnValue); */
/* 	  break; */

/* 	case -6: */
/* 	  printk ("\ni_APCI3120_InitAnalogInputAcquisition error"); */
/* 	  printk ("\nError = %d. Wrong parameter detected in table pb_PolarityArray", i_ReturnValue); */
/* 	  break; */

/* 	case -7: */
/* 	  printk ("\ni_APCI3120_InitAnalogInputAcquisition error"); */
/* 	  printk ("\nError = %d. Waiting time between two conversion cycles is too long", i_ReturnValue); */
/* 	  break; */

/* 	case -8: */
/* 	  printk ("\ni_APCI3120_InitAnalogInputAcquisition error"); */
/* 	  printk ("\nError = %d. The selected time for ui_AcquisitionTiming or l_DelayTiming is wrong", i_ReturnValue); */
/* 	  break; */

/* 	case -9: */
/* 	  printk ("\ni_APCI3120_InitAnalogInputAcquisition error"); */
/* 	  printk ("\nError = %d. Parameter b_DMAUsed is wrong", i_ReturnValue); */
/* 	  break; */

/* 	case -10: */
/* 	  printk ("\ni_APCI3120_InitAnalogInputAcquisition error"); */
/* 	  printk ("\nError = %d. Parameter running time of the DMA conversion cycles is wrong ", i_ReturnValue); */
/* 	  printk ("\n (APCI3120_CONTINUOUS or APCI3120_SINGLE)"); */
/* 	  break; */

/* 	case -11: */
/* 	  printk ("\ni_APCI3120_InitAnalogInputAcquisition error"); */
/* 	  printk ("\nError = %d. Parameter conversion cycle is wrong (APCI3120_SIMPLE_MODUS or APCI3120_DELAY_MODUS)", i_ReturnValue); */
/* 	  break; */

/* 	case -12: */
/* 	  printk ("\ni_APCI3120_InitAnalogInputAcquisition error"); */
/* 	  printk ("\nError = %d. Not enough memory available", i_ReturnValue); */
/* 	  break; */

/* 	case -13: */
/* 	  printk ("\ni_APCI3120_InitAnalogInputAcquisition error"); */
/* 	  printk ("\nError = %d. Parameter b_ExternTrigger is wrong (APCI3120_ENABLE or APCI3120_DISABLE)", i_ReturnValue); */
/* 	  break; */

/* 	case -14: */
/* 	  printk ("\ni_APCI3120_InitAnalogInputAcquisition error"); */
/* 	  printk ("\nError = %d. Parameter ul_NumberOfAcquisition is wrong", i_ReturnValue); */
/* 	  break; */

/* 	default: */
/* 	  printk ("\ni_APCI3120_InitAnalogInputAcquisition error"); */
/* 	  printk ("\nError = %d. ", i_ReturnValue); */
/* 	  break; */
/* 	} // switch (i_ReturnValue) */

/*       if(i_ReturnValue == 0) */
/* 	{ */
/* 	  APCI3120_InterruptValue.ReceiveInterrupt = 0; */

/* 	  i_ReturnValue = i_APCI3120_StartAnalogInputAcquisition (b_BoardHandle); */

/* 	  /\*************************\/ */
/* 	  /\* Test the return value *\/ */
/* 	  /\*************************\/ */

/* 	  switch (i_ReturnValue) */
/* 	    { */
/* 	    case 0: */
/* 	      printk ("\ni_APCI3120_StartAnalogInputAcquisition OK"); */
/* 	      break; */

/* 	    case -1: */
/* 	      printk ("\ni_APCI3120_StartAnalogInputAcquisition error"); */
/* 	      printk ("\nError = %d. The handle of the board is wrong", i_ReturnValue); */
/* 	      break; */

/* 	    case -2: */
/* 	      printk ("\ni_APCI3120_StartAnalogInputAcquisition error"); */
/* 	      printk ("\nError = %d. Cyclyc acquisition has not benn initialised", i_ReturnValue); */
/* 	      break; */

/* 	    case -3: */
/* 	      printk ("\ni_APCI3120_StartAnalogInputAcquisition error"); */
/* 	      printk ("\nError = %d. User interrupt Routine has not been installed", i_ReturnValue); */
/* 	      break; */

/* 	    default: */
/* 	      printk ("\ni_APCI3120_StartAnalogInputAcquisition error"); */
/* 	      printk ("\nError = %d. ", i_ReturnValue); */
/* 	      break; */
/* 	    } // switch (i_ReturnValue) */
/* 	} */
/*     } */
/* /\*	  if(i_ReturnValue == 0) */
/* 	    { */
/* 	      i=0; */

/* 	      do */
/* 		{ */
/* 		  while (APCI3120_InterruptValue.ReceiveInterrupt == 0) */
/* 		    { */
/* 		      i++; */
			
/* 		      if ((i % 100000) == 0) */
/* 			{ */
/* 			  printk ("\nAPCI3120_InterruptValue.ReceiveInterrupt = %d", APCI3120_InterruptValue.ReceiveInterrupt); */
/* 			  i=0; */
/* 			} */
/* 		    } */
		
/* 		  if (APCI3120_InterruptValue.ReceiveInterrupt != 0)  */
/* 		    { */
/* 		      printk("\nInterrupt %d", i_InterruptCounter); */
		  	
/* 		      for (i=0; i<16; i++)		     */
/* 			printk("\nAcquisition : %u", APCI3120_InterruptValue.ui_SaveArray [i]); */
		    
/* 		      i_InterruptCounter++;  */
/* 		      APCI3120_InterruptValue.ReceiveInterrupt = 0;  */
/* 		    }     */
/* 		} */
/*               while ((i_InterruptCounter < 5) && (b_AcquisitionCycle == APCI3120_CONTINUOUS)); */
              		                    
/* 	      i_ReturnValue = i_APCI3120_StopAnalogInputAcquisition(b_BoardHandle); */
/* *\/ */
/* 	      /\*************************\/ */
/* 	      /\* Test the return value *\/ */
/* 	      /\*************************\/ */
/* /\* */
/* 	      switch (i_ReturnValue) */
/* 		{ */
/* 		case 0: */
/* 		  printk ("\ni_APCI3120_StopAnalogInputAcquisition OK"); */
/* 		  break; */

/* 		case -1: */
/* 		  printk ("\ni_APCI3120_StopAnalogInputAcquisition error"); */
/* 		  printk ("\nError = %d. The handle of the board is wrong", i_ReturnValue); */
/* 		  break; */

/* 		case -2: */
/* 		  printk ("\ni_APCI3120_StopAnalogInputAcquisition error"); */
/* 		  printk ("\nError = %d. Cyclyc acquisition has not benn started", i_ReturnValue); */
/* 		  break; */

/* 		default: */
/* 		  printk ("\ni_APCI3120_StopAnalogInputAcquisition error"); */
/* 		  printk ("\nError = %d. ", i_ReturnValue); */
/* 		  break; */
/* 		} // switch (i_ReturnValue) */
/* 	    } */
/* 	  i_ReturnValue = i_APCI3120_ClearAnalogInputAcquisition(b_BoardHandle); */
/* *\/ */
/* 	  /\*************************\/ */
/* 	  /\* Test the return value *\/ */
/* 	  /\*************************\/ */
/* /\* */
/* 	  switch (i_ReturnValue) */
/* 	    { */
/* 	    case 0: */
/* 	      printk ("\ni_APCI3120_ClearAnalogInputAcquisition OK"); */
/* 	      break; */

/* 	    case -1: */
/* 	      printk ("\ni_APCI3120_ClearAnalogInputAcquisition error"); */
/* 	      printk ("\nError = %d. The handle of the board is wrong", i_ReturnValue); */
/* 	      break; */

/* 	    case -2: */
/* 	      printk ("\ni_APCI3120_ClearAnalogInputAcquisition error"); */
/* 	      printk ("\nError = %d. Cyclyc acquisition has not benn initialised", i_ReturnValue); */
/* 	      break; */

/* 	    default: */
/* 	      printk ("\ni_APCI3120_ClearAnalogInputAcquisition error"); */
/* 	      printk ("\nError = %d. ", i_ReturnValue); */
/* 	      break; */
/* 	    } // switch (i_ReturnValue) */
/* 	} */
/*     } */
    
/*   i_APCI3120_ResetBoardIntRoutine     (b_BoardHandle); */
/* *\/   */
/*   return 0; */
/* } */

/*
  +----------------------------------------------------------------------------+
  | Function name   : int i_APCI3120_DevInit (void)                              |
  +----------------------------------------------------------------------------+
  | Task              : Search all APCI-3120                                   |
  +----------------------------------------------------------------------------+
  | Input parameters  : -                                                      |
  +----------------------------------------------------------------------------+
  | Output Parameters : -                                                      |
  +----------------------------------------------------------------------------+
  | Return Value      :  Number of boards                                      |
  |                      -1 : No board found                                   |
  +----------------------------------------------------------------------------+
*/

int i_APCI3120_DevInit(void) 
{
  int      		i;
  xPCI3120_DEV   *pDev = xPci3120_data;
  struct pci_dev *pPCILinuxStruct         = NULL;

  printk("\n +------------------------------+\n");
  printk(" | ADDI-DATA GmbH -+- APCI-3120 |\n");
  printk(" +------------------------------+\n");
  printk(" |      Module installation     |\n");
  printk(" +------------------------------+\n");
	
  xPci3120_nrDev = 0;

  if(!pci_present())
    {
      printk("<1> NO PCI BUS;\n");
    }      
  else
    {
      //Filling str_PCI_Information stucture
      pPCILinuxStruct = pci_find_device (xPCI3120_VENDOR_ID, 
					 xPCI3120_DEVICE_ID, 
					 pPCILinuxStruct);
					                            
      while (pPCILinuxStruct)
	{
		
	  /* Found a PCI-3120 board, get the relevant information from it */
	  pDev = &xPci3120_data[xPci3120_nrDev];	/* set up a pointer to the relevant info-structure */

	  /* Initialize the device header */
	  memset(pDev, 0, sizeof(xPCI3120_DEV));
	   		
	  for (i = 0; i < 5; i++)
	    {          
	      // Get board address
	      pDev->addr[i] = PCI_BASE_ADDRESS_IO_MASK & pPCILinuxStruct->resource[i].start;

	    }
          
	  // Get board interrupt 
	  pDev->intrpt = pPCILinuxStruct->irq;
	  pDev->interruptInitialized = APCI3120_DISABLE;

	  pDev->pciBus = PCI_SLOT(pPCILinuxStruct->devfn);
	  pDev->pciDev = PCI_SLOT(pPCILinuxStruct->devfn);
	  pDev->pciFunc = PCI_SLOT(pPCILinuxStruct->devfn);
	    
	    
	  pDev->nrAnaIn = 16;
	  pDev->nrAnaOut = 8;
	  
	  pDev->created = TRUE;	    

	  xPci3120_nrDev++;
	  pDev->BoardHandle = xPci3120_nrDev;

	  printk(" |      NumberOfBoard: %2d       |\n",xPci3120_nrDev);
	  printk(" +------------------------------+\n");
	  printk(" | Baseaddress[0,1,2]:          |\n");
	  printk(" |      %4X, %4X, %4X        |\n", pDev->addr[0], pDev->addr[1], pDev->addr[2]);   
	  printk(" | Interrupt No.:               |\n");
	  printk(" |      %2d                      |\n",pDev->intrpt);   
	  printk(" +------12.07.2002---15:30------+\n");
		
	  pPCILinuxStruct = pci_find_device (xPCI3120_VENDOR_ID, 
					     xPCI3120_DEVICE_ID, 
					     pPCILinuxStruct);      
	}
    }
   
  if (xPci3120_nrDev == 0) 
    {
      /* No module found */
      return -1;
    }
            
  return xPci3120_nrDev;
}

/*
  +----------------------------------------------------------------------------+
  | Function name   : int i_APCI3120_SearchAllAPCI3120 (void)                  |
  +----------------------------------------------------------------------------+
  | Task              : Search all APCI-3120                                   |
  +----------------------------------------------------------------------------+
  | Input parameters  : -                                                      |
  +----------------------------------------------------------------------------+
  | Output Parameters : -                                                      |
  +----------------------------------------------------------------------------+
  | Return Value      :  Number of boards                                      |
  |                      -1 : No board found                                   |
  +----------------------------------------------------------------------------+
*/

INT i_APCI3120_SearchAllAPCI3120 (VOID) 
{
    return i_APCI3120_DevInit();
}


/*
  +----------------------------------------------------------------------------+
  | Function name   : int init_module (void)                                   |
  +----------------------------------------------------------------------------+
  | Task              : First function called by the module.                   |
  +----------------------------------------------------------------------------+
  | Input parameters  : -                                                      |
  +----------------------------------------------------------------------------+
  | Output Parameters : -                                                      |
  +----------------------------------------------------------------------------+
  | Return Value      : -                                                      |
  +----------------------------------------------------------------------------+
*/
/*
int init_module(void)
{
  // Search all APCI-3120 in the computer
  if (i_APCI3120_DevInit() > 0)
    {
      // Test the DMA Acquisition 
	   sample02 ();

      // Test other functions  
	// sample01 ();     	        
    }     
    
  return 0;
}
*/
/*
  +----------------------------------------------------------------------------+
  | Function name   : int cleanup_module (void)                                |
  +----------------------------------------------------------------------------+
  | Task              : Last function called by the module.                    |
  +----------------------------------------------------------------------------+
  | Input parameters  : -                                                      |
  +----------------------------------------------------------------------------+
  | Output Parameters : -                                                      |
  +----------------------------------------------------------------------------+
  | Return Value      : -                                                      |
  +----------------------------------------------------------------------------+
*/
/*
void cleanup_module (void)
{
  printk ("\ncleanup_module");
  i_APCI3120_StopAnalogInputAcquisition(0);
  i_APCI3120_ClearAnalogInputAcquisition(0);
  i_APCI3120_ResetBoardIntRoutine     (0);

}
*/
