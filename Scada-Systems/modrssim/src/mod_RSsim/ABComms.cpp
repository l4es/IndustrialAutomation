/////////////////////////////////////////////////////////////////////////////
//
// FILE: ABComms.cpp : implementation file
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
// implementation of the CABCommsProcessor class.
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "message.h"
#include "ABCommsProcessor.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//

PCHAR parityNames[] =
{
   "N ",     //NOPARITY    
   "O ",     //ODDPARITY   
   "E ",     //EVENPARITY  
   "Mark ",     //MARKPARITY  
   "Space"      //SPACEPARITY 
};

PCHAR stopNames[]=
{
   "1",
   "1.5",
   "2"
};



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// constructor to open port
IMPLEMENT_DYNAMIC( CABCommsProcessor, CCommsProcessor);


CABCommsProcessor::CABCommsProcessor(LPCTSTR portNameShort, 
                                     DWORD  baud, 
                                     DWORD byteSize, 
                                     DWORD parity, 
                                     DWORD stopBits,
                                     DWORD rts,
                                     int   responseDelay,
									         BOOL bcc
                                     ) 
   : CCommsProcessor(portNameShort,
                       19200, //baud,        
                       8,//byteSize,     
                       parity,       
                       ONESTOPBIT, //stopBits,     
                       RTS_CONTROL_ENABLE,//rts,          
                       responseDelay
                       ) 
{
CString portName;
CString description;

   if (PROTOCOL_SELAB232 == pGlobalDialog->m_selectedProtocol)
      SetProtocolName("Allen-Bradley DF1");
   else
      SetProtocolName("AB DF1 Master");


   m_ABmasterIDLETime = 100;
   m_ABtimeoutValue = 300;
   m_masterBusy = FALSE;               // master-mode PLC internal state flags
   m_masterWaiting = FALSE;
   m_guardJunk = 0xAAAAAAAA;
   m_lastMasterTick = GetTickCount();  //
   m_useBCCchecks = TRUE;
   m_primaryPolls = 0;

   m_CPUmode = (pGlobalDialog->m_PLCMaster? MODE_MASTER : MODE_SLAVE);
   m_CPUstateEngineStep = ENG_STATE_IDLE;
   m_noiseLength = 0;
   //m_transactionID = 0;
   memset(m_masterTN, 0 , sizeof(m_masterTN));

   m_ABMasterSource = 1; 
   m_ABMasterDest = 10;   
   m_ABMasterNumFiles = 22;
   m_ABMasterRunMode = TRUE;
   m_ABMasterReadVariables = FALSE;
   m_ABMasterWriteVariables = TRUE;
   m_ABmasterIDLETime = 100;
   m_ABtimeoutValue = 300;


   if (PROTOCOL_SELAB232 == pGlobalDialog->m_selectedProtocol)
   {
      m_useBCCchecks = bcc;
      // SLC PLC defaults for DF1
      if ((pGlobalDialog->m_baud !=19200)||
          (pGlobalDialog->m_parity != NOPARITY)||
          (pGlobalDialog->m_byteSize != 8)||
          (pGlobalDialog->m_rts != RTS_CONTROL_ENABLE)||
          (pGlobalDialog->m_stopBits != ONESTOPBIT))
          if (IDYES == AfxMessageBox("Load DF1 port setting defaults Y/N?\n Settings: 19200,8,N,1 and RTS enabled", MB_YESNO))
      {
         pGlobalDialog->m_baud = 19200;
         pGlobalDialog->m_parity = NOPARITY;
         pGlobalDialog->m_byteSize = 8;
         pGlobalDialog->m_rts = RTS_CONTROL_ENABLE;
         pGlobalDialog->m_stopBits = ONESTOPBIT;
         pGlobalDialog->SaveApplicationSettings();
      }
   }
   else
   {
      m_CPUmode = MODE_MASTER;
      //m_useBCCchecks = TRUE;
      // Allen-bradley DF-1 Master
      if ((pGlobalDialog->m_baud !=19200)||
          (pGlobalDialog->m_parity != EVENPARITY)||
          (pGlobalDialog->m_byteSize != 8)||
          (pGlobalDialog->m_rts != RTS_CONTROL_ENABLE)||
          (pGlobalDialog->m_stopBits != ONESTOPBIT))
          if (IDYES == AfxMessageBox("Load Allen-Bradley DF1 Master port setting defaults Y/N?\n Settings: 19200,8,E,1 and RTS enabled", MB_YESNO))
      {
         pGlobalDialog->m_baud = 19200;
         pGlobalDialog->m_parity = EVENPARITY;
         pGlobalDialog->m_byteSize = 8;
         pGlobalDialog->m_rts = RTS_CONTROL_ENABLE;
         pGlobalDialog->m_stopBits = ONESTOPBIT;
         pGlobalDialog->SaveApplicationSettings();
      }
   }

   // 
   portName.Format("%s %d:%d-%s-%s", portNameShort, pGlobalDialog->m_baud, pGlobalDialog->m_byteSize, parityNames[pGlobalDialog->m_parity], stopNames[pGlobalDialog->m_stopBits]);

   portName += (IsMaster()?" -Master":" -Slave");
   description.Format("Starting comms emulation : %s", portName);
   RSDataMessage(description);

   // open the port etc...
   if (OpenPort(portNameShort))
   {
   COMMTIMEOUTS timeout;
   BOOL err;
   DWORD reason;
      ConfigurePort(pGlobalDialog->m_baud, pGlobalDialog->m_byteSize, pGlobalDialog->m_parity, pGlobalDialog->m_stopBits, pGlobalDialog->m_rts, (NOPARITY==parity?FALSE:TRUE));
      //set the timeouts shorter

      // fill timeout structure
      GetCommTimeouts(h232Port, &timeout);
      timeout.ReadIntervalTimeout = 100;           // 
      timeout.ReadTotalTimeoutConstant = 200;
      timeout.ReadTotalTimeoutMultiplier = 0;      // #chars to read does not add to timeout amount
      timeout.WriteTotalTimeoutConstant = 2000;
      timeout.WriteTotalTimeoutMultiplier = 60;    // 60ms per char sent
                                                
      err = SetCommTimeouts(h232Port, &timeout);
      reason = GetLastError();
   }
   m_responseDelay = responseDelay;

}

