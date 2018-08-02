/////////////////////////////////////////////////////////////////////////////
//
// FILE: PLCApplication.cpp : implementation file
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
// implementation of the CPLCApplication class.
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


//////////////////////////////////////////////////////////////////////
// CPLCApplication IMPLEMENTATION
//
IMPLEMENT_DYNAMIC( CPLCApplication, CABCommsProcessor);

CPLCApplication::CPLCApplication(LPCTSTR portNameShort, 
                                     DWORD  baud, 
                                     DWORD byteSize, 
                                     DWORD parity, 
                                     DWORD stopBits,
                                     DWORD rts,
                                     int   responseDelay,
                                     BOOL  MOSCADchecks,
                                     BOOL modifyThenRespond,
                                     BOOL disableWrites,
									 BOOL bcc
                                     ) 
   : CABCommsProcessor(portNameShort,
                       baud,        
                       byteSize,     
                       parity,       
                       stopBits,     
                       rts,          
                       responseDelay,
					   bcc
                       )
{
   m_TNS = 0;
}

// ------------------------------- SendPLCMessage --------------------------------------
// pAppLayerMsg = buffer pointig to start of the app layer (stationID)
// length = length of the app layer only (before DLE expansion)
//
BOOL CPLCApplication::SendPLCMessage(const BYTE* pAppLayerMsg, DWORD length)  // if False, then re-send
{
   // call base class
   return(CABCommsProcessor::SendPLCMessage(pAppLayerMsg, length));
}

// ----------------------------- SendPLCBlock -------------------------------------
BOOL CPLCApplication::FetchPLCBlock(BYTE sourceStationID, 
                     BYTE destStationID, 
                     WORD TNS, 
                     WORD fileNum,
                     WORD startRegister,
                     WORD numRegisters)
{
CString info;
BYTE appMessage[MAX_AB_MESSAGELEN];
BYTE *pHeadderPtr;         // work pointer in the headder
DWORD lengthAppMsg=0;
BOOL sendOK;

   // Do BASIC range checking
   if (numRegisters>234)
      return(FALSE);

   info.Format("MASTER- Initiate %02X to %02X, READ file %d, registers %d to %d (trans=%04X)",
               sourceStationID, destStationID,
               fileNum, startRegister, startRegister+ numRegisters, TNS);
   RSDataMessage(info);


   appMessage[AB_SOURCEBYTEOFF] = sourceStationID;
   appMessage[AB_DESTINATIONBYTEOFF] = destStationID;
   appMessage[AB_COMMANDBYTEOFF] = ALLENBRADLEY_SLC_CMD; // 0x0F
   appMessage[AB_STATUSBYTEOFF] = AB_STS_NOERROR;        // 0x00
   *(WORD*)&appMessage[AB_TNSBYTEOFF] = TNS;
   appMessage[AB_FUNCBYTEOFF] = ALLENBRADLEY_WORD_READ;  // 0xA2

      // build the rest of the headder
   pHeadderPtr = &appMessage[AB_FUNCBYTEOFF+1];
   *pHeadderPtr++ = 2*numRegisters;   // # bytes to send in the frame
   *pHeadderPtr = (BYTE)fileNum;
   if (fileNum > 254)
   {
      *pHeadderPtr++ = 0xFF;
      *(WORD*)pHeadderPtr = fileNum;
      pHeadderPtr++;
   }
   pHeadderPtr++;
   *pHeadderPtr++ = 0x89;  // always INTEGER file
   // element#
   *pHeadderPtr = (BYTE)startRegister;
   if (startRegister > 254)
   {
      *pHeadderPtr++ = 0xFF;
      *(WORD*)pHeadderPtr = startRegister;
      pHeadderPtr++;
   }
   pHeadderPtr++;
   // sub-element is always 0
   *pHeadderPtr++ = 0x00;

   lengthAppMsg = (LONG)pHeadderPtr - (LONG)appMessage; // message pre-amble including STX
   //
   lengthAppMsg -= 2; // substract the STX

   sendOK = SendPLCMessage(&appMessage[2], lengthAppMsg);
   // await the response ACK and frame
   m_masterWaiting = TRUE;
   SetEngineState(ENG_STATE_RECEIVE);  // await a read-response
   return(sendOK);
}


