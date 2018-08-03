/* Classic Ladder Project */
/* Copyright (C) 2001-2014 Marc Le Douarain */
/* http://www.multimania.com/mavati/classicladder */
/* http://www.sourceforge.net/projects/classicladder */
/* May 2010 */
/* -------------------------------------- */
/* Serial low-level functions for Windows */
/* -------------------------------------- */

/* This library is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU Lesser General Public */
/* License as published by the Free Software Foundation; either */
/* version 2.1 of the License, or (at your option) any later version. */

/* This library is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU */
/* Lesser General Public License for more details. */

/* You should have received a copy of the GNU Lesser General Public */
/* License along with this library; if not, write to the Free Software */
/* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "serial_common.h"
void DoPauseMilliSecs( int MilliSecsTime );

char PortIsOpened[ NBR_SERIAL_PORTS ];
HANDLE hSerialComm[ NBR_SERIAL_PORTS ];
//int ResponseSizeExpected = 0;
char DebugLevelMsgs[ NBR_SERIAL_PORTS ];
char UseRtsSignal[ NBR_SERIAL_PORTS ];

void SerialGeneralInit( void )
{
	int ScanPort;
	for( ScanPort=0; ScanPort<NBR_SERIAL_PORTS; ScanPort++ )
	{
		PortIsOpened[ ScanPort ] = FALSE;
		hSerialComm[ ScanPort ] = INVALID_HANDLE_VALUE;
		DebugLevelMsgs[ ScanPort ] = 0;
		UseRtsSignal[ ScanPort ] = 0;
	}
}

char SerialOpen( int PortIndex, const char * SerialPortName, int SerialSpeed, int DataBits, int Parity, int StopBits )
{
	char tcNomPort[ 32 ];
	char tcInitPort[ 256 ];
	DCB dcb;

	if ( PortIndex<0 || PortIndex>=NBR_SERIAL_PORTS )
		return FALSE;
	/* if port already opened => close it before */
	if ( PortIsOpened[PortIndex] )
		SerialClose( PortIndex );

	sprintf(tcNomPort, "\\\\.\\%s",SerialPortName);
	hSerialComm[PortIndex] = CreateFile( tcNomPort,
									GENERIC_READ | GENERIC_WRITE,
									0, 0, OPEN_EXISTING, 0, 0);
	if ( hSerialComm[PortIndex]!=INVALID_HANDLE_VALUE )
	{
		char ParityChar;
		FillMemory(&dcb, sizeof(dcb), 0);
		dcb.DCBlength = sizeof( dcb );
		switch (Parity)
		{
			case SERIAL_PARITY_NONE:
			default:                       //none
				ParityChar = 'n';
				break;
			case SERIAL_PARITY_ODD:                        //odd
				ParityChar = 'o';
				break;
			case SERIAL_PARITY_EVEN:                        //even
				ParityChar = 'e';
				break;
		}  //end of switch parity
		sprintf(tcInitPort,"%d,%c,%d,%d",SerialSpeed,ParityChar,DataBits,StopBits);
		if (BuildCommDCB( tcInitPort , &dcb))
		{
			dcb.fDtrControl = DTR_CONTROL_ENABLE;
			if ( !UseRtsSignal[PortIndex] )
				dcb.fRtsControl = RTS_CONTROL_ENABLE;
			if (SetCommState( hSerialComm[PortIndex], &dcb ))
			{
				PortIsOpened[PortIndex] = TRUE;
//ADDED TO TEST MONITOR MASTER UNDER WINDOWS (ELSE BLOCK...!)
SerialSetResponseSize( PortIndex, 0, 50 );
printf("Serial port %s successfully opened.\n",SerialPortName);
			}
		}
	}
	if ( !PortIsOpened[PortIndex] )
		printf( "Failed to open serial port %s !!!\n", SerialPortName );

	return PortIsOpened[PortIndex];
}

void SerialClose( int PortIndex )
{
	if ( PortIndex<0 || PortIndex>=NBR_SERIAL_PORTS )
		return;
	if ( PortIsOpened[PortIndex] )
	{
		PortIsOpened[PortIndex] = FALSE;
		CloseHandle( hSerialComm[PortIndex]);
	}
}

char SerialPortIsOpened( int PortIndex )
{
	if ( PortIndex<0 || PortIndex>=NBR_SERIAL_PORTS )
		return FALSE;
	return PortIsOpened[PortIndex];
}

void SerialSetRTS( int PortIndex, int State )
{
	if ( PortIndex<0 || PortIndex>=NBR_SERIAL_PORTS )
		return;
	if ( PortIsOpened[PortIndex] )
	{
		if ( DebugLevelMsgs[PortIndex]>=3 )
			printf( "Set RTS=%d\n", State );
		EscapeCommFunction(hSerialComm[PortIndex], State?SETRTS:CLRRTS);
	}
}

