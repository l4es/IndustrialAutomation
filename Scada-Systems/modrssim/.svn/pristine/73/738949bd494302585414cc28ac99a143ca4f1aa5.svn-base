/////////////////////////////////////////////////////////////////////////////
//
// FILE: MODCommsProcessor.cpp : implementation file
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
// implementation of the CMOD232CommsProcessor class.
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ABCommsProcessor.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



// English meanings for MODBUS (Exception) error codes
PCHAR MODBUSplcError[9] =     
{
   "No error",                // 0x0
   "Illegal Function",        // 0x1
   "Illegal Data Address",    // 0x2
   "Illegal Data value",      // 0x3
   "Slave Device failure",    // 0x4
   "Acknowledge",             // 0x5
   "Slave device busy",       // 0x6
   "Negative acknowledge",    // 0x7
   "Memory Parity error"      // 0x8
};  

BOOL CMODMessage::m_protocolEthernet = FALSE;   // default to serial

// ----------------------- constructor -----------------------------------
CMODMessage::CMODMessage(const CHAR * pMessageRX, DWORD len)
{
BYTE *pTelePtr;
BYTE *crcPtr;  // CRC bytes here
BYTE *crcStartPtr = (BYTE*)pMessageRX;
WORD  crc = 0;
static BYTE EthernetHeadder[4]= {
         0,0,0,0
      };

   m_packError = FALSE;
   frameEthernet = m_protocolEthernet;
   frameASCII = FALSE;

   // break it down
   pTelePtr = (BYTE*)pMessageRX;
   totalLen = (WORD)len;
   count = 0;

   if (m_protocolEthernet)
   {
      m_EthernetTransNum = *(WORD*)pTelePtr;
      pTelePtr += sizeof(EthernetHeadder);
      // grab the TCP frame length from the actual headder
      m_frameLength = *(WORD*)pTelePtr;
      m_frameLength = SwapBytes(m_frameLength);
      pTelePtr += sizeof(WORD);
   }
   //Pre-Amble
   if (frameASCII)
   {
      stationID =    (BYTE)UnPackASCIIField(&pTelePtr, 1, m_packError); // 2 char 
      functionCode = (BYTE)UnPackASCIIField(&pTelePtr, 1, m_packError); // 2 char
      address   = UnPackASCIIField(&pTelePtr, 2, m_packError); // 2 chars
   }
   else
   {
      stationID =    (BYTE)UnPackField(&pTelePtr, 1); // 2 char 
      functionCode = (BYTE)UnPackField(&pTelePtr, 1); // 2 char
      address   = UnPackField(&pTelePtr, 2); // 2 chars
   }
   switch (functionCode)
   {
      case MOD_WRITE_SINGLE_COIL /*0x05*/ : //Write single coils dont have count bytes
         // Therefore just adjust buffer to go through common unpacker/write-updater.
         /*if (0xff == *pTelePtr)
         {
           *pTelePtr = 0x00;
           *(pTelePtr+1) = 0x01;
         }
         else
         {
           *pTelePtr = 0x00;
           *(pTelePtr+1) = 0x00;
         } */
         byteCount = 2;
         overalLen = byteCount;
         break;
      case MOD_WRITE_MULTIPLE_COILS:
         byteCount = UnPackField(&pTelePtr, 2); // 2 chars, the count is in bits
         count = byteCount;
         overalLen = (WORD)ceil(byteCount/8.0);
         coilByteCount = *pTelePtr++; // increment past the #bytes byte which is the # bytes of data to expect (max 255)
         overalLen += 3;
         break;
      case MOD_READ_COILS  : 
      case MOD_READ_DIGITALS  : 
      case MOD_READ_REGISTERS : 
      case MOD_READ_HOLDING   :
      case MOD_READ_EXTENDED  :
         // byteCount= # bytes to read
         byteCount = UnPackField(&pTelePtr, 2); // 2 chars, the count is in REGISTERS
         overalLen = 2;
         break;
      case MOD_WRITE_HOLDING:
      case MOD_WRITE_EXTENDED:
         // byteCount=# bytes if a write, else # bytes to read
         byteCount = UnPackField(&pTelePtr, 2)*2; // 2 chars, the count is in bytes
         overalLen = byteCount;
         pTelePtr++; // increment past the #bytes byte which is the # bytes of data to expect (max 255)
         overalLen+=3;  // skip the 3 bytes for the req. size (byte) and length/quantity word
         break;
      case MOD_WRITE_SINGLEHOLDING:
         byteCount = 2; // 2 chars, only 1 register
         overalLen = byteCount;
         break;
      case MOD_MASKEDWRITE_HOLDING:
         byteCount = 4; // 2 masks
         overalLen = byteCount;
         m_andMask = UnPackField(&pTelePtr, 2); // 2 bytes, word
         m_orMask  = UnPackField(&pTelePtr, 2); // 2 bytes, word
         break;
      default   : //All other commands not supported
         //ASSERT (0);
         overalLen = 0;
         byteCount = 0;
         break;
   }
   overalLen += 4; //now it points to the CRC

   //Now  (at last) pTelePtr points to the data to read/write
   dataPtr = pTelePtr;  // data starts here
   
//   ASSERT(totalLen >= overalLen + MODBUS_CRC_LEN); // range-check here
   if (totalLen < overalLen + MODBUS_CRC_LEN)
   {
      // turf this message it is duff!
      overalLen = totalLen - MODBUS_CRC_LEN;
      m_packError = TRUE;

   }
   if (frameEthernet)
   {
      ASSERT(totalLen >= overalLen - ETH_PREAMBLE_LENGTH); // range-check here
      // Ethernet frame does not have an embedded CRC
      if (totalLen < overalLen - ETH_PREAMBLE_LENGTH)
      {
         overalLen = totalLen - ETH_PREAMBLE_LENGTH;
         // turf this message it is duff!
         m_packError = TRUE;
      }
      else
         crcCheckedOK = TRUE;
   }
   else
   {
      // check the CRC
      crcPtr = (BYTE*)&pMessageRX[overalLen];
      crcStartPtr = (BYTE*)pMessageRX;

      crc = 0xffff;
      CalcCRC(crcStartPtr, overalLen, &crc);      // Only one buffer to calc crc of

      if (*(WORD *)crcPtr != crc)
      {
         // CRC did not match
         crcCheckedOK = FALSE;
      }
      else 
         crcCheckedOK = TRUE;
   }
} // CMODMessage

