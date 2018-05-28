/*
  +-----------------------------------------------------------------------+
  | (C) ADDI-DATA GmbH          Dieselstrasse 3      D-77833 Ottersweier  |
  +-----------------------------------------------------------------------+
  | Tel : +49 (0) 7223/9493-0     | email    : info@addi-data.com         |
  | Fax : +49 (0) 7223/9493-92    | Internet : http://www.addi-data.com   |
  +-----------------------------------------------------------------------+
  | Project   : APCI-1500         | Compiler : GCC                        |
  | Modulname : apci1500.h        | Version  : 2.95                       |
  +-------------------------------+---------------------------------------+
  | Author    : STOLZ, Eric       | Date     : 03.06.2001                 |
  +-----------------------------------------------------------------------+
  | Description : APCI1500 header	MAIN                              |
  +-----------------------------------------------------------------------+
  |                             UPDATE'S                                  |
  +-----------------------------------------------------------------------+
  |   Date   |   Author  |          Description of updates                |
  +----------+-----------+------------------------------------------------+
  | 28.06.04 | J. Krauth | - Update about interrupt and                   |
  |          |           |   function prototypes.                         |
  |          |           | - Change structure names.                      | 
  +-----------------------------------------------------------------------+
*/

/*******************************/
/* Include for ADDI-DATA types */
/*******************************/
#ifndef __ADDIDATA_TYPE
   #include "addidata_type.h"
   #define __ADDIDATA_TYPE
#endif

/*********************/
/* APCI-1500 defines */
/*********************/

#ifndef __APCI1500_DEFINES

#define __APCI1500_DEFINES   

#define APCI1500_BOARD_NAME                      "APCI1500"
#define APCI1500_BOARD_VENDOR_ID                 0x10E8
#define APCI1500_BOARD_DEVICE_ID                 0x80FC
#define APCI1500_MAX_BOARD_NBR                   10

#define APCI1500_IO_ADDRESS_RANGE                8
#define APCI1500_ZILLOG_ADDRESS_RANGE            4
#define APCI1500_INTERRUPT_ADDRESS_RANGE         0x42

#define APCI1500_DISABLE                         0
#define APCI1500_ENABLE                          1

#define APCI1500_AND                             2
#define APCI1500_OR                              4
#define APCI1500_OR_PRIORITY                     6

#define APCI1500_TIMER                           0
#define APCI1500_COUNTER                         0x20
#define APCI1500_WATCHDOG                        0
#define APCI1500_SINGLE                          0
#define APCI1500_CONTINUOUS                      0x80
#define APCI1500_SOFTWARE_TRIGGER                0x4
#define APCI1500_HARDWARE_TRIGGER                0x10
#define APCI1500_SOFTWARE_GATE                   0
#define APCI1500_HARDWARE_GATE                   0x8
#define APCI1500_1_8_KHZ                         2
#define APCI1500_3_6_KHZ                         1
#define APCI1500_115_KHZ                         0

enum
{
   APCI1500_Z8536_PORT_C,   
   APCI1500_Z8536_PORT_B,   
   APCI1500_Z8536_PORT_A,
   APCI1500_Z8536_CONTROL_REGISTER      
}
;

enum
{
   APCI1500_RW_MASTER_INTERRUPT_CONTROL,
   APCI1500_RW_MASTER_CONFIGURATION_CONTROL,
   APCI1500_RW_PORT_A_INTERRUPT_CONTROL,
   APCI1500_RW_PORT_B_INTERRUPT_CONTROL,
   APCI1500_RW_TIMER_COUNTER_INTERRUPT_VECTOR,
   APCI1500_RW_PORT_C_DATA_PCITCH_POLARITY,
   APCI1500_RW_PORT_C_DATA_DIRECTION,
   APCI1500_RW_PORT_C_SPECIAL_IO_CONTROL,
   
   APCI1500_RW_PORT_A_COMMAND_AND_STATUS,
   APCI1500_RW_PORT_B_COMMAND_AND_STATUS,
   APCI1500_RW_CPT_TMR1_CMD_STATUS,
   APCI1500_RW_CPT_TMR2_CMD_STATUS,
   APCI1500_RW_CPT_TMR3_CMD_STATUS,
   APCI1500_RW_PORT_A_DATA,
   APCI1500_RW_PORT_B_DATA,
   APCI1500_RW_PORT_C_DATA,
   
   APCI1500_R_CPT_TMR1_VALUE_HIGH,
   APCI1500_R_CPT_TMR1_VALUE_LOW,
   APCI1500_R_CPT_TMR2_VALUE_HIGH,
   APCI1500_R_CPT_TMR2_VALUE_LOW,
   APCI1500_R_CPT_TMR3_VALUE_HIGH,
   APCI1500_R_CPT_TMR3_VALUE_LOW,
   APCI1500_RW_CPT_TMR1_TIME_CST_HIGH,
   APCI1500_RW_CPT_TMR1_TIME_CST_LOW,
   APCI1500_RW_CPT_TMR2_TIME_CST_HIGH,
   APCI1500_RW_CPT_TMR2_TIME_CST_LOW,
   APCI1500_RW_CPT_TMR3_TIME_CST_HIGH,
   APCI1500_RW_CPT_TMR3_TIME_CST_LOW,
   APCI1500_RW_CPT_TMR1_MODE_SPECIFICATION,
   APCI1500_RW_CPT_TMR2_MODE_SPECIFICATION,
   APCI1500_RW_CPT_TMR3_MODE_SPECIFICATION,
   APCI1500_R_CURRENT_VECTOR,
   
   APCI1500_RW_PORT_A_SPECIFICATION,
   APCI1500_RW_PORT_A_HANDSHAKE_SPECIFICATION,
   APCI1500_RW_PORT_A_DATA_PCITCH_POLARITY,
   APCI1500_RW_PORT_A_DATA_DIRECTION,
   APCI1500_RW_PORT_A_SPECIAL_IO_CONTROL,
   APCI1500_RW_PORT_A_PATTERN_POLARITY,
   APCI1500_RW_PORT_A_PATTERN_TRANSITION,
   APCI1500_RW_PORT_A_PATTERN_MASK,

   APCI1500_RW_PORT_B_SPECIFICATION,
   APCI1500_RW_PORT_B_HANDSHAKE_SPECIFICATION,
   APCI1500_RW_PORT_B_DATA_PCITCH_POLARITY,
   APCI1500_RW_PORT_B_DATA_DIRECTION,
   APCI1500_RW_PORT_B_SPECIAL_IO_CONTROL,
   APCI1500_RW_PORT_B_PATTERN_POLARITY,
   APCI1500_RW_PORT_B_PATTERN_TRANSITION,
   APCI1500_RW_PORT_B_PATTERN_MASK
};

//DIGITAL OUTPUT OFFSET DEFINE
#define APCI1500_DIGITAL_OUTPUT                  0x02 

//TIMER DEFINE

#define CMD_MAX 30 	//Number of ioctl functions

	 /************************/
         /*  PCI BUS board infos */
	 /************************/

typedef struct
{
   UINT	  ui_BaseAddress[5];
   BYTE	  b_Interrupt;
   BYTE   b_SlotNumber;
}
str_APCI1500_BaseInformation ;	

