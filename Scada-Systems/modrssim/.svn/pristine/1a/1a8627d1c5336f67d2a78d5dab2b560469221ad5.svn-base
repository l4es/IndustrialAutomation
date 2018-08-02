// DDKSocket.cpp: implementation of the CDDKSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


// WSA initalized
BOOLEAN				CDDKSocket::m_wsaInitialized = FALSE;
// WSA interlocking
CRITICAL_SECTION    CDDKSocket::m_wsaCS;
CRITICAL_SECTION *	CDDKSocket::m_pwsaCS = NULL;

IMPLEMENT_DYNAMIC( CDDKSocket, CObject );
//IMPLEMENT_DYNCREATE( CDDKSocket, CObject );

CDDKSocket::CDDKSocket()
{
   m_socket = INVALID_SOCKET;
   m_serverObject = FALSE;
   // do WSA start-up
   Initialize();

   { // Create a Client socket

   }
}


CDDKSocket::CDDKSocket(DWORD timeout)
{
   m_socket = INVALID_SOCKET;
   m_serverObject = FALSE;
   // do WSA start-up
   Initialize();

   { // Create a Client socket

   }
}

// -------------------------------- CloseSocket -------------------------------
//  Close a socket
BOOL CDDKSocket::CloseSocket(BOOL gracefully /*= FALSE */, SOCKET sock/* = NULL*/)
{
SOCKET lSock;
linger lingerOpt;
int sockError=0;
   
   if (NULL == sock)
      lSock = *m_pSocket;
   else
      lSock = sock;

   if ((gracefully)&&(NULL != lSock))
   {
      // set up LINGER to do a gracefull close.
      lingerOpt.l_linger = SO_LINGER;
      lingerOpt.l_onoff = TRUE;
      sockError = setsockopt(lSock, SOL_SOCKET, SO_LINGER, (CHAR*)&lingerOpt, sizeof(lingerOpt));

      sockError = shutdown(lSock, SD_SEND ); // disable any more sends on the shut
      if (sockError)
         sockError = wsaerrno;
      if (WSAENOTSOCK == sockError+WSABASEERR)
         lSock = NULL;
      //
   }
   if (NULL != lSock)
      sockError = closesocket(lSock);
   return (TRUE);
} // CloseSocket

// -------------------------------- Initialize -------------------------------
void CDDKSocket::Initialize()
{
int errCode;
	
	// Init the WSA only once
	EnterWSA();
	if (!m_wsaInitialized)
	{
		m_wVersionRequested = MAKEWORD( 1, 1 );
		errCode = WSAStartup( m_wVersionRequested, &(m_WSAData) );
      if ( errCode != 0 )
      {
         // ? Tell the user that we could not find a usable WinSock DLL.
         OutputDebugString("Unable to find a useable WinSock DLL"); 
      }
	}
	LeaveWSA();

   // init all internal variables
	m_CallBackFuncPtr = NULL; // set the CB pointer to null
	m_listenThreadStatus  = SOCKET_EX_TERMINATED;  
	m_listenThreadCreated = FALSE;
	m_pWorkerThread       = NULL;
   memset((void * ) &(m_destSockaddr_in), 0, sizeof (m_destSockaddr_in));
   memset((void * ) &(m_localSockaddr_in), 0, sizeof (m_localSockaddr_in));
   m_socket = INVALID_SOCKET;
   m_serverBufferSize = 2048;
   m_buffer = NULL;
}


// ------------------------------- ~CDDKSocket --------------------------
CDDKSocket::~CDDKSocket()
{
   //OutputDebugString("In Base socket destructor\n");
   if (NULL!=m_socket)
   {
      OutputDebugString("Base: Client socket closing\n");
      closesocket(m_socket); //kill accepted instance immediately
   }

//	EnterWSA();
//	if (m_wsaInitialized)
//	{
//		WSACleanup();
//	}
//	LeaveWSA();
   OutputDebugString("[DEBUGSTEP]\n");
   if (NULL != m_buffer)
      delete m_buffer;
   //OutputDebugString("Leaving Base socket destructor\n");

}

#ifdef _DEBUG
VOID CDDKSocket::Dump(CDumpContext& dc) const
{
   // call the base class first
   CObject::Dump(dc);

   // dump our object to the debuggers output
   // all important members can be dumped at this stage.
   dc << "Socket: " << "\n";
} // Dump
#endif // _DEBUG


// ------------------------- SockStateChanged ----------------------------
void CDDKSocket::SockStateChanged(DWORD state)
{
   // do nothing in the base class
} // SockStateChanged

void CDDKSocket::SockDataMessage(LPCTSTR msg)
{

}

// ------------------------- SockDataDebugger ----------------------------
void CDDKSocket::SockDataDebugger(const CHAR * buffer, LONG length, dataDebugAttrib att)
{
CString prefix, ASCIIdata;
LONG index, i;

   if (dataDebugTransmit == att)
      prefix.Format("(%d)TX:", (int)*m_pSocket);
   else
      if (dataDebugReceive == att)
         prefix.Format("(%d)RX:", (int)*m_pSocket);
   index = 0;
   while (index < length)
   {
      i=0;
      OutputDebugString("\n");
      OutputDebugString(prefix);
      while ((index+i < length)&&(i<8))
      {
         ASCIIdata.Format("%02X", buffer[index+i]);
         OutputDebugString(ASCIIdata);
         i++;
      }
      index +=8;
   }
   if (dataDebugOther == att)
      OutputDebugString("]");
} // SockDataDebugger