// --------------------------- CMODMessage --------------------------
// PURPOSE: copy constructor used to build responses, does not actually 
// copy the message.
CMODMessage::CMODMessage(const CMODMessage & oldMODMessage) 
{
   m_packError = FALSE;

   //Copy in common stuff from both messages here!
   this->stationID    = oldMODMessage.stationID;
   this->functionCode = oldMODMessage.functionCode;
   this->address = oldMODMessage.address;       // where to copy data from
   this->byteCount = oldMODMessage.byteCount;   // length of data to copy
   this->m_andMask = oldMODMessage.m_andMask;
   this->m_orMask  = oldMODMessage.m_orMask;
   
   this->overalLen = 0;   //New message so 0 for now!
   
   this->dataPtr = (BYTE*)buffer; //Nice an fresh pointer to the beginning!
   this->m_EthernetTransNum = oldMODMessage.m_EthernetTransNum;
}

// ------------------------------ BuildMessagePreamble -------------------------
// PURPOSE: Builds the STN,FN and LEN bytes of the telegram.
// on completion dataPtr pointsto where the data must be packed in (if any)
CHAR * CMODMessage::BuildMessagePreamble(BOOL error, WORD errorCode)
{
BYTE *pWorkArea;
BYTE numBytesData;

   //
   pWorkArea = (BYTE*)buffer;
   *pWorkArea++ = (BYTE)stationID;
   if (error)
   { // error flag 80 + error meaning byte
      *pWorkArea++ = (BYTE)(functionCode|0x80);
      *pWorkArea++ = (BYTE)errorCode;
   }
   else
   {
      // normal processing
      *pWorkArea++ = (BYTE)functionCode;
      switch (functionCode)
      {
          case MOD_WRITE_HOLDING        : 
          case MOD_WRITE_EXTENDED       : 
             // HF fixed the return address.
             *pWorkArea++ = HIBYTE(address);
             *pWorkArea++ = LOBYTE(address);
             *pWorkArea++ = HIBYTE(byteCount/2);
             *pWorkArea++ = LOBYTE(byteCount/2);
             break;
          case MOD_WRITE_SINGLEHOLDING :
             *pWorkArea++ = HIBYTE(address); // CDB fixed return address rev 7.0
             *pWorkArea++ = LOBYTE(address);
             //*pWorkArea++ = HIBYTE(PLCMemory[GetAddressArea(functionCode)][address]);
             //*pWorkArea++ = LOBYTE(PLCMemory[GetAddressArea(functionCode)][address]);
             break;
          case MOD_MASKEDWRITE_HOLDING:
             *pWorkArea++ = HIBYTE(address);
             *pWorkArea++ = LOBYTE(address);
             //add the masks
             *pWorkArea++ = HIBYTE(m_andMask);
             *pWorkArea++ = LOBYTE(m_andMask);
             *pWorkArea++ = HIBYTE(m_orMask);
             *pWorkArea++ = LOBYTE(m_orMask);

             break;
          case MOD_WRITE_MULTIPLE_COILS : 
             *pWorkArea++ = HIBYTE(address);
             *pWorkArea++ = LOBYTE(address);
             *pWorkArea++ = HIBYTE(byteCount);  // # bits actually
             *pWorkArea++ = LOBYTE(byteCount);

             break;
          case MOD_WRITE_SINGLE_COIL    :
             *pWorkArea++ = HIBYTE(address);  // fixed thanks to Joan Lluch-Zorrilla
             *pWorkArea++ = LOBYTE(address);
             break;
          case MOD_READ_DIGITALS  : // in
          case MOD_READ_COILS     : // out
             numBytesData = (BYTE)ceil((float)byteCount/8.0);  // # registers*2
             *pWorkArea++ = numBytesData; 
             break;
          case MOD_READ_REGISTERS : 
          case MOD_READ_HOLDING   :
          case MOD_READ_EXTENDED  : 
             numBytesData = byteCount*2;  // # registers*2
             *pWorkArea++ = numBytesData; 
             break;
      }
   }   
   dataPtr = pWorkArea; // must now point to 1st byte of data

   return (buffer);
} // BuildMessagePreamble

