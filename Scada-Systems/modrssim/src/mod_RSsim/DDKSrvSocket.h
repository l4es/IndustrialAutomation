// DDKSrvSocket.h: interface for the CDDKSrvSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DDKSRVSOCKET_H__30C44B26_2876_4CE0_AA67_6A37BD065CAE__INCLUDED_)
#define AFX_DDKSRVSOCKET_H__30C44B26_2876_4CE0_AA67_6A37BD065CAE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <afxmt.h>

// U have to derive from this class in order to use it.
class CDDKSrvSocket : public CDDKSocket
{
public:
   CDDKSrvSocket(unsigned short port,unsigned long IPAddrULONG = ADDR_ANY, SOCKET * pServerSocket = NULL);
   DECLARE_DYNAMIC(CDDKSrvSocket)

   // diagnostic
#ifdef _DEBUG
   VOID Dump(CDumpContext& dc) const;
#endif

   virtual ~CDDKSrvSocket();

   void Poll(CHAR * debugStr);
   UINT friend AsyncFriend(LPVOID pParam);
   
   // user must derive from this class and override this method.
   virtual BOOL ProcessData(SOCKET openSocket, 
                            const CHAR *pBuffer, 
                            const DWORD numBytes) = NULL;

   // data
   BOOL        accepted;
   SOCKET      AcceptedAsyncSocket;
public:
   CEvent      m_threadStartupEvent, m_threadDeadEvent;
protected:
   LONG        m_debuggerStep;
};

#endif // !defined(AFX_DDKSRVSOCKET_H__30C44B26_2876_4CE0_AA67_6A37BD065CAE__INCLUDED_)