// ------------------------------- MasterTimedOut ---------------------------
BOOL CABCommsProcessor::MasterTimedOut()
{
LONG tick = (LONG)GetTickCount();

   if (tick - (LONG)m_lastMasterTick > (LONG)m_ABmasterIDLETime) //PORT_MAX_IDLETIME*3)
   {
      return(TRUE);
   }
   return(FALSE);
}

// ------------------------------------- DoMaster ----------------------------
// called in the IDLE state
void CABCommsProcessor::DoMaster()
{
//static WORD m_masterTN[256];

   //check if scripts are running
   if (pGlobalDialog->ScriptIsRunning())
      return;
   
   // check if we may send
   if (m_masterBusy && (ENG_STATE_IDLE == m_CPUstateEngineStep))
   {
      if (MasterTimedOut() || m_masterWaiting)
      {
         m_lastMasterTick = GetTickCount();  // transaction has ended
         m_masterBusy = FALSE;
         m_masterWaiting = FALSE;
         return;
      }
   }
   if (m_masterWaiting)
      return;  // not timed out yet, hold on
   
   // check if it is time to send again
   if (GetTickCount() - m_lastMasterTick > m_ABtimeoutValue)//ABMASTER_IDLETIME )
   {  // send a message.

// *************************************************      
// The following code is a sample hard-coded master.
// U can put in your own code here to simulate master 
// activity, alternately write a script and send/fetch
// blocks from script VB is not my strongest language, 
// hence this code for the master section.


   CPLCApplication *pApp;
   WORD fileNumber, numElements = 10;
static  WORD lastShieldNumber;      // starting shield number is arbitrary
static  WORD lastCutterDirection;   // direction is arbitrary

      // go back to shield 0 (non-existent) to make us move to shield 1 on the next transaction
      if (lastShieldNumber >= ABMASTER_NUMFILES)
         lastShieldNumber = 0;

      m_masterBusy = TRUE;
      m_masterWaiting = FALSE;
      // we need to use the App layer to do this, so let's verify our app layer object
      pApp = (CPLCApplication *)this;
      ASSERT(pApp->IsKindOf(RUNTIME_CLASS(CPLCApplication)));

      if (m_ABMasterReadVariables && (0 >= pApp->m_primaryPolls))
      { // do a shearer read
         fileNumber = ABMASTER_FIRSTFILENUM + (m_masterRandom % (WORD)ABMASTER_NUMFILES) + (WORD)ABMASTER_FIRSTSHIELD;
         pApp->FetchPLCBlock(AB_MASTER_ID, // source
                             ABMASTER_PRIMARYPLC_ID,
                             GetNextTransactionID(ABMASTER_PRIMARYPLC_ID), // increments the TNS as well
                             fileNumber,
                             0,         // start offset
                             numElements // 40         // # elements
                            );
         pApp->m_primaryPolls = ABMASTER_PRIMARYPLCPOLLCOUNT;
      }
      else
      {
         if (m_ABMasterWriteVariables)
         {
            // decide which PLC file # to send data for
            if (0 == m_masterRandom % 10)
            {
            WORD direction, cutterPosition;
            WORD lastCutterPosition;    // position
            BOOL oldDirection;

               // every 10 transactions, we send cutter motor position info again
               fileNumber = 99;
               numElements = 3;  // cutter info is 3 items only
               lastCutterPosition = PLCMemory[99][0];

               if ((lastCutterPosition > ABMASTER_NUMFILES)||(0 == lastCutterPosition))// cutter pos wrap check
                  lastCutterPosition = 1;

               direction = PLCMemory[99][1];
               oldDirection = direction;
               cutterPosition = lastCutterPosition;
               switch(direction)
               {
               case 1 : //up
                  if (lastCutterPosition == ABMASTER_NUMFILES)
                     PLCMemory.SetAt(99, 1, 2); // change direction to DOWN
                  else
                     PLCMemory.SetAt(99, 1, 1);
                  break;
               case 2 : // down
               default:
                  if (lastCutterPosition == 1)
                     PLCMemory.SetAt(99, 1, 1); // change direction to UP
                  else
                     PLCMemory.SetAt(99, 1, 2);
                  break;
               }
               
               // use direction to determine shield #
               direction = PLCMemory[99][1];
               if (direction == oldDirection)
               { // cutter does not actually move on the "turn-around"
                  switch(direction)
                  {
                  case 1 : //up
                     cutterPosition = lastCutterPosition+1;
                     break;
                  case 2 : // down
                  default:
                     cutterPosition = lastCutterPosition-1;
                     break;
                  }
               }
               // update things
               PLCMemory.SetAt(99, 0, cutterPosition);
               PLCMemory.SetAt(99, 2, ABMASTER_NUMFILES);
            }
            else
            {
               // increment
               lastShieldNumber++;
               fileNumber = lastShieldNumber + 100;
            }
            //fileNumber = 101 + (m_masterRandom % JOY_NUMSHIELDS) + JOY_FIRSTSHIELD;

            if (0 == m_masterRandom % 10) // every 10th message is the cutter position
            {
               fileNumber = 99;
            }
            pApp->SendPLCBlock((BYTE)AB_MASTER_ID,       // source
                               (BYTE)ABMASTER_PRIMARYPLC_ID,  // destination
                               GetNextTransactionID(ABMASTER_PRIMARYPLC_ID), // increments the TNS as well
                               fileNumber, 
                               ABMASTER_ELEMENTS_START,
                               numElements );  // 10 registers
            pApp->m_primaryPolls --;
         }
         else
            pApp->m_primaryPolls = 0;  // read again
      }
      m_masterRandom++;
   }
}
   
