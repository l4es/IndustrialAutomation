#include "stdafx.h"
#include "DjSocket.h"
#include "TestSvr.h"


//___________________________________________________________________________
/////////////////////////////////////////////////////////////////////////////
// DjSocket implementation
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC( DjSocket , CObject )
DjSocket::DjSocket(DWORD timeout) : CObject()
{   
    //Note: any number of calls to WSAStartup is allowed an internal counter is maintained
    //      and by design this class will delete WSA correctly so.... relax
   
    localTimeout = timeout;
    //Test to see if winsock dll is available
    sockCfg.wVersionRequested = MAKEWORD( 1, 1 );
    errCode = WSAStartup( sockCfg.wVersionRequested, &(sockCfg.WSAData) );
    if ( errCode != 0 )
    {
        // ? Tell the user that we could not find a usable WinSock DLL.                                
         OutputDebugString("Unable to find a useable WinSock DLL"); 
         //return(FALSE);
    }
    sockCfg.winsockWasInit = true;
    // Confirm that the WinSock DLL supports 1.1.
    if ( LOBYTE( sockCfg.WSAData.wVersion ) != 1 ||
         HIBYTE( sockCfg.WSAData.wVersion ) != 1 ) 
    {
       // ? Tell the user that we could not find a usable WinSock DLL.                                  
       WSACleanup( );
       sockCfg.winsockWasInit = false;
    }
 
      // The WinSock DLL is acceptable. Proceed.
    // Zero Remote endpoint address..for the first time ie - clear this buffer ptr's zero'ed
   memset((void * ) &(sockCfg.destSockaddr_in), 0, sizeof (sockCfg.destSockaddr_in));
   sockCfg.hSock = NULL;
   sockCfg.socket = NULL;
}

DjSocket::~DjSocket(void)
{
   KillSocket();
   if (sockCfg.winsockWasInit) WSACleanup( );
   sockCfg.winsockWasInit = false;
}


VOID DjSocket::EnableTimeOutTimer(BOOL on)      //True-enabled,False-Disabled
{
   if (on)
      sockCfg.TimerEnabled = true;
   else
      sockCfg.TimerEnabled = false;
}

BOOL DjSocket::CheckTimeOutTimer(seconds testTime)  //Compares stored time to current time, returns true if exceeded
{
LONG     totalSecs = 0;

   sockCfg.EndTime     = CTime::GetCurrentTime();
   // Calculate how long the socket has been unhealthy for
   sockCfg.ElapsedTime = sockCfg.EndTime - sockCfg.StartTime;             
   // Convert this to a time in seconds
   totalSecs                     = sockCfg.ElapsedTime.GetTotalSeconds();

   if (totalSecs > testTime)
      return(true);//oops out of time!
   else
      return(false);//still ok!
}

VOID DjSocket::ResetTimeOutTimer(void)              //loads current time into struct
{ 
   sockCfg.StartTime   = CTime::GetCurrentTime();
   sockCfg.EndTime     = 0;
   sockCfg.ElapsedTime = 0;
}

void DjSocket::GetSockError(CHAR * errString,BOOL reset/*=true*/)
{
LONG wx = wsaerrno;

   sprintf(errString,sys_wsaerrlist[wx]);
    if (reset) WSASetLastError(0);
}


