/////////////////////////////////////////////////////////////////////////////
//
// FILE: Message.cpp : file
//
// Mod_RSSim (c) Embedded Intelligence Ltd. 1993,2009
// AUTHOR: Conrad Braam.  http://www.plcsimulator.org
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Affero General Public License as
//    published by the Free Software Foundation, either version 3 of the
//    License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Affero General Public License for more details.
//
//    You should have received a copy of the GNU Affero General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "abcommsprocessor.h"
#include "message.h"

// English meanings for Allen-Bradley (Exception) error codes
PCHAR plcErrorLocal[9] =     // local STS (low nibble)
{
   "No error",                            // 0x0
   "DST node out of buffer space",        // 0x1
   "Cannot guarantee delivery",           // 0x2
   "Duplicate token holder detected",     // 0x3
   "Local port is disconnected",          // 0x4
   "Application layer timed out waiting for response",             // 0x5
   "Duplicate node detected",             // 0x6
   "Station is offline",                  // 0x7
   "Hardware fault"                       // 0x8
};  

PCHAR plcErrorRemote[16] =     // remote STS (high nibble)
{
   "No error",                               // 0x00
   "Illegal command or format",              // 0x10
   "Host has a problem and will not comm.",                       // 0x20
   "Remote node host is missing,",                                // 0x30
   "Host could not complete function due to h/w fault",           // 0x40
   "Addressing problem or memory protect rungs",                  // 0x50
   "Function not allowed due to command protection selection",    // 0x60
   "Processor is in program mode",                                // 0x70
   "Compatibility mode file missing or comm. zone problem",       // 0x80
   "Remote node cannot buffer command",              // 0x90
   "xxxx",              // 0xA0
   "Remote node problem due to download",              // 0xB0
   "xxxx",              // 0xC0
   "unused",              // 0xD0
   "unused",              // 0xE0
   "Error code is in EXT-STS"              // 0xF0
};  


// Global - Global data
BYTE txDLE_STX[SIZEOF_TAIL] =
{
   ALLENBRADLEY_DLE,
   ALLENBRADLEY_STX
};

BYTE txDLE_ACK[SIZEOF_TAIL] =
{
   ALLENBRADLEY_DLE,
   ALLENBRADLEY_ACK
};

BYTE txDLE_NAK[SIZEOF_TAIL] =
{
   ALLENBRADLEY_DLE,
   ALLENBRADLEY_NAK
};

BYTE txDLE_ETX[SIZEOF_TAIL] =
{
   ALLENBRADLEY_DLE,
   ALLENBRADLEY_ETX
};

BYTE txDLE_ENQ[SIZEOF_TAIL] =
{
   ALLENBRADLEY_DLE,
   ALLENBRADLEY_ENQ
};

// -------------------------------- CalcBCC ----------------------------
// Calculate Allen-Bradley specified BCC code
// the BCC is a 2'scomp sum of all bytes in the message frame excluding DLE-STX and DLE-ETX
BYTE CalcBCC(BYTE * lrcBuffPtr,    //  -> pointer to buffer at start of BCC
             DWORD  buffLen        //  -> 
            )
{
BYTE bcc=0;

   CalcLRC(lrcBuffPtr,    //  -> pointer to buffer at start of LRC
           buffLen,       //  -> 
           FALSE,     //  -> Calc 2's Compliment also (last call)
           &bcc            // <-> 
            );
   bcc = 256-bcc;
   return(bcc);
} // CalcBCC


/////////////////////////////////////////////////////////////////////////////////
// CProtocolMessage implementation
/////////////////////////////////////////////////////////////////////////////////
CProtocolMessage::CProtocolMessage(const BYTE * pMessageRX, DWORD len)
{

}
CProtocolMessage::CProtocolMessage(const CProtocolMessage & oldProtMessage)
{ //copy constructor

}



/////////////////////////////////////////////////////////////////////////////////
// CABMessage implementation
/////////////////////////////////////////////////////////////////////////////////


