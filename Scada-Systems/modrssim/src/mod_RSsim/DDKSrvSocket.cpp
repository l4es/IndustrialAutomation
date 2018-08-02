// DDKSrvSocket.cpp: implementation of the CDDKSrvSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// # of times a connected server socket will loop for without getting any RX data 
// before it closes and gets ready to accept new connections again.
#define MAX_IDLE_LOOPS   10


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC( CDDKSrvSocket, CDDKSocket);

// This server socket object will construct a listen socket of accept a pointer to an existing socket.
CDDKSrvSocket::CDDKSrvSocket(unsigned short port,unsigned long IPAddrULONG /*= ADDR_ANY*/, SOCKET * pServerSocket /*=NULL*/) : CDDKSocket()
{
int error;
CHAR errStr[180],debugStr[180];
INT      sockoptEnable = TRUE;   

   m_serverObject = TRUE;
   m_pSocket      = NULL;

   int check1 = 0x1234;
   m_buffer = new char[m_serverBufferSize];
   int check2 = 0x1234;

   // server stuff
   // Create a Thread and then a Server-end socket to listen on later

   // create the listening thread
   m_listenThreadStatus = SOCKET_EX_PENDING;
   m_pWorkerThread      = AfxBeginThread((AFX_THREADPROC)SockAsyncFriend,
                                     this,
                                     THREAD_PRIORITY_TIME_CRITICAL, 0,
                                     CREATE_SUSPENDED
                                    );
   // construct with IP and port or with an existing socket
   if (NULL == pServerSocket)
   {

      // Setup local addressing for listen socket
      m_localSockaddr_in.sin_family           = PF_INET;
      m_localSockaddr_in.sin_addr.S_un.S_addr = INADDR_ANY; //usually default
      if ((m_localSockaddr_in.sin_port = htons((u_short) port)) == 0)
      {
         sprintf(debugStr, "Cannot use port %ld", port);
         OutputDebugString(debugStr);

         // Fail the connection process
         m_socketStatus = SOCKET_UNCONFIGURED;
         return;
      }

      // Map protocol name to protocol number
      if ((m_ppe = getprotobyname("tcp")) == NULL)
      {
         GetSockError(errStr);
         sprintf(debugStr,"Driver cannot connect for listen :%s",errStr);
         OutputDebugString(debugStr);
         // Fail the connection process
         m_socketStatus = SOCKET_UNCONFIGURED;
         return;
      }

      // Allocate a listen socket
      m_socket = socket(PF_INET, SOCK_STREAM, m_ppe->p_proto); 
      // If we could not allocate a socket, we must fail the connection process
      if (INVALID_SOCKET == m_socket)  // recommended NT error check
      {
         GetSockError(errStr);
         sprintf(debugStr, "Cannot create Listen socket :%s",errStr);
         OutputDebugString(debugStr);
          // Fail the connection process
         m_socketStatus = SOCKET_UNCONFIGURED;
         return;
      }
      // now to bind socket to local address+port
      if (0 != bind(m_socket, (sockaddr *)&(m_localSockaddr_in), sizeof(m_localSockaddr_in) ) )
      {
         GetSockError(errStr);
         sprintf(debugStr, "Cannot Bind to Listen socket :%s",errStr);
         OutputDebugString(debugStr);
         // Fail the connection process
         m_socketStatus = SOCKET_UNCONFIGURED;
         return;
      }
      // Set the socket to not delay any sends 
      error = setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, (CHAR FAR * ) &m_sockoptEnable, sizeof (INT));
      // If we could not set the socket parameters, we must fail the connection process
      if (error == (LONG) SOCKET_ERROR)
      {
         int lastError = WSAGetLastError();
         sprintf(debugStr, "Cannot setsockopt error (%ld)", lastError);
         OutputDebugString(debugStr);
         // Fail the connection process
         m_socketStatus = SOCKET_UNCONFIGURED;
         return;
      }
      m_pSocket = &m_socket;
      m_socketStatus = SOCKET_INITIALISED;
   }
   else
   {
      // use a socket provided to us.
      // do this when many threads listen and accept connections on the same socket.
      m_pSocket = pServerSocket;
   }
   m_socketStatus = SOCKET_INITIALISED;

   strcpy(debugStr, "Socket created OK.");
   
   SockDataMessage(debugStr);

   m_listenThreadStatus = SOCKET_EX_RUNNING;
}