// ----------------------------- SetEthernetFrames --------------------------
// supply FALSE for normal serial 232 frames
BOOL CMODMessage::SetEthernetFrames(BOOL ethernetFrames/* = TRUE*/)
{
BOOL oldV = m_protocolEthernet;
   m_protocolEthernet = ethernetFrames;
   return (m_protocolEthernet);
}

// ------------------------------ BuildMessageEnd -------------------------------
// PURPOSE: glue a CRC onto the end of the message
// totalLen must be = the full telegram length (+CRC) when this is called.
CHAR * CMODMessage::BuildMessageEnd()
{
WORD length;
BYTE *pCrcStart = (BYTE*)buffer;
WORD crc = 0xFFFF;
BYTE *crcPtr;

   // Add the CRC bytes
   length = totalLen - MODBUS_CRC_LEN; //calc the CRC of all bytes but the 2 CRC bytes

   CalcCRC(pCrcStart, length, &crc);
   crcPtr = (BYTE*)&buffer[length];
   *(WORD *)crcPtr = crc;

   return (buffer);
} // BuildMessageEnd


// ------------------------------ GetAddressArea --------------------
// Returns:    A supported MEM area index for any MOD address class
// Parameter:  A modbus command (e.g. 3 =read holding register)
//
WORD CMODMessage::GetAddressArea(WORD classCode //  modbus command byte
                                )
{
   switch(classCode)
   {
      // read commands 
      case MOD_READ_COILS     : return(0); break;
      case MOD_READ_DIGITALS  : return(1); break;
      case MOD_READ_REGISTERS : return(2); break;   
      case MOD_READ_HOLDING   : return(3); break;
      case MOD_READ_EXTENDED  : return(4); break;
      // write commands      
      case MOD_WRITE_HOLDING        : return(3); break;
      case MOD_WRITE_SINGLEHOLDING  : return(3); break;
      case MOD_MASKEDWRITE_HOLDING  : return(3); break;
      case MOD_WRITE_SINGLE_COIL    : return(0); break;
      case MOD_WRITE_MULTIPLE_COILS : return(0); break;
      case MOD_WRITE_EXTENDED       : return(4); break;
   }
   return(3); //Default here for now, Should never get here anyways!

} // GetAddressArea

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
IMPLEMENT_DYNAMIC( CMOD232CommsProcessor, SimulationSerialPort);

//////////////////////////////////////////////////////////////////////
// constructor to open port
CMOD232CommsProcessor::CMOD232CommsProcessor(LPCTSTR portNameShort, 
                                     DWORD  baud, 
                                     DWORD byteSize, 
                                     DWORD parity, 
                                     DWORD stopBits,
                                     DWORD rts,
                                     int   responseDelay,
                                     BOOL  MOSCADchecks,
                                     BOOL modifyThenRespond,
                                     BOOL disableWrites) : SimulationSerialPort()
{
CString description;
 m_protocolName = "MODBUS RTU";

   InitializeCriticalSection(&stateCS);
   
   m_noiseLength = 0;
   
   description.Format("Starting comms emulation : %s", "MODBUS RS-232");
   RSDataMessage(description);
   
   // open the port etc...
   if (OpenPort(portNameShort))
   {
      ConfigurePort(baud, byteSize, parity, stopBits, rts, (NOPARITY==parity?FALSE:TRUE));
   }
   m_responseDelay = responseDelay;

   SetEmulationParameters(MOSCADchecks, modifyThenRespond, disableWrites);
   m_pWorkerThread->ResumeThread(); //start thread off here

}


