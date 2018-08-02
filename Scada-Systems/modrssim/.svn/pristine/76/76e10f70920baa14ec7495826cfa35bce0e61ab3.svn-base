// RS232Noise.cpp: implementation of the CRS232Noise class.
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
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RS232Noise.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRS232Noise::CRS232Noise()
{
   SetErrorTypes(FALSE, 0); // set for no errors
   m_nukedPort = FALSE;
   reserved = 0x5555AAAA;
   m_nowInject = FALSE;
}

CRS232Noise::~CRS232Noise()
{

}

void CRS232Noise::Inject()
{
   m_nowInject = TRUE;
}

// --------------------------------- SetErrorTypes -----------------------
void CRS232Noise::SetErrorTypes(CRS232Noise *pOther)
{
   if (!pOther)
      return;
   m_enableFaults     = pOther->m_enableFaults;
   m_insertCharacters = pOther->m_insertCharacters;
   m_removeCharacters = pOther->m_removeCharacters;
   m_corruptFraming   = pOther->m_corruptFraming;
   m_modifyCharacters = pOther->m_modifyCharacters;
   m_errorFrequency   = pOther->m_errorFrequency;
   m_ignoreReq        = pOther->m_ignoreReq;
   m_delaysPeriod     = pOther->m_delaysPeriod;
   m_parityFaults     = pOther->m_parityFaults;

   //
   m_errorsInjected = 0;
   m_beep = pOther->m_beep;

   // disable this feature in v7.7 untill I get a chance to test it
   m_enableFaults = FALSE;

}

// --------------------------------- SetErrorTypes -----------------------
void CRS232Noise::SetErrorTypes(BOOL enable,
                                LONG freq,
                               LONG period,
                               BOOL inserts , 
                               BOOL removes , 
                               BOOL misFrames , // off by default
                               BOOL corrupts,
                               BOOL ignore,
                               BOOL parityFaults,
                               BOOL beep
                               )
{
   m_enableFaults     = enable;
   m_insertCharacters = inserts;
   m_removeCharacters = removes;
   m_corruptFraming   = misFrames;
   m_modifyCharacters = corrupts;
   m_errorFrequency   = freq;
   m_ignoreReq        = ignore;
   m_delaysPeriod     = period;
   m_parityFaults     = parityFaults;

   //
   m_errorsInjected = 0;
   m_beep = beep;

   // disable this feature in v7.7 untill I get a chance to test it
   m_enableFaults = FALSE;

} // SetErrorTypes

// ------------------------------- GetErrorTypes ----------------------
void CRS232Noise::GetErrorTypes(BOOL *enable,
                               int *freq,
                               int *period,    // delays by (0=disable)
                               BOOL *inserts, 
                               BOOL *removes, 
                               BOOL *misFrames,
                               BOOL *corrupts,
                               BOOL *ignore,
                               BOOL *parity,
                               BOOL *beep
                               )
{
   *enable  =   m_enableFaults;
   *inserts =   m_insertCharacters; 
   *removes =   m_removeCharacters; 
   *misFrames = m_corruptFraming;   
   *corrupts =  m_modifyCharacters; 
   *freq =      m_errorFrequency;   
   *ignore =    m_ignoreReq;        
   *period =    m_delaysPeriod;
   *parity =    m_parityFaults;
   //                 
   *beep =      m_beep;             
} // GetErrorTypes

