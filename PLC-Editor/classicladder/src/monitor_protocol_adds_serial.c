/* Classic Ladder Project */
/* Copyright (C) 2001-2012 Marc Le Douarain */
/* http://membres.lycos.fr/mavati/classicladder/ */
/* http://www.sourceforge.net/projects/classicladder */
/* February 2011 */
/* ----------------------------------------------------- */
/* Monitor protocol - Addons in case of serial link used */
/* ----------------------------------------------------- */
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
#include <stdlib.h>
#include <string.h>

#include "monitor_protocol_adds_serial.h"
#include "protocol_modbus_master.h"
//#include "monitor_windows_gtk.h"
#include "frames_log_buffers.h"

// Serial frames format: STX,@,...,HEXCRC16a,HEXCRC16b,HEXCRC16c,HEXCRC16d,ETX
// @ address char not used for now. for multi drop lines in the future, you never know ?!
// CRC include @ char. 

// for serial (both slave & master, to permit to have master/slave running at both time with one same executable ! very usefull for debug...)
StrMonitorSerialBuffer MonitorSerialBuffer[ NBR_MONITOR_SERIALS_BUFFERS ];

// Frame pointer given here has first char available (to store first "STX" of the frame)...
// return new length of final frame with encapsulation
int MonitorAddSerialEncapsul( char * Frame, int LgtPureFrame )
{
	int Crc16Value; 
	Frame[ 0 ] = CHAR_STX;
	Frame[ 1 ] = '*'; //address char
	Crc16Value = CRC16( (unsigned char *)&Frame[ 1 ], 1+LgtPureFrame );
	sprintf( &Frame[2+LgtPureFrame], "%04X", Crc16Value );
	Frame[ 2+LgtPureFrame+4 ] = CHAR_ETX;
	Frame[ 2+LgtPureFrame+5 ] = '\0';
	return 2+LgtPureFrame+5;
}

// return 1, if frame valid
char TestSerialEncapsul( char * FrameRecv, int LgtFrameRecv )
{
	char cFrameOk = 0;
	if ( LgtFrameRecv>1+4+1 )
	{
		int CrcReceived;
		int CrcCalc = CRC16( (unsigned char *)&FrameRecv[ 1 ], LgtFrameRecv-1-4-1 );
		FrameRecv[ LgtFrameRecv ] = '\0';
		sscanf( &FrameRecv[ LgtFrameRecv-5 ], "%X", &CrcReceived );
printf("Debug CRC16 frames values... Calc=%X, Received=%X\n", CrcCalc, CrcReceived );
		if ( CrcCalc==CrcReceived )
			cFrameOk = 1;
//		else
//			printf("#################### CRC RECEIVED FRAME ERROR !!!... Calc=%X, Received=%X\n", CrcCalc, CrcReceived );
	}
	return cFrameOk;
}

// when reading on serial, each time we get part of the frame...
// we concatene chars received here, and extract interesting pure frame at the end !
// return >0 with pure frame length, if complete and correct serial frame seen
// we are not able to return more than one complete frame ! (but can have another one just starting)
int MonitorSerialDatasReceivedIsEnd( int SerialChannel, char * SerialDatasRecv, int LgtSerialDatasRecv, char * CompletePureFrameToStoreThere )
{
	StrMonitorSerialBuffer *pSerialBuffer = &MonitorSerialBuffer[ SerialChannel ];
	int LgtCompletePureFrameSeen = 0;
	char *pCharRead = SerialDatasRecv;
	while( LgtSerialDatasRecv>0 )
	{
		// first frame char received ?
		if ( *pCharRead==CHAR_STX )
		{
			pSerialBuffer->CurrLgtRecv = 0; // now starting to receive a new frame...
			FrameLogString( (SerialChannel==MONITOR_SERIAL_MASTER)?FRAMES_LOG_MONITOR_MASTER:FRAMES_LOG_MONITOR_SLAVE_SERIAL, '!', "--- START OF SERIAL MONITOR FRAME SEEN. ---" );
/*#ifdef GTK_INTERFACE
			if ( SerialChannel==MONITOR_SERIAL_MASTER )
				MonitorWindowAddText( FRAMES_LOG_MONITOR_MASTER, "monmaster: --- START OF SERIAL MONITOR FRAME SEEN. ---\n" );
			else
#endif
				printf("--- START OF SERIAL MONITOR FRAME SEEN. ---\n" );*/
		}
		if ( pSerialBuffer->CurrLgtRecv>=0 )
		{
			if ( pSerialBuffer->CurrLgtRecv<LGT_MONITOR_SERIAL_BUFFER )
			{
				pSerialBuffer->BuffRecv[ pSerialBuffer->CurrLgtRecv++ ] = *pCharRead;
				if ( *pCharRead==CHAR_ETX )
				{
					if ( TestSerialEncapsul( pSerialBuffer->BuffRecv, pSerialBuffer->CurrLgtRecv ) )
					{
						memcpy( (void*)CompletePureFrameToStoreThere, (void*)&pSerialBuffer->BuffRecv[2], pSerialBuffer->CurrLgtRecv );
						LgtCompletePureFrameSeen = pSerialBuffer->CurrLgtRecv-(2+4+1);
						// Else for sure JSON will not really like to find datas of the end of the serial frame! ;-)
						CompletePureFrameToStoreThere[ LgtCompletePureFrameSeen ] = '\0';
					}
					else
					{
						FrameLogString( (SerialChannel==MONITOR_SERIAL_MASTER)?FRAMES_LOG_MONITOR_MASTER:FRAMES_LOG_MONITOR_SLAVE_SERIAL, '!', "#################### CRC RECEIVED FRAME ERROR !!!" );
/*#ifdef GTK_INTERFACE
						if ( SerialChannel==MONITOR_SERIAL_MASTER )
							MonitorWindowAddText( FRAMES_LOG_MONITOR_MASTER, "monmaster: ##### CRC RECEIVED FRAME ERROR !!! #####\n" );
						else
#endif
							printf("#################### CRC RECEIVED FRAME ERROR !!!\n" );*/
					}
					pSerialBuffer->CurrLgtRecv = -1; // no frame under receive.
//NO DON'T, TO READ IF ANOTHER FRAME STARTING AFTER THIS ONE !!!!! LgtSerialDatasRecv = 1; //to stop frame reading
				}
			}
			else
			{
				FrameLogString( (SerialChannel==MONITOR_SERIAL_MASTER)?FRAMES_LOG_MONITOR_MASTER:FRAMES_LOG_MONITOR_SLAVE_SERIAL, '!', "#################### ERROR BUFFER RECEIVE OVERFLOW - NOT SEEN END OF FRAME !!!..." );
/*#ifdef GTK_INTERFACE
				if ( SerialChannel==MONITOR_SERIAL_MASTER )
					MonitorWindowAddText( FRAMES_LOG_MONITOR_MASTER, "monmaster: ##### ERROR BUFFER RECEIVE OVERFLOW - NOT SEEN END OF FRAME !!! #####\n" );
				else
#endif
					printf("#################### ERROR BUFFER RECEIVE OVERFLOW - NOT SEEN END OF FRAME !!!...\n" );*/
//ADDED !!!!!!
				pSerialBuffer->CurrLgtRecv = -1; // no frame under receive.
				LgtSerialDatasRecv = 1; //to stop characters reading for this time
			}
		}
		pCharRead++;
		LgtSerialDatasRecv--;
	}
	return LgtCompletePureFrameSeen;
}