// ----------------------------- SendPLCBlock -------------------------------------
BOOL CPLCApplication::SendPLCBlock(BYTE sourceStationID, 
                     BYTE destStationID, 
                     WORD TNS, 
                     WORD fileNum,
                     WORD startRegister,
                     WORD numRegisters)
{
BYTE appMessage[MAX_AB_MESSAGELEN];
DWORD lengthAppMsg=0;
BYTE *pHeadderPtr;   // work pointer in the headder
WORD *pwDataStart;
CString info;
BOOL sendOK;


   // Do BASIC range checking
   if (numRegisters>234)
      return(FALSE);

   // initiate a WRITE message to the required PLC, pack in the data and TNS field specified
   // use SendPLCMessage() to fire it off, then we will just wait for and do nothing with the response in
   // OnMessageReceived() which fires once the PLC responds to us.
   info.Format("MASTER- initiate %02X to %02X, SEND file %d, registers %d to %d (trans=%04X)",
               sourceStationID, destStationID,
               fileNum, startRegister, startRegister+ numRegisters, TNS);
   RSDataMessage(info);

   //
   appMessage[AB_SOURCEBYTEOFF] = sourceStationID;
   appMessage[AB_DESTINATIONBYTEOFF] = destStationID;
   appMessage[AB_COMMANDBYTEOFF] = ALLENBRADLEY_SLC_CMD; // 0x0F
   appMessage[AB_STATUSBYTEOFF] = AB_STS_NOERROR;        // 0x00
   *(WORD*)&appMessage[AB_TNSBYTEOFF] = TNS;
   appMessage[AB_FUNCBYTEOFF] = ALLENBRADLEY_WORD_WRITE; // 0xAA
   
   // build the rest of the headder
   pHeadderPtr = &appMessage[AB_FUNCBYTEOFF+1];
   *pHeadderPtr++ = 2*numRegisters;   // # bytes to send in the frame
   *pHeadderPtr = (BYTE)fileNum;
   if (fileNum > 254)
   {
      *pHeadderPtr++ = 0xFF;
      *(WORD*)pHeadderPtr = fileNum;
      pHeadderPtr++;
   }
   pHeadderPtr++;
   *pHeadderPtr++ = 0x89;  // always INTEGER file
   // element#
   *pHeadderPtr = (BYTE)startRegister;
   if (startRegister > 254)
   {
      *pHeadderPtr++ = 0xFF;
      *(WORD*)pHeadderPtr = startRegister;
      pHeadderPtr++;
   }
   pHeadderPtr++;
   // sub-element is always 0
   *pHeadderPtr++ = 0x00;

   // pack in the data
   pwDataStart = (WORD*)(pHeadderPtr);
   for (int i=0; i < numRegisters; i++)
   {
      *pwDataStart++ = PLCMemory[fileNum].GetAt(startRegister + i);
   }

   lengthAppMsg = (LONG)pHeadderPtr - (LONG)appMessage; // message pre-amble including STX
   lengthAppMsg += (2*numRegisters);
   //lengthAppMsg+= 4;  // Add in message-end framing : ETX and CRC fields
   lengthAppMsg -= 2; // substract the STX

   sendOK = SendPLCMessage(&appMessage[2], lengthAppMsg);
   // await the response ACK and frame
   m_masterWaiting = TRUE;
   SetEngineState(ENG_STATE_FINALACK);
   return(sendOK);
   
}


// ------------------------- OnMessageReceived ---------------------------------
// return false to reject the message
BOOL CPLCApplication::OnMessageReceived(BYTE* pAppLayerMsg, DWORD length)
{
BYTE appMessage[MAX_AB_MESSAGELEN];
DWORD lengthAppMsg=0;
BYTE  commandCode;

   CABMessage msg(pAppLayerMsg, length, FALSE);
   commandCode = msg.command;
   if ((commandCode & 0x40)==0x40)  // handle command responses for MASTER mode
   {
   CString debugMess;
      // this is a response frame, since the CPU layer already ACKed it before passing it here
      // we are done with it unless it is an error message frame
      if (msg.transmissionStatus)
      {
         debugMess.Format("MASTER : Message %04X delivered with STS error:\n", msg.transactionID);
         return(FALSE); // re-send frame
      }
      debugMess.Format("MASTER : Message %04X delivered OK:\n", msg.transactionID);
      RSDataMessage(debugMess);
      SetEngineState(ENG_STATE_MASTERIDLE);
      return(TRUE);
   }

   // simulate the I/O and network delays of a real PLC
   Sleep(m_responseDelay);
   if (pGlobalDialog->StationEnabled(msg.stationIDDest))
   {
      // build up a response
      BuildResponse(pAppLayerMsg, length, appMessage, &lengthAppMsg);

      SendPLCMessage(appMessage, lengthAppMsg);
      RSDataMessage("Await response ACK:");
      SetEngineState(ENG_STATE_FINALACK); //and then wait for the ack
   
      return(TRUE);
   }
   // station not listening
   SetEngineState(ENG_STATE_IDLE); //and then wait for the ack
   return(FALSE);
}

