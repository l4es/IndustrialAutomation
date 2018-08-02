/////////////////////////////////////////////////////////////////////////////
//
// FILE: ABCommsProcessor.h : headder file
//
// See _README.CPP
//
// Interface for the CMODCommsProcessor class.
// This was previosly where the TICommsProcessor class lived.
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMSPROCESSOR_H__FDE99A84_435C_4094_826C_175DBDB6E61F__INCLUDED_)
#define AFX_COMMSPROCESSOR_H__FDE99A84_435C_4094_826C_175DBDB6E61F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define AB_FUNC_WRITE3     (ALLENBRADLEY_WORD_WRITE)// 0xAA  // logical write with 3 fields
#define AB_FUNC_READ3      (ALLENBRADLEY_WORD_READ)//0xA2

#define AB_COMMAND_DIAGS   0x06  // the AB diagnostic funcs
#define AB_COMMAND_CPU     0x07  // CPU commands
#define AB_COMMAND_DRIVER  (ALLENBRADLEY_SLC_CMD)//0x0F  // DF1 driver commands

#define ABDF1SIM_NAME  "ABDF1SIM"

#define MAX_AB_MESSAGELEN  256  // RX of a command
#define AB_HEADDER_LEN       4  // STN CMD and Address and Length fields
#define AB_NET_LENGTH        8  // net layer length excl. data
#define AB_CRC_LEN           2  //
#define AB_BCC_LEN           1  //
#define AB_NORMAL_LEN        (AB_HEADDER_LEN + AB_NET_LENGTH + AB_CRC_LEN)

#define AB_MAXIMUM_NAKS       5

#define MAX_ABPORT_MESSAGELEN       512
#define MAX_AB_MEMFILES             255    // !!! valid files are 0-254

#define ABMASTER_IDLETIME           500     // between messages (end-to-start) time

#define AB_MASTER_ID                 (BYTE)(m_ABMasterSource) // 10       // SCC unit station # is 10
#define ABMASTER_SECONDPLC_ID        0                   // sheared data slave PLC station #
#define ABMASTER_PRIMARYPLC_ID       (BYTE)(m_ABMasterDest)   //2        // surface data slave PLC station #

#define ABMASTER_NUMFILES            (BYTE)(m_ABMasterNumFiles) //20
#define ABMASTER_FIRSTSHIELD         1   // always 1
#define ABMASTER_FIRSTFILENUM        100 // add file # to this
#define ABMASTER_ELEMENTS_START      0   // 1st register
#define ABMASTER_ELEMENTS_END        10  //39  // last reg.

#define ABMASTER_PRIMARYPLCPOLLCOUNT 5   // # surface plc polls to do

#define JOY_MAXRAMPOSITION           50  // shield hydraulic ram limit
#define JOY_MINROOFPRESSURE          39  // 0=8000 psi = 39-200 (raw)
#define JOY_12VOLTS                  204 // 0-15v = 0-255 (raw)

extern CControllerMemory PLCMemory;

// Declares the BASE communication state-engine, which will simulate 
// a real CPU's actions. 
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

   virtual void ActivateStationLED(LONG stationID);

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



//////////////////////////////////////////////////////////////////////
//
class CABCommsProcessor : public CCommsProcessor
{
public:
   DECLARE_DYNAMIC(CABCommsProcessor)

   CABCommsProcessor(LPCTSTR    portNameShort, 
                     DWORD    baud, 
                     DWORD    byteSize, 
                     DWORD    parity, 
                     DWORD    stopBits,
                     DWORD    rts,
                     int      responseDelay,
					      BOOL     bcc);

   LONG Send(int numberOfBytestoWrite, 
             const BYTE* bufferPtr,
             BOOL insertDLEs, 
             CHAR* debugStrPtr
            );

   LPCTSTR ProtocolName() { return(m_protocolName);};
   void SetProtocolName(LPCTSTR name) { m_protocolName = name;};


   virtual BOOL StationIsEnabled(LONG stationID);

   // event handlers
   virtual BOOL OnMessageReceived(BYTE* pAppLayerMsg, DWORD length);  // return false to reject the message

   virtual BOOL OnProcessData(const CHAR *pBuffer, 
                            DWORD numBytes, 
                            BOOL *discardData);

   virtual void RSDataDebugger(const BYTE * buffer, LONG length, int transmit);
   virtual void RSDataMessage(LPCTSTR msg);
   virtual void RSStateChanged(DWORD state);
   virtual void RSModemStatus(DWORD status);

   // message sending
   virtual BOOL SendPLCMessage(const BYTE* pAppLayerMsg, DWORD length);  // if False, then re-send
   
   virtual void OnHWError(DWORD dwCommError);

   LONG GetRALength();

   BOOL  LoadRegisters();
   BOOL  SaveRegisters();

   WORD GetNextTransactionID(BYTE station);
   WORD GetCurrentTransactionID(BYTE station);
   WORD SetCurrentTransactionID(BYTE station, WORD id);

   BOOL TestReceiveMessage();
   //void BuildResponse(BYTE *buffer, DWORD *length);
   LONG InsertDLEs(const BYTE * buffer, DWORD len, BYTE* destBuff, DWORD *destLen);
   LONG RemoveDLE(const BYTE * buffer, DWORD len, BYTE * destBuff, DWORD *destLen);
   