CMOD232CommsProcessor::~CMOD232CommsProcessor()
{

}

void CMOD232CommsProcessor::SetEmulationParameters(BOOL moscadChecks, 
                                                BOOL modifyThenRespond, 
                                                BOOL disableWrites)
{
   m_MOSCADchecks = moscadChecks;
   m_modifyThenRespond = modifyThenRespond;
   m_disableWrites = disableWrites;
}

// ------------------------------ RSDataDebugger ------------------------------
void CMOD232CommsProcessor::RSDataDebugger(const BYTE * buffer, LONG length, int transmit)
{
   CRS232Port::GenDataDebugger(buffer,length,transmit);
} // RSDataDebugger

// ------------------------------- RSStateChanged -----------------------
void CMOD232CommsProcessor::RSStateChanged(DWORD state)
{
   EnterCriticalSection(&stateCS);
   if (NULL==pGlobalDialog)
      return;
   pGlobalDialog->m_ServerRSState = state;
   LeaveCriticalSection(&stateCS);
} // RSStateChanged

// ------------------------------ RSDataMessage ------------------------------
void CMOD232CommsProcessor::RSDataMessage(LPCTSTR msg)
{
   EnterCriticalSection(&stateCS);
   OutputDebugString("##");
   if (NULL!=pGlobalDialog)
      pGlobalDialog->AddCommsDebugString(msg);
   LeaveCriticalSection(&stateCS);
}

// ------------------------------- RSModemStatus ---------------------------
void CMOD232CommsProcessor::RSModemStatus(DWORD status)
{
   EnterCriticalSection(&stateCS);
   if (NULL!=pGlobalDialog)
      pGlobalDialog->SendModemStatusUpdate(status);
   LeaveCriticalSection(&stateCS);
}


// ------------------------------ OnProcessData --------------------------------
BOOL CMOD232CommsProcessor::OnProcessData(const CHAR *pBuffer, DWORD numBytes, BOOL *discardData)
{
   // build noise telegram
   if (numBytes)
   { //append recieved bytes to the noise telegram
      if (m_noiseLength + numBytes >= sizeof(m_noiseBuffer))
      {
         RSDataMessage("OVERFLOW:Restarting interpretation.");

         m_noiseLength = 0;
         //SetEngineState(ENG_STATE_IDLE);
         return(TRUE);
      }
      memcpy(&m_noiseBuffer[m_noiseLength], pBuffer, numBytes);
      m_noiseLength += numBytes;
      *discardData = TRUE;
   }

   if (m_noiseLength < MODBUS_NORMAL_LEN)
      return(FALSE);
   CMODMessage::SetEthernetFrames(FALSE);
   CMODMessage msg((char*)m_noiseBuffer, m_noiseLength);
   if (msg.CRCOK())
   {
   BOOL ret;
      // build a response etc
      ret = ProcessData((char*)m_noiseBuffer, msg.overalLen + 2);   //+2 for the CRC
      m_noiseLength = 0;
      return(ret);
   }
   else
   {
      // try strip away leading byte "noise"?
// comment out the following 2 lines, from spaccabbomm [beta@mmedia.it]
/*
      m_noiseLength--;
      memmove(m_noiseBuffer, &m_noiseBuffer[1], m_noiseLength);
*/
      return(TRUE);
   }
   *discardData = FALSE;
   return(FALSE);
}

