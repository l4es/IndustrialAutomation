/*
This file is part of CanFestival, a library implementing CanOpen Stack.

Copyright (C): Edouard TISSERANT and Francis DUPIN

See COPYING file for copyrights details.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


/*
	Wrapper Layer for CANFestival to interact with IXXAT VCI V3.X.X Drivers (vcisdk.lib)
	Provides external references for win32 library see win32.c (CanFestival-3)

	http://www.ixxat.com/download_vci_v3_en.html

	Currently the VCI CAN driver for Windows supports the following IXXAT interfaces:

	PC-I 04/PCI
	iPC-I 320/PCI II
	iPC-I 165/PCI
	iPC-I XC16/PCI
	iPC-I XC16/PMC
	iPC-I XC16/PCIe
	USB-to-CAN compact
	USB-to-CAN II
	CAN-IB100/PCIe
	CAN-IB200/PCIe
	CAN-IB120/PCIe Mini
	CAN-IB130/PCIe 104
	CAN-IB230/PCIe 104
	CAN@net II/VCI
	CANblue II
	FR-IB100/PCIe (only in combination with VCI V2.20)
	tinCAN 161

*/


/*************************************************************************
**    
**************************************************************************
**
**    File: can_ixxat_win32.c
**    Summary: Wrapper to encapsulate handling of VCI3
**	  Include vcisdk.lib	
**
**************************************************************************
**************************************************************************
**
**  Functions:  canOpen_driver
**              canReceive_driver
**              TimerProc1
**              canClose_driver
**				canSend_drive
**				
**/

#include <stdio.h> 
#include "can_driver.h"
#include "def.h"
#include "winuser.h"

//  Include a path to the following header files provided with VCI V3.X.X
//      ...sdk/Microsoft_VisualC/inc
#include "vcinpl.h" 
#include "VCI3.h"  
#include "vcitype.h"
#include "vciguid.h"
#include "vcierr.h" 



/************************************************************************
**   bus status polling cycle  milliseconds
*************************************************************************/
#define STATUS_POLLING_CYCLE 1000

/*************************************************************************
**   function prototypes
*************************************************************************/
void CALLBACK TimerProc1(void* lpParametar, BOOL TimerOrWaitFired );
void Display_Error(HRESULT hResult);


/*************************************************************************
**   static variables 
*************************************************************************/
static HANDLE hDevice;       // device handle
static LONG   lCtrlNo;       // controller number
static HANDLE hCanCtl;       // controller handle 
static HANDLE hCanChn;       // channel handle
static LONG   lMustQuit = 0; // quit flag for the receive thread
static HANDLE hTimerHandle;  // timer handle 

/*************************************************************************
**    datatypes
*************************************************************************/
 
struct sLook_up_table
{
	char baud_rate[20];
	UINT8 bt0;
	UINT8 bt1; 
};

struct sInterface_lookup_table
{
	char board_num[10];
	UINT8 num;
};

