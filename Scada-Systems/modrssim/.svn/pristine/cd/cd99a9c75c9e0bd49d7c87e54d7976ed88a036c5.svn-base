///////////////////////////////////////////////////////////////////////////////////////////
// "DjSocket.h"
// 
// ADT for a basic socket layer
///////////////////////////////////////////////////////////////////////////////////////////
//#include <winsock.h>
#include <winsock2.h.>

#ifndef __DjSocket_H
#define __DjSocket_H

// state for connect method
#define SOCKET_CONNECTED            1    // Used for select socket function 

//Polled Spcket config states
#define SOCKET_UNCONFIGURED         0    //Used by logic
#define SOCKET_CONFIGURED           1    //
#define SOCKET_HEALTHY              2    //
#define SOCKET_UNHEALTHY            3    //

#define SOCKET_SWITCHED             4    //


#define SOCKET_UNINITIALISED 5//compatability remove soon
#define SOCKET_NOT_AVAILABLE 6
#define SOCKET_INITIALISED   7
#define SOCKET_NOTHEALTHY    8

#define EX_PENDING	 0
#define EX_SUSPENDED  1
#define EX_RUNNING	 2
#define EX_TERMINATE  3
#define EX_TERMINATED 4



typedef struct _SockCfgStruct //this is used only for listen socket not the polled socket
//see comres for SOCKSTRUCT
{
   //Socket configuration info
   HANDLE         hSock;                  
   SOCKET         socket;
   DWORD          socketStatus;                 
   SOCKADDR_IN    destSockaddr_in; //socket plc/destination ip address
   SOCKADDR_IN    localSockaddr_in;
   fd_set         socket_fd_set;

   //Socket Global info
   WORD                wVersionRequested;      //Winsock dll version support var
   WSADATA             WSAData;                //winsock data struc     
   struct  protoent  * ppe;                    // pointer to protocol information entry
   BOOL                winsockWasInit;         // to kill or not to kill

   //Socket Timer info
   BOOL               TimerStatus;   //True - is still ok, false - Time exceeded
   BOOL               TimerEnabled;  //True - if you want time-out function.
   CTime              StartTime;     //Timer set for Reciever
   CTime              EndTime;
   CTimeSpan          ElapsedTime;
   timeval            selectTimer;  //resolution milliseconds

 }SockCfgStruct;

typedef int seconds;

///////////////////////////////////////////////////////////////////////////////
// Debug directive
#define INTERNALDEBUG 0 //0-no 1-yes
///////////////////////////////////////////////////////////////////////////////


//_________________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////////
// TCPSocket Listen socket class declaration
///////////////////////////////////////////////////////////////////////////////////
//_________________________________________________________________________________

class ListenTCPSocket:public CObject
{
DECLARE_DYNAMIC( ListenTCPSocket )

public:   
  //________________________________Construction methods___________________________

   ListenTCPSocket(unsigned short port,unsigned long IPAddrULONG = ADDR_ANY);
   /*
    The port is explicitly specified and ip defaulted to local (any localy Avail)
    The thread is created but is left on hold.  
    The socket is also created here, but not set to listen here
    socketStatus return either SOCKET_CONFIGURED or UNCONFIGURED
    listenThreadStatus returns EX_SUSPENDED or TERMINATED+handle=null
    NOTE: WSASTARTUP MUST BE INITIALIZED BEFORE THIS .... TEMP (FIX LATER!)
   */
   ~ListenTCPSocket(void);/* The thread is cleaned up here if needed */

  //________________________________Thread control methods_________________________

   BOOL StartThread(void);/* The thread is set to running mode */
   BOOL PauseThread(void);/* The thread is stoped but not killed */
 
  //________________________________Generic decoder call back methods______________
  
   BOOL DecoderFuncSetup(BOOL (*func)(CHAR *Buffer,WORD *bufSize,SOCKADDR_IN *ip));
   BOOL CallDecode(void);
   /* Decode will call a specific function in driver to 
      1.) decode recieved message
      2.) find the correct job,and update adroit with this data*/ 
  
   void friend AsyncFriend(ListenTCPSocket * LTCPsockPtr);
   void Poll(CHAR * debugStr);