// --------------------------------- TestMessage ------------------------
//
BOOL CMOD232CommsProcessor::TestMessage(CMODMessage &modMsg, 
                                        WORD &startRegister, 
                                        WORD &endRegister, 
                                        WORD &MBUSerrorCode,
                                        WORD &requestMemArea,
                                        WORD &numBytesInReq
                                        )
{
BOOL MBUSError = FALSE;

   if (!modMsg.CRCOK())
   {
      // bail
   }

   //Get memory area which to update or retrieve from
   requestMemArea = modMsg.GetAddressArea(modMsg.functionCode);
   if (requestMemArea >= MAX_MOD_MEMTYPES)
   {
      // TO DO!
      // handle the error
      Beep(2000,200);
      requestMemArea = 3;  // for now just default to "Holding" for now!
   }

   // validate the request is a valid command code
   startRegister = modMsg.address;
   //endRegister = startRegister + modMsg.byteCount;
   endRegister = startRegister + modMsg.byteCount/2;  // CDB rev 7.0

   if ((modMsg.functionCode == MOD_READ_COILS)||      // 01
       (modMsg.functionCode == MOD_READ_DIGITALS)||   // 02
       (modMsg.functionCode == MOD_READ_REGISTERS)||  // 04
       (modMsg.functionCode == MOD_READ_HOLDING)||    // 03
       (modMsg.functionCode == MOD_READ_EXTENDED)||   // 14
       (modMsg.functionCode == MOD_WRITE_SINGLE_COIL)||     // 05
       (modMsg.functionCode == MOD_WRITE_MULTIPLE_COILS)||  // 0F
       (modMsg.functionCode == MOD_WRITE_HOLDING)||         // 10
       (modMsg.functionCode == MOD_WRITE_SINGLEHOLDING)||   // 06
       (modMsg.functionCode == MOD_MASKEDWRITE_HOLDING)||   // 16 (testing)
       (modMsg.functionCode == MOD_WRITE_EXTENDED)          // 15
      )
   {
      // Check the request length against our PDU size.
      if ((modMsg.functionCode == MOD_READ_COILS)||      // 01
          (modMsg.functionCode == MOD_READ_DIGITALS)||   // 02
          (modMsg.functionCode == MOD_WRITE_MULTIPLE_COILS))  // 0F
         numBytesInReq = modMsg.byteCount/8; // # bits
      else
         numBytesInReq = modMsg.byteCount*2; // # registers
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
   
/*   if (modMsg.m_packError)
   {
      // request message has a corrupted field somewhere
      MBUSError = TRUE;
      MBUSerrorCode = MOD_EXCEPTION_ILLEGALVALUE;   // too long data field
   }*/

   // 3. build response

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
      deb.Format("Writting data or I/O currently disabled, see Advanced Settings!\n");
      OutputDebugString(deb);
      RSDataMessage(deb);
   }

   if (modMsg.functionCode == MOD_WRITE_MULTIPLE_COILS) 
   {
      if (modMsg.coilByteCount != (modMsg.overalLen-7))
      {    
      CString deb;
         MBUSError = TRUE;
         MBUSerrorCode = MOD_EXCEPTION_ILLEGALFUNC;   // 02
         deb.Format("Invalid I/O length, check PDU size!\n");
         OutputDebugString(deb);
         RSDataMessage(deb);
      }
   }
   // do a address+length range check too
   if (!MBUSError)
      if (PLCMemory[requestMemArea].GetSize() < endRegister)
      {
         MBUSError = TRUE;
         MBUSerrorCode = (PLCMemory[requestMemArea].GetSize() < startRegister ?
                             MOD_EXCEPTION_ILLEGALADDR:MOD_EXCEPTION_ILLEGALVALUE);   // 02
      }

   if (MBUSError)
   {
   CString msg;
      msg.Format("Modbus message in error. Code= x%02X\n", MBUSerrorCode);
      OutputDebugString(msg);
      RSDataMessage(msg);
   }
   return(MBUSError);
}


// --------------------------------- ProcessData -----------------------------
// Interpret MODBUS request pBuffer, and respond to it.
//
BOOL CMOD232CommsProcessor::ProcessData(const CHAR *pBuffer, DWORD numBytes)
{
BYTE  telegramBuffer[MAX_RX_MESSAGELENGTH+MODBUS_FRAME_LENGTH_MAX];
CHAR  debugStr[160];
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

   m_debuggerStep = 100;
   // inc counter
   pGlobalDialog->PacketsReceivedInc();

   // simulate the I/O and network delays of a real PLC
   Sleep(m_responseDelay);

   //
   // Parse the telegram
   //

   // 1. break up the telegram
   memcpy(telegramBuffer, pBuffer, numBytes);
   CMODMessage::SetEthernetFrames(FALSE);
   CMODMessage  modMsg((CHAR*)telegramBuffer, numBytes);
   
   ActivateStationLED(modMsg.stationID);
   if (!StationIsEnabled(modMsg.stationID))
      return(TRUE);

   // 2. parse it, by testing it first
   CMODMessage  responseModMsg(modMsg); //Call copy constructor


   MBUSError = TestMessage(modMsg, 
                           startRegister,  
                           endRegister,    
                           MBUSerrorCode,  
                           requestMemArea, 
                           numBytesInReq   
                          );
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

                        // 1st 3 bytes + any others up to data get 
                        // added in at this time 
   responseModMsg.BuildMessagePreamble(MBUSError,
                                       MBUSerrorCode); 

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

            deb.Format("Read In/output from %d for %d bits.\n", modMsg.address, modMsg.byteCount);
            OutputDebugString(deb);
            RSDataMessage(deb);

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
            ASSERT(0); // this is supposed to be caught in TestMessage()
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

            deb.Format("Read Register from %d for %d .\n", modMsg.address, modMsg.byteCount);
            OutputDebugString(deb);
            RSDataMessage(deb);

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
            ASSERT(0); // this is supposed to be caught in TestMessage()
            responseModMsg.buffer[1] |= 0x80;
            responseModMsg.buffer[2] = 0x02;    // exception code here (could also use 0x03)

            deb.Format("Read register past %d error x%02X!\n", MAX_MOD_MEMWORDS, (BYTE)responseModMsg.buffer[2]);
            OutputDebugString(deb);
            RSDataMessage(deb);

         }
         break;
      default :
          // Writes acks are all built in copy constructor
          // But the update is done here!
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
                        RSDataMessage(deb);
                     }
                     //data gets copied in now
                     if (m_modifyThenRespond)
                        PLCMemory.SetAt(requestMemArea, seperationOffset +modMsg.address, (*(WORD*)modMsg.dataPtr?1:0));

