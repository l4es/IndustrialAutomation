// ServerSocketArray.cpp: implementation of the CServerSocketArray class.
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
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mod_rssim.h"
#include "ServerSocketArray.h"
#include "MOD_rssimDlg.h"

//#include "modcommsprocessor.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CMOD_simDlg * pGlobalDialog;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CServerSocketArray::CServerSocketArray(int size /*= 10*/)
{
   SetSize(size);
   kill = FALSE;
   initiating = FALSE;
}

CServerSocketArray::~CServerSocketArray()
{
// free up all of the items pointed to before this object looses the pointers
CMODEthCommsProcessor * sockPtr;  
LONG n=GetUpperBound(),i=0;

   // go backwards through the list so that the objects
   // are destroyed in reverse order of creation.
   while (i<=n)
   {
      sockPtr = (CMODEthCommsProcessor *)GetAt(i);  // Make sure to delete all items this array holds 
      SetAt((DWORD)i, NULL);
      if (NULL != sockPtr)
         delete sockPtr;
      i++;
   }
}


// ------------------------- CloseAll -----------------------------------------
// close them all
void CServerSocketArray::CloseAll()
{
CDDKSrvSocket * sockPtr, *sockPtr1;
LONG n=GetUpperBound(); 

   // go backwards through the list so that the objects
   // are destroyed in reverse order of creation.
   sockPtr1 = (CDDKSrvSocket *)GetAt(0);  // Make sure to delete all items this array holds 
   while (n>=0)
   {
      sockPtr = (CDDKSrvSocket *)GetAt(n);  // Make sure to delete all items this array holds 
      //Remove((HANDLE)n);               // remove from the list
      if ((NULL != sockPtr) && (NULL != sockPtr1->m_pSocket))
         sockPtr->CloseSocket(TRUE, *sockPtr1->m_pSocket);
      n--;                             
   }
} // CloseAll

// ------------------------- AddServers ------------------------------------
// Start all the servers you want
LONG CServerSocketArray::AddServers(int numServers, 
                                    int port, 
                                    int responseDelay, 
                                    BOOL linger,
                                    BOOL MOSCADchecks,
                                    BOOL modifyThenRespond,
                                    BOOL disableWrites,
                                    DWORD PDUSize)
{
int index;
CMODEthCommsProcessor *pServer1;
CMODEthCommsProcessor *pServer;

   initiating = TRUE;   
   RemoveAll();
   SetSize(0,numServers);
   for (index = 0; index < numServers; index++)
   {
   CRS232Noise *pNoise = pGlobalDialog->m_pNoiseSettings;
      if (kill)
         return(index);
      if (0==index)
      { // 1st server is the creator of the listen socket
         pServer1 = (CMODEthCommsProcessor *) new CMODEthCommsProcessor(responseDelay,
                                                                       MOSCADchecks,
                                                                       modifyThenRespond,
                                                                       disableWrites,
                                                                       PDUSize,
                                                                       port);

         pServer1->m_linger = linger;
         pServer1->m_threadStartupEvent.SetEvent();// CEvent
         pServer1->m_NoiseSimulator.SetErrorTypes(pNoise);
         pServer = pServer1;
      }
      else
      {
         pServer = (CMODEthCommsProcessor *) new CMODEthCommsProcessor(responseDelay,
                                                                    MOSCADchecks,
                                                                    modifyThenRespond,
                                                                    disableWrites,
                                                                    PDUSize,
                                                                    &pServer1->m_socket);
         //pServer->m_responseDelay = responseDelay;
         pServer->m_linger = linger;
         pServer->m_threadStartupEvent.SetEvent();// CEvent
         pServer->m_NoiseSimulator.SetErrorTypes(pNoise);
      }
      // add it to our array
      //SetAt(index, (CObject*)pServer);
      Add((CObject*)pServer);
      
      if (kill)
         return(index);
      // do some idle processing to allow the window to repaint
      pGlobalDialog->MyProcessingLoop(20);
   }
   initiating = FALSE;
   return (index);
} // AddServers


// -------------------------------- NumberConnected ---------------------------
// return # of sockets that are still connected to something
LONG CServerSocketArray::NumberConnected()
{
CDDKSrvSocket * sockPtr;  
LONG n=GetUpperBound(); 
LONG ret = 0;

   // go backwards through the list so that the objects
   // are destroyed in reverse order of creation.
   while (n>=0)
   {
      sockPtr = (CDDKSrvSocket *)GetAt(n);  // Make sure to delete all items this array holds 
      if (NULL != sockPtr)
         if (sockPtr->accepted)
            ret++;
      n--;                             
   }
   return(ret);
} // NumberConnected

// -------------------------------- FirstConnected ---------------------------
// return # of sockets that are still connected to something
SOCKET CServerSocketArray::FirstConnected()
{
CDDKSrvSocket * sockPtr;  
LONG n=GetUpperBound(); 
LONG ret = 0;

   // go backwards through the list so that the objects
   // are destroyed in reverse order of creation.
   while (n>=0)
   {
      sockPtr = (CDDKSrvSocket *)GetAt(n);  // Make sure to delete all items this array holds 
      if (NULL != sockPtr)
         if (sockPtr->accepted)
            return(sockPtr->AcceptedAsyncSocket);
      n--;                             
   }
   return(INVALID_SOCKET);
} // FirstConnected