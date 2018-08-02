/////////////////////////////////////////////////////////////////////////////
//
// FILE: Message.h : headder file
//
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _MESSAGE_H_HEADDER_FILE
#define _MESSAGE_H_HEADDER_FILE

#define MAX_RX_MESSAGELENGTH  4096     // a big buffer
#define AB_FRAME_LENGTH_MAX   236      // max bytes of data

// protocol specific macros etc here
#define ALLENBRADLEY_STX         0x02
#define ALLENBRADLEY_ETX         0x03
#define ALLENBRADLEY_SOH         0x01
#define ALLENBRADLEY_ENQ         0x05
#define ALLENBRADLEY_ACK         0x06
#define ALLENBRADLEY_DLE         0x10
#define ALLENBRADLEY_NAK         0x15  // disputed 0x0F

#define SIZEOF_TAIL          2
#define MIN_PLC_FILE         3      // Extended File Registers Blocks
#define MAX_PLC_FILE       255

#define ALLENBRADLEY_SLC_CMD                        0x0F     // SLC compatible command
#define ALLENBRADLEY_PLC3_LOGICAL_ADDRESS_INDICATOR 0x2C // For PLC3 ascii addressing
#define ALLENBRADLEY_BIT_WRITE                      0x02        // Non priveleged commands
#define ALLENBRADLEY_WORD_READ                      0xA2  // 0x01
#define ALLENBRADLEY_WORD_WRITE                     0xAA  // 0x00
#define ALLENBRADLEY_LOGIC_ADDRESS_DELIMITER        0xFF      

#define AB_STS_NOERROR                 0x00  // 
#define AB_STS_ILLEGALCMD              0x10  // all errors > 8 have EXT-STS bytes

#define AB_EXTSTS_ILLEGALFIELDVALUE    0x01
#define AB_EXTSTS_CANNOTEXECUTE        0x0E
#define AB_EXTSTS_ILLEGALTYPE          0x11

// zero-based telegram offsets for bytes in the MSG headder
#define AB_SOURCEBYTEOFF         2
#define AB_DESTINATIONBYTEOFF    3
#define AB_COMMANDBYTEOFF        4
#define AB_STATUSBYTEOFF         5
#define AB_TNSBYTEOFF            6
#define AB_FUNCBYTEOFF           8
#define AB_LENGTHFRAMEOFF        9


// Global - Global data
extern BYTE txDLE_STX[SIZEOF_TAIL];
extern BYTE txDLE_ACK[SIZEOF_TAIL];
extern BYTE txDLE_NAK[SIZEOF_TAIL];
extern BYTE txDLE_ETX[SIZEOF_TAIL];
extern BYTE txDLE_ENQ[SIZEOF_TAIL];

extern BYTE CalcBCC(BYTE * lrcBuffPtr,  //  -> pointer to buffer at start of BCC
             DWORD  buffLen				//  -> 
            );

extern LONG CalcMODBUSCRC( BYTE * crcStartPtr,   //  ->
              DWORD  buffLen,       //  ->
              WORD * crc            // <->
             );



// ----------------------------------------------------------------------
// class CProtocolMessage declares the BASE protocol frame.
//
class CProtocolMessage : public CObject
{
public:
   CProtocolMessage(const BYTE * pMessageRX, DWORD len);
   CProtocolMessage(const CProtocolMessage & oldProtMessage); //copy constructor

   BOOL CRCOK() { return (crcCheckedOK);};
   static LONG StripDLEs(CHAR *bufferPtr, DWORD *numBytes);

protected:
   BOOL crcCheckedOK;
};

// ----------------------------------------------------------------------
// class CABMessage declares a Allen-Bradley Full-duplex protocol frame.
//
// message format is:
//                                 ===============================
// DLE STX DST SRC CMD STS TNS TNS FNC SIZ FIL TYP ELE ELS DAT DAT DLE ETX CRC CRC
// 10  02  05  01  0F  00  3E  00  AA  02  0A  89  00  00  3F  00  10  03  D8  0F
//
// FNC - A2 = typed logical read
//     - AA = typed logical write (3 address fields)
// SIZ = size in bytes
// FIL = file #
// TYP - 89 = Integer file.
// ELE = element
// ELS = sub-element
// DAT = data
// DAT = data
//
// The correct response format is:
//
// DLE STX SRC DST CMD STS TNS TNS EXT-STS DLE ETX CRC CRC
// 10  02  01  05  4F  00  00  AA  00      10  03  xx  xx
// CMD is +40H
//
// CRC is calculated from the SRC/DEST thru to the DLE-ETX
//
class CABMessage : public CProtocolMessage
{
public:
   CABMessage(const BYTE * pMessageRX, DWORD len, BOOL dataLayer = TRUE);
   CABMessage(const CABMessage & oldABMessage); //copy constructor to assist in building the response message!

   BYTE m_localStationID;  // our station #

   BYTE stationIDSource;   // MSG source station #
   BYTE stationIDDest;     // MSG station # the message is sent to
   BYTE functionCode;
   BYTE command;
   BYTE transmissionStatus;      // STS byte
   WORD transactionID;
   BYTE transmissionStatusEXT;   // Extended STS byte

   WORD fileType;       //
   WORD fileNum;        // file #
   WORD address;
   WORD subElement;
   WORD count;          // # of data words?
   WORD overalLen;      // telegram data length
   WORD byteCount;      // # of items, (if In/Outputs), then it is the # of bits.
   WORD totalLen;

   WORD appLength;               // length of the application data (minus the ENQ, "TAIL" and CRC)

   CHAR buffer[MAX_RX_MESSAGELENGTH + AB_FRAME_LENGTH_MAX];     //TX/RX buffer
   BYTE * dataPtr;
   
   BOOL m_useBCC;

   // methods
   WORD GetValue(WORD wIndex);
   CHAR * BuildMessagePreamble(BOOL error=FALSE,WORD errorCode=0);
   CHAR * BuildMessageEnd();
   WORD GetAddressArea(WORD classCode); 

   static WORD CalculateReadAheadLength(const BYTE*pBuffer, DWORD length);
   static BOOL Status(const BYTE*pBuffer, DWORD length);
};

#endif // _MESSAGE_H_HEADDER_FILE