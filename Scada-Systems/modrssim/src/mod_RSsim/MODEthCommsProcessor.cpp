// CMODEthCommsProcessor.cpp: 
//
// Mod_RSSim (c) Embedded Intelligence Ltd. 1993,2009
// AUTHOR: Conrad Braam.  http://www.plcsimulator.org
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Affero General Public License as
//    published by the Free Software Foundation, either version 3 of the
//    License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Affero General Public License for more details.
//
//    You should have received a copy of the GNU Affero General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// implementation of the Ethernet CMODEthCommsProcessor class.
//
// Ethernet:
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define MAX_MOD_MESSAGELENGTH  2048



// ------------------------- GetBCC ---------------------------------
// PURPOSE: Calculates a messages BCC value and returns it
// NOTE:
//    The message must be "[NNNNNNNN"
//    The bcc may be "cccc" or come from a valid message from the PLC.
//
LONG GetBCC(CHAR * bccString, DWORD msgLen)
{
LONG    accumulator = 0;

   return (accumulator);
} // GetBCC



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// constructor to create listen socket
CMODEthCommsProcessor::CMODEthCommsProcessor(int responseDelay,
                                             BOOL MOSCADchecks,
                                             BOOL modifyThenRespond,
                                             BOOL disableWrites,
                                             LONG PDUSize,
                                             WORD portNum) : CDDKSrvSocket(portNum)
{
CString description;
 m_protocolName = "MODBUS Eth.";

   InitializeCriticalSection(&stateCS);
   description.Format("Starting comms emulation : %s", "MODBUS TCP/IP [host]");
   SockDataMessage(description);

   m_responseDelay = 0;
   m_linger = FALSE;

   m_responseDelay = responseDelay;
   SetPDUSize(PDUSize);

   SetEmulationParameters(MOSCADchecks, modifyThenRespond, disableWrites);
   m_pWorkerThread->ResumeThread(); //start thread off here

}

// constructor to re-use the listen socket
CMODEthCommsProcessor::CMODEthCommsProcessor(int responseDelay,
                                             BOOL  MOSCADchecks,
                                             BOOL modifyThenRespond,
                                             BOOL disableWrites,
                                             LONG PDUSize,
                                             SOCKET * pServerSocket) : CDDKSrvSocket(0, 0, pServerSocket)
{
   InitializeCriticalSection(&stateCS);
   m_responseDelay = 0;
   m_linger = FALSE;

   m_responseDelay = responseDelay;
   SetPDUSize(PDUSize);

   SetEmulationParameters(MOSCADchecks, modifyThenRespond, disableWrites);
   m_pWorkerThread->ResumeThread(); //start thread off here

}

CMODEthCommsProcessor::~CMODEthCommsProcessor()
{

}

// ------------------------------- SockDataMessage ------------------------------
void CMODEthCommsProcessor::SockDataMessage(LPCTSTR msg)
{
   EnterCriticalSection(&stateCS);
   OutputDebugString("##");
   if (NULL!=pGlobalDialog)
      pGlobalDialog->AddCommsDebugString(msg);
   LeaveCriticalSection(&stateCS);
}


// ------------------------------ SockDataDebugger ------------------------------
void CMODEthCommsProcessor::SockDataDebugger(const CHAR * buffer, LONG length, dataDebugAttrib att)
{
   CRS232Port::GenDataDebugger((BYTE*)buffer, length, att);
} // SockDataDebugger

// ------------------------------- SockStateChanged -----------------------
void CMODEthCommsProcessor::SockStateChanged(DWORD state)
{
   EnterCriticalSection(&stateCS);
   if (NULL != pGlobalDialog)
      pGlobalDialog->m_ServerSocketState = state;

   LeaveCriticalSection(&stateCS);
} // SockStateChanged

// ------------------------------- ActivateStationLED ---------------------------
void CMODEthCommsProcessor::ActivateStationLED(LONG stationID)
{
   if (stationID>0 && stationID<STATIONTICKBOXESMAX)
   {
      //start the counter for this station at the beginning
      if (NULL != pGlobalDialog)
         pGlobalDialog->m_microTicksCountDown[stationID] = pGlobalDialog->GetAnimationOnPeriod();
      // it will count down untill it extinguishes
   }
} // ActivateStation