//                     pDataPortion = responseModMsg.dataPtr; //Get offset to fill in data
                     *pDataPortion++ = (PLCMemory[requestMemArea][(seperationOffset +modMsg.address)+i] ? 0xFF : 0x00);
                     *pDataPortion++ = 0x00;
                     if (!m_modifyThenRespond)
                        PLCMemory.SetAt(requestMemArea, seperationOffset +modMsg.address, (*(WORD*)modMsg.dataPtr?1:0));
                     
                     numRegs = 1;   // repaint 1 item

                     break;
                 case MOD_WRITE_MULTIPLE_COILS  :
                    // unpack into the SIMul memory on WORD of sim memory for every BIT in the data 
                     {
                        int coilCount = modMsg.byteCount;
                        numBytesInReq = modMsg.count/8;
                        if (modMsg.count%8)  // if we overflow a byte
                           numBytesInReq++;

                        {
                        CString deb;
                           deb.Format("Write multiple outputs from %d for %d bits.\n", modMsg.address, modMsg.count);
                           OutputDebugString(deb);
                           RSDataMessage(deb);
                        }
                        numRegs = numBytesInReq;   // repaint X bits (modMsg.coilByteCount)
                     
                        while (coilCount>0)
                        {
                           for (i=0;i<numBytesInReq;i++)
                           {
                           WORD bitOffset;

                              for (bitOffset=0;bitOffset<8;bitOffset++)
                              {
                                 if (coilCount >0)
                                 {
                                    coilCount--;
                                    if (*(BYTE*)modMsg.dataPtr & (0x01<<bitOffset))
                                       PLCMemory.SetAt(requestMemArea, seperationOffset +modMsg.address+(i*8)+bitOffset, 1);
                                    else
                                       PLCMemory.SetAt(requestMemArea, seperationOffset +modMsg.address+(i*8)+bitOffset, 0);
                                 }
                              }
                              modMsg.dataPtr++;
                           }
                        }
                     }
                     break;

                 case MOD_WRITE_HOLDING : //WRITE multiple holdings
                 case MOD_WRITE_EXTENDED:
                     numRegs = modMsg.byteCount/2;

                     {
                     CString deb;
                        deb.Format("Write multiple registers from %d for %d registers.\n", modMsg.address, numRegs);
                        OutputDebugString(deb);
                        RSDataMessage(deb);
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
                        RSDataMessage(deb);

                        numRegs = 1;   //repaint 1 register

                        if (m_modifyThenRespond)
                           PLCMemory.SetAt(requestMemArea, seperationOffset + modMsg.address , SwapBytes(*(WORD*)modMsg.dataPtr));

                        memValueTemp = PLCMemory[requestMemArea][(seperationOffset +modMsg.address)];
                        *(WORD*)pDataPortion =  SwapBytes( memValueTemp );

                        if (!m_modifyThenRespond)
                           PLCMemory.SetAt(requestMemArea, seperationOffset + modMsg.address , SwapBytes(*(WORD*)modMsg.dataPtr));
                        modMsg.dataPtr +=2;   // inc pointer by 2 bytes
                        pDataPortion += 2;

                     }
                     break;

                 case MOD_MASKEDWRITE_HOLDING : // WRITE with MASK single holding reg.
                     {
                     CString deb;
                     WORD memValueTemp, memValueResult;
                     WORD orMask, andMask;

                        deb.Format("Write Mask register %d.\n", modMsg.address);
                        OutputDebugString(deb);
                        RSDataMessage(deb);
                        // fetch it
                        memValueTemp = PLCMemory[requestMemArea][(seperationOffset +modMsg.address)];
                        // 
                        andMask = SwapBytes(*(WORD*)modMsg.dataPtr);
                        modMsg.dataPtr+= sizeof(WORD);
                        orMask = SwapBytes(*(WORD*)modMsg.dataPtr);
                        modMsg.dataPtr+= sizeof(WORD);
                        memValueResult = ( memValueTemp & andMask) | ( orMask & (~andMask));

                        deb.Format("In=%04X And=%04X Or=%04X Out=%04X.\n", memValueTemp, andMask, orMask, memValueResult);
                        OutputDebugString(deb);
                        RSDataMessage(deb);

                        //*(WORD*)pDataPortion =  SwapBytes( memValueTemp );

                        numRegs = 1;   //repaint 1 register

                        PLCMemory.SetAt(requestMemArea, seperationOffset + modMsg.address , SwapBytes(memValueResult));


//                        modMsg.dataPtr +=2;   // inc pointer by 2 bytes
//                        pDataPortion += 2;

                     }
                     break;
                 default:
                    ASSERT(0);
                    break;
                 }
          }
          // we can only call on the GUI thread once we have un-locked
          if (pGlobalDialog)
          {
             int cols = pGlobalDialog->GetListDisplayedWidth();
             pGlobalDialog->RedrawListItems(modMsg.GetAddressArea(modMsg.functionCode), 
                                            (seperationOffset +modMsg.address)/(cols), 
                                            (seperationOffset +modMsg.address+(numRegs-1))/(cols)
                                           ); // repaint only the needed rows
          }
          break;
  
      }
   }
   else
   { // error occurred
      pDataPortion = responseModMsg.dataPtr; //Get offset to fill in data
   }
   if (0 == modMsg.stationID)   // broadcast, don't respond at all.
      return (TRUE);
   
   // finnish building the response
   responseModMsg.totalLen = (WORD)((LONG)pDataPortion-(LONG)responseModMsg.buffer);
   responseModMsg.totalLen += MODBUS_CRC_LEN;

   // 5. append the CRC
   //OutputDebugString("Calculate CRC\n");
   {
      responseModMsg.BuildMessageEnd();
   }
   
   // 6. send it back
   m_debuggerStep = 102;

