// MTransportProvider.cpp: implementation of the MTransportProvider class.
//
//////////////////////////////////////////////////////////////////////

#pragma warning(disable:4786) 
#include "stdafx.h"
#include "TestSvr.h"
#include "MTransportProvider.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void CMediumRoot::DebugFrame(CProtFrame &frame, const DWORD length)
{
   CDataBuffer buff1(frame);
   buff1.SetSize(length);
   CString text;
   CSimplePrinter hexPrinter;

   buff1.Print(text, hexPrinter);
   if (frame.GetDirection() == CProtFrame::EDirectionOUT)
      printf("TX:%s\n", text);
   else
      printf("RX:%s\n", text);
}

/////////////////////////////////////////////////////////////////////
//
CSerialRoot::CSerialRoot() : CMediumRoot()
{
   m_hPort = INVALID_HANDLE_VALUE;
}

bool CSerialRoot::Open(CPortSettingBase& protSettings)
{
CHAR portName[MAX_PORT_NAME];

   // try to open the port
   if (protSettings.portName[0])
   {
      DWORD error;

      GetLongComPortName(protSettings.portName, portName);   // PIP 1823-400-1997
      m_hPort =  CreateFile( portName,                    // PIP 1823-400-1997
                                  GENERIC_READ | GENERIC_WRITE,
                                  (DWORD)NULL,   // exclusive access
                                  NULL,          // no security
                                  OPEN_EXISTING,
                                  FILE_ATTRIBUTE_NORMAL,
                                  NULL           // hTemplate
                                );
      if(m_hPort != INVALID_HANDLE_VALUE)
      {
      COMMTIMEOUTS   toStruct;
         // set up the comms parameters now that we have a handle
         error = SetupComm(m_hPort,
                           (DWORD)2048, (DWORD)2048);
         if(GetCommState(m_hPort, &dcb))
         {
            // Set with our defaults                                
            dcb.BaudRate          = protSettings.dcb.BaudRate;
            dcb.ByteSize          = protSettings.dcb.ByteSize; 
            dcb.fParity           = protSettings.dcb.Parity == NOPARITY ? FALSE : TRUE; 
            // Handshaking
            dcb.Parity            = protSettings.dcb.Parity;
            dcb.fOutxDsrFlow      = FALSE;    //TRUE;     // Stop tx chars if DSR is LOW
            dcb.fDsrSensitivity   = FALSE;

            dcb.fRtsControl       = RTS_CONTROL_DISABLE;//RTS_CONTROL_ENABLE;
            dcb.fOutxCtsFlow      = FALSE;

            dcb.StopBits          = protSettings.dcb.StopBits;
            dcb.fAbortOnError     = (DWORD)TRUE;
            dcb.fBinary           = TRUE;
            dcb.fTXContinueOnXoff = FALSE;
            dcb.fOutX             = FALSE;    // Disable XON flow control for tx
            dcb.fInX              = FALSE;    // Disable XON flow control for rx
            dcb.fErrorChar        = 0;
            dcb.fNull             = 0;
            dcb.fDummy2           = 0;

            error = SetCommState(m_hPort,  &dcb );

            GetCommTimeouts(m_hPort, &toStruct);

            toStruct.ReadTotalTimeoutMultiplier  = 20;
            toStruct.ReadTotalTimeoutConstant    = 500;
            toStruct.WriteTotalTimeoutMultiplier = 20;
            toStruct.WriteTotalTimeoutConstant   = 500;
            toStruct.ReadIntervalTimeout         = 0;

            SetCommTimeouts( m_hPort, &toStruct );
         }
      }
      else
         return(false);
   }
   else
      return(false);
   return(true);
}


bool CSerialRoot::Close()
{
   if(m_hPort != INVALID_HANDLE_VALUE)  // cannot close without open
      CloseHandle( m_hPort);
   return(true);
}


bool CSerialRoot::SendRaw(CProtFrame &frame)
{
   ASSERT(m_hPort != INVALID_HANDLE_VALUE); 
   DWORD dwWritten;
   DWORD writeLen = frame.GetSize();

   WriteFile(m_hPort, frame.GetData(), writeLen, &dwWritten, NULL);
   frame.SetDirection( CProtFrame::EDirectionOUT);
   DebugFrame(frame, dwWritten);

   return(dwWritten == frame.GetSize());
}


bool CSerialRoot::RecvRaw(CProtFrame &frame, const DWORD length)
{
DWORD dwToRead(length), dwRead(0);

   frame.SetSize(dwToRead);
   ReadFile(m_hPort, frame.GetData(), dwToRead, &dwRead, NULL);

DWORD dwErrors;
COMSTAT stat;
   ClearCommError(m_hPort, &dwErrors, &stat);

   frame.SetSize(dwRead);
   frame.SetDirection(CProtFrame::EDirectionIN);
   DebugFrame(frame, dwRead);
   
   return(dwToRead == dwRead);
}


CModbusRTURoot::CModbusRTURoot() : CSerialRoot()
{

}


bool CModbusRTURoot::SendRaw(CProtFrame &aFrame)
{
WORD           crc(0xFFFF);
CProtFrame frame(aFrame);

   // append CRC
   CalcCRC(frame.GetData(), frame.GetSize(), &crc);
   frame.Add(LOBYTE(crc));
   frame.Add(HIBYTE(crc));

   // send raw
   return(CSerialRoot::SendRaw(frame));
}


bool CModbusRTURoot::RecvRaw(CProtFrame &frame, const DWORD length)
{
bool ret;

   ret = CSerialRoot::RecvRaw(frame, length+2);
   frame.ResetCRC();
   // test CRC
   frame.CheckCRC();
   
   // strip the CRC
   if (frame.CRCValid())
      frame.SetSize(frame.GetSize()-2);
   return(ret);
}


void CSerialRoot::Flush()
{
   Sleep(200); // TODO:use timeout
   ASSERT(m_hPort != INVALID_HANDLE_VALUE); 

   if (!PurgeComm(m_hPort  , PURGE_TXCLEAR | PURGE_RXCLEAR))
   {
      printf("Error purge comm port failed!\n");
      ASSERT(0); 
   }
}