/*************************************************************************
**
**    Function      : canOpen_driver
**
**    Description   : Initializes the Control and Message Channels
**    Parameters    : s_BOARD *board		- pointer to board information
**    Returnvalue   : (CAN_HANDLE)board		- handle for CAN controller 
**   
*************************************************************************/
CAN_HANDLE __stdcall canOpen_driver(s_BOARD *board)
{
    HANDLE        hEnumerator;     // enumerator handle
    VCIDEVICEINFO VCIDeviceInfo;   // device info
	HRESULT hResult;
	int index,  boardNum; 
	BOOL bResult; 

	struct sLook_up_table sBitRate_lookup[9] = {
		{"10K",0x31,0x1C},
		{"20K",0x18,0x1C},
		{"50K",0x09,0x1C},
        {"100K",0x04,0x1C},
        {"125K",0x03,0x1C},
        {"250K",0x01,0x1C},
        {"500K",0x00,0x1C},
        {"800K",0x00,0x16},
        {"1M",0x00,0x14}};

	struct sInterface_lookup_table sInterfaceList[4]={
		{"vcan0",0},
		{"vcan1",1},
		{"vcan2",2},
		{"vcan3",3}};
	
	for (boardNum =0 ; boardNum<4;boardNum++)   // determine canline selected 
	{
		if (strcmp(sInterfaceList[boardNum].board_num,board->busname )==0)
			break;
	}

	for (index = 0; index < 10; ++index)       // determine baudrate 
	{
		if (strcmp(sBitRate_lookup[index].baud_rate,board->baudrate)==0)
          break;
	}

	if (index == 9)
	{
		MSG_ERR_DRV("IXXAT::open: The given baudrate %S is invalid.", baud_rate);
		return NULL ;
	}


	/*
	**	The following can be used when the client has multiple CAN interfaces to slect from, and does not wish to use the 
	**  selection dialog box
	/*

    /*		 
	hResult= vciEnumDeviceOpen(&hEnumerator);
	 // This loop will increment the index of the device list and returns the decription of the CAN line chose by user
	if (hResult== VCI_OK)
	{
		for (index1=0; index1 <= sInterfaceList[boardNum].num; index1++){
			hResult=vciEnumDeviceNext(hEnumerator, &VCIDeviceInfo);
		}
	}
	printf("Device Selected: %s %s\n",VCIDeviceInfo.Manufacturer, VCIDeviceInfo.Description); 
	if (hResult== VCI_OK)
		hResult=vciEnumDeviceClose(hEnumerator);
	
	if (hResult== VCI_OK)
   		hResult= vciDeviceOpen(&VCIDeviceInfo.VciObjectId, &hDevice);
	*/



	/* 
	**  Display Interface Selection Dialog Box 
	*/
	hResult= vciDeviceOpenDlg(0, &hDevice);

	/*
	**  Establish and activate the message Channel 
	*/
	if (hResult== VCI_OK)
		hResult= canChannelOpen(hDevice, 0, TRUE, &hCanChn);
	//  Select Rx fifo size, Rx threshold, Tx fifo size, Tx threshold
    if (hResult== VCI_OK)
		hResult=canChannelInitialize( hCanChn, 1024, 1,128,1);  

    if (hResult== VCI_OK)
	    hResult=canChannelActivate(hCanChn, TRUE);


	/* 
	** Establish and Activate the Contol Channel 
	*/
    if (hResult== VCI_OK)
		 hResult=canControlOpen(hDevice, 0, &hCanCtl);
	 
	//  Select 11 or 29 bit IDs, Select operating mode 
    if (hResult== VCI_OK)
		hResult=canControlInitialize( hCanCtl, CAN_OPMODE_STANDARD | CAN_OPMODE_ERRFRAME, sBitRate_lookup[index].bt0, sBitRate_lookup[index].bt1 );
    
	
	//  With VCI it is possible to filter IDs, See VCI V3 manual. The following will accept all IDs
	if (hResult== VCI_OK)
	     hResult= canControlSetAccFilter( hCanCtl, FALSE, CAN_ACC_CODE_ALL, CAN_ACC_MASK_ALL);
    
	if (hResult== VCI_OK)
	    hResult=canControlStart(hCanCtl, TRUE);
  
	if (hResult!=VCI_OK)
	{
		Display_Error(hResult);
		return NULL; 
	}


	/*
	**   Create timer to poll bus status 
	*/  
	bResult= CreateTimerQueueTimer(
			& hTimerHandle,
			NULL,
			TimerProc1,    // Callback function
			NULL,
		    0,
			STATUS_POLLING_CYCLE,
			WT_EXECUTEINIOTHREAD
		    );
	
	return (CAN_HANDLE)board;
}


/*************************************************************************
**
**    Function      : canReceive_driver 
**
**    Description   : Transfers RX messages to Festival application
**    Parameters    : CAN_HANDLE inst		- handle for CAN controller 
					  Message *m			- pointer to Message struct
**    Returnvalue   : hResult				-   VCI_OK if success
**   
*************************************************************************/
UNS8 __stdcall canReceive_driver(CAN_HANDLE inst, Message *m)
{
    HRESULT hResult;
	CANMSG rCanMsg;
	//Read message from the receive buffer 
	hResult=canChannelReadMessage( hCanChn, INFINITE, &rCanMsg );
	if (hResult !=VCI_OK )
		return 1; 		

	m->cob_id =  rCanMsg.dwMsgId;   
	m->len = rCanMsg.uMsgInfo.Bits.dlc;
	m->rtr = rCanMsg.uMsgInfo.Bits.rtr;
	   if (m->rtr == NOT_A_REQUEST)
	    	memcpy(m->data, rCanMsg.abData , m->len);
	return (UNS8)hResult; 
}

