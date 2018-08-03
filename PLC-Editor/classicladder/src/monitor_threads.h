
void InitGeneralMonitor( int ListenPortForSlave, char InitOnlyTheSlave );
void EndGeneralMonitor( void );
void MonitorSlaveLoopThread( int SlaveThreadForSerial );
void MonitorMasterLoopThread( void );

#ifdef GTK_INTERFACE
//////gboolean DoDisconnectTargetInGtkWithLock( void );
gboolean DoFlipFlopConnectDisconnectTargetInGtk( void );
#endif
