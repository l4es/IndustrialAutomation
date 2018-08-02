/////////////////////////////////////////////////////////////////////////////
//
// FILE: MODCommsProcessor.h : headder file
//
// See _README.CPP
//
// Interface for the CMOD232CommsProcessor class.
// RS232 :
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_232COMMSPROCESSOR_H__FDE99A84_435C_4094_826C_175DBDB6E61F__INCLUDED_)
#define AFX_232COMMSPROCESSOR_H__FDE99A84_435C_4094_826C_175DBDB6E61F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// reads
#define MOD_READ_COILS           0x01     // protocol cmd 1
#define MOD_READ_DIGITALS        0x02     // command 2 (read inputs)
#define MOD_READ_HOLDING         0x03
#define MOD_READ_REGISTERS       0x04      // analog input registers
// writes
#define MOD_WRITE_SINGLE_COIL    0x05     // un-implemented?
#define MOD_WRITE_SINGLEHOLDING  0x06     // tested, preset single register
#define MOD_WRITE_MULTIPLE_COILS 0x0F
#define MOD_WRITE_HOLDING        0x10
#define MOD_MASKEDWRITE_HOLDING  0x16     // AND,OR mask
// Extended registers
#define MOD_READ_EXTENDED        0x14
#define MOD_WRITE_EXTENDED       0x15

// modbus error codes
#define MOD_EXCEPTION_ILLEGALFUNC   0x01
#define MOD_EXCEPTION_ILLEGALADDR   0x02
#define MOD_EXCEPTION_ILLEGALVALUE  0x03
#define MOD_EXCEPTION_BUSY          0x06

#define MAX_232RX_MESSAGELENGTH   4096     // a big buffer
#define MODBUS_FRAME_LENGTH_MAX   8    // longest frame excluding data

// MOSCAD macros
#define MOSCADTABLENUMBER(a) ((WORD)(a & 0xF800) >> 11)
#define MOSCADCOLNUMBER(a)   ((WORD)(a & 0x0700) >> 8)
#define MOSCADROWNUMBER(a)   ((WORD)(a & 0xFF))
#define MOSCADOFFSET(a,b,c) (a*2048 + b*256 + c)   // table, col, row

#define ETH_PREAMBLE_LENGTH      6  // 6 bytes

class CMODMessage;

// Declares the modbus communication state-engine, which will simulate 
// a real CPU's actions.
class CMOD232CommsProcessor : public SimulationSerialPort
{
public:
   DECLARE_DYNAMIC(CMOD232CommsProcessor)

   CMOD232CommsProcessor(LPCTSTR portNameShort, 
                     DWORD  baud, 
                     DWORD byteSize, 
                     DWORD parity, 
                     DWORD stopBits,
                     DWORD rts,
                     int responseDelay,
                     BOOL MOSCADchecks,
                     BOOL modifyThenRespond,
                     BOOL disableWrites);

	virtual ~CMOD232CommsProcessor();

   BOOL ProcessData(const CHAR *pBuffer, DWORD numBytes);
   void SetPDUSize(DWORD size) {m_PDUSize = size;};
   void SetEmulationParameters(BOOL moscadChecks, BOOL modifyThenRespond, BOOL disableWrites);
   
   // over-ridden functions
   void RSDataDebugger(const BYTE * buffer, LONG length, int transmit);
   void RSStateChanged(DWORD state);
   void RSDataMessage(LPCTSTR msg);
   void RSModemStatus(DWORD status);

   virtual BOOL OnProcessData(const CHAR *pBuffer, DWORD numBytes, BOOL *discardData);

   BOOL  LoadRegisters();
   BOOL  SaveRegisters();

   static BOOL  LoadRegistersIMP();
   static BOOL  SaveRegistersIMP();

   virtual void ActivateStationLED(LONG stationID);
   virtual BOOL StationIsEnabled(LONG stationID);

private:
   // helpers
   BOOL TestMessage(CMODMessage &modMsg,
                    WORD &startRegister, 
                    WORD &endRegister, 
                    WORD &MBUSerrorCode,
                    WORD &requestMemArea,
                    WORD &numBytesInReq

                   );

   // data/members
public:
   UINT  m_responseDelay;
   BOOL  m_MOSCADchecks;      // MOSCAD
   BOOL  m_disableWrites;     // simple test

   BOOL  m_modifyThenRespond;
   CRS232Noise m_NoiseSimulator;

   LPCTSTR ProtocolName() { return(m_protocolName);};

   // noise string (raw incomming buffer)
   BYTE  m_noiseBuffer[MAX_MODBUS_MESSAGELEN*2];  // keep space for 2 messages
   DWORD m_noiseLength;

private:
 CString m_protocolName;


   DWORD m_PDUSize;
   CRITICAL_SECTION stateCS;
};


// ----------------------------------------------------------------------
// class CMODMessage declares a MODBUS RTU protcol frame.
//
class CMODMessage : public CObject
{
public:
   CMODMessage(const CHAR * pMessageRX, DWORD len);
   CMODMessage(const CMODMessage & oldMODMessage); //copy constructor to assist in building the response message!

   BYTE stationID;
   BYTE functionCode;
   WORD overalLen;      // telegram data length
   WORD address;
   WORD count;          // # of data words?
   WORD byteCount;      // # of items, (if In/Outputs), then it is the # of bits.
   BYTE coilByteCount;
   WORD totalLen;
   // mask
   WORD m_andMask;
   WORD m_orMask;

   // Ethernet
   BYTE * preambleLenPtr; 
   BYTE * dataByteCountPtr;
   BOOL  frameEthernet;
   BOOL  frameASCII;
   WORD  m_frameLength;  // the length as specified in the TCP headder field
   BOOL  m_packError;    // funny ASCII characters or bad data fields were found


   CHAR buffer[MAX_RX_MESSAGELENGTH + MODBUS_FRAME_LENGTH_MAX];     //TX/RX buffer
   BYTE * dataPtr;
   
   // methods
   CHAR * BuildMessagePreamble(BOOL error=FALSE,WORD errorCode=0);
   CHAR * BuildMessageEnd();
   WORD GetAddressArea(WORD classCode); 
   BOOL CRCOK() { return (crcCheckedOK);};
   static BOOL SetEthernetFrames(BOOL ethernetFrames = TRUE);

   WORD GetEthernetTransNum() { return(m_EthernetTransNum);}
private:
   BOOL crcCheckedOK;
   static BOOL m_protocolEthernet;
   WORD  m_EthernetTransNum;
};

#endif // !defined(AFX_232COMMSPROCESSOR_H__FDE99A84_435C_4094_826C_175DBDB6E61F__INCLUDED_)