// ----------------------- constructor -----------------------------------
CABMessage::CABMessage(const BYTE * pMessageRX, DWORD len, BOOL dataLayer) 
      : CProtocolMessage(pMessageRX, len)
{
BYTE *crcStartPtr = (BYTE*)pMessageRX;
WORD  crc = 0;
BYTE bcc;
WORD *crcPtr;
WORD *bccPtr;
DWORD appStartOffset = 2; // data layer telegram
DWORD addressWidth, elementWidth;

   transactionID = -1;
   transmissionStatus = 0;
   transmissionStatusEXT = 0;
   if (!dataLayer)
   {
      appStartOffset=0; // this is an app layer telegram
      len+=2;
   }
   ASSERT(len > 2);
   ASSERT(len < sizeof(buffer)); 
   memset(buffer, 0xCD, sizeof(buffer));
   memcpy(&buffer[2], &pMessageRX[appStartOffset], len);

   // double DLE stripping here
   if (dataLayer)
      StripDLEs(buffer, &len);
   totalLen = (WORD)len;

   // at this point we have a data layer message
   crcCheckedOK = FALSE;

   // break down the msg
   count = 0;
   if ((len>2) && (dataLayer))
   {
      if (buffer[0] != ALLENBRADLEY_DLE)
         return;
      if ((buffer[1] != ALLENBRADLEY_SOH )&&
          (buffer[1] != ALLENBRADLEY_STX )&&
          (buffer[1] != ALLENBRADLEY_ACK )&&
          (buffer[1] != ALLENBRADLEY_NAK )&&
          (buffer[1] != ALLENBRADLEY_ENQ )
         )
         return;
      // we seem to have the beginning of the message, parse it
   }
   if (len > 3)
   {
      stationIDDest = (BYTE)buffer[AB_SOURCEBYTEOFF];          // Dest addr
      stationIDSource = (BYTE)buffer[AB_DESTINATIONBYTEOFF];   // Source addr
   }
   if (len >4)
      command = buffer[AB_COMMANDBYTEOFF];       // may be 0x0F other values are not processed
   if (len >5)
      transmissionStatus = buffer[AB_STATUSBYTEOFF];
   if (len >=7)
   {
      transactionID = (buffer[6]) + (buffer[7]<<8);
   }
   if (len >8)
      functionCode = buffer[8];
   if (len > 9)
      count = (BYTE)buffer[9];   // # bytes to be read (/2 to get integers)
   if (len > 10)
      fileNum = (BYTE)buffer[10];
   if (len >11)
      fileType = (BYTE)buffer[11];
   addressWidth = 1;
   elementWidth = 1;
   if (len >12)
   {
      address = (BYTE)buffer[12];
      if (address == 0xFF)
      {
         addressWidth=3;
         address = buffer[12+1] + (buffer[13+1]<<8);
         address = SwapBytes(address);
      }
   }
   if (len >=(13+addressWidth))   // 14
   {
      subElement = buffer[12+addressWidth];
      if (subElement == 0xFF)
      {
         elementWidth=3;
         subElement = buffer[12+addressWidth+1] + (buffer[13+addressWidth+1]<<8);
         subElement = SwapBytes(subElement);
      }

   }
   if (len >=(13+addressWidth+elementWidth))  // 15
      dataPtr = (BYTE*)&buffer[12+addressWidth+elementWidth];

   if (dataLayer) // app messages will not have a CRC byte anyway
   {
      //check the CRC at this point
      if (pGlobalDialog->m_useBCC)
      {
         if (len >= 17)
         {
            bccPtr = (WORD*)&buffer[totalLen-1];//count + 16];
            crcStartPtr = (BYTE*)&buffer[2];

            bcc = CalcBCC(crcStartPtr, totalLen-6);      // Only one buffer to calc crc of
            overalLen = count+16 + AB_BCC_LEN;
            crcCheckedOK = (bcc==*bccPtr);
         }
      }
      else
      {
         if (len >= 18)
         {
         CHAR ETXBuff[2];
            crcPtr = (WORD*)&buffer[totalLen-2];//count + 16];
            crcStartPtr = (BYTE*)&buffer[2];

            crc = 0x0;
            CalcCRC(crcStartPtr, totalLen-6, &crc);      // Only one buffer to calc crc of
            ETXBuff[0] = 0x03;
            CalcCRC((BYTE*)&ETXBuff[0], 1, &crc);      // include the ETX
            overalLen = count+16 + AB_CRC_LEN;
            crcCheckedOK = (crc==*crcPtr);
         }
      }
   }

   if (!crcCheckedOK)
      return;
   else
   {
      crc = 0xffff;
   }
} // CABMessage

// --------------------------- CABMessage --------------------------
// PURPOSE: copy constructor used to build responses, does not actually 
// copy the message.
CABMessage::CABMessage(const CABMessage & oldABMessage) 
    : CProtocolMessage(oldABMessage)
{
      memset(buffer, 0xCD, sizeof(buffer));
      //Copy in common stuff from both messages here!
      m_localStationID = oldABMessage.m_localStationID;
      stationIDSource    = oldABMessage.stationIDSource;
      stationIDDest    = oldABMessage.stationIDDest;
      functionCode = oldABMessage.functionCode;
      fileType = oldABMessage.fileType;
      address = oldABMessage.address;       // where to copy data from
      byteCount = oldABMessage.byteCount;   // length of data to copy
      
      overalLen = 0;   //New message so 0 for now!
      
      dataPtr = (BYTE*)buffer; //Nice a fresh pointer to the beginning!
}