// ------------------------------- StationIsEnabled ---------------------------
// Return TRUE if station is enabled
BOOL CABCommsProcessor::StationIsEnabled(LONG stationID)
{
   if (stationID>0 && stationID<STATIONTICKBOXESMAX)
   {
      return (pGlobalDialog->StationEnabled(stationID));//m_microTickState==1);
   }
   return TRUE;
} // StationIsEnabled

// --------------------------------- SendPLCMessage ------------------------------------
// pAppLayerMsg = buffer pointig to start of the app layer (stationID)
// length = length of the app layer only (before DLE expansion)
//
BOOL CABCommsProcessor::SendPLCMessage(const BYTE* pAppLayerMsg, DWORD length)  // if False, then re-send
{
BYTE telegram[MAX_AB_MESSAGELEN];
DWORD telegramLength;
WORD  crc = 0;
BYTE bcc = 0;
BYTE *crcStartPtr;

   crcStartPtr = (BYTE*)pAppLayerMsg;
   m_noiseLength = 0;   // kill all stuff in the buffer before we start to respond

   // send of DLE-STX
   // calculate CRC
   // send telegram (expand DLEs)
   // send DLE-ETX and CRC

   //////////////////////////////////////////////////////////
   // send DLE-STX
   Send(2, txDLE_STX, FALSE, NULL);

   // calc CRC
   {
   CHAR ETXBuff[2];

      crcStartPtr = (BYTE*)pAppLayerMsg;
      CalcCRC(crcStartPtr, length, &crc);      // application layer CRC

      ETXBuff[0] = 0x03;
      CalcCRC((BYTE*)&ETXBuff[0], 1, &crc);      // include the ETX
   }
   // calc a bcc
   bcc = CalcBCC(crcStartPtr, length);
   
   // send telegram
   Send(length, pAppLayerMsg, TRUE, NULL);
   
   //make a copy in case it gets lost on the device side, and they NAK us
   m_lastAppLength = length;
   memcpy(m_lastAppBuffer, pAppLayerMsg, length);

   //Send DLE-ETX
   Send(2, txDLE_ETX, FALSE, NULL);

   if (!m_useBCCchecks)
   {
	   // send CRC
	   *(WORD*)telegram = crc;
	   telegramLength = 2;
	   Send(telegramLength, telegram, FALSE, NULL);
   }
   else
   {
	   // send a bcc
	   *(BYTE*)telegram = bcc;
	   telegramLength = 1;
	   Send(telegramLength, telegram, FALSE, NULL);
   }
   return(TRUE);
}