#ifdef _COMMS_DEBUGGING
   sprintf(debugStr, "Send %d bytes\n", responseModMsg.totalLen);
   OutputDebugString(debugStr);
#endif
   
   RSStateChanged(RSPORTCURRENTLY_WRITTING);
   // Send it on the wire , but first kill any incomming messages as well so we don't overflow or anything
   Purge();
   if (!m_NoiseSimulator.ErrorsEnabled())
      Send(responseModMsg.totalLen, (BYTE*)responseModMsg.buffer, debugStr);
   else
   {
      // disabled v7.7
      m_NoiseSimulator.InjectErrors((CRS232Port*)this, (BYTE*)responseModMsg.buffer, responseModMsg.totalLen, debugStr);
      RSDataMessage("Comm-Error injection/simulation active.\n");

   }
   m_debuggerStep = 103;
   
   // inc our counter
   pGlobalDialog->PacketsSentInc();
   RSDataMessage(".\n");

   /*if (TRUE)
   {
   BOOL linesSet;
      linesSet = EscapeCommFunction(h232Port, 
      Sleep(300);
      linesSet = EscapeCommFunction(h232Port, 
   } */
   return (TRUE);
} // ProcessData

// ----------------------------- LoadRegistersIMP -----------------------------
// STATIC : implements the function to load register values from file
BOOL CMOD232CommsProcessor::LoadRegistersIMP()
{
CFileException ex;
CFile dat;
LONG area;
DWORD wordIndex;

   if (!dat.Open("MODDATA.DAT", CFile::modeRead|CFile::shareDenyRead, &ex) )
   {
      // complain if an error happened
      // no need to delete the exception object

      TCHAR szError[1024];
      ex.GetErrorMessage(szError, 1024);
      OutputDebugString( "Couldn't open source file: ");
      OutputDebugString( szError);
      
      return FALSE;
   }
   // read it in
   for (area=0;area < MAX_MOD_MEMTYPES;area++)
   {
   DWORD numRead,totalRead=0;
   WORD dataWord;
   DWORD maxIndex = PLCMemory[area].GetSize();

      // lock the memory for reading
//      CMemWriteLock lk(pGlobalDialog->m_pMemWriteSync);
      CMemWriteLock  lk(PLCMemory.GetMutex());
      // loop thru all registers (WORD)
      if (!lk.IsLocked())
      {
         for (wordIndex=0; wordIndex < maxIndex/*MAX_MOD_MEMWORDS*/; wordIndex++)
         {
            numRead = dat.Read((BYTE*)&dataWord, sizeof(WORD));
            PLCMemory.SetAt(area, wordIndex, dataWord);
            totalRead +=numRead;
            if (numRead != sizeof(WORD))
               return FALSE;
         }
         // Read past the rest of the block
         while (wordIndex < MAX_MOD_MEMWORDS)
         {
            numRead = dat.Read((BYTE*)&dataWord, sizeof(WORD));
            totalRead +=numRead;
            if (numRead != sizeof(WORD))
               return FALSE;
            wordIndex++;
         }
      }
      else
      {
      CString errorMsg;
         //error
         errorMsg.LoadString(IDS_SYNC_READING);
         AfxMessageBox(errorMsg, MB_ICONEXCLAMATION);
         return FALSE;
      }
   }
   return(TRUE);
}