typedef struct
{
   BYTE   b_TimerCounter1Init;
   BYTE   b_TimerCounter1Select;
   BYTE   b_TimerCounter1Enabled;
   BYTE   b_TimerCounter1InterruptEnabled;
   BYTE   b_TimerCounter2Init;
   BYTE   b_TimerCounter2Select;
   BYTE   b_TimerCounter2Enabled;
   BYTE   b_TimerCounter2InterruptEnabled;
   BYTE   b_WatchdogCounter3Init;
   BYTE   b_WatchdogCounter3Select;
   BYTE   b_Counter3Enabled;
   BYTE   b_Counter3InterruptEnabled;
}
str_APCI1500_TimerCounterInformation ;

typedef struct
{
   LONG   l_DigitalOutputRegister;   /* Digital Output Register */
   BYTE   b_OutputMemoryStatus;
}
str_APCI1500_DigitalOutputInformation ;

typedef struct
{
   BYTE   b_SetEventMaskPort1;
   BYTE   b_SetEventMaskPort2;
   BYTE   b_Event1Status;
   BYTE   b_Event2Status;
   BYTE   b_Event1InterruptLogik;
   BYTE   b_Event2InterruptLogik;
   BYTE   b_Port1Logik;
   BYTE   b_Port2Logik;
   BYTE   b_ProcessEnableEvent1Status;
   BYTE   b_ProcessEnableEvent2Status;
}
str_APCI1500_EventInformation ;

typedef struct
{
   BYTE   b_InterruptStatus;
   BYTE   b_InterruptInitialised;
}
str_APCI1500_InterruptInformation ;


         /************************/
	 /* Hardware board infos */
	 /************************/
typedef struct
{
   str_APCI1500_BaseInformation             s_BaseInformation ; /* PCI BUS Information     */
   str_APCI1500_DigitalOutputInformation    s_DigitalOutputInformation ;
   str_APCI1500_TimerCounterInformation     s_TimerCounterInformation ;
   str_APCI1500_InterruptInformation        s_InterruptInformation ;
   str_APCI1500_EventInformation            s_EventInformation ;
}
str_APCI1500_BoardInformation;


typedef struct 
{
   BYTE   b_InstallationCpt;   
   ULONG dw_BoardHandleArray;   
}str_APCI1500_GlobalInterruptInf;

typedef struct
{
   str_APCI1500_BoardInformation   s_BoardInformation   [APCI1500_MAX_BOARD_NBR];
   str_APCI1500_GlobalInterruptInf s_GlobalInterruptInf [256];
   VOID           (*usrHdl) (BYTE handle, BYTE intMask);
   BYTE                   b_NumberOfBoard;
}
str_APCI1500_DriverStructure;
 
/***********************/
/* Function prototypes */
/***********************/

/*
+----------------------------------------------------------------------------+
| Function   Name   : INT       i_APCI1500_Read1DigitalInput                 |
|                                               (BYTE     b_BoardHandle,     |
|                                                BYTE     b_Channel,         |
|                                                PBYTE  pb_ChannelValue)     |
+----------------------------------------------------------------------------+
| Task              : Reads the state of an input                            |
+----------------------------------------------------------------------------+
| Input Parameters  : BYTE    b_BoardHandle  : Handle of the APCI-1500       |
|                     BYTE    b_Channel      : Number of the input           |
|                                              to be read                    |
|                                              (1 to 16)                     |
+----------------------------------------------------------------------------+
| Output Parameters : PBYTE  pb_ChannelValue : Digital input channel         |
|                                              State :          0 -> Low     |
|                                                               1 -> High    |
+----------------------------------------------------------------------------+
| Return Value      :  0 : No error                                          |
|                     -1 : The handle parameter of the board is wrong        |
|                     -2 : Channel number is not between 1 and 16            |
+----------------------------------------------------------------------------+
*/

INT i_APCI1500_Read1DigitalInput (BYTE    b_BoardHandle,
				  BYTE    b_Channel,
				  PBYTE  pb_ChannelValue);
				  
/*
+----------------------------------------------------------------------------+
| Function name     : INT       i_APCI1500_Read8DigitalInput                 |
|                                          (BYTE    b_BoardHandle,           |
|                                           BYTE    b_Port,                  |
|                                           PBYTE  pb_PortValue)             |
+----------------------------------------------------------------------------+
| Task              : Reads the state of a port                              |
|                                                                            |
|                    Example:                                                |
|                         b_Port = 1                                         |
|                         pb_PortValue = 55 Hex                              |
|                         There is voltage on inputs 1, 3, 5, 7              |
|                         There is no voltage on the inputs 2, 4, 6, 8       |
|                                                                            |
+----------------------------------------------------------------------------+
| Input Parameters  : BYTE    b_BoardHandle : Handle of the APCI-1500        |
|                     BYTE    b_Port        : Number of the input port       |
|                                             to be read (1 or 2)            |
+----------------------------------------------------------------------------+
| Output Parameters : PBYTE  pb_PortValue   : State of the digital input     |
|                                             port (0 to 255)                |
+----------------------------------------------------------------------------+
| Return Value      :  0 : No error                                          |
|                     -1 : The handle parameter of the board is wrong        |
|                     -2 : Port number is not 1 or 2                         |
+----------------------------------------------------------------------------+
*/				  				  
				    
INT i_APCI1500_Read8DigitalInput (BYTE    b_BoardHandle,
				  BYTE    b_Port,
				  PBYTE  pb_PortValue);
				  				  
/*
+----------------------------------------------------------------------------+
| Function name     : INT       i_APCI1500_Read16DigitalInput                |
|                                          (BYTE    b_BoardHandle,           |
|                                           PLONG   pl_InputValue)           |
+----------------------------------------------------------------------------+
| Task              : Reads the state of both ports                          |
|                                                                            |
|                     Example:                                               |
|                             pl_InputValue = 5555 Hex                       |
|                             There is voltage on inputs 1, 3, 5, 7, 9, 11,  |
|                             13, 15                                         |
|                             There is no voltage on inputs 2, 4, 6, 8, 10,  |
|                             12, 14, 16                                     |
+----------------------------------------------------------------------------+
| Input Parameters  : BYTE    b_BoardHandle : Handle of the APCI-1500        |
+----------------------------------------------------------------------------+
| Output Parameters : PLONG   pl_InputValue  : State of the digital inputs of|
|                                             both ports (0 to 65535)        |
+----------------------------------------------------------------------------+
| Return Value      :  0 : No error                                          |
|                     -1 : The handle parameter of the board is wrong        |
+----------------------------------------------------------------------------+
*/
				    
INT i_APCI1500_Read16DigitalInput (BYTE    b_BoardHandle,
				   PLONG  pl_InputValue);
				   
/*
+----------------------------------------------------------------------------+
| Function name     : INT       i_APCI1500_Set1DigitalOutputOn               |
|                                                       (BYTE  b_BoardHandle,|
|                                                        BYTE  b_Channel)    |
+----------------------------------------------------------------------------+
| Task              : Activates an output                                    |
+----------------------------------------------------------------------------+
| Input Parameters  : BYTE    b_BoardHandle : Handle of the APCI-1500        |
|                     BYTE    b_Channel     : Number of the output channel   |
|                                             to be read (1 to 16)           |
+----------------------------------------------------------------------------+
| Output Parameters : -                                                      |
+----------------------------------------------------------------------------+
| Return Value      :  0 : No error                                          |
|                     -1 : The handle parameter of the board is wrong        |
|                     -2 : Channel number is not between 1 and 16            |
+----------------------------------------------------------------------------+
*/				   
				     
