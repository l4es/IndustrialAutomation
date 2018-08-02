// MTransportProvider.h: interface for the MTransportProvider class.
//
//////////////////////////////////////////////////////////////////////

// interface class provides the root for customising transport layer (tcp/serial)

#if !defined(AFX_MTRANSPORTPROVIDER_H__3C878DA4_1002_4A42_9D1F_88014EE6403B__INCLUDED_)
#define AFX_MTRANSPORTPROVIDER_H__3C878DA4_1002_4A42_9D1F_88014EE6403B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4786) 


#include "DataBuffer.h"
#include "djsocket.h"


using namespace std;

// member variables for channel settings
class CPortSettingBase
{
public:
   //rs232 datas
   CString  portName; // serial com
   DCB      dcb;     // serial com

   // ip data
   int      ipPort;
   // 64-bit ip addr IPV4
};


// base-class for transports
class CMediumRoot
{
public:
   virtual bool Open(CPortSettingBase& protSettings) = 0;
   virtual bool Close() = 0;

   bool Send(CProtFrame &frame) {return(SendRaw(frame));}

   bool Recv(CProtFrame &frame, const DWORD length) {return(RecvRaw(frame, length));};

   void DebugFrame(CProtFrame &frame, const DWORD length);

   virtual void Flush() {};
   virtual bool SendRaw(CProtFrame &frame)=0;

   virtual bool RecvRaw(CProtFrame &frame, const DWORD length)=0;

};


class CSerialRoot : public CMediumRoot
{
public:
   CSerialRoot();
   bool Open(CPortSettingBase& protSettings);
   bool Close();

   bool SendRaw(CProtFrame &frame);

   bool RecvRaw(CProtFrame &frame, const DWORD length);

   /*virtual */void Flush();

private:
   HANDLE   m_hPort;
   DCB      dcb;

};
 

class CModbusRTURoot : public CSerialRoot
{
public:
   CModbusRTURoot();


   bool SendRaw(CProtFrame &frame);

   bool RecvRaw(CProtFrame &frame, const DWORD length);

private:

};

class CTcpIpRoot : public CMediumRoot
{
public:

};


#endif // !defined(AFX_MTRANSPORTPROVIDER_H__3C878DA4_1002_4A42_9D1F_88014EE6403B__INCLUDED_)