// ---------------------------------------- InjectErrors ----------------------
LONG CRS232Noise::InjectErrors(CRS232Port *pPort, 
                  const BYTE *transmitBuffer, 
                  int writeLength, 
                  char*pDebugStr)
{
BYTE *pBuffer, *pBufferPtr;
int bufferIndex;        // index into the un-corrupted data
int randN, faultType, faultPercent;   // random values
int len;                   // length of data to send in a burst.
LONG retValue;
CString ErrorDescription;
BYTE chErr;


   if (!m_enableFaults)
   {
      //call the 232 port method since there are not supposed to be any errors
      return (pPort->Send(writeLength, transmitBuffer, pDebugStr));
   }

   m_pPort = pPort;
   // new some data for the buffer before we corrupt it
   pBuffer = (BYTE*)new char[writeLength+1]; //allocate 1 byte extra to allow for a "INSERT" corruption character
   pBufferPtr = pBuffer;
   bufferIndex = 0;
   while (bufferIndex < writeLength)
   {
      // work out a random #, this must be done on a curve : W. Wahli AG, P. Christen
      randN = rand();
      faultPercent = (int)(randN*200/(float)RAND_MAX);
      if (m_nowInject)
         faultPercent = 0;
      m_nowInject = FALSE;
      
      // IGNORE (only valid on the 1st byte of the message)
      if ((0==bufferIndex)&&(faultPercent < m_errorFrequency)&&(m_ignoreReq))
      {
         ReportError("Ignore request");
         break; // send 0 bytes out
      }

      if (faultPercent*8 < m_errorFrequency)   // only corrupt about every 8th byte
      {
         // simulate a fault
         randN = rand();
         faultType = (int)(randN/(float)RAND_MAX*6);  // fault types
         chErr = (BYTE)(rand());
         switch(faultType)
         {
            case 0:// INSERT
               if (m_insertCharacters)
               {
                  *pBufferPtr++ = chErr;  // random character
                  *pBufferPtr++ = transmitBuffer[bufferIndex++];
                  ErrorDescription.Format("Inserted character x%02X", chErr);
                  ReportError(ErrorDescription);
               }
               break;
            case 1:// DELETE
               if (m_removeCharacters)
               {
                  ErrorDescription.Format("Skipped character x%02X", transmitBuffer[bufferIndex]);
                  ReportError(ErrorDescription);
                  bufferIndex++; // do nothing, skips this byte
               }
               break;
            case 2:// CORRUPT
               if (m_modifyCharacters)
               {
                  *pBufferPtr++ = chErr;  // random character
                  bufferIndex++; //skip the character
                  ErrorDescription.Format("Corrupted to character x%02X", chErr);
                  ReportError(ErrorDescription);
               }
               break;
            case 3:// MISSFRAME
               if ((m_corruptFraming)&&((bufferIndex+1)!=writeLength))
               {
                  *pBufferPtr++ = transmitBuffer[bufferIndex++];
                  // character added, now nuke the line.
                  NukePort();
                  ErrorDescription.Format("Generate framing error");
                  ReportError(ErrorDescription);
               }
               break;
            case 4:// DELAY/SLOW
               if (m_delaysPeriod)
               {
                  len = (int)((LONG)pBufferPtr - (LONG)pBuffer);
                  if (len>0)
                     retValue = pPort->Send(len, pBuffer, pDebugStr);
                  //delay and then carry on
                  ErrorDescription.Format("Slow-down");
                  ReportError(ErrorDescription);
                  Sleep(m_delaysPeriod);
                  pBufferPtr = pBuffer;
                  *pBufferPtr++ = transmitBuffer[bufferIndex++];
               }
               break;
            case 5:// PARITY
               if ((m_parityFaults)&&((bufferIndex+1)!=writeLength))
               {
                  *pBufferPtr++ = transmitBuffer[bufferIndex++];
                  // character added, now nuke the line.
                  NukePort();
                  ErrorDescription.Format("Generate parity error");
                  ReportError(ErrorDescription);
               }
               break;
         }
         // send the message thus far
         //...
         len = (int)((LONG)pBufferPtr - (LONG)pBuffer);
         if (len)
            retValue = pPort->Send(len, pBuffer, pDebugStr);

         pBufferPtr = pBuffer; // reset our pointer to the beginning of the rest of this message
      }
      else
      {
         // append to the send buffer
         *pBufferPtr++ = transmitBuffer[bufferIndex++];
         // this way we do not have to TX each byte seperately
      }
   }

   // un-nuke the port if it was
   //...
   UnNukePort();
   
   // send the remaining bytes of the message normally.
   len = (int)((LONG)pBufferPtr - (LONG)pBuffer);
   if (len)
      retValue = pPort->Send(len, pBuffer, pDebugStr);
   else 
      retValue = FALSE;
   // clean up
   delete pBuffer;
   return (retValue);
} // InjectErrors

// -------------------------------- ReportError --------------------------
void CRS232Noise::ReportError(LPCTSTR descr)
{
CString debuggerText;
   // inc error counter
   m_errorsInjected++;

#ifdef _COMMS_DEBUGGING
   OutputDebugString(descr);
   OutputDebugString(".\r\n");
#endif
   
   debuggerText.Format("[%d] SIMERROR :: %s", GetCurrentThreadId(), descr);
   pGlobalDialog->AddCommsDebugString(debuggerText);

   // play a sound
   if (m_beep)
   {
      Beep(4000, 100);
   }
} // ReportError

