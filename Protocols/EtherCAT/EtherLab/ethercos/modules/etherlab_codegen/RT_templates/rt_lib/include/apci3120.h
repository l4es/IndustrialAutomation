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
  |               Some defines and structure for the APCI-3120            |
  +-----------------------------------------------------------------------+
  |                             UPDATES                                   |
  +----------+-----------+------------------------------------+-----------+
  |   Date   |   Author  |  Description of updates            | Version   |
  +----------+-----------+------------------------------------+-----------+
  | 18.06.04 | J. Krauth |  Creation                          | 01.00.00  |
  +----------*-----------*------------------------------------*-----------+
  */

/*********************/
/* APCI-3120 defines */
/*********************/

#ifndef __APCI3120_DEFINES

#define __APCI3120_DEFINES   


/*******************************/
/* Include for ADDI-DATA types */
/*******************************/
#ifndef __ADDIDATA_TYPE
   #include "addidata_type.h"
   #define __ADDIDATA_TYPE
#endif
  
enum {APCI3120_CHANNEL_0,
      APCI3120_CHANNEL_1,
      APCI3120_CHANNEL_2,
      APCI3120_CHANNEL_3,
      APCI3120_CHANNEL_4,
      APCI3120_CHANNEL_5,
      APCI3120_CHANNEL_6,
      APCI3120_CHANNEL_7,
      APCI3120_CHANNEL_8,
      APCI3120_CHANNEL_9,
      APCI3120_CHANNEL_10,
      APCI3120_CHANNEL_11,
      APCI3120_CHANNEL_12,
      APCI3120_CHANNEL_13,
      APCI3120_CHANNEL_14,
      APCI3120_CHANNEL_15};


#define APCI3120_1_GAIN                           0x00
#define APCI3120_10_GAIN                          0x30
#define APCI3120_2_GAIN                           0x10
#define APCI3120_5_GAIN                           0x20
#define APCI3120_BIPOLAR                          0x00

#define APCI3120_CLEAR_PA                         0xFFF0U
#define APCI3120_CLEAR_PR                         0xF0FFU
#define APCI3120_CLEAR_PA_PR                      (APCI3120_CLEAR_PR & APCI3120_CLEAR_PA)

#define APCI3120_DigitalInput			  0x2
#define APCI3120_DigitalOutput			  0x0D
#define APCI3120_DISABLE                         0
#define APCI3120_ENABLE                          1
#define APCI3120_ENABLE_SCAN                      0x08
#define APCI3120_EOC                              0x8000U
#define APCI3120_EOS                              0x2000U
#define APCI3120_FIFO_ADDRESS                     0x06
#define APCI3120_RD_STATUS                        0x02
#define APCI3120_RESET_FIFO                       0x0C
#define APCI3120_SELECT_TIMER_0_WORD              0x00
#define APCI3120_START_CONVERSION                 0x02
#define APCI3120_TIMER_0_MODE_2                   0x01
#define APCI3120_TIMER_0_MODE_5                   0x03
#define APCI3120_TIMER_1_MODE_2                  0x4
#define APCI3120_TIMER_2_MODE_0                  0x0
#define APCI3120_TIMER_2_MODE_5               0x30
#define APCI3120_TIMER_CRT0                       0x0D
#define APCI3120_TIMER_CRT1                       0x0C
#define APCI3120_TIMER_VALUE                      0x04 
#define APCI3120_UNIPOLAR                         0x80
#define APCI3120_WR_ADDRESS                       0x00
#define APCI3120_WRITE_MODE_SELECT                0x0E
#define APCI3120_SELECT_TIMER_2_LOW_WORD         0x02
#define APCI3120_SELECT_TIMER_2_HIGH_WORD        0x03
#define APCI3120_ENABLE_TIMER0                   0x1000U
#define APCI3120_ENABLE_TIMER1                   0x2000U
#define APCI3120_ENABLE_TIMER2                   0x4000U
#define APCI3120_DISABLE_TIMER0                  (~APCI3120_ENABLE_TIMER0)
#define APCI3120_DISABLE_TIMER1                  (~APCI3120_ENABLE_TIMER1)
#define APCI3120_DISABLE_TIMER2                  (~APCI3120_ENABLE_TIMER2)
#define APCI3120_DISABLE_ALL_TIMER               (APCI3120_DISABLE_TIMER0 & APCI3120_DISABLE_TIMER1 & APCI3120_DISABLE_TIMER2)
#define APCI3120_SELECT_TIMER_1_WORD             0x01
#define APCI3120_TIMER2_SELECT_EOS               0xC0
#define APCI3120_TIMER					1
#define APCI3120_TIMER_2_MODE_2		0x10
#define APCI3120_ENABLE_TIMER_INT		4
#define APCI3120_DISABLE_TIMER_INT  (~APCI3120_ENABLE_TIMER_INT)
#define APCI3120_WATCHDOG                  2
#define APCI3120_ENABLE_WATCHDOG         0x20
#define APCI3120_DISABLE_WATCHDOG        (~APCI3120_ENABLE_WATCHDOG)
#define APCI3120_TIMER_STATUS_REGISTER 0xd
#define APCI3120_ENABLE_TIMER_COUNTER 	0x10
#define APCI3120_DISABLE_TIMER_COUNTER 	(~APCI3120_ENABLE_TIMER_COUNTER)
#define APCI3120_FC_TIMER				0x1000

