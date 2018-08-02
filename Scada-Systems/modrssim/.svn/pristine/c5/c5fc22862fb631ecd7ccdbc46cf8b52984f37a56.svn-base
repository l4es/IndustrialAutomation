// MODCommsProcessor.h: interface for the CMODEthCommsProcessor class.
//
// Ethernet :
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TICOMMSPROCESSOR_H__FDE99A84_435C_4094_826C_175DBDB6E61F__INCLUDED_)
#define AFX_TICOMMSPROCESSOR_H__FDE99A84_435C_4094_826C_175DBDB6E61F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DDKSrvSocket.h"

#define MAX_RX_MESSAGELENGTH   4096     // a big buffer


class CMODEthCommsProcessor : public CDDKSrvSocket  
{
public:
	CMODEthCommsProcessor(int responseDelay,
                         BOOL  MOSCADchecks,
                         BOOL modifyThenRespond,
                         BOOL disableWrites,
                         LONG PDUSize,
                         WORD portNum);
	CMODEthCommsProcessor(int responseDelay,
                         BOOL  MOSCADchecks,
                         BOOL modifyThenRespond,
                         BOOL disableWrites,
                         LONG PDUSize,
                         SOCKET * pServerSocket);
	virtual ~CMODEthCommsProcessor();

   BOOL ProcessData(SOCKET openSocket, 
                    const CHAR *pBuffer, 
                    const DWORD numBytes);
   
   // overrridden functions
   void SockDataDebugger(const CHAR * buffer, LONG length, dataDebugAttrib att);
   void SockStateChanged(DWORD state);

   virtual void SockDataMessage(LPCTSTR msg);
   void ActivateStationLED(LONG stationID);
   BOOL StationIsEnabled(LONG stationID);


   void SetEmulationParameters(BOOL moscadChecks, 
                               BOOL modifyThenRespond, 
                               BOOL disableWrites);
   void SetPDUSize(DWORD size) {m_PDUSize = size;};
   BOOL LoadRegisters();
   BOOL SaveRegisters();


// data members   
   UINT m_responseDelay;
   BOOL m_linger;
   //BOOL m_useEthernetStationID;
   //DWORD m_stationID;
   // noise/error sim
   BOOL  m_modifyThenRespond;
   BOOL  m_MOSCADchecks;      // MOSCAD
   BOOL  m_disableWrites;     // simple test

   LONG  m_noiseIndexValue;
   CEthernetNoise m_NoiseSimulator;
   //CRS232Noise m_NoiseSimulator;

   LPCTSTR ProtocolName() { return(m_protocolName);};

private:
 CString m_protocolName;
   DWORD m_PDUSize;
   CRITICAL_SECTION stateCS;

};

#endif // !defined(AFX_TICOMMSPROCESSOR_H__FDE99A84_435C_4094_826C_175DBDB6E61F__INCLUDED_)
