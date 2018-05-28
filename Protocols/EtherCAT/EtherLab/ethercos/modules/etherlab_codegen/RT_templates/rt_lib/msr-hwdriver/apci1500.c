/*
  +-----------------------------------------------------------------------------+
  | (C) ADDI-DATA GmbH         Dieselstrasse 3       D-77833 Ottersweier        |
  +-----------------------------------------------------------------------------+
  | Tel : +49 (0) 7223/9493-0     | email    : info@addi-data.com               |
  | Fax : +49 (0) 7223/9493-92    | Internet : http://www.addi-data.com         |
  +-------------------------------+---------------------------------------------+
  | Project     : APCI-1500       |     Compiler   : GCC                        |
  | Module name : apci1500.c      |     Version    : 2.96                       |
  +-------------------------------+---------------------------------------------+
  | Author      : E. Stolz        |     Date       : 21.06.2002                 |
  +-------------------------------+---------------------------------------------+
  | Description :                                                               |
  |               Includes the following functions for the APCI-1500            |
  |                                                                             |
  |                 - i_APCI1500_Read1DigitalInput ()                           |
  |                 - i_APCI1500_Read8DigitalInput ()                           |
  |                 - i_APCI1500_Read16DigitalInput ()                          |
  |                 - i_APCI1500_Set1DigitalOutputOn ()                         |
  |                 - i_APCI1500_Set1DigitalOutputOff ()                        |
  |                 - i_APCI1500_Set8DigitalOutputOn ()                         |
  |                 - i_APCI1500_Set8DigitalOutputOff ()                        |
  |                 - i_APCI1500_Set16DigitalOutputOn ()                        |
  |                 - i_APCI1500_Set16DigitalOutputOff ()                       |
  |                 - i_APCI1500_SetOutputMemoryOn ()                           |
  |                 - i_APCI1500_SetOutputMemoryOff ()                          |
  |                 - i_APCI1500_InitZilog ()                                   |
  |                 - i_APCI1500_InitTimerInputClock ()                         |
  |                 - i_APCI1500_InitTimerCounter1 ()                           |
  |                 - i_APCI1500_InitTimerCounter2 ()                           |
  |                 - i_APCI1500_InitWatchdogCounter3 ()                        |
  |                 - i_APCI1500_StartTimerCounter1 ()                          |
  |                 - i_APCI1500_StartTimerCounter2 ()                          |
  |                 - i_APCI1500_StartCounter3 ()                               |         
  |                 - i_APCI1500_StopTimerCounter1 ()                           |
  |                 - i_APCI1500_StopTimerCounter2 ()                           |
  |                 - i_APCI1500_StopCounter3 ()                                |
  |                 - i_APCI1500_TriggerTimerCounter1 ()                        |
  |                 - i_APCI1500_TriggerTimerCounter2  ()                       |
  |                 - i_APCI1500_TriggerCounter3 ()                             |
  |                 - i_APCI1500_TriggerWatchdog ()                             |
  |                 - i_APCI1500_ReadTimerCounter1 ()                           |
  |                 - i_APCI1500_ReadTimerCounter2 ()                           |
  |                 - i_APCI1500_ReadCounter3 ()                                |
  |                 - i_ConvertString ()                                        |
  |                 - i_APCI1500_SetInputEventMask ()                           |
  |                 - i_APCI1500_StartInputEvent ()                             |
  |                 - i_APCI1500_StopInputEvent ()                              |
  |                 - i_APCI1500_SetBoardInterruptRoutine ()                    |
  |                 - i_APCI1500_ResetBoardInterruptRoutine ()                  |
  |                 - i_APCI1500_SearchAllAPCI1500  ()                          |
  |                                                                             |      
  +-----------------------------------------------------------------------------+
  |                             UPDATES                                         |
  +----------+-----------+------------------------------------------------------+
  |   Date   |   Author  |          Description of updates             |Version |
  +----------+-----------+---------------------------------------------+--------+
  | 21.06.02 | E. Stolz	 | - Creation                                  |        |
  +----------*-----------*---------------------------------------------+--------+
  | 28.06.04 | J. Krauth | - Update i_APCI1500_SetBoardInterruptRoutine|        |
  |          |           |   it is yet possible to set a user interrupt|        | 
  |          |           |   with an other name.                       |01.00.01|
  |          |           | - Separation from board function and from   |        |
  |          |           |   module function.                          |        |
  |          |           | - Change structure names.                   |        |
  +----------*-----------*---------------------------------------------+--------+
*/

/*
+----------------------------------------------------------------------------+
|                               Included files                               |
+----------------------------------------------------------------------------+
*/

#ifndef MODULE
   #define MODULE
#endif 
   
#ifndef __KERNEL__
   #define __KERNEL__
#endif   


#include <apci1500.h>
#include <linux/fs.h>
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


#include <msr_rcsinfo.h>

RCS_ID("$Header: /home/hm/projekte/msr_messen_steuern_regeln/linux/kernel_space/simulator/rt/rt_lib/msr-hwdriver/RCS/apci1500.c,v 1.1 2005/06/14 12:34:23 hm Exp $");

str_APCI1500_DriverStructure s_APCI1500_DriverStruct; 

/*----------------------------------------------------------------------*/
/* This function is called if an interrupt occurs. Its the top half IRQ */
/*----------------------------------------------------------------------*/
void APCI1500_Interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
   BYTE    b_RegValue       = 0;
   BYTE    b_BoardHandle    = 0;
   BYTE    b_CheckInterrupt = 0;
   ULONG  ul_Status         = 0;

   str_APCI1500_BoardInformation *BoardInformation;
   
   str_APCI1500_GlobalInterruptInf *ps_GlobalInterruptInf = 
                                   (str_APCI1500_GlobalInterruptInf*) dev_id;
   
   for (b_BoardHandle = 0; 
        b_BoardHandle < s_APCI1500_DriverStruct.
	                b_NumberOfBoard; 
	b_BoardHandle ++)
   {
      /* Test if interrupt enabled */
      
      if (((ps_GlobalInterruptInf->dw_BoardHandleArray >> 
            b_BoardHandle) & 1) == 1)
      {
         /************************/
         /* Get the board struct */
         /************************/
      
         BoardInformation
 = &(s_APCI1500_DriverStruct.
                              s_BoardInformation[b_BoardHandle]);
      
         /**********************************************/
         /* Reads the status of the interrupt register */
         /**********************************************/
         ul_Status = inl ((UINT)BoardInformation->s_BaseInformation.
	                  ui_BaseAddress[0]+ 0x38);

		    
         /*************************************************/
         /* if status == 0x800000UL, an interrupt occured */		    
         /*************************************************/
         if ((ul_Status & 0x800000UL) == 0x800000UL)
         {
            do
            {
               b_CheckInterrupt = 0;
   
               /*******************************************/
               /* Tests if an interrupt occured on port 1 */
               /* Selects the port 1 command register     */ 
               /*******************************************/
               outb (APCI1500_RW_PORT_A_COMMAND_AND_STATUS,
	             (UINT)BoardInformation->s_BaseInformation.
	                                     ui_BaseAddress[1]+
		                             APCI1500_Z8536_CONTROL_REGISTER);
               /****************************************/
               /* Reads the command register of port 1 */          
               /****************************************/
               b_RegValue = inb ((UINT)BoardInformation->s_BaseInformation.
	                         ui_BaseAddress[1]+
		                 APCI1500_Z8536_CONTROL_REGISTER);
               /************************************************/
               /* If the RegValue == 0x60, an interrupt occurs */
               /************************************************/
               if ((b_RegValue & 0x60) == 0x60)
               {
                  /***************************************/
                  /* Selects the port 1 command register */ 
                  /***************************************/
                  outb (APCI1500_RW_PORT_A_COMMAND_AND_STATUS,
	                (UINT)BoardInformation->s_BaseInformation.
	                                        ui_BaseAddress[1]+
		                                APCI1500_Z8536_CONTROL_REGISTER);

                  /************************************************/
                  /* Prepares the variable to reset the interrupt */					
                  /************************************************/
	          b_RegValue = (b_RegValue & 0x0F) | 0x20;

                  /************************************************/
                  /* Writes the new RegValue to the board */
                  /************************************************/
                  outb (b_RegValue,
	                (UINT)BoardInformation->s_BaseInformation.
	                                        ui_BaseAddress[1]+
					        APCI1500_Z8536_CONTROL_REGISTER);

                  b_CheckInterrupt = b_CheckInterrupt | 0x1;
               }					

               /*******************************************/
               /* Tests if an interrupt occured on port 2 */
               /* Selects the port 2 command register     */ 
               /*******************************************/
               outb (APCI1500_RW_PORT_B_COMMAND_AND_STATUS,
	             (UINT)BoardInformation->s_BaseInformation.
	                                     ui_BaseAddress[1]+
		                             APCI1500_Z8536_CONTROL_REGISTER);
          
               /****************************************/
               /* Reads the command register of port 2 */          
               /****************************************/
               b_RegValue = inb ((UINT)BoardInformation->s_BaseInformation.
	                         ui_BaseAddress[1]+
		                 APCI1500_Z8536_CONTROL_REGISTER);

               /************************************************/
               /* If the RegValue == 0x60, an interrupt occurs */
               /************************************************/
               if ((b_RegValue & 0x60) == 0x60)
               { 
                  /***************************************/
                  /* Selects the port 2 command register */ 
                  /***************************************/
                  outb (APCI1500_RW_PORT_B_COMMAND_AND_STATUS,
	                (UINT)BoardInformation->s_BaseInformation.
	                                        ui_BaseAddress[1]+
		                                APCI1500_Z8536_CONTROL_REGISTER);
                  /************************************************/
                  /* Prepares the variable to reset the interrupt */					
                  /************************************************/
	          b_RegValue = (b_RegValue & 0x0F) | 0x20;

                  /****************************************/
                  /* Writes the new RegValue to the board */
                  /****************************************/
                  outb (b_RegValue,
	                (UINT)BoardInformation->s_BaseInformation.
	                                        ui_BaseAddress[1]+
					        APCI1500_Z8536_CONTROL_REGISTER);
                  if(BoardInformation-> s_EventInformation.
	  	                        b_ProcessEnableEvent2Status == APCI1500_ENABLE)
		  {
                     b_CheckInterrupt = b_CheckInterrupt | 0x2;
		  }
               }					

               /**********************************************/
               /* Tests if an interrupt occured from timer 1 */
               /* Selects the timer 1 command register       */ 
               /**********************************************/
               outb (APCI1500_RW_CPT_TMR1_CMD_STATUS,
	             (UINT)BoardInformation->s_BaseInformation.
	                                     ui_BaseAddress[1]+
		                             APCI1500_Z8536_CONTROL_REGISTER);
          
               /*****************************************/
               /* Reads the command register of timer 1 */          
               /*****************************************/
               b_RegValue = inb ((UINT)BoardInformation->s_BaseInformation.
	                         ui_BaseAddress[1]+
		                 APCI1500_Z8536_CONTROL_REGISTER);

               /************************************************/
               /* If the RegValue == 0x60, an interrupt occurs */
               /************************************************/
               if ((b_RegValue & 0x60) == 0x60)
               { 
                  outb (APCI1500_RW_CPT_TMR1_CMD_STATUS,
	                (UINT)BoardInformation->s_BaseInformation.
	                                        ui_BaseAddress[1]+
		                                APCI1500_Z8536_CONTROL_REGISTER);
      
                  /************************************************/
                  /* Prepares the variable to reset the interrupt */					
                  /************************************************/
	          b_RegValue = (b_RegValue & 0x0F) | 0x20;

                  /****************************************/
                  /* Writes the new RegValue to the board */
                  /****************************************/
                  outb (b_RegValue,
	                (UINT)BoardInformation->s_BaseInformation.
	                                        ui_BaseAddress[1]+
					        APCI1500_Z8536_CONTROL_REGISTER);

                  b_CheckInterrupt = b_CheckInterrupt | 0x4;
	 
               }

               /**********************************************/
               /* Tests if an interrupt occured from timer 2 */
               /* Selects the timer 2 command register       */ 
               /**********************************************/
               outb (APCI1500_RW_CPT_TMR2_CMD_STATUS,
	             (UINT)BoardInformation->s_BaseInformation.
	                                     ui_BaseAddress[1]+
		                             APCI1500_Z8536_CONTROL_REGISTER);
          
               /*****************************************/
               /* Reads the command register of timer 2 */          
               /*****************************************/
               b_RegValue = inb ((UINT)BoardInformation->s_BaseInformation.
	                         ui_BaseAddress[1]+
		                 APCI1500_Z8536_CONTROL_REGISTER);

               /************************************************/
               /* If the RegValue == 0x60, an interrupt occurs */
               /************************************************/
               if ((b_RegValue & 0x60) == 0x60)
               { 
                  outb (APCI1500_RW_CPT_TMR2_CMD_STATUS,
	                (UINT)BoardInformation->s_BaseInformation.
	                                        ui_BaseAddress[1]+
		                                APCI1500_Z8536_CONTROL_REGISTER);
      
                  /************************************************/
                  /* Prepares the variable to reset the interrupt */					
                  /************************************************/
	          b_RegValue = (b_RegValue & 0x0F) | 0x20;

                  /****************************************/
                  /* Writes the new RegValue to the board */
                  /****************************************/
                  outb (b_RegValue,
	                (UINT)BoardInformation->s_BaseInformation.
	                                        ui_BaseAddress[1]+
					        APCI1500_Z8536_CONTROL_REGISTER);

                  b_CheckInterrupt = b_CheckInterrupt | 0x8;
               }

               /**********************************************/
               /* Tests if an interrupt occured from timer 3 */
               /* Selects the timer 3 command register       */ 
               /**********************************************/
               outb (APCI1500_RW_CPT_TMR3_CMD_STATUS,
	             (UINT)BoardInformation->s_BaseInformation.
	                                     ui_BaseAddress[1]+
		                             APCI1500_Z8536_CONTROL_REGISTER);
          
               /*****************************************/
               /* Reads the command register of timer 3 */          
               /*****************************************/
               b_RegValue = inb ((UINT)BoardInformation->s_BaseInformation.
	                         ui_BaseAddress[1]+
		                 APCI1500_Z8536_CONTROL_REGISTER);

               /************************************************/
               /* If the RegValue == 0x60, an interrupt occurs */
               /************************************************/
               if ((b_RegValue & 0x60) == 0x60)
               { 
                  outb (APCI1500_RW_CPT_TMR3_CMD_STATUS,
	                (UINT)BoardInformation->s_BaseInformation.
	                                        ui_BaseAddress[1]+
		                                APCI1500_Z8536_CONTROL_REGISTER);
      
                  /************************************************/
                  /* Prepares the variable to reset the interrupt */					
                  /************************************************/
	          b_RegValue = (b_RegValue & 0x0F) | 0x20;

                  /****************************************/
                  /* Writes the new RegValue to the board */
                  /****************************************/
                  outb (b_RegValue,
	                (UINT)BoardInformation->s_BaseInformation.
	                                        ui_BaseAddress[1]+
					        APCI1500_Z8536_CONTROL_REGISTER);

                  b_CheckInterrupt = b_CheckInterrupt | 0x10;
	 
               }
               /*************************************/
               /* Tests if an interrupt has occured */
               /*************************************/
               if (b_CheckInterrupt != 0)
               {
               	printk ("An interrupt occured");
                  /************************************/
                  /* Calls the user interrupt routine */
                  /************************************/
                  //v_APCI1500_UserInterruptRoutine (b_BoardHandle, b_CheckInterrupt);
                  
                  if (s_APCI1500_DriverStruct.usrHdl)
                     s_APCI1500_DriverStruct.usrHdl (b_BoardHandle, b_CheckInterrupt);
               } //if (b_CheckInterrupt != 0)
            }
            while (b_CheckInterrupt != 0);
         }//(If Interrupt occured)
      }//(If Interrupt installed for the BoardHandle)
   }//(for all BoardHandles)
}

    


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