INT i_APCI1500_Set1DigitalOutputOn (BYTE  b_BoardHandle,
                                    BYTE  b_Channel);
                                    
/*
+----------------------------------------------------------------------------+
| Function name     : INT       i_APCI1500_Set1DigitalOutputOff              |
|                                                       (BYTE  b_BoardHandle,|
|                                                        BYTE  b_Channel)    |
+----------------------------------------------------------------------------+
| Task              : Deactivates an output.                                 |
|                                                                            |
|                     Attention: This function can only be used if the       |
|                                output memory is on.                        |
|                                See the function                            |
|                                i_APCI1500_SetOutputMemoryOn (..).          |
+----------------------------------------------------------------------------+
| Input Parameters  : BYTE    b_BoardHandle : Handle of the APCI-1500        |
|                     BYTE    b_Channel     : Number of the output channel   |
|                                             to deactivate                  |
|                                             (1 to 16)                      |
+----------------------------------------------------------------------------+
| Output Parameters : -                                                      |
+----------------------------------------------------------------------------+
| Return Value      :  0 : No error                                          |
|                     -1 : The handle parameter of the board is wrong        |
|                     -2 : Channel number is not between 1 to 16             |
|                     -3 : Output memory is not on                           |
+----------------------------------------------------------------------------+
*/                                    
					   
INT i_APCI1500_Set1DigitalOutputOff (BYTE  b_BoardHandle,
                                     BYTE  b_Channel);
                                     
/*
+----------------------------------------------------------------------------+
| Function name     : INT       i_APCI1500_Set8DigitalOutputOn               |
|                                                       (BYTE  b_BoardHandle,|
|                                                        BYTE  b_Port,       |
|                                                        BYTE  b_Value)      |
+----------------------------------------------------------------------------+
| Task              : Activates one or several outputs on a port             |
|                                                                            |
|                     Example:                                               |
|                               b_Port     = 1                               |
|                               b_Value = 55 Hex                             |
|                               The outputs 1, 3, 5, 7 are activated         |
|                                                                            |
+----------------------------------------------------------------------------+
| Input Parameters  : BYTE   b_BoardHandle : Handle of the APCI-1500         |
|                     BYTE   b_Port        : Number of the output port       |
|                                            (1 or 2)                        |
|                     BYTE   b_Value       : Output value (0 to 255)         |
+----------------------------------------------------------------------------+
| Output Parameters : -                                                      |
+----------------------------------------------------------------------------+
| Return Value      :  0 : No error                                          |
|                     -1 : The handle parameter of the board is wrong        |
|                     -2 : Port number is not 1 or 2                         |
+----------------------------------------------------------------------------+
*/                                     
					   
INT i_APCI1500_Set8DigitalOutputOn (BYTE  b_BoardHandle,
                                    BYTE  b_Port,
                                    BYTE  b_Value);
                                    
/*
+----------------------------------------------------------------------------+
| Function name     : INT       i_APCI1500_Set8DigitalOutputOff              |
|                                                       (BYTE  b_BoardHandle,|
|                                                        BYTE  b_Port,       |
|                                                        BYTE  b_Value)      |
+----------------------------------------------------------------------------+
| Task              : Deactivates one or several outputs of one port         |
|                                                                            |
|                     Example:                                               |
|                             b_Port     = 1                                 |
|                             b_Value = 55 Hex                               |
|                             The outputs 1, 3, 5, 7                         |
|                             are deactivated                                |
|                                                                            |
|                     Attention: This function can only be used if the       |
|                                output memory is on                         |
|                                See the function                            |
|                                i_APCI1500_SetOutputMemoryOn (..).          |
+----------------------------------------------------------------------------+
| Input Parameters  : BYTE  b_BoardHandle : Handle of the APCI-1500          |
|                     BYTE  b_Port        : Number of the output port        |
|                                           (1 or 2)                         |
|                     BYTE  b_Value       : output value (0 to 255)          |
+----------------------------------------------------------------------------+
| Output Parameters : -                                                      |
+----------------------------------------------------------------------------+
| Return Value      :  0 : No error                                          |
|                     -1 : The handle parameter of the board is wrong        |
|                     -2 : Port number is not 1 or 2                         |
|                     -3 : Output memory is not on                           |
+----------------------------------------------------------------------------+
*/                                    
					      
INT i_APCI1500_Set8DigitalOutputOff (BYTE  b_BoardHandle,
                                     BYTE  b_Port,
                                     BYTE  b_Value);
                                     
/*
+----------------------------------------------------------------------------+
| Function   Name   : INT       i_APCI1500_Set16DigitalOutputOn              |
|                                                       (BYTE  b_BoardHandle,|
|                                                        LONG  l_Value)      |
+----------------------------------------------------------------------------+
| Task              : Activates 1 or several outputs of the APCI-1500        |
|                                                                            |
|                     Example:                                               |
|                             b_Port  = 1                                    |
|                             l_Value = 5555 Hex                             |
|                             The outputs 1, 3, 5, 7, 9, 11, 13, 15          |
|                             are activated                                  |
+----------------------------------------------------------------------------+
| Input Parameters  : BYTE   b_BoardHandle : Handle of the APCI-1500         |
|                     LONG   l_Value       : Output value (0 to 65535)       |
+----------------------------------------------------------------------------+
| Output Parameters : -                                                      |
+----------------------------------------------------------------------------+
| Return Value      :  0 : No error                                          |
|                     -1 : The handle parameter of the board is wrong        |
+----------------------------------------------------------------------------+
*/                                     
					      
INT i_APCI1500_Set16DigitalOutputOn (BYTE  b_BoardHandle,
                                     LONG  l_Value);
                                     
/*
+----------------------------------------------------------------------------+
| Function Name     : INT       i_APCI1500_Set16DigitalOutputOff             |
|                                                       (BYTE  b_BoardHandle,|
|                                                        LONG  l_Value)      |
+----------------------------------------------------------------------------+
| Task              : Deactivates 1 or several outputs of the APCI-1500      |
|                                                                            |
|                     Example:                                               |
|                             b_Port  = 1                                    |
|                             l_Value = 5555 Hex                             |
|                             The outputs 1, 3, 5, 7, 9, 11, 13, 15          |
|                             are deactivated                                |
|                                                                            |
|                     Attention: This function can only be used if the       |
|                                the output memory is on                     |
|                                See the function                            |
|                                i_APCI1500_SetOutputMemoryOn (..).          |
+----------------------------------------------------------------------------+
| Input Parameters  : BYTE   b_BoardHandle : Handle of the APCI-1500         |
|                     LONG   l_Value       : Output value (0 to 65535)       |
+----------------------------------------------------------------------------+
| Output Parameters : -                                                      |
+----------------------------------------------------------------------------+
| Return Value      :  0 : No error                                          |
|                     -1 : The handle parameter of the board is wrong        |
|                     -2 : Output memory is not on                           |
+----------------------------------------------------------------------------+
*/                                     
					      
INT i_APCI1500_Set16DigitalOutputOff (BYTE  b_BoardHandle,
                                      LONG  l_Value);

