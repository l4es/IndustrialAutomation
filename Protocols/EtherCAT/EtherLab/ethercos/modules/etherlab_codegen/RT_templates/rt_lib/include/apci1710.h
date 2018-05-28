/*
  +----------------------------------------------------------------------------+
  | (C) ADDI-DATA GmbH          Dieselstrasse 3      D-77833 Ottersweier       |
  +----------------------------------------------------------------------------+
  | Tel : +49 (0) 7223/9493-0     | email    : info@addi-data.com              |
  | Fax : +49 (0) 7223/9493-92    | Internet : http://www.addi-data.com        |
  +----------------------------------------------------------------------------+
  | Project   : APCI1710          | Compiler : GCC                             |
  | Modulname : rt_apci1710.h     | Version  : 2.96 (SuSE 7.1)                 |
  +-------------------------------+--------------------------------------------+
  | Author    : Stolz Eric        | Date     : 01.10.2001                      |
  +----------------------------------------------------------------------------+
  | Description : APCI1710 header	                                       |
  +----------------------------------------------------------------------------+
  |                             UPDATE'S                                       |
  +----------+-----------+--------------------------------------------+--------+
  |   Date   |   Author  |          Description of updates            |Version |
  +----------+-----------+--------------------------------------------+--------+
  | 21.06.04 | J. Krauth | - Append module configuration recognition. |        | 
  |          |           | - Append PWM functions.                    |01.00.01|
  |          |           | - Append i_APCI1710_Read16BitCounterValue  |        |
  |          |           |          i_APCI1710_Write16BitCounterValue |        |
  |          |           |          i_APCI1710_Write32BitCounterValue |        |
  +----------+-----------+--------------------------------------------+--------+  
  | 28.06.04 | J. Krauth | - Append function prototypes and change    |        |
  |          |           |  structure names to be used in combination |01.00.02|
  |          |           |   with other drivers.                      |        |
  +----------+-----------+--------------------------------------------+--------+ 
*/

/*******************************/
/* Include for ADDI-DATA types */
/*******************************/
#ifndef __ADDIDATA_TYPE
#include "addidata_type.h"
#define __ADDIDATA_TYPE
#endif

/*********************/
/* APCI-1710 defines */
/*********************/

#ifndef __APCI1710_DEFINES

#define __APCI1710_DEFINES   

#define APCI1710_MAX_BOARD_NBR       10

#define APCI1710_BOARD_NAME          "APCI1710"
#define APCI1710_BOARD_VENDOR_ID     0x10E8
#define APCI1710_BOARD_DEVICE_ID     0x818F

#define APCI1710_INCREMENTAL_COUNTER 0x53430000UL
#define APCI1710_SSI_COUNTER         0x53490000UL
#define APCI1710_TTL_IO              0x544C0000UL
#define APCI1710_DIGITAL_IO          0x44490000UL
#define APCI1710_82X54_TIMER         0x49430000UL
#define APCI1710_CHRONOMETER         0x43480000UL
#define APCI1710_PULSE_ENCODER       0x495A0000UL
#define APCI1710_TOR_COUNTER         0x544F0000UL
#define APCI1710_PWM                 0x50570000UL
#define APCI1710_ETM                 0x45540000UL
#define APCI1710_CDA		     0x43440000UL

#define APCI1710_30MHZ		30
#define APCI1710_33MHZ		33
#define APCI1710_40MHZ		40

#define APCI1710_GRAY_MODE	0x0
#define APCI1710_BINARY_MODE	0x1

#define APCI1710_16BIT_COUNTER	0x10
#define APCI1710_32BIT_COUNTER	0x0
#define APCI1710_QUADRUPLE_MODE	0x0
#define APCI1710_DOUBLE_MODE	0x3
#define APCI1710_SIMPLE_MODE	0xF
#define APCI1710_DIRECT_MODE	0x80
#define APCI1710_HYSTERESIS_ON	0x60
#define APCI1710_HYSTERESIS_OFF	0x0
#define APCI1710_INCREMENT	0x60
#define APCI1710_DECREMENT	0x0
#define APCI1710_LATCH_COUNTER	0x1
#define APCI1710_CLEAR_COUNTER	0x0
#define APCI1710_LOW		0x0
#define APCI1710_HIGH		0x1
   
#define APCI1710_DISABLE	0
#define APCI1710_ENABLE		1

#define APCI1710_SINGLE		0
#define APCI1710_CONTINUOUS	1
   

   
/************************/
/*  PCI BUS board infos */
/************************/

typedef struct
{
  UINT	ui_BaseAddress[5];
  BYTE 	b_Interrupt;
  BYTE	b_SlotNumber;
}str_APCI1710_BaseInformation;	

/**************************/
/* Digital IO board infos */
/**************************/

typedef struct
{
  BYTE b_DigitalInit;
  BYTE b_ChannelAMode;
  BYTE b_ChannelBMode;
  BYTE b_DigitalOutputMemory;
  BYTE b_DigitalOutputStatus;
}str_APCI1710_DigitalIOInformation;	

/******************/
/* Register infos */
/******************/

typedef struct
{
  BYTE b_ModeRegister1;
  BYTE b_ModeRegister2;
  BYTE b_ModeRegister3;
  BYTE b_ModeRegister4;
}str_APCI1710_ByteModeRegister;

