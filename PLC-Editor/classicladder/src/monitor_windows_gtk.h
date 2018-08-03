
#ifndef MONITOR_WINDOWS_GTK_H
#define MONITOR_WINDOWS_GTK_H

#include "frames_log_buffers.h"

// +1 for monitor master not stored in buffer (only displayed in window)
#define NBR_FRAMES_LOG_WINDOWS (NBR_FRAMES_LOG_BUFFERS+1)

// if this file included and gtk.h not included before... (this is the case in monitor_protocol_adds_serial.c !)
#ifdef __GTK_H__
char OpenDialogMonitorConnect( void );
void OpenMonitorWindow0( GtkAction * ActionOpen, gboolean OpenIt );
void OpenMonitorWindow1( GtkAction * ActionOpen, gboolean OpenIt );
void OpenMonitorWindow2( GtkAction * ActionOpen, gboolean OpenIt );
void OpenMonitorWindow3( GtkAction * ActionOpen, gboolean OpenIt );
void OpenMonitorWindow4( GtkAction * ActionOpen, gboolean OpenIt );
void RememberMonitorWindowPrefs( int NumFramesLogWindow );
void FramesLogWindowsInitGtk( void );
void UpdateMonitorModbusSlaveListStats( void );
void SignalStatisticsModbusSlaveSelect( GtkComboBox * pCombo, gpointer user_data );
gboolean StatisticsModbusSlaveSelectedRefreshInfosGtk( void );
#endif
void MonitorWindowAddText( int NumFramesLogWindow, char * text );

void DisplayFrameLogFromFileTextCompressed( int NumFramesLogWindow, char * FramesLogFile );
#ifdef __GTK_H__
gboolean DisplayFrameLogFromFileTextCompressedFromMainGtk( gpointer text );
#endif

#endif