BOOL DjSocket::ConfigDestSocket(CHAR * IPaddrDec,
                                USHORT port,
                                int sockType,
                                CHAR * protocol,
                                CHAR * debugStrPtr
                               )
{
CHAR errStr[80];
LONG error;
INT sockoptEnable = 1; //enable
ULONG    commandParam;

  if (sockCfg.socket) //oops we must kill existing socket first
   {
      error = closesocket(sockCfg.socket);
      if (SUCCESS != error)
      {
         GetSockError(errStr);
         sprintf(debugStrPtr,"Critical in config - Socket error : %s",errStr);
         if (INTERNALDEBUG) OutputDebugString(debugStrPtr);
         // Fail the connection process
         sockCfg.socketStatus = SOCKET_UNCONFIGURED;
         return (false);
      }
      sockCfg.socket = (UINT) NULL;
      sockCfg.socketStatus = SOCKET_UNCONFIGURED;
   }

   // Zero Remote endpoint address..
   memset((void * ) &(sockCfg.destSockaddr_in), 0, sizeof (sockCfg.destSockaddr_in));
   
   // Set up the type of network (HARD - intel)
   sockCfg.destSockaddr_in.sin_family = PF_INET;

   // Check to see if the user has supplied an address 
   if ((sockCfg.destSockaddr_in.sin_addr.s_addr = inet_addr(IPaddrDec)) == INADDR_NONE)
   {
      sprintf(debugStrPtr, "No IP Address specified : Error : %ld", INADDR_NONE);
      OutputDebugString(debugStrPtr);
      // Fail the connection process
      sockCfg.socketStatus = SOCKET_UNCONFIGURED;
      return (false);
   }
  
   // Convert Port number to network byte order
   if ((sockCfg.destSockaddr_in.sin_port = htons((u_short)port)) == 0)
   {
      sprintf(debugStrPtr, "Connot use port %ld", port);
      if (INTERNALDEBUG) OutputDebugString(debugStrPtr);
      // Fail the connection process
      sockCfg.socketStatus = SOCKET_UNCONFIGURED;
      return (false);
   }

   // Map protocol name to protocol number
   if ((sockCfg.ppe = getprotobyname("tcp")) == NULL)
   {

      GetSockError(errStr);
      sprintf(debugStrPtr,"Critical in config - Socket tcp error : %s",errStr);
      if (INTERNALDEBUG) OutputDebugString(debugStrPtr);
      // Fail the connection process
      sockCfg.socketStatus = SOCKET_UNCONFIGURED;
      return (false);
   }

   // We will be using Streams (Connection-Based TCP/IP protocol)
   //type = SOCK_STREAM;

   // Allocate a socket
   sockCfg.socket = socket(sockCfg.destSockaddr_in.sin_family,
                           sockType,
                           sockCfg.ppe->p_proto
                          );

   // If we could not allocate a socket, we must fail the connection process
   if (sockCfg.socket == INVALID_SOCKET)  // recommended NT error check
   {
      GetSockError(errStr);
      sprintf(debugStrPtr, "Cannot create socket :%s", errStr);
      if (INTERNALDEBUG) OutputDebugString(debugStrPtr);
      // Fail the connection process
      sockCfg.socketStatus = SOCKET_UNCONFIGURED;
      return (false);
   }
   // Set the socket to not delay any sends 
   error = setsockopt( sockCfg.socket, SOL_SOCKET, TCP_NODELAY, (CHAR FAR * ) &sockoptEnable, sizeof (INT));
   
   // If we could not set the socket parameters, we must fail the connection process
   if (error == (LONG) SOCKET_ERROR)
   {
      GetSockError(errStr);
      sprintf(debugStrPtr, "Cannot set sockoption :%s", errStr);
      if (INTERNALDEBUG) OutputDebugString(debugStrPtr);
      // Fail the connection process
      sockCfg.socketStatus = SOCKET_UNCONFIGURED;
      return (false);
   }

   // Make socket non blocking
   commandParam = 12;
   if (ioctlsocket(sockCfg.socket, FIONBIO, &commandParam) == SOCKET_ERROR)
   {
      GetSockError(errStr);
      sprintf(debugStrPtr, "Cannot set socket non blocking :%s", errStr);
      if (INTERNALDEBUG) OutputDebugString(debugStrPtr);
      // Fail the connection process
      sockCfg.socketStatus = SOCKET_UNCONFIGURED;
      return (false);
     
   }
   
   //all is well here socket is configured
   sockCfg.socketStatus = SOCKET_CONFIGURED;
   return (true);
}


BOOL    DjSocket::ConnectSocket(CHAR * debugStrPtr)
{
LONG error;
CHAR errStr[80];
fd_set writefds;

   if (SOCKET_CONFIGURED == sockCfg.socketStatus) 
   {
       // Connect the socket (non - blocking)
      if (connect(sockCfg.socket,
                  (struct sockaddr * ) &sockCfg.destSockaddr_in,
                   sizeof (sockCfg.destSockaddr_in)
                 ) == SOCKET_ERROR
         )
      {//Note : here i think it should return a socket_error because a blocking socket cannot connect imediatly!
       
         // Put the socket in the fd_set for use by the select call
         FD_ZERO(&writefds);
      
         FD_SET(sockCfg.socket, &writefds);
      
         // Put the timeout to be equal to tiTimeout set by the user in the registry
         sockCfg.selectTimer.tv_sec  = localTimeout / 1000;  //Offset by 2 seconds (fudge)
         sockCfg.selectTimer.tv_usec = localTimeout * 1000;  // convert ms to usec
         //Using select to check because socket is non_blocking.
         error = select( (INT) NULL,      //ignored berkly sockets compatability only
                          NULL,           //no optional ptr to be checked for readability status
                          &writefds,      //ptr to be checked for writeablility status
                          NULL,           //no optional ptr to be checked for error status
                          &sockCfg.selectTimer  //maximum timeout to wait
                       );
         // select will return the number of sockets that can be written to and since we only have
         // one (the present) socket in the fd_set we only expect -1 (failed), 0 (timed out) or 1 (success)
      
         if (SOCKET_ERROR == error)//socket_connected = 1 where one is what was in fd_set
         {
            GetSockError(errStr);
            sprintf(debugStrPtr, "Cannot connect to socket error  :%s", errStr);
      
            // Fail the connection process
            sockCfg.socketStatus = SOCKET_UNHEALTHY;
            if (INTERNALDEBUG) OutputDebugString(debugStrPtr);
            return (false);
         }
         else
           if (0 == error) //timed out
           {
              sprintf(debugStrPtr, "Cannot connect to socket: Socket timed out");
                    // Fail the connection process
              sockCfg.socketStatus = SOCKET_UNHEALTHY;
              if (INTERNALDEBUG) OutputDebugString(debugStrPtr);
              return (false);
           }


      }
      
      
      // We passed all the checks
      // Start timing out with the sockets
      if (sockCfg.TimerEnabled)
      {     
         ResetTimeOutTimer();
      }     
      sockCfg.socketStatus = SOCKET_HEALTHY;
      return (true);
   }//if configured
   else
     return (false); //was obviously unconfigured! 
   
}

