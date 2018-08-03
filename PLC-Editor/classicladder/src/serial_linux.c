/* Classic Ladder Project */
/* Copyright (C) 2001-2014 Marc Le Douarain */
/* http://www.multimania.com/mavati/classicladder */
/* http://www.sourceforge.net/projects/classicladder */
/* August 2005 */
/* ------------------------------------ */
/* Serial low-level functions for Linux */
/* ------------------------------------ */

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

#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h> 
#include <sys/ioctl.h>

#include "serial_common.h"
void DoPauseMilliSecs( int MilliSecsTime );

int SerialSpeedList[ ] = { 300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 0 };
int SerialBaudCorres[ ] = { B300, B600, B1200, B2400, B4800, B9600, B19200, B38400, B57600, B115200, 0 };

char PortIsOpened[ NBR_SERIAL_PORTS ];
int fd[ NBR_SERIAL_PORTS ];
struct termios oldtio[ NBR_SERIAL_PORTS ];
struct termios newtio[ NBR_SERIAL_PORTS ];
char DebugLevelMsgs[ NBR_SERIAL_PORTS ];
char UseRtsSignal[ NBR_SERIAL_PORTS ];

void SerialGeneralInit( void )
{
	int ScanPort;
	for( ScanPort=0; ScanPort<NBR_SERIAL_PORTS; ScanPort++ )
	{
		PortIsOpened[ ScanPort ] = 0;
		fd[ ScanPort ] = -1;
		DebugLevelMsgs[ ScanPort ] = 0;
		UseRtsSignal[ ScanPort ] = 0;
	}
}

/* a litle doc...
c_cc[VTIME] sets the character timer, and c_cc[VMIN] sets the minimum number of characters to receive before satisfying the read.

If MIN > 0 and TIME = 0, MIN sets the number of characters to receive before the read is satisfied. As TIME is zero, the timer is not used.
If MIN = 0 and TIME > 0, TIME serves as a timeout value. The read will be satisfied if a single character is read, or TIME is exceeded (t = TIME *0.1 s). If TIME is exceeded, no character will be returned.
If MIN > 0 and TIME > 0, TIME serves as an inter-character timer. The read will be satisfied if MIN characters are received, or the time between two characters exceeds TIME. The timer is restarted every time a character is received and only becomes active after the first character has been received.
If MIN = 0 and TIME = 0, read will be satisfied immediately. The number of characters currently available, or the number of characters requested will be returned. According to Antonino (see contributions), you could issue a fcntl(fd, F_SETFL, FNDELAY); before reading to get the same result. 
*/

