/* Classic Ladder Project */
/* Copyright (C) 2001-2017 Marc Le Douarain */
/* http://membres.lycos.fr/mavati/classicladder/ */
/* http://www.sourceforge.net/projects/classicladder */
/* July 2014 */
/*----------------------------------------------------------------- */
/* Frames log buffers (to store on embedded datas frames exchanged) */
/* ---------------------------------------------------------------- */
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

/* Log store Based on :
	ClassicLadder I/O Module hardware firmware
	(running on AVR ATMega644p) */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <zlib.h>

#include "classicladder.h"
//#include "global.h"
#include "frames_log_buffers.h"
//#include "tasks.h" // for GetCurrentIntTime() & ConvertIntTimeToAsc()
#include "time_and_rtc.h"

#ifdef GTK_INTERFACE
#include "monitor_windows_gtk.h" // for MonitorWindowAddText() function
#endif

#define MAGIC_CHAR 0xAA
StrFramesLogBuff FramesLogBuff[ NBR_FRAMES_LOG_BUFFERS ];

void InitFramesLogBuffer( StrFramesLogBuff * pLogBuff )
{
	pLogBuff->pTraceFirst = pLogBuff->TraceBuff;
	pLogBuff->pTraceLast = pLogBuff->TraceBuff;
	pLogBuff->TraceSize = 0;
	pLogBuff->CurrentTraceType = 0;
	pLogBuff->AskedToCleanupLog = FALSE;
}

void InitAllFramesLogBuffers( void )
{
	int Scan;
	for( Scan=0; Scan<NBR_FRAMES_LOG_BUFFERS; Scan++ )
		InitFramesLogBuffer( &FramesLogBuff[ Scan ] );
}

void OneMoreStored( StrFramesLogBuff * pLogBuff )
{
	pLogBuff->pTraceLast++;
	if ( pLogBuff->pTraceLast==&pLogBuff->TraceBuff[ TRACE_LGT ] )
		pLogBuff->pTraceLast = pLogBuff->TraceBuff;
	if ( pLogBuff->TraceSize<TRACE_LGT )
	{
		pLogBuff->TraceSize++;
	}
	else
	{
		pLogBuff->pTraceFirst++;
		if ( pLogBuff->pTraceFirst==&pLogBuff->TraceBuff[ TRACE_LGT ] )
			pLogBuff->pTraceFirst = pLogBuff->TraceBuff;
	}
}
void StoreLog( StrFramesLogBuff * pLogBuff, unsigned char Type, unsigned char CharToTrace )
{
	if ( pLogBuff->AskedToCleanupLog )
	{
		InitFramesLogBuffer( pLogBuff );
printf(">>> %s(), Cleanup asked now done for frames log!\n", __FUNCTION__ );
		pLogBuff->AskedToCleanupLog = FALSE;
	}
	if ( Type!=pLogBuff->CurrentTraceType )
	{
		int ScanStoreTime;
		time_t CurrTime = GetCurrentIntTime( );
		char * pCharCurrTime = (char *)&CurrTime;
		*pLogBuff->pTraceLast = MAGIC_CHAR; // to indicate, 'type' value coming...
		OneMoreStored( pLogBuff );
		*pLogBuff->pTraceLast = Type;
		OneMoreStored( pLogBuff );
		pLogBuff->CurrentTraceType = Type;
		for( ScanStoreTime=0; ScanStoreTime<sizeof( time_t ); ScanStoreTime++ )
		{
			*pLogBuff->pTraceLast = *pCharCurrTime++;
			OneMoreStored( pLogBuff );
		}
	}
	// double char if same value...
	if ( CharToTrace==MAGIC_CHAR )
	{
		*pLogBuff->pTraceLast = MAGIC_CHAR;
		OneMoreStored( pLogBuff );
	}
	*pLogBuff->pTraceLast = CharToTrace;
	OneMoreStored( pLogBuff );
}


void FrameLogDatas( int NumFrameLogBuff, unsigned char Type, unsigned char * Datas, int LgtDatas )
{
	if ( NumFrameLogBuff>=0 && NumFrameLogBuff<NBR_FRAMES_LOG_BUFFERS )
	{
		int ScanChar;
		StrFramesLogBuff * pLogBuff = &FramesLogBuff[ NumFrameLogBuff ];
		for( ScanChar=0; ScanChar<LgtDatas; ScanChar++ )
			StoreLog( pLogBuff, Type, *Datas++ );
	}
}
void FrameLogString( int NumFrameLogBuff, unsigned char Type, char * String )
{
	if ( NumFrameLogBuff>=0 )
	{
		// if not stored here (no buffer for it), directly send to correspondant frame window...
		if ( NumFrameLogBuff>=NBR_FRAMES_LOG_BUFFERS )
		{
#ifdef GTK_INTERFACE
//TODO: review with adding Type / CurrentTime... (for now directly done in monitor master log send...)
			StrFramesLogBuff * pLogBuff = &FramesLogBuff[ NumFrameLogBuff ];
			if ( pLogBuff->CurrentTraceType!=0 && Type!=pLogBuff->CurrentTraceType )
				MonitorWindowAddText( NumFrameLogBuff, "\n" );
			MonitorWindowAddText( NumFrameLogBuff, String );
			pLogBuff->CurrentTraceType = Type;
#endif
		}
		else
		{
			int Lgt = strlen( String );
			FrameLogDatas( NumFrameLogBuff, Type, (unsigned char *)String, Lgt );
		}
	}
}