void DjSocket::KillSocket(void)
{
CHAR errStr[80];
CHAR LocaldebugStr[80];
LONG error;

  //Note lingering is probably going to be helpfull if hard close is required

   if (sockCfg.socket) //oops we must kill existing socket first
   {
      error = closesocket(sockCfg.socket);
      if (SUCCESS != error)
      {
         GetSockError(errStr);
         sprintf(LocaldebugStr,"Critical in config - Socket error : %s",errStr);
         if (INTERNALDEBUG) OutputDebugString(LocaldebugStr);
      }
      sockCfg.socket = (UINT) NULL;
      sockCfg.socketStatus = SOCKET_UNCONFIGURED; // or is the nothealthy check????????????
   }

}
//KillSocket

DWORD    DjSocket::CheckForDataRecieved(BOOL delay,CHAR * debugStrPtr)
//return -1 if error else return number of bytes available
{
int     retCode;
int     tempInt;
CHAR    errStr[80];
                             //%%%%%%%%%%%%!!!!!!!!!!!!***********&&&&&&&&&&&&&
    if (delay) 
    {
       sockCfg.selectTimer.tv_sec  = localTimeout / 1000;//timeout of x seconds
       sockCfg.selectTimer.tv_usec = 0;
    }
    else
    {
      sockCfg.selectTimer.tv_sec  = 0;
      sockCfg.selectTimer.tv_usec = 0; 
    }
    FD_ZERO(&(sockCfg.socket_fd_set));
    // Add our socket to the set
    FD_SET(sockCfg.socket, &(sockCfg.socket_fd_set));
    // Check to see if our socket has actually been added
    if ( FALSE == (FD_ISSET(sockCfg.socket, &(sockCfg.socket_fd_set))) )
    {
       sprintf(debugStrPtr,"Socket could not be added to fd_set(CheckForDataRecieved)");
       if (INTERNALDEBUG) OutputDebugString(debugStrPtr);
    }
    
    // Ask if there is any data in the socket (retCode = number of bytes available)
    retCode = select (0,       // reserved
                      &(sockCfg.socket_fd_set), // sock to read
                      NULL,    // sock to write
                      NULL,    // sockets for errors
                      &(sockCfg.selectTimer) // time to wait (0,0) = return immediately without wait
                     );
    if (SOCKET_ERROR == retCode)
     {
        tempInt = wsaerrno;
        if (tempInt >= 0)
        {
           GetSockError(errStr);
           sprintf(debugStrPtr,"Socket error : %s",errStr);
           if (INTERNALDEBUG) OutputDebugString(debugStrPtr);
        }
        else 
        {
           GetSockError(errStr);
           sprintf(debugStrPtr, "RetCode was -1 but wsaerrno was negative -> %ld\n",tempInt);
           if (INTERNALDEBUG) OutputDebugString(debugStrPtr);
        }
        retCode = -1; //prepare to tell caller
     }
    return(retCode);  

}//CheckForDataRecieved


DWORD DjSocket::GetRxData(int    numberOfBytesToRead,// -> number of bytes to read.
                         CHAR * BufferPtr,       //<- Data read from socket    
                         CHAR * debugStrPtr       //Any errors 
                        )
{
//returns -1 if error, or the number actualy read from socket
int numread;
int temp;
   //call recv func

   numread =  recv(sockCfg.socket,            // Our precious socket
                   BufferPtr,                  // RxBuffer 
                   (INT) numberOfBytesToRead,
                   (INT) NULL                  // no fancy flaggies
                  ); 
   //on sock_error
   if (SOCKET_ERROR == numread) 
   {
      sockCfg.socketStatus                = SOCKET_UNHEALTHY;
      temp = wsaerrno;
      (temp >= 0) ? (sprintf(debugStrPtr, "Read Error on socket : %s",sys_wsaerrlist[temp]))
                     : (sprintf(debugStrPtr, "Unknown Read error on socket"));
         WSASetLastError(0);
      return(-1);
      if (INTERNALDEBUG) OutputDebugString(debugStrPtr);
   }
   //on mismatch
   if (numread != numberOfBytesToRead)
   {
       sockCfg.socketStatus                = SOCKET_UNHEALTHY;
       sprintf(debugStrPtr, "Read Timeout (%ld/%ld) ", numread,numberOfBytesToRead);
       return(numread);
       if (INTERNALDEBUG) OutputDebugString(debugStrPtr);
       //I expect the caller to initiatate retries or whatever!
   }
   return(numread);
}