// ----------------------------- LoadRegisters -----------------------------
// load binary dump of the register values from file.
BOOL CMOD232CommsProcessor::LoadRegisters()
{
BOOL ret = LoadRegistersIMP();

   if (ret)
      RSDataMessage("Register values loaded OK\n");
   return (ret);
} // LoadRegisters


// --------------------------------------- SaveRegisters ---------------------------
// save a binary dump of the values to file.
BOOL CMOD232CommsProcessor::SaveRegisters()
{
BOOL ret = SaveRegistersIMP();

   if (ret)
      RSDataMessage("Register values saved OK\n");
   return (ret);

}

// --------------------------------------- SaveRegistersIMP ---------------------------
// STATIC : save all register values to a flat file.
BOOL CMOD232CommsProcessor::SaveRegistersIMP()
{
CFileException ex;
CFile dat;
LONG area;
DWORD wordIndex;


   if (!dat.Open("MODDATA.DAT", CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate, &ex) )
   {
      // complain if an error happened
      // no need to delete the ex object

      TCHAR szError[1024];
      ex.GetErrorMessage(szError, 1024);
      OutputDebugString( "Couldn't open source file: ");
      OutputDebugString( szError);
      
      return FALSE;
   }
   // read it in
   for (area=0;area < MAX_MOD_MEMTYPES;area++)
   {
   WORD wordData;
   DWORD maxIndex = PLCMemory[area].GetSize();
      // lock the memory for writting
      CMemWriteLock lk(PLCMemory.GetMutex());
      // loop thru all registers (WORD)
      if (!lk.IsLocked())
      {
         for (wordIndex=0; wordIndex < maxIndex/*MAX_MOD_MEMWORDS*/; wordIndex++)
         {
            wordData = PLCMemory[area][wordIndex];
            dat.Write((BYTE*)&wordData, sizeof(WORD));
         }
         // Fill the rest with NULLs
         while (wordIndex < MAX_MOD_MEMWORDS)
         {
            wordData = 0;
            dat.Write((BYTE*)&wordData, sizeof(WORD));
            wordIndex++;
         }
      }
      else
      {
      CString errorMsg;
         errorMsg.LoadString(IDS_SYNC_WRITTING);
         AfxMessageBox(errorMsg, MB_ICONEXCLAMATION);
         // error
         return FALSE;
      }
   }
   return TRUE;
} // SaveRegistersIMP

// ------------------------------- ActivateStation ---------------------------
void CMOD232CommsProcessor::ActivateStationLED(LONG stationID)
{
   if (stationID>0 && stationID<STATIONTICKBOXESMAX)
   {
      //start the counter for this station at the beginning
      pGlobalDialog->m_microTicksCountDown[stationID] = pGlobalDialog->GetAnimationOnPeriod();
      // it will count down untill it extinguishes
   }
} // ActivateStation

// ------------------------------- StationIsEnabled ---------------------------
// Return TRUE if station is enabled
BOOL CMOD232CommsProcessor::StationIsEnabled(LONG stationID)
{
   if (stationID>0 && stationID<STATIONTICKBOXESMAX)
   {
      return (pGlobalDialog->StationEnabled(stationID));//m_microTickState==1);
   }
   return TRUE;
} // StationIsEnabled