/*
+----------------------------------------------------------------------------+
| Function name     : INT   i_APCI1500_SetOutputMemoryOn(BYTE  b_BoardHandle)|
+----------------------------------------------------------------------------+
| Task              :  Sets the output memory on.                            |
+----------------------------------------------------------------------------+
| Input Parameters  : BYTE   b_BoardHandle : Handle of the APCI-1500         |
+----------------------------------------------------------------------------+
| Output Parameters : -                                                      |
+----------------------------------------------------------------------------+
| Return Value      :  0 : No error                                          |
|                     -1 : The handle parameter of the board is wrong        |
+----------------------------------------------------------------------------+
*/
					      
INT i_APCI1500_SetOutputMemoryOn (BYTE  b_BoardHandle);

/*
+----------------------------------------------------------------------------+
| Function nName    : INT   i_APCI1500_SetOutputMemoryOff                    |
|						(BYTE  b_BoardHandle)	     |
+----------------------------------------------------------------------------+
| Task              : Sets the output memory off.                            |
+----------------------------------------------------------------------------+
| Input Parameters  : BYTE   b_BoardHandle : Handle of the APCI-1500         |
+----------------------------------------------------------------------------+
| Output Parameters : -                                                      |
+----------------------------------------------------------------------------+
| Return Value      :  0 : No error                                          |
|                     -1 : The handle parameter of the board is wrong        |
|                     -2 : Output Status file does not exist                 | 
+----------------------------------------------------------------------------+
*/

INT i_APCI1500_SetOutputMemoryOff (BYTE  b_BoardHandle);

/*
+----------------------------------------------------------------------------+
| Function nName    : INT   i_APCI1500_InitZilog                             |
|						(BYTE  b_BoardHandle)	     |
+----------------------------------------------------------------------------+
| Task              : Set the ZiLOG in the used state. Resets the ZiLOG per  |
|                     software.                                              |
+----------------------------------------------------------------------------+
| Input Parameters  : BYTE   b_BoardHandle : Handle of the APCI-1500         |
+----------------------------------------------------------------------------+
| Output Parameters : -                                                      |
+----------------------------------------------------------------------------+
| Return Value      :  0 : No error                                          |
|                     -1 : The handle parameter of the board is wrong        |
+----------------------------------------------------------------------------+
*/

INT i_APCI1500_InitZilog (BYTE  b_BoardHandle); 

/*
+----------------------------------------------------------------------------+
| Function name   : INT         i_APCI1500_InitTimerInputClock               |
|                               (BYTE  b_BoardHandle,                        |
|                                BYTE  b_InputClockSelect)                   |
+----------------------------------------------------------------------------+
| Task              : Sets the input clock for all timers                    |
+----------------------------------------------------------------------------+
| Input parameters  : BYTE    b_BoardHandle      : APCI-1500 board handle    |
|                     BYTE    b_InputClockSelect : APCI1500_115_KHZ : 115 KHz|
|                                                  APCI1500_3_6_KHZ : 3.6 KHz|
|                                                  APCI1500_1_8_KHZ : 1.8 KHz|
+----------------------------------------------------------------------------+
| Output Parameters : -                                                      |
+----------------------------------------------------------------------------+
| Return Value      :  0 : No error                                          |
|                     -1 : The handle parameter of the board is wrong        |
|                     -2 : Input clock Error                                 |
+----------------------------------------------------------------------------+
*/

INT i_APCI1500_InitTimerInputClock (BYTE  b_BoardHandle,
                                    BYTE  b_InputClockSelect);
                                    
/*
+----------------------------------------------------------------------------+
| Function name   : INT       i_APCI1500_InitTimerCounter1                   |
|                                    (BYTE   b_BoardHandle,                  |
|                                     BYTE   b_CounterOrTimerSelect,         |
|                                     LONG   l_ReloadValue,                  |
|                                     BYTE   b_ContinuousOrSingleCycleSelect,|
|                                     BYTE   b_InterruptHandling)            |
+----------------------------------------------------------------------------+
| Task              : Selects the operating mode for the first               |
|                     counter/timer                                          |
+----------------------------------------------------------------------------+
| Input parameters  : BYTE   b_BoardHandle          :Handle of the APCI-1500 |
|                     BYTE   b_CounterOrTimerSelect :Selects the mode for    |
|                                                    the counter/timer       |
|                                                    APCI1500_TIMER:The 1st  |
|                                                                   counter/ |
|                                                                   timer is |
|                                                                   used as  |
|                                                                   a timer  |
|                                                                            |
|                                                    APCI1500_ZAHLER:The 1st |
|                                                                   counter/ |
|                                                                   timer is |
|                                                                   used as  |
|                                                                   a counter|
|                                                                            |
|                     LONG   l_ReloadValue           : This parameter has    |
|                                                      two meanings.         |
|                                                    - If the counter/timer  |
|                                                      is used as a counter  |
|                                                      the limit value of    |
|                                                      the counter is given  |
|                                                                            |
|                                                    - If the counter/timer  |
|                                                      is used as a timer,   |
|                                                      the divider factor    |
|                                                      for the output is     |
|                                                      given.                |
|                     BYTE  b_ContinuousOrSingleCycleSelect :                |
|                                                    APCI1500_CONTINUOUS:    |
|                                                       Everytime the        |
|                                                       counter or timer     |
|                                                       value is set on 1,   |
|                                                       the l_ReloardValue   |
|                                                       is loaded            |
|                                                    APCI1500_SINGLE:        |
|                                                       Everytime the        |
|                                                       counter or timer     |
|                                                       value is set on 1,   |
|                                                       the counter or timer |
|                                                       stops.               |
|                     BYTE  b_InterruptHandling      : At counter run down   |
|                                                      or timer output level |
|                                                      high, an interrupt    |
|                                                      can be generated.     |
|                                                      This parameter allows |
|                                                      the user to decide    |
|                                                      if he uses the        |
|                                                      interrupt or not      |
|                                                    APCI1500_ENABLE:        |
|                                                      Enables the           |
|                                                      interrupt.            |
|                                                    APCI1500_DISABLE:       |
|                                                      Disables the          |
|                                                      interrupt.            |
+----------------------------------------------------------------------------+
| Output Parameters : -                                                      |
+----------------------------------------------------------------------------+
| Return Value      :  0 : No error                                          |
|                     -1 : The handle parameter of the board is wrong        |
|                     -2 : The parameter for selecting the counter or the    |
|                          timer is wrong                                    |
|                     -3 : Error with th interrupt selection                 |
|                     -4 : User interrupt routine not installed              |
|                     -5 : The cycle parameter is wrong                      |
+----------------------------------------------------------------------------+
*/                                    
					  
INT i_APCI1500_InitTimerCounter1 (BYTE   b_BoardHandle,
                                  BYTE   b_CounterOrTimerSelect,
                                  LONG   l_ReloadValue,
                                  BYTE   b_ContinuousOrSingleCycleSelect,
                                  BYTE   b_InterruptHandling);
                                  