   // get these into the dialog
   UINT  m_lastABFile;
   UINT  m_lastABItem;
   UINT  m_lastABTrans;
   UINT  m_lastABValue;
   UINT  m_lastABLength;
   CString m_lastRX;
   CString m_lastTX;
   // JOY
   DWORD m_ABMasterSource;   // this ID
   DWORD m_ABMasterDest;     // SCADA/slave station ID
   DWORD m_ABMasterNumFiles;
   BOOL  m_ABMasterRunMode;
   BOOL  m_ABMasterReadVariables;
   BOOL  m_ABMasterWriteVariables;


   void DoMaster();  // any master PLC routines can run here

   virtual BOOL IsMasterBusy() {return(m_masterBusy);}

   virtual BOOL SupportsMaster() {return(TRUE);}

   // CPU state engine data
   enum __EngineModes
   {
      MODE_SLAVE,
      MODE_MASTER
   };

   enum __EngineStates
   {
      ENG_STATE_IDLE=0,    // wait for start
      ENG_STATE_RECEIVE,   // RX rest of telegram + ETX + CRC
      ENG_STATE_SENDACK,   // sending an ACK
      ENG_STATE_RESPOND,   // sending msg response
      ENG_STATE_FINALACK,  // wait for final ACK
      ENG_STATE_MASTERIDLE // for PLC master
   };

   BOOL  m_masterBusy;        // so that master knows it is waiting for a ACK
   BOOL  m_masterWaiting;
   DWORD m_lastMasterTick;    // last busy trans tick (to stop master just sending all the time)
   DWORD m_ABmasterIDLETime;
   DWORD m_ABtimeoutValue;


   void SetEngineState(__EngineStates stateStep);
   CString m_CPUstateEngineStepname;

   BOOL m_modifyThenRespond;
   BOOL m_disableWrites;
   BOOL	m_useBCCchecks;

   BOOL IsMaster() { return( MODE_MASTER == m_CPUmode);};
   BOOL MasterTimedOut();

private:
   __EngineStates m_CPUstateEngineStep;
   __EngineModes  m_CPUmode;  // MODE_MASTER or slave

   BYTE  m_lastAppBuffer[MAX_ABPORT_MESSAGELEN*2];  //keep space for double DLE
   DWORD m_lastAppLength;
   DWORD m_messageNAKs;


   // noise string
   BYTE  m_noiseBuffer[MAX_ABPORT_MESSAGELEN*2];  //keep space for 2 messages
   DWORD m_guardJunk;
   DWORD m_noiseLength;


protected:
 CString m_protocolName;

   WORD m_masterTN[256];   // transaction numbers are kept per station
   WORD m_masterRandom;    // pseudo-random sequence variable

   LONG m_primaryPolls;    // counts down to zero
};



///////////////////////////////////////////////////////////////////////
// PLC application program layer class
//
class CPLCApplication : public CABCommsProcessor
{
public:
   DECLARE_DYNAMIC(CPLCApplication)

   CPLCApplication(LPCTSTR    portNameShort, 
                     DWORD    baud, 
                     DWORD    byteSize, 
                     DWORD    parity, 
                     DWORD    stopBits,
                     DWORD    rts,
                     int      responseDelay,
                     BOOL  MOSCADchecks,
                      BOOL modifyThenRespond,
                      BOOL disableWrites,
					  BOOL bcc);
   // event handlers
   virtual BOOL OnMessageReceived(BYTE* pAppLayerMsg, DWORD length);  // return false to reject the message

   // message sending
   virtual BOOL SendPLCMessage(const BYTE* pAppLayerMsg, DWORD length);  // if False, then re-send

   BOOL SendPLCBlock(BYTE sourceStationID, 
                     BYTE destStationID, 
                     WORD TNS, 
                     WORD fileNum,
                     WORD startRegister,
                     WORD numRegisters);
   BOOL FetchPLCBlock(BYTE sourceStationID, 
                      BYTE destStationID, 
                      WORD TNS, 
                      WORD fileNum,
                      WORD startRegister,
                      WORD numRegisters);
private:
   void BuildResponse(const BYTE *inBuffer, DWORD inLength, BYTE *buffer, DWORD *length);
   
   // members
   WORD  m_TNS;
};

//
//
class CAB232CommsProcessor : public CPLCApplication
{
public:

   DECLARE_DYNAMIC(CAB232CommsProcessor)

   CAB232CommsProcessor(LPCTSTR portNameShort, 
                        DWORD  baud, 
                        DWORD byteSize, 
                        DWORD parity, 
                        DWORD stopBits,
                        DWORD rts,
                        int responseDelay,
                        BOOL MOSCADchecks,
                        BOOL modifyThenRespond,
                        BOOL disableWrites,
		       				BOOL bcc,
                        DWORD idleTime,
                        DWORD timeoutValue
                       );


   CRS232Noise m_NoiseSimulator;

   void SetEmulationParameters(BOOL moscadChecks, 
                               BOOL modifyThenRespond, 
                               BOOL disableWrites
                              );
   void SetJoyParameters(DWORD source, DWORD dest, DWORD shields, BOOL run, BOOL read, BOOL write)\
      { m_ABMasterSource = source; m_ABMasterDest = dest; \
        m_ABMasterNumFiles = shields; m_ABMasterRunMode = run; \
        m_ABMasterReadVariables = read; m_ABMasterWriteVariables = write; }


};


#endif // !defined(AFX_COMMSPROCESSOR_H__FDE99A84_435C_4094_826C_175DBDB6E61F__INCLUDED_)