DWORD DjSocket::PutTxData(int    numberOfBytesToWrite,// <- number of bytes to write.
                         CHAR * BufferPtr,            // -> Data to write to socket  
                         CHAR * debugStrPtr           // Any errors 
                        )
{

//returns -1 if error, or the number actualy written to socket
int numwritten;
int temp;
   //call recv func

   numwritten =  send(sockCfg.socket,             // Our precious socket
                      BufferPtr,                  // TxBuffer 
                      (INT) numberOfBytesToWrite,
                      (INT) NULL                  // no fancy flaggies
                     ); 
   //on sock_error
   if (SOCKET_ERROR == numwritten) 
   {
      sockCfg.socketStatus                = SOCKET_UNHEALTHY;
      temp = wsaerrno;
      (temp >= 0) ? (sprintf(debugStrPtr, "Write Error on socket : %s",sys_wsaerrlist[temp]))
                     : (sprintf(debugStrPtr, "Unknown Write error on socket"));
         WSASetLastError(0);
      return(-1);
      if (INTERNALDEBUG) OutputDebugString(debugStrPtr);
   }
   //on mismatch
   if (numwritten != numberOfBytesToWrite)
   {
       sockCfg.socketStatus                = SOCKET_UNHEALTHY;
       sprintf(debugStrPtr, "Write Timeout (%ld/%ld) ", numwritten,numberOfBytesToWrite);
       return(numwritten);
       if (INTERNALDEBUG) OutputDebugString(debugStrPtr);
       //I expect the caller to initiatate retries or whatever!
   }
   return(numwritten);
}
//////////////////////////////////////////////////////////////////////////////////////


//___________________________________________________________________________
/////////////////////////////////////////////////////////////////////////////
// Listen TCP Socket implementation
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC( ListenTCPSocket , CObject )