   void GetSockError(CHAR * errString,BOOL reset = true);/*nice clean error message*/

   LONG GetRxData(SOCKET ackSock,              //The accepted socket
                   int    numberOfBytesToRead, // -> number of bytes to read.
                   CHAR * BufferPtr,           //<- Data read from socket    
                   CHAR * debugStrPtr          //Any errors 
                  );
   LONG SendTxData(SOCKET ackSock,             //The accepted socket
                   int    numberOfBytesToWrite,// -> number of bytes to write.
                   CHAR * BufferPtr,           //-> buffer    
                   CHAR * debugStrPtr          //Any errors 
                  );

   //______________________________Members_________________________________________

   //______________________________Call back members_______________________________
   BOOL (*CallBackFuncPtr)(CHAR        *buffer, //-> rx'd message <- tx message to reply with
                           WORD        *bufSize,//-> rx size      <- tx size
                           SOCKADDR_IN *ip      //-> Plc ip address! 
                          );

   //______________________________Tread management members________________________
   HANDLE             hListenThread;             //Handle to exceptional read thread
   DWORD              dwListenThreadID;          //
   WORD               listenThreadStatus;        //
   BOOL               listenThreadCreated;       //is listenthread created.
   CHAR               buffer[1000];              //recieved buffer.
   WORD               bufSize;                   //recieved buffer size.
   CEvent             KillEvent;

   SockCfgStruct      sockCfg;
private:
   SOCKADDR_IN ackSocketAddr;

};//class ListenTCPSocket




///////////////////////////////////////////////////////////////////////////////
// DjSocket object declaration
///////////////////////////////////////////////////////////////////////////////

class DjSocket:public CObject
{
DECLARE_DYNAMIC( DjSocket )

public:   
  //Construction methods

   DjSocket(DWORD timeout = 3000);  //Where winsock is called , version checking etc               
  ~DjSocket(void);  //Where winsock is cleaned up.
  
  // timer methods

  BOOL    CheckTimeOutTimer(seconds testTime);  //Compares stored time to current time, returns true if exceeded
  VOID    ResetTimeOutTimer(void);              //loads current time into struct
  VOID    EnableTimeOutTimer(BOOL on);          //True-enabled,False-Disabled

  BOOL    ConfigDestSocket(CHAR *IPaddrDec,      //} 
                           USHORT port,          //}
                           int sockType,         //} combine to be 
                           CHAR * protocol,      //}      
                           CHAR * debugStrPtr    //}
                          );
  /* ConfigDestSocket will configure the socket even if allready configured it will return
     true if succesful else it will return false plus an error string
     It also sets socketStaus to SOCKET_CONFIGURED or SOCKET_UNCONFIGURED
     It also loads all ip info into sockCfg for quick further reference
  */

  BOOL    ConnectSocket(CHAR * debugStrPtr);
  /* Connect Socket will  attemp tion connect to the socket as non-blocking it will return
     true if successful else it will return false plus an error string
     It also set socketStatus to SOCKET_HEALTHY or else SOCKET_UNHEALTHY
  */



  //Data/Buffer stuff
  DWORD    GetRxData(int  numberOfBytesToRead,// -> number of bytes to read.
                    CHAR * BufferPtr,       //<- Data read from socket    
                    CHAR * debugStrPtr       //Any errors 
                   );
  DWORD    PutTxData(int    numberOfBytesToWrite,// -> number of bytes to read.
                    CHAR * BufferPtr,           //<- Data read from socket    
                    CHAR * debugStrPtr          //Any errors 
                   );
  DWORD    CheckForDataRecieved(BOOL delay,CHAR * debugStrPtr);

  //error handling methods
  void    GetSockError(CHAR * errString,BOOL reset = true);
  void    KillSocket(void);

  // LONG    GetLocalIPAddressAtRuntime(SOCKET testOnThisSocket); //Returns local IP address, specify a connected socket to test on or defaults to creating a temporary socket to test on.

  //members
  SockCfgStruct    sockCfg;
 // ListenTCPSocket  listenSockObj(502);
 // BOOL             listenEnabled;

private:
 
 
  //members
  int  errCode;
  DWORD localTimeout;

  

};




