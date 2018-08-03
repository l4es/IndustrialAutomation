//added for time_t...
#include <time.h>

void InitTimeAndRtc( void );

void GetTimeDatasInThread( char DoUseRtcDevice );
time_t GetCopyCurrentIntTime( void );

char OpenRtcDevice( void );
void CloseRtcDevice( void );
time_t ReadRtc( void );
char WriteRtc( time_t time_to_set );

char SetTimeClock( time_t IntTimeValueToSet, char DoUseRtcDevice );

time_t GetCurrentIntTime( void );
void ConvertIntTimeToAsc( time_t IntTime, char * Buff, char WithDate, char InUTC );
void GetCurrentAscTime( char * Buff );
void GetCurrentAscTimeUTC( char * Buff );
time_t ConvertAscUtcToIntTime( char * Buff );