#define APCI3120_COUNTER                   0
#define APCI3120_DISABLE_SCAN            (~APCI3120_ENABLE_SCAN)
#define APCI3120_ENABLE_EOS_INT          0x2
#define APCI3120_DISABLE_EOS_INT         (~APCI3120_ENABLE_EOS_INT)
#define APCI3120_ENABLE_EOC_INT          0x1
#define APCI3120_DISABLE_EOC_INT         (~APCI3120_ENABLE_EOC_INT)
#define APCI3120_DISABLE_ALL_INTERRUPT_WITHOUT_TIMER   (APCI3120_DISABLE_EOS_INT & APCI3120_DISABLE_EOC_INT)
#define APCI3120_DISABLE_ALL_INTERRUPT   (APCI3120_DISABLE_TIMER_INT & APCI3120_DISABLE_EOS_INT & APCI3120_DISABLE_EOC_INT)
   
#define APCI3120_MAX_INPUT_CHANNEL   16
#define APCI3120_ENABLE_TRANSFER_ADD_ON_LOW       0x00
#define APCI3120_ENABLE_TRANSFER_ADD_ON_HIGH      0x1200
#define APCI3120_A2P_FIFO_MANAGEMENT              0x04000400L
#define APCI3120_AMWEN_ENABLE                     0x02
#define APCI3120_A2P_FIFO_WRITE_ENABLE            0x01
#define APCI3120_FIFO_ADVANCE_ON_BYTE_2           0x20000000L
#define APCI3120_ENABLE_WRITE_TC_INT              0x00004000L
#define APCI3120_CLEAR_WRITE_TC_INT               0x00040000L
#define APCI3120_DISABLE_AMWEN_AND_A2P_FIFO_WRITE 0x0
#define APCI3120_DISABLE_BUS_MASTER_ADD_ON        0x0
#define APCI3120_DISABLE_BUS_MASTER_PCI           0x0
#define APCI3120_ADD_ON_MWAR_LOW         0x24
#define APCI3120_ADD_ON_MWAR_HIGH        (APCI3120_ADD_ON_MWAR_LOW + 2)
#define APCI3120_ADD_ON_MWTC_LOW         0x058
#define APCI3120_ADD_ON_MWTC_HIGH        (APCI3120_ADD_ON_MWTC_LOW + 2)
#define APCI3120_ENABLE_EXT_TRIGGER              0x8000U
#define APCI3120_DISABLE_EXT_TRIGGER             (~APCI3120_ENABLE_EXT_TRIGGER)
#define APCI3120_SIMPLE_MODUS         0
#define APCI3120_DELAY_MODUS          1
#define APCI3120_DELAY_1_MODUS        2
#define APCI3120_SINGLE               0
#define APCI3120_CONTINUOUS           1
#define APCI3120_MAX_DMA_LENG         32767U
#define APCI3120_OUTPUT_MAX_VALUE     4095
#define APCI3120_EOC_INT_MODE         1
#define APCI3120_EOS_INT_MODE         2
#define APCI3120_DMA_INT_MODE         3
#define APCI3120_DMA_USED           1
#define APCI3120_DMA_NOT_USED       0
#define APCI3120_AMCC_OP_REG_INTCSR      0x38
#define APCI3120_ADD_ON_AGCSTS_LOW       0x3C
#define APCI3120_ADD_ON_AGCSTS_HIGH      (APCI3120_ADD_ON_AGCSTS_LOW + 2)
#define APCI3120_AMCC_OP_MCSR            0x3C