// ------------------------------- NukePort ---------------------------
// configure the port for a different baud so that it transmits garbage
//
BOOL CRS232Noise::NukePort()
{
BOOL error;
DCB  dcb;

   if (m_nukedPort)
      return (TRUE);

   error = GetCommState(m_pPort->h232Port,
                           &dcb);
//   m_oldBaud = dcb.BaudRate;
   // corrupt this one
   switch (dcb.Parity)
   {
      case ODDPARITY:
         dcb.Parity = EVENPARITY;
         break;
      case EVENPARITY:
         dcb.Parity = ODDPARITY;
         break;
      case MARKPARITY:
         dcb.Parity = SPACEPARITY;
         break;
      case SPACEPARITY:
         dcb.Parity = MARKPARITY;
         break;
      default:
         dcb.Parity = NOPARITY;
         break;
   }   
   SetCommState(m_pPort->h232Port, &dcb);
   m_nukedPort = TRUE;
   return (TRUE);
} // NukePort

// ------------------------------- NukeParity ---------------------------
// configure the port for a different baud so that it transmits garbage
//
BOOL CRS232Noise::NukeParity()
{
BOOL error;
DCB  dcb;

   if (m_nukedPort)
      return (TRUE);

   error = GetCommState(m_pPort->h232Port,
                           &dcb);
//   m_oldBaud = dcb.BaudRate;
   // corrupt this one
   if (dcb.BaudRate > CBR_19200)
      dcb.BaudRate = CBR_1200;
   else
      dcb.BaudRate = CBR_57600;
   
   SetCommState(m_pPort->h232Port, &dcb);
   m_nukedPort = TRUE;
   return (TRUE);
} // NukeParity

// ----------------------------------- UnNukePort --------------------
BOOL CRS232Noise::UnNukePort()
{
DWORD error;
//DCB dcb;

   if (m_nukedPort)
   {
      //restore the settings
      //error = GetCommState(m_pPort->h232Port,
      //                        &dcb);
      //dcb.BaudRate = m_oldBaud;
      
      //SetCommState(m_pPort->h232Port, &dcb);
      m_pPort->ClosePort();
      m_pPort->OpenPort(m_pPort->portNameS);

      m_pPort->ReConfigurePort();
      m_pPort->Purge();
      ClearCommError(m_pPort->h232Port, &error, NULL);

#ifdef _COMMS_DEBUGGING
      OutputDebugString("Port restored.\n");
#endif
   }
   m_nukedPort = FALSE;
   return (TRUE);
} // UnNukePort


CEthernetNoise::CEthernetNoise() : CRS232Noise()
{
   // nothing to do
}

// -------------------------------- InjectErrors -----------------------------
// TX function, it will randomly insert a error only if errors are enabled
LONG CEthernetNoise::InjectErrors(CDDKSrvSocket *pSock, 
                     SOCKET     openSock,
                     const BYTE *transmitBuffer, 
                     int writeLength,
                     char *pDebugStr)
{
BOOL sendResponse = TRUE;
int randN, faultType, faultPercent;   // random values
CString ErrorDescription;

   if (!m_enableFaults)
   {
      //call the 232 port method since there are not supposed to be any errors
      return (pSock->Send(openSock, writeLength, (char*)transmitBuffer, pDebugStr));
   }

   // work out a random #, this must be done on a curve : W. Wahli AG, P. Christen
   randN = rand();
   faultPercent = (int)(randN*200.0/(float)RAND_MAX);
   if (m_nowInject)
      faultPercent = 0;
   m_nowInject = FALSE;

   if (faultPercent < m_errorFrequency)
   {
      faultType = (int)(randN*2/(float)RAND_MAX);
      switch (faultType)
      {
      case 0: // IGNORE
         if (m_ignoreReq)
         {
            ReportError("Ignore request");
            sendResponse = FALSE;
         }
         break;
      case 1 :
         if (m_delaysPeriod)
         {
            ErrorDescription.Format("(%d)Delayed response %d (ms)", (int)*m_pSocketObj->m_pSocket, m_delaysPeriod);
            ReportError(ErrorDescription);
            Sleep(m_delaysPeriod);
            // still send the message!
         }
         break;
      default:
         break;
      }
   }
   if (sendResponse)
      return (pSock->Send(openSock, writeLength, (char*)transmitBuffer, pDebugStr));
   // ...
   return(FALSE); // error
} // InjectErrors