char SerialOpen( int PortIndex, const char * SerialPortName, int SerialSpeed, int DataBits, int Parity, int StopBits )
{
	if ( PortIndex<0 || PortIndex>=NBR_SERIAL_PORTS )
		return 0;
	/* if port already opened => close it before */
	if ( PortIsOpened[ PortIndex ] )
		SerialClose( PortIndex );

	/* open the device to be non-blocking (read will return immediatly) */
	fd[PortIndex] = open( SerialPortName, O_RDWR | O_NOCTTY | O_NDELAY/*don't wait DTR*/ );
	if (fd[PortIndex] >=0)
	{
		int BaudRate = -1;
		int ScanBaudRate = 0;
		// EMC addon, usefull ?
		fcntl(fd[PortIndex], F_SETFL, O_RDWR | O_NOCTTY ); /* perform blocking reads */
		while( BaudRate==-1 && SerialSpeedList[ ScanBaudRate ]>=0 )
		{
			if ( SerialSpeedList[ ScanBaudRate ]==SerialSpeed )
				BaudRate = SerialBaudCorres[ ScanBaudRate ];
			else
				ScanBaudRate++;
			
		}
		if ( BaudRate!=-1 )
		{        
			long DATABITS;
			long STOPBITS;
			long PARITYON;
			long PARITY;

printf("Serial%d: config %s speed=%d data=%d, parity=%d, stop=%d\n", PortIndex, SerialPortName, SerialSpeed, DataBits, Parity, StopBits );
			// Nice EMC addons for more serial parameters !
			switch (DataBits)
			{
				case 8:
				default:
					DATABITS = CS8;
					break;
				case 7:
					DATABITS = CS7;
					break;
				case 6:
					DATABITS = CS6;
					break;
				case 5:
					DATABITS = CS5;
					break;
			}  //end of switch data_bits
			switch (StopBits)
			{
				case 1:
				default:
					STOPBITS = 0;
					break;
				case 2:
					STOPBITS = CSTOPB;
					break;
			}  //end of switch stop bits
			switch (Parity)
			{
				case SERIAL_PARITY_NONE:
				default:                       //none
					PARITYON = 0;
					PARITY = 0;
					break;
				case SERIAL_PARITY_ODD:                        //odd
					PARITYON = PARENB;
					PARITY = PARODD;
					break;
				case SERIAL_PARITY_EVEN:                        //even
					PARITYON = PARENB;
					PARITY = 0;
					break;
			}  //end of switch parity

			tcgetattr(fd[PortIndex],&oldtio[PortIndex]); /* save current port settings */
			/* set new port settings */
			bzero(&newtio[PortIndex], sizeof(newtio[PortIndex]));
			//newtio[PortIndex].c_cflag = BaudRate | /*CRTSCTS |*/ CS8 | CLOCAL | CREAD;
			//newtio[PortIndex].c_cflag |= PARENB
			newtio[PortIndex].c_cflag = BaudRate | DATABITS | STOPBITS | PARITYON | PARITY | CLOCAL | CREAD;
			newtio[PortIndex].c_iflag = IGNPAR    | IGNBRK; // | ICRNL;
			newtio[PortIndex].c_oflag = 0;
			newtio[PortIndex].c_lflag = 0;
			newtio[PortIndex].c_cc[VMIN]=0; //1;
			newtio[PortIndex].c_cc[VTIME]=0;
			tcsetattr(fd[PortIndex],TCSANOW,&newtio[PortIndex]);
tcflush( fd[PortIndex], TCIFLUSH ); //discard possible datas not read
printf("Serial%d: port %s successfully opened.\n",PortIndex,SerialPortName);
			PortIsOpened[PortIndex] = 1;
		}
		else
		{
			printf( "Serial%d: speed value %d not found for serial init!!!\n", PortIndex, SerialSpeed );
		}
	}
	else
	{
		printf( "Serial%d: Failed to open serial port %s !!!\n", PortIndex, SerialPortName );
	}
	return PortIsOpened[PortIndex];
}

void SerialClose( int PortIndex )
{
	if ( PortIndex<0 || PortIndex>=NBR_SERIAL_PORTS )
		return;
	if ( PortIsOpened[PortIndex] )
	{
		PortIsOpened[PortIndex] = 0;
		/* restore old port settings */
		tcsetattr(fd[PortIndex],TCSANOW,&oldtio[PortIndex]);
		close(fd[PortIndex]);
printf("Serial%d: port closed.\n",PortIndex);
	}
}

char SerialPortIsOpened( int PortIndex )
{
	if ( PortIndex<0 || PortIndex>=NBR_SERIAL_PORTS )
		return 0;
	return PortIsOpened[PortIndex];
}

void SerialSetRTS( int PortIndex, int State )
{
	int status;
	if ( PortIndex<0 || PortIndex>=NBR_SERIAL_PORTS )
		return;
	if ( PortIsOpened[PortIndex] )
	{
		ioctl(fd[PortIndex], TIOCMGET, &status);
		if ( State )
			status |= TIOCM_RTS;
		else
			status &= ~TIOCM_RTS;
		if ( DebugLevelMsgs[PortIndex]>=3 )
			printf( "Serial%d: Set signal RTS=%d\n", PortIndex, State );
		ioctl(fd[PortIndex], TIOCMSET, &status);
	}
}