/*************************************************************************
**
**    Function      : canSend_driver 
**
**    Description   : Transfers RX messages to Interface 
**    Parameters    : CAN_HANDLE inst		- handle for CAN controller 
**					  Message *m			- pointer to Message struct
**    Returnvalue   : hResult				-   VCI_OK if success
**   
*************************************************************************/
UNS8 __stdcall canSend_driver(CAN_HANDLE inst, Message const *m)
{
   	HRESULT hResult;
	CANMSG sCanMsg; 

	sCanMsg.uMsgInfo.Bytes.bType  = CAN_MSGTYPE_DATA;                
	sCanMsg.uMsgInfo.Bytes.bFlags = CAN_MAKE_MSGFLAGS(8,0,0,0,0);
	sCanMsg.uMsgInfo.Bits.srr     = 0;
	sCanMsg.dwTime   = 0;
	sCanMsg.dwMsgId  = m->cob_id ;                         
	memcpy(sCanMsg.abData,m->data, m->len);	                     
	sCanMsg.uMsgInfo.Bits.dlc = m->len;     
	sCanMsg.uMsgInfo.Bits.rtr=m->rtr; 

	// write the CAN message into the transmit FIFO without waiting for transmission
	 hResult = canChannelPostMessage(hCanChn, &sCanMsg);

	 return (UNS8)hResult; 
}




/*************************************************************************
**
**    Function      : canClose_driver 
**
**    Description   : Close the message and control channel
**    Parameters    : CAN_HANDLE inst		- handle for CAN controller 
**    Returnvalue   : 
**   
*************************************************************************/
 int __stdcall canClose_driver(CAN_HANDLE inst)
   {
	printf("CAN close \n");
	canControlReset(hCanCtl);
	canChannelClose(hCanChn);
	canControlClose(hCanCtl);
	vciDeviceClose(hDevice);
	DeleteTimerQueueTimer(NULL,hTimerHandle,NULL); 
   return 1;
   }

 
/*************************************************************************
**
**    Function      : canChangeBaudRate_driver 
**
**    Description   : Changes the Baudrate of the interface (not supported) 
**    Parameters    : 
**    Returnvalue   : 
**   
*************************************************************************/
   UNS8 __stdcall canChangeBaudRate_driver( CAN_HANDLE fd, char* baud)
	{
	//printf("canChangeBaudRate not yet supported by this driver\n");
	return 0;
	}


 /*************************************************************************
**
**    Function      : TimerProc1 
**
**    Description   :  Basic Polling of the  the canline status,
**					   print debug message for buffer overflow, and BUSOFF condition
**    Parameters    :  void* lpParametar, BOOL TimerOrWaitFired
**    Returnvalue   :  none 
**   
*************************************************************************/
   void CALLBACK TimerProc1(void* lpParametar, BOOL TimerOrWaitFired )
   {
	   HRESULT hResult;
	   CANLINESTATUS canStatus;
	  

	   /* Establish CAN controller status */
	   hResult = canControlGetStatus ( hCanCtl, &canStatus);
	   if (hResult!=VCI_OK)
		   printf("Error did not read CAN STATUS\n"); 
	   switch ( canStatus.dwStatus)
	   {
		   case CAN_STATUS_OVRRUN:
			   printf("Overrun of the recieve buffer\n");
			   break;
		   case CAN_STATUS_ERRLIM:
				printf("Overrun of the CAN controller error counter \n");
			   break;
		   case CAN_STATUS_BUSOFF:
				printf("CAN status: BUSOFF");
			   break; 
		   case CAN_STATUS_ININIT:				
			   break; 
		   case (CAN_STATUS_BUSOFF)+(CAN_STATUS_ININIT):
			   printf("CAN status: BUSOFF\n");
			    
	/* 
	** Bus off recovery should come after a software reset, and 128*11 recessive bits.  
	** This can only happen when an Error-Active node sends an active error flag,
	** and other nodes respond with Error Echo Flag
	*/
			   break;
		   default: 
			   break; 
	   }
   };


 /*************************************************************************
**
**    Function      : Display_Error
**
**    Description   :  Display the CANline Error
**    Parameters    :  HANDLE hResult
**    Returnvalue   :  none 
**   
*************************************************************************/
	void Display_Error(HRESULT hResult )
   {
	char szError[VCI_MAX_ERRSTRLEN];
	if (hResult != NO_ERROR)
	{
		if (hResult == -1)
			hResult = GetLastError();
		szError[0] = 0;
		vciFormatError(hResult, szError, sizeof(szError));
		printf("Error Establishing CAN channel, Error Code: %s\n",szError); 
	}
   }