/*
+----------------------------------------------------------------------------+
| Function name     : INT       i_APCI1500_InitTimerCounter2                 |
|                                  (BYTE   b_BoardHandle,                    |
|                                   BYTE   b_CounterOrTimerSelect,           |
|                                   LONG   l_ReloadValue,                    |
|                                   BYTE   b_ContinuousOrSingleCycleSelect,  |
|                                   BYTE   b_HardwareOrSoftwareTriggerSelect,|
|                                   BYTE   b_HardwareOrSoftwareGateSelect,   |
|                                   BYTE   b_InterruptHandling)              |
+----------------------------------------------------------------------------+
| Task              : Selects the operating mode for the second              |
|                     counter/timer                                          |
+----------------------------------------------------------------------------+
| Input parameters  : BYTE  b_BoardHandle          : Handle of the APCI-1500 |
|                     BYTE  b_CounterOrTimerSelect : Selection of the second |
|                                                    counter/timer mode      |
|                                                   APCI1500_TIMER:The second|
|                                                                  counter/  |
|                                                                  timer is  |
|                                                                  used as a |
|                                                                  timer     |
|                                                   APCI1500_ZAHLER:         |
|								   The second|
|                                                                  counter/  |
|                                                                  timer is  |
|                                                                  used as a |
|                                                                  counter   |
|                     LONG   l_ReloadValue           : This parameter has    |
|                                                      two meanings.         |
|                                                    - If the counter/timer  |
|                                                      is used as a counter  |
|                                                      the limit value of    |
|                                                      the counter is loaded |
|                                                    - If the counter/timer  |
|                                                      is used as a timer    |
|                                                      the divider factor    |
|                                                      for the output is     |
|                                                      loaded.               |
|                     BYTE  b_ContinuousOrSingleCycleSelect :                |
|                                                    APCI1500_CONTINUOUS:    |
|                                                       Everytime the        |
|                                                       counter or timer     |
|                                                       value is on 1        |
|                                                       the                  |
|                                                       l_ReloardValue       |
|                                                       is be loaded         |
|                                                    APCI1500_SINGLE:        |
|                                                       If the counter or    |
|                                                       timer value is on 1  |
|                                                       the counter or timer |
|                                                       is stopped.          |
|                     BYTE  b_HardwareOrSoftwareTriggerSelect :              |
|                                              APCI1500_HARDWARE_TRIGGER:    |
|                                                       Input channel 12 is  |
|                                                       used for the trigger |
|                                              APCI1500_SOFTWARE_TRIGGER:    |
|                                                       Input 12 has no      |
|                                                       effect on trigger    |
|                     BYTE  b_HardwareOrSoftwareGateSelect    :              |
|                                               APCI1500_HARDWARE_GATE:      |
|                                                       Input channel 13 is  |
|                                                       used for the gate.   |
|                                               APCI1500_SOFTWARE_GATE:      |
|                                                       Input channel 13 has |
|                                                       no effect on gate    |
|                     BYTE  b_InterruptHandling      :  At counter run down  |
|                                                       or if the output     |
|                                                       is on high level     |
|                                                       an interrupt can be  |
|                                                       generated.           |
|                                                       This parameter allows|
|                                                       the user to decide   |
|                                                       if the interrupt is  |
|                                                       to be used or not.   |
|                                                    APCI1500_ENABLE:        |
|                                                       Enables the          |
|                                                       interrupt            |
|                                                    APCI1500_DISABLE:       |
|                                                       Disables  the        |
|                                                       interrupt.           |
+----------------------------------------------------------------------------+
| Output Parameters : -                                                      |
+----------------------------------------------------------------------------+
| Return value      :  0 : No error                                          |
|                     -1 : The handle parameter of the board is wrong        |
|                     -2 : Parameter for selecting the timer/counter is wrong|
|                     -3 : Error with the interrupt selection                |
|                     -4 : User interrupt routine not installed              |
|                     -5 : Cyle parameter wrong                              |
|                     -6 : Wrong gate parameter                              |
|                     -7 : Wrong trigger parameter                           |
+----------------------------------------------------------------------------+
*/                                  
					   
INT i_APCI1500_InitTimerCounter2 (BYTE   b_BoardHandle,
                                  BYTE   b_CounterOrTimerSelect,
                                  LONG   l_ReloadValue,
                                  BYTE   b_ContinuousOrSingleCycleSelect,
                                  BYTE   b_HardwareOrSoftwareTriggerSelect,
                                  BYTE   b_HardwareOrSoftwareGateSelect,
                                  BYTE   b_InterruptHandling);
                                  
/*
+-----------------------------------------------------------------------------+
| Function name   : INT    i_APCI1500_InitWatchdogCounter3                    |
|                                    (BYTE   b_BoardHandle,                   |
|                                     BYTE   b_WatchdogOrCounterSelect,       |
|                                     LONG   l_ReloadValue,                   |
|                                     BYTE   b_ContinuousOrSingleCycleSelect, |
|                                     BYTE   b_HardwareOrSoftwareGateSelect,  |
|                                     BYTE   b_InterruptHandling)             |
+-----------------------------------------------------------------------------+
| Task              : Selects the working mode for the third                  |
|                     counter/watchdog                                        |
+-----------------------------------------------------------------------------+
| Input parameters  : BYTE  b_BoardHandle             : Handle of the APCI1500|
|                     BYTE  b_WatchdogOrCounterSelect : Selection of the      |
|                                                       watchdog/counter mode |
|                                                    APCI1500_WATCHDOG: The   |
|                                                                  counter/   |
|                                                                  watchdog   |
|                                                                  is used as |
|                                                                  a watchdog |
|                                                    APCI1500_ZAHLER : The    |
|                                                                   counter/  |
|                                                                   watchdog  |
|                                                                   is used as|
|                                                                   a counter |
|                     LONG  l_ReloadValue           :  This parameter has     |
|                                                      two meanings.          |
|                                                    - If the                 |
|                                                      counter/watchdog       |
|                                                      is used as a counter,  |
|                                                      the limit value of the |
|                                                      counter is loaded      |
|                                                    - If the                 |
|                                                      counter/watchdog       |
|                                                      is used as a watchdog  |
|                                                      the divider factor     |
|                                                      is loaded.             |
|                     BYTE  b_ContinuousOrSingleCycleSelect :                 |
|                                                    APCI1500_CONTINUOUS:     |
|                                                       Everytime the         |
|                                                       counting value is     |
|                                                       on 1 the              |
|                                                       l_ReloardValue        |
|                                                       is loaded             |
|                                                    APCI1500_SINGLE:         |
|                                                       If the counting       |
|                                                       value is on 1         |
|                                                       the  counter or timer |
|                                                       is stopped.           |
|                     BYTE  b_HardwareOrSoftwareGateSelect    :               |
|                                               APCI1500_HARDWARE_GATE:       |
|                                                       Input channel 16 is   |
|                                                       used for the gate.    |
|                                               APCI1500_SOFTWARE_GATE:       |
|                                                       Input channel 16 has  |
|                                                       no effect on the gate |
|                     BYTE  b_InterruptHandling      :  Interrupts can be     |
|                                                       generated at counter  |
|                                                       run down or if the    |
|                                                       watchdog output is on |
|                                                       high. With this       |
|                                                       parameter the user    |
|                                                       decides if interrupt  |
|                                                       is used or not.       |
|                                                    APCI1500_ENABLE:         |
|                                                       Enables  the          |
|                                                       interrupt.            |
|                                                    APCI1500_DISABLE:        |
|                                                       Disables  the         |
|                                                       interrupt.            |
+-----------------------------------------------------------------------------+
| Output Parameters : -                                                       |
+-----------------------------------------------------------------------------+
| Return Value      :  0 : No error                                           |
|                     -1 : The handle parameter of the board is wrong         |
|                     -2 : Parameter for selecting the counter/watchdog wrong |
|                     -3 : Error with the interrupt selection                 |
|                     -4 : User interrupt routine not installed               |
|                     -5 : Cycle parameter is wrong                           |
|                     -6 : Wrong gate parameter                               |
+-----------------------------------------------------------------------------+
*/                                  
					   
