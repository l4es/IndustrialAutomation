/////////////////////////////////////////////////////////////////////////////
//
// FILE: ServerRS232Array.h : headder file
//
// See _README.CPP
//
// interface for the CServerRS232Array class.
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ServerRS232Array_H__FFE95178_01F8_4161_870F_B035AD11D09A__INCLUDED_)
#define AFX_ServerRS232Array_H__FFE95178_01F8_4161_870F_B035AD11D09A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CServerRS232Array : public CObArray  
{
public:
	CServerRS232Array(int size = 2);
	virtual ~CServerRS232Array();

   void CloseAll();
   LONG AddServers(DWORD protocolSel,
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
				       BOOL useBCC,
                   DWORD source,
                   DWORD dest,
                   DWORD files,
                   BOOL run,
                   BOOL JOYread,
                   BOOL JOYwrite,
                   DWORD masterIdleTime,
                   DWORD timeoutValue
                   );
   LONG NumberConnected(); // # servers that are still connected
};

#endif // !defined(AFX_ServerRS232Array_H__FFE95178_01F8_4161_870F_B035AD11D09A__INCLUDED_)