// ------------------------------- StationIsEnabled ---------------------------
// Return TRUE if station is enabled
BOOL CMODEthCommsProcessor::StationIsEnabled(LONG stationID)
{
   if (!pGlobalDialog)
      return(FALSE);
   if (stationID>0 && stationID<STATIONTICKBOXESMAX)
   {
      return (pGlobalDialog->StationEnabled(stationID));
   }
   return TRUE;
} // StationIsEnabled



// --------------------------------- ProcessData -----------------------------
// The buffer passed in is all of the data available on the socket, so we could have 
// multiple requests if the other end is timing out or sends too fast.
BOOL CMODEthCommsProcessor::ProcessData(SOCKET openSocket, const CHAR *pBuffer, const DWORD numBytes)
{
CHAR  telegramBuffer[MAX_MOD_MESSAGELENGTH];
CHAR  responseBuffer[MAX_MOD_MESSAGELENGTH];
CHAR  debugStr[MAX_MOD_MESSAGELENGTH];
WORD  responseLen;
BYTE  *pDataPortion;
int   i=0;
//WORD  guardword1=1;
WORD  requestMemArea;   // telegram read/write are being referenced=0..MAX_MOD_MEMTYPES
//WORD  guardword2=2;
WORD  startRegister, endRegister, MBUSerrorCode=0;
WORD  seperationOffset; // offset added to each address, due to stations having seperate reg.s
BOOL  MBUSError = TRUE;
WORD  numBytesInReq;
WORD  numRegs;
CString deb;
BOOL transmitted = TRUE;

   m_debuggerStep = 100;
   // inc counter
   pGlobalDialog->PacketsReceivedInc();

   responseBuffer[0] = '\0';

   // copy the Rx'd telegram neatly
   memcpy(telegramBuffer, pBuffer, numBytes);
   telegramBuffer[numBytes] = '\0';
   

   // simulate the I/O and network delays
   Sleep(m_responseDelay);
   if (pGlobalDialog->m_Unloading)
      return(TRUE);           // stop processing during shutdown of the socket array to allow easy closing.

   // parse the telegram

   // 1. break up the telegram
   CMODMessage::SetEthernetFrames();
   CMODMessage  modMsg(telegramBuffer, numBytes);

   //check the station #
   ActivateStationLED(modMsg.stationID);
   if (!StationIsEnabled(modMsg.stationID))
   {
   CString msg;
      // station off-line
      msg.Format("Station ID %d off-line, no response sent\n", telegramBuffer[6]);
      SockDataMessage(msg);
      return(TRUE);
   }
   // 2. parse 

   // Get memory area which we have to update or retrieve from
   requestMemArea = modMsg.GetAddressArea(modMsg.functionCode);//.type);
   if (requestMemArea >= MAX_MOD_MEMTYPES)
   {
      // TO DO!
      // handle the error
      Beep(2000,200);
      requestMemArea = 3;  // for now just default to "Holding" for now!
   }

   //
   // Validate that the request is a valid command code
   //
   startRegister = modMsg.address;
   //endRegister = startRegister + modMsg.byteCount/2;
   if (MOD_WRITE_SINGLE_COIL == modMsg.functionCode)
      endRegister = startRegister;
   else
      endRegister = startRegister + modMsg.byteCount;

   if ((modMsg.functionCode == MOD_READ_COILS)||      // 01
       (modMsg.functionCode == MOD_READ_DIGITALS)||   // 02
       (modMsg.functionCode == MOD_READ_REGISTERS)||  // 04
       (modMsg.functionCode == MOD_READ_HOLDING)||    // 03
       (modMsg.functionCode == MOD_READ_EXTENDED)||   // 14
       (modMsg.functionCode == MOD_WRITE_SINGLE_COIL)||     // 05
       (modMsg.functionCode == MOD_WRITE_MULTIPLE_COILS)||  // 0F
       (modMsg.functionCode == MOD_WRITE_HOLDING)||         // 10
       (modMsg.functionCode == MOD_WRITE_SINGLEHOLDING)||   // 06 
       (modMsg.functionCode == MOD_MASKEDWRITE_HOLDING)||   // 16 (new/testing)
       (modMsg.functionCode == MOD_WRITE_EXTENDED)          // 15
      )
   {
      // Check the request length against our PDU size.
      switch (modMsg.functionCode)
      {
      case MOD_READ_COILS:      // 01
      case MOD_READ_DIGITALS:   // 02
         numBytesInReq = modMsg.byteCount/8; // # bits
         break;
      case MOD_WRITE_MULTIPLE_COILS:  // 0F
         numBytesInReq = (WORD)ceil((double)modMsg.byteCount/8); // # bits
         break;
      case MOD_WRITE_SINGLE_COIL:
         numBytesInReq = 1;
         break;
      default:
         numBytesInReq = modMsg.byteCount*2; // # registers
         break;
      }
      if (numBytesInReq > m_PDUSize)
      {
         MBUSError = TRUE;
         MBUSerrorCode = MOD_EXCEPTION_ILLEGALVALUE;   // too long data field
      }
      else
         MBUSError = FALSE;
   }
   else
   {
      MBUSError = TRUE;
      MBUSerrorCode = MOD_EXCEPTION_ILLEGALFUNC;   // 01
   }

   if (modMsg.m_packError)
   {
      // request message has a corrupted field somewhere
      MBUSError = TRUE;
      MBUSerrorCode = MOD_EXCEPTION_ILLEGALVALUE;   // too long data field
   }
   
   // 3. build response
   CMODMessage  responseModMsg(modMsg); //Call copy constructor

   //
   // Do some more message validation tests etc.
   //
   if (!MBUSError)
   {
      if ((m_MOSCADchecks)&& // Is a (Analog/holding/extended register)
          ((requestMemArea == 2)||(requestMemArea == 3)||(requestMemArea == 4))
         )
      {
      WORD startTable,endTable;     // table #
      WORD startCol,endCol;         // col #
   
         endTable = MOSCADTABLENUMBER(endRegister); // MOSCAD specify register # on the wire for the formula
         endCol = MOSCADCOLNUMBER(endRegister);
         startTable = MOSCADTABLENUMBER(startRegister);
         startCol = MOSCADCOLNUMBER(startRegister);
         // test that this request does not bridge 2 columns.
         // , else we cannot job/request them together.
         if ((endTable != startTable) ||
             (endCol != startCol))
         {
            MBUSError = TRUE;
            MBUSerrorCode = MOD_EXCEPTION_ILLEGALADDR;   // 02
         }
      }
   }

   if (!MBUSError)
   {
      // if we want to disable all writes
      if ((m_disableWrites) &&
          ((modMsg.functionCode == MOD_WRITE_SINGLE_COIL) ||
           (modMsg.functionCode == MOD_WRITE_SINGLEHOLDING) ||
           (modMsg.functionCode == MOD_MASKEDWRITE_HOLDING) ||
           (modMsg.functionCode == MOD_WRITE_MULTIPLE_COILS) ||
           (modMsg.functionCode == MOD_WRITE_HOLDING) ||
           (modMsg.functionCode == MOD_WRITE_EXTENDED) 
          )
         )
      {
      CString deb;
         MBUSError = TRUE;
         MBUSerrorCode = MOD_EXCEPTION_ILLEGALFUNC;   // 02
         deb.Format("Writting to registers or I/O is disabled!\n");
         OutputDebugString(deb);
         SockDataMessage(deb);
      }
   }
   // do a address+length range check too
   if (!MBUSError)
   {
      if (PLCMemory[requestMemArea].GetSize() < endRegister)
      {
         MBUSError = TRUE;
         MBUSerrorCode = (PLCMemory[requestMemArea].GetSize() < startRegister ?
                             MOD_EXCEPTION_ILLEGALADDR:MOD_EXCEPTION_ILLEGALVALUE);   // 02
      }
   }

   if (MBUSError)
   {
   CString msg;
      msg.Format("Modbus message in error x%02X\n", MBUSerrorCode);
      OutputDebugString(msg);
      SockDataMessage(msg);
   }

   if (pGlobalDialog->m_seperateRegisters)
   {
      seperationOffset = (WORD)(pGlobalDialog->m_numSeperate * modMsg.stationID);
      if ((PLCMemory[requestMemArea].GetSize() < seperationOffset+endRegister) ||
          (endRegister > pGlobalDialog->m_numSeperate))
      {
         MBUSError = TRUE;
         MBUSerrorCode = (PLCMemory[requestMemArea].GetSize() < seperationOffset + startRegister ?
                             MOD_EXCEPTION_ILLEGALADDR:MOD_EXCEPTION_ILLEGALVALUE);   // 02
      }
   }
   else
      seperationOffset = 0;


   //
   // Request message seems error free, process it.
   //
                        // 1st 3 bytes + any others up to data get 
                        // added in at this time 
   responseModMsg.BuildMessagePreamble(MBUSError,
                                       MBUSerrorCode); 

   // If a write is done then the copy constructor will have done all required for ack
   // else a read must now pack into dataPtr onwards! and calc len etc!

   // A read must now pack into dataPtr onwards! and calc len etc!
   //   writes must update our mem areas accordingly.
   if (!MBUSError)
   {
      // 4. fill in the data portion of telegram
      switch (modMsg.functionCode)
      {
      case MOD_READ_COILS     : // 01 READ
      case MOD_READ_DIGITALS  : // 02 READ
         pDataPortion = responseModMsg.dataPtr; //Get offset to fill in data
         if (MAX_MOD_MEMWORDS >= modMsg.address + modMsg.byteCount)
         {
         WORD memValueTemp;
         WORD bitOffset;

//#ifdef _COMMS_DEBUGGING
            deb.Format("Read In/output from %d for %d bits.\n", modMsg.address, modMsg.byteCount);
            OutputDebugString(deb);
            SockDataMessage(deb);
//#endif

            // pack SIM memory: one WORD of sim memory for every BIT in the data.
            // Ie the sim uses one word for each I/O bit in modbus.
            numBytesInReq = modMsg.byteCount/8;
            if (modMsg.byteCount%8)  // if we overflow the byte
               numBytesInReq++;
            for (i=0; i <numBytesInReq;i++)
            {
               // grab the memory now
               memValueTemp = 0;
               for (bitOffset=0;bitOffset<8;bitOffset++)
               {
                  if ((i*8)+bitOffset < modMsg.byteCount)
                     if (PLCMemory[requestMemArea][(seperationOffset + modMsg.address)+(i*8)+bitOffset])
                        memValueTemp += (0x01<<bitOffset);
                     // else bit is off
               }
               *(BYTE*)pDataPortion =  (BYTE)memValueTemp;
               pDataPortion +=1;
            }

         }
         else
         {
            // pack the exception code into the message
            responseModMsg.buffer[1] |= 0x80;
            responseModMsg.buffer[2] = 0x02;    // exception code here (could also use 0x03)
         }
         break;

      case MOD_READ_REGISTERS : 
      case MOD_READ_HOLDING   :
      case MOD_READ_EXTENDED  :
         pDataPortion = responseModMsg.dataPtr; //Get offset to fill in data
         if (MAX_MOD_MEMWORDS >= modMsg.address + modMsg.byteCount)
         {
            WORD memValueTemp;

//#ifdef _COMMS_DEBUGGING
            deb.Format("Read Register from %d for %d .\n", modMsg.address, modMsg.byteCount);
            OutputDebugString(deb);
            SockDataMessage(deb);
//#endif
            for (i=0; i <modMsg.byteCount;i++)
            {
               // grab the memory now
               memValueTemp = PLCMemory[requestMemArea][(seperationOffset + modMsg.address)+i];
               *(WORD*)pDataPortion =  SwapBytes( memValueTemp );
               pDataPortion += 2;
            }
         }
         else
         {
            // pack the exception code into the message
            responseModMsg.buffer[1] |= 0x80;
            responseModMsg.buffer[2] = 0x02;    // exception code here (could also use 0x03)

            deb.Format("Read register past %d error x%02X!\n", MAX_MOD_MEMWORDS, (BYTE)responseModMsg.buffer[2]);
            OutputDebugString(deb);
            SockDataMessage(deb);

         }
         break;
      default :
          //Writes acks are all built in copy constructor
          //But the update is done here!
          if (MAX_MOD_MEMWORDS >= modMsg.address + modMsg.byteCount)
          {
          // lock memory for writting
          CMemWriteLock lk(PLCMemory.GetMutex());
      
              pDataPortion = responseModMsg.dataPtr; //Get offset to fill in data

              if (!lk.IsLocked())
                 //...Update
                 switch (modMsg.functionCode)
                 { 
                 case MOD_WRITE_SINGLE_COIL     :
                     {
                     CString deb;
                        deb.Format("Write single output %d.\n", modMsg.address);
                        OutputDebugString(deb);
                        SockDataMessage(deb);
                     }
                     //data gets copied in now
                     if (m_modifyThenRespond)
                        PLCMemory.SetAt(requestMemArea, seperationOffset + modMsg.address, (*(WORD*)modMsg.dataPtr?1:0));

                     pDataPortion = responseModMsg.dataPtr; //Get offset to fill in data
                     *pDataPortion++ = (PLCMemory[requestMemArea][(seperationOffset + modMsg.address)+i] ? 0xFF : 0x00);
                     *pDataPortion++ = 0x00;
                     if (!m_modifyThenRespond)
                        PLCMemory.SetAt(requestMemArea, seperationOffset + modMsg.address, (*(WORD*)modMsg.dataPtr?1:0));
                     
                     numRegs = 1;   // repaint 1 item

                     break;
                 case MOD_WRITE_MULTIPLE_COILS  :
                    // unpack into the SIM memory on WORD of sim memory for every BIT in the data 
                    //WORD numBytes;

                     numBytesInReq = modMsg.count/8;
                     if (modMsg.count%8)  // if we overflow a byte
                        numBytesInReq++;

                     {
                     CString deb;
                        deb.Format("Write multiple outputs from %d for %d bits.\n", modMsg.address, modMsg.count);
                        OutputDebugString(deb);
                        SockDataMessage(deb);
                     }
                     numRegs = numBytesInReq;   // repaint X bits

                     for (i=0;i<numBytesInReq;i++)
                     {
                     WORD bitOffset;
                        for (bitOffset=0;bitOffset<8;bitOffset++)
                        {
                           if ((i*8)+bitOffset <= modMsg.count)
                           {
                              if (*(BYTE*)modMsg.dataPtr & (0x01<<bitOffset))
                                 PLCMemory.SetAt(requestMemArea, seperationOffset + modMsg.address+(i*8)+bitOffset, 1);
                              else
                                 PLCMemory.SetAt(requestMemArea, seperationOffset + modMsg.address+(i*8)+bitOffset, 0);
                           }
                        }
                        modMsg.dataPtr++;
                     }
                     break;
                 case MOD_WRITE_HOLDING : //WRITE multiple holdings
                 case MOD_WRITE_EXTENDED:
                     //PLCMemory[requestMemArea][modMsg.address] = SwapBytes(*(WORD*)modMsg.dataPtr);
                     //break;

                     numRegs = modMsg.byteCount/2;

                     {
                     CString deb;
                        deb.Format("Write multiple registers from %d for %d registers.\n", modMsg.address, numRegs);
                        OutputDebugString(deb);
                        SockDataMessage(deb);
                     }
                     for (i=0;i<numRegs;i++)
                     {
                        PLCMemory.SetAt(requestMemArea, seperationOffset + modMsg.address + i, SwapBytes(*(WORD*)modMsg.dataPtr));
                        modMsg.dataPtr +=2;   // inc pointer by 2 bytes
                     }
                     break;
                 case MOD_WRITE_SINGLEHOLDING : //WRITE single holding reg.
                     {
                     CString deb;
                     WORD memValueTemp;
                        deb.Format("Write single register %d.\n", modMsg.address);
                        OutputDebugString(deb);
                        SockDataMessage(deb);
                     
                        numRegs = 1;   //repaint 1 register

                        if (m_modifyThenRespond)
                           PLCMemory.SetAt(requestMemArea, seperationOffset + modMsg.address , SwapBytes(*(WORD*)modMsg.dataPtr));
                        //   PLCMemory.SetAt(requestMemArea,   modMsg.address + i, SwapBytes(*(WORD*)modMsg.dataPtr));
                        memValueTemp = PLCMemory[requestMemArea][(seperationOffset +modMsg.address)];
                        *(WORD*)pDataPortion =  SwapBytes( memValueTemp );

                        if (!m_modifyThenRespond)
                           PLCMemory.SetAt(requestMemArea, seperationOffset + modMsg.address , SwapBytes(*(WORD*)modMsg.dataPtr));
                        modMsg.dataPtr +=2;   // inc pointer by 2 bytes
                        pDataPortion += 2;
                     }
                     break;
                 case MOD_MASKEDWRITE_HOLDING:
                    ASSERT(0); // TODO
                    break;
                 }
          }
          // we can only call on the GUI thread once we have un-locked
          if (pGlobalDialog)
          {
             int cols = pGlobalDialog->GetListDisplayedWidth();
             pGlobalDialog->RedrawListItems(modMsg.GetAddressArea(modMsg.functionCode), 
                                            seperationOffset + modMsg.address/(cols), 
                                            (seperationOffset + modMsg.address+(numRegs-1))/(cols)
                                           ); // repaint only the needed rows
          }
          break;
  
      } //end switch
   } 
   else
   { // error occurred
      pDataPortion = responseModMsg.dataPtr; // Get offset to fill in data
   }
   
   // THERE IS no CRC, so do not call BuildMessageEnd
   responseModMsg.totalLen = (WORD)((LONG)pDataPortion-(LONG)responseModMsg.buffer);

   // insert the frame info
   //responseModMsg
   memmove(&responseModMsg.buffer[ETH_PREAMBLE_LENGTH], responseModMsg.buffer, responseModMsg.totalLen);
   responseLen = ETH_PREAMBLE_LENGTH + responseModMsg.totalLen; //SwapBytes(*(WORD*)(responseModMsg.preambleLenPtr)) + 6; //hdr;
   memset(responseModMsg.buffer, 0 , ETH_PREAMBLE_LENGTH);
   WORD tn = responseModMsg.GetEthernetTransNum();
   *((WORD*)&responseModMsg.buffer[0]) = tn;
   *((WORD*)&responseModMsg.buffer[4]) = SwapBytes(responseModMsg.totalLen);
   
   // 6. send it back
   m_debuggerStep = 102;
   sprintf(debugStr, "Send %d bytes\n", responseLen);
   OutputDebugString(debugStr);

   SockStateChanged(SOCKETCURRENTLY_WRITTING);

   // disabled in v7.7
   m_NoiseSimulator.m_pSocketObj = (CDDKSrvSocket*)this;
   transmitted = m_NoiseSimulator.InjectErrors((CDDKSrvSocket*)this, 
                                               openSocket, 
                                               (BYTE*)responseModMsg.buffer, 
                                               responseLen, 
                                               debugStr
                                              );

   m_debuggerStep = 103;
   if (transmitted)
   {
      // inc counter
      pGlobalDialog->PacketsSentInc();
   }

   // If there are still more bytes in the data we read, process them recursively
   if ((WORD)(modMsg.m_frameLength + 6) < numBytes)
   {
      SockDataMessage("## Processing queued data bytes...");
      SockDataDebugger(&telegramBuffer[modMsg.m_frameLength + 6], numBytes - (modMsg.m_frameLength + 6), dataDebugOther);
      // recursive call using the data from our own stack-space
      ProcessData(openSocket, &telegramBuffer[modMsg.m_frameLength + 6], numBytes - (modMsg.m_frameLength + 6));
   }
   return (TRUE);
}

// ----------------------------- LoadRegisters -----------------------------
// load binary dump of the register values from file.
BOOL CMODEthCommsProcessor::LoadRegisters()
{
   BOOL ret = CMOD232CommsProcessor::LoadRegistersIMP();

   if (ret)
      SockDataMessage("Register values loaded OK\n");
   return (ret);
} // LoadRegisters

// ----------------------------- SaveRegisters -----------------------------
// Save binary dump of the register values from file.
BOOL CMODEthCommsProcessor::SaveRegisters()
{
   BOOL ret = CMOD232CommsProcessor::SaveRegistersIMP();

   if (ret)
      SockDataMessage("Register values Saved OK\n");
   return (ret);
} // SaveRegisters

// -------------------------- SetEmulationParameters ------------------------
void CMODEthCommsProcessor::SetEmulationParameters(BOOL moscadChecks, 
                                                BOOL modifyThenRespond, 
                                                BOOL disableWrites)
{
   m_MOSCADchecks = moscadChecks;
   m_modifyThenRespond = modifyThenRespond;
   m_disableWrites = disableWrites;
}