// ----------------------------- SetEngineState -----------------------------
void CABCommsProcessor::SetEngineState(__EngineStates stateStep)
{
   switch (stateStep)
   {
   case ENG_STATE_IDLE:    // wait for start
         m_CPUstateEngineStepname = "idle";
         break;
   case ENG_STATE_RECEIVE:   // RX rest of telegram + ETX + CRC
         m_CPUstateEngineStepname = "rx request body";
         break;
   case ENG_STATE_SENDACK:   // sending an ACK
         m_CPUstateEngineStepname = "ack request";
         break;
   case ENG_STATE_RESPOND:   // sending msg response
         m_CPUstateEngineStepname = "respond";
         break;
   case ENG_STATE_FINALACK:   // wait for final ACK
         m_CPUstateEngineStepname = "wait for ack";
         break;
   case ENG_STATE_MASTERIDLE: // master eneded last cycle, will start next transaction without waiting for an RX
         m_CPUstateEngineStepname = "master end-of-cycle";
         m_masterHasWork = TRUE; // skip the RX, and do the DoMaster() again, 
                                 // since we just done one transaction, and want to do another right away
         break;
   default:
         m_CPUstateEngineStepname = "unknown!!!";
         break;
   }
   m_CPUstateEngineStep = stateStep;
}



// ------------------------- GetNextTransactionID ----------------------------
WORD CABCommsProcessor::GetNextTransactionID(BYTE station)
{
   return (++m_masterTN[station]);
}

// -------------------------- GetCurrentTransactionID -------------------------
WORD CABCommsProcessor::GetCurrentTransactionID(BYTE station)
{
   return (m_masterTN[station]);
}

// -------------------------- SetCurrentTransactionID -------------------------
WORD CABCommsProcessor::SetCurrentTransactionID(BYTE station, WORD id)
{
WORD last = m_masterTN[station];
   m_masterTN[station] = id;
   return (last);
}

// ------------------------------ RSDataDebugger ------------------------------
void CABCommsProcessor::RSDataDebugger(const BYTE * buffer, LONG length, int transmit)
{
   CRS232Port::GenDataDebugger(buffer,length,transmit);
} // RSDataDebugger

// ------------------------------------------ Scentence ----------------------------
CString &Scentence(LPCTSTR text, BOOL startScentence)
{
static CString scentence;

   scentence = text;
   if (startScentence)
   {
      if (islower(text[0]))
         scentence.SetAt(0, toupper(text[0]));
   }
   else
   {
      if (isupper(text[0]))
         scentence.SetAt(0, tolower(text[0]));
   }
   return(scentence);
}

// ------------------------------------------- Get232ErrorDisc ----------------------
//
CString &Get232ErrorDisc(DWORD commError)
{
DWORD mask = 0x01, index;
BOOL and = FALSE;
static CString description;
#define AND    " and ";

   description.Format ("x%X ", commError);
   for (index = 0 ; index < 16; index++)
   {
      switch (commError & mask)
      {                                                         
         case CE_BREAK    :    // The hardware detected a break condition.
            if (and)
               description += AND;
            description += Scentence("The hardware detected a break condition", !and);
            and = TRUE;
            break;
         case CE_FRAME    :    // The hardware detected a framing error.
            if (and)
               description += AND;
            description += Scentence("The hardware detected a framing error", !and);
            and = TRUE;
            break;
         case CE_IOE      :    // An I/O error occurred during communications with the device.
            if (and)
               description += AND;
            description += Scentence("An I/O error occurred during communications with the device", !and);
            and = TRUE;
            break;
         case CE_MODE     :    // The requested mode is not supported, or the hCommDev parameter is
                               // invalid. If this bit set, this is the only valid error.
            if (and)
               description += AND;
            description += Scentence("The requested mode is not supported", !and);
            and = TRUE;
            break;

         case CE_OVERRUN  :    // A character-buffer overrun has occurred. The next character is lost.
            if (and)
               description += AND;
            description += Scentence("A character-buffer overrun has occurred", !and);
            and = TRUE;
            break;
         case CE_RXOVER   :    // An input buffer overflow has occurred. There is either no room in the input
                               // buffer, or a character was received after the end-of-file (EOF) character
                               //  was received.
            if (and)
               description += AND;
            description += Scentence("An input buffer overflow has occurred", !and);
            and = TRUE;
            break;
         case CE_RXPARITY :    // The hardware detected a parity error.
            if (and)
               description += AND;
            description += Scentence("The hardware detected a parity error", !and);
            and = TRUE;
            break;
         case CE_TXFULL   :    // The application tried to transmit a character, but the output buffer was full.
            if (and)
               description += AND;
            description += Scentence("Transmision of a character failed, output buffer full", !and);
            and = TRUE;
            break;
         default          :
            break;
      }
      //next bit
      mask = mask << 1;
   }
   description += ".";
   return(description);
}