// --------------------------------- GetSockError -------------------------
void CDDKSocket::GetSockError(CHAR * errString, BOOL reset/*=true*/)
{
LONG wx = wsaerrno;

   sprintf(errString,sys_wsaerrlist[wx]);
    if (reset) WSASetLastError(0);
} // GetSockError

// -------------------------------- EnterWSA -------------------------------
void CDDKSocket::EnterWSA()
{
   // If the critical section has not been instantiated,
   // instantiate and initialize and enter
   if( NULL != m_pwsaCS)
      EnterCriticalSection(m_pwsaCS);
   else
   {
      m_pwsaCS = &m_wsaCS;
      InitializeCriticalSection(m_pwsaCS);
      EnterCriticalSection(m_pwsaCS);
   }
} // EnterWSA


// -------------------------------- LeaveWSA -------------------------------
void CDDKSocket::LeaveWSA()
{
	LeaveCriticalSection(m_pwsaCS);
}



// --------------------------- Recieve -------------------------------
// RETURNS : -1 if error, or the number actually read from socket
LONG CDDKSocket::Recieve(SOCKET ackSock, //The accepted socket
                           int    numberOfBytesToRead,// -> number of bytes to read.
                           CHAR * BufferPtr,       //<- Data read from socket    
                           CHAR * debugStrPtr       //Any errors 
                          )
{
int numread;
int temp;


   
   //call recv func
   int check1 = 0x1234;
   numread =  recv(ackSock,                    // Our precious socket
                   BufferPtr,//buffPtr,                  // RxBuffer 
                   (INT) numberOfBytesToRead,
                   (INT) NULL                  // no fancy flaggies
                  ); 
   int check2 = 0x5678;

   if (numread != 0xffffffff)
      SockDataDebugger(BufferPtr, numread, dataDebugReceive);// a little debugger msg

   //on sock_error
   if (SOCKET_ERROR == numread) 
   {
      m_socketStatus = SOCKET_UNHEALTHY;
      temp = wsaerrno;
      (temp >= 0) ? (sprintf(debugStrPtr, "Read Error on Listen socket : %s\n",sys_wsaerrlist[temp]))
                     : (sprintf(debugStrPtr, "Unknown Read error on Listen socket\n"));
      WSASetLastError(0);
      OutputDebugString(debugStrPtr);
      CloseSocket(TRUE, ackSock);
      return(-1);
   }
   //on mismatch
   if (numread != numberOfBytesToRead)
   {
       m_socketStatus = SOCKET_UNHEALTHY;
       sprintf(debugStrPtr, "Read Timeout (%ld/%ld) ", numread,numberOfBytesToRead);
       OutputDebugString(debugStrPtr);
       return(numread);
       //I expect the caller to initiatate retries or whatever!
   }
   else
      m_socketStatus = SOCKET_HEALTHY;
   return(numread);
}

// --------------------------- Send -------------------------------
LONG CDDKSocket::Send(SOCKET ackSock, //The accepted socket
                           int    numberOfBytesToWrite,// -> number of bytes to write
                           CHAR * BufferPtr,       //<- Data read from socket    
                           CHAR * debugStrPtr       //Any errors 
                          )
{
//returns -1 if error, or the number actualy read from socket
int temp;
int numberOfBytesWritten; 

   numberOfBytesWritten = send(ackSock,
                               BufferPtr,
                               (INT) numberOfBytesToWrite,
                               (INT) NULL
                              );
   if (SOCKET_ERROR == numberOfBytesWritten) 
   {
      // We have a socket transmit error, so time to start counting down to a socket reset
      numberOfBytesWritten = 0;
      m_socketStatus = SOCKET_UNHEALTHY;
      temp = wsaerrno;
      (temp >= 0) ? (sprintf(debugStrPtr, "Write Error on Listen socket : %s\n",sys_wsaerrlist[temp]))
                     : (sprintf(debugStrPtr, "Unknown Write error on Listen socket\n"));
      WSASetLastError(0);
      OutputDebugString(debugStrPtr);
      SockDataMessage(debugStrPtr);
   }


   // a leettel debugger msg
   SockDataDebugger(BufferPtr, numberOfBytesWritten, dataDebugTransmit);
   // on mismatch
   if (numberOfBytesWritten != numberOfBytesToWrite)
   {
       m_socketStatus = SOCKET_UNHEALTHY;
       sprintf(debugStrPtr, "Write Timeout (%ld/%ld) ", numberOfBytesToWrite,numberOfBytesToWrite);
       //OutputDebugString(debugStrPtr);
       //I expect the caller to initiatate retries or whatever!
   }
   else
      m_socketStatus = SOCKET_HEALTHY;
   return(numberOfBytesWritten);
} // Send