INT  i_APCI1500_Read1DigitalInput  (BYTE    b_BoardHandle,
				    BYTE    b_Channel,
				    PBYTE  pb_ChannelValue)
{
   INT   i_ReturnValue     = 0;
   UINT ui_PortValue       = 0;

   /*******************************************************/
   /* Gets the pointer to the board information structure */
   /*******************************************************/
   str_APCI1500_BoardInformation *BoardInformation = NULL; 
   
   BoardInformation = &(s_APCI1500_DriverStruct.
                        s_BoardInformation[b_BoardHandle]);

   if (b_BoardHandle < s_APCI1500_DriverStruct.b_NumberOfBoard)
   {
      /***************************/
      /* Test the channel number */
      /***************************/

      if (b_Channel >= 1 && b_Channel <= 16)
      {
         /*****************/
         /* Read the port */
         /*****************/

         ui_PortValue = inw (BoardInformation -> 
	                     s_BaseInformation.ui_BaseAddress[2]);

	 *pb_ChannelValue = (ui_PortValue >> (b_Channel - 1)) & 1;
      } // if (b_Channel >= 1 && b_Channel <= 16)
      else
      {
         /******************************************/
         /* Channel number is not between 1 and 16 */
         /******************************************/

         i_ReturnValue = -2;
      } // if (b_Channel >= 1 && b_Channel <= 16)

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

INT  i_APCI1500_Read8DigitalInput  (BYTE    b_BoardHandle,
				    BYTE    b_Port,
				    PBYTE  pb_PortValue)
{
   UINT ui_ReadValue;
   INT   i_ReturnValue  = 0;

   /*******************************************************/
   /* Gets the pointer to the board information structure */
   /*******************************************************/
   str_APCI1500_BoardInformation *BoardInformation = NULL; 
   
   BoardInformation = &(s_APCI1500_DriverStruct.
                        s_BoardInformation[b_BoardHandle]);

   /***************************/
   /* Test if board handle OK */
   /***************************/
   if (b_BoardHandle < s_APCI1500_DriverStruct.b_NumberOfBoard)
   {

      /************************/
      /* Test the port number */
      /************************/

      if (b_Port >= 1 && b_Port <= 2)
      {
         ui_ReadValue = inw (BoardInformation -> 
	                     s_BaseInformation.ui_BaseAddress[2]);

         *pb_PortValue = (BYTE) (ui_ReadValue >> (8 * (b_Port - 1)));
      } // if (b_Port >= 1 && b_Port <= 2)
      else
      {
         /*****************************/
         /* Port number is not 1 or 2 */
         /*****************************/

         i_ReturnValue = -2;
      } // if (b_Port >= 1 && b_Port <= 2)

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


/*
+----------------------------------------------------------------------------+
| Function name     : INT       i_APCI1500_Read16DigitalInput                |
|                                          (BYTE    b_BoardHandle,           |
|                                           PLONG   pl_InputValue)            |
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
| Output Parameters : PLONG   pl_InputValue  : State of the digital inputs of |
|                                             both ports (0 to 65535)        |
+----------------------------------------------------------------------------+
| Return Value      :  0 : No error                                          |
|                     -1 : The handle parameter of the board is wrong        |
+----------------------------------------------------------------------------+
*/

INT  i_APCI1500_Read16DigitalInput  (BYTE    b_BoardHandle,
				     PLONG  pl_InputValue)
{
   INT   i_ReturnValue  = 0;

   /*******************************************************/
   /* Gets the pointer to the board information structure */
   /*******************************************************/
   str_APCI1500_BoardInformation *BoardInformation = NULL; 
   
   BoardInformation = &(s_APCI1500_DriverStruct.
                        s_BoardInformation[b_BoardHandle]);

   /***************************/
   /* Test if board handle OK */
   /***************************/
   if (b_BoardHandle < s_APCI1500_DriverStruct.b_NumberOfBoard)
   {
      *pl_InputValue = 0;

      *pl_InputValue = inw (BoardInformation -> 
	                   s_BaseInformation.ui_BaseAddress[2]);

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

INT     i_APCI1500_Set1DigitalOutputOn    (BYTE  b_BoardHandle,
					   BYTE  b_Channel)
{
   INT   i_ReturnValue        = 0;
   UINT ui_DigitalOutputValue = 0;

   /*******************************************************/
   /* Gets the pointer to the board information structure */
   /*******************************************************/
   str_APCI1500_BoardInformation *BoardInformation = NULL; 
   
   BoardInformation = &(s_APCI1500_DriverStruct.
                        s_BoardInformation[b_BoardHandle]);

   /***************************/
   /* Test if board handle OK */
   /***************************/
   if (b_BoardHandle < s_APCI1500_DriverStruct.b_NumberOfBoard)
   {
      /***************************/
      /* Test the channel number */
      /***************************/

      if (b_Channel >= 1 && b_Channel <= 16)
      {
         /**************************************/
         /* Tests if the external memory is on */
         /**************************************/

         if (BoardInformation -> s_DigitalOutputInformation.
                                 b_OutputMemoryStatus == APCI1500_ENABLE)
         {
            BoardInformation ->
	    s_DigitalOutputInformation.
	    l_DigitalOutputRegister
 = BoardInformation ->
	                              s_DigitalOutputInformation.
	                              l_DigitalOutputRegister |
                                      (UINT) (1 << (b_Channel-1));

	    ui_DigitalOutputValue = BoardInformation ->
	                            s_DigitalOutputInformation.
                  	            l_DigitalOutputRegister
;
         }                              
	 else
	 {
	    BoardInformation ->
	    s_DigitalOutputInformation.
	    l_DigitalOutputRegister = 
	                        (UINT) (1U << (BYTE) ((BYTE) b_Channel - 1));
	 } 


	 outw (BoardInformation->s_DigitalOutputInformation.
	                          l_DigitalOutputRegister,
	       BoardInformation->s_BaseInformation.ui_BaseAddress[2] +
		                  APCI1500_DIGITAL_OUTPUT);


      } // if (b_Channel >= 1 && b_Channel <= 16)
      else
      {
         /******************************************/
         /* Channel number is not between 1 and 16 */
         /******************************************/

         i_ReturnValue = -2;
      } // if (b_Channel >= 1 && b_Channel <= 16)

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

INT     i_APCI1500_Set1DigitalOutputOff   (BYTE  b_BoardHandle,
					   BYTE  b_Channel)
{
   INT   i_ReturnValue     = 0;

   /*******************************************************/
   /* Gets the pointer to the board information structure */
   /*******************************************************/
   str_APCI1500_BoardInformation *BoardInformation = NULL; 
   
   BoardInformation = &(s_APCI1500_DriverStruct.
                        s_BoardInformation[b_BoardHandle]);

   /***************************/
   /* Test if board handle OK */
   /***************************/
   if (b_BoardHandle < s_APCI1500_DriverStruct.b_NumberOfBoard)
   {
      /****************************/
      /* Tests the channel number */
      /****************************/

      if (b_Channel >= 1 && b_Channel <= 16)
      {
         /**************************************/
         /* Tests if the external memory is on */
         /**************************************/

         if (BoardInformation -> s_DigitalOutputInformation.
                                 b_OutputMemoryStatus == APCI1500_ENABLE)
         {
            BoardInformation ->
	    s_DigitalOutputInformation.
	    l_DigitalOutputRegister = BoardInformation ->
	                              s_DigitalOutputInformation.
	                              l_DigitalOutputRegister &

                                      (0xFFFFU - (UINT) (1 << (b_Channel-1))); 

	    outw (BoardInformation->s_DigitalOutputInformation.
	                             l_DigitalOutputRegister,
	          BoardInformation->s_BaseInformation.ui_BaseAddress[2] +
		                     APCI1500_DIGITAL_OUTPUT);

         } 
	 else
	 {
	    /***************************/
	    /* Output memory is not on */
	    /***************************/

	    i_ReturnValue = -3;
	 } 
      } // if (b_Channel >= 1 && b_Channel <= 16)
      else
      {
         /******************************************/
         /* Channel number is not between 1 and 16 */
         /******************************************/

         i_ReturnValue = -2;
      } // if (b_Channel >= 1 && b_Channel <= 16)
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

INT     i_APCI1500_Set8DigitalOutputOn       (BYTE  b_BoardHandle,
					      BYTE  b_Port,
					      BYTE  b_Value)
{
   INT   i_ReturnValue        = 0;
   UINT ui_DigitalOutputValue = 0;

   /*******************************************************/
   /* Gets the pointer to the board information structure */
   /*******************************************************/
   str_APCI1500_BoardInformation *BoardInformation = NULL; 
   
   BoardInformation = &(s_APCI1500_DriverStruct.
                        s_BoardInformation[b_BoardHandle]);

   /***************************/
   /* Test if board handle OK */
   /***************************/
   if (b_BoardHandle < s_APCI1500_DriverStruct.b_NumberOfBoard)
   {
      /********************************/
      /* Tests the number of the port */
      /********************************/

      if (b_Port == 1 || b_Port == 2)
      {
         /**************************************/
         /* Tests if the external memory is on */
         /**************************************/

         if (BoardInformation -> s_DigitalOutputInformation.
                                 b_OutputMemoryStatus == APCI1500_ENABLE)
         {
            BoardInformation ->
            s_DigitalOutputInformation.
  	    l_DigitalOutputRegister
 = BoardInformation ->
	                              s_DigitalOutputInformation.
	                              l_DigitalOutputRegister |
                                      ((INT) b_Value << (8 * (b_Port - 1)));

	    ui_DigitalOutputValue = BoardInformation ->
	                            s_DigitalOutputInformation.
                 	            l_DigitalOutputRegister
;

         } 
	 else
	 {
	    BoardInformation ->
            s_DigitalOutputInformation.
  	    l_DigitalOutputRegister = 
	    ((UINT) ((UINT) (b_Value) << (8 * (b_Port - 1))));
	 } 

	 outw (BoardInformation->s_DigitalOutputInformation.
	                          l_DigitalOutputRegister,
	       BoardInformation->s_BaseInformation.ui_BaseAddress[2] +
		                  APCI1500_DIGITAL_OUTPUT);

      } // if (b_Port == 1 || b_Port == 2)
      else
      {
         /*****************************/
         /* Port number is not 1 or 2 */
         /*****************************/

         i_ReturnValue = -2;
      } // if (b_Port == 1 || b_Port == 2)

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

INT     i_APCI1500_Set8DigitalOutputOff      (BYTE  b_BoardHandle,
					      BYTE  b_Port,
					      BYTE  b_Value)
{
   INT   i_ReturnValue     = 0;

   /*******************************************************/
   /* Gets the pointer to the board information structure */
   /*******************************************************/
   str_APCI1500_BoardInformation *BoardInformation = NULL; 
   
   BoardInformation = &(s_APCI1500_DriverStruct.
                        s_BoardInformation[b_BoardHandle]);

   /***************************/
   /* Test if board handle OK */
   /***************************/
   if (b_BoardHandle < s_APCI1500_DriverStruct.b_NumberOfBoard)
   {
      /********************************/
      /* Tests the number of the port */
      /********************************/

      if (b_Port == 1 || b_Port == 2)
      {
         /**************************************/
         /* Tests if the external memory is on */
         /**************************************/

         if (BoardInformation -> s_DigitalOutputInformation.
                                 b_OutputMemoryStatus == APCI1500_ENABLE)
         {
            BoardInformation ->
	    s_DigitalOutputInformation.
	    l_DigitalOutputRegister = BoardInformation ->
	                              s_DigitalOutputInformation.
	                              l_DigitalOutputRegister &

                                      (0xFFFFU - (b_Value << (8 * (b_Port - 1))));

	    outw (BoardInformation->s_DigitalOutputInformation.
	                             l_DigitalOutputRegister,
	          BoardInformation->s_BaseInformation.ui_BaseAddress[2] +
		                     APCI1500_DIGITAL_OUTPUT);


	 } 
	 else
	 {
	    /***************************/
	    /* Output memory is not on */
	    /***************************/

	    i_ReturnValue = -3;
	 } 
      } // if (b_Port == 1 || b_Port == 2)
      else
      {
         /*****************************/
         /* Port number is not 1 or 2 */
         /*****************************/

         i_ReturnValue = -2;
      } // if (b_Port == 1 || b_Port == 2)
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

INT     i_APCI1500_Set16DigitalOutputOn      (BYTE  b_BoardHandle,
					      LONG  l_Value)
{
   INT   i_ReturnValue        = 0;
   UINT ui_DigitalOutputValue = 0;

   /*******************************************************/
   /* Gets the pointer to the board information structure */
   /*******************************************************/
   str_APCI1500_BoardInformation *BoardInformation = NULL; 
   
   BoardInformation = &(s_APCI1500_DriverStruct.
                        s_BoardInformation[b_BoardHandle]);

   /***************************/
   /* Test if board handle OK */
   /***************************/
   if (b_BoardHandle < s_APCI1500_DriverStruct.b_NumberOfBoard)
   {
      /**************************************/
      /* Tests if the external memory is on */
      /**************************************/

      if (BoardInformation -> s_DigitalOutputInformation.
                              b_OutputMemoryStatus == APCI1500_ENABLE)
      {
         BoardInformation ->
         s_DigitalOutputInformation.
  	 l_DigitalOutputRegister
 = BoardInformation ->
	                              s_DigitalOutputInformation.
	                              l_DigitalOutputRegister | (UINT) l_Value;

         ui_DigitalOutputValue = BoardInformation ->
	                         s_DigitalOutputInformation.
               	                 l_DigitalOutputRegister
;
      } 
      else
      {
	 BoardInformation ->
         s_DigitalOutputInformation.
  	 l_DigitalOutputRegister
 = (UINT) l_Value;
      } 

      outw (BoardInformation->s_DigitalOutputInformation.
                              l_DigitalOutputRegister,
	    BoardInformation->s_BaseInformation.ui_BaseAddress[2] +
	                      APCI1500_DIGITAL_OUTPUT);
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

INT     i_APCI1500_Set16DigitalOutputOff     (BYTE  b_BoardHandle,
					      LONG  l_Value)
{
   INT   i_ReturnValue     = 0;

   /*******************************************************/
   /* Gets the pointer to the board information structure */
   /*******************************************************/
   str_APCI1500_BoardInformation *BoardInformation = NULL; 
   
   BoardInformation = &(s_APCI1500_DriverStruct.
                        s_BoardInformation[b_BoardHandle]);

   /***************************/
   /* Test if board handle OK */
   /***************************/
   if (b_BoardHandle < s_APCI1500_DriverStruct.b_NumberOfBoard)
   {
      /**************************************/
      /* Tests if the external memory is on */
      /**************************************/

      if (BoardInformation -> s_DigitalOutputInformation.
                              b_OutputMemoryStatus == APCI1500_ENABLE)
      {
         BoardInformation ->
	 s_DigitalOutputInformation.
	 l_DigitalOutputRegister = BoardInformation ->
	                           s_DigitalOutputInformation.
	                           l_DigitalOutputRegister &
                                   (0xFFFFU - (UINT) l_Value);
	 outw (BoardInformation->s_DigitalOutputInformation.
	                         l_DigitalOutputRegister,
	       BoardInformation->s_BaseInformation.ui_BaseAddress[2] +
	                         APCI1500_DIGITAL_OUTPUT);
      } 
      else
      {
         /***************************/
         /* Output memory is not on */
         /***************************/

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
   return (i_ReturnValue);
}


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

INT     i_APCI1500_SetOutputMemoryOn      (BYTE  b_BoardHandle)
{
   INT   i_ReturnValue     = 0;

   /*******************************************************/
   /* Gets the pointer to the board information structure */
   /*******************************************************/
   str_APCI1500_BoardInformation *BoardInformation = NULL; 
   
   BoardInformation = &(s_APCI1500_DriverStruct.
                        s_BoardInformation[b_BoardHandle]);

   /***************************/
   /* Test if board handle OK */
   /***************************/
   if (b_BoardHandle < s_APCI1500_DriverStruct.b_NumberOfBoard)
   {

      /****************************/
      /* Enable the output memory */
      /****************************/

      BoardInformation -> s_DigitalOutputInformation.
                          b_OutputMemoryStatus = APCI1500_ENABLE;
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

INT     i_APCI1500_SetOutputMemoryOff     (BYTE  b_BoardHandle)
{
   INT   i_ReturnValue     = 0;

   /*******************************************************/
   /* Gets the pointer to the board information structure */
   /*******************************************************/
   str_APCI1500_BoardInformation *BoardInformation = NULL; 
   
   BoardInformation = &(s_APCI1500_DriverStruct.
                        s_BoardInformation[b_BoardHandle]);

   /***************************/
   /* Test if board handle OK */
   /***************************/
   if (b_BoardHandle < s_APCI1500_DriverStruct.b_NumberOfBoard)
   {

      /*****************************/
      /* Disable the output memory */
      /*****************************/

      BoardInformation -> s_DigitalOutputInformation.
                          b_OutputMemoryStatus = APCI1500_DISABLE;
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

INT     i_APCI1500_InitZilog     (BYTE  b_BoardHandle)
{
   INT   i_ReturnValue     = 0;
   BYTE  b_DummyRead       = 0;
   BYTE  b_ControlReg      = 0;

   /*******************************************************/
   /* Gets the pointer to the board information structure */
   /*******************************************************/
   str_APCI1500_BoardInformation *BoardInformation = NULL; 
   
   BoardInformation = &(s_APCI1500_DriverStruct.
                        s_BoardInformation[b_BoardHandle]);

   /***************************/
   /* Test if board handle OK */
   /***************************/
   if (b_BoardHandle < s_APCI1500_DriverStruct.b_NumberOfBoard)
   {
      /*-----------------------------*/
      /* Software reset of the ZiLOG */
      /*-----------------------------*/

      /* The following functions prepares the ZiLOG I/O ports and */
      /* the used registers for the right directions and contents */	    
      b_DummyRead = inb((UINT)BoardInformation -> 
                                 s_BaseInformation.
                                 ui_BaseAddress[1] +
                                 APCI1500_Z8536_CONTROL_REGISTER);
	    
      outb (0,
            (UINT)BoardInformation -> s_BaseInformation.
  	                               ui_BaseAddress[1]+
		                       APCI1500_Z8536_CONTROL_REGISTER);
       
      b_DummyRead = inb((UINT)BoardInformation -> 
                                 s_BaseInformation.
				 ui_BaseAddress[1] +
                                 APCI1500_Z8536_CONTROL_REGISTER);
	    
      outb (0,
            (UINT)BoardInformation-> s_BaseInformation.
 	                             ui_BaseAddress[1]+
		                     APCI1500_Z8536_CONTROL_REGISTER);
       
      outb (1,
            (UINT)BoardInformation-> s_BaseInformation.
  	                             ui_BaseAddress[1]+
                               	     APCI1500_Z8536_CONTROL_REGISTER);
       
      outb (0,
            (UINT)BoardInformation-> s_BaseInformation.
  	                             ui_BaseAddress[1]+
		                     APCI1500_Z8536_CONTROL_REGISTER);
	    
      /*---------------------------------------------------*/
      /* Selects the master configuration control register */
      /*---------------------------------------------------*/
	    
      outb (APCI1500_RW_MASTER_CONFIGURATION_CONTROL,
            (UINT)BoardInformation-> s_BaseInformation.
 	                             ui_BaseAddress[1]+
		                     APCI1500_Z8536_CONTROL_REGISTER);
	    
      outb (0xF4,
            (UINT)BoardInformation-> s_BaseInformation.
  	                             ui_BaseAddress[1]+
		                     APCI1500_Z8536_CONTROL_REGISTER);
	    
      /*---------------------------------------------------*/
      /* Selects the master configuration control register */
      /*---------------------------------------------------*/
	    
      outb (APCI1500_RW_MASTER_CONFIGURATION_CONTROL,
            (UINT)BoardInformation-> s_BaseInformation.
  	                             ui_BaseAddress[1]+
		                     APCI1500_Z8536_CONTROL_REGISTER);
       

      /*------------------------------------------------------------*/
      /* Reads configuration of port A, B, C and watchdog / counter */
      /*------------------------------------------------------------*/
      b_ControlReg = inb((UINT)BoardInformation-> 
                                     s_BaseInformation.
 				     ui_BaseAddress[1] +
	                             APCI1500_Z8536_CONTROL_REGISTER);
			       
      /*---------------------------*/
      /* Tests if board is present */
      /*---------------------------*/
      if (b_ControlReg == 0xF4)
      {
         /*---------------------------------------------------*/
         /* Selects the mode specification register of port A */
         /*---------------------------------------------------*/
         outb (APCI1500_RW_PORT_A_SPECIFICATION,
               (UINT)BoardInformation-> s_BaseInformation.
  	                                ui_BaseAddress[1]+
	   	                        APCI1500_Z8536_CONTROL_REGISTER);

         outb (0x10,
               (UINT)BoardInformation-> s_BaseInformation.
 	                                ui_BaseAddress[1]+
		                        APCI1500_Z8536_CONTROL_REGISTER);

         /*------------------------------------------*/
         /* Selects the data path polarity of port A */
         /*------------------------------------------*/
         outb (APCI1500_RW_PORT_A_DATA_PCITCH_POLARITY,
               (UINT)BoardInformation-> s_BaseInformation.
  	                                ui_BaseAddress[1]+
		                        APCI1500_Z8536_CONTROL_REGISTER);

         /*------------------------------*/
         /* High level on port A means 1 */
         /*------------------------------*/
         outb (0xFF,
               (UINT)BoardInformation-> s_BaseInformation.
 	                                ui_BaseAddress[1]+
		                        APCI1500_Z8536_CONTROL_REGISTER);

         /*-----------------------------------------------*/
	 /* Selects the data direction register of port A */
	 /*-----------------------------------------------*/
         outb (APCI1500_RW_PORT_A_DATA_DIRECTION,
	       (UINT)BoardInformation-> s_BaseInformation.
	                                ui_BaseAddress[1]+
	                                APCI1500_Z8536_CONTROL_REGISTER);

	 /*-----------------------------*/
	 /* All bits are used as inputs */
	 /*-----------------------------*/
         outb (0xFF,
	       (UINT)BoardInformation-> s_BaseInformation.
	                                ui_BaseAddress[1]+
	                                APCI1500_Z8536_CONTROL_REGISTER);

	 /*---------------------------------------------------*/
	 /* Selects the command and status register of port A */
	 /*---------------------------------------------------*/
         outb (APCI1500_RW_PORT_A_COMMAND_AND_STATUS,
	       (UINT)BoardInformation-> s_BaseInformation.
	                                ui_BaseAddress[1]+
	                                APCI1500_Z8536_CONTROL_REGISTER);

	 /*-------------------*/
	 /* Delete IP and IUS */
	 /*-------------------*/
         outb (0x20,
	       (UINT)BoardInformation-> s_BaseInformation.
	                                ui_BaseAddress[1]+
	                                APCI1500_Z8536_CONTROL_REGISTER);

	 /*---------------------------------------------------*/
	 /* Selects the command and status register of port A */
	 /*---------------------------------------------------*/
         outb (APCI1500_RW_PORT_A_COMMAND_AND_STATUS,
	       (UINT)BoardInformation-> s_BaseInformation.
	                                ui_BaseAddress[1]+
	                                APCI1500_Z8536_CONTROL_REGISTER);

         outb (0xE0,
	       (UINT)BoardInformation-> s_BaseInformation.
	                                ui_BaseAddress[1]+
	                                APCI1500_Z8536_CONTROL_REGISTER);

	 /*--------------------------------------------------------*/
	 /* Selects the handshake specification register of port A */
	 /*--------------------------------------------------------*/
         outb (APCI1500_RW_PORT_A_HANDSHAKE_SPECIFICATION,
	       (UINT)BoardInformation-> s_BaseInformation.
	                                ui_BaseAddress[1]+
	                                APCI1500_Z8536_CONTROL_REGISTER);
         /*----------------------*/
         /* Deletes the register */
         /*----------------------*/
         outb (0,
               (UINT)BoardInformation-> s_BaseInformation.
   	                                ui_BaseAddress[1]+
		                        APCI1500_Z8536_CONTROL_REGISTER);

         /*---------------------------------------------------*/
	 /* Selects the mode specification register of port B */
	 /*---------------------------------------------------*/
         outb (APCI1500_RW_PORT_B_SPECIFICATION,
	       (UINT)BoardInformation-> s_BaseInformation.
	                                ui_BaseAddress[1]+
	                                APCI1500_Z8536_CONTROL_REGISTER);

         outb (0x10,
	       (UINT)BoardInformation-> s_BaseInformation.
	                                ui_BaseAddress[1]+
	                                APCI1500_Z8536_CONTROL_REGISTER);

	 /*------------------------------------------*/
	 /* Selects the data path polarity of port B */
	 /*------------------------------------------*/
         outb (APCI1500_RW_PORT_B_DATA_PCITCH_POLARITY,
	       (UINT)BoardInformation-> s_BaseInformation.
	                                ui_BaseAddress[1]+
	                                APCI1500_Z8536_CONTROL_REGISTER);

	 /*------------------------------*/
	 /* High level on port B means 1 */
	 /*------------------------------*/
         outb (0x7F,
	       (UINT)BoardInformation-> s_BaseInformation.
	                                ui_BaseAddress[1]+
	                                APCI1500_Z8536_CONTROL_REGISTER);

	 /*-----------------------------------------------*/
	 /* Selects the data direction register of port B */
	 /*-----------------------------------------------*/
         outb (APCI1500_RW_PORT_B_DATA_DIRECTION,
	       (UINT)BoardInformation-> s_BaseInformation.
	                                ui_BaseAddress[1]+
	                                APCI1500_Z8536_CONTROL_REGISTER);

	 /*-----------------------------*/
	 /* All bits are used as inputs */
	 /*-----------------------------*/
         outb (0xFF,
	       (UINT)BoardInformation-> s_BaseInformation.
	                                ui_BaseAddress[1]+
	                                APCI1500_Z8536_CONTROL_REGISTER);

	 /*---------------------------------------------------*/
	 /* Selects the command and status register of port B */
	 /*---------------------------------------------------*/
         outb (APCI1500_RW_PORT_B_COMMAND_AND_STATUS,
	       (UINT)BoardInformation-> s_BaseInformation.
	                                ui_BaseAddress[1]+
	                                APCI1500_Z8536_CONTROL_REGISTER);

	 /*-------------------*/
	 /* Delete IP and IUS */
	 /*-------------------*/
         outb (0x20,
	       (UINT)BoardInformation-> s_BaseInformation.
	                                ui_BaseAddress[1]+
	                                APCI1500_Z8536_CONTROL_REGISTER);

         /*---------------------------------------------------*/
	 /* Selects the command and status register of port B */
	 /*---------------------------------------------------*/
         outb (APCI1500_RW_PORT_B_COMMAND_AND_STATUS,
	       (UINT)BoardInformation-> s_BaseInformation.
	                                ui_BaseAddress[1]+
	                                APCI1500_Z8536_CONTROL_REGISTER);

         outb (0xE0,
	       (UINT)BoardInformation-> s_BaseInformation.
	                                ui_BaseAddress[1]+
	                                APCI1500_Z8536_CONTROL_REGISTER);

	 /*--------------------------------------------------------*/
	 /* Selects the handshake specification register of port A */
	 /*--------------------------------------------------------*/
         outb (APCI1500_RW_PORT_B_HANDSHAKE_SPECIFICATION,
	       (UINT)BoardInformation-> s_BaseInformation.
	                                ui_BaseAddress[1]+
	                                APCI1500_Z8536_CONTROL_REGISTER);

	 /*----------------------*/
	 /* Deletes the register */
	 /*----------------------*/
         outb (0,
	       (UINT)BoardInformation-> s_BaseInformation.
	                                ui_BaseAddress[1]+
	                                APCI1500_Z8536_CONTROL_REGISTER);

	 /*------------------------------------------*/
	 /* Selects the data path polarity of port B */
	 /*------------------------------------------*/
         outb (APCI1500_RW_PORT_C_DATA_PCITCH_POLARITY,
	       (UINT)BoardInformation-> s_BaseInformation.
	                                ui_BaseAddress[1]+
	                                APCI1500_Z8536_CONTROL_REGISTER);

	 /*------------------------------*/
	 /* High level on port C means 1 */
	 /*------------------------------*/
         outb (0x9,
	       (UINT)BoardInformation-> s_BaseInformation.
	                                ui_BaseAddress[1]+
	                                APCI1500_Z8536_CONTROL_REGISTER);

	 /*-----------------------------------------------*/
	 /* Selects the data direction register of port C */
	 /*-----------------------------------------------*/
         outb (APCI1500_RW_PORT_C_DATA_DIRECTION,
	       (UINT)BoardInformation-> s_BaseInformation.
	                                ui_BaseAddress[1]+
	                                APCI1500_Z8536_CONTROL_REGISTER);

	 /*----------------------------------------------*/
	 /* All bits are used as inputs except channel 1 */
	 /*----------------------------------------------*/
         outb (0x0E,
	       (UINT)BoardInformation-> s_BaseInformation.
	                                ui_BaseAddress[1]+
	                                APCI1500_Z8536_CONTROL_REGISTER);

	 /*--------------------------------------------*/
	 /* Selects the special I/O register of port C */
	 /*--------------------------------------------*/
         outb (APCI1500_RW_PORT_C_SPECIAL_IO_CONTROL,
	       (UINT)BoardInformation-> s_BaseInformation.
	                                ui_BaseAddress[1]+
	                                APCI1500_Z8536_CONTROL_REGISTER);

         /*----------------------*/
	 /* Deletes the register */
	 /*----------------------*/
         outb (0,
	       (UINT)BoardInformation-> s_BaseInformation.
	                                ui_BaseAddress[1]+
	                                APCI1500_Z8536_CONTROL_REGISTER);

	 /*----------------------------------------------------*/
	 /* Selects the command and status register of timer 1 */
	 /*----------------------------------------------------*/
         outb (APCI1500_RW_CPT_TMR1_CMD_STATUS,
	       (UINT)BoardInformation-> s_BaseInformation.
	                                ui_BaseAddress[1]+
	                                APCI1500_Z8536_CONTROL_REGISTER);

	 /*--------------------*/
	 /* Deletes IP and IUS */
	 /*--------------------*/
         outb (0x20,
	       (UINT)BoardInformation-> s_BaseInformation.
	                                ui_BaseAddress[1]+
	                                APCI1500_Z8536_CONTROL_REGISTER);

         /*----------------------------------------------------*/
	 /* Selects the command and status register of timer 1 */
	 /*----------------------------------------------------*/
         outb (APCI1500_RW_CPT_TMR1_CMD_STATUS,
	       (UINT)BoardInformation-> s_BaseInformation.
	                                ui_BaseAddress[1]+
	                                APCI1500_Z8536_CONTROL_REGISTER);

         outb (0xE0,
	       (UINT)BoardInformation-> s_BaseInformation.
	                                ui_BaseAddress[1]+
	                                APCI1500_Z8536_CONTROL_REGISTER);

	 /*----------------------------------------------------*/
	 /* Selects the command and status register of timer 2 */
	 /*----------------------------------------------------*/
         outb (APCI1500_RW_CPT_TMR2_CMD_STATUS,
	       (UINT)BoardInformation-> s_BaseInformation.
	                                ui_BaseAddress[1]+
	                                APCI1500_Z8536_CONTROL_REGISTER);

         /*--------------------*/
	 /* Deletes IP and IUS */
	 /*--------------------*/
         outb (0x20,
	       (UINT)BoardInformation-> s_BaseInformation.
	                                ui_BaseAddress[1]+
	                                APCI1500_Z8536_CONTROL_REGISTER);

	 /*----------------------------------------------------*/
	 /* Selects the command and status register of timer 2 */
	 /*----------------------------------------------------*/
         outb (APCI1500_RW_CPT_TMR2_CMD_STATUS,
	       (UINT)BoardInformation-> s_BaseInformation.
	                                ui_BaseAddress[1]+
	                                APCI1500_Z8536_CONTROL_REGISTER);

         outb (0xE0,
	       (UINT)BoardInformation-> s_BaseInformation.
	                                ui_BaseAddress[1]+
	                                APCI1500_Z8536_CONTROL_REGISTER);

	 /*----------------------------------------------------*/
	 /* Selects the command and status register of timer 3 */
	 /*----------------------------------------------------*/
         outb (APCI1500_RW_CPT_TMR3_CMD_STATUS,
	       (UINT)BoardInformation-> s_BaseInformation.
	                                ui_BaseAddress[1]+
	                                APCI1500_Z8536_CONTROL_REGISTER);

	 /*--------------------*/
	 /* Deletes IP and IUS */
	 /*--------------------*/
         outb (0x20,
	       (UINT)BoardInformation-> s_BaseInformation.
	                                ui_BaseAddress[1]+
	                                APCI1500_Z8536_CONTROL_REGISTER);

	 /*----------------------------------------------------*/
	 /* Selects the command and status register of timer 3 */
	 /*----------------------------------------------------*/
         outb (APCI1500_RW_CPT_TMR3_CMD_STATUS,
	       (UINT)BoardInformation-> s_BaseInformation.
	                                ui_BaseAddress[1]+
	                                APCI1500_Z8536_CONTROL_REGISTER);

         outb (0xE0,
	       (UINT)BoardInformation-> s_BaseInformation.
	                                ui_BaseAddress[1]+
	                                APCI1500_Z8536_CONTROL_REGISTER);

	 /*-----------------------------------------------*/
	 /* Selects the master interrupt control register */
	 /*-----------------------------------------------*/
         outb (APCI1500_RW_MASTER_INTERRUPT_CONTROL,
	       (UINT)BoardInformation-> s_BaseInformation.
	                                ui_BaseAddress[1]+
	                                APCI1500_Z8536_CONTROL_REGISTER);

         outb (0,
	       (UINT)BoardInformation-> s_BaseInformation.
	                                ui_BaseAddress[1]+
	                                APCI1500_Z8536_CONTROL_REGISTER);
      }
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

INT     i_APCI1500_InitTimerInputClock   (BYTE  b_BoardHandle,
					  BYTE  b_InputClockSelect)
{
   INT   i_ReturnValue         = 0;

   /*******************************************************/
   /* Gets the pointer to the board information structure */
   /*******************************************************/
   str_APCI1500_BoardInformation *BoardInformation = NULL; 
   
   BoardInformation = &(s_APCI1500_DriverStruct.
                        s_BoardInformation[b_BoardHandle]);

   /***************************/
   /* Test if board handle OK */
   /***************************/
   if (b_BoardHandle < s_APCI1500_DriverStruct.b_NumberOfBoard)
   {
      /****************************/
      /* Test the clock selection */
      /****************************/

      if ((b_InputClockSelect == APCI1500_115_KHZ) ||
          (b_InputClockSelect == APCI1500_3_6_KHZ) ||
          (b_InputClockSelect == APCI1500_1_8_KHZ))
      {
         /****************************/
	 /* Test if 115 Khz selected */
	 /****************************/

	 if (b_InputClockSelect == APCI1500_115_KHZ)
	 {
	    outw (0,
	          BoardInformation->s_BaseInformation.ui_BaseAddress[2]);
         } // if (b_InputClockSelect == APCI1500_115_KHZ)

         /****************************/
         /* Test if 3.6 Khz selected */
         /****************************/

         if (b_InputClockSelect == APCI1500_3_6_KHZ)
         {
	    outw (1,
	          BoardInformation->s_BaseInformation.ui_BaseAddress[2]);
         } // if (b_InputClockSelect == APCI1500_3_6_KHZ)

         /****************************/
         /* Test if 1.8 Khz selected */
         /****************************/

         if (b_InputClockSelect == APCI1500_1_8_KHZ)
         {
	    outw (2,
	          BoardInformation->s_BaseInformation.ui_BaseAddress[2]);
         } // if (b_InputClockSelect == APCI1500_1_8_KHZ)
      } // if ((b_InputClockSelect == APCI1500_115_KHZ) || (b_InputClockSelect == APCI1500_3_6_KHZ) || (b_InputClockSelect == APCI1500_1_8_KHZ))
      else
      {
         /*********************/
         /* Input clock Error */
         /*********************/

         i_ReturnValue = -2;
      } // if ((b_InputClockSelect == APCI1500_115_KHZ) || (b_InputClockSelect == APCI1500_3_6_KHZ) || (b_InputClockSelect == APCI1500_1_8_KHZ))

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

INT     i_APCI1500_InitTimerCounter1      (BYTE   b_BoardHandle,
					   BYTE   b_CounterOrTimerSelect,
					   LONG   l_ReloadValue,
					   BYTE   b_ContinuousOrSingleCycleSelect,
					   BYTE   b_InterruptHandling)
{
   INT   i_ReturnValue         = 0;
   BYTE  b_TimerCounterMode    = 0;
   BYTE  b_MasterConfiguration = 0;

   /*******************************************************/
   /* Gets the pointer to the board information structure */
   /*******************************************************/
   str_APCI1500_BoardInformation *BoardInformation = NULL; 
   
   BoardInformation = &(s_APCI1500_DriverStruct.
                        s_BoardInformation[b_BoardHandle]);

   /***************************/
   /* Test if board handle OK */
   /***************************/
   if (b_BoardHandle < s_APCI1500_DriverStruct.b_NumberOfBoard)
   {
      /*******************************/
      /* Test the counter/timer mode */
      /*******************************/


      if ((b_CounterOrTimerSelect == APCI1500_TIMER)  ||
          (b_CounterOrTimerSelect == APCI1500_COUNTER))
      {
         /**********************************/
         /* Tests the interrupt parameter  */
         /**********************************/

 
         if ((b_InterruptHandling == APCI1500_ENABLE) ||
             (b_InterruptHandling == APCI1500_DISABLE))
         {
            /*******************************/
            /* Tests the cycle parameter   */
            /*******************************/

            if ((b_ContinuousOrSingleCycleSelect == APCI1500_SINGLE) ||
      	        (b_ContinuousOrSingleCycleSelect == APCI1500_CONTINUOUS))
	    {
	       /*************************/
	       /* Test the reload value */
	       /*************************/

	       if ((l_ReloadValue >= 0) && (l_ReloadValue <= 65535U))
	       {
	          /***************************/
	          /* Test the interrupt flag */
	          /***************************/

	          if ((b_InterruptHandling == APCI1500_DISABLE) ||
	   	      ((b_InterruptHandling == APCI1500_ENABLE) && (((BoardInformation-> s_InterruptInformation.
									                 b_InterruptInitialised) & 1) == 1)))
		  {
  	             /******************************/
  	             /* Set the timer/counter mode */
	             /******************************/

	             b_TimerCounterMode = (BYTE) (b_CounterOrTimerSelect |
				                  b_ContinuousOrSingleCycleSelect |
				                  7);

     	             /************************************************/
 	             /* Selects the mode register of timer/counter 1 */
	             /************************************************/

	             outb (APCI1500_RW_CPT_TMR1_MODE_SPECIFICATION,
		           BoardInformation->s_BaseInformation.
			                     ui_BaseAddress[1] + 
					     APCI1500_Z8536_CONTROL_REGISTER);

	             /***********************/
	             /* Writes the new mode */
 	             /***********************/

	             outb (b_TimerCounterMode,
		           BoardInformation->s_BaseInformation.
			                     ui_BaseAddress[1] + 
					     APCI1500_Z8536_CONTROL_REGISTER);

	             /****************************************************/
	             /* Selects the constant register of timer/counter 1 */
	             /****************************************************/

	             outb (APCI1500_RW_CPT_TMR1_TIME_CST_LOW,
		           BoardInformation->s_BaseInformation.
			                     ui_BaseAddress[1] + 
					     APCI1500_Z8536_CONTROL_REGISTER);

	             /*************************/
	             /* Writes the low value  */
	             /*************************/

	             outb ((BYTE) l_ReloadValue,
		           BoardInformation->s_BaseInformation.
			                     ui_BaseAddress[1] + 
					     APCI1500_Z8536_CONTROL_REGISTER);

	             /****************************************************/
	             /* Selects the constant register of timer/counter 1 */
	             /****************************************************/

	             outb (APCI1500_RW_CPT_TMR1_TIME_CST_HIGH,
		           BoardInformation->s_BaseInformation.
			                     ui_BaseAddress[1] + 
					     APCI1500_Z8536_CONTROL_REGISTER);

	             /**************************/
	             /* Writes the high value  */
	             /**************************/

	             outb ((BYTE) (l_ReloadValue >> 8),
		           BoardInformation->s_BaseInformation.
			                     ui_BaseAddress[1] + 
					     APCI1500_Z8536_CONTROL_REGISTER);

	             /*********************************************/
	             /* Selects the master configuration register */
	             /*********************************************/

	             outb (APCI1500_RW_MASTER_CONFIGURATION_CONTROL,
		           BoardInformation->s_BaseInformation.
			                     ui_BaseAddress[1] + 
					     APCI1500_Z8536_CONTROL_REGISTER);

	             /**********************/
	             /* Reads the register */
	             /**********************/

  	             b_MasterConfiguration = inb (BoardInformation->
		                                  s_BaseInformation.
			                          ui_BaseAddress[1] + 
					          APCI1500_Z8536_CONTROL_REGISTER);

	             /********************************************************/
	             /* Enables timer/counter 1 and triggers timer/counter 1 */
	             /********************************************************/

	             b_MasterConfiguration = (BYTE) (b_MasterConfiguration | 0x40);

	             /*********************************************/
	             /* Selects the master configuration register */
	             /*********************************************/

	             outb (APCI1500_RW_MASTER_CONFIGURATION_CONTROL,
		           BoardInformation->s_BaseInformation.
			                     ui_BaseAddress[1] + 
					     APCI1500_Z8536_CONTROL_REGISTER);

	             /********************************/
	             /* Writes the new configuration  */
	             /********************************/

	             outb (b_MasterConfiguration
,
		           BoardInformation->s_BaseInformation.
			                     ui_BaseAddress[1] + 
					     APCI1500_Z8536_CONTROL_REGISTER);

	             /****************************************/
	             /* Selects the commands register of     */
	             /* timer/counter 1                      */
	             /****************************************/

	             outb (APCI1500_RW_CPT_TMR1_CMD_STATUS,
		           BoardInformation->s_BaseInformation.
			                     ui_BaseAddress[1] + 
					     APCI1500_Z8536_CONTROL_REGISTER);

	             /***************************/
	             /* Disable timer/counter 1 */
	             /***************************/

	             outb (0x0,
		           BoardInformation->s_BaseInformation.
			                     ui_BaseAddress[1] + 
					     APCI1500_Z8536_CONTROL_REGISTER);

	             /****************************************/
	             /* Selects the commands register of     */
	             /* timer/counter 1                      */
	             /****************************************/

	             outb (APCI1500_RW_CPT_TMR1_CMD_STATUS,
		           BoardInformation->s_BaseInformation.
			                     ui_BaseAddress[1] + 
					     APCI1500_Z8536_CONTROL_REGISTER);

 	             /***************************/
	             /* Trigger timer/counter 1 */
	             /***************************/

	             outb (0x2,
		           BoardInformation->s_BaseInformation.
			                     ui_BaseAddress[1] + 
					     APCI1500_Z8536_CONTROL_REGISTER);

	             /*******************************************/
	             /* Stores the selection (timer or counter) */
	             /*******************************************/

	             BoardInformation-> s_TimerCounterInformation.
		                        b_TimerCounter1Select = 
					(BYTE) b_CounterOrTimerSelect;

	             BoardInformation-> s_TimerCounterInformation.
		                        b_TimerCounter1Init =
					APCI1500_ENABLE;
					 
	             /**********************************/
	             /* Stores the interrupt selection */
	             /**********************************/
		     if (b_InterruptHandling == APCI1500_ENABLE)
		     {
	                BoardInformation-> s_TimerCounterInformation.
		                           b_TimerCounter1InterruptEnabled =
					   APCI1500_ENABLE; 
		     }
		     else
		     {
	                BoardInformation-> s_TimerCounterInformation.
		                           b_TimerCounter1InterruptEnabled =
					   APCI1500_DISABLE; 
		     }

		  } // if ((b_InterruptHandling == APCI1500_DISABLE) || ((b_InterruptHandling == APCI1500_ENABLE) 
		  else
		  {
		     /****************************************/
		     /* User interrupt routine not installed */
		     /****************************************/

		     i_ReturnValue = -4;
		  } // if ((b_InterruptHandling == APCI1500_DISABLE) || ((b_InterruptHandling == APCI1500_ENABLE) 
	       } // if ((l_ReloadValue >= 0) && (l_ReloadValue <= 65535U))
	       else
	       {
	          /********************************/
	          /* Reload value selection Error */
	          /********************************/

    	          i_ReturnValue = -6;
	       } // if ((l_ReloadValue >= 0) && (l_ReloadValue <= 65535U))
	    } // if ((b_ContinuousOrSingleCycleSelect == APCI1500_SINGLE) || (b_ContinuousOrSingleCycleSelect == APCI1500_CONTINUOUS))
	    else
	    {
	       /********************************/
	       /* The cycle parameter is wrong */
	       /********************************/

	       i_ReturnValue = -5;
	    } // if ((b_ContinuousOrSingleCycleSelect == APCI1500_SINGLE) || (b_ContinuousOrSingleCycleSelect == APCI1500_CONTINUOUS))
	 } // if ((b_InterruptHandling == APCI1500_ENABLE) || (b_InterruptHandling == APCI1500_DISABLE))
	 else
	 {
	    /*************************************/
	    /* Error with th interrupt selection */
	    /*************************************/

	    i_ReturnValue = -3;
	 } // if ((b_InterruptHandling == APCI1500_ENABLE) || (b_InterruptHandling == APCI1500_DISABLE))
      } // if ((b_CounterOrTimerSelect == APCI1500_TIMER) || (b_CounterOrTimerSelect == APCI1500_COUNTER))
      else
      {
         /*****************************************************************/
         /* The parameter for selecting the counter or the timer is wrong */
         /*****************************************************************/

         i_ReturnValue = -2;
      } // if ((b_CounterOrTimerSelect == APCI1500_TIMER) || (b_CounterOrTimerSelect == APCI1500_COUNTER))

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

INT     i_APCI1500_InitTimerCounter2      (BYTE   b_BoardHandle,
					   BYTE   b_CounterOrTimerSelect,
					   LONG   l_ReloadValue,
					   BYTE   b_ContinuousOrSingleCycleSelect,
					   BYTE   b_HardwareOrSoftwareTriggerSelect,
					   BYTE   b_HardwareOrSoftwareGateSelect,
					   BYTE   b_InterruptHandling)
{
   INT   i_ReturnValue         = 0;
   BYTE  b_TimerCounterMode    = 0;
   BYTE  b_MasterConfiguration = 0;

   /*******************************************************/
   /* Gets the pointer to the board information structure */
   /*******************************************************/
   str_APCI1500_BoardInformation *BoardInformation = NULL; 
   
   BoardInformation = &(s_APCI1500_DriverStruct.
                        s_BoardInformation[b_BoardHandle]);

   /***************************/
   /* Test if board handle OK */
   /***************************/
   if (b_BoardHandle < s_APCI1500_DriverStruct.b_NumberOfBoard)
   {
      /*******************************/
      /* Test the counter/timer mode */
      /*******************************/


      if ((b_CounterOrTimerSelect == APCI1500_TIMER)  ||
          (b_CounterOrTimerSelect == APCI1500_COUNTER))
      {
         /**********************************/
         /* Tests the interrupt parameter  */
         /**********************************/

 
         if ((b_InterruptHandling == APCI1500_ENABLE) ||
             (b_InterruptHandling == APCI1500_DISABLE))
         {
            /*******************************/
            /* Tests the cycle parameter   */
            /*******************************/

            if ((b_ContinuousOrSingleCycleSelect == APCI1500_SINGLE) ||
      	        (b_ContinuousOrSingleCycleSelect == APCI1500_CONTINUOUS))
	    {
 	       /*************************/
	       /* Test the reload value */
	       /*************************/

	       if ((l_ReloadValue >= 0) && (l_ReloadValue <= 65535U))
	       {
	          /*****************************************/
	          /* Test the interrupt function installed */
	          /*****************************************/

	          if ((b_InterruptHandling == APCI1500_DISABLE) ||
		      ((b_InterruptHandling == APCI1500_ENABLE) && (((BoardInformation-> s_InterruptInformation.
									                 b_InterruptInitialised) & 1) == 1)))
		  {
		     /*******************************/
		     /* Tests the trigger parameter */
		     /*******************************/

		     if ((b_HardwareOrSoftwareTriggerSelect == APCI1500_SOFTWARE_TRIGGER) ||
		         (b_HardwareOrSoftwareTriggerSelect == APCI1500_HARDWARE_TRIGGER))
		     {
		        /****************************/
		        /* Tests the gate parameter */
		        /****************************/

		        if ((b_HardwareOrSoftwareGateSelect == APCI1500_SOFTWARE_GATE) ||
		            (b_HardwareOrSoftwareGateSelect == APCI1500_HARDWARE_GATE))
		        {

	                   b_TimerCounterMode = (BYTE) (b_CounterOrTimerSelect |
				                        b_HardwareOrSoftwareGateSelect |
				                        b_ContinuousOrSingleCycleSelect |
				                        b_HardwareOrSoftwareTriggerSelect |
				                        7);
			
     	                   /************************************************/
 	                   /* Selects the mode register of timer/counter 2 */
	                   /************************************************/

	                   outb (APCI1500_RW_CPT_TMR2_MODE_SPECIFICATION,
		                 BoardInformation->s_BaseInformation.
			                           ui_BaseAddress[1] + 
					           APCI1500_Z8536_CONTROL_REGISTER);

	                   /***********************/
	                   /* Writes the new mode */
 	                   /***********************/

	                   outb (b_TimerCounterMode,
		                 BoardInformation->s_BaseInformation.
			                           ui_BaseAddress[1] + 
					           APCI1500_Z8536_CONTROL_REGISTER);

	                   /****************************************************/
	                   /* Selects the constant register of timer/counter 2 */
	                   /****************************************************/

	                   outb (APCI1500_RW_CPT_TMR2_TIME_CST_LOW,
		                 BoardInformation->s_BaseInformation.
			                           ui_BaseAddress[1] + 
					           APCI1500_Z8536_CONTROL_REGISTER);

	                   /*************************/
	                   /* Writes the low value  */
	                   /*************************/

	                   outb ((BYTE) l_ReloadValue,
		                 BoardInformation->s_BaseInformation.
			                           ui_BaseAddress[1] + 
					           APCI1500_Z8536_CONTROL_REGISTER);

	                   /****************************************************/
	                   /* Selects the constant register of timer/counter 2 */
	                   /****************************************************/

	                   outb (APCI1500_RW_CPT_TMR2_TIME_CST_HIGH,
		                 BoardInformation->s_BaseInformation.
			                           ui_BaseAddress[1] + 
					           APCI1500_Z8536_CONTROL_REGISTER);

	                   /**************************/
	                   /* Writes the high value  */
	                   /**************************/

	                   outb ((BYTE) (l_ReloadValue >> 8),
		                 BoardInformation->s_BaseInformation.
			                           ui_BaseAddress[1] + 
					           APCI1500_Z8536_CONTROL_REGISTER);

	                   /*********************************************/
	                   /* Selects the master configuration register */
	                   /*********************************************/

	                   outb (APCI1500_RW_MASTER_CONFIGURATION_CONTROL,
		                 BoardInformation->s_BaseInformation.
			                           ui_BaseAddress[1] + 
					           APCI1500_Z8536_CONTROL_REGISTER);

	                   /**********************/
	                   /* Reads the register */
	                   /**********************/

  	                   b_MasterConfiguration = inb (BoardInformation->
		                                        s_BaseInformation.
			                                ui_BaseAddress[1] + 
					                APCI1500_Z8536_CONTROL_REGISTER);

	                   /********************************************************/
	                   /* Enables timer/counter 1 and triggers timer/counter 1 */
	                   /********************************************************/

	                   b_MasterConfiguration = (BYTE) (b_MasterConfiguration | 0x20);

	                   /*********************************************/
	                   /* Selects the master configuration register */
	                   /*********************************************/

	                   outb (APCI1500_RW_MASTER_CONFIGURATION_CONTROL,
		                 BoardInformation->s_BaseInformation.
			                           ui_BaseAddress[1] + 
					           APCI1500_Z8536_CONTROL_REGISTER);

	                   /********************************/
	                   /* Writes the new configuration  */
	                   /********************************/

	                   outb (b_MasterConfiguration
,
		                 BoardInformation->s_BaseInformation.
			                           ui_BaseAddress[1] + 
					           APCI1500_Z8536_CONTROL_REGISTER);

	                   /****************************************/
	                   /* Selects the commands register of     */
	                   /* timer/counter 2                      */
	                   /****************************************/

	                   outb (APCI1500_RW_CPT_TMR2_CMD_STATUS,
		                 BoardInformation->s_BaseInformation.
			                           ui_BaseAddress[1] + 
					           APCI1500_Z8536_CONTROL_REGISTER);

	                   /***************************/
	                   /* Disable timer/counter 2  */
	                   /***************************/

	                   outb (0x0,
		                 BoardInformation->s_BaseInformation.
			                           ui_BaseAddress[1] + 
					           APCI1500_Z8536_CONTROL_REGISTER);

	                   /****************************************/
	                   /* Selects the commands register of     */
	                   /* timer/counter 2                      */
	                   /****************************************/

	                   outb (APCI1500_RW_CPT_TMR2_CMD_STATUS,
		                 BoardInformation->s_BaseInformation.
			                           ui_BaseAddress[1] + 
					           APCI1500_Z8536_CONTROL_REGISTER);

 	                   /***************************/
	                   /* Trigger timer/counter 2 */
	                   /***************************/

	                   outb (0x2,
		                 BoardInformation->s_BaseInformation.
			                           ui_BaseAddress[1] + 
					           APCI1500_Z8536_CONTROL_REGISTER);

	                   /*******************************************/
	                   /* Stores the selection (timer or counter) */
	                   /*******************************************/

	                   BoardInformation-> s_TimerCounterInformation.
		                              b_TimerCounter2Select = 
					      (BYTE) b_CounterOrTimerSelect;

	                   BoardInformation-> s_TimerCounterInformation.
		                              b_TimerCounter2Init =
					      APCI1500_ENABLE;
					 
	                   /**********************************/
	                   /* Stores the interrupt selection */
	                   /**********************************/
		           if (b_InterruptHandling == APCI1500_ENABLE)
		           {
	                      BoardInformation-> s_TimerCounterInformation.
		                                 b_TimerCounter2InterruptEnabled =
					         APCI1500_ENABLE; 
		           }
		           else
		           {
	                      BoardInformation-> s_TimerCounterInformation.
		                                 b_TimerCounter2InterruptEnabled =
					         APCI1500_DISABLE; 
		           }


			} // if ((b_HardwareOrSoftwareGateSelect == APCI1500_SOFTWARE_GATE) || (b_HardwareOrSoftwareGateSelect == APCI1500_HARDWARE_GATE))
			else
			{
			   /************************/
			   /* Wrong gate parameter */
			   /************************/

			   i_ReturnValue = -6;
			} // if ((b_HardwareOrSoftwareGateSelect == APCI1500_SOFTWARE_GATE) || (b_HardwareOrSoftwareGateSelect == APCI1500_HARDWARE_GATE))
		     } // if ((b_HardwareOrSoftwareTriggerSelect == APCI1500_SOFTWARE_TRIGGER) ||(b_HardwareOrSoftwareTriggerSelect == APCI1500_HARDWARE_TRIGGER))
		     else
		     {
		        /****************************/
		        /* Wrong trigger parameter  */
		        /****************************/

		        i_ReturnValue = -7;
		     } // if ((b_HardwareOrSoftwareTriggerSelect == APCI1500_SOFTWARE_TRIGGER) ||(b_HardwareOrSoftwareTriggerSelect == APCI1500_HARDWARE_TRIGGER))
		  } // if ((b_InterruptHandling == APCI1500_DISABLE) || ((b_InterruptHandling == APCI1500_ENABLE) && (((ps_APCI1500_GlobalStruct->...w_BoardHandleList >> b_BoardHandle) & 1) == 1)))
		  else
		  {
		     /****************************************/
		     /* User interrupt routine not installed */
		     /****************************************/

  	             i_ReturnValue = -4;
		  } // if ((b_InterruptHandling == APCI1500_DISABLE) || ((b_InterruptHandling == APCI1500_ENABLE) && (((ps_APCI1500_GlobalStruct->...w_BoardHandleList >> b_BoardHandle) & 1) == 1)))
	       } // if ((l_ReloadValue >= 0) && (l_ReloadValue <= 65535U))
	       else
	       {
	          /********************************/
	          /* Reload value selection Error */
	          /********************************/

	          i_ReturnValue = -8;
	       } // if ((l_ReloadValue >= 0) && (l_ReloadValue <= 65535U))
	    } // if ((b_ContinuousOrSingleCycleSelect == APCI1500_SINGLE) || (b_ContinuousOrSingleCycleSelect == APCI1500_CONTINUOUS))
	    else
	    {
	       /********************************/
	       /* The cycle parameter is wrong */
	       /********************************/

	       i_ReturnValue = -5;
	    } // if ((b_ContinuousOrSingleCycleSelect == APCI1500_SINGLE) || (b_ContinuousOrSingleCycleSelect == APCI1500_CONTINUOUS))
	 } // if ((b_InterruptHandling == APCI1500_ENABLE) || (b_InterruptHandling == APCI1500_DISABLE))
	 else
	 {
	    /*************************************/
	    /* Error with th interrupt selection */
	    /*************************************/

	    i_ReturnValue = -3;
	 } // if ((b_InterruptHandling == APCI1500_ENABLE) || (b_InterruptHandling == APCI1500_DISABLE))
      } // if ((b_CounterOrTimerSelect == APCI1500_TIMER) || (b_CounterOrTimerSelect == APCI1500_COUNTER))
      else
      {
         /*****************************************************************/
         /* The parameter for selecting the counter or the timer is wrong */
         /*****************************************************************/

         i_ReturnValue = -2;
      } // if ((b_CounterOrTimerSelect == APCI1500_TIMER) || (b_CounterOrTimerSelect == APCI1500_COUNTER))

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
+----------------------------------------------------------------------------+
| Output Parameters : -                                                      |
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

INT     i_APCI1500_InitWatchdogCounter3   (BYTE   b_BoardHandle,
					   BYTE   b_WatchdogOrCounterSelect,
					   LONG   l_ReloadValue,
					   BYTE   b_ContinuousOrSingleCycleSelect,
					   BYTE   b_HardwareOrSoftwareGateSelect,
					   BYTE   b_InterruptHandling)
{
   INT   i_ReturnValue         = 0;
   BYTE  b_TimerCounterMode    = 0;
   BYTE  b_MasterConfiguration = 0;

   /*******************************************************/
   /* Gets the pointer to the board information structure */
   /*******************************************************/
   str_APCI1500_BoardInformation *BoardInformation = NULL; 
   
   BoardInformation = &(s_APCI1500_DriverStruct.
                        s_BoardInformation[b_BoardHandle]);

   /***************************/
   /* Test if board handle OK */
   /***************************/
   if (b_BoardHandle < s_APCI1500_DriverStruct.b_NumberOfBoard)
   {
      /**********************************/
      /* Test the watchdog/counter mode */
      /**********************************/


      if ((b_WatchdogOrCounterSelect == APCI1500_WATCHDOG) ||
	  (b_WatchdogOrCounterSelect == APCI1500_COUNTER))
      {
         /*********************************/
	 /* Tests the interrupt parameter */
	 /*********************************/

	 if ((b_InterruptHandling == APCI1500_ENABLE) ||
	     (b_InterruptHandling == APCI1500_DISABLE))
	 {
	    /*****************************/
	    /* Tests the cycle parameter */
	    /*****************************/

	    if ((b_ContinuousOrSingleCycleSelect == APCI1500_SINGLE)    ||
	        (b_ContinuousOrSingleCycleSelect == APCI1500_CONTINUOUS))
	    {
	       /****************************/
	       /* Tests the gate parameter */
	       /****************************/

	       if ((b_HardwareOrSoftwareGateSelect == APCI1500_SOFTWARE_GATE) ||
	           (b_HardwareOrSoftwareGateSelect == APCI1500_HARDWARE_GATE) ||
	           (b_WatchdogOrCounterSelect	   == APCI1500_WATCHDOG))
	       {
	          /*************************/
		  /* Test the reload value */
		  /*************************/

		  if ((l_ReloadValue >= 0) && (l_ReloadValue <= 65535U))
		  {
		     /*****************************************/
		     /* Test the interrupt function installed */
		     /*****************************************/

		     if ((b_InterruptHandling  == APCI1500_DISABLE) ||
		         ((b_InterruptHandling == APCI1500_ENABLE) && (((BoardInformation-> s_InterruptInformation.
									                 b_InterruptInitialised) & 1) == 1)))
		     {

	                /*****************************/
	                /* Test if used for watchdog */
	                /*****************************/

	                if (b_WatchdogOrCounterSelect == APCI1500_WATCHDOG)
	                {
	                   /*****************************/
	                   /* - Enables the output line */
	                   /* - Enables retrigger       */
	                   /* - Pulses output           */
	                   /*****************************/

	                   b_TimerCounterMode = APCI1500_WATCHDOG   |
				                b_ContinuousOrSingleCycleSelect |
				                0x54;
	                }
	                else
	                {
	                   b_TimerCounterMode = (BYTE) (b_WatchdogOrCounterSelect |
					                b_HardwareOrSoftwareGateSelect |
					                b_ContinuousOrSingleCycleSelect |
					                7);
	                }
     	                /***************************************************/
 	                /* Selects the mode register of watchdog/counter 3 */
	                /***************************************************/

	                outb (APCI1500_RW_CPT_TMR3_MODE_SPECIFICATION,
		              BoardInformation->s_BaseInformation.
			                        ui_BaseAddress[1] + 
				                APCI1500_Z8536_CONTROL_REGISTER);

	                /***********************/
	                /* Writes the new mode */
 	                /***********************/

	                outb (b_TimerCounterMode,
		              BoardInformation->s_BaseInformation.
			                        ui_BaseAddress[1] + 
			  	                APCI1500_Z8536_CONTROL_REGISTER);

	                /*******************************************************/
	                /* Selects the constant register of watchdog/counter 3 */
	                /*******************************************************/

	                outb (APCI1500_RW_CPT_TMR3_TIME_CST_LOW,
		              BoardInformation->s_BaseInformation.
			                        ui_BaseAddress[1] + 
					        APCI1500_Z8536_CONTROL_REGISTER);

	                /*************************/
	                /* Writes the low value  */
	                /*************************/

	                outb ((BYTE) l_ReloadValue,
		              BoardInformation->s_BaseInformation.
			                        ui_BaseAddress[1] + 
			   	                APCI1500_Z8536_CONTROL_REGISTER);

	                /*******************************************************/
	                /* Selects the constant register of watchdog/counter 3 */
	                /*******************************************************/

	                outb (APCI1500_RW_CPT_TMR3_TIME_CST_HIGH,
		              BoardInformation->s_BaseInformation.
			                        ui_BaseAddress[1] + 
					        APCI1500_Z8536_CONTROL_REGISTER);

	                /**************************/
	                /* Writes the high value  */
	                /**************************/

	                outb ((BYTE) (l_ReloadValue >> 8),
		              BoardInformation->s_BaseInformation.
			                        ui_BaseAddress[1] + 
			 	                APCI1500_Z8536_CONTROL_REGISTER);

	                /*********************************************/
	                /* Selects the master configuration register */
	                /*********************************************/

	                outb (APCI1500_RW_MASTER_CONFIGURATION_CONTROL,
		              BoardInformation->s_BaseInformation.
			                        ui_BaseAddress[1] + 
			  	                APCI1500_Z8536_CONTROL_REGISTER);

	                /**********************/
	                /* Reads the register */
	                /**********************/

  	                b_MasterConfiguration = inb (BoardInformation->
		                                     s_BaseInformation.
			                             ui_BaseAddress[1] + 
					             APCI1500_Z8536_CONTROL_REGISTER);

	                /********************************************************/
	                /* Enables watchdog/counter 3 and triggers timer/counter 1 */
	                /********************************************************/

	                b_MasterConfiguration = (BYTE) (b_MasterConfiguration | 0x10);

	                /*********************************************/
	                /* Selects the master configuration register */
	                /*********************************************/

	                outb (APCI1500_RW_MASTER_CONFIGURATION_CONTROL,
		              BoardInformation->s_BaseInformation.
			                        ui_BaseAddress[1] + 
			 	                APCI1500_Z8536_CONTROL_REGISTER);

	                /********************************/
	                /* Writes the new configuration  */
	                /********************************/

	                outb (b_MasterConfiguration
,
		              BoardInformation->s_BaseInformation.
			                        ui_BaseAddress[1] + 
					        APCI1500_Z8536_CONTROL_REGISTER);
	
	                /**********************************************/
	                /* Stores the selection (watchdog or counter) */
	                /**********************************************/
	                BoardInformation-> s_TimerCounterInformation.
	                                   b_WatchdogCounter3Select = 
					   (BYTE) b_WatchdogOrCounterSelect;

	                /********************/
	                /* Test if watchdog */
	                /********************/

	                if (b_WatchdogOrCounterSelect == APCI1500_WATCHDOG)
	                { 
	                   /*****************************/
	                   /* Stores the interrupt mode */
	                   /*****************************/

	                   BoardInformation-> s_TimerCounterInformation.
 	                                      b_Counter3InterruptEnabled = (BYTE) b_InterruptHandling;

	                   BoardInformation-> s_TimerCounterInformation.
                                              b_Counter3Enabled = APCI1500_ENABLE;

	                   /*****************************/
	                   /* Test if interrupt enabled */
	                   /*****************************/

	                   if (b_InterruptHandling == APCI1500_ENABLE)
	                   {
	                      /************************************/
	                      /* Selects the commands register of */
	                      /* watchdog/counter 3               */
	                      /************************************/

	                      outb (APCI1500_RW_CPT_TMR3_CMD_STATUS,
		                    BoardInformation->s_BaseInformation.
			                              ui_BaseAddress[1] + 
			  	                      APCI1500_Z8536_CONTROL_REGISTER);

	                      /************************************/
	                      /* Enables the interrupt for the    */
	                      /* watchdog/counter 3 and starts it */
	                      /************************************/

	                      outb (0xC6,
		                    BoardInformation->s_BaseInformation.
			                              ui_BaseAddress[1] + 
			  	                      APCI1500_Z8536_CONTROL_REGISTER);
	                   }

	                   /*************************************/
	                   /* Selects the command register of   */
	                   /* watchdog/counter 3                */
	                   /*************************************/

	                   outb (APCI1500_RW_CPT_TMR3_CMD_STATUS,
		                 BoardInformation->s_BaseInformation.
			                           ui_BaseAddress[1] + 
			  	                   APCI1500_Z8536_CONTROL_REGISTER);

	                   /*************************************************/
	                   /* Enables the  watchdog/counter 3 and starts it */
	                   /*************************************************/

	                   outb (0x6,
		                 BoardInformation->s_BaseInformation.
			                           ui_BaseAddress[1] + 
			  	                   APCI1500_Z8536_CONTROL_REGISTER);
	                } // if (*pb_WatchdogOrCounterSelect == APCI1500_WATCHDOG)
	                else
	                {
	                   /*************************************/
	                   /* Selects the command register of   */
	                   /* watchdog/counter 3                */
	                   /*************************************/

	                   outb (APCI1500_RW_CPT_TMR3_CMD_STATUS,
		                 BoardInformation->s_BaseInformation.
			                           ui_BaseAddress[1] + 
			  	                   APCI1500_Z8536_CONTROL_REGISTER);

	                   /*************************************************/
	                   /* Disable the  watchdog/counter 3 and starts it */
	                   /*************************************************/

	                   outb (0x0,
		                 BoardInformation->s_BaseInformation.
			                           ui_BaseAddress[1] + 
			  	                   APCI1500_Z8536_CONTROL_REGISTER);

	                   /*************************************/
	                   /* Selects the command register of   */
	                   /* watchdog/counter 3                */
	                   /*************************************/

	                   outb (APCI1500_RW_CPT_TMR3_CMD_STATUS,
		                 BoardInformation->s_BaseInformation.
			                           ui_BaseAddress[1] + 
			  	                   APCI1500_Z8536_CONTROL_REGISTER);

	                   /*************************************************/
	                   /* Trigger the  watchdog/counter 3 and starts it */
	                   /*************************************************/

	                   outb (0x2,
		                 BoardInformation->s_BaseInformation.
			                           ui_BaseAddress[1] + 
			  	                   APCI1500_Z8536_CONTROL_REGISTER);

	                } // if (*pb_WatchdogOrCounterSelect == APCI1500_WATCHDOG)

	                /*******************************************/
	                /* Stores the selection (timer or counter) */
	                /*******************************************/

	                BoardInformation-> s_TimerCounterInformation.
		                           b_WatchdogCounter3Select = 
					   (BYTE) b_WatchdogOrCounterSelect;

	                BoardInformation-> s_TimerCounterInformation.
		                           b_WatchdogCounter3Init =
			 	           APCI1500_ENABLE;
					 
	                /**********************************/
	                /* Stores the interrupt selection */
	                /**********************************/
		        if (b_InterruptHandling == APCI1500_ENABLE)
		        {
	                   BoardInformation-> s_TimerCounterInformation.
		                              b_Counter3InterruptEnabled =
			 	              APCI1500_ENABLE; 
		        }
		        else
		        {
	                   BoardInformation-> s_TimerCounterInformation.
		                              b_Counter3InterruptEnabled =
			 	              APCI1500_DISABLE; 
		        }

		     } // if (((b_WatchdogOrCounterSelect == APCI1500_COUNTER) && ...)
		     else
		     {
		        /****************************************/
		        /* User interrupt routine not installed */
		        /****************************************/

		        i_ReturnValue = -4;
		     } // if (((b_WatchdogOrCounterSelect == APCI1500_COUNTER) && ...)
		  } // if ((l_ReloadValue >= 0) && (l_ReloadValue <= 65535U))
		  else
		  {
		     /**********************/
		     /* Reload value Error */
		     /**********************/

		     i_ReturnValue = -7;
		  } // if ((l_ReloadValue >= 0) && (l_ReloadValue <= 65535U))
	       } // if ((b_HardwareOrSoftwareGateSelect == APCI1500_SOFTWARE_GATE) || (b_HardwareOrSoftwareGateSelect == APCI1500_HARDWARE_GATE) || (b_TimerCounterMode == APCI1500_WATCHDOG))
	       else
	       {
	          /************************/
	          /* Wrong gate parameter */
	          /************************/

	          i_ReturnValue = -6;
	       } // if ((b_HardwareOrSoftwareGateSelect == APCI1500_SOFTWARE_GATE) || (b_HardwareOrSoftwareGateSelect == APCI1500_HARDWARE_GATE) || (b_TimerCounterMode == APCI1500_WATCHDOG))
	    } // if ((b_ContinuousOrSingleCycleSelect == APCI1500_SINGLE) || (b_ContinuousOrSingleCycleSelect == APCI1500_CONTINUOUS))
	    else
	    {
	       /****************************/
	       /* Cycle parameter is wrong */
	       /****************************/

	       i_ReturnValue = -5;
	    } // if ((b_ContinuousOrSingleCycleSelect == APCI1500_SINGLE) || (b_ContinuousOrSingleCycleSelect == APCI1500_CONTINUOUS))
	 } // if ((b_InterruptHandling == APCI1500_ENABLE) || (b_InterruptHandling == APCI1500_DISABLE))
	 else
	 {
	    /**************************************/
	    /* Error with the interrupt selection */
	    /**************************************/

	    i_ReturnValue = -3;
	 } // if ((b_InterruptHandling == APCI1500_ENABLE) || (b_InterruptHandling == APCI1500_DISABLE))
      } // if ((b_WatchdogOrCounterSelect == APCI1500_WATCHDOG) || (b_WatchdogOrCounterSelect == APCI1500_COUNTER))
      else
      {
         /*************************************************/
	 /* Watchdog or counter selection parameter Error */
	 /*************************************************/

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
   return (i_ReturnValue);
}


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

INT     i_APCI1500_StartTimerCounter1     (BYTE  b_BoardHandle)
{
   INT   i_ReturnValue           = 0;
   BYTE  b_CommandAndStatusValue = 0;
   /*******************************************************/
   /* Gets the pointer to the board information structure */
   /*******************************************************/
   str_APCI1500_BoardInformation *BoardInformation = NULL; 
   
   BoardInformation = &(s_APCI1500_DriverStruct.
                        s_BoardInformation[b_BoardHandle]);

   /***************************/
   /* Test if board handle OK */
   /***************************/
   if (b_BoardHandle < s_APCI1500_DriverStruct.b_NumberOfBoard)
   {
      /***************************************/
      /* Test if timer/counter 1 initialised */
      /***************************************/

      if (BoardInformation-> s_TimerCounterInformation.
		               b_TimerCounter1Init == APCI1500_ENABLE)
      {
	 /**************************/
	 /* Starts timer/counter 1 */
	 /**************************/

         BoardInformation-> s_TimerCounterInformation.
	 b_TimerCounter1Enabled = APCI1500_ENABLE;


	 /********************************************/
	 /* Selects the commands and status register */
	 /********************************************/

	 outb (APCI1500_RW_CPT_TMR1_CMD_STATUS,
               BoardInformation->s_BaseInformation.
  	                         ui_BaseAddress[1] + 
				 APCI1500_Z8536_CONTROL_REGISTER);

	 if (BoardInformation-> s_TimerCounterInformation.
	                        b_TimerCounter1InterruptEnabled != 0)
	 {
	    /************************/
	    /* Enable the interrupt */
	    /************************/
	    printk ("Interrupt enabled!");

	    b_CommandAndStatusValue = (BYTE) 0xC4;
	 } 
	 else
	 {
	    /*************************/
	    /* Disable the interrupt */
	    /*************************/

	    b_CommandAndStatusValue = (BYTE) 0xE4;
	 }

	 /***************************/
	 /* Writes the new commands */
	 /***************************/

	 outb (b_CommandAndStatusValue,
               BoardInformation->s_BaseInformation.
  	                         ui_BaseAddress[1] + 
				 APCI1500_Z8536_CONTROL_REGISTER);

      } 
      else
      {
         /**********************************/
         /* Counter or timer not installed */
         /**********************************/

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
   return (i_ReturnValue);
}


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

INT     i_APCI1500_StartTimerCounter2     (BYTE  b_BoardHandle)
{
   INT   i_ReturnValue           = 0;
   BYTE  b_CommandAndStatusValue = 0;
   /*******************************************************/
   /* Gets the pointer to the board information structure */
   /*******************************************************/
   str_APCI1500_BoardInformation *BoardInformation = NULL; 
   
   BoardInformation = &(s_APCI1500_DriverStruct.
                        s_BoardInformation[b_BoardHandle]);

   /***************************/
   /* Test if board handle OK */
   /***************************/
   if (b_BoardHandle < s_APCI1500_DriverStruct.b_NumberOfBoard)
   {
      /***************************************/
      /* Test if timer/counter 1 initialised */
      /***************************************/

      if (BoardInformation-> s_TimerCounterInformation.
		               b_TimerCounter2Init == APCI1500_ENABLE)
      {
	 /**************************/
	 /* Starts timer/counter 1 */
	 /**************************/

         BoardInformation-> s_TimerCounterInformation.
	 b_TimerCounter2Enabled = APCI1500_ENABLE;


	 /********************************************/
	 /* Selects the commands and status register */
	 /********************************************/

	 outb (APCI1500_RW_CPT_TMR2_CMD_STATUS,
               BoardInformation->s_BaseInformation.
  	                         ui_BaseAddress[1] + 
				 APCI1500_Z8536_CONTROL_REGISTER);

	 if (BoardInformation-> s_TimerCounterInformation.
	                        b_TimerCounter2InterruptEnabled != 0)
	 {
	    /************************/
	    /* Enable the interrupt */
	    /************************/
	    printk ("Interrupt enabled!");

	    b_CommandAndStatusValue = (BYTE) 0xC4;
	 } 
	 else
	 {
	    /*************************/
	    /* Disable the interrupt */
	    /*************************/

	    b_CommandAndStatusValue = (BYTE) 0xE4;
	 }

	 /***************************/
	 /* Writes the new commands */
	 /***************************/

	 outb (b_CommandAndStatusValue,
               BoardInformation->s_BaseInformation.
  	                         ui_BaseAddress[1] + 
				 APCI1500_Z8536_CONTROL_REGISTER);

      } 
      else
      {
         /**********************************/
         /* Counter or timer not installed */
         /**********************************/

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
   return (i_ReturnValue);
}


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

INT     i_APCI1500_StartCounter3     (BYTE  b_BoardHandle)
{
   INT   i_ReturnValue           = 0;
   BYTE  b_CommandAndStatusValue = 0;
   /*******************************************************/
   /* Gets the pointer to the board information structure */
   /*******************************************************/
   str_APCI1500_BoardInformation *BoardInformation = NULL; 
   
   BoardInformation = &(s_APCI1500_DriverStruct.
                        s_BoardInformation[b_BoardHandle]);

   /***************************/
   /* Test if board handle OK */
   /***************************/
   if (b_BoardHandle < s_APCI1500_DriverStruct.b_NumberOfBoard)
   {
      /***************************************/
      /* Test if timer/counter 1 initialised */
      /***************************************/

      if (BoardInformation-> s_TimerCounterInformation.
		               b_WatchdogCounter3Init == APCI1500_ENABLE)
      {
	 /**************************/
	 /* Starts timer/counter 1 */
	 /**************************/

         BoardInformation-> s_TimerCounterInformation.
	 b_Counter3Enabled = APCI1500_ENABLE;


	 /********************************************/
	 /* Selects the commands and status register */
	 /********************************************/

	 outb (APCI1500_RW_CPT_TMR3_CMD_STATUS,
               BoardInformation->s_BaseInformation.
  	                         ui_BaseAddress[1] + 
				 APCI1500_Z8536_CONTROL_REGISTER);

	 if (BoardInformation-> s_TimerCounterInformation.
	                        b_Counter3InterruptEnabled != 0)
	 {
	    /************************/
	    /* Enable the interrupt */
	    /************************/
	    printk ("Interrupt enabled!");

	    b_CommandAndStatusValue = (BYTE) 0xC4;
	 } 
	 else
	 {
	    /*************************/
	    /* Disable the interrupt */
	    /*************************/

	    b_CommandAndStatusValue = (BYTE) 0xE4;
	 }

	 /***************************/
	 /* Writes the new commands */
	 /***************************/

	 outb (b_CommandAndStatusValue,
               BoardInformation->s_BaseInformation.
  	                         ui_BaseAddress[1] + 
				 APCI1500_Z8536_CONTROL_REGISTER);

      } 
      else
      {
         /*************************/
         /* Counter not installed */
         /*************************/

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
   return (i_ReturnValue);
}

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

INT     i_APCI1500_StopTimerCounter1      (BYTE  b_BoardHandle)
{
   INT   i_ReturnValue         = 0;

   /*******************************************************/
   /* Gets the pointer to the board information structure */
   /*******************************************************/
   str_APCI1500_BoardInformation *BoardInformation = NULL; 
   
   BoardInformation = &(s_APCI1500_DriverStruct.
                        s_BoardInformation[b_BoardHandle]);

   /***************************/
   /* Test if board handle OK */
   /***************************/
   if (b_BoardHandle < s_APCI1500_DriverStruct.b_NumberOfBoard)
   {
      /***************************************/
      /* Test if timer/counter 1 initialised */
      /***************************************/

      if (BoardInformation-> s_TimerCounterInformation.
		               b_TimerCounter1Init == APCI1500_ENABLE)
      {
	 /*******************************************/
	 /* Selects the command and status register */
	 /*******************************************/

	 outb (APCI1500_RW_CPT_TMR1_CMD_STATUS,
               BoardInformation->s_BaseInformation.
  	                         ui_BaseAddress[1] + 
				 APCI1500_Z8536_CONTROL_REGISTER);

	 /**************************/
	 /* Writes the new command */
	 /**************************/

	 outb (0x00,
               BoardInformation->s_BaseInformation.
  	                         ui_BaseAddress[1] + 
				 APCI1500_Z8536_CONTROL_REGISTER);
	

         BoardInformation-> s_TimerCounterInformation.	
	                    b_TimerCounter1Enabled = APCI1500_DISABLE;

      } 
      else
      {
         /************************************/
         /* Counter or timer not initialized */
	 /************************************/

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
   return (i_ReturnValue);
}


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

INT     i_APCI1500_StopTimerCounter2      (BYTE  b_BoardHandle)
{
   INT   i_ReturnValue         = 0;

   /*******************************************************/
   /* Gets the pointer to the board information structure */
   /*******************************************************/
   str_APCI1500_BoardInformation *BoardInformation = NULL; 
   
   BoardInformation = &(s_APCI1500_DriverStruct.
                        s_BoardInformation[b_BoardHandle]);

   /***************************/
   /* Test if board handle OK */
   /***************************/
   if (b_BoardHandle < s_APCI1500_DriverStruct.b_NumberOfBoard)
   {
      /***************************************/
      /* Test if timer/counter 2 initialised */
      /***************************************/

      if (BoardInformation-> s_TimerCounterInformation.
		               b_TimerCounter2Init == APCI1500_ENABLE)
      {
	 /*******************************************/
	 /* Selects the command and status register */
	 /*******************************************/

	 outb (APCI1500_RW_CPT_TMR2_CMD_STATUS,
               BoardInformation->s_BaseInformation.
  	                         ui_BaseAddress[1] + 
				 APCI1500_Z8536_CONTROL_REGISTER);

	 /**************************/
	 /* Writes the new command */
	 /**************************/

	 outb (0x00,
               BoardInformation->s_BaseInformation.
  	                         ui_BaseAddress[1] + 
				 APCI1500_Z8536_CONTROL_REGISTER);
	

         BoardInformation-> s_TimerCounterInformation.	
	                    b_TimerCounter2Enabled = APCI1500_DISABLE;

      } 
      else
      {
         /************************************/
         /* Counter or timer not initialized */
	 /************************************/

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
   return (i_ReturnValue);
}


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

INT     i_APCI1500_StopCounter3      (BYTE  b_BoardHandle)
{
   INT   i_ReturnValue         = 0;

   /*******************************************************/
   /* Gets the pointer to the board information structure */
   /*******************************************************/
   str_APCI1500_BoardInformation *BoardInformation = NULL; 
   
   BoardInformation = &(s_APCI1500_DriverStruct.
                        s_BoardInformation[b_BoardHandle]);

   /***************************/
   /* Test if board handle OK */
   /***************************/
   if (b_BoardHandle < s_APCI1500_DriverStruct.b_NumberOfBoard)
   {
      /*********************************/
      /* Test if counter 3 initialised */
      /*********************************/

      if (BoardInformation-> s_TimerCounterInformation.
		               b_WatchdogCounter3Init == APCI1500_ENABLE)
      {
	 /*******************************************/
	 /* Selects the command and status register */
	 /*******************************************/

	 outb (APCI1500_RW_CPT_TMR3_CMD_STATUS,
               BoardInformation->s_BaseInformation.
  	                         ui_BaseAddress[1] + 
				 APCI1500_Z8536_CONTROL_REGISTER);

	 /**************************/
	 /* Writes the new command */
	 /**************************/

	 outb (0x00,
               BoardInformation->s_BaseInformation.
  	                         ui_BaseAddress[1] + 
				 APCI1500_Z8536_CONTROL_REGISTER);
	

         BoardInformation-> s_TimerCounterInformation.	
	                    b_Counter3Enabled = APCI1500_DISABLE;

      } 
      else
      {
         /***************************/
         /* Counter not initialized */
	 /***************************/

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
   return (i_ReturnValue);
}

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

INT     i_APCI1500_TriggerTimerCounter1      (BYTE  b_BoardHandle)
{
   INT   i_ReturnValue           = 0;
   BYTE  b_CommandAndStatusValue
 = 0;

   /*******************************************************/
   /* Gets the pointer to the board information structure */
   /*******************************************************/
   str_APCI1500_BoardInformation *BoardInformation = NULL; 
   
   BoardInformation = &(s_APCI1500_DriverStruct.
                        s_BoardInformation[b_BoardHandle]);

   /***************************/
   /* Test if board handle OK */
   /***************************/
   if (b_BoardHandle < s_APCI1500_DriverStruct.b_NumberOfBoard)
   {
      /***************************************/
      /* Test if timer/counter 1 initialised */
      /***************************************/

      if (BoardInformation-> s_TimerCounterInformation.
		               b_TimerCounter1Init == APCI1500_ENABLE)
      {
	 /*************************/
	 /* Test if Timer enabled */
	 /*************************/

	 if (BoardInformation-> s_TimerCounterInformation.
		                b_TimerCounter1Enabled
 == APCI1500_ENABLE)
         {
	    /************************/
	    /* Set Trigger and gate */
	    /************************/

	    b_CommandAndStatusValue = 0x6;
	 } 
	 else
	 {
	    /***************/
	    /* Set Trigger */
	    /***************/

	    b_CommandAndStatusValue = 0x2;
         } 

	 /********************************************/
	 /* Selects the commands and status register */
	 /********************************************/

	 outb (APCI1500_RW_CPT_TMR1_CMD_STATUS,
               BoardInformation->s_BaseInformation.
  	                         ui_BaseAddress[1] + 
				 APCI1500_Z8536_CONTROL_REGISTER);
	 

	 /***************************/
	 /* Writes the new commands */
	 /***************************/

	 outb (b_CommandAndStatusValue,
               BoardInformation->s_BaseInformation.
  	                         ui_BaseAddress[1] + 
				 APCI1500_Z8536_CONTROL_REGISTER);
      }
      else
      {
         /************************************/
         /* Counter or timer not initialized */
         /************************************/

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
   return (i_ReturnValue);
}


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

INT     i_APCI1500_TriggerTimerCounter2      (BYTE  b_BoardHandle)
{
   INT   i_ReturnValue           = 0;
   BYTE  b_CommandAndStatusValue
 = 0;

   /*******************************************************/
   /* Gets the pointer to the board information structure */
   /*******************************************************/
   str_APCI1500_BoardInformation *BoardInformation = NULL; 
   
   BoardInformation = &(s_APCI1500_DriverStruct.
                        s_BoardInformation[b_BoardHandle]);

   /***************************/
   /* Test if board handle OK */
   /***************************/
   if (b_BoardHandle < s_APCI1500_DriverStruct.b_NumberOfBoard)
   {
      /***************************************/
      /* Test if timer/counter 2 initialised */
      /***************************************/

      if (BoardInformation-> s_TimerCounterInformation.
		               b_TimerCounter2Init == APCI1500_ENABLE)
      {
	 /*************************/
	 /* Test if Timer enabled */
	 /*************************/

	 if (BoardInformation-> s_TimerCounterInformation.
		                b_TimerCounter2Enabled
 == APCI1500_ENABLE)
         {
	    /************************/
	    /* Set Trigger and gate */
	    /************************/

	    b_CommandAndStatusValue = 0x6;
	 } 
	 else
	 {
	    /***************/
	    /* Set Trigger */
	    /***************/

	    b_CommandAndStatusValue = 0x2;
         } 

	 /********************************************/
	 /* Selects the commands and status register */
	 /********************************************/

	 outb (APCI1500_RW_CPT_TMR2_CMD_STATUS,
               BoardInformation->s_BaseInformation.
  	                         ui_BaseAddress[1] + 
				 APCI1500_Z8536_CONTROL_REGISTER);
	 

	 /***************************/
	 /* Writes the new commands */
	 /***************************/

	 outb (b_CommandAndStatusValue,
               BoardInformation->s_BaseInformation.
  	                         ui_BaseAddress[1] + 
				 APCI1500_Z8536_CONTROL_REGISTER);
      }
      else
      {
         /************************************/
         /* Counter or timer not initialized */
         /************************************/

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
   return (i_ReturnValue);
}

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

INT     i_APCI1500_TriggerCounter3      (BYTE  b_BoardHandle)
{
   INT   i_ReturnValue           = 0;
   BYTE  b_CommandAndStatusValue
 = 0;

   /*******************************************************/
   /* Gets the pointer to the board information structure */
   /*******************************************************/
   str_APCI1500_BoardInformation *BoardInformation = NULL; 
   
   BoardInformation = &(s_APCI1500_DriverStruct.
                        s_BoardInformation[b_BoardHandle]);

   /***************************/
   /* Test if board handle OK */
   /***************************/
   if (b_BoardHandle < s_APCI1500_DriverStruct.b_NumberOfBoard)
   {
      /*********************************/
      /* Test if counter 3 initialised */
      /*********************************/

      if (BoardInformation-> s_TimerCounterInformation.
		               b_WatchdogCounter3Init == APCI1500_ENABLE)
      {
         /***************************************************/
	 /* Test if watchdog/counter initialised as counter */
	 /***************************************************/
	 if (BoardInformation-> s_TimerCounterInformation.
		                b_WatchdogCounter3Select
 == APCI1500_COUNTER)
         {
	    /***************************/
	    /* Test if counter enabled */
	    /***************************/

	    if (BoardInformation-> s_TimerCounterInformation.
		                   b_Counter3Enabled
 == APCI1500_ENABLE)
            { 
	       /************************/
	       /* Set Trigger and gate */
	       /************************/

	       b_CommandAndStatusValue = 0x6;
	    } 
	    else
	    {
	       /***************/
	       /* Set Trigger */
	       /***************/

	       b_CommandAndStatusValue = 0x2;
            } 

	    /********************************************/
	    /* Selects the commands and status register */
	    /********************************************/

	    outb (APCI1500_RW_CPT_TMR2_CMD_STATUS,
                  BoardInformation->s_BaseInformation.
  	                            ui_BaseAddress[1] + 
				    APCI1500_Z8536_CONTROL_REGISTER);
	 

	    /***************************/
	    /* Writes the new commands */
	    /***************************/

	    outb (b_CommandAndStatusValue,
                  BoardInformation->s_BaseInformation.
  	                            ui_BaseAddress[1] + 
				    APCI1500_Z8536_CONTROL_REGISTER);
         }
	 else
	 {
            /************************************/
            /* Counter or timer not initialized */
            /************************************/

            i_ReturnValue = -2;
	 
	 }
      }      
      else
      {
         /**********************************************/
         /* Watchdog / Counter initialized as watchdog */
         /**********************************************/

         i_ReturnValue = -3;
      }

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

INT     i_APCI1500_TriggerWatchdog      (BYTE  b_BoardHandle)
{
   INT   i_ReturnValue           = 0;

   /*******************************************************/
   /* Gets the pointer to the board information structure */
   /*******************************************************/
   str_APCI1500_BoardInformation *BoardInformation = NULL; 
   
   BoardInformation = &(s_APCI1500_DriverStruct.
                        s_BoardInformation[b_BoardHandle]);

   /***************************/
   /* Test if board handle OK */
   /***************************/
   if (b_BoardHandle < s_APCI1500_DriverStruct.b_NumberOfBoard)
   {
      /*********************************/
      /* Test if counter 3 initialised */
      /*********************************/

      if (BoardInformation-> s_TimerCounterInformation.
		               b_WatchdogCounter3Init == APCI1500_ENABLE)
      {
         /****************************************************/
	 /* Test if watchdog/counter initialised as watchdog */
	 /****************************************************/
	 if (BoardInformation-> s_TimerCounterInformation.
		                b_WatchdogCounter3Select
 == APCI1500_WATCHDOG)
         {
	    /*******************************************/
	    /* Selects the command and status register */
	    /*******************************************/

	    outb (APCI1500_RW_CPT_TMR3_CMD_STATUS,
                  BoardInformation->s_BaseInformation.
  	                            ui_BaseAddress[1] + 
				    APCI1500_Z8536_CONTROL_REGISTER);

	    /**************************/
	    /* Writes the new command */
	    /**************************/

	    outb (0x6,
                  BoardInformation->s_BaseInformation.
  	                            ui_BaseAddress[1] + 
				    APCI1500_Z8536_CONTROL_REGISTER);
	 
         }
	 else
	 {
            /**********************************************/
            /* Watchdog / Counter initialized as watchdog */
            /**********************************************/

            i_ReturnValue = -3;
	 
	 }
      }      
      else
      {
         /************************************/
         /* Counter or timer not initialized */
         /************************************/

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
   return (i_ReturnValue);
}

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

INT      i_APCI1500_ReadTimerCounter1      (BYTE    b_BoardHandle,
					    PLONG  pl_ReadValue)
{
   INT   i_ReturnValue           = 0;
   BYTE  b_HighValue             = 0;
   BYTE  b_CommandAndStatusValue = 0;
   LONG  l_ReadValue             = 0;

   /*******************************************************/
   /* Gets the pointer to the board information structure */
   /*******************************************************/
   str_APCI1500_BoardInformation *BoardInformation = NULL; 
   
   BoardInformation = &(s_APCI1500_DriverStruct.
                        s_BoardInformation[b_BoardHandle]);

   /***************************/
   /* Test if board handle OK */
   /***************************/
   if (b_BoardHandle < s_APCI1500_DriverStruct.b_NumberOfBoard)
   {
      /***************************************/
      /* Test if timer/counter 1 initialised */
      /***************************************/

      if (BoardInformation-> s_TimerCounterInformation.
		               b_TimerCounter1Init == APCI1500_ENABLE)
      {
	 /*************************/
	 /* Test if Timer enabled */
	 /*************************/

	 if (BoardInformation-> s_TimerCounterInformation.
	                       b_TimerCounter1Enabled == APCI1500_ENABLE)
	 {
	    /************************/
	    /* Set Trigger and gate */
	    /************************/

	    b_CommandAndStatusValue = 0xC;
	 }
	 else
	 {
	    /***************/
	    /* Set Trigger */
	    /***************/

	    b_CommandAndStatusValue = 0x8;
	 }

	 /********************************************/
	 /* Selects the commands and status register */
	 /********************************************/

	 outb (APCI1500_RW_CPT_TMR1_CMD_STATUS,
               BoardInformation->s_BaseInformation.
  	                         ui_BaseAddress[1] + 
				 APCI1500_Z8536_CONTROL_REGISTER);

	 /***************************/
	 /* Writes the new commands */
	 /***************************/

	 outb (b_CommandAndStatusValue,
               BoardInformation->s_BaseInformation.
  	                         ui_BaseAddress[1] + 
				 APCI1500_Z8536_CONTROL_REGISTER);

	 /***************************************/
	 /* Selects the counter register (high) */
	 /***************************************/

	 outb (APCI1500_R_CPT_TMR1_VALUE_HIGH,
               BoardInformation->s_BaseInformation.
  	                         ui_BaseAddress[1] + 
				 APCI1500_Z8536_CONTROL_REGISTER);

	 /**********************/
	 /* Reads the register */
	 /**********************/

	 b_HighValue = inb (BoardInformation->s_BaseInformation.
  	                                     ui_BaseAddress[1] + 
				             APCI1500_Z8536_CONTROL_REGISTER);

	 /**************************************/
	 /* Selects the counter register (low) */
	 /**************************************/

	 outb (APCI1500_R_CPT_TMR1_VALUE_LOW,
               BoardInformation->s_BaseInformation.
  	                         ui_BaseAddress[1] + 
				 APCI1500_Z8536_CONTROL_REGISTER);


	 /**********************/
	 /* Reads the register */
	 /**********************/

	 l_ReadValue = inb (BoardInformation->s_BaseInformation.
  	                                      ui_BaseAddress[1] + 
				              APCI1500_Z8536_CONTROL_REGISTER);
					     
	 l_ReadValue = (l_ReadValue & 0xFF) + (UINT) ((UINT) (b_HighValue) << 8);

	 *pl_ReadValue = l_ReadValue;
      } 
      else
      {
         /************************************/
         /* Counter or timer not initialized */
         /************************************/

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
   return (i_ReturnValue);
}

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

INT      i_APCI1500_ReadTimerCounter2      (BYTE    b_BoardHandle,
					    PLONG  pl_ReadValue)
{
   INT   i_ReturnValue           = 0;
   BYTE  b_HighValue             = 0;
   BYTE  b_CommandAndStatusValue = 0;
   LONG  l_ReadValue             = 0;

   /*******************************************************/
   /* Gets the pointer to the board information structure */
   /*******************************************************/
   str_APCI1500_BoardInformation *BoardInformation = NULL; 
   
   BoardInformation = &(s_APCI1500_DriverStruct.
                        s_BoardInformation[b_BoardHandle]);

   /***************************/
   /* Test if board handle OK */
   /***************************/
   if (b_BoardHandle < s_APCI1500_DriverStruct.b_NumberOfBoard)
   {
      /***************************************/
      /* Test if timer/counter 1 initialised */
      /***************************************/

      if (BoardInformation-> s_TimerCounterInformation.
		               b_TimerCounter2Init == APCI1500_ENABLE)
      {
	 /*************************/
	 /* Test if Timer enabled */
	 /*************************/

	 if (BoardInformation-> s_TimerCounterInformation.
	                       b_TimerCounter2Enabled == APCI1500_ENABLE)
	 {
	    /************************/
	    /* Set Trigger and gate */
	    /************************/

	    b_CommandAndStatusValue = 0xC;
	 }
	 else
	 {
	    /***************/
	    /* Set Trigger */
	    /***************/

	    b_CommandAndStatusValue = 0x8;
	 }

	 /********************************************/
	 /* Selects the commands and status register */
	 /********************************************/

	 outb (APCI1500_RW_CPT_TMR2_CMD_STATUS,
               BoardInformation->s_BaseInformation.
  	                         ui_BaseAddress[1] + 
				 APCI1500_Z8536_CONTROL_REGISTER);

	 /***************************/
	 /* Writes the new commands */
	 /***************************/

	 outb (b_CommandAndStatusValue,
               BoardInformation->s_BaseInformation.
  	                         ui_BaseAddress[1] + 
				 APCI1500_Z8536_CONTROL_REGISTER);

	 /***************************************/
	 /* Selects the counter register (high) */
	 /***************************************/

	 outb (APCI1500_R_CPT_TMR2_VALUE_HIGH,
               BoardInformation->s_BaseInformation.
  	                         ui_BaseAddress[1] + 
				 APCI1500_Z8536_CONTROL_REGISTER);

	 /**********************/
	 /* Reads the register */
	 /**********************/

	 b_HighValue = inb (BoardInformation->s_BaseInformation.
  	                                     ui_BaseAddress[1] + 
				             APCI1500_Z8536_CONTROL_REGISTER);

	 /**************************************/
	 /* Selects the counter register (low) */
	 /**************************************/

	 outb (APCI1500_R_CPT_TMR2_VALUE_LOW,
               BoardInformation->s_BaseInformation.
  	                         ui_BaseAddress[1] + 
				 APCI1500_Z8536_CONTROL_REGISTER);


	 /**********************/
	 /* Reads the register */
	 /**********************/

	 l_ReadValue = inb (BoardInformation->s_BaseInformation.
  	                                      ui_BaseAddress[1] + 
				              APCI1500_Z8536_CONTROL_REGISTER);
					     
	 l_ReadValue = (l_ReadValue & 0xFF) + (UINT) ((UINT) (b_HighValue) << 8);

	 *pl_ReadValue = l_ReadValue;
      } 
      else
      {
         /************************************/
         /* Counter or timer not initialized */
         /************************************/

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
   return (i_ReturnValue);
   
}

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

INT      i_APCI1500_ReadCounter3           (BYTE    b_BoardHandle,
					    PLONG  pl_ReadValue)
{
   INT   i_ReturnValue           = 0;
   BYTE  b_HighValue             = 0;
   BYTE  b_CommandAndStatusValue = 0;
   LONG  l_ReadValue             = 0;

   /*******************************************************/
   /* Gets the pointer to the board information structure */
   /*******************************************************/
   str_APCI1500_BoardInformation *BoardInformation = NULL; 
   
   BoardInformation = &(s_APCI1500_DriverStruct.
                        s_BoardInformation[b_BoardHandle]);

   /***************************/
   /* Test if board handle OK */
   /***************************/
   if (b_BoardHandle < s_APCI1500_DriverStruct.b_NumberOfBoard)
   {
      /*********************************/
      /* Test if counter 3 initialised */
      /*********************************/

      if (BoardInformation-> s_TimerCounterInformation.
		               b_WatchdogCounter3Init == APCI1500_ENABLE)
      {
         /********************************************/
         /* Test if counter 3 initialised as counter */
         /********************************************/

         if (BoardInformation-> s_TimerCounterInformation.
		                b_WatchdogCounter3Select == APCI1500_COUNTER)
         {
	    /*************************/
	    /* Test if Timer enabled */
	    /*************************/

	    if (BoardInformation-> s_TimerCounterInformation.
	                           b_TimerCounter2Enabled == APCI1500_ENABLE)
	    {
	       /************************/
	       /* Set Trigger and gate */
	       /************************/

	       b_CommandAndStatusValue = 0xC;
	    }
	    else
	    {
	       /***************/
	       /* Set Trigger */
	       /***************/

	       b_CommandAndStatusValue = 0x8;
	    }

	    /********************************************/
	    /* Selects the commands and status register */
	    /********************************************/

	    outb (APCI1500_RW_CPT_TMR3_CMD_STATUS,
                  BoardInformation->s_BaseInformation.
  	                            ui_BaseAddress[1] + 
				    APCI1500_Z8536_CONTROL_REGISTER);

	    /***************************/
	    /* Writes the new commands */
	    /***************************/

	    outb (b_CommandAndStatusValue,
                  BoardInformation->s_BaseInformation.
  	                            ui_BaseAddress[1] + 
				    APCI1500_Z8536_CONTROL_REGISTER);

	    /***************************************/
	    /* Selects the counter register (high) */
	    /***************************************/

	    outb (APCI1500_R_CPT_TMR2_VALUE_HIGH,
                  BoardInformation->s_BaseInformation.
  	                            ui_BaseAddress[1] + 
				    APCI1500_Z8536_CONTROL_REGISTER);

	    /**********************/
	    /* Reads the register */
	    /**********************/

	    b_HighValue = inb (BoardInformation->s_BaseInformation.
  	                                         ui_BaseAddress[1] + 
				                 APCI1500_Z8536_CONTROL_REGISTER);

	    /**************************************/
	    /* Selects the counter register (low) */
	    /**************************************/

	    outb (APCI1500_R_CPT_TMR2_VALUE_LOW,
                  BoardInformation->s_BaseInformation.
  	                            ui_BaseAddress[1] + 
				    APCI1500_Z8536_CONTROL_REGISTER);


	    /**********************/
	    /* Reads the register */
	    /**********************/

	    l_ReadValue = inb (BoardInformation->s_BaseInformation.
  	                                         ui_BaseAddress[1] + 
				                 APCI1500_Z8536_CONTROL_REGISTER);
					     
	    l_ReadValue = (l_ReadValue & 0xFF) + (UINT) ((UINT) (b_HighValue) << 8);

	    *pl_ReadValue = l_ReadValue;
         }
	 else
	 {
            /***********************************/
            /* Counter initialized as watchdog */
            /***********************************/

            i_ReturnValue = -3;
	 }
      }	 
      else
      {
         /***************************/
         /* Counter not initialized */
         /***************************/

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
   return (i_ReturnValue);
   
}

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

INT     i_ConvertString         (CHAR*  pc_EventMask,
				 BYTE    b_PortNbr,
				 PBYTE  pb_PatternPolarity,
				 PBYTE  pb_PatternTransition,
				 PBYTE  pb_PatternMask)
{
   BYTE b_Cpt         = 0;
   BYTE b_BeginCpt    = 0;
   BYTE b_MaxChannel  = 0;
   INT  i_ReturnValue = 0;

   *pb_PatternMask       = 0;
   *pb_PatternPolarity   = 0;
   *pb_PatternTransition = 0;

   if (b_PortNbr == 1)
   {
      b_MaxChannel = 8;
   }
  
   else
   {
      b_MaxChannel = 6;
   }

   for (b_Cpt = b_BeginCpt; b_Cpt < b_MaxChannel; b_Cpt ++)
   {
      if (pc_EventMask [b_Cpt] == '0')
      {
         *pb_PatternMask = *pb_PatternMask | (BYTE) (1 << (b_MaxChannel - b_Cpt - 1));
      } // if (pc_EventMask [b_Cpt] == '0')
      else
      {
         if (pc_EventMask [b_Cpt] == '1')
	 {
 	    *pb_PatternMask     = *pb_PatternMask | 
	                          (BYTE) (1 << (b_MaxChannel - b_Cpt - 1));
	    *pb_PatternPolarity = *pb_PatternPolarity | 
	                          (BYTE) (1 << (b_MaxChannel - b_Cpt - 1));
 	 } // if (pc_EventMask [b_Cpt] == '1')
         else
	 {
	    if (pc_EventMask [b_Cpt] == '2')
	    {
	       *pb_PatternMask       = *pb_PatternMask | (BYTE) (1 << (b_MaxChannel - b_Cpt - 1));
	       *pb_PatternTransition = *pb_PatternTransition | (BYTE) (1 << (b_MaxChannel - b_Cpt - 1));
	    } // if (pc_EventMask [b_Cpt] == '2')
	    else
	    {
	       if (pc_EventMask [b_Cpt] == '3')
	       {
	          *pb_PatternMask       = *pb_PatternMask | (BYTE) (1 << (b_MaxChannel - b_Cpt - 1));
	          *pb_PatternPolarity   = *pb_PatternPolarity | (BYTE) (1 << (b_MaxChannel - b_Cpt - 1));
	          *pb_PatternTransition = *pb_PatternTransition | (BYTE) (1 << (b_MaxChannel - b_Cpt - 1));
	       } // if (pc_EventMask [b_Cpt] == '3')
	       else
	       {
	          if (pc_EventMask [b_Cpt] == '4')
		  {
	  	     *pb_PatternTransition = *pb_PatternTransition | (BYTE) (1 << (b_MaxChannel - b_Cpt - 1));
		  } // if (pc_EventMask [b_Cpt] == '4')
		  else
		  {
		     if (!((pc_EventMask [b_Cpt] == 'x') || 
		        (pc_EventMask [b_Cpt] == 'X')))
		     {
		        i_ReturnValue = -1;
		     } // if (!((pc_EventMask [b_Cpt] == 'x') || (pc_EventMask [b_Cpt] == 'X')))
		  } // if (pc_EventMask [b_Cpt] == '4')
	       } // if (pc_EventMask [b_Cpt] == '3')
	    } // if (pc_EventMask [b_Cpt] == '2')
	 } // if (pc_EventMask [b_Cpt] == '1')
      } // if (pc_EventMask [b_Cpt] == '0')
   } // for (b_Cpt = b_BeginCpt; b_Cpt < b_MaxChannel; b_Cpt ++)
   
   return (i_ReturnValue);
}

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

INT      i_APCI1500_SetInputEventMask        (BYTE    b_BoardHandle,
					      BYTE    b_PortNbr,
					      BYTE    b_Logik,
					      CHAR*  pc_EventMask)
{
   BYTE b_Cpt                  = 0;
   BYTE b_RegValue             = 0;
   INT  i_ReturnValue          = 0;
   BYTE b_PatternMask          = 0;
   BYTE b_PatternPolarity      = 0;
   BYTE b_PatternTransition    = 0;
   BYTE b_PatternTransitionCpt = 0;

   /*******************************************************/
   /* Gets the pointer to the board information structure */
   /*******************************************************/
   str_APCI1500_BoardInformation *BoardInformation = NULL; 
   
   BoardInformation = &(s_APCI1500_DriverStruct.
                        s_BoardInformation[b_BoardHandle]);

   /***************************/
   /* Test if board handle OK */
   /***************************/
   if (b_BoardHandle < s_APCI1500_DriverStruct.b_NumberOfBoard)
   {
      /*******************************************************/
      /* Test if interrupt function for this board installed */
      /*******************************************************/

      if (BoardInformation-> s_InterruptInformation.
  	                     b_InterruptInitialised == APCI1500_ENABLE)
      {
         /*******************************/
         /* Converts and tests the word */
         /*******************************/

         if (i_ConvertString  (pc_EventMask,
   			      (BYTE)   b_PortNbr,
			      (PBYTE) &b_PatternPolarity,
			      (PBYTE) &b_PatternTransition,
			      (PBYTE) &b_PatternMask) == 0)
         {
	    /*************************/
	    /* Tests the port number */
	    /*************************/

	    if (b_PortNbr == 1 || b_PortNbr == 2)
	    {
	       /************************************/
	       /* Test if event setting for port 1 */
	       /************************************/

	       if (b_PortNbr == 1)
	       {
		  /****************************/
		  /* Test the interrupt logik */
		  /****************************/

		  if (b_Logik == APCI1500_AND ||
		      b_Logik == APCI1500_OR  ||
		      b_Logik == APCI1500_OR_PRIORITY)
		  {
		     /**************************************/
		     /* Tests if a transition was declared */
		     /* for a OR PRIORITY logic            */
		     /**************************************/

		     if (b_Logik == APCI1500_OR_PRIORITY && b_PatternTransition != 0)
		     {
		        /********************************************/
			/* Transition error on an OR PRIORITY logic */
			/********************************************/

			i_ReturnValue = -7;
		     }

		     /*************************************/
		     /* Tests if more than one transition */
		     /* was declared for an AND logic     */
		     /*************************************/

		     if (b_Logik == APCI1500_AND)
		     {
		        for (b_Cpt = 0; b_Cpt < 8; b_Cpt++)
		        {
		           b_PatternTransitionCpt = b_PatternTransitionCpt + ((b_PatternTransition >> b_Cpt) & 1);
		        } // for (b_Cpt = 0; b_Cpt < 8; b_Cpt++)

		        if (b_PatternTransitionCpt > 1)
		        {
		           /****************************************/
		           /* Transition error on an AND logic     */
		           /****************************************/

		           i_ReturnValue = -6;
		        } // if (b_PatternTransitionCpt > 1)
		     } // if (b_Logik == APCI1500_AND)

		     /**************************/
		     /* Test if no error occur */
		     /**************************/

		     if (!i_ReturnValue)
		     {
		        /*****************************************************************/
			/* Selects the APCI1500_RW_MASTER_CONFIGURATION_CONTROL register */
			/*****************************************************************/

			outb (APCI1500_RW_MASTER_CONFIGURATION_CONTROL,
                              BoardInformation->s_BaseInformation.
  	                                        ui_BaseAddress[1] + 
                                                APCI1500_Z8536_CONTROL_REGISTER);

			/******************/
			/* Disable Port A */
			/******************/

			outb (0xF0,
                              BoardInformation->s_BaseInformation.
  	                                        ui_BaseAddress[1] + 
                                                APCI1500_Z8536_CONTROL_REGISTER);

			/**********************************************/
			/* Selects the polarity register of port 1    */
			/**********************************************/

			outb (APCI1500_RW_PORT_A_PATTERN_POLARITY,
                              BoardInformation->s_BaseInformation.
  	                                        ui_BaseAddress[1] + 
                                                APCI1500_Z8536_CONTROL_REGISTER);

			/******************************************/
			/* Writes the new value in the polarity   */
			/* register of port 1                     */
			/******************************************/

			outb (b_PatternPolarity,
                              BoardInformation->s_BaseInformation.
  	                                        ui_BaseAddress[1] + 
                                                APCI1500_Z8536_CONTROL_REGISTER);


			/*********************************************/
			/* Selects the pattern mask register of      */
			/* port 1                                    */
			/*********************************************/

			outb (APCI1500_RW_PORT_A_PATTERN_MASK,
                              BoardInformation->s_BaseInformation.
  	                                        ui_BaseAddress[1] + 
                                                APCI1500_Z8536_CONTROL_REGISTER);

			/**********************************************/
			/* Writes the new value in the pattern mask   */
			/* register of port 1                         */
			/**********************************************/

			outb (b_PatternMask,
                              BoardInformation->s_BaseInformation.
  	                                        ui_BaseAddress[1] + 
                                                APCI1500_Z8536_CONTROL_REGISTER);

			/********************************************/
			/* Selects the pattern transition register  */
			/* of port 1                                */
			/********************************************/

			outb (APCI1500_RW_PORT_A_PATTERN_TRANSITION,
                              BoardInformation->s_BaseInformation.
  	                                        ui_BaseAddress[1] + 
                                                APCI1500_Z8536_CONTROL_REGISTER);

			/*****************************************/
			/* Writes the new value in the pattern   */
			/* transition register of port 1         */
			/*****************************************/

			outb (b_PatternTransition,
                              BoardInformation->s_BaseInformation.
  	                                        ui_BaseAddress[1] + 
                                                APCI1500_Z8536_CONTROL_REGISTER);


			/******************************************/
			/* Selects the mode specification mask    */
			/* register of port 1                     */
			/******************************************/

			outb (APCI1500_RW_PORT_A_SPECIFICATION,
                              BoardInformation->s_BaseInformation.
  	                                        ui_BaseAddress[1] + 
                                                APCI1500_Z8536_CONTROL_REGISTER);

			/*******************/
			/* Reads the value */
			/*******************/

			b_RegValue = inb (BoardInformation->s_BaseInformation.
  	                                                    ui_BaseAddress[1] + 
                                                            APCI1500_Z8536_CONTROL_REGISTER);

			/******************************************/
			/* Selects the mode specification mask    */
			/* register of port 1                     */
			/******************************************/

			outb (APCI1500_RW_PORT_A_SPECIFICATION,
                              BoardInformation->s_BaseInformation.
  	                                        ui_BaseAddress[1] + 
                                                APCI1500_Z8536_CONTROL_REGISTER);

			/**********************/
			/* Port A new mode    */
			/**********************/

			outb ((BYTE) ((b_RegValue & 0xF9) | b_Logik),
                              BoardInformation->s_BaseInformation.
  	                                        ui_BaseAddress[1] + 
                                                APCI1500_Z8536_CONTROL_REGISTER);

			BoardInformation-> s_EventInformation.
			                   b_Event1Status = APCI1500_ENABLE;

			BoardInformation-> s_EventInformation.
			                   b_Event1InterruptLogik = b_Logik;

			/*****************************************************************/
			/* Selects the APCI1500_RW_MASTER_CONFIGURATION_CONTROL register */
			/*****************************************************************/

			outb (APCI1500_RW_MASTER_CONFIGURATION_CONTROL,
                              BoardInformation->s_BaseInformation.
  	                                        ui_BaseAddress[1] + 
                                                APCI1500_Z8536_CONTROL_REGISTER);

			/*****************/
			/* Enable Port A */
			/*****************/

			outb (0xF4,
                              BoardInformation->s_BaseInformation.
  	                                        ui_BaseAddress[1] + 
                                                APCI1500_Z8536_CONTROL_REGISTER);
			
		     } // if (!i_ReturnValue)
		  } // if (b_Logik == APCI1500_AND || b_Logik == APCI1500_OR  || b_Logik == APCI1500_OR_PRIORITY)
		  else
		  {
		     /**************************/
		     /* Logic parameter error  */
		     /**************************/

		     i_ReturnValue = -3;
		  }
	       } // if (b_PortNbr == 1)

	       /************************************/
	       /* Test if event setting for port 2 */
	       /************************************/

	       if (b_PortNbr == 2)
	       {
	          /************************/
	          /* Test the event logik */
	          /************************/

	          if (b_Logik == APCI1500_OR)
	          {

		     /*****************************************************************/
		     /* Selects the APCI1500_RW_MASTER_CONFIGURATION_CONTROL register */
		     /*****************************************************************/

		     outb (APCI1500_RW_MASTER_CONFIGURATION_CONTROL,
                           BoardInformation->s_BaseInformation.
  	                                     ui_BaseAddress[1] + 
                                             APCI1500_Z8536_CONTROL_REGISTER);

		     /******************/
		     /* Disable Port B */
		     /******************/

		     outb (0x74,
                           BoardInformation->s_BaseInformation.
  	                                     ui_BaseAddress[1] + 
                                             APCI1500_Z8536_CONTROL_REGISTER);

		     /****************************************/
		     /* Selects the mode specification mask  */
		     /* register of port B                   */
		     /****************************************/

		     outb (APCI1500_RW_PORT_B_SPECIFICATION,
                           BoardInformation->s_BaseInformation.
  	                                     ui_BaseAddress[1] + 
                                             APCI1500_Z8536_CONTROL_REGISTER);

		     /*******************/
		     /* Reads the value */
		     /*******************/

		     b_RegValue = inb (
BoardInformation->s_BaseInformation.
  	                                                 ui_BaseAddress[1] + 
                                                         APCI1500_Z8536_CONTROL_REGISTER);

		     /******************************************/
		     /* Selects the mode specification mask    */
		     /* register of port B                     */
		     /******************************************/

		     outb (APCI1500_RW_PORT_B_SPECIFICATION,
                           BoardInformation->s_BaseInformation.
  	                                     ui_BaseAddress[1] + 
                                             APCI1500_Z8536_CONTROL_REGISTER);

		     /**********************/
		     /* Port B new mode    */
		     /**********************/

		     outb ((BYTE) (b_RegValue & 0xF9),
                           BoardInformation->s_BaseInformation.
  	                                     ui_BaseAddress[1] + 
                                             APCI1500_Z8536_CONTROL_REGISTER);

		     /**********************************/
		     /* Selects error channels 1 and 2 */
		     /**********************************/

		     b_PatternMask       = (BYTE) (b_PatternMask       | 0xC0);
		     b_PatternPolarity   = (BYTE) (b_PatternPolarity   | 0xC0);
		     b_PatternTransition = (BYTE) (b_PatternTransition | 0xC0);

		     /**********************************************/
		     /* Selects the polarity register of port 2    */
		     /**********************************************/

		     outb (APCI1500_RW_PORT_B_PATTERN_POLARITY,
                           BoardInformation->s_BaseInformation.
  	                                     ui_BaseAddress[1] + 
                                             APCI1500_Z8536_CONTROL_REGISTER);

		     /*******************************************/
		     /* Writes the new value in the polarity    */
		     /* register of port 2                      */
		     /*******************************************/

		     outb (b_PatternPolarity,
                           BoardInformation->s_BaseInformation.
  	                                     ui_BaseAddress[1] + 
                                             APCI1500_Z8536_CONTROL_REGISTER);

		     /**********************************************/
		     /* Selects the pattern transition register    */
		     /* of port 2                                  */
		     /**********************************************/

		     outb (APCI1500_RW_PORT_B_PATTERN_TRANSITION,
                           BoardInformation->s_BaseInformation.
  	                                     ui_BaseAddress[1] + 
                                             APCI1500_Z8536_CONTROL_REGISTER);

		     /******************************************/
		     /* Write the new value in the pattern     */
		     /* transition register of port 2          */
		     /******************************************/

		     outb (b_PatternTransition,
                           BoardInformation->s_BaseInformation.
  	                                     ui_BaseAddress[1] + 
                                             APCI1500_Z8536_CONTROL_REGISTER);

		     /**********************************************/
		     /* Selects the pattern transition register    */
		     /* of port 2                                  */
		     /**********************************************/

		     outb (APCI1500_RW_PORT_B_PATTERN_MASK,
                           BoardInformation->s_BaseInformation.
  	                                     ui_BaseAddress[1] + 
                                             APCI1500_Z8536_CONTROL_REGISTER);

		     /*********************************************/
		     /* Writes the new value in the pattern mask  */
		     /* register of port 2                        */
		     /*********************************************/

		     outb (b_PatternMask,
                           BoardInformation->s_BaseInformation.
  	                                     ui_BaseAddress[1] + 
                                             APCI1500_Z8536_CONTROL_REGISTER);

		     /******************************************/
		     /* Selects the mode specification mask    */
		     /* register of port 2                     */
		     /******************************************/

		     outb (APCI1500_RW_PORT_B_SPECIFICATION,
                           BoardInformation->s_BaseInformation.
  	                                     ui_BaseAddress[1] + 
                                             APCI1500_Z8536_CONTROL_REGISTER);

		     /***************+***/
		     /* Reads the value */
		     /****************+**/

		     b_RegValue = inb (BoardInformation->s_BaseInformation.
  	                                                 ui_BaseAddress[1] + 
                                                         APCI1500_Z8536_CONTROL_REGISTER
);

		     /******************************************/
		     /* Selects the mode specification mask    */
		     /* register of port 2                     */
		     /******************************************/

		     outb (APCI1500_RW_PORT_B_SPECIFICATION,
                           BoardInformation->s_BaseInformation.
  	                                     ui_BaseAddress[1] + 
                                             APCI1500_Z8536_CONTROL_REGISTER);

		     /**********************/
		     /* Port 2 new mode    */
		     /**********************/

		     outb ((BYTE) ((b_RegValue & 0xF9) | 4),
                           BoardInformation->s_BaseInformation.
  	                                     ui_BaseAddress[1] + 
                                             APCI1500_Z8536_CONTROL_REGISTER);

		     BoardInformation-> s_EventInformation.
		                        b_Event2Status = APCI1500_ENABLE;

		     /*****************************************************************/
		     /* Selects the APCI1500_RW_MASTER_CONFIGURATION_CONTROL register */
		     /*****************************************************************/

		     outb (APCI1500_RW_MASTER_CONFIGURATION_CONTROL,
                           BoardInformation->s_BaseInformation.
  	                                     ui_BaseAddress[1] + 
                                             APCI1500_Z8536_CONTROL_REGISTER);

		     /*****************/
		     /* Enable Port B */
		     /*****************/

		     outb (0xF4,
                           BoardInformation->s_BaseInformation.
  	                                     ui_BaseAddress[1] + 
                                             APCI1500_Z8536_CONTROL_REGISTER);

		  } // if (b_Logik == APCI1500_OR)
		  else
		  {
		     /**************************/
		     /* Logic parameter error  */
		     /**************************/

		     i_ReturnValue = -3;
		  } // if (b_Logik == APCI1500_OR)
	       } // if (b_PortNbr == 2)
	    } // if (b_PortNbr == 1 || b_PortNbr == 2)
	    else
	    {
	       /**************************************/
	       /* Port number is not between 1 and 2 */
	       /**************************************/

	       i_ReturnValue = -2;
	    } // if (b_PortNbr == 1 || b_PortNbr == 2)
	 } // if (i_ConvertString  (...) == 0)
	 else
	 {
	    /********************************/
	    /* Event mask parameter error   */
	    /********************************/

	    i_ReturnValue = -4;
         } // if (i_ConvertString  (...) == 0)
     } 
     else
     {
        /***************************************/
        /* Interrupt routine is not installed  */
        /***************************************/

        i_ReturnValue = -5;
     } // if (((ps_APCI1500_GlobalStruct->s_ProcessInformation [b_ProcessIndex].s_ProcessInterruptInformations.w_BoardHandleList >> b_BoardHandle) & 1) == 1)

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

INT      i_APCI1500_StartInputEvent       (BYTE       b_BoardHandle,
				           BYTE       b_PortNbr)
{
   INT   i_ReturnValue     = 0;

   /*******************************************************/
   /* Gets the pointer to the board information structure */
   /*******************************************************/
   str_APCI1500_BoardInformation *BoardInformation = NULL; 
   
   BoardInformation = &(s_APCI1500_DriverStruct.
                        s_BoardInformation[b_BoardHandle]);

   /***************************/
   /* Test if board handle OK */
   /***************************/
   if (b_BoardHandle < s_APCI1500_DriverStruct.b_NumberOfBoard)
   {
      /*******************************************************/
      /* Test if interrupt function for this board installed */
      /*******************************************************/

      if (BoardInformation-> s_InterruptInformation.
  	                     b_InterruptInitialised == APCI1500_ENABLE)
      {
	 /*************************/
	 /* Tests the port number */
	 /*************************/

	 if (b_PortNbr == 1 || b_PortNbr == 2)
	 {
	    /***************************/
	    /* Test if port 1 selected */
	    /***************************/

	    if (b_PortNbr == 1)
	    {
	       /*****************************/
	       /* Test if event initialised */
	       /*****************************/

	       if (BoardInformation-> s_EventInformation.
		                      b_Event1Status == APCI1500_ENABLE)
	       {
		  /*********************************************/
		  /* Test if no process have enabled the event */
		  /*********************************************/

		  if (BoardInformation-> s_EventInformation.
			                    b_ProcessEnableEvent1Status == 0)
		  {
		     /*****************************************************************/
		     /* Selects the APCI1500_RW_MASTER_CONFIGURATION_CONTROL register */
		     /*****************************************************************/

		     outb (APCI1500_RW_MASTER_CONFIGURATION_CONTROL,
                           BoardInformation->s_BaseInformation.
  	                                     ui_BaseAddress[1] + 
                                             APCI1500_Z8536_CONTROL_REGISTER);

		     /******************/
		     /* Disable Port A */
		     /******************/

		     outb (0xF0,
                           BoardInformation->s_BaseInformation.
  	                                     ui_BaseAddress[1] + 
                                             APCI1500_Z8536_CONTROL_REGISTER);

		     /***************************************************/
		     /* Selects the command and status register of      */
		     /* port 1                                          */
		     /***************************************************/

		     outb (APCI1500_RW_PORT_A_COMMAND_AND_STATUS,
                           BoardInformation->s_BaseInformation.
  	                                     ui_BaseAddress[1] + 
                                             APCI1500_Z8536_CONTROL_REGISTER);

		     /*************************************/
		     /* Allows the pattern interrupt      */
		     /*************************************/

		     outb (0xC0,
                           BoardInformation->s_BaseInformation.
  	                                     ui_BaseAddress[1] + 
                                             APCI1500_Z8536_CONTROL_REGISTER);

		     /*****************************************************************/
		     /* Selects the APCI1500_RW_MASTER_CONFIGURATION_CONTROL register */
		     /*****************************************************************/

		     outb (APCI1500_RW_MASTER_CONFIGURATION_CONTROL,
                           BoardInformation->s_BaseInformation.
  	                                     ui_BaseAddress[1] + 
                                             APCI1500_Z8536_CONTROL_REGISTER);

		     /*****************/
		     /* Enable Port A */
		     /*****************/

	             outb (0xF4,
                           BoardInformation->s_BaseInformation.
  	                                     ui_BaseAddress[1] + 
                                             APCI1500_Z8536_CONTROL_REGISTER);
                     BoardInformation-> s_EventInformation.
			                b_ProcessEnableEvent1Status = APCI1500_ENABLE;
		  } 
	       } // if (ps_APCI1500_GlobalStruct->s_SingleBoardInformation [b_BoardHandle].s_EventInfos.b_Event1Status == APCI1500_ENABLE)
	       else
	       {
	          /*****************************/
	          /* Event not initialised     */
	          /*****************************/

	          i_ReturnValue = -3;
	       } // if (ps_APCI1500_GlobalStruct->s_SingleBoardInformation [b_BoardHandle].s_EventInfos.b_Event1Status == APCI1500_ENABLE)
	    } // if (b_PortNbr == 1)

	    /***************************/
	    /* Test if port 2 selected */
	    /***************************/

	    if (b_PortNbr == 2)
	    {
	       /*****************************/
	       /* Test if event initialised */
	       /*****************************/

	       if (BoardInformation-> s_EventInformation.
	                              b_Event2Status == APCI1500_ENABLE)
	       {
                  BoardInformation-> s_EventInformation.
		                     b_ProcessEnableEvent2Status = APCI1500_ENABLE;
	       } // if (ps_APCI1500_GlobalStruct->s_SingleBoardInformation [b_BoardHandle].s_EventInfos.b_Event2Status == APCI1500_ENABLE)
	       else
	       {
	          /*****************************/
	          /* Event not initialised     */
	          /*****************************/

	          i_ReturnValue = -3;
	       } // if (ps_APCI1500_GlobalStruct->s_SingleBoardInformation [b_BoardHandle].s_EventInfos.b_Event2Status == APCI1500_ENABLE)
	    } // if (b_PortNbr == 2)
	 } // if (b_PortNbr == 1 || b_PortNbr == 2)
	 else
	 {
	    /********************************/
	    /* Port number parameter error  */
	    /********************************/

	    i_ReturnValue = -2;
	 } // if (b_PortNbr == 1 || b_PortNbr == 2)
      } 
      else
      {
         /*****************************/
         /* Event not initialised     */
         /*****************************/

         i_ReturnValue = -3;
      } 

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

INT     i_APCI1500_StopInputEvent         (BYTE  b_BoardHandle,
					   BYTE  b_PortNbr)
{
   INT   i_ReturnValue     = 0;

   /*******************************************************/
   /* Gets the pointer to the board information structure */
   /*******************************************************/
   str_APCI1500_BoardInformation *BoardInformation = NULL; 
   
   BoardInformation = &(s_APCI1500_DriverStruct.
                        s_BoardInformation[b_BoardHandle]);

   /***************************/
   /* Test if board handle OK */
   /***************************/
   if (b_BoardHandle < s_APCI1500_DriverStruct.b_NumberOfBoard)
   {
      /*************************/
      /* Tests the port number */
      /*************************/

      if (b_PortNbr == 1 || b_PortNbr == 2)
      {
	 /***************************/
	 /* Test if port 1 selected */
	 /***************************/

	 if (b_PortNbr == 1)
	 {
	    /*****************************/
	    /* Test if event initialised */
	    /*****************************/

	    if (BoardInformation-> s_EventInformation.
		                     b_Event1Status == APCI1500_ENABLE)
	    {

	       outb (APCI1500_RW_MASTER_CONFIGURATION_CONTROL,
                     BoardInformation->s_BaseInformation.
  	                               ui_BaseAddress[1] + 
                                       APCI1500_Z8536_CONTROL_REGISTER);

	       /******************/
	       /* Disable Port A */
	       /******************/

	       outb (0xF0,
                     BoardInformation->s_BaseInformation.
  	                               ui_BaseAddress[1] + 
                                       APCI1500_Z8536_CONTROL_REGISTER);

	       /************************************************/
	       /* Selects the command and status register of   */
	       /* port 1                                       */
	       /************************************************/

	       outb (APCI1500_RW_PORT_A_COMMAND_AND_STATUS,
                     BoardInformation->s_BaseInformation.
  	                               ui_BaseAddress[1] + 
                                       APCI1500_Z8536_CONTROL_REGISTER);

	       /**********************************/
	       /* Inhibits the pattern interrupt */
	       /**********************************/

	       outb (0xE0,
                     BoardInformation->s_BaseInformation.
  	                               ui_BaseAddress[1] + 
                                       APCI1500_Z8536_CONTROL_REGISTER);

	       /*****************************************************************/
	       /* Selects the APCI1500_RW_MASTER_CONFIGURATION_CONTROL register */
	       /*****************************************************************/

	       outb (APCI1500_RW_MASTER_CONFIGURATION_CONTROL,
                     BoardInformation->s_BaseInformation.
  	                               ui_BaseAddress[1] + 
                                       APCI1500_Z8536_CONTROL_REGISTER);

	       /*****************/
	       /* Enable Port A */
	       /*****************/

	       outb (0xF4,
                     BoardInformation->s_BaseInformation.
  	                               ui_BaseAddress[1] + 
                                       APCI1500_Z8536_CONTROL_REGISTER);
		  
               BoardInformation-> s_EventInformation.
	  	                  b_ProcessEnableEvent1Status = APCI1500_DISABLE;
	    } 
	    else
	    {
	       /*****************************/
	       /* Event not initialized     */
	       /*****************************/

	       i_ReturnValue = -3;
	    } // if (ps_APCI1500_GlobalStruct->s_SingleBoardInformation [b_BoardHandle].s_EventInfos.b_Event1Status == APCI1500_ENABLE)
	 } // if (b_PortNbr == 1)

	 /***************************/
	 /* Test if port 2 selected */
	 /***************************/

	 if (b_PortNbr == 2)
	 {
	    /*****************************/
	    /* Test if event initialised */
	    /*****************************/

	    if (BoardInformation-> s_EventInformation.
		                      b_Event2Status == APCI1500_ENABLE)
	    {
               BoardInformation-> s_EventInformation.
	  	                  b_ProcessEnableEvent2Status = APCI1500_DISABLE;
	    } // if (ps_APCI1500_GlobalStruct->s_SingleBoardInformation [b_BoardHandle].s_EventInfos.b_Event2Status == APCI1500_ENABLE)
	    else
	    {
	       /*****************************/
	       /* Event not initialized     */
	       /*****************************/

	       i_ReturnValue = -3;
	    } // if (ps_APCI1500_GlobalStruct->s_SingleBoardInformation [b_BoardHandle].s_EventInfos.b_Event2Status == APCI1500_ENABLE)
	 } // if (b_PortNbr == 2)
      } // if (b_PortNbr == 1 || b_PortNbr == 2)
      else
      {
         /********************************/
         /* Port number parameter error  */
         /********************************/

         i_ReturnValue = -2;
      } // if (b_PortNbr == 1 || b_PortNbr == 2)

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


/*
+----------------------------------------------------------------------------+
| Function name   : INT        i_APCI1500_SetBoardInterruptRoutine           |
|                                              (BYTE     b_BoardHandle)      |
+----------------------------------------------------------------------------+
| Task              : Initialises the interruptroutine for the APCI-1500     |
+----------------------------------------------------------------------------+
| Input parameters  : BYTE   b_BoardHandle :Handle of the APCI-1500          |
+----------------------------------------------------------------------------+
| Output Parameters : ---                                                    |
+----------------------------------------------------------------------------+
| Return Value      :  0 : No error                                          |
|                     -1 : The handle parameter of the board is wrong        |
+----------------------------------------------------------------------------+
*/

INT      i_APCI1500_SetBoardInterruptRoutine (BYTE    b_BoardHandle,
                                              void (*usrHdl) (BYTE handle, BYTE intMask))
{
       INT    i_ReturnValue          = 0;
       BYTE   b_RegValue             = 0;
       ULONG ul_Status               = 0;

   /*******************************************************/
   /* Gets the pointer to the board Information structure */
   /*******************************************************/
   str_APCI1500_BoardInformation *BoardInformation = NULL; 
   
   BoardInformation = &(s_APCI1500_DriverStruct.
                        s_BoardInformation[b_BoardHandle]);

   /***************************/
   /* Test if board handle OK */
   /***************************/
   if (b_BoardHandle < s_APCI1500_DriverStruct.b_NumberOfBoard)
   {
      /****************************************/
      /* Test if interrupt allready installed */
      /****************************************/

      if (BoardInformation -> s_InterruptInformation.
		             b_InterruptInitialised == APCI1500_DISABLE)
      {
         
	 
         /***********************************/
	 /* Test if interrupt not installed */
         /***********************************/
	 
	 if (s_APCI1500_DriverStruct. 
	     s_GlobalInterruptInf [BoardInformation->
	                           s_BaseInformation.
	                           b_Interrupt].
            b_InstallationCpt == 0)
	 {
	    /******************************************************************/
	    /* Get the user interrupt, only one user interrupt for the driver */	 
	    /******************************************************************/
	    if (s_APCI1500_DriverStruct.usrHdl == NULL)	    
               s_APCI1500_DriverStruct.usrHdl = usrHdl;
            	 	
            i_ReturnValue = request_irq(BoardInformation->s_BaseInformation.
	                                                  b_Interrupt, 
				        APCI1500_Interrupt, 
				        SA_SHIRQ, 
				        "APCI1500", 
				        &(s_APCI1500_DriverStruct.
					s_GlobalInterruptInf [BoardInformation->
					                      s_BaseInformation.
	                                                      b_Interrupt]));	
	 }
	   
         /*****************************************/
	 /* Increment the number of installations */
         /*****************************************/
	   
	 s_APCI1500_DriverStruct. 
         s_GlobalInterruptInf [BoardInformation->
	                       s_BaseInformation.
	                       b_Interrupt].
         b_InstallationCpt++;
	 
         /*************************/
	 /* Save the board handle */
         /*************************/
			       
	 s_APCI1500_DriverStruct. 
         s_GlobalInterruptInf [BoardInformation->
	                       s_BaseInformation.
	                       b_Interrupt].
	 dw_BoardHandleArray = s_APCI1500_DriverStruct. 
                               s_GlobalInterruptInf [BoardInformation->
	                                             s_BaseInformation.
	                                             b_Interrupt].
	                       dw_BoardHandleArray | (1 << b_BoardHandle);

/*         Reset

	   
	 s_APCI1500_DriverStruct. 
         s_GlobalInterruptInf [BoardInformation->
	                       s_BaseInformation.
	                       b_Interrupt].
	 dw_BoardHandleArray = s_APCI1500_DriverStruct. 
                               s_GlobalInterruptInf [BoardInformation->
	                                             s_BaseInformation.
	                                             b_Interrupt].
	                       dw_BoardHandleArray & (0xFFFFFFFF - (1 << b_BoardHandle));
*/	

         outl (0x3000UL,
	       BoardInformation->s_BaseInformation.
	                          ui_BaseAddress[0]+
	                          0x38);
				     
	 ul_Status = inl (BoardInformation->s_BaseInformation.
		                             ui_BaseAddress[0]+
		                             0x10);

	 ul_Status = inl (BoardInformation->s_BaseInformation.
		                             ui_BaseAddress[0]+
		                             0x38);
    
         outl (0x23000UL,
               BoardInformation->s_BaseInformation.
   	                          ui_BaseAddress[0]+
		                  0x38);
				     
         outb (APCI1500_RW_PORT_B_SPECIFICATION,
               BoardInformation->s_BaseInformation.
		                  ui_BaseAddress[1]+
		                  APCI1500_Z8536_CONTROL_REGISTER);
			      
         b_RegValue = inb (BoardInformation->s_BaseInformation.
		                              ui_BaseAddress[1]+
		                              APCI1500_Z8536_CONTROL_REGISTER);

         outb (APCI1500_RW_PORT_B_SPECIFICATION,
	       BoardInformation->s_BaseInformation.
	                          ui_BaseAddress[1]+
	                          APCI1500_Z8536_CONTROL_REGISTER);
			      
         b_RegValue = (BYTE) ((b_RegValue & 0xF9) | APCI1500_OR);		      
	    
         outb (b_RegValue,
	       BoardInformation->s_BaseInformation.
	                          ui_BaseAddress[1]+
	                          APCI1500_Z8536_CONTROL_REGISTER);
			      
         outb (APCI1500_RW_PORT_B_COMMAND_AND_STATUS,
	       BoardInformation->s_BaseInformation.
	                          ui_BaseAddress[1]+
	                          APCI1500_Z8536_CONTROL_REGISTER);
			      
         outb (0xC0,
	       BoardInformation->s_BaseInformation.
	                          ui_BaseAddress[1]+
	                          APCI1500_Z8536_CONTROL_REGISTER);

         outb (APCI1500_RW_PORT_B_PATTERN_POLARITY,
	       BoardInformation->s_BaseInformation.
	                          ui_BaseAddress[1]+
	                          APCI1500_Z8536_CONTROL_REGISTER);

            outb (0xC0,
	          BoardInformation->s_BaseInformation.
		                     ui_BaseAddress[1]+
		                     APCI1500_Z8536_CONTROL_REGISTER);
			      
            outb (APCI1500_RW_PORT_B_PATTERN_TRANSITION,
	          BoardInformation->s_BaseInformation.
		                     ui_BaseAddress[1]+
		                     APCI1500_Z8536_CONTROL_REGISTER);

            outb (0xC0,
	          BoardInformation->s_BaseInformation.
		                     ui_BaseAddress[1]+
		                     APCI1500_Z8536_CONTROL_REGISTER);
			      
            outb (APCI1500_RW_PORT_B_PATTERN_MASK,
	          BoardInformation->s_BaseInformation.
		                     ui_BaseAddress[1]+
		                     APCI1500_Z8536_CONTROL_REGISTER);

            outb (0xC0,
	          BoardInformation->s_BaseInformation.
		                     ui_BaseAddress[1]+
		                     APCI1500_Z8536_CONTROL_REGISTER);
			      
				     
            outb (APCI1500_RW_PORT_A_COMMAND_AND_STATUS,
	          BoardInformation->s_BaseInformation.
		                     ui_BaseAddress[1]+
		                     APCI1500_Z8536_CONTROL_REGISTER);
			      
            b_RegValue = inb (BoardInformation->
	                                  s_BaseInformation.
		                          ui_BaseAddress[1]+
		                          APCI1500_Z8536_CONTROL_REGISTER);

            outb (APCI1500_RW_PORT_A_COMMAND_AND_STATUS,
	          BoardInformation->s_BaseInformation.
		                     ui_BaseAddress[1]+
		                     APCI1500_Z8536_CONTROL_REGISTER);
			      
            outb ((BYTE) ((b_RegValue & 0x0F) | 0x20),
	          BoardInformation->s_BaseInformation.
		                     ui_BaseAddress[1]+
		                     APCI1500_Z8536_CONTROL_REGISTER);
			      
            outb (APCI1500_RW_PORT_B_COMMAND_AND_STATUS,
	          BoardInformation->s_BaseInformation.
		                     ui_BaseAddress[1]+
		                     APCI1500_Z8536_CONTROL_REGISTER);
			      
            b_RegValue = inb (BoardInformation->
	                                  s_BaseInformation.
		                          ui_BaseAddress[1]+
		                          APCI1500_Z8536_CONTROL_REGISTER);

            outb (APCI1500_RW_PORT_B_COMMAND_AND_STATUS,
	          BoardInformation->s_BaseInformation.
		                     ui_BaseAddress[1]+
		                     APCI1500_Z8536_CONTROL_REGISTER);
			      
            outb ((BYTE) ((b_RegValue & 0x0F) | 0x20),
	          BoardInformation->s_BaseInformation.
		                     ui_BaseAddress[1]+
		                     APCI1500_Z8536_CONTROL_REGISTER);
			      
            outb (APCI1500_RW_CPT_TMR1_CMD_STATUS,
	          BoardInformation->s_BaseInformation.
		                     ui_BaseAddress[1]+
		                     APCI1500_Z8536_CONTROL_REGISTER);
			      
            b_RegValue = inb (BoardInformation->
	                                  s_BaseInformation.
		                          ui_BaseAddress[1]+
		                          APCI1500_Z8536_CONTROL_REGISTER);

            outb (APCI1500_RW_CPT_TMR1_CMD_STATUS,
	          BoardInformation->s_BaseInformation.
		                     ui_BaseAddress[1]+
		                     APCI1500_Z8536_CONTROL_REGISTER);
			      
            outb ((BYTE) ((b_RegValue & 0x0F) | 0x20),
	          BoardInformation->s_BaseInformation.
		                     ui_BaseAddress[1]+
		                     APCI1500_Z8536_CONTROL_REGISTER);
			      
            outb (APCI1500_RW_CPT_TMR2_CMD_STATUS,
	          BoardInformation->s_BaseInformation.
		                     ui_BaseAddress[1]+
		                     APCI1500_Z8536_CONTROL_REGISTER);
			      
            b_RegValue = inb (BoardInformation->
	                                  s_BaseInformation.
		                          ui_BaseAddress[1]+
		                          APCI1500_Z8536_CONTROL_REGISTER);

            outb (APCI1500_RW_CPT_TMR2_CMD_STATUS,
	          BoardInformation->s_BaseInformation.
		                     ui_BaseAddress[1]+
		                     APCI1500_Z8536_CONTROL_REGISTER);
			      
            outb ((BYTE) ((b_RegValue & 0x0F) | 0x20),
	          BoardInformation->s_BaseInformation.
		                     ui_BaseAddress[1]+
		                     APCI1500_Z8536_CONTROL_REGISTER);
			      
            outb (APCI1500_RW_CPT_TMR3_CMD_STATUS,
	          BoardInformation->s_BaseInformation.
		                     ui_BaseAddress[1]+
		                     APCI1500_Z8536_CONTROL_REGISTER);
			      
            b_RegValue = inb (BoardInformation->
	                                  s_BaseInformation.
		                          ui_BaseAddress[1]+
		                          APCI1500_Z8536_CONTROL_REGISTER);

            outb (APCI1500_RW_CPT_TMR3_CMD_STATUS,
	          BoardInformation->s_BaseInformation.
		                     ui_BaseAddress[1]+
		                     APCI1500_Z8536_CONTROL_REGISTER);
			      
            outb ((BYTE) ((b_RegValue & 0x0F) | 0x20),
	          BoardInformation->s_BaseInformation.
		                     ui_BaseAddress[1]+
		                     APCI1500_Z8536_CONTROL_REGISTER);
			      
            outb (APCI1500_RW_MASTER_INTERRUPT_CONTROL,
	          BoardInformation->s_BaseInformation.
		                     ui_BaseAddress[1]+
		                     APCI1500_Z8536_CONTROL_REGISTER);
			      
            outb (0xD0,
	          BoardInformation->s_BaseInformation.
		                     ui_BaseAddress[1]+
		                     APCI1500_Z8536_CONTROL_REGISTER);

            BoardInformation-> s_InterruptInformation.
		               b_InterruptInitialised = APCI1500_ENABLE;
      }
      else
      {
         /***************************************/
         /* Interrupt routine already installed */
         /***************************************/

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
   return (i_ReturnValue);
}
      
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

INT      i_APCI1500_ResetBoardInterruptRoutine (BYTE    b_BoardHandle)
{
   INT    i_ReturnValue          = 0;
   

   /*******************************************************/
   /* Gets the pointer to the board Information structure */
   /*******************************************************/
   str_APCI1500_BoardInformation *BoardInformation = NULL; 
   
   BoardInformation = &(s_APCI1500_DriverStruct.
                        s_BoardInformation[b_BoardHandle]);

   /***************************/
   /* Test if board handle OK */
   /***************************/
   if (b_BoardHandle < s_APCI1500_DriverStruct.b_NumberOfBoard)
   {
      /****************************************/
      /* Test if interrupt allready installed */
      /****************************************/

      if (BoardInformation-> s_InterruptInformation.
		             b_InterruptInitialised == APCI1500_ENABLE)
      {
         /*****************************************/
	 /* Decrement the number of installations */
         /*****************************************/
	   
	 s_APCI1500_DriverStruct. 
         s_GlobalInterruptInf [BoardInformation->
	                       s_BaseInformation.
	                       b_Interrupt].
         b_InstallationCpt--;
	 
         /****************************/
	 /* Releases the BoardHandle */
         /****************************/
	 s_APCI1500_DriverStruct. 
         s_GlobalInterruptInf [BoardInformation->
	                       s_BaseInformation.
	                       b_Interrupt].
	 dw_BoardHandleArray = s_APCI1500_DriverStruct. 
                               s_GlobalInterruptInf [BoardInformation->
	                                             s_BaseInformation.
	                                             b_Interrupt].
	                       dw_BoardHandleArray & (0xFFFFFFFF - (1 << b_BoardHandle));

         /*******************************/
	 /* Disables the Interrupt flag */
         /*******************************/
         BoardInformation-> s_InterruptInformation.
		            b_InterruptInitialised = APCI1500_DISABLE;

         /***************************************/
	 /* Test if no more interrupt installed */
         /***************************************/
	 
	 if (s_APCI1500_DriverStruct. 
	     s_GlobalInterruptInf [BoardInformation->
	                           s_BaseInformation.
	                           b_Interrupt].
            b_InstallationCpt == 0)
	 {
	    /*******************************/
	    /* Disable the board interrupt */
	    /*******************************/

	    /*************************************************/
	    /* Selects the master interrupt control register */
 	    /*************************************************/

            outb (APCI1500_RW_MASTER_INTERRUPT_CONTROL,
	          BoardInformation->s_BaseInformation.
		                     ui_BaseAddress[1]+
		                     APCI1500_Z8536_CONTROL_REGISTER);


	    /******************************/
	    /* Deactivates all interrupts */
	    /******************************/

            outb (0,
	          BoardInformation->s_BaseInformation.
		                     ui_BaseAddress[1]+
		                     APCI1500_Z8536_CONTROL_REGISTER);

 	    /*****************************************************/
	    /* Selects the command and status register of port A */
	    /*****************************************************/

            outb (APCI1500_RW_PORT_A_COMMAND_AND_STATUS,
	          BoardInformation->s_BaseInformation.
		                     ui_BaseAddress[1]+
		                     APCI1500_Z8536_CONTROL_REGISTER);

	    /*****************************/
	    /* Deactivates the interrupt */
	    /*****************************/

            outb (0x00,
	          BoardInformation->s_BaseInformation.
		                     ui_BaseAddress[1]+
		                     APCI1500_Z8536_CONTROL_REGISTER);

	    /*****************************************************/
 	    /* Selects the command and status register of port B */
	    /*****************************************************/

            outb (APCI1500_RW_PORT_B_COMMAND_AND_STATUS,
	          BoardInformation->s_BaseInformation.
		                     ui_BaseAddress[1]+
		                     APCI1500_Z8536_CONTROL_REGISTER);

	    /*****************************/
	    /* Deactivates the interrupt */
	    /*****************************/

            outb (0x00,
	          BoardInformation->s_BaseInformation.
		                     ui_BaseAddress[1]+
		                     APCI1500_Z8536_CONTROL_REGISTER);

	    /******************************************************/
	    /* Selects the command and status register of timer 1 */
	    /******************************************************/

            outb (APCI1500_RW_CPT_TMR1_CMD_STATUS,
	          BoardInformation->s_BaseInformation.
		                     ui_BaseAddress[1]+
		                     APCI1500_Z8536_CONTROL_REGISTER);

	    /*****************************/
	    /* Deactivates the interrupt */
	    /*****************************/

            outb (0x00,
	          BoardInformation->s_BaseInformation.
		                     ui_BaseAddress[1]+
		                     APCI1500_Z8536_CONTROL_REGISTER);

	    /******************************************************/
	    /* Selects the command and status register of timer 2 */
	    /******************************************************/

            outb (APCI1500_RW_CPT_TMR2_CMD_STATUS,
	          BoardInformation->s_BaseInformation.
		                     ui_BaseAddress[1]+
		                     APCI1500_Z8536_CONTROL_REGISTER);

	    /*****************************/
	    /* Deactivates the interrupt */
	    /*****************************/

            outb (0x00,
	          BoardInformation->s_BaseInformation.
		                     ui_BaseAddress[1]+
		                     APCI1500_Z8536_CONTROL_REGISTER);

	    /******************************************************/
	    /* Selects the command and status register of timer 3 */
	    /******************************************************/

            outb (APCI1500_RW_CPT_TMR3_CMD_STATUS,
	          BoardInformation->s_BaseInformation.
		                     ui_BaseAddress[1]+
		                     APCI1500_Z8536_CONTROL_REGISTER);

	    /******************************************/
	    /* Deactivates the interrupt and the gate */
	    /******************************************/

            outb (0x00,
	          BoardInformation->s_BaseInformation.
		                     ui_BaseAddress[1]+
		                     APCI1500_Z8536_CONTROL_REGISTER);
	   

            free_irq(BoardInformation->s_BaseInformation.
	                               b_Interrupt, 
				       &(s_APCI1500_DriverStruct.
				       s_GlobalInterruptInf [BoardInformation->
					                     s_BaseInformation.
	                                                     b_Interrupt]));
         }//All Interrupts disabled
      }
      else
      {
         /***********************************/
         /* Interrupt routine not installed */
         /***********************************/

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
   return (i_ReturnValue);
}


INT i_APCI1500_SearchAllAPCI1500 (VOID)
{
    int i                                   = 0;
    struct pci_dev *pPCILinuxStruct         = NULL;
    
    memset (&s_APCI1500_DriverStruct, 0, sizeof(s_APCI1500_DriverStruct));

    printk("\n +------------------------------+\n");
    printk(" | ADDI-DATA GmbH -+- APCI-1500 |\n");
    printk(" +------------------------------+\n");
    printk(" |      Module installation     |\n");
    printk(" +------------------------------+\n");
	
    s_APCI1500_DriverStruct.b_NumberOfBoard = 0;


   if(!pci_present())
   {
      printk("<1> NO PCI BUS;\n");
   }      
   else
   {
      //Filling str_PCI_Information stucture
      while (pPCILinuxStruct = pci_find_device (APCI1500_BOARD_VENDOR_ID, 
                                                APCI1500_BOARD_DEVICE_ID, 
					        pPCILinuxStruct))
      {
         for (i = 0; i < 5; i++)
	 {
            s_APCI1500_DriverStruct.s_BoardInformation
	                           [s_APCI1500_DriverStruct.b_NumberOfBoard]
	                           .s_BaseInformation.ui_BaseAddress[i] = 
			           (UINT) PCI_BASE_ADDRESS_IO_MASK & 
			           pPCILinuxStruct->resource[i].start;
         }
         s_APCI1500_DriverStruct.s_BoardInformation
	                        [s_APCI1500_DriverStruct.b_NumberOfBoard]
	                        .s_BaseInformation.b_Interrupt =
			         (BYTE) pPCILinuxStruct->irq;
         s_APCI1500_DriverStruct.s_BoardInformation
	                        [s_APCI1500_DriverStruct.b_NumberOfBoard]
	                        .s_BaseInformation.b_SlotNumber = 
			        (BYTE)  PCI_SLOT(pPCILinuxStruct->devfn);

         s_APCI1500_DriverStruct.b_NumberOfBoard++;
         printk(" |      NumberOfBoard: %2d       |\n",s_APCI1500_DriverStruct.b_NumberOfBoard);
         printk(" +------------------------------+\n");
         printk(" | Baseaddress[0,1,2]:          |\n");
         printk(" |      %4lX, %4lX, %4lX        |\n", pPCILinuxStruct->resource[0].start, pPCILinuxStruct->resource[1].start, pPCILinuxStruct->resource[2].start);   
         printk(" | Interrupt No.:               |\n");
         printk(" |      %2d                      |\n",pPCILinuxStruct->irq);   
         printk(" +------08.07.2002---08:25------+\n");
		
      
      }
      s_APCI1500_DriverStruct.usrHdl = NULL;
    }
   return (s_APCI1500_DriverStruct.b_NumberOfBoard);    	
}
