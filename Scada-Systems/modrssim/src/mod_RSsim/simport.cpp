/////////////////////////////////////////////////////////////////////////////
//
// FILE: simport.cpp : implementation file
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
// Implementation of the SimulationSerialPort wrapper class for CRS232Port.
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SimPort.h"
#include "abcommsprocessor.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC( SimulationSerialPort, CRS232Port);

//constructor
SimulationSerialPort::SimulationSerialPort()
{

}


#ifdef _DEBUG
void SimulationSerialPort::Dump(CDumpContext& dc) const
{
   // call the base class first
   CObject::Dump(dc);

   // dump our object to the debuggers output
   // all important members can be dumped at this stage.
   dc << "Simulator 232 Port: " << "\n";
} // Dump
#endif // _DEBUG

// ----------------------------- CalculateReadAheadLength ---------------------
// Calculates amount of bytes to read int order to recieve a full modbus message, 
// based on the 1st 6 bytes. if less than 6 bytes have been read, the function 
// returns the difference.
//
LONG SimulationSerialPort::CalculateReadAheadLength(const BYTE* rxBuffer, DWORD rxBufferIndex)
{
LONG raLength=0;  // read-ahead length
DWORD numItems;   // Modbus command length

   if (pGlobalDialog->m_selectedProtocol == PROTOCOL_SELMOD232)
   {
      if (rxBufferIndex ==0)
         return(MODBUS_HEADDER_LEN);
      else
      {
         if (rxBufferIndex<MODBUS_HEADDER_LEN)
            return(MODBUS_HEADDER_LEN - rxBufferIndex); // read at least 6 bytes before we continue
         else
         {
            // OK we have at least 6 bytes of data in our buffer.

            // Get clever, and look at the command we think we may have gotten
            // remember to do range-checking all of the time
            // This only works under normal comms situations, so give 
            // me a break on the optimisation side.
            raLength = MAX_MODBUS_MESSAGELEN;
            switch (rxBuffer[1])
            {
               case MOD_READ_COILS:
               case MOD_READ_DIGITALS:
                  raLength = MODBUS_CRC_LEN;   //just the CRC follows on this one
                  break;
               case MOD_WRITE_MULTIPLE_COILS:
                  //numItems = rxBuffer[5] + (rxBuffer[6]<<8);
                  numItems = rxBuffer[5] + (rxBuffer[4]<<8);
                  raLength = MODBUS_CRC_LEN + numItems/8 + 1;   // X-data bytes + CRC + 1 length 
                  break;
               case MOD_WRITE_SINGLE_COIL:
                  //numItems = rxBuffer[5] + rxBuffer[6]<<8;
                  raLength = MODBUS_CRC_LEN + 2;   // CRC + 2 data bytes 
                  break;
               case MOD_READ_REGISTERS:
               case MOD_READ_HOLDING:  //analogs are also 16-bit
               case MOD_READ_EXTENDED:
                  raLength = MODBUS_CRC_LEN;   //just the CRC follows on this one
                  break;
               case MOD_WRITE_HOLDING:
               case MOD_WRITE_EXTENDED:
                  //numItems = rxBuffer[5] + (rxBuffer[6]<<8);
                  numItems = rxBuffer[5] + (rxBuffer[4]<<8);
                  raLength = MODBUS_CRC_LEN + 1 + (numItems*2);   // CRC + 2 data bytes 
                  break;
               case MOD_WRITE_SINGLEHOLDING:
                  numItems = 1;  // writes 1 register
                  raLength = MODBUS_CRC_LEN + 1 + (numItems*2);   // CRC + 2 data bytes 
                  break;
               case MOD_MASKEDWRITE_HOLDING:
                  numItems = 1;
                  raLength = MODBUS_CRC_LEN + 1 + (numItems*2*2); // holds 2 masks
               default:
                  break;
            }
            if (raLength >= MAX_MODBUS_MESSAGELEN)
            {
               raLength = MAX_MODBUS_MESSAGELEN;
               //error occured. maybe we should spit out an exception response further along?
            }
         }
         return (raLength);
      }
      return (MODBUS_HEADDER_LEN);  // we should never get here
   }
   else
   {
      // Allen-Bradley DF1 (and JOY)
      CABCommsProcessor *pAB;
      if (pGlobalDialog->m_pServerRS232Array->GetUpperBound() == -1)
         return(1);
      pAB = (CABCommsProcessor *)(pGlobalDialog->m_pServerRS232Array->GetAt(0));
      if (pAB->IsKindOf(RUNTIME_CLASS(CABCommsProcessor)))
      {
         return(pAB->GetRALength());
      }
      return(18);
   }
   return(1);
} // CalculateReadAheadLength