typedef union
{
  str_APCI1710_ByteModeRegister s_ByteModeRegister;      
  DWORD dw_ModeRegister1_2_3_4;
   
}str_APCI1710_ModeRegister;

/*****************************/
/* Incremental counter infos */
/*****************************/

typedef struct
{
  str_APCI1710_ModeRegister s_ModeRegister;
  BYTE b_CounterInit;
}str_APCI1710_Inc_CptInformation;	

/*************/
/* SSI infos */
/*************/

typedef struct
{
  BYTE b_SSIInit;
  BYTE b_SSIProfile;
  BYTE b_PositionTurnLength;
  BYTE b_TurnCptLength;
}str_APCI1710_SSIInformation;	

// BEGIN JK 08.09.03
/*********************/
/* Chronometer infos */
/*********************/

typedef struct
{
  BYTE    b_ChronoInit;
  BYTE    b_InterruptMask;
  BYTE    b_PCIInputClock;
  BYTE    b_TimingUnit;
  BYTE    b_CycleMode;
  double  d_TimingInterval;
  DWORD  dw_ConfigReg;
}str_APCI1710_ChronoModuleInformation;
// END JK 08.09.03

/*************/
/* PWM infos */
/*************/

typedef struct
{
  struct
  {
    BYTE    b_PWMInit;
    BYTE    b_TimingUnit;
    BYTE    b_InterruptEnable;
    ULONG  d_LowTiming;
    ULONG  d_HighTiming;
    ULONG  ul_RealLowTiming;
    ULONG  ul_RealHighTiming;
  }s_PWMInfo [2];
 
  BYTE b_ClockSelection;
}str_APCI1710_PWMModuleInfo;

typedef struct
{
  str_APCI1710_PWMModuleInfo s_PWMModuleInfo;	
  str_APCI1710_Inc_CptInformation    s_Inc_CptInformation;        /* Incremental encoder information */
  str_APCI1710_DigitalIOInformation  s_DigitalIOInformation;      /* Digital I/O information */
  str_APCI1710_SSIInformation        s_SSIInformation;            /* SSI information */
  // BEGIN JK 08.09.03
  str_APCI1710_ChronoModuleInformation s_ChronoModuleInformation; /* Chronometer information */
  // END JK 08.09.03   
}str_APCI1710_ModuleInformation;

/************************/
/* Hardware board infos */
/************************/
typedef struct
{
  str_APCI1710_BaseInformation       s_BaseInformation;           /* PCI BUS INFORMATIONS */
  str_APCI1710_ModuleInformation     s_ModuleInformation[4];      /* Module information */
  BYTE   b_BoardVersion;
  BYTE   b_InterruptInitialized;
  DWORD dw_MolduleConfiguration [4];
}
  str_APCI1710_BoardInformation;

typedef struct
{
  str_APCI1710_BoardInformation s_BoardInformation[APCI1710_MAX_BOARD_NBR];
  BYTE                 b_NumberOfBoard;
}
  str_APCI1710_DriverStruct;   


// BEGIN JK 09.09.03 : Implementation from CHRONO functionality

/***********************/
/* Function prototypes */
/***********************/

/*
+----------------------------------------------------------------------------+
| Function Name     : _INT_     i_APCI1710_InitChrono                        |
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
			     PULONG pul_RealTimingInterval);
			     
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
			       BYTE b_InterruptEnable);
			       
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
                                BYTE b_ModuleNumber);

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
					  PBYTE  pb_ChronoStatus);

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
                                  PULONG pul_ChronoValue);

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
				       PUINT  pui_NanoSecond);

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
					 BYTE  b_OutputChannel);

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
					 BYTE  b_OutputChannel);

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
					 PBYTE pb_ChannelStatus);

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
					 PBYTE pb_PortValue);

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
					       UINT ui_WriteValue);

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
					       ULONG ul_WriteValue);

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

VOID v_APCI1710_ReadAllModuleConfiguration (BYTE b_BoardHandle);

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
				BYTE           b_SecondCounterOption);

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
					 BYTE  b_ModuleNumber);

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
					       PUINT pui_CounterValue);

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
						 PULONG  pul_CounterValue);

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
                              BYTE b_ChannelBMode);

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
				   PBYTE pb_ChannelStatus);

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
                                    PBYTE pb_PortValue);

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
                                  BYTE b_ModuleNumber);

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
                                   BYTE b_ModuleNumber);

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
                                  BYTE b_OutputChannel);

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
                                    BYTE b_PortValue);

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
			BYTE   b_SSICountingMode);

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
			      PULONG pul_TurnCpt);


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
				BYTE     b_ModuleNumber);

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
				PULONG pul_TurnCpt);

INT i_APCI1710_ReadAllSSIValueRaw (BYTE     b_BoardHandle,  //HM 2006.09.19
				BYTE     b_ModuleNumber,
				   PULONG pul_Position);


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
				      PBYTE  pb_ChannelStatus);

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
				        PBYTE pb_InputStatus);

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
				     BYTE b_OutputValue);


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
                        PULONG pul_RealHighTiming) ;

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
                                ULONG   ul_HighTiming);

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
			       BYTE  b_InterruptEnable);

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
			       BYTE  b_PWM);

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
				       PBYTE   pb_Enable);

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
			       PBYTE  pb_ExternGateStatus);

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

INT i_APCI1710_SearchAllAPCI1710 (VOID);
                                
                                			       			     
#endif