/***********************/
/* Supported Functions */
/***********************/

#define PCI_FUNCTION_ID         0xb1
#define PCI_BIOS_PRESENT        0x01
#define FIND_PCI_DEVICE         0x02
#define FIND_PCI_CLASS_CODE     0x03
#define GENERATE_SPECIAL_CYC    0x06
#define READ_CONFIG_BYTE        0x08
#define READ_CONFIG_WORD        0x09
#define READ_CONFIG_DWORD       0x0a
#define WRITE_CONFIG_BYTE       0x0b
#define WRITE_CONFIG_WORD       0x0c
#define WRITE_CONFIG_DWORD      0x0d
#define GET_IRQ_ROUTING_OPTIONS 0x0e
#define SET_PCI_HW_INT          0x0f

#define TRUE  1
#define FALSE 0
#define OK    0

/*---------------------------------------------------------------------------
** DEFINES
**-------------------------------------------------------------------------*/



/* This driver is restricted to 20 xPCI3120 modules. */
#define MAX_NO_3120 20

#define xPCI3120_VENDOR_ID 0x10E8
#define xPCI3120_DEVICE_ID 0x818D


/* Definition of the status codes */
#define xPCI3120_OK		0x0000
#define xPCI3120_NO_INT 0x8001


/*---------------------------------------------------------------------------
** TYPEDEFS
**-------------------------------------------------------------------------*/

/* Description of the user interupt routine: */

void (*usrHdl) (uint8 handle, uint8 intMask, uint16 *aiv);

/*
with handle: board handle
intMask: interrupt mask;
aiv:	  pointer to the analog input values
*/

typedef struct /*_DMA_INFO */
{
  uint8		dmaUsed;
  uint8		dmaSelect;
  uint8		acqCycle;
  uint8		acqMode;
  uint8		acqEnable;
  uint32	nrAcq;
  uint8		seqArraySize;
  uint16	seqArray[16];
  uint32	*dmaBuffer;			/* Pointer to the DMA memory area */
  uint32	*dmaBufferHw;
  uint32	*oldDmaBuffer;	/* Pointer to the DMA memory area */
  uint32	*oldDmaBufferHw;
  uint32        dmaBufferSize;
  uint32        dmaBufferPages;  
  uint32        oldDmaBufferSize;
  uint32        oldDmaBufferPages; 
  uint8		bufIndex;
  uint8		extTrigger;
  uint16	t0interval;
  uint16	t1interval;
} DMA_INFO;


/* Device header */

typedef struct /*_xPCI3120_DEV*/
{
  //	DEV_HDR        devHdr;
  BOOL           found;	/* indicates whether a module has been initialized */
  BOOL           created;	/* true if this device has been created */
  int            pciBus;
  int            pciDev;
  int            pciFunc;
  uint16         addr[5];
  uint8          intrpt;	/* holds the PCI interrupt # */
  uint8          nrAnaIn;
  uint8          nrAnaOut;
  uint8          outputPolarity[16];
  uint8          digOutReg;
  uint8          crt0;				/* saved HW register */
  uint8          crt1;				/* saved HW register */
  uint8          modeSelect;			/* saved HW register: APCI3120_WRITE_MODE_SELECT */
  uint16         nWrAddr;				/* saved HW register: APCI3120_WR_ADDRESS */
  BOOL           digOutMem;
  void           (*usrHdl) (uint8 handle, uint8 intMask, uint16 *aiv);	/* user defined interupt routine */
  DMA_INFO       dma;
  uint8          lastChannel;
  uint16         oldArray[17];
  int32          t2Delay;
  uint8          t2Init;
  uint8          t2Started;
  uint8          t2lastInterrupt;
  uint8          t2Interrupt;
  uint8          t2lastMode;
  uint8          timerMode;
  uint8          intMode;
  uint8          oldIntMask;
  uint8          intOccur;
  uint8          interruptInitialized;  
  uint8          BoardHandle;    
} xPCI3120_DEV;


