/* Classic Ladder Project */
/* Copyright (C) 2001-2017 Marc Le Douarain */
/* http://www.sourceforge.net/projects/classicladder */
/* http://sites.google.com/site/classicladder */
/* February 2001 */

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

#ifdef MAT_CONNECTION
#include "../../lib/plc.h"
#define TYPE_FOR_BOOL_VAR plc_pt_t
#else
#define TYPE_FOR_BOOL_VAR char
#endif

#ifdef MODBUS_IO_MASTER
#include "protocol_modbus_master.h"
#endif

extern StrRung * RungArray;
extern TYPE_FOR_BOOL_VAR * VarArray;
extern unsigned char * VarSetArray;
extern unsigned char * LogVarArray;
extern int * VarWordArray;
#ifdef OLD_TIMERS_MONOS_SUPPORT
extern StrTimer * TimerArray;
extern StrMonostable * MonostableArray;
#endif
extern StrCounter * CounterArray;
extern StrTimerIEC * NewTimerArray;
extern StrRegister * RegisterArray;
extern int * RegistersListsDatas;
extern StrArithmExpr * ArithmExpr;
extern StrInfosGene * InfosGene;
extern StrInfosGUI * InfosGUI;
extern StrSection * SectionArray;
#ifdef SEQUENTIAL_SUPPORT
extern StrSequential *Sequential;
extern StrSequential EditSeqDatas;
#endif
extern StrSymbol * SymbolArray;
extern StrInputFilterAndState * InputFilterAndStateArray;

extern StrEditRung EditDatas;
extern StrArithmExpr * EditArithmExpr;

extern StrDatasForBase CorresDatasForBase[3];
extern char * TimersModesStrings[ NBR_TIMERSMODES ];
extern char * RegistersModesStrings[ NBR_REGISTERS_MODES ];

#ifdef __GTK_H__
extern GtkWidget *MainSectionWindow;
//Cairo extern GdkPixmap *pixmap;
extern GtkWidget *drawing_area;
extern int PrintRightMarginPosiX;
extern int PrintRightMarginWidth;
#endif

extern char TmpDirectoryRoot[ 400 ];
extern char TmpDirectoryProject[ 400 ];

extern StrGeneralParams GeneralParamsMirror;

#ifdef MODBUS_IO_MASTER
extern StrModbusMasterReq ModbusMasterReq[ NBR_MODBUS_MASTER_REQ ];
extern StrModbusSlave ModbusSlaveList[ NBR_MODBUS_SLAVES ];
extern StrModbusConfig ModbusConfig;
#endif

extern char * ErrorMessageVarParser;

extern char LogEventsDataFile[ 400 ];
extern StrLog Log;
extern StrConfigEventLog ConfigEventLog[ NBR_CONFIG_EVENTS_LOG ]; 
extern unsigned char ListCurrentDefType[ NBR_CURRENT_DEFS_MAX ];
extern int ListCurrentDefParam[ NBR_CURRENT_DEFS_MAX ];
extern StrRemoteAlarms RemoteAlarmsConfig;

extern StrPreferences Preferences;

extern StrSearchDatas SearchDatas;

extern StrNetworkConfigDatas NetworkConfigDatas;

extern StrModem Modem;

extern char TmpLogEventsFileFromTarget[ 400 ];

//extern char * LaunchScriptParamList[ 30 ];

//////extern int PipeLaunchExternalCmd[ 2 ];
extern int PipeLaunchExternalCmd;

//extern struct tm * ptm_for_vars_sys;
extern struct tm tm_copy_for_vars_sys;