// ------------------------- CalculateReadAheadLength -------------------------
// static function
WORD CABMessage::CalculateReadAheadLength(const BYTE*pBuffer, DWORD length)
{
   //if (length < 2)
   //   return(2);  // DLE ACK/NAK or DLE/STX in our case
   // look at the rest of the telegram
   return(1);
} // CalculateReadAheadLength

// -------------------------------- Status ----------------------------
// static function
BOOL CABMessage::Status(const BYTE*pBuffer, DWORD length)
{
   if (length >6)
      return(pBuffer[5]);  // STS byte

   return(FALSE);
} // CalculateReadAheadLength

// ------------------------------- GetValue -------------------------
//
WORD CABMessage::GetValue(WORD wIndex)
{
WORD *pData = (WORD*)dataPtr;
   pData+=wIndex;
   return(*pData);
}

// ------------------------------- StripDLEs ------------------------
LONG CProtocolMessage::StripDLEs(CHAR *bufferPtr, DWORD *numBytes)
{
char *pSourceData= bufferPtr;
char *pDestData = bufferPtr;
BOOL stripDLE = FALSE;
int count = *numBytes;
int bytesXferred=0;
int dleCount = 0;
   //if (*numBytes <= 20)
      return(0);
   while (count>0)
   {
      if (*pSourceData == 0x10)  // if current byte is a DLE
         stripDLE = TRUE;        // flag that byte was a DLE
      else
         stripDLE = FALSE;
      // copy current byte
      *pDestData++ = *pSourceData++;
      bytesXferred++;

      if ((stripDLE)&&(*pSourceData == 0x10))
      {
         *pSourceData++;   // skip the byte and clear the flag
         dleCount++;
         count--;
         stripDLE = FALSE;
      }
      count--;
   }
   *numBytes = bytesXferred;
   return(dleCount);
}


// ------------------------------ BuildMessagePreamble -------------------------
// PURPOSE: Builds the STN,FN and LEN bytes of the telegram.
// on completion dataPtr pointsto where the data must be packed in (if any)
CHAR * CABMessage::BuildMessagePreamble(BOOL error, WORD errorCode)
{
WORD  crc = 0;
WORD  dataLength=6;

   // swap the station IDs
   buffer[2] = stationIDSource;
   buffer[3] = stationIDDest;
   //
   buffer[AB_COMMANDBYTEOFF] |= 0x40;    // CMD byte
   buffer[5] = (BYTE)errorCode; // STS byte
   if (error)
   {
      buffer[8] = (BYTE)(errorCode>>8); //EXT-STS
      dataLength++;
   }
   // byte 6 & 7 contain the sender's TNS value
   byteCount = 0;
   totalLen = dataLength + 6;//13;  6+6=12 bytes

   return(buffer);
} // BuildMessagePreamble

  /*
// ------------------------------ BuildMessageEnd -------------------------------
// PURPOSE: glue a CRC onto the end of the message
// totalLen must be = the full telegram length (+CRC) when this is called.
CHAR * CABMessage::BuildMessageEnd()
{
WORD length;
BYTE *pCrcStart = (BYTE*)buffer;
WORD crc = 0xFFFF;
BYTE *crcPtr;

   // Add the CRC bytes
   length = totalLen - AB_CRC_LEN; //calc the CRC of all bytes but the 2 CRC bytes

   CalcCRC(pCrcStart, length, &crc);
   crcPtr = (BYTE*)&buffer[length];
   *(WORD *)crcPtr = crc;

   return (buffer);
} // BuildMessageEnd
*/

// ------------------------------ GetAddressArea --------------------
// Returns:    A supported MEM area index for any MOD address class
// Parameter:  A modbus command (e.g. 3 =read holding register)
//
WORD CABMessage::GetAddressArea(WORD classCode //  modbus command byte
                                )
{
   /*
   switch(classCode)
   {
      // read commands 
      case MOD_READ_COILS     : return(0); break;
      case MOD_READ_DIGITALS  : return(1); break;
      case MOD_READ_REGISTERS : return(2); break;   
      case MOD_READ_HOLDING   : return(3); break;
      case MOD_READ_EXTENDED  : return(4); break;
      // write commands      
      case MOD_WRITE_HOLDING        : return(3); break;
      case MOD_WRITE_SINGLEHOLDING  : return(3); break;
      case MOD_WRITE_SINGLE_COIL    : return(0); break;
      case MOD_WRITE_MULTIPLE_COILS : return(0); break;
      case MOD_WRITE_EXTENDED       : return(4); break;
   }
   */
   return(3); //Default here for now, Should never get here anyways!

} // GetAddressArea