/***********************/
/* Function prototypes */
/***********************/

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
		                UINT   ui_ConvertTiming);
		                
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
	                		                
INT i_APCI3120_StartAnalogInput (BYTE b_BoardHandle);

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

INT i_APCI3120_ReadAnalogInputBit (BYTE b_BoardHandle);

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
				     UINT    ui_ConvertTiming);
				     
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
				     
INT i_APCI3120_StartAnalogInputScan (BYTE b_BoardHandle);

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

INT i_APCI3120_ReadAnalogInputScanBit (BYTE b_BoardHandle);

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

INT i_APCI3120_ReadAnalogInputScanValue (BYTE b_BoardHandle,
                                         PUINT pui_AnalogInputValueArray);	
                                         
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
                                         
INT i_APCI3120_InitAnalogOutput (BYTE b_BoardHandle,
                                 BYTE b_ChannelNbr, 
				 BYTE b_Polarity);
				 
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
				 
INT i_APCI3120_ReadAnalogOutputBit (BYTE b_BoardHandle);

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

INT i_APCI3120_Write1AnalogOutput (BYTE b_BoardHandle,
                                   BYTE b_ChannelNumber, 
				   UINT ui_ValueToWrite);
				   
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
				   
INT i_APCI3120_WriteMoreAnalogOutput (BYTE b_BoardHandle,
                                      UINT *ui_ValueToWriteArray);
                                      
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
                                     PUINT pui_ReadValue);
                                     
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
                                     
INT i_APCI3120_StopAnalogInputAcquisition (BYTE b_BoardHandle);

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

INT i_APCI3120_Read4DigitalInput (BYTE b_BoardHandle,
                                  PBYTE pb_PortValue);
                                  
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
                                  
INT i_APCI3120_SetOutputMemoryOn (BYTE b_BoardHandle);

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

INT i_APCI3120_SetOutputMemoryOff (BYTE b_BoardHandle);

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

INT i_APCI3120_Set1DigitalOutputOn (BYTE b_BoardHandle,
                                    BYTE b_ChannelNumber);
                                    
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

INT i_APCI3120_Set1DigitalOutputOff (BYTE b_BoardHandle,
                                     BYTE b_ChannelNumber);
                                     
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
                                     
INT i_APCI3120_Set4DigitalOutputOn (BYTE b_BoardHandle,
                                    BYTE b_PortValue);
                                    
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
                                    
INT i_APCI3120_Set4DigitalOutputOff (BYTE b_BoardHandle,
                                     BYTE b_PortValue);
                                     
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
                                     
INT i_APCI3120_InitTimerWatchdog (BYTE   b_BoardHandle,
                                  BYTE   b_TimerMode,
                                  LONG   l_DelayTime,
                                  BYTE   b_InterruptFlag);
                                  
/*
  +----------------------------------------------------------------------------+
  | Function's Name   : INT       i_APCI3120_StartTimerWatchdog                |
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
					  
INT i_APCI3120_StartTimerWatchdog (BYTE  b_BoardHandle);

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

INT i_APCI3120_StopTimerWatchdog (BYTE  b_BoardHandle);

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


INT i_APCI3120_ReadTimer (BYTE    b_BoardHandle,
                          PLONG  pl_ReadValue);
                          
/*
+----------------------------------------------------------------------------+
| Function's Name   :  INT    i_APCI3120_ReadWatchdogStatus                  |
|                                       (BYTE    b_BoardHandle,              |
|                                        PBYTE  pb_WatchdogStatus)           |
+----------------------------------------------------------------------------+
| Task              : Read watchdog status .                                 |
+----------------------------------------------------------------------------+
| Input Parameters  : BYTE      b_BoardHandle   : Handle of board APCI 3120  |
+----------------------------------------------------------------------------+
| Onput Parameters  : PBYTE    pb_WatchdogStatus :                           |
|                       0 : Watchdog has not run down                        |
|                       1 : Watchdog has run down .                          |
+----------------------------------------------------------------------------+
| Return Value      :  0: No error.                                          |
|                     -1: The handle parameter of the board is wrong.        |
|                     -2: Timer has not been initialized.                    |
|                     -3: Timer initialized                                  |
+----------------------------------------------------------------------------+
*/                          
                                                           