ListenTCPSocket::ListenTCPSocket(unsigned short port,unsigned long IPAddrULONG/* = ADDR_ANY*/) : CObject()
   /*
    The port is explicitly specified and ip defaulted to local (any localy Avail)
    The thread is created but is left on hold.  
    The socket is also created here, but not set to listen here
   */
{
   CHAR     debugStr[MAX_DEBUG_STR_LEN];
   CHAR     err[MAX_DEBUG_STR_LEN];
   DWORD    result;
   // ULONG    commandParam;
   LONG     error;
   INT      sockoptEnable = TRUE;  
   struct   protoent  * ppe;          // pointer to protocol information entry

      CallBackFuncPtr = NULL; // set the CB pointer to null
      listenThreadStatus  = EX_TERMINATED;  
      listenThreadCreated = false;
      hListenThread       = NULL;
      // Zero local point address..
      memset((void * ) &(sockCfg.localSockaddr_in), 0, sizeof (sockCfg.localSockaddr_in));
      // Setup local addressing for listen socket
      sockCfg.localSockaddr_in.sin_family           = PF_INET;
      sockCfg.localSockaddr_in.sin_addr.S_un.S_addr = INADDR_ANY;//usually default
      /*sockCfg.localSockaddr_in.sin_addr.S_un.S_un_b.s_b1 = 40;
      sockCfg.localSockaddr_in.sin_addr.S_un.S_un_b.s_b2 = 10;
      sockCfg.localSockaddr_in.sin_addr.S_un.S_un_b.s_b3 = 10;
      sockCfg.localSockaddr_in.sin_addr.S_un.S_un_b.s_b4 = 15;*/

    //  sockCfg.localSockaddr_in.sin_port             = port;

       if ((sockCfg.localSockaddr_in.sin_port = htons((u_short) port)) == 0)
       {
          sprintf(debugStr, "Connot use port %ld", port);
          OutputDebugString(debugStr);
          
          // Fail the connection process
          sockCfg.socketStatus = SOCKET_UNCONFIGURED;
          return;
       }

      // Map protocol name to protocol number
      if ((ppe = getprotobyname("tcp")) == NULL)
      {
         GetSockError(err);
         sprintf(debugStr,"Driver cannot connect for listen :%s",err);
         OutputDebugString(debugStr);
         // Fail the connection process
         sockCfg.socketStatus = SOCKET_UNCONFIGURED;
         return;
      }

      // Allocate a listen socket
      sockCfg.socket = socket(PF_INET, SOCK_STREAM, ppe->p_proto); 
      // If we could not allocate a socket, we must fail the connection process
      if (INVALID_SOCKET == sockCfg.socket)  // recommended NT error check
      {
         GetSockError(err);
         sprintf(debugStr, "Cannot create Listen socket :%s",err);
         OutputDebugString(debugStr);
          // Fail the connection process
         sockCfg.socketStatus = SOCKET_UNCONFIGURED;
         return;
      }
      //now to bind socket to local address+port
      if (0 != bind(sockCfg.socket,(sockaddr *)&(sockCfg.localSockaddr_in),sizeof(sockCfg.localSockaddr_in) ) )
      {
         GetSockError(err);
         sprintf(debugStr, "Cannot Bind to Listen socket :%s",err);
         OutputDebugString(debugStr);
         // Fail the connection process
         sockCfg.socketStatus = SOCKET_UNCONFIGURED;
         return;

      }
      // Set the socket to not delay any sends 
      error = setsockopt(sockCfg.socket, IPPROTO_TCP, TCP_NODELAY, (CHAR FAR * ) &sockoptEnable, sizeof (INT));
      // If we could not set the socket parameters, we must fail the connection process
      if (error == (LONG) SOCKET_ERROR)
      {
         int lastError = WSAGetLastError();
         sprintf(debugStr, "Cannot setsockopt error (%ld)", lastError);
         OutputDebugString(debugStr);
         // Fail the connection process
         sockCfg.socketStatus = SOCKET_UNCONFIGURED;
         return;
      }
       /*
      // Make socket non blocking
      commandParam = 12;
      if (SOCKET_ERROR == ioctlsocket(sockCfg.socket, FIONBIO, &commandParam) )
      {
         int lastError = WSAGetLastError();
         sprintf(debugStr, "Driver cannot make socket non-blocking, error :%ld", lastError);
         // Fail the connection process
         sockCfg.socketStatus = SOCKET_UNCONFIGURED;
         return;
      } */
      //By default socket is blocking.. thats what i want here
 
   //all options are now setup now to listen.
   sockCfg.socketStatus = SOCKET_CONFIGURED;

   //Create Thread from here!
   //Create this thread only once for one or more devices
   if (!listenThreadCreated) //shouldn't ever realy be created as once per object is by design anyway
   {
      // Create read thread
      listenThreadStatus = EX_PENDING;
      hListenThread      = CreateThread(NULL,
                                        0,
                                        (LPTHREAD_START_ROUTINE)AsyncFriend,
                                        this,
                                        CREATE_SUSPENDED,
                                        (LPDWORD) &(dwListenThreadID) 
                                       );
      SetThreadPriority(hListenThread,/* THREAD_PRIORITY_NORMAL */THREAD_PRIORITY_TIME_CRITICAL);
   
      listenThreadStatus = EX_RUNNING;

      if (0 == KillEvent.ResetEvent() ) //set the event to HOLD
      {
          sprintf(debugStr, "Cannot reset event for Listen Socket kill");
          OutputDebugString(debugStr);
          listenThreadStatus = EX_TERMINATED; //cant go on if not ok
      }
    
      result = ResumeThread(hListenThread);//start thread here
   }
   
}


ListenTCPSocket::~ListenTCPSocket(void)/* The thread is cleaned up here if needed */
{
   CString szDebug;

   //First kill the socket
   closesocket(sockCfg.socket);
   sockCfg.socketStatus = SOCKET_UNCONFIGURED;

   
   //now kill the thread
   listenThreadStatus = EX_TERMINATE; // Signal ExRead member to stop 
   szDebug.Format( "Terminating Thread <<<%d>>> and STATUS {%d}\n", hListenThread, listenThreadStatus);
   OutputDebugString(szDebug);
   WaitForSingleObject((HANDLE)KillEvent,10000);
   // Terminate read thread
   TerminateThread(hListenThread, 0);
   CloseHandle(hListenThread);
}

  //________________________________Thread control methods_________________________

BOOL ListenTCPSocket::StartThread(void)/* The thread is set to running mode */
{
   DWORD result;
   DWORD x;
   result = ResumeThread(hListenThread);
   x =GetLastError();
   if ( ( result > 1) || (0xffffffff == result) )
   {
      listenThreadStatus = EX_SUSPENDED;
      return(false);
   }
   else //either was not suspended or was ... but is now running!
   {
      listenThreadStatus = EX_RUNNING;
      return(true);
   }
   return(false);
} 