// --------------------------------- OnHWError ---------------------------------
void CABCommsProcessor::OnHWError(DWORD dwCommError)
{
   // restart interpreter
   if (0 == dwCommError)
   {
      RSDataMessage("COMM IDLE: Restarting interpretation.");
   }
   else
   {
      RSDataMessage(Get232ErrorDisc(dwCommError));
   }
   m_noiseLength = 0;
   // TODO! if recieving a response, and it is corrupted in this way, 
   // send a DLE-ENQ and stay on this step for 3 retries
   if (m_CPUstateEngineStep != ENG_STATE_IDLE)
   {
      if (dwCommError)
         Send(2, txDLE_ENQ, FALSE, NULL); // send a ??? out

      SetEngineState(ENG_STATE_IDLE);
   }
}

// ------------------------------- RSStateChanged -----------------------
void CABCommsProcessor::RSStateChanged(DWORD state)
{
   EnterCriticalSection(&stateCS);
   if (NULL==pGlobalDialog)
      return;
   pGlobalDialog->m_ServerRSState = state;
   LeaveCriticalSection(&stateCS);
} // RSStateChanged

// ------------------------------ RSDataMessage ------------------------------
void CABCommsProcessor::RSDataMessage(LPCTSTR msg)
{
CString message;
   EnterCriticalSection(&stateCS);
   message = "##";
   message += msg;
   //OutputDebugString("##");
   OutputDebugString(message);
   OutputDebugString("\n");
   if (NULL!=pGlobalDialog)
      pGlobalDialog->AddCommsDebugString(message);
   LeaveCriticalSection(&stateCS);
}

// ------------------------------- RSModemStatus ---------------------------
void CABCommsProcessor::RSModemStatus(DWORD status)
{
   EnterCriticalSection(&stateCS);
   if (NULL!=pGlobalDialog)
      pGlobalDialog->SendModemStatusUpdate(status);
   LeaveCriticalSection(&stateCS);
}


// ------------------------------ GetRALength --------------------------------
// Return the # bytes still to read
LONG CABCommsProcessor::GetRALength()
{
WORD minFrameLength = 18;

   if (this->m_useBCCchecks)
      minFrameLength --;

   switch (m_CPUstateEngineStep)
   {
      case ENG_STATE_IDLE:
      case ENG_STATE_FINALACK:
         if ((m_noiseLength==1) && (m_noiseBuffer[0] == ALLENBRADLEY_DLE))
            return(1);
          return(2);
          break;
      case ENG_STATE_RECEIVE:
         {
         LONG length=1;
            if (m_noiseLength < minFrameLength)
               length = minFrameLength-m_noiseLength;
            if ((m_noiseLength > AB_LENGTHFRAMEOFF) && 
                ((m_noiseBuffer[AB_COMMANDBYTEOFF]== ALLENBRADLEY_SLC_CMD) &&
                 (m_noiseBuffer[AB_FUNCBYTEOFF]== ALLENBRADLEY_WORD_WRITE))
               )
            {
            DWORD correctDataLen;
               // write data function, read up to # registers sent
               // calculate minimum length for a 4-byte address, the data and the ETX and CRC
               correctDataLen = m_noiseBuffer[AB_LENGTHFRAMEOFF] + 4 + (AB_LENGTHFRAMEOFF+1) + 4;
               length = correctDataLen-m_noiseLength;
               if (length <=0)
                  length=1;
            }
            return (length);
         }
         break;
   }
   return(1);
}