void FrameLogCleanupAsked( int NumFrameLogBuff )
{
	if ( NumFrameLogBuff>=0 && NumFrameLogBuff<NBR_FRAMES_LOG_BUFFERS )
	{
		StrFramesLogBuff * pLogBuff = &FramesLogBuff[ NumFrameLogBuff ];
printf(">>> %s(), Cleanup asked to be done for frames log %d...\n", __FUNCTION__, NumFrameLogBuff );
		pLogBuff->AskedToCleanupLog = TRUE;
	}
}

void AdvanceForLogText( StrFramesLogBuff * pLogBuffer, unsigned char ** pFirst, short int * Size )
{
	(*pFirst)++;
	if ( *pFirst==&pLogBuffer->TraceBuff[ TRACE_LGT ] )
		*pFirst = pLogBuffer->TraceBuff;
	(*Size)--;
}

void SaveFramesLogTextCompressed( int NumFrameLogBuff, char * File )
{
	if ( NumFrameLogBuff>=0 && NumFrameLogBuff<NBR_FRAMES_LOG_BUFFERS )
	{
		StrFramesLogBuff * pLogBuff = &FramesLogBuff[ NumFrameLogBuff ];
printf("*** SAVING ---FRAMES LOG TEXT--- FOR %d IN FILE %s\n", NumFrameLogBuff, File );
		gzFile pLogFileGz = gzopen( File, "wt" );
		if ( pLogFileGz )
		{
			if ( pLogBuff->TraceSize>0 )
			{
		
				unsigned char * pScanFirst = pLogBuff->pTraceFirst;
				short int ScanSize = pLogBuff->TraceSize;
				char FirstInfoSeen = 0; // do not display partial frame (start of the buffer after rollup)
				char CurrentTypeDatas = 0;
				while( ScanSize>0 )
				{
					unsigned char CurrentChar = *pScanFirst;
					char Info = 0;
					if ( CurrentChar==MAGIC_CHAR )
					{
						AdvanceForLogText( pLogBuff, &pScanFirst, &ScanSize );
						CurrentChar = *pScanFirst;
						if ( CurrentChar!=MAGIC_CHAR )
						{
							int ScanReadTime;
							time_t FrameTime;
							char * pCharFrameTime = (char *)&FrameTime;
							char BuffFrameTime[ 20 ];
							Info = 1;
							if ( FirstInfoSeen )
							{
								gzputs( pLogFileGz, "\n" );
							}
							// type
							CurrentTypeDatas = *pScanFirst;

							// time
							for( ScanReadTime=0; ScanReadTime<sizeof( time_t ); ScanReadTime++ )
							{
								AdvanceForLogText( pLogBuff, &pScanFirst, &ScanSize );
								*pCharFrameTime++ = *pScanFirst;
							}
							ConvertIntTimeToAsc( FrameTime, BuffFrameTime, FALSE/*WithDate*/, FALSE/*InUTC*/ );
							gzputs( pLogFileGz, BuffFrameTime );

							switch( CurrentTypeDatas )
							{
								case ')': case '>': gzputs( pLogFileGz, " #> " ); break;
								case '(': case '<': gzputs( pLogFileGz, " #< " ); break;
								default: gzprintf( pLogFileGz, " %c  ", CurrentTypeDatas ); break;
							}
							FirstInfoSeen = 1;
						}
					}
					if ( !Info && FirstInfoSeen )
					{
						// in hexa only if type = ')' or '('...
						switch( CurrentTypeDatas )
						{
							case ')': case '(': gzprintf( pLogFileGz, "%02X ", CurrentChar ); break;
							default: gzprintf( pLogFileGz, "%c", CurrentChar ); break;
						}
					}
					AdvanceForLogText( pLogBuff, &pScanFirst, &ScanSize );
				}
				gzputs( pLogFileGz, "\n" );
			}
			else
			{
				gzputs( pLogFileGz, "Empty..." );
			}
			gzclose( pLogFileGz );
		}
	}
}