void SerialSend( int PortIndex, char * Buff, int BuffLength )
{
	if ( PortIndex<0 || PortIndex>=NBR_SERIAL_PORTS )
		return;
	if ( PortIsOpened[PortIndex] )
	{
		unsigned long ulNbr;
		if ( UseRtsSignal[PortIndex] )
		{
			SerialSetRTS( PortIndex, 1 );
		}
		if ( DebugLevelMsgs[PortIndex]>=2 )
			printf("Serial writing...\n");
		WriteFile( hSerialComm[PortIndex], Buff, BuffLength, &ulNbr, NULL);
		if ( DebugLevelMsgs[PortIndex]>=2 )
			printf("Writing done!\n");
		if ( UseRtsSignal[PortIndex] )
		{
			// wait until everything has been transmitted
			FlushFileBuffers( hSerialComm[PortIndex] );
//premiers essais avec mon module AVR...
DoPauseMilliSecs( 10 );
			SerialSetRTS( PortIndex, 0 );
		}
	}
}

void SerialSetResponseSize( int PortIndex, int Size, int TimeOutResp )
{
	if ( PortIndex<0 || PortIndex>=NBR_SERIAL_PORTS )
		return;
	if ( PortIsOpened[PortIndex] )
	{
		COMMTIMEOUTS timeouts;
		if ( DebugLevelMsgs[PortIndex]>=2 )
			printf("Serial config...(SizeExpected=%d;TimeOut=%d\n",Size,TimeOutResp);
		/* 
ReadIntervalTimeout:
    Specifies the maximum acceptable time, in milliseconds, to elapse between the arrival of two characters on the communication line.
    In Windows Embedded CE, during a ReadFile operation, the time period begins immediately.
    If the interval between the arrivals of two characters exceeds the time amount specified in ReadIntervalTimeout, the ReadFile operation is completed and buffered data is returned.
A value of zero indicates that interval timeouts are not used.

ReadTotalTimeoutMultiplier:
    Specifies the multiplier, in milliseconds, used to calculate the total timeout period for read operations.
    For each read operation, this value is multiplied by the requested number of bytes to be read.
ReadTotalTimeoutConstant:
    Specifies the constant, in milliseconds, used to calculate the total timeout period for read operations.
    For each read operation, this value is added to the product of the ReadTotalTimeoutMultiplier member and the requested number of bytes.
    A value of zero for the ReadTotalTimeoutMultiplier and ReadTotalTimeoutConstant members indicates that total timeouts are not used for read operations.
*/
		timeouts.ReadIntervalTimeout = 20;
		timeouts.ReadTotalTimeoutMultiplier = 0;
		timeouts.ReadTotalTimeoutConstant = TimeOutResp;
		timeouts.WriteTotalTimeoutMultiplier = 2;
		timeouts.WriteTotalTimeoutConstant = 60;
		SetCommTimeouts( hSerialComm[PortIndex], &timeouts );
//		ResponseSizeExpected = Size;
	}
}

int SerialReceive( int PortIndex, char * Buff, int MaxBuffLength )//, int TimeOutResp )
{
	DWORD NbrCarsReceived = 0;
	if ( PortIndex<0 || PortIndex>=NBR_SERIAL_PORTS )
		return 0;
	if ( PortIsOpened[PortIndex] )
	{
		if ( DebugLevelMsgs[PortIndex]>=3 )
			printf("Serial reading...\n");
		if ( ReadFile( hSerialComm[PortIndex], Buff, MaxBuffLength/*ResponseSizeExpected*/, &NbrCarsReceived, NULL )!=TRUE )
			return 0;
		if ( DebugLevelMsgs[PortIndex]>=3 )
			printf("Serial read %d chars.\n",(int)NbrCarsReceived);
	}
	return NbrCarsReceived;
}

void SerialPurge( int PortIndex )
{
	if ( PortIndex<0 || PortIndex>=NBR_SERIAL_PORTS )
		return;
	if ( PortIsOpened[PortIndex] )
	{
		if ( DebugLevelMsgs[PortIndex]>=2 )
			printf("Serial flush all!\n");
		PurgeComm( hSerialComm[PortIndex], PURGE_RXCLEAR );
	}
}

void SerialConfigDebugAndRts( int PortIndex, int DbgLvl, char RtsToUse )
{
	if ( PortIndex<0 || PortIndex>=NBR_SERIAL_PORTS )
		return;
	DebugLevelMsgs[ PortIndex ] = DbgLvl;
	UseRtsSignal[ PortIndex ] = RtsToUse;
}