// --------------------------------- ProcessData -----------------------------
// Interpret MODBUS request pBuffer, and respond to it.
//
BOOL CABCommsProcessor::OnProcessData(const CHAR *pBuffer, DWORD numBytes, BOOL *discardData)
{
int   i=0;
WORD  guardword1=1;
WORD  guardword2=2;
WORD minFrameLength = 18;
WORD minRESP_FrameLength = 12;

   if (m_useBCCchecks)
   {
      minFrameLength--;
      minRESP_FrameLength--;
   }

   // build noise telegram
   if (numBytes)
   { //append recieved bytes to the noise telegram
      if (m_noiseLength + numBytes >= sizeof(m_noiseBuffer))
      {
         RSDataMessage("OVERFLOW:Restarting interpretation.");

         m_noiseLength = 0;
         SetEngineState(ENG_STATE_IDLE);
         return(TRUE);
      }
      // append bytes to noise telegram
      memcpy(&m_noiseBuffer[m_noiseLength], pBuffer, numBytes);
      m_noiseLength += numBytes;
      *discardData = TRUE;
   }
   //else
      Sleep(0);
   
//   ASSERT((m_noiseLength<50) || (0==numBytes));
   if (m_masterBusy && MasterTimedOut())
   { // SEND message timed out
      m_noiseLength = 0;
      SetEngineState(ENG_STATE_IDLE);
   }

   switch (m_CPUstateEngineStep)
   {
      case ENG_STATE_MASTERIDLE :
         SetEngineState(ENG_STATE_IDLE);
      case ENG_STATE_IDLE :
         m_messageNAKs = 0;
         if (IsMaster())
         {
            // PLC master may initiate a transaction now
            DoMaster();
            if ((m_masterBusy && (!m_masterWaiting)))
            {
               return(FALSE);
            }
            // otherwize, master is idle, Or waiting for response, continue.
         }
         if (m_noiseLength >= 2)
         {
            if (( m_noiseBuffer[0] == ALLENBRADLEY_DLE)&&
                (m_noiseBuffer[1] == ALLENBRADLEY_STX ))
            {
               RSDataMessage("DLE-STX recieved");
               SetEngineState(ENG_STATE_RECEIVE);
               if (m_noiseLength > 2)
                  return(OnProcessData((char*)m_noiseBuffer, 0, discardData));
               return TRUE;
            }
            // strip the leading char from the noise buffer
            m_noiseLength--;
            memmove(m_noiseBuffer, &m_noiseBuffer[1], m_noiseLength);
            // recurse to keep looking at the buffer untill we get a match or an empty buffer
            if (m_noiseLength >= 2)
            {
               return(OnProcessData((char*)m_noiseBuffer, 0, discardData));
            }
            return(TRUE);
         }
         //wait for more data
         return(FALSE);
         break;
      case ENG_STATE_RECEIVE :
         // DLE STX ---> DST SRC CMD STS TNS TNS FNC CNT FIL ADDR ADDR [ADDR] .... DLE ETX CRC CRC
         {
         BYTE  nodleBuffer[MAX_ABPORT_MESSAGELEN];
         DWORD nodleLen;
         BOOL valid;

            if (m_noiseLength >= 2)
            {
               if (( m_noiseBuffer[0] == ALLENBRADLEY_DLE)&&
                   (m_noiseBuffer[1] == ALLENBRADLEY_ACK ))
               {
                  RSDataMessage("GOT ACK: await remaining response.");
                  // strip these 2 chars
                  m_noiseLength-= 2;
                  memmove(m_noiseBuffer, &m_noiseBuffer[2], m_noiseLength);
                  memset(&m_noiseBuffer[m_noiseLength], 0xAA, 2);
               }
            }
            // strip double DLE's 
            RemoveDLE(m_noiseBuffer, m_noiseLength, nodleBuffer, &nodleLen);

            valid= (nodleLen >= minFrameLength);
            if ((nodleLen >= minRESP_FrameLength/*12*/) && ((nodleBuffer[AB_COMMANDBYTEOFF] & 0x40)==0x40)) // command response
               valid = TRUE;
            if (valid)
            {
            CHAR ETXBuff[2];
            BYTE *crcStartPtr;
            WORD  crc = 0;
            WORD *crcPtr;
            BYTE  bccCode;
            BYTE *bccPtr;
            BOOL checksummingOK = FALSE;

               //memcpy(&nodleBuffer[nodleLen], &m_receivedBuffer[m_noiseLength-2],2);

               crcPtr = (WORD*)&nodleBuffer[nodleLen-2];   //skip the DLE STX
               bccPtr = (BYTE*)&nodleBuffer[nodleLen-1];  
               crcStartPtr = (BYTE*)&nodleBuffer[2];

               crc = 0;
               CalcCRC(crcStartPtr, nodleLen-6, &crc);      // Only one buffer to calc crc of
               bccCode = CalcBCC(crcStartPtr, nodleLen-5);      
               // include the ETX, remember that the DLE gets ignored
               ETXBuff[0] = 0x03;
               CalcCRC((BYTE*)&ETXBuff[0], 1, &crc);      // include the ETX

               if (!m_useBCCchecks)
                  checksummingOK = (crc == *crcPtr);
               else
                  checksummingOK = (bccCode == *bccPtr);

               if (checksummingOK)
               {
               CABMessage msg(&nodleBuffer[2], nodleLen-2-4, FALSE);

                  ActivateStationLED((BYTE)nodleBuffer[AB_SOURCEBYTEOFF]);
                  ActivateStationLED((BYTE)nodleBuffer[AB_DESTINATIONBYTEOFF]);
                  if (pGlobalDialog->StationEnabled(msg.stationIDDest))
                  {
                  DWORD dataLength = nodleLen-2-4; // exclude the STX, ETX and CRC portions
                     // send an ACK, and process the message
                     RSDataMessage("ACK device message.");
                     Send(2, txDLE_ACK, FALSE, NULL);
                     if (pGlobalDialog->m_useBCC)
                        dataLength++;
                     BOOL result = OnMessageReceived(&nodleBuffer[2], dataLength ); //(-2-4) 
                     // will do it's own SetEngineState(ENG_STATE_FINALACK);
                  }
                  else
                  {
                  CString message;
                     message.Format("Station %d is off-line, no response will be sent", msg.stationIDDest);
                     RSDataMessage(message);
                     SetEngineState(ENG_STATE_IDLE);
                  }
                  m_noiseLength = 0;
                  return (TRUE);
               }
               else
               {
               DWORD etxPos=-1;
               DWORD stxPos = -1;

                  // try find a DLE-ETX
                  if (FindFragment(nodleBuffer, nodleLen, txDLE_ETX, 2, &etxPos))
                     if (etxPos+4 <= nodleLen)
                  {
                     FindFragment(nodleBuffer, nodleLen, txDLE_STX, 2, &stxPos);
                     if (stxPos < etxPos)
                     { // prevent parsing of an STX occuring after an ETX
                        RSDataMessage("Cannot match STX-ETX framing, removing DLE-ETX.");
                        m_noiseLength-= etxPos+2;
                        // swallow all chars up to & including the DLE-ETX
                        memmove(m_noiseBuffer, &m_noiseBuffer[etxPos+2], m_noiseLength);
                        memset(&m_noiseBuffer[m_noiseLength], 0xAA, sizeof(m_noiseBuffer)-m_noiseLength);
                        SetEngineState(ENG_STATE_IDLE);
                        return(TRUE);
                     }
                     crcStartPtr = (BYTE*)&nodleBuffer[stxPos + 2];
                     crcPtr = (WORD*)&nodleBuffer[etxPos+2];   //skip the DLE STX
                     crc = 0;
                     bccCode = 0;
                     ASSERT(stxPos > etxPos); // Fix this possible - number
                     CalcCRC(crcStartPtr, etxPos-2- stxPos, &crc);      // Only one buffer to calc crc of
                     
                     bccCode = CalcBCC(crcStartPtr, etxPos-2- stxPos);
                     // include the ETX, remember that the DLE gets ignored
                     ETXBuff[0] = 0x03;
                     CalcCRC((BYTE*)&ETXBuff[0], 1, &crc);      // include the ETX

                     if (!m_useBCCchecks)
                        checksummingOK = (crc == *crcPtr);
                     else
                        checksummingOK = (bccCode == *bccPtr);

                     if (checksummingOK)
                     {
                        ActivateStationLED((BYTE)nodleBuffer[AB_SOURCEBYTEOFF]);
                        ActivateStationLED((BYTE)nodleBuffer[AB_DESTINATIONBYTEOFF]);
                  
                        // send an ACK, and process the message
                        RSDataMessage("ACK device message.");
                        Send(2, txDLE_ACK, FALSE, NULL);
                        BOOL result = OnMessageReceived(&nodleBuffer[2], nodleLen-2-4); //(-2-4) exclude the STX, ETX and CRC portions
                        // will do it's own SetEngineState(ENG_STATE_FINALACK);
                        //m_noiseLength = 0;
                        return (TRUE);
                     }
                     else
                     {  // kill this frame, I cannot match a CRC for it
                        if (FindFragment(m_noiseBuffer, m_noiseLength, txDLE_ETX, 2, &etxPos))
                        {
                           RSDataMessage("Skipping frame, cannot match the CRC.");
                           m_noiseLength-= etxPos+4;
                           memmove(m_noiseBuffer, &m_noiseBuffer[etxPos+4], m_noiseLength);
                           SetEngineState(ENG_STATE_IDLE);
                           return(TRUE);
                        }
                     }
                  }
               }

            }
         }
         return(TRUE);
         break;
      case ENG_STATE_FINALACK :
         if (m_noiseLength >= 2)
         {
            if (( m_noiseBuffer[0] == ALLENBRADLEY_DLE)&&
                (m_noiseBuffer[1] == ALLENBRADLEY_ACK ))
            {
               // strip these 2 chars
               m_noiseLength-= 2;
               memmove(m_noiseBuffer, &m_noiseBuffer[2], m_noiseLength);
               // determine next state
               SetEngineState(ENG_STATE_IDLE);   //return to idle state
               RSDataMessage("SEND: completed OK:");
               if (m_noiseLength)
               {
                  // process the leftover data/next request in our buffer
                  return(OnProcessData((char*)m_noiseBuffer, 0, discardData));
               }
               return TRUE;
            }
            // NAK
            if (( m_noiseBuffer[0] == ALLENBRADLEY_DLE)&&
                (m_noiseBuffer[1] == ALLENBRADLEY_NAK ))
            {
               m_messageNAKs++;
               if (m_messageNAKs >= AB_MAXIMUM_NAKS)
               {
                  RSDataMessage("Too many NAKs, will abort:");
                  Send(2, txDLE_ENQ, FALSE, NULL);
                  SetEngineState(ENG_STATE_IDLE);   // wait for ACK again
                  m_noiseLength = 0;
                  return TRUE;
               }
               RSDataMessage("NAK, will re-send:");
               SendPLCMessage(m_lastAppBuffer, m_lastAppLength);
               SetEngineState(ENG_STATE_FINALACK);   // wait for ACK again
               m_noiseLength = 0;
               return TRUE;
            }
            // strip the leading char from the noise buffer
            RSDataMessage("Bad device final ACK response, restarting interpreter:");
            SetEngineState(ENG_STATE_IDLE);   //return to idle state
            m_noiseLength = 0;
            return(TRUE);
         }
         //wait for more data
         return(FALSE);
         
         break;
      default:
         return(TRUE);
         break;
   }

   return(TRUE);
}


