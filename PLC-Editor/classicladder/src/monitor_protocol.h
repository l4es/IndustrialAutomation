
#include "cJSON.h"

char * MonitorParseGeneralResponseToSwitch( char * TextReq, char SlaveMode );
char * MonitorCreateRequestReadVars( void );
void MonitorParseResponseReadVars( cJSON *JsonRoot, char SlaveMode );
char * MonitorCreateRequestReadRung( int NumRung );
void MonitorParseResponseReadRung( cJSON *JsonRoot, char SlaveMode );
char * MonitorCreateRequestReadSequential( int NumPage );
void MonitorParseResponseReadSequential( cJSON *JsonRoot, char SlaveMode );

char * MonitorCreateRequestSetRunStopState( int RunStopStateWanted );
void MonitorParseResponseSetRunStopState( cJSON *JsonRoot, char SlaveMode );
char * MonitorCreateRequestResetTarget( void );
void MonitorParseResponseResetTarget( cJSON *JsonRoot, char SlaveMode );
char * MonitorCreateRequestSetClockTime( );
void MonitorParseResponseSetClockTime( cJSON *JsonRoot, char SlaveMode );

char * MonitorCreateRequestGetTargetInfosVersion( void );
void MonitorParseResponseGetTargetInfosVersion( cJSON *JsonRoot, char SlaveMode );
char * MonitorCreateRequestGetProjectProperties( void );
void MonitorParseResponseGetProjectProperties( cJSON *JsonRoot, char SlaveMode );

char * MonitorCreateRequestRebootTarget( void );
void MonitorParseResponseRebootTarget( cJSON *JsonRoot, char SlaveMode );
char * MonitorCreateRequestWriteVarValue( );
void MonitorParseResponseWriteVarValue( cJSON *JsonRoot, char SlaveMode );
char * MonitorCreateRequestSetOrUnsetVar( );
void MonitorParseResponseSetOrUnsetVar( cJSON *JsonRoot, char SlaveMode );

char * MonitorCreateRequestGetNetworkConfig( void );
void MonitorParseResponseGetNetworkConfig( cJSON *JsonRoot, char SlaveMode );
char * MonitorCreateRequestWriteNetworkConfig( void );
void MonitorParseResponseWriteNetworkConfig( cJSON *JsonRoot, char SlaveMode );

char * MonitorCreateRequestGetMonitorSerialConfig( void );
void MonitorParseResponseGetMonitorSerialConfig( cJSON *JsonRoot, char SlaveMode );
char * MonitorCreateRequestWriteMonitorSerialConfig( void );
void MonitorParseResponseWriteMonitorSerialConfig( cJSON *JsonRoot, char SlaveMode );

char * MonitorCreateRequestGetModbusMasterStats( int SlaveNbr);
void MonitorParseResponseGetModbusMasterStats( cJSON *JsonRoot, char SlaveMode );

char * MonitorCreateRequestCleanUp( void );
void MonitorParseResponseCleanUp( cJSON *JsonRoot, char SlaveMode );

