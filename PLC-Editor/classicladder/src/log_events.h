typedef void(*FunctionAlarmTransmitToCall)(int,char,StrEventLog*);

void InitLogDatas( void );
void InitLogConfig( void );
void InitRemoteAlarmsConfig( void );
void InitVarsArrayLogTags( void );
void VarStateChanged( int Type, int Num, int NewStateValue );
void CreateEventLog( int ConfigArrayNumber, int ParameterValue, char StartNewOne );
void SetTransmitAlarmsGlobalFlag( unsigned char SlotsSelected );
void CleanupEventsLog( void );
void WriteBoolVarsTo1FromPendingEventsLog( void );
StrLog * GetCopyLogDatasToFreeAfterUse( void );
int FindCurrentDefaults( void );

void LoadLogEventsData( void );
void SaveLogEventsData( void );

char SaveLogEventsTextFile( char * File, char Compressed );
char ConvertCompressedCsvToUncompressedCsv( char * FileGz, char * NewFileCsv );

void CreateAlarmEventMessage( int AlarmType, StrEventLog * pEvent, char ForEnd, char * BuffMsg, char * BuffMsgSubject );

void RemoteAlarmsTransmitScanAllEvents( FunctionAlarmTransmitToCall FunctionToCall );
char RemoteAlarmsCheckTransmitFlag( int ScanAlarmSlot, char ForEnd, StrEventLog * pEvent );
void RemoteAlarmsResetTransmitFlag( int ScanAlarmSlot, char ForEnd, int iIdEventSearched );

void RemoteAlarmsTransmitForEmails( void );
void RemoteAlarmsTransmitSendEmailIfEventConcerned( int ScanAlarmSlot, char ForEnd, StrEventLog * pEvent );