INT i_APCI1500_InitWatchdogCounter3 (BYTE   b_BoardHandle,
                                     BYTE   b_WatchdogOrCounterSelect,
                                     LONG   l_ReloadValue,
                                     BYTE   b_ContinuousOrSingleCycleSelect,
                                     BYTE   b_HardwareOrSoftwareGateSelect,
                                     BYTE   b_InterruptHandling);
                                     
/*
+----------------------------------------------------------------------------+
| Function name   : INT   i_APCI1500_StartTimerCounter1(BYTE  b_BoardHandle) |
+----------------------------------------------------------------------------+
| Task              : Starts the first counter/timer                         |
+----------------------------------------------------------------------------+
| Input parameters  : BYTE   b_BoardHandle : Handle of the APCI-1500         |
+----------------------------------------------------------------------------+
| Output Parameters : -                                                      |
+----------------------------------------------------------------------------+
| Return Value      :  0 : No error                                          |
|                     -1 : The handle parameter of the board is wrong        |
|                     -2 : Counter or timer not installed                    |
+----------------------------------------------------------------------------+
*/                                     
					   
INT i_APCI1500_StartTimerCounter1 (BYTE  b_BoardHandle);

/*
+----------------------------------------------------------------------------+
| Function name   : INT   i_APCI1500_StartTimerCounter2(BYTE  b_BoardHandle) |
+----------------------------------------------------------------------------+
| Task              : Starts the second counter/timer                        |
+----------------------------------------------------------------------------+
| Input parameters  : BYTE   b_BoardHandle : Handle of the APCI-1500         |
+----------------------------------------------------------------------------+
| Output Parameters : -                                                      |
+----------------------------------------------------------------------------+
| Return Value      :  0 : No error                                          |
|                     -1 : The handle parameter of the board is wrong        |
|                     -2 : Counter or timer not installed                    |
+----------------------------------------------------------------------------+
*/

INT i_APCI1500_StartTimerCounter2 (BYTE  b_BoardHandle);

/*
+----------------------------------------------------------------------------+
| Function name   : INT   i_APCI1500_StartCounter3 (BYTE  b_BoardHandle)     |
+----------------------------------------------------------------------------+
| Task              : Starts the third counter                               |
+----------------------------------------------------------------------------+
| Input parameters  : BYTE   b_BoardHandle : Handle of the APCI-1500         |
+----------------------------------------------------------------------------+
| Output Parameters : -                                                      |
+----------------------------------------------------------------------------+
| Return Value      :  0 : No error                                          |
|                     -1 : The handle parameter of the board is wrong        |
|                     -2 : Counter not installed                             |
+----------------------------------------------------------------------------+
*/

INT i_APCI1500_StartCounter3 (BYTE  b_BoardHandle);

/*
+----------------------------------------------------------------------------+
| Function name   : INT   i_APCI1500_StopTimerCounter1 (BYTE  b_BoardHandle) |
+----------------------------------------------------------------------------+
| Task              : Stops the first counter/timer                          |
+----------------------------------------------------------------------------+
| Input parameters  : BYTE  b_BoardHandle : Handle of the APCI-1500          |
+----------------------------------------------------------------------------+
| Output Parameters : -                                                      |
+----------------------------------------------------------------------------+
| Return Value      :  0 : No error                                          |
|                     -1 : The handle parameter of the board is wrong        |
|                     -2 : Counter or timer not initialized                  |
+----------------------------------------------------------------------------+
*/

INT i_APCI1500_StopTimerCounter1 (BYTE  b_BoardHandle);

/*
+----------------------------------------------------------------------------+
| Function name   : INT   i_APCI1500_StopTimerCounter2 (BYTE  b_BoardHandle) |
+----------------------------------------------------------------------------+
| Task              : Stops the second counter/timer                         |
+----------------------------------------------------------------------------+
| Input parameters  : BYTE  b_BoardHandle : Handle of the APCI-1500          |
+----------------------------------------------------------------------------+
| Output Parameters : -                                                      |
+----------------------------------------------------------------------------+
| Return Value      :  0 : No error                                          |
|                     -1 : The handle parameter of the board is wrong        |
|                     -2 : Counter or timer not initialized                  |
+----------------------------------------------------------------------------+
*/

INT i_APCI1500_StopTimerCounter2 (BYTE  b_BoardHandle);

/*
+----------------------------------------------------------------------------+
| Function name   : INT   i_APCI1500_StopCounter3 (BYTE  b_BoardHandle)      |
+----------------------------------------------------------------------------+
| Task              : Stops the third counter                                |
+----------------------------------------------------------------------------+
| Input parameters  : BYTE  b_BoardHandle : Handle of the APCI-1500          |
+----------------------------------------------------------------------------+
| Output Parameters : -                                                      |
+----------------------------------------------------------------------------+
| Return Value      :  0 : No error                                          |
|                     -1 : The handle parameter of the board is wrong        |
|                     -2 : Counter or timer not initialized                  |
+----------------------------------------------------------------------------+
*/

INT i_APCI1500_StopCounter3 (BYTE  b_BoardHandle);

/*
+----------------------------------------------------------------------------+
| Function name   : INT   i_APCI1500_TriggerTimerCounter1                    |
|                                               (BYTE  b_BoardHandle)        |
+----------------------------------------------------------------------------+
| Task              : Triggers the first counter/timer                       |
+----------------------------------------------------------------------------+
| Input parameters  : BYTE  b_BoardHandle : Handle of the APCI-1500          |
+----------------------------------------------------------------------------+
| Output Parameters : -                                                      |
+----------------------------------------------------------------------------+
| Return Value      :  0 : No error                                          |
|                     -1 : The handle parameter of the board is wrong        |
|                     -2 : counter or timer not initialized                  |
+----------------------------------------------------------------------------+
*/

INT i_APCI1500_TriggerTimerCounter1 (BYTE  b_BoardHandle);

/*
+----------------------------------------------------------------------------+
| Function name   : INT   i_APCI1500_TriggerTimerCounter2                    |
|                                               (BYTE  b_BoardHandle)        |
+----------------------------------------------------------------------------+
| Task              : Triggers the second counter/timer                      |
+----------------------------------------------------------------------------+
| Input parameters  : BYTE  b_BoardHandle : Handle of the APCI-1500          |
+----------------------------------------------------------------------------+
| Output Parameters : -                                                      |
+----------------------------------------------------------------------------+
| Return Value      :  0 : No error                                          |
|                     -1 : The handle parameter of the board is wrong        |
|                     -2 : counter or timer not initialized                  |
+----------------------------------------------------------------------------+
*/

INT i_APCI1500_TriggerTimerCounter2 (BYTE  b_BoardHandle);

/*
+----------------------------------------------------------------------------+
| Function name   : INT   i_APCI1500_TriggerCounter3                         |
|                                               (BYTE  b_BoardHandle)        |
+----------------------------------------------------------------------------+
| Task              : Triggers the third counter                             |
+----------------------------------------------------------------------------+
| Input parameters  : BYTE  b_BoardHandle : Handle of the APCI-1500          |
+----------------------------------------------------------------------------+
| Output Parameters : -                                                      |
+----------------------------------------------------------------------------+
| Return Value      :  0 : No error                                          |
|                     -1 : The handle parameter of the board is wrong        |
|                     -2 : counter not initialized                           |
|                     -3 : counter initialized as watchdog                   |
+----------------------------------------------------------------------------+
*/

