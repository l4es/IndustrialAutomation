/////////////////////////////////////////////////////////////////////////////
//
// FILE: ServerRS232Array.cpp : implementation file
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
// Implementation of the CServerRS232Array class.
//    This class does almost nothing, because we only ever open 1 port.
//    If in future multiple ports are allowed, this class will need 
//    minimal changes.
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mod_RSsim.h"
#include "ServerRS232Array.h"
#include "MOD_RSsimDlg.h"
#include "abcommsprocessor.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CMOD_simDlg * pGlobalDialog;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CServerRS232Array::CServerRS232Array(int size /*= 10*/)
{
   SetSize(size);
}

CServerRS232Array::~CServerRS232Array()
{
// free up all of the items pointed to before this object looses the pointers
CMOD232CommsProcessor * sockPtr;  
LONG n=GetUpperBound(),i=0;

   // go backwards through the list so that the objects
   // are destroyed in reverse order of creation.
   while (i<=n)
   {
      sockPtr = (CMOD232CommsProcessor *)GetAt(i);  // Make sure to delete all items this array holds 
      SetAt((DWORD)i, NULL);
      if (NULL != sockPtr)
      {
      LONG i=100;
         sockPtr->keepPolling = FALSE;
         OutputDebugString("Stop comms Processor!\n");
         // To-do!!! Must get beter clean-up in here, will 
         // have to wait for thread to finnish running before continuing
         while (i)
         {
            // process our message queue in the meantime while we wait for the thread to die
            pGlobalDialog->MyProcessingLoop(20);
            if (WAIT_OBJECT_0 ==WaitForSingleObject(sockPtr->m_threadDeadEvent, 50))
               break;
            i--;
         }
         // the thread will be dead by now, if it isn't well...
         delete sockPtr;
      }
      i++;
   }
}


// ------------------------- CloseAll -----------------------------------------
// close them all
void CServerRS232Array::CloseAll()
{

CRS232Port *pPort,*pPort1;
LONG n=GetUpperBound(); 

   // go backwards through the list so that the objects
   // are destroyed in reverse order of creation.
   pPort1 = (CRS232Port *)GetAt(0);  
   while (n>=0)
   {
      pPort = (CRS232Port *)GetAt(n);  // Make sure to close all items this array holds 
      pPort->keepPolling = FALSE;
      if (NULL != pPort)
         pPort->ClosePort();
      n--;                             
   }
} // CloseAll

// ------------------------- AddServers ------------------------------------
// Start the only port server.
LONG CServerRS232Array::AddServers(DWORD protocolSel,
                                   LPCTSTR portNameShort, 
                                   DWORD  baud, 
                                   DWORD byteSize, 
                                   DWORD parity, 
                                   DWORD stopBits,
                                   DWORD rts,
                                   int responseDelay, 
                                   BOOL MOSCADchecks,
                                   BOOL modifyThenRespond,
                                   BOOL disableWrites,
                                   DWORD PDUSize,
								           BOOL  useBCC,
                                   DWORD source,
                                   DWORD dest,
                                   DWORD files,
                                   BOOL  run,
                                   BOOL JOYread,
                                   BOOL JOYwrite,
                                   DWORD masterIdleTime,
                                   DWORD timeoutValue
                                   )
{
CMOD232CommsProcessor *pServerMOD;
CAB232CommsProcessor *pServerAB;
   
   RemoveAll();
   SetSize(0, 1); //numServers);
   // 1st server is the creator 
   switch (protocolSel)
   {
   case PROTOCOL_SELMOD232 :
      pServerMOD = (CMOD232CommsProcessor *) new CMOD232CommsProcessor(portNameShort, 
                                                             baud, 
                                                             byteSize, 
                                                             parity, 
                                                             stopBits,
                                                             rts,
                                                             responseDelay,
                                                             MOSCADchecks, 
                                                             modifyThenRespond,
                                                             disableWrites);
      pServerMOD->SetPDUSize(PDUSize);
      //pServer1->m_responseDelay = responseDelay;
      pServerMOD->m_threadStartupEvent.SetEvent();// CEvent
      //pServer = pServer1;
   
      // add it to our array
      Add((CObject*)pServerMOD);
      break;
   case PROTOCOL_SELABMASTER232 :
      pServerAB = (CAB232CommsProcessor *) new CAB232CommsProcessor(portNameShort, 
                                                             baud, 
                                                             byteSize, 
                                                             parity, 
                                                             stopBits,
                                                             rts,
                                                             responseDelay,
                                                             MOSCADchecks, 
                                                             modifyThenRespond,
                                                             disableWrites,
															                useBCC,
                                                             masterIdleTime,
                                                             timeoutValue
                                                             );
      pServerAB->SetPDUSize(PDUSize);
      pServerAB->SetJoyParameters(source, dest, files, run, JOYread, JOYwrite);

      //pServer1->m_responseDelay = responseDelay;
      pServerAB->m_threadStartupEvent.SetEvent();// CEvent
      //pServer = pServer1;
   
      // add it to our array
      Add((CObject*)pServerAB);
      break;
   case PROTOCOL_SELAB232 :
      pServerAB = (CAB232CommsProcessor *) new CAB232CommsProcessor(portNameShort, 
                                                             baud, 
                                                             byteSize, 
                                                             parity, 
                                                             stopBits,
                                                             rts,
                                                             responseDelay,
                                                             MOSCADchecks, 
                                                             modifyThenRespond,
                                                             disableWrites,
                															 useBCC,
                                                             masterIdleTime,
                                                             timeoutValue
                                                             );
      pServerAB->SetPDUSize(PDUSize);
      //pServer1->m_responseDelay = responseDelay;
      pServerAB->m_threadStartupEvent.SetEvent();// CEvent
      //pServer = pServer1;
   
      // add it to our array
      Add((CObject*)pServerAB);
      break;
   }
   // do some idle processing to allow the window to repaint
   pGlobalDialog->MyProcessingLoop(20);
   return (1);
} // AddServers


// -------------------------------- NumberConnected ---------------------------
// return # of sockets that are still connected to something
LONG CServerRS232Array::NumberConnected()
{
CRS232Port *pPort;
LONG n=GetUpperBound(); 
LONG ret = 0;

   // go backwards through the list so that the objects
   // are destroyed in reverse order of creation.
   while (n>=0)
   {
      pPort = (CRS232Port *)GetAt(n);  // Make sure to delete all items this array holds 
      if (NULL != pPort)
         if (INVALID_HANDLE_VALUE != pPort->h232Port)
            ret++;
      n--;                             
   }
   return(ret);
} // NumberConnected