BOOL ListenTCPSocket::PauseThread(void)/* The thread is stoped but not killed */
{
   SuspendThread(hListenThread );
   listenThreadStatus = EX_SUSPENDED;
   return(true);
}

//________________________________Generic decoder call back methods______________
  
BOOL ListenTCPSocket::DecoderFuncSetup(BOOL (*func)(CHAR *bufferPtr,WORD *bufSize, SOCKADDR_IN *ip))
{
   CallBackFuncPtr = func;
   return(true);
}

BOOL ListenTCPSocket::CallDecode(void)
   /* Decode will call a specific function in driver to 
      1.) decode recieved message
      2.) find the correct job,and update adroit with this data
      3.) return the reply in bufferPtr/bufSize */ 

{
BOOL result = FALSE;
CHAR * bufferPtr = buffer;

   if (NULL != CallBackFuncPtr) 
      result = (CallBackFuncPtr)(bufferPtr,&bufSize, &ackSocketAddr) ;
   return (result);
}

void AsyncFriend(ListenTCPSocket * LTCPSockPtr)
{
CHAR     debugStr2[MAX_DEBUG_STR_LEN];
CHAR     debugStr[MAX_DEBUG_STR_LEN];

   
   try 
   { 
      sprintf(debugStr2, "AsyncFriend Thread <<<%d>>> and STATUS {%d} PseudoHandle %x\n", LTCPSockPtr->hListenThread, LTCPSockPtr->listenThreadStatus, LTCPSockPtr->hListenThread);
      OutputDebugString(debugStr2);
		// call the function the thread will run in
      LTCPSockPtr->Poll(debugStr);
   }
   catch (...) 
   {
      OutputDebugString( "Catch\n" );
      sprintf(debugStr2, "Ex Read Thread FAILED !!!!\n");
      OutputDebugString(debugStr2);
   }
   return;

}


void ListenTCPSocket::Poll(CHAR * debugStr)
{
 
CHAR *       msgPtr = buffer; //rxMessage;
CHAR *       debugStrPtr = debugStr;
BOOL         incommingMsgOK = FALSE;
BOOL         outgoingMsgOK = FALSE;

// Socket "notification" setup variables

u_int        retCode; 
int          errorCode;    
BOOL         exitFlag;
SOCKET       AcceptedAsyncSocket;
sockaddr     acceptedAsyncSocketAddr;
int          addrlength;
unsigned long numBytes;


      exitFlag = false;
      *debugStr = NULL;//zero string

      if (EX_TERMINATE == listenThreadStatus) //termination routine 
      {
         sprintf(debugStr, "EX_TERMINATE recieved, terminating!");
         OutputDebugString(debugStr);
         exitFlag = true;
      }
   
     

      // We want to read the asynchronous data responses
      // and update the associated Jobs.

      //-------------------------------Thread Control Loopy--------------------------//
      while ( (EX_RUNNING == listenThreadStatus) 
               && (false == exitFlag)
            )
      {
         exitFlag = false;

         if (EX_TERMINATE == listenThreadStatus) //termination routine 
         {
            exitFlag = true;
         }

         errorCode = listen(sockCfg.socket,SOMAXCONN); //listen with backlog maximum reasonable
         
         if (SOCKET_ERROR == errorCode)
         {
            sprintf(debugStr, "Listen returned with a socket error : %s",sys_wsaerrlist[wsaerrno]);
            OutputDebugString(debugStr);
            exitFlag = true;
         }
            //now listening - no error !!!!

         addrlength = sizeof(acceptedAsyncSocketAddr);//for now
         WSASetLastError(0);

            //-------------------------Listen Control Loopy-------------------//
         do  
         {
             //Beep(3000,50);
             OutputDebugString("About to listen\n");

             AcceptedAsyncSocket = accept(sockCfg.socket,  //listen away returns imidiatly
                                          &acceptedAsyncSocketAddr,
                                          &addrlength
                                         );
             // copy the IP address67
             memcpy(&ackSocketAddr,&acceptedAsyncSocketAddr, sizeof(acceptedAsyncSocketAddr));
             int test = h_errno;
             if ( (INVALID_SOCKET == AcceptedAsyncSocket) //check for fails
                   && (h_errno != WSAEWOULDBLOCK) //no connection avail to be accepted
                ) 
             {
                sprintf(debugStr, "Listen Accept returned with a socket error : %s",sys_wsaerrlist[wsaerrno]);
                OutputDebugString(debugStr);                    
                exitFlag = true;
                continue;   //skip the rest of this loopy
             }

         }//Listen
         while ( (EX_RUNNING == listenThreadStatus) 
                  && (false == exitFlag)
                  && (AcceptedAsyncSocket == INVALID_SOCKET)           
                  && (h_errno == WSAEWOULDBLOCK)
               );

         //Ok if still thread loop still valid now the socket is accepted/connected

         //Beep(1000,100);
         do
         {
         retCode = ioctlsocket(AcceptedAsyncSocket,FIONREAD,&numBytes);
         }
         while (0 == numBytes); //delay until an actual data bearing packet is brought in
                                //remembering only 1 packet per transaction allways

         if ( (SOCKET_ERROR != retCode)
               && (0 != numBytes)
            )
         {//Yup got a message
            incommingMsgOK = (GetRxData(AcceptedAsyncSocket, numBytes, msgPtr, debugStrPtr)==(LONG)numBytes);
            OutputDebugString("Listen Thread polling PLC\n");
            // process this message as valid data from a PLC
            if (incommingMsgOK)
            {
               bufSize = (unsigned short)numBytes; //ok so all bytes were read
               if (TRUE == CallDecode() )
               {
                  //ok adroit updated successfuly so now tx ack to plc 
                  //buffer now contains the reply message
                  //note even if message was corrupted a suitable relpy must be tx'd
                  outgoingMsgOK = (SendTxData(AcceptedAsyncSocket, bufSize, msgPtr, debugStrPtr)==(LONG)bufSize);
                  closesocket(AcceptedAsyncSocket); //kill accepted instance immediatly
                  
               }
            }
         }
         else
         {//We have a problem please be a patient
            sprintf(debugStr, "Listen Poll Failed on rx, BytesRx'd : %d\n",numBytes);
            OutputDebugString(debugStr);
            // close this PLC connection so that new data still comes in
            closesocket(AcceptedAsyncSocket); //kill accepted instance immediatly
         }

      }//Thread
       
   //This is where we go to Terminate the Thread.
   //Under healthy conditions we should never get here.
   
   if (0 == KillEvent.SetEvent())
   {
      //could not clear event 
      //dont do anything as destructor will eventually timeout anyway!
   }

   listenThreadStatus = EX_TERMINATED;
}


