/////////////////////////////////////////////////////////////////////////////
//
// FILE: SimPort.cpp : headder file
//
// See _README.CPP
//
// interface for the SimulationSerialPort class.
/////////////////////////////////////////////////////////////////////////////

#if !defined(SIMPORT_H__265B8E5E_6549_42d7_98C5_99CB5BD69127_INCLUDED)
#define SIMPORT_H__265B8E5E_6549_42d7_98C5_99CB5BD69127_INCLUDED


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// modbus stuff here 
#define MAX_MODBUS_MESSAGELEN  256  // RX of a command
#define MODBUS_HEADDER_LEN       6  // STN CMD and Address and Length fields
#define MODBUS_CRC_LEN           2  //
#define MODBUS_NORMAL_LEN        (MODBUS_HEADDER_LEN + MODBUS_CRC_LEN)

class SimulationSerialPort : public CRS232Port
{
public:

   DECLARE_DYNAMIC(SimulationSerialPort)

	SimulationSerialPort();

   static LONG CalculateReadAheadLength(const BYTE* rxBuffer, DWORD rxBufferIndex);

   // implemented in the parent class
   virtual void ActivateStationLED(LONG stationID) = NULL;
   virtual BOOL StationIsEnabled(LONG stationID) = NULL;

   virtual LPCTSTR ProtocolName() = NULL;
   // diagnostic

#ifdef _DEBUG
   VOID Dump(CDumpContext& dc) const;
#endif
};

#endif // SIMPORT_H__265B8E5E_6549_42d7_98C5_99CB5BD69127_INCLUDED