CDDKSrvSocket::~CDDKSrvSocket()
{
   // get the thread to die off
   //delete(m_buffer);
   //OutputDebugString("In Server socket destructor\n");
   m_listenThreadStatus = SOCKET_EX_TERMINATE;
   
   // abort any listen in progress, this only executes on 1st comms thread's socket, since it created it
   if (INVALID_SOCKET != m_socket)
   {
      closesocket(m_socket);
      m_socket = INVALID_SOCKET;
   }
   // wait for thread to die off
   CSingleLock lk(&m_threadDeadEvent);
   lk.Lock(5000); // wait max 5 seconds

   if (NULL != AcceptedAsyncSocket)
   {
      closesocket(AcceptedAsyncSocket); //kill accepted instance immediately
      accepted = FALSE;
      OutputDebugString("Server socket closing normally.\n");
      SockStateChanged(SOCKETCURRENTLY_CLOSING);
   }
}

#ifdef _DEBUG
VOID CDDKSrvSocket::Dump(CDumpContext& dc) const
{
   // call the base class first
   CDDKSocket::Dump(dc);

   // dump our object to the debuggers output
   // all important members can be dumped at this stage.
   dc << "Server socket: " << "\n";
} // Dump
#endif // _DEBUG


// ------------------------------ SockSockAsyncFriend ------------------------------
UINT SockAsyncFriend(LPVOID pParam)
{
CHAR     debugStr2[MAX_DEBUG_STR_LEN];
CHAR     debugStr[MAX_DEBUG_STR_LEN];
CDDKSrvSocket* DDKSockPtr;

   Sleep(500);
   // OK now since everyone will be asking me what is this sleep doing here. So I am
   // going to tell U. We are being naughty, this thread actually starts executing 
   // while the parent class is constructing, consequently all calls to virtual functions
   // are made before the parent has initialized completely (constructor body has not yet run)
   // and U ned up using un-initialized variables EEK.

   DDKSockPtr = (CDDKSrvSocket*)pParam;
   try 
   { 
		
      // call the function the thread will run in
      if (DDKSockPtr->IsKindOf(RUNTIME_CLASS( CDDKSrvSocket)))
      {
      CString msgStartup;
         // wait untill the Application is ready for us
         CSingleLock lk(&DDKSockPtr->m_threadStartupEvent);
         lk.Lock(5000); // wait max 5 seconds
         msgStartup.Format("Socket %d listen thread ID=[%d] running", 
                          DDKSockPtr->m_socket, 
                          GetCurrentThreadId());
         DDKSockPtr->SockDataMessage(msgStartup);
         DDKSockPtr->SockStateChanged(SOCKETCURRENTLY_VOID);
               
         DDKSockPtr->Poll(debugStr);
      }
      else
      {
         sprintf(debugStr2, "CDDKSrvSocket SockAsyncFriend pointer corruption!!!!\n");
         OutputDebugString(debugStr2);
      }
   }
   catch (...) 
   {
      OutputDebugString( "Catch\n" );
      sprintf(debugStr2, "CDDKSrvSocket SockAsyncFriend Exception !!!!\n");
      OutputDebugString(debugStr2);
   }
   try
   {
      DDKSockPtr->m_listenThreadStatus = SOCKET_EX_TERMINATED;
      {
      CString d;
         d.Format("[Thread %4d Terminating.]\n", GetCurrentThreadId());
         OutputDebugString(d);
      }
      DDKSockPtr->m_threadDeadEvent.SetEvent();// CEvent
   }
   catch (...)
   {
      CString msg;
         msg.Format("INTERNAL APPLICATION ERROR FILE %s LINE: %d\n%s\n%s", 
            __FILE__, __LINE__, __MY_APPVERSION__, __DATE__);
      OutputDebugString(msg);
   }
   //AfxEndThread(0);

   return(0);
} // SockAsyncFriend


int HexToBin(const char * str, byte *pBuffer)
{
	int len=0;
	while (*str)
	{
		ConvertASCIIToByte(str, *pBuffer++);
		*str++;
		if (*str)
			*str++;
		if (*str)
			*str++;
		if (*str)
			*str++;
		len++;
	}
	return(len);
}


