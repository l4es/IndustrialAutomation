
#ifndef FRAMES_LOG_BUFFERS_H
#define FRAMES_LOG_BUFFERS_H

#define FRAMES_LOG_MODBUS_MASTER 0
#define FRAMES_LOG_MONITOR_SLAVE_IP 1
#define FRAMES_LOG_MONITOR_SLAVE_SERIAL 2
#define FRAMES_LOG_MODBUS_SLAVE_IP 3
#define NBR_FRAMES_LOG_BUFFERS 4

// not stored in buffer (only displayed when its monitor window is opened...)
#define FRAMES_LOG_MONITOR_MASTER 4

// for SlaveMode in monitor protocol functions...
#define NUM_FRAME_LOG_FOR_MON( SlaveModeValue ) ( (SlaveModeValue==0)?FRAMES_LOG_MONITOR_MASTER:( (SlaveModeValue==1)?FRAMES_LOG_MONITOR_SLAVE_IP:FRAMES_LOG_MONITOR_SLAVE_SERIAL   ) )

#define TRACE_LGT 10000
typedef struct StrFramesLogBuff
{
	// contains both datas chars received/send and type information with 2 chars (MAGIC_CHAR+Type)
	//TODO: add after Type, 4 bytes current time_t !
	unsigned char TraceBuff[ TRACE_LGT ];
	unsigned char * pTraceFirst;
	unsigned char * pTraceLast;
	int TraceSize;
	unsigned char CurrentTraceType;
	char AskedToCleanupLog;
}StrFramesLogBuff;

void InitFramesLogBuffer( StrFramesLogBuff * pLogBuff );
void InitAllFramesLogBuffers( void );

void FrameLogDatas( int NumFrameLogBuff, unsigned char Type, unsigned char * Datas, int LgtDatas );
void FrameLogString( int NumFrameLogBuff, unsigned char Type, char * String );

void FrameLogCleanupAsked( int NumFrameLogBuff );

void SaveFramesLogTextCompressed( int NumFrameLogBuff, char * File );
#endif