// --------------------------- BuildResponse ----------------------------------
void CPLCApplication::BuildResponse(const BYTE *inBuffer, DWORD inLength, 
                                    BYTE *buffer, DWORD *length)
{
CString diagnoseMsg;
char * dataPtr = NULL;
BOOL  ABError = TRUE;
WORD  ABErrorCode = 0;

   CABMessage msg(inBuffer, inLength, FALSE);

   RSDataMessage("Building Response Frame:");
   // determine if the request was valid
   if (AB_COMMAND_DRIVER == msg.command)
   {
      switch (msg.functionCode)
      {
         case (AB_FUNC_WRITE3 | 0x40):   // MASTER mode
            RSDataMessage("Write response received:");
            msg.count =0; // there no data in the response frame

            break;
         case AB_FUNC_WRITE3:  // SLAVE
            // build response
            {
            LONG dataStartOff=14;
               if (msg.fileNum >254)
                  dataStartOff+=2;     // 3-byte field was used
               if (msg.address >254)
                  dataStartOff+=2;

               msg.BuildMessagePreamble();
               dataPtr = &msg.buffer[dataStartOff];
               for (int i=0; i < msg.count/2; i++)
               {
               //CMemWriteLock  lk(PLCMemory.GetMutex());
                  diagnoseMsg.Format("Set File %d:%d = %d\n", msg.fileNum, msg.address + i, *(WORD*)dataPtr);
                  //RSDataMessage(diagnoseMsg);
                  PLCMemory[msg.fileNum].SetAt(msg.address + i, *(WORD*)dataPtr, PLCMemory.GetMutex());
                  dataPtr +=2;
               }
               ABError = FALSE;
               if (pGlobalDialog)
               {
                   int cols = pGlobalDialog->GetListDisplayedWidth();
                   pGlobalDialog->RedrawListItems(msg.fileNum, 
                                                  msg.address/(cols), 
                                                  (msg.address+((msg.count/2)-1))/(cols)
                                                 ); // repaint only the needed rows
               }

               RSDataMessage("Write processed OK:");
               msg.count =0; // there no data in the response frame
            }
            break;
         case AB_FUNC_READ3:   // SLAVE
            // build response
            {
            DWORD linePos=0;

               msg.BuildMessagePreamble();
               dataPtr = &msg.buffer[8];
               diagnoseMsg.Format("Get File %3d:%3d\n", msg.fileNum, msg.address);
               //RSDataMessage(diagnoseMsg);

               for (int i=0; i < msg.count/2; i++)
               {
               CMemWriteLock  lk(PLCMemory.GetMutex());
                  
                  diagnoseMsg.Format("%3d = x%04X |", msg.address + i, PLCMemory[msg.fileNum].GetAt(msg.address+i));
                  linePos+= diagnoseMsg.GetLength();
                  if (linePos > 79)
                  {
                     //RSDataMessage("\n");
                     linePos = diagnoseMsg.GetLength();
                  }
                  //RSDataMessage(diagnoseMsg);
                  *(WORD*)dataPtr = PLCMemory[msg.fileNum].GetAt(msg.address+i);
                  dataPtr +=2;
               }
               RSDataMessage("Read processed OK:");
               ABError = FALSE;
            }
            break;
         default:
            ABErrorCode = AB_STS_ILLEGALCMD + (AB_EXTSTS_ILLEGALFIELDVALUE << 8);
            break;
      }
   }
   else
   {
      ABErrorCode = AB_STS_ILLEGALCMD + (AB_EXTSTS_CANNOTEXECUTE << 8);
   }

   if (ABError)
   {
      msg.BuildMessagePreamble(ABError, ABErrorCode);
      //Send(msg.overalLen-8, (BYTE*)&msg.buffer[2], TRUE, NULL);
      // send a NAK or error message
   }
   else
   {

      *length = msg.count + 6;
      memcpy(buffer, (BYTE*)&msg.buffer[2], *length);
   }
} // 

// --------------------------- SetEmulationParameters -----------------------
void CAB232CommsProcessor::SetEmulationParameters(BOOL moscadChecks, 
                                                BOOL modifyThenRespond, 
                                                BOOL disableWrites)
{
   //m_MOSCADchecks = moscadChecks;
   m_modifyThenRespond = modifyThenRespond;
   m_disableWrites = disableWrites;
}