void ListenTCPSocket::GetSockError(CHAR * errString,BOOL reset/*=true*/)
{
LONG wx = wsaerrno;

    sprintf(errString,sys_wsaerrlist[wx]);
    if (reset) WSASetLastError(0);
}

LONG ListenTCPSocket::GetRxData(SOCKET ackSock, //The accepted socket
                                 int    numberOfBytesToRead,// -> number of bytes to read.
                                 CHAR * BufferPtr,       //<- Data read from socket    
                                 CHAR * debugStrPtr       //Any errors 
                                )
{
//returns -1 if error, or the number actualy read from socket
int numread;
int temp;
CHAR debuggerMSG[1024];


   //call recv func

   numread =  recv(ackSock,                    // Our precious socket
                   BufferPtr,                  // RxBuffer 
                   (INT) numberOfBytesToRead,
                   (INT) NULL                  // no fancy flaggies
                  ); 

   // a littel debugger msg
   for (int i = 0; i < numread; i++)
   {
      sprintf(&debuggerMSG[i*3], "%02X ", (BYTE*)BufferPtr[i]);
   }
   strcat(debuggerMSG, " <- This came from the PLC\n");
   OutputDebugString(debuggerMSG);

   //on sock_error
   if (SOCKET_ERROR == numread) 
   {
      sockCfg.socketStatus                = SOCKET_UNHEALTHY;
      temp = wsaerrno;
      (temp >= 0) ? (sprintf(debugStrPtr, "Read Error on Listen socket : %s",sys_wsaerrlist[temp]))
                     : (sprintf(debugStrPtr, "Unknown Read error on Listen socket"));
         WSASetLastError(0);
      return(-1);
      OutputDebugString(debugStrPtr);
   }
   //on mismatch
   if (numread != numberOfBytesToRead)
   {
       sockCfg.socketStatus                = SOCKET_UNHEALTHY;
       sprintf(debugStrPtr, "Read Timeout (%ld/%ld) ", numread,numberOfBytesToRead);
       return(numread);
       OutputDebugString(debugStrPtr);
       //I expect the caller to initiatate retries or whatever!
   }

   return(numread);
}