void SerialSend( int PortIndex, char * Buff, int BuffLength )
{
	if ( PortIndex<0 || PortIndex>=NBR_SERIAL_PORTS )
		return;
	if ( PortIsOpened[PortIndex] )
	{
		if ( UseRtsSignal[PortIndex] )
		{
			SerialSetRTS( PortIndex, 1 );
//premiers essais avec mon module AVR...
//////DoPauseMilliSecs( 30 );
		}
		if ( DebugLevelMsgs[PortIndex]>=2 )
			printf("Serial%d: writing...\n",PortIndex);
		write(fd[PortIndex],Buff,BuffLength);
		if ( DebugLevelMsgs[PortIndex]>=2 )
			printf("Serial%d: Writing done!\n",PortIndex);
		if ( UseRtsSignal[PortIndex] )
		{
			// wait until everything has been transmitted
			tcdrain( fd[PortIndex] );
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
		newtio[PortIndex].c_cc[VMIN] = Size; //Nbr chars we should receive;
		newtio[PortIndex].c_cc[VTIME] = TimeOutResp/100; // TimeOut in 0.1s
//		tcflush(fd[PortIndex], TCIFLUSH);
		if ( DebugLevelMsgs[PortIndex]>=2 )
			printf("Serial%d: config before reading... (length=%d,timeout=%dms)\n",PortIndex,Size,TimeOutResp);
		tcsetattr(fd[PortIndex],TCSANOW,&newtio[PortIndex]);
	}
}

// per default, will not block if no chars received (see SerialSetResponseSize()
// & explains parameters MIN & TIME at the top...
int SerialReceive( int PortIndex, char * Buff, int MaxBuffLength )//, int TimeOutResp )
{
	int NbrCarsReceived = 0;
	if ( PortIndex<0 || PortIndex>=NBR_SERIAL_PORTS )
		return 0;
	if ( PortIsOpened[PortIndex] )
	{

int recep_descrip;
int TimeOutInterChar = newtio[PortIndex].c_cc[VTIME]*100 *1000; //micro-seconds
if ( TimeOutInterChar!=0 )
{
// the select is used if no char at all is received (else read() block...)
fd_set myset;
struct timeval tv;
FD_ZERO( &myset);
// add descrip to survey and set time-out wanted !
FD_SET( fd[PortIndex], &myset );
tv.tv_sec = 0; //seconds
tv.tv_usec = TimeOutInterChar; //micro-seconds
if ( DebugLevelMsgs[PortIndex]>=3 )
	printf("Serial%d: select() for  reading...\n",PortIndex);
recep_descrip = select( /*16*/fd[PortIndex]+1, &myset, NULL, NULL, &tv );
}
else
{
recep_descrip = 1;
}
if ( recep_descrip>0 )
{
		if ( DebugLevelMsgs[PortIndex]>=2 )
			printf("Serial%d: reading...\n",PortIndex);
			NbrCarsReceived = read(fd[PortIndex],Buff,MaxBuffLength);
		if ( DebugLevelMsgs[PortIndex]>=2 )
			printf("Serial%d: %d chars found\n", PortIndex, NbrCarsReceived);
}
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
			printf("Serial%d: purge all!\n",PortIndex);
		tcflush( fd[PortIndex], TCIOFLUSH );
		usleep( 250*1000 );
		tcflush( fd[PortIndex], TCIOFLUSH );
	}
}

void SerialConfigDebugAndRts( int PortIndex, int DbgLvl, char RtsToUse )
{
	if ( PortIndex<0 || PortIndex>=NBR_SERIAL_PORTS )
		return;
	printf("Serial%d: choice DebugLevel=%d, UseRts=%d.\n",PortIndex,DbgLvl,RtsToUse);
	DebugLevelMsgs[ PortIndex ] = DbgLvl;
	UseRtsSignal[ PortIndex ] = RtsToUse;
}
