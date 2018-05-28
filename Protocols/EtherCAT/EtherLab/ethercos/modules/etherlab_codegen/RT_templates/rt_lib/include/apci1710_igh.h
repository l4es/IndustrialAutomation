/*
  +-----------------------------------------------------------------------+
  | (C) ADDI-DATA GmbH          Dieselstrasse 3      D-77833 Ottersweier  |
  +-----------------------------------------------------------------------+
  | Tel : +49 (0) 7223/9493-0     | email    : info@addi-data.com         |
  | Fax : +49 (0) 7223/9493-92    | Internet : http://www.addi-data.com   |
  +-----------------------------------------------------------------------+
  | Project   : APCI1710          | Compiler : GCC                        |
  | Modulname : rt_apci1710.h     | Version  : 2.96 (SuSE 7.1)            |
  +-------------------------------+---------------------------------------+
  | Author    : Stolz Eric        | Date     : 01.10.2001                 |
  +-----------------------------------------------------------------------+
  | Description : APCI1710 header	                                  |
  +-----------------------------------------------------------------------+
  |                             UPDATE'S                                  |
  +-----------------------------------------------------------------------+
  |   Date   |   Author  |          Description of updates                |
  +----------+-----------+------------------------------------------------+
  |          |           |                                                |
  +-----------------------------------------------------------------------+
*/

/*--Schutz vor mehrfachem includieren------------------------------------------------------------*/

#ifndef _APCI1710_H_  /* hm */
#define _APCI1710_H_

#include "addidata_type.h"

//GENERAL DEFINE
#define APCI1710_MAX_BOARD_NBR                  10

#define APCI1710_BOARD_NAME                     "APCI1710"
#define APCI1710_BOARD_VENDOR_ID                 0x10E8
#define APCI1710_BOARD_DEVICE_ID                 0x818F

#define APCI1710_DIGITAL_IO                      1
#define APCI1710_SSI                             2
#define APCI1710_INC_CPT                         4
// BEGIN JK 08.09.03
#define APCI1710_CHRONOMETER			 5
// END JK 08.09.03

#define APCI1710_MODULE_0                        APCI1710_SSI
#define APCI1710_MODULE_1                        APCI1710_SSI
#define APCI1710_MODULE_2                        APCI1710_INC_CPT
#define APCI1710_MODULE_3                        APCI1710_INC_CPT

#define APCI1710_30MHZ                           30
#define APCI1710_33MHZ                           33
//Begin HM 02.06.2004
#define APCI1710_40MHZ                           40
//End HM 02.06.2004

#define APCI1710_GRAY_MODE                       0x0
#define APCI1710_BINARY_MODE                     0x1

#define APCI1710_16BIT_COUNTER   		 0x10
#define APCI1710_32BIT_COUNTER   		 0x0
#define APCI1710_QUADRUPLE_MODE  		 0x0
#define APCI1710_DOUBLE_MODE     		 0x3
#define APCI1710_SIMPLE_MODE     		 0xF
#define APCI1710_DIRECT_MODE     		 0x80
#define APCI1710_HYSTERESIS_ON   		 0x60
#define APCI1710_HYSTERESIS_OFF  		 0x0
#define APCI1710_INCREMENT       		 0x60
#define APCI1710_DECREMENT       		 0x0
#define APCI1710_LATCH_COUNTER   		 0x1
#define APCI1710_CLEAR_COUNTER   		 0x0
#define APCI1710_LOW             		 0x0
#define APCI1710_HIGH            		 0x1



#define APCI1710_DISABLE                         0
#define APCI1710_ENABLE                          1


#ifndef APCI1710_SINGLE
   #define APCI1710_SINGLE     0
   #define APCI1710_CONTINUOUS 1
#endif

 /************************/
 /*  PCI BUS board infos */
 /************************/

/************************/
 /*  PCI BUS board infos */
 /************************/

typedef struct
{
        UINT    ui_BaseAddress[5];
        BYTE    b_Interrupt;
        BYTE    b_SlotNumber;
}str_BaseInformation;