INT i_APCI1500_TriggerCounter3 (BYTE  b_BoardHandle);

/*
+----------------------------------------------------------------------------+
| Function name     : INT   i_APCI1500_TriggerWatchdog                       |
|                                               (BYTE  b_BoardHandle)        |
+----------------------------------------------------------------------------+
| Task              : Triggers the third  counter                            |
+----------------------------------------------------------------------------+
| Input parameters  : BYTE  b_BoardHandle : Handle of the APCI-1500          |
+----------------------------------------------------------------------------+
| Output Parameters : -                                                      |
+----------------------------------------------------------------------------+
| Return Value      :  0 : No error                                          |
|                     -1 : The handle parameter of the board is wrong        |
|                     -2 : Counter or timer not initialized                  |
|                     -3 : Counter/watchdog as counter initialized           |
+----------------------------------------------------------------------------+
*/

INT i_APCI1500_TriggerWatchdog (BYTE  b_BoardHandle);

/*
+----------------------------------------------------------------------------+
| Function name   : INT        i_APCI1500_ReadTimerCounter1                  |
|                                              (BYTE     b_BoardHandle,      |
|                                               PLONG   pl_ReadValue)        |
+----------------------------------------------------------------------------+
| Task              : Reads the current value of the first counter/timer     |
|                     if it is used as a counter or reads                    |
|                     the timer value if it is used as a timer               |
+----------------------------------------------------------------------------+
| Input parameters  : BYTE   b_BoardHandle :Handle of the APCI-1500          |
+----------------------------------------------------------------------------+
| Output Parameters : PLONG  pl_ReadValue  :This parameter has two meanings  |
|                                           If the counter/timer is used as a|
|                                           counter it loads the actuel      |
|                                           value of the counter.            |
|                                           If the counter/timer is used as a|
|                                           timer it loads the current       |
|                                           value of the timer.              |
+----------------------------------------------------------------------------+
| Return Value      :  0 : No error                                          |
|                     -1 : The handle parameter of the board is wrong        |
|                     -2 : Counter or timer not initialized                  |
+----------------------------------------------------------------------------+
*/

INT i_APCI1500_ReadTimerCounter1 (BYTE    b_BoardHandle,
                                  PLONG  pl_ReadValue);

/*
+----------------------------------------------------------------------------+
| Function name   : INT        i_APCI1500_ReadTimerCounter2                  |
|                                              (BYTE     b_BoardHandle,      |
|                                               PLONG   pl_ReadValue)        |
+----------------------------------------------------------------------------+
| Task              : Reads the current value of the second counter/timer    |
|                     if it is used as a counter or reads                    |
|                     the timer value if it is used as a timer               |
+----------------------------------------------------------------------------+
| Input parameters  : BYTE   b_BoardHandle :Handle of the APCI-1500          |
+----------------------------------------------------------------------------+
| Output Parameters : PLONG  pl_ReadValue  :This parameter has two meanings  |
|                                           If the counter/timer is used as a|
|                                           counter it loads the actuel      |
|                                           value of the counter.            |
|                                           If the counter/timer is used as a|
|                                           timer it loads the current       |
|                                           value of the timer.              |
+----------------------------------------------------------------------------+
| Return Value      :  0 : No error                                          |
|                     -1 : The handle parameter of the board is wrong        |
|                     -2 : Counter or timer not initialized                  |
+----------------------------------------------------------------------------+
*/
					    
INT i_APCI1500_ReadTimerCounter2 (BYTE    b_BoardHandle,
                                  PLONG  pl_ReadValue);
                                  
/*
+----------------------------------------------------------------------------+
| Function name   : INT        i_APCI1500_ReadCounter3                       |
|                                              (BYTE     b_BoardHandle,      |
|                                               PLONG   pl_ReadValue)        |
+----------------------------------------------------------------------------+
| Task              : Reads the current value of the third counter/timer     |
|                     if it is used as a counter or reads                    |
|                     the timer value if it is used as a timer               |
+----------------------------------------------------------------------------+
| Input parameters  : BYTE   b_BoardHandle :Handle of the APCI-1500          |
+----------------------------------------------------------------------------+
| Output Parameters : PLONG  pl_ReadValue  :This parameter has two meanings  |
|                                           If the counter/timer is used as a|
|                                           counter it loads the actuel      |
|                                           value of the counter.            |
|                                           If the counter/timer is used as a|
|                                           timer it loads the current       |
|                                           value of the timer.              |
+----------------------------------------------------------------------------+
| Return Value      :  0 : No error                                          |
|                     -1 : The handle parameter of the board is wrong        |
|                     -2 : Counter or timer not initialized                  |
|                     -3 : Watchdog / Counter initialized as watchdog        |
+----------------------------------------------------------------------------+
*/                                  
					    
INT i_APCI1500_ReadCounter3 (BYTE    b_BoardHandle,
                             PLONG  pl_ReadValue);
                             
/*
+------------------------------------------------------------------------------+
| Function name   : INT       i_ConvertString (PCHAR pc_EventMask,             |
|                                                BYTE  b_PortNbr,              |
|                                                PBYTE pb_PatternPolarity,     |
|                                                PBYTE pb_PatternTransition,   |
|                                                PBYTE pb_PatternMask)         |
+------------------------------------------------------------------------------+
| Task              : Converts the string                                      |
+------------------------------------------------------------------------------+
| Input parameters  : PCHAR pc_EventMask         : This 8-character word for   |
|                                                  for port 1 and              |
|                                                  6-character word for port 2 |
|                                                  gives the mask of the event |
|                     BYTE   b_PortNbr           : Number of the input port    |
|                                                  on which the event will     |
|                                                  take place (1 or 2)         |
+------------------------------------------------------------------------------+
| Output Parameters : PBYTE pb_PatternPolarity   : Polarity                    |
|                     PBYTE pb_PatternTransition : Transition                  |
|                     PBYTE pb_PatternMask       : Mask                        |
+------------------------------------------------------------------------------+
| Return value      :  0 : No error                                            |
|                     -1 : Error                                               |
+------------------------------------------------------------------------------+
*/                             
					    
INT i_ConvertString (CHAR*  pc_EventMask,
                     BYTE    b_PortNbr,
                     PBYTE  pb_PatternPolarity,
                     PBYTE  pb_PatternTransition,
                     PBYTE  pb_PatternMask);
                     