LONG ListenTCPSocket::SendTxData(SOCKET ackSock,             //The accepted socket
                                 int    numberOfBytesToWrite,// -> number of bytes to write.
                                 CHAR * BufferPtr,           //-> buffer    
                                 CHAR * debugStrPtr          //Any errors 
                                )
{
//returns -1 if error, or the number actualy read from socket
int numwritten;
int temp;
CHAR debuggerMSG[1024];


   //call recv func

   numwritten =  send(ackSock,                    // Our precious socket
                   BufferPtr,                  // RxBuffer 
                   (INT) numberOfBytesToWrite,
                   (INT) NULL                  // no fancy flaggies
                  ); 

   // a littel debugger msg
   for (int i = 0; i < numwritten; i++)
   {
      sprintf(&debuggerMSG[i*3], "%02X ", (BYTE*)BufferPtr[i]);
   }
   strcat(debuggerMSG, " -> This is going to the PLC\n");
   OutputDebugString(debuggerMSG);

   //on sock_error
   if (SOCKET_ERROR == numwritten) 
   {
      sockCfg.socketStatus                = SOCKET_UNHEALTHY;
      temp = wsaerrno;
      (temp >= 0) ? (sprintf(debugStrPtr, "Write Error on Listen socket : %s",sys_wsaerrlist[temp]))
                     : (sprintf(debugStrPtr, "Unknown Write error on Listen socket"));
         WSASetLastError(0);
      return(-1);
      OutputDebugString(debugStrPtr);
   }
   //on mismatch
   if (numwritten != numberOfBytesToWrite)
   {
       sockCfg.socketStatus                = SOCKET_UNHEALTHY;
       sprintf(debugStrPtr, "Write Timeout (%ld/%ld) ", numwritten,numberOfBytesToWrite);
       return(numwritten);
       OutputDebugString(debugStrPtr);
       //I expect the caller to initiatate retries or whatever!
   }

   return(numwritten);

}







/*
/////////////////////////////////////////////////////////////////////////////////
// SocketArray class definition
/////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC( SocketArray , CObArray )
SocketArray::SocketArray() : CObArray()
{
   // the array collection class allocates memory internally to save time on the 
   // next call. When this "buffer" is used up it allocates enough memory for a 
   // couple of more elements.
   // devices are rarely allocated so we set the array size to grow by to 1

   // set array growing size.
   SetSize(GetSize(),  // set current size
           1);         // grow by 1 only
} // ctor SocketArray

// -------------------------- ~SocketArray --------------------------
SocketArray::~SocketArray()
{
DjSocket * SocketPtr;  
LONG n=GetUpperBound(); 

   // go backwards through the list so that the objects
   // are destroyed in reverse order of creation.
   while (n>=0)
   {
      SocketPtr = (DjSocket *)GetAt(n);  // Make sure to delete all items this array holds 
      Remove((HANDLE)n);               // remove from the list
      delete SocketPtr;
      n--;                             
   }
} // ~SocketArray

  

// ----------------------------------- SocketArray::Add ----------------------------------
//
LONG SocketArray::Add(DjSocket * SocketPtr)
{
DWORD size;
DWORD i;
   // add new object to list if succesfull
   if ( (SOCKET_CONFIGURED || SOCKET_HEALTHY) == SocketPtr->sockCfg.socketStatus)
   {  
      // Try to find empty element before growing array !
      size = GetSize();
      for(i=0 ; i<size ; i++)
      {
         if( NULL == GetAt(i))
         {
            SetAt(i, SocketPtr);
            return(SUCCESS);
         }
      }
      CObArray::Add((CObject *)SocketPtr);
      return(SUCCESS);
   }
   return(FAILED);
} 

// ----------------------------------- SocketArray::Find ----------------------------------
//
Socket * SocketArray::Find(Device  * devicePtr)
{
DjSocket * SocketPtr; 
DWORD size;
DWORD i;

   size = GetSize();
   for(i=0 ; i<size ; i++)
   {
      SocketPtr = GetAt(i);
      if(NULL == SocketPtr)
         continue;
      if( *SocketPtr == *devicePtr)
      {
         return (SocketPtr);
      }
   }
   return((DjSocket *)NULL);
} 


// ------------------------------ Remove ----------------------------------------
// Will detach resource and remove it from the array.
//
LONG SocketArray::Remove(HANDLE hSocket)
{
   SetAt((DWORD)hSocket, NULL);
   return(SUCCESS);

} // Remove

// ------------------------------ Remove ----------------------------------------
// Will detach resource and remove it from the array.
//
LONG SocketArray::Remove(DjSocket * SocketPtr)
{
DWORD n;
   n = GetSize();      
   while(n)
   {
      if( GetAt(n-1) == SocketPtr)
         break;
      n--;
   }
   if(n)
   {
      SetAt(n-1, NULL);
      return(SUCCESS);
   }
   return(FAILED);

} // Remove

// ----------------------------- GetAt ------------------------------------------
DjSocket * SocketArray::GetAt(const DWORD index)
{
   if((LONG)index > GetUpperBound() || 
      ((LONG)INVALID_HANDLE_VALUE == (LONG)index) 
     )
      return ((DjSocket *) NULL);
   return (DjSocket *) CObArray::GetAt(index);
} // GetAt()

// ------------------------- operator [] -----------------------------
DjSocket * SocketArray::operator [] (const DWORD index)
{
   return GetAt(index); // use overriden member
} // operator []

/////////////////////////////////////////////////////////////////////////////////
*/