typedef struct
{
   BYTE b_DigitalInit;
   BYTE b_ChannelAMode;
   BYTE b_ChannelBMode;
   BYTE b_DigitalOutputMemory;
   BYTE b_DigitalOutputStatus;
}str_DigitalIOInformation;	

typedef struct
{
   BYTE b_ModeRegister1;
   BYTE b_ModeRegister2;
   BYTE b_ModeRegister3;
   BYTE b_ModeRegister4;
}str_ByteModeRegister;

typedef union
{
   str_ByteModeRegister s_ByteModeRegister;      
   DWORD dw_ModeRegister1_2_3_4;
   
}str_ModeRegister;

typedef struct
{
   str_ModeRegister s_ModeRegister;
   BYTE b_CounterInit;
}str_Inc_CptInformation;	

typedef struct
{
   BYTE b_SSIInit;
   BYTE b_SSIProfile;
   BYTE b_PositionTurnLength;
   BYTE b_TurnCptLength;
}str_SSIInformation;	

// BEGIN JK 08.09.03
typedef struct
	      {
	      UINT w_BoardHandleArray;

	      #ifndef _WIN32
		 BYTE b_BoardEOIValue;                 /* Interrupt controler end of inter. value */
		 #ifdef __cplusplus                    /* Old interrupt function address          */
		    __VOID__ interrupt (*v_APCI1710_OldFunction) (__CPPARGS);
		 #else
		    //__VOID__ (interrupt *v_APCI1710_OldFunction) (__CPPARGS);
		 #endif
	      // Begin 16/07/01 CG 0701/0232 -> 0701/0233
	      #else
	         HANDLE h_InterruptCallBackHandle;
		 HANDLE h_InterruptIdentifier;
	      // End 16/07/01 CG 0701/0232 -> 0701/0233
	      #endif
	      }str_InterruptInfos;
// END JK 08.09.03

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
	 }str_ChronoModuleInformation;
// END JK 08.09.03

typedef struct
{
   str_Inc_CptInformation    s_Inc_CptInformation;        /* Incremental encoder information */
   str_DigitalIOInformation  s_DigitalIOInformation;      /* Digital I/O information */
   str_SSIInformation        s_SSIInformation;            /* SSI information */
// BEGIN JK 08.09.03
   str_ChronoModuleInformation s_ChronoModuleInformation; /* Chronometer information */
// END JK 08.09.03
}str_ModuleInformation;


/************************/
/* Hardware board infos */
/************************/
typedef struct
{
// BEGIN JK 08.09.03
	   struct
	      {
	      UINT  ui_Address;                  /* Board address          */
	      UINT  ui_FlashAddress;
	      BYTE   b_InterruptNbr;             /* Board interrupt number */
	      BYTE   b_SlotNumber;               /* PCI slot number        */
	      BYTE   b_BoardVersion;
	      DWORD dw_MolduleConfiguration [4]; /* Module configuration   */
	      }s_BoardInfos;

   str_BaseInformation       s_BaseInformation;           /* PCI BUS INFORMATIONS      */
   str_ModuleInformation     s_ModuleInformation[0];      /* Module information */
}
str_APCI1710BoardInformation;

