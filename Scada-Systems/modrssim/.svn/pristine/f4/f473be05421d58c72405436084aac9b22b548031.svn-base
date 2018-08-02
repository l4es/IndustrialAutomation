// DDKSocket.h: interface for the CDDKSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DDKSOCKET_H__9CB02237_FD74_4292_987A_BA0AB6CB65F2__INCLUDED_)
#define AFX_DDKSOCKET_H__9CB02237_FD74_4292_987A_BA0AB6CB65F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//Polled Socket config states
#define SOCKET_UNCONFIGURED         0    // Used by logic
#define SOCKET_HEALTHY              1    // connection was ok at last check
#define SOCKET_UNHEALTHY            2    // connection maybe bad
#define SOCKET_INITIALISED          3    // connected

// server thread states
#define SOCKET_EX_PENDING	       0
#define SOCKET_EX_SUSPENDED       1
#define SOCKET_EX_RUNNING	       2
#define SOCKET_EX_TERMINATE       3
#define SOCKET_EX_TERMINATED      4

#define SOCKETCURRENTLY_VOID        0
#define SOCKETCURRENTLY_LISTENING   1
#define SOCKETCURRENTLY_READING     2
#define SOCKETCURRENTLY_WRITTING    3
#define SOCKETCURRENTLY_CLOSING     4
#define SOCKETCURRENTLY_IDLE        5

typedef enum dataDebugAttrib
{
   dataDebugTransmit = 0,
   dataDebugReceive,
   dataDebugOther,
   dataDebugText,

};

class CDDKSocket : public CObject
{
public:
   CDDKSocket();
	CDDKSocket(DWORD timeout);  // create connect (client) socket
	virtual ~CDDKSocket();

   // diagnostic
#ifdef _DEBUG
   VOID Dump(CDumpContext& dc) const;
#endif
   
   void GetSockError(CHAR * errString, BOOL reset=TRUE);
   LONG Recieve(SOCKET ackSock, //The accepted socket
                int    numberOfBytesToRead,// -> number of bytes to read.
                CHAR * BufferPtr,       //<- Data read from socket    
                CHAR * debugStrPtr       //Any errors 
               );
   LONG Send(SOCKET ackSock, //The accepted socket
               int    numberOfBytesToWrite,// -> number of bytes to write
               CHAR * BufferPtr,       //<- Data read from socket    
               CHAR * debugStrPtr       //Any errors 
              );
   
   // overridable notification functions
   virtual void SockStateChanged(DWORD state);
   virtual void SockDataDebugger(const CHAR * buffer, LONG length, dataDebugAttrib att);
   virtual void SockDataMessage(LPCTSTR msg);

   BOOL  CloseSocket(BOOL gracefully =FALSE, SOCKET sock = NULL);

	// data
   SOCKET			   m_socket;
   SOCKET          * m_pSocket;         // pointer to the real socket
	DWORD             m_socketStatus;                 
	SOCKADDR_IN       m_destSockaddr_in; //socket plc/destination ip address
	SOCKADDR_IN       m_localSockaddr_in;
//	fd_set            m_socket_fd_set;
   struct protoent   * m_ppe;                    // pointer to protocol information entry
	WORD			      m_wVersionRequested;      //Winsock dll version support var
	WSADATA           m_WSAData;                //winsock data struc     
   INT				   m_sockoptEnable;  
   BOOLEAN           m_serverObject;
   CHAR              * m_buffer;
   WORD              m_serverBufferSize;

protected:
	// methods
	void EnterWSA();
	void LeaveWSA();
   void Initialize();

   UINT friend SockAsyncFriend(LPVOID pParam);

   // data
   BOOL (*m_CallBackFuncPtr)(CHAR        *buffer, //-> rx'd message <- tx message to reply with
                           WORD        *bufSize,//-> rx size      <- tx size
                           SOCKADDR_IN *ip      //-> Plc ip address! 
                          );

   //HANDLE             m_hListenThread;             //Handle to exceptional read thread
   //DWORD              m_dwListenThreadID;          //
   CWinThread *       m_pWorkerThread;
   WORD               m_listenThreadStatus;        //
   BOOL               m_listenThreadCreated;       //is listen thread created.


	static BOOLEAN m_wsaInitialized;
	static CRITICAL_SECTION	m_wsaCS;
	static CRITICAL_SECTION	*m_pwsaCS;

   DECLARE_DYNAMIC(CDDKSocket)
//   DECLARE_DYNCREATE(CDDKSocket)
};

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


#endif // !defined(AFX_DDKSOCKET_H__9CB02237_FD74_4292_987A_BA0AB6CB65F2__INCLUDED_)
