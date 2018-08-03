
void InitPreferences( void );
void LoadPreferences( void );
char SavePreferences( void );

#if defined( GTK_INTERFACE ) && defined( __GTK_H__ )
StrWindowPosisOpenPrefs * GetPtrWindowPrefs( char * WindowName, char CanBeANewWin );
void RestoreWindowPosiPrefs( char * WindowName, GtkWidget * TheGtkWindow );
void RememberWindowPosiPrefs( char * WindowName, GtkWidget * TheGtkWindow, char SaveWindowSize );
char GetWindowOpenPrefs( char * WindowName );
void RememberWindowOpenPrefs( char * WindowName, char WindowOpened );
#endif

char LoadNetworkConfigDatas( );
char SaveNetworkConfigDatas( );
char LoadHostNameFile( );
char SaveHostNameFile( );

char LoadSetsVarsList( void );
char SaveSetsVarsList( void );