// ----------------------------- LoadRegisters -----------------------------
// load binary dump of the register values from file.
BOOL CABCommsProcessor::LoadRegisters()
{
CFileException ex;
CFile dat;
LONG area;
DWORD wordIndex;

   if (!dat.Open("ABDATA.DAT", CFile::modeRead|CFile::shareDenyRead, &ex) )
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
   for (area=0;area < MAX_AB_MEMFILES ; area++)
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
         errorMsg = "an error occured!";//errorMsg.LoadString(IDS_SYNC_READING);
         AfxMessageBox(errorMsg, MB_ICONEXCLAMATION);
         return FALSE;
      }
   }
   return TRUE;
} // LoadRegisters


// --------------------------------------- SaveRegisters ---------------------------
// save a binary dump of the values to file.
BOOL CABCommsProcessor::SaveRegisters()
{
CFileException ex;
CFile dat;
LONG area;
DWORD wordIndex;


   if (!dat.Open("ABDATA.DAT", CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate, &ex) )
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
   for (area=0;area < MAX_AB_MEMFILES; area++)
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
         while (wordIndex < maxIndex)
         {
            wordData = 0;
            dat.Write((BYTE*)&wordData, sizeof(WORD));
            wordIndex++;
         }
      }
      else
      {
      CString errorMsg;
         errorMsg = "an error occured!";//.LoadString(IDS_SYNC_WRITTING);
         AfxMessageBox(errorMsg, MB_ICONEXCLAMATION);
         // error
         return FALSE;
      }
   }
   
   return TRUE;
} // SaveRegisters