typedef struct
{
   str_APCI1710BoardInformation s_BoardInformation[APCI1710_MAX_BOARD_NBR];
   BYTE                 b_NumberOfBoard;
   // BEGIN JK 08.09.03
   str_InterruptInfos s_InterruptInfos [16];
   // END JK 08.09.03

}
str_APCI1710_DriverStruct;



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
| Output Parameters : PULONG_  pul_CounterValue : 32-Bit counter value       |
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
int i_APCI1710_InitDigitalIO (BYTE b_BoardHandle,
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
int i_APCI1710_Read1DigitalInput ( BYTE  b_BoardHandle,
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
int i_APCI1710_ReadAllDigitalInput (BYTE b_BoardHandle,
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
int i_APCI1710_SetOutputMemoryOn (BYTE b_BoardHandle,
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
int i_APCI1710_SetOutputMemoryOff (BYTE b_BoardHandle,
                                   BYTE b_ModuleNumber);

/*
+----------------------------------------------------------------------------+
| Function   Name   : INT     i_APCI1710_Set1DigitalOutput                   |
|                                               (BYTE     b_BoardHandle,     |
|                                                BYTE     b_ModuleNumber,     |
|                                                BYTE     b_OutputValue,     |
|                                                BYTE     b_OutputChannel)   |
+----------------------------------------------------------------------------+
| Task              : (Re)sets the output which has been parametered with the| 
|                     variable b_OutputChannel. Setting an Output means      | 
|                     setting the output high.                               | 
+----------------------------------------------------------------------------+
| Input Parameters  : BYTE   b_BoardHandle  : Handle of the APCI-1710        |
|                     BYTE   b_ModuleNumber  : Selected Module number (0 - 3) |
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
int i_APCI1710_Set1DigitalOutput (BYTE b_BoardHandle,
                                  BYTE b_ModuleNumber,
                                  BYTE b_OutputValue,
                                  BYTE b_OutputChannel);

/*
+----------------------------------------------------------------------------+
| Function   Name   : INT     i_APCI1710_SetAllDigitalOutput                 |
|                                               (BYTE     b_BoardHandle,     |
|                                                BYTE     b_ModuleNumber,     |
|                                                BYTE     b_OutputValue,     |
|                                                BYTE     b_PortValue)       |
+----------------------------------------------------------------------------+
| Task              : Sets or resets the outputs which are parametered       | 
|                     with the variable b_PortValue. Setting an Output       | 
|                     means setting the output high.                         | 
+----------------------------------------------------------------------------+
| Input Parameters  : BYTE   b_BoardHandle  : Handle of the APCI-1710        |
|                     BYTE   b_ModuleNumber  : Selected Module number (0 - 3) |
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
int i_APCI1710_SetAllDigitalOutput (BYTE b_BoardHandle,
                                    BYTE b_ModuleNumber,
                                    BYTE b_OutputValue,
                                    BYTE b_PortValue);

/*
+----------------------------------------------------------------------------+
| Function Name     : _INT_ i_APCI1710_InitSSI                               |
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
| Input Parameters  : BYTE_ b_BoardHandle         : Handle of board APCI-1710|
|                     BYTE_ b_ModulNbr            : Module number to         |
|                                                   configure (0 to 3)       |
|                     BYTE_  b_SSIProfile         : Selection from SSI       |
|                                                   profile length (2 to 32).|
|                     BYTE_  b_PositionTurnLength : Selection from SSI       |
|                                                   position data length     |
|                                                   (1 to 31).               |
|                     BYTE_  b_TurnCptLength      : Selection from SSI turn  |
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
|                     ULONG_ ul_SSIOutputClock    : Selection from SSI output|
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
int i_APCI1710_InitSSI (BYTE   b_BoardHandle,
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
int i_APCI1710_Read1SSIValue (BYTE     b_BoardHandle,
			      BYTE     b_ModuleNumber,
			      BYTE     b_SelectedSSI,
			      PULONG pul_Position,
			      PULONG pul_TurnCpt);

/*
+----------------------------------------------------------------------------+
| Function Name     :     int  i_APCI1710_ReadAllSSIValue                    |
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

int i_APCI1710_ReadAllSSIValue (BYTE     b_BoardHandle,
				BYTE     b_ModuleNumber,
				PULONG pul_Position,
				PULONG pul_TurnCpt);



// BEGIN JK 08.09.03 : Implementation from CHRONO functionality
/*
+----------------------------------------------------------------------------+
| Function Name     : _INT_     i_APCI1710_InitChrono                        |
|                                       (BYTE_     b_BoardHandle,            |
|                                        BYTE_     b_ModulNbr,               |
|                                        BYTE_     b_ChronoMode,             |
|                                        BYTE_     b_PCIInputClock,          |
|                                        BYTE_     b_TimingUnit,             |
|                                        ULONG_   ul_TimingInterval,         |
|                                        PULONG_ pul_RealTimingInterval)     |
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
| Input Parameters  : BYTE_   b_BoardHandle    : Handle of board APCI-1710   |
|                     BYTE_   b_ModulNbr       : Module number to configure  |
|                                                (0 to 3)                    |
|                     BYTE_   b_ChronoMode     : Chronometer action mode     |
|                                                (0 to 7).                   |
|                     BYTE_   b_PCIInputClock  : Selection from PCI bus clock|
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
|                     BYTE_   b_TimingUnit    : Base timing unity (0 to 4)   |
|                                                 0 : ns                     |
|                                                 1 : µs                     |
|                                                 2 : ms                     |
|                                                 3 : s                      |
|                                                 4 : mn                     |
|                     ULONG_ ul_TimingInterval : Base timing value.          |
+----------------------------------------------------------------------------+
| Output Parameters : PULONG_  pul_RealTimingInterval : Real  base timing    |
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
| Function Name     : _INT_ i_APCI1710_EnableChrono                          |
|                                               (BYTE_ b_BoardHandle,        |
|                                                BYTE_ b_ModulNbr,           |
|                                                BYTE_ b_CycleMode,          |
|                                                BYTE_ b_InterruptEnable)    |
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
| Input Parameters  : BYTE_ b_BoardHandle  : Handle of board APCI-1710       |
|                     BYTE_ b_ModulNbr     : Selected module number (0 to 3) |
|                     BYTE_ b_CycleMode    : Selected the chronometer        |
|                                            acquisition mode                |
|                     BYTE_ b_InterruptEnable : Enable or disable the        |
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
| Function Name     : _INT_ i_APCI1710_DisableChrono                         |
|                                               (BYTE_  b_BoardHandle,       |
|                                                BYTE_  b_ModulNbr)          |
+----------------------------------------------------------------------------+
| Task              : Disable the chronometer from selected module           |
|                     (b_ModulNbr). If you disable the chronometer after a   |
|                     start signal occur and you restart the chronometer     |
|                     witch the " i_APCI1710_EnableChrono" function, if no   |
|                     stop signal occur this start signal is ignored.        |
+----------------------------------------------------------------------------+
| Input Parameters  : BYTE_ b_BoardHandle  : Handle of board APCI-1710       |
|                     BYTE_ b_ModulNbr     : Selected module number (0 to 3) |
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
| Function Name     : _INT_ i_APCI1710_GetChronoProgressStatus               |
|                               (BYTE_    b_BoardHandle,                     |
|                                BYTE_    b_ModulNbr,                        |
|                                PBYTE_  pb_ChronoStatus)                    |
+----------------------------------------------------------------------------+
| Task              : Return the chronometer status (pb_ChronoStatus) from   |
|                     selected chronometer module (b_ModulNbr).              |
+----------------------------------------------------------------------------+
| Input Parameters  : BYTE_ b_BoardHandle  : Handle of board APCI-1710       |
|                     BYTE_ b_ModulNbr     : Selected module number (0 to 3) |
+----------------------------------------------------------------------------+
| Output Parameters : PULONG_  pb_ChronoStatus : Return the chronometer      |
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
| Function Name     : _INT_ i_APCI1710_ReadChronoValue                       |
|                               (BYTE_     b_BoardHandle,                    |
|                                BYTE_     b_ModulNbr,                       |
|                                UINT_    ui_TimeOut,                        |
|                                PBYTE_   pb_ChronoStatus,                   |
|                                PULONG_ pul_ChronoValue)                    |
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
| Input Parameters  : BYTE_ b_BoardHandle  : Handle of board APCI-1710       |
|                     BYTE_ b_ModulNbr     : Selected module number (0 to 3) |
+----------------------------------------------------------------------------+
| Output Parameters : PULONG_  pb_ChronoStatus : Return the chronometer      |
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
| Function Name     : _INT_ i_APCI1710_ConvertChronoValue                    |
|                               (BYTE_     b_BoardHandle,                    |
|                                BYTE_     b_ModulNbr,                       |
|                                ULONG_   ul_ChronoValue,                    |
|                                PULONG_ pul_Hour,                           |
|                                PBYTE_   pb_Minute,                         |
|                                PBYTE_   pb_Second,                         |
|                                PUINT_  pui_MilliSecond,                    |
|                                PUINT_  pui_MicroSecond,                    |
|                                PUINT_  pui_NanoSecond)                     |
+----------------------------------------------------------------------------+
| Task              : Convert the chronometer measured timing                |
|                     (ul_ChronoValue) in to h, mn, s, ms, µs, ns.           |
+----------------------------------------------------------------------------+
| Input Parameters  : BYTE_   b_BoardHandle : Handle of board APCI-1710      |
|                     BYTE_   b_ModulNbr    : Selected module number (0 to 3)|
|                     ULONG_ ul_ChronoValue : Measured chronometer timing    |
|                                             value.                         |
|                                             See"i_APCI1710_ReadChronoValue"|
+----------------------------------------------------------------------------+
| Output Parameters : PULONG_   pul_Hour        : Chronometer timing hour    |
|                     PBYTE_     pb_Minute      : Chronometer timing minute  |
|                     PBYTE_     pb_Second      : Chronometer timing second  |
|                     PUINT_    pui_MilliSecond  : Chronometer timing mini   |
|                                                 second                     |
|                     PUINT_    pui_MicroSecond : Chronometer timing micro   |
|                                                 second                     |
|                     PUINT_    pui_NanoSecond  : Chronometer timing nano    |
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
| Function Name     : _INT_ i_APCI1710_SetChronoChlOn                        |
|                               (BYTE_  b_BoardHandle,                       |
|                                BYTE_  b_ModulNbr,                          |
|                                BYTE_  b_OutputChannel)                     |
+----------------------------------------------------------------------------+
| Task              : Sets the output witch has been passed with the         |
|                     parameter b_Channel. Setting an output means setting an|
|                     output high.                                           |
+----------------------------------------------------------------------------+
| Input Parameters  : BYTE_ b_BoardHandle   : Handle of board APCI-1710      |
|                     BYTE_ b_ModulNbr      : Selected module number (0 to 3)|
|                     BYTE_ b_OutputChannel : Selection from digital output  |
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
| Function Name     : _INT_ i_APCI1710_SetChronoChlOff                       |
|                               (BYTE_  b_BoardHandle,                       |
|                                BYTE_  b_ModulNbr,                          |
|                                BYTE_  b_OutputChannel)                     |
+----------------------------------------------------------------------------+
| Task              : Resets the output witch has been passed with the       |
|                     parameter b_Channel. Resetting an output means setting |
|                     an output low.                                         |
+----------------------------------------------------------------------------+
| Input Parameters  : BYTE_ b_BoardHandle   : Handle of board APCI-1710      |
|                     BYTE_ b_ModulNbr      : Selected module number (0 to 3)|
|                     BYTE_ b_OutputChannel : Selection from digital output  |
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
| Function Name     : _INT_ i_APCI1710_ReadChronoChlValue                    |
|                               (BYTE_   b_BoardHandle,                      |
|                                BYTE_   b_ModulNbr,                         |
|                                BYTE_   b_InputChannel,                     |
|                                PBYTE_ pb_ChannelStatus)                    |
+----------------------------------------------------------------------------+
| Task              : Return the status from selected digital input          |
|                     (b_InputChannel) from selected chronometer             |
|                     module (b_ModulNbr).                                   |
+----------------------------------------------------------------------------+
| Input Parameters  : BYTE_ b_BoardHandle   : Handle of board APCI-1710      |
|                     BYTE_ b_ModulNbr      : Selected module number (0 to 3)|
|                     BYTE_ b_InputChannel  : Selection from digital input   |
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
| Function Name     : _INT_ i_APCI1710_ReadChronoPortValue                   |
|                               (BYTE_   b_BoardHandle,                      |
|                                BYTE_   b_ModulNbr,                         |
|                                PBYTE_ pb_PortValue)                        |
+----------------------------------------------------------------------------+
| Task              : Return the status from digital inputs port from        |
|                     selected  (b_ModulNbr) chronometer module.             |
+----------------------------------------------------------------------------+
| Input Parameters  : BYTE_ b_BoardHandle   : Handle of board APCI-1710      |
|                     BYTE_ b_ModulNbr      : Selected module number (0 to 3)|
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
// END JK 08.09.03 : Implementation from CHRONO functionality




/* Rückgabe Anzahl gefundener Boards, -1 wenn kein pciBus */
int init_apci1710_module(void);

void cleanup_apci1710_module(void);


#endif