/*
+----------------------------------------------------------------------------+
| Function name      : INT   i_APCI1500_SetInputEventMask                     |
|						      (BYTE    b_BoardHandle,|
|                                                      BYTE    b_PortNbr,    |
|                                                      BYTE    b_logic,      |
|                                                      PCHAR      pc_EventMask)  |
+----------------------------------------------------------------------------+
| Task              : An event can be generated for each port.               |
|                     The first event is related to the first 8 channels     |
|                     (port 1) and the second to the following 6 channels    |
|                     (port 2). An interrupt is generated when one or both   |
|                     events have occurred                                   |
+----------------------------------------------------------------------------+
| Input Parameters  : BYTE    b_BoardHandle: Handle of the APCI-1500         |
|                     BYTE    b_PortNbr    : Number of the input port on     |
|                                            which the event will take place |
|                                            (1 or 2)                        |
|                     BYTE    b_logik      : The event logic for port 1 has  |
|                                            three possibilities             |
|                                            APCI1500_AND       :This logic  |
|                                                                links       |
|                                                                the inputs  |
|                                                                with an AND |
|                                                                logic.      |
|                                            APCI1500_OR        :This logic  |
|                                                                links       |
|                                                                the inputs  |
|                                                                with a      |
|                                                                OR logic.   |
|                                            APCI1500_OR_PRIORITY            |
|								:This logic  |
|                                                                links       |
|                                                                the inputs  |
|                                                                with a      |
|                                                                polarity    |
|                                                                OR logic.   |
|                                                                Input 1     |
|                                                                has the     |
|                                                                highest     |
|                                                                polarity    |
|                                                                level and   |
|                                                                input   16  |
|                                                                the smallest|
|                                            For the second port the user has|
|                                            1 possibility:                  |
|                                            APCI1500_OR        :This logic  |
|                                                                links       |
|                                                                the inputs  |
|                                                                with a      |
|                                                                polarity    |
|                                                                OR logic    |
|                     PCHAR      pc_EventMask  : These 8-character word for port1|
|                                            and 6-character word for port 2 |
|                                            give the mask of the event.     |
|                                            Each place gives the state      |
|                                            of the input channels and can   |
|                                            have one of these six characters|
|                                      "X" : This input is not               |
|                                            used for event                  |
|                                      "0" : This input must be on 0         |
|                                      "1" : This input must be on 1         |
|                                      "2" : This input reacts to            |
|                                            a falling edge                  |
|                                      "3" : This input reacts to a          |
|                                            rising edge                     |
|                                      "4" : This input reacts to both edges |
|                                                                            |
+----------------------------------------------------------------------------+
| Output Parameters : -                                                      |
+----------------------------------------------------------------------------+
| Return value      :  0 : No error                                          |
|                     -1 : The handle parameter of the board is wrong        |
|                     -2 : Port number is not 1 or 2                         |
|                     -3 : Logic parameter error                             |
|                     -4 : Event mask parameter error                        |
|                     -5 : Interrupt routine not installed                   |
|                     -6 : More than 1 edge event declared for an AND logic  |
|                     -7 : The OR PRIORITY logic does not support any        |
|                          edge events                                       |
+----------------------------------------------------------------------------+
*/                     
				 
INT i_APCI1500_SetInputEventMask (BYTE    b_BoardHandle,
                                  BYTE    b_PortNbr,
                                  BYTE    b_Logik,
                                  CHAR*  pc_EventMask);
                                  
/*
+----------------------------------------------------------------------------+
| Function name   : INT i_APCI1500_StartInputEvent (BYTE      b_BoardHandle, |
|                                                   BYTE      b_PortNbr)     |
+----------------------------------------------------------------------------+
| Task              : Allows a port event.                                   |
+----------------------------------------------------------------------------+
| Input parameters  : BYTE    b_BoardHandle : Handle of the APCI-1500        |
|                     BYTE    b_PortNbr     : Number of the port             |
|                                             (1 or 2)                       |
+----------------------------------------------------------------------------+
| Output Parameters : -                                                      |
+----------------------------------------------------------------------------+
| Return Value      :  0 : No error                                          |
|                     -1 : The handle parameter of the board is wrong        |
|                     -2 : Port number parameter error                       |
|                     -3 : Event not initialized                             |
+----------------------------------------------------------------------------+
*/                                  
					      
INT i_APCI1500_StartInputEvent (BYTE       b_BoardHandle,
				BYTE       b_PortNbr);
				
/*
+----------------------------------------------------------------------------+
| Function name   : INT    i_APCI1500_StopInputEvent   (BYTE  b_BoardHandle, |
|                                                       BYTE  b_PortNbr)     |
+----------------------------------------------------------------------------+
| Task              : Inhibits one port event.                               |
+----------------------------------------------------------------------------+
| Input Parameters  : BYTE    b_BoardHandle : Handle of the APCI-1500        |
|                     BYTE    b_PortNbr     : Number of the input port       |
|                                             (1 or 2)                       |
+----------------------------------------------------------------------------+
| Output Parameters : -                                                      |
+----------------------------------------------------------------------------+
| Return Value      :  0 : No error                                          |
|                     -1 : The handle parameter of the board is wrong        |
|                     -2 : Port number parameter error                       |
|                     -3 : Event not initialized                             |
+----------------------------------------------------------------------------+
*/				
				           
INT i_APCI1500_StopInputEvent (BYTE  b_BoardHandle,
                               BYTE  b_PortNbr);
                               
/*
+----------------------------------------------------------------------------+
| Function name   : INT        i_APCI1500_SetBoardInterruptRoutine           |
|                                              (BYTE     b_BoardHandle       |
|                                 VOID (*usrHdl) (BYTE handle, BYTE intMask))|
+----------------------------------------------------------------------------+
| Task              : Initialises the interruptroutine for the APCI-1500     |
+----------------------------------------------------------------------------+
| Input parameters  : BYTE   b_BoardHandle :Handle of the APCI-1500          |
|                     VOID (*usrHdl) : User interrupt function               |
+----------------------------------------------------------------------------+
| Output Parameters : ---                                                    |                   
+----------------------------------------------------------------------------+
| Return Value      :  0 : No error                                          |
|                     -1 : The handle parameter of the board is wrong        |
+----------------------------------------------------------------------------+
*/                               
					   
INT i_APCI1500_SetBoardInterruptRoutine (BYTE    b_BoardHandle,
                                         VOID (*usrHdl) (BYTE handle, BYTE intMask));
                                         
/*
+----------------------------------------------------------------------------+
| Function name   : INT        i_APCI1500_ResetBoardInterruptRoutine         |
|                                              (BYTE     b_BoardHandle)      |
+----------------------------------------------------------------------------+
| Task              : Deinitialises the interruptroutine for the APCI-1500   |
+----------------------------------------------------------------------------+
| Input parameters  : BYTE   b_BoardHandle :Handle of the APCI-1500          |
+----------------------------------------------------------------------------+
| Output Parameters : ---                                                    |
+----------------------------------------------------------------------------+
| Return Value      :  0 : No error                                          |
|                     -1 : The handle parameter of the board is wrong        |
+----------------------------------------------------------------------------+
*/                                         

INT i_APCI1500_ResetBoardInterruptRoutine (BYTE    b_BoardHandle);

/*
+----------------------------------------------------------------------------+
| Function name   : INT        i_APCI1500_SearchAllAPCI1500 (VOID)           |
+----------------------------------------------------------------------------+
| Task              : Search for APCI-1500 on the PCI bus                    |
+----------------------------------------------------------------------------+
| Input parameters  : ---                                                    |
+----------------------------------------------------------------------------+
| Output Parameters : ---                                                    |
+----------------------------------------------------------------------------+
| Return Value      :  Number of boards, if 0 no boards                      |
+----------------------------------------------------------------------------+
*/
                                     
INT i_APCI1500_SearchAllAPCI1500 (VOID);                                      				   				           					      				 					    					    					    					   					   					   					  

#endif					      					      					      					      					   					   				     				    				    
