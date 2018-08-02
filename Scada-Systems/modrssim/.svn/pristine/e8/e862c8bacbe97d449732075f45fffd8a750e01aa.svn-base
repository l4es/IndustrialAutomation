
#ifndef _PROCESSOR_H_INLCUDED
#define _PROCESSOR_H_INLCUDED

///////////////////////////////////////////////////////////////////////////////////
// Declares the BASE communication state-engine, which will simulate 
// a real CPU's actions.
//
// TCP/Ethernet comms is far simpler, and has no state engine, TCP/IP protocols 
// will derive a CPU class directly from the socket wrapper class.
class CCommsProcessor : public SimulationSerialPort  
{
public:
   DECLARE_DYNAMIC(CCommsProcessor)

   CCommsProcessor(LPCTSTR    portNameShort, 
                     DWORD    baud, 
                     DWORD    byteSize, 
                     DWORD    parity, 
                     DWORD    stopBits,
                     DWORD    rts,
                     int      responseDelay);

	virtual ~CCommsProcessor();

   virtual BOOL OnProcessData(const CHAR *pBuffer, DWORD numBytes, BOOL *discardData) = NULL;

   void SetPDUSize(DWORD size) {m_PDUSize = size;};
   
   // overrridden functions
   virtual void RSDataDebugger(const BYTE * buffer, LONG length, int transmit);
   void RSStateChanged(DWORD state);

   virtual void ActivateStation(LONG stationID);
   virtual BOOL StationIsActivated(LONG stationID);

   // data/members
   virtual BOOL IsMasterBusy() {return(TRUE);}
   virtual BOOL SupportsMaster() {return(FALSE);}

   UINT  m_responseDelay;
   //BOOL  m_MOSCADchecks;      // MOSCAD
   //CRS232Noise m_NoiseSimulator;

   CRITICAL_SECTION stateCS;

private:

   DWORD m_PDUSize;
};


#endif //_PROCESSOR_H_INLCUDED