// ------------------------------- Poll -----------------------------------
//
void CDDKSrvSocket::Poll(CHAR * debugStr)
{
CHAR *      msgPtr = m_buffer; //rxMessage;
CHAR *      debugStrPtr = debugStr;
BOOL        incommingMsgOK = FALSE;
BOOL        outgoingMsgOK = FALSE;
// Socket "notification" setup variables
u_int       retCode; 
int         errorCode;
BOOL        exitFlag;
sockaddr    acceptedAsyncSocketAddr, ackSocketAddr;
int         addrlength;
unsigned long  numBytes=0;
CString        debuggerString;
LONG           numIdleLoops = MAX_IDLE_LOOPS;
DWORD		bufSize;


   m_debuggerStep = 0;
   accepted = FALSE;
   exitFlag = false;
   *debugStr = NULL;//zero string

   if (SOCKET_EX_TERMINATE == m_listenThreadStatus) //termination routine 
   {
      sprintf(debugStr, "EX_TERMINATE recieved, terminating!");
      OutputDebugString(debugStr);
      exitFlag = true;
   }
   AcceptedAsyncSocket = INVALID_SOCKET;

   // We want to read the asynchronous data responses
   // ..........Thread Control Loop
   while ( (SOCKET_EX_RUNNING == m_listenThreadStatus) 
            && (false == exitFlag)
         )
   {
      exitFlag = false;
      m_debuggerStep = 1;

      if (SOCKET_EX_TERMINATE == m_listenThreadStatus) //termination routine 
      {
         exitFlag = true;
         break;
      }
//#ifndef _TEST
//	bufSize = HexToBin("x06 x7D x00 x00 x00 x06 x01 x05 x00 x01 xFF x00", (byte*)msgPtr);
//	ProcessData(AcceptedAsyncSocket, msgPtr, bufSize);
//#endif
      if (!accepted)
      {
         m_debuggerStep = 2;
         numIdleLoops = MAX_IDLE_LOOPS;
         SockStateChanged(SOCKETCURRENTLY_LISTENING);
         debuggerString.Format("[%4d] Listen for connection...\n", GetCurrentThreadId());
         OutputDebugString(debuggerString);
         // Send to debugger list-box
         SockDataMessage(debuggerString);

         errorCode = listen(*m_pSocket, SOMAXCONN); //listen with backlog maximum reasonable
      
         if (SOCKET_ERROR == errorCode)
         {
            sprintf(debugStr, "Listen returned with a socket error : %s\n", sys_wsaerrlist[wsaerrno]);
            OutputDebugString(debugStr);
            exitFlag = true;
         }
         //now listening - no error !!!!

         addrlength = sizeof(acceptedAsyncSocketAddr);//for now
         WSASetLastError(0);
         if (AcceptedAsyncSocket == INVALID_SOCKET)
            accepted = FALSE;

         //------------------------- Listen Control Loop -------------------//
         do  
         {
            m_debuggerStep = 3;
             //Beep(3000,50);
            debuggerString.Format("[%4d] Accept connection...\n", GetCurrentThreadId());
            OutputDebugString(debuggerString);
            AcceptedAsyncSocket = accept(*m_pSocket,  //listen away returns immediatly
                                          &acceptedAsyncSocketAddr,
                                          &addrlength
                                         );
            if (INVALID_SOCKET !=AcceptedAsyncSocket)
            {
               m_debuggerStep = 4;
               debuggerString.Format("(%d)[%4d] Connection accepted.\n", *m_pSocket, GetCurrentThreadId());
               OutputDebugString(debuggerString);
               SockDataMessage(debuggerString);
                // copy the IP address
               memcpy(&ackSocketAddr, &acceptedAsyncSocketAddr, sizeof(acceptedAsyncSocketAddr));
               int test = h_errno;
               if ( (INVALID_SOCKET == AcceptedAsyncSocket) //check for fails
                      && (h_errno != WSAEWOULDBLOCK) //no connection avail to be accepted
                   ) 
               {
                   m_debuggerStep = 5;
                   sprintf(debugStr, "Listen Accept returned with a socket error : %s\n",sys_wsaerrlist[wsaerrno]);
                   OutputDebugString(debugStr);                    
                   exitFlag = true;
                   continue;   //skip the rest of this loopy
               }
               accepted = TRUE;   
            }
            else
            {
            LONG temp;
               
               // get the error.
               m_debuggerStep = 6;
               temp = wsaerrno;
               (temp >= 0) ? (sprintf(debugStrPtr, "Accept() Error on Listen socket : %s\n",sys_wsaerrlist[temp]))
                              : (sprintf(debugStrPtr, "Unknown Accept() error on Listen socket\n"));
               OutputDebugString(debugStrPtr);
               SockDataMessage(debugStrPtr);
               WSASetLastError(0);
            }
         }//Listen
         while ( (SOCKET_EX_RUNNING == m_listenThreadStatus) 
                  && (false == exitFlag)
                  && (AcceptedAsyncSocket == INVALID_SOCKET)
                  && (h_errno == WSAEWOULDBLOCK)
               );
      }
      if (INVALID_SOCKET !=AcceptedAsyncSocket)
      {
      DWORD initialTicks = GetTickCount();
         m_debuggerStep = 7;
         debuggerString.Format("%4d Currently connected.\n", GetCurrentThreadId());
         OutputDebugString(debuggerString);
         do
         {
            // Find out how many bytes are currently available for reading on the socket
            m_debuggerStep = 8;
            retCode = ioctlsocket(AcceptedAsyncSocket, FIONREAD, &numBytes);
            if (0==numBytes)
               Sleep(1);
            else
               SockStateChanged(SOCKETCURRENTLY_READING);
            if (m_serverBufferSize <= numBytes)
               numBytes = m_serverBufferSize-1; // -1 because we add a NULL char to the buffer contents latter

            if ((GetTickCount() - initialTicks) > 10000)
            {
               ASSERT(0);  // CDB, why do we ever get here?
               break;   // time out
            }
            if (SOCKET_EX_TERMINATE == m_listenThreadStatus) //termination routine 
            {
               exitFlag = true;
               break;
            }
         }
         while (0 == numBytes); //delay until an actual data bearing packet is brought in
                                //remembering only 1 packet per transaction allways

         if ( (SOCKET_ERROR != retCode)
               && (0 != numBytes)
            )
         { // Yes got have some bytes available on the socket
            m_debuggerStep = 9;
            numIdleLoops = MAX_IDLE_LOOPS;
            debuggerString.Format("%4d Server socket thread reading.\n", GetCurrentThreadId());
            OutputDebugString(debuggerString);
            // ...read them out
            incommingMsgOK = (Recieve(AcceptedAsyncSocket, numBytes, msgPtr, debugStrPtr)==(LONG)numBytes);
            m_debuggerStep = 10;
            if (SOCKET_EX_RUNNING != m_listenThreadStatus) //termination routine 
            {
               //exitFlag = true;
               break;
            }

            // process this message as valid data from a PLC
            if (incommingMsgOK)
            {

               m_debuggerStep = 11;
               bufSize = numBytes;     // ok so all bytes were read
               msgPtr[bufSize] = '\0'; // append a null for neatness
               m_debuggerStep = 12;
               if (FALSE == ProcessData(AcceptedAsyncSocket, msgPtr, bufSize))
               {
                  m_debuggerStep = 13;
                  
                  // close the server socket
                  closesocket(AcceptedAsyncSocket); // kill accepted instance immediately
                  accepted = FALSE;
                  OutputDebugString("Closing socket normally.\n");
                  SockStateChanged(SOCKETCURRENTLY_CLOSING);
                  SockDataMessage(debuggerString);
                  // 13 Rembrandt st Petervale
               }
               m_debuggerStep = 14;
            }
         }
         else
         { //We may have a problem please be a patient
            if (SOCKET_ERROR == retCode)
            {
               sprintf(debugStr, "Listen Poll Failed on rx, BytesRx'd : %d\n",numBytes);
               OutputDebugString(debugStr);
               // close this PLC connection so that new data still comes in
               closesocket(AcceptedAsyncSocket); //kill accepted instance immediatly
               accepted = FALSE;
               OutputDebugString("Closing socket after error.\n");
               SockStateChanged(SOCKETCURRENTLY_CLOSING);
            }
            else
            {  // no problem at all, socket was just idle.
               numIdleLoops--;
               if (numIdleLoops > 0)
               {
                  debuggerString.Format("[%4d] Listen Poll idle.\n", GetCurrentThreadId());
                  OutputDebugString(debuggerString);
                  SockStateChanged(SOCKETCURRENTLY_IDLE);
               }
               else
               {
                  debuggerString.Format("[%4d] Closing socket, idle for too long.\n", GetCurrentThreadId());
                  OutputDebugString(debuggerString);
                  SockDataMessage(debuggerString);

                  closesocket(AcceptedAsyncSocket); //kill accepted instance immediatly
                  accepted = FALSE;
                  SockStateChanged(SOCKETCURRENTLY_CLOSING);
               }
            }
         }
      }
   } //Thread
       
   //This is where we go to Terminate the Thread.

   m_listenThreadStatus = SOCKET_EX_TERMINATED;
} // Poll
