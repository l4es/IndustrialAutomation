// ServerSocketArray.h: interface for the CServerSocketArray class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVERSOCKETARRAY_H__FFE95178_01F8_4161_870F_B035AD11D09A__INCLUDED_)
#define AFX_SERVERSOCKETARRAY_H__FFE95178_01F8_4161_870F_B035AD11D09A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CServerSocketArray : public CObArray  
{
public:
	CServerSocketArray(int size = 10);
	virtual ~CServerSocketArray();

   void CloseAll();
   LONG AddServers(int numServers, 
                   int port, 
                   int responseDelay, 
                   BOOL linger,
                   BOOL MOSCADchecks,
                   BOOL modifyThenRespond,
                   BOOL disableWrites,
                   DWORD PDUSize);
   LONG NumberConnected(); // # servers that are still connected
   SOCKET FirstConnected(); // # servers that are still connected

   BOOL kill;
   BOOL initiating;
};

#endif // !defined(AFX_SERVERSOCKETARRAY_H__FFE95178_01F8_4161_870F_B035AD11D09A__INCLUDED_)