/*
///////////////////////////////////////////////////////////////////////////////
// DjSocket SocketArray object declaration
///////////////////////////////////////////////////////////////////////////////

class SocketArray : public CObArray
{
DECLARE_DYNAMIC( SocketArray )
public:                                              

    SocketArray();
   // destructor . delete all resources
   ~SocketArray();

   DjSocket * operator [] (const DWORD index);

   LONG       Remove(HANDLE hSocket);
   LONG       Remove(DjSocket * SocketPtr);      
   
   // overriden members
   DjSocket * GetAt(const DWORD index);

   // member functions
   LONG       Add(DjSocket * SocketPtr);  
   DjSocket * Find(Device * devicePtr);
 
}; // class SocketArray

*/


/* 
 * helpful array corresponding to WSAGetLastError() returns
 * UNIX-style error messages for network errors
 *
 */

#define wsaerrno (WSAGetLastError() - WSABASEERR)

static char *sys_wsaerrlist[] = {

	"","","","","","","","","","","","","","","","","","",	/* 0  - 17 */
	"","","","","WSAEINVAL","","","","","","","","","","","","",	/* 18 - 34 */
	
	/* non-blocking and interrupt i/o */

	"Operation would block",		/* WSAEWOULDBLOCK - 35 */
	"Operation now in progress",		/* WSAEINPROGRESS - 36 */
	"Operation already in progress",	/* WSAEALREADY    - 37 */

	/* ipc/network software */ 

		/* argument errors */

	"Socket operation on non-socket",	/* WSAENOTSOCK    - 38 */
	"Destination address required",		/* WSAEDESTADDRREQ- 39 */
	"Message too long",			/* WSAEMSGSIZE    - 40 */
	"Protocol wrong type for socket",	/* WSAEPROTOTYPE  - 41 */
	"Protocol not available",		/* WSAENOPROTOOPT - 42 */
	"Protocol not supported",		/* WSAEPROTONOSUPPORT - 43 */
	"Socket type not supported",		/* WSAESOCKTNOSUPPORT - 44 */
	"Operation not supported on socket",	/* WSAEOPNOTSUPP  - 45 */
	"Protocol family not supported",	/* WSAEPFNOSUPPORT- 46 */
	"Address family not supported by protocol family",	
						/* WSAEAFNOSUPPORT- 47 */       
	"Address already in use",		/* WSAEADDRINUSE  - 48 */ 
	"Can't assign requested address",	/* WSAEADDRNOTAVAIL-49 */

   		/* operational errors */

	"Network is down",			/* WSAENETDOWN    - 50 */ 				
	"Network is unreachable",		/* WSAENETUNREACH - 51 */
	"Network dropped connection on reset",	/* WSAENETRESET   - 52 */
	"Software caused connection abort",	/* WSAECONNABORTED- 53 */
	"Connection reset by peer",		/* WSAECONNRESET  - 54 */
	"No buffer space available",		/* WSAENOBUFS     - 55 */
	"Socket is already connected",		/* WSAEISCONN     - 56 */
	"Socket is not connected",		/* WSAENOTCONN    - 57 */
	"Can't send after socket shutdown",	/* WSAESHUTDOWN   - 58 */
	"Too many references: can't splice",	/* WSAETOOMANYREFS- 59 */
	"Connection timed out",			/* WSAETIMEDOUT   - 60 */
	"Connection refused",			/* WSAECONNREFUSED- 61 */
	"Too many levels of symbolic links",	/* WSAELOOP	  - 62 */
	"File name too long",			/* WSAENAMETOOLONG- 63 */
	"Host is down",				/* WSAEHOSTDOWN   - 64 */
	"No route to host",			/* WSAEHOSTUNREACH- 65 */
	"Directory not empty",			/* WSAENOTEMPTY   - 66 */

	/* quotas & mush */

	"Too many processes",			/* WSAEPROCLIM    - 67 */
	"Too many users",			/* WSAEUSERS      - 68 */
	"Disc quota exceeded",			/* WSAEDQUOT      - 69 */
	"Stale file handle",			/* WSAESTALE	  - 70 */
	"Remote file system not responding"	/* WSAEREMOTE     - 71 */
};

 
#endif // __DjSocket_H      
        
        
        
 