INT i_APCI3120_ReadWatchdogStatus (BYTE    b_BoardHandle,
                                   PBYTE  pb_watchdogStatus);
                                   
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
				  PBYTE pb_InputValue);

/*
  +----------------------------------------------------------------------------+
  | Function's Name   :  INT  i_APCI3120_InitAnalogInputAcquisition            |
  |                               (BYTE     b_BoardHandle,                     |
  |                                BYTE     b_SequenzArraySize,                |
  |                                PBYTE   pb_ChannelArray,                    |
  |                                PBYTE   pb_GainArray,                       |
  |                                PBYTE   pb_PolarityArray,                   |
  |                                BYTE     b_AcquisitionMode,                 |
  |                                BYTE     b_ExternTrigger,                   |
  |                                UINT    ui_AcquisitionTiming,               |
  |                                LONG     l_DelayTiming,                     |
  |                                ULONG    ul_NumberOfAcquisition,            |
  |                                BYTE     b_DMAMode,                         |
  |                                BYTE     b_AcquisitionCycle)                |
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
				  
INT i_APCI3120_InitAnalogInputAcquisition (BYTE    b_BoardHandle,
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
					   BYTE    b_AcquisitionCycle);

/*
  +----------------------------------------------------------------------------+
  | Function's Name   :  INT  i_APCI3120_StartAnalogInputAcquisition           |
  |                               (BYTE     b_BoardHandle)                     |
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
					     
INT i_APCI3120_StartAnalogInputAcquisition (BYTE b_BoardHandle);

/*
  +----------------------------------------------------------------------------+
  | Function's Name   :  INT  i_APCI3120_ClearAnalogInputAcquisition           |
  |                               (BYTE     b_BoardHandle)                     |
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

INT i_APCI3120_ClearAnalogInputAcquisition (BYTE b_BoardHandle);

/*
  +----------------------------------------------------------------------------+
  | Function name   : INT        i_APCI3120_SetBoardInterruptRoutine           |
  |                                              (BYTE     b_BoardHandle,      |
  |                 VOID (*usrHdl) (BYTE handle, BYTE intMask, USHORT *aiv))   |
  +----------------------------------------------------------------------------+
  | Task              : Initialises the interruptroutine for the APCI-3120     |
  +----------------------------------------------------------------------------+
  | Input parameters  : BYTE   b_BoardHandle :Handle of the APCI-3120          |
  |                     VOID (*usrHdl): User interrupt function                | 
  +----------------------------------------------------------------------------+
  | Output Parameters : ---                                                    |
  +----------------------------------------------------------------------------+
  | Return Value      :  0 : No error                                          |
  |                     -1 : The handle parameter of the board is wrong        |
  |                     -2 : Interrupt routine already installed               |
  |                     -3 : Interrupt routine not installed                   |
  +----------------------------------------------------------------------------+
*/

INT i_APCI3120_SetBoardInterruptRoutine (BYTE    b_BoardHandle,
                                         VOID (*usrHdl) (BYTE handle, BYTE intMask, USHORT *aiv));
                                         
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
                                              
INT i_APCI3120_ResetBoardIntRoutine (BYTE b_BoardHandle);

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

INT i_APCI3120_SearchAllAPCI3120 (VOID);                                            					     				  					                                         					                                                                                                                                                                                                                                                                 				   				                                          			     		                

/* muß beim Interrupt von APCI3120 aufgerufen werden .... */
//int APCI3120_Handle_Interrupt(uint8 b_BoardHandle,uint8* b_InterruptMask, uint16** pui_ValueArray);
int APCI3120_Handle_Interrupt(uint8 b_BoardHandle,uint8* rb_InterruptMask);

/* gibt die Interruptnummer für die Karte zurück */
  
int i_APCI3120_Get_irq(BYTE b_BoardHandle);
uint16 *i_APCI3120_GetDMABufferP (uint8 b_BoardHandle);

INT i_APCI3120_FakeBoardInterruptRoutine (BYTE    b_BoardHandle);



#endif