// ------------------------------------------ TestReceiveMessage -----------------------------
BOOL CABCommsProcessor::TestReceiveMessage()
{
   return(FALSE);
}

// ----------------------------------------- OnMessageReceived -------------------------------
BOOL CABCommsProcessor::OnMessageReceived(BYTE * pAppMessage, DWORD length)
{
   return(FALSE);
}

// --------------------------- RemoveDLE ----------------------------------
LONG CABCommsProcessor::RemoveDLE(const BYTE * buffer, DWORD len, BYTE * destBuff, DWORD *destLen)
{
DWORD dleCount=0;
BYTE *pSource, *pDest;
DWORD i=0;

   pSource = (BYTE*)buffer;
   pDest = destBuff;
   *destLen = len;
   while( i < len)
   {
      if ((*pSource == ALLENBRADLEY_DLE) &&     // Detect first DLE and post inc Ptr
       // check if it's a double
         (pSource[1] == ALLENBRADLEY_DLE))
      { // copy one of the doubled chars only
         pSource++;
         i++;
         if (i < len)
         { // verify this was not the buffer end
            *pDest++ = *pSource++;
            i++;
            dleCount++;
         }
      }
      else
      { // copy char
         *pDest++ = *pSource++;
         i++;
      }
   }   
   // subtract the # of double-DLEs actually removed
   *destLen -= dleCount;
   return(dleCount);
}                                 

