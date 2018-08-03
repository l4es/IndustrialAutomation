/* Classic Ladder Project */
/* Copyright (C) 2001-2017 Marc Le Douarain */
/* http://www.sourceforge.net/projects/classicladder */
/* http://sites.google.com/site/classicladder */
/* July 2003 */
/* ----------------------------- */
/* Editor Config - GTK interface */
/* ----------------------------- */
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

#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libintl.h> // i18n
#include <locale.h> // i18n
#include "classicladder.h"
#include "manager.h"
#include "edit.h"
#include "hardware.h"
#include "global.h"
#include "classicladder_gtk.h"
#include "vars_names.h"
#ifdef COMPLETE_PLC
#include "log_events.h"
#endif
#include "socket_modbus_master.h"
#include "config_gtk.h"

#ifdef OLD_TIMERS_MONOS_SUPPORT
#define NBR_OBJECTS_GENERAL 19+2
#else
#define NBR_OBJECTS_GENERAL 17+2
#endif
#define NBR_SUBS_VBOX 3
GtkWidget *LabelParam[ NBR_OBJECTS_GENERAL ],*ValueParam[ NBR_OBJECTS_GENERAL ];

static char * Devices[] = { N_("None"), N_("DirectPortAccess"), N_("DirectPortConfig"), N_("Raspberry_GPIO"), N_("Atmel_SAM_GPIO"),
#ifdef COMEDI_SUPPORT
"/dev/comedi0", "/dev/comedi1", "/dev/comedi2", "/dev/comedi3",
#endif
 NULL };

#define NBR_IO_PARAMS 7
GtkWidget *InputParamEntry[ NBR_INPUTS_CONF ][ NBR_IO_PARAMS ];
GtkWidget *InputDeviceParam[ NBR_INPUTS_CONF ];
GtkWidget *InputFlagParam[ NBR_INPUTS_CONF ];

GtkWidget *OutputParamEntry[ NBR_OUTPUTS_CONF ][ NBR_IO_PARAMS ];
GtkWidget *OutputDeviceParam[ NBR_OUTPUTS_CONF ];
GtkWidget *OutputFlagParam[ NBR_OUTPUTS_CONF ];

#ifdef MODBUS_IO_MASTER
// ModbusReqType must be in the same order as MODBUS_REQ_ in protocol_modbus_master.h
static char * ModbusReqType[] = { N_("ReadInputs (to %I)"), N_("WriteCoils (from %Q)"), N_("ReadInputRegs (to %IW)"), N_("WriteHoldRegs (from %QW)"), N_("ReadCoils (to %Q)"), N_("ReadHoldRegs (to %QW)"), N_("ReadStatus (to %IW)"), N_("Diagnostic (from %IW/to %QW - 1stEle=sub-code used)"), NULL };
#define NBR_MODBUS_REQ_PARAMS 6
GtkWidget *ModbusReqParamEntry[ NBR_MODBUS_MASTER_REQ ][ NBR_MODBUS_REQ_PARAMS ];
#define NBR_MODBUS_SLAVES_PARAMS 4
GtkWidget *ModbusSlaveParamEntry[ NBR_MODBUS_SLAVES ][ NBR_MODBUS_SLAVES_PARAMS ];
//GtkWidget *SerialPortEntry;
//GtkWidget *SerialSpeedEntry;
//GtkWidget *PauseInterFrameEntry;
//GtkWidget *DebugLevelEntry;
#define NBR_COM_PARAMS 17
#define NBR_RADIO_BUT_COM_PARAMS 5
GtkWidget *EntryComParam[ NBR_COM_PARAMS ];
GtkWidget *RadioButComParams[ NBR_COM_PARAMS ][ NBR_RADIO_BUT_COM_PARAMS ];
#endif

#define NBR_CONFIG_EVENTS_PARAMS (5+8/*AlarmsSlots*/)
GtkWidget *EventConfigParamEntry[ NBR_CONFIG_EVENTS_LOG ][ NBR_CONFIG_EVENTS_PARAMS ];

GtkWidget * AutoAdjustTime;
#define NBR_MODEM_CONF (1+4+4)
GtkWidget * ModemConfEntry[ NBR_MODEM_CONF ];

#define NBR_LINES_REMOTE_ALARMS (1/*Global*/+1/*LabelsLine*/+8/*Slots*/+1/*CenterSMS*/+4/*SmtpParameters*/)
#define NBR_PARAMS_SLOTS_ALARMS 4
GtkWidget * RemoteAlarmConfEntry[ NBR_LINES_REMOTE_ALARMS ][ NBR_PARAMS_SLOTS_ALARMS ];

GtkWidget *ConfigWindow;

void ButtonSetDefaultProject_click( void )
{
	strcpy( Preferences.DefaultProjectFileNameToLoadAtStartup, InfosGene->CurrentProjectFileName );
	gtk_entry_set_text( GTK_ENTRY(ValueParam[NBR_OBJECTS_GENERAL-1]), Preferences.DefaultProjectFileNameToLoadAtStartup );
}
void ButtonClearDefaultProject_click( void )
{
	Preferences.DefaultProjectFileNameToLoadAtStartup[ 0 ] = '\0';
	gtk_entry_set_text( GTK_ENTRY(ValueParam[NBR_OBJECTS_GENERAL-1]), Preferences.DefaultProjectFileNameToLoadAtStartup );
}

GtkWidget * CreateGeneralParametersPage( void )
{
	GtkWidget *vbox_main;
	GtkWidget *hbox[ NBR_OBJECTS_GENERAL ];
	GtkWidget *hbox_for_subs;
	GtkWidget *vbox_sub[ NBR_SUBS_VBOX ];
	int NumObj,ScanSub;
	int CurrentSub = 0;

	vbox_main = gtk_vbox_new (FALSE, 0);

	for (NumObj=0; NumObj<NBR_OBJECTS_GENERAL; NumObj++)
	{
		char BuffLabel[ 100 ];
		char BuffValue[ 200 ];
                
		int InfoUsed = 0;
		hbox[NumObj] = gtk_hbox_new (FALSE, 0);

		switch( NumObj )
		{
			case 1:
				sprintf( BuffLabel, _("Periodic Refresh Rate 'inputs scan' (milliseconds)") );
				sprintf( BuffValue, "%d", GeneralParamsMirror.PeriodMilliSecsTaskScanInputs );
				break;
			case 2:
				sprintf( BuffLabel, _("Periodic Refresh Rate 'logic' (milliseconds)") );
				sprintf( BuffValue, "%d", GeneralParamsMirror.PeriodMilliSecsTaskLogic );
				break;
			case 3:
				InfoUsed = GetNbrRungsDefined( )*100/InfosGene->GeneralParams.SizesInfos.nbr_rungs;
				sprintf( BuffLabel, "%s (%d%c %s - %s=%d %s=%d%s)", _("Nbr.rungs"), InfoUsed,'%', _("used"), _("current alloc"), NBR_RUNGS, _("size"), NBR_RUNGS*sizeof( StrRung ), _("bytes") );
				sprintf( BuffValue, "%d", GeneralParamsMirror.SizesInfos.nbr_rungs );
				break;
			case 4:
				sprintf( BuffLabel, "%s (%s=%d)", _("Nbr.Bits"), _("current alloc"), NBR_BITS );
				sprintf( BuffValue, "%d", GeneralParamsMirror.SizesInfos.nbr_bits );
				break;
			case 5:
				sprintf( BuffLabel, "%s (%s=%d)", _("Nbr.Words"), _("current alloc"), NBR_WORDS );
				sprintf( BuffValue, "%d", GeneralParamsMirror.SizesInfos.nbr_words );
				break;
			case 6:
				sprintf( BuffLabel, "%s (%s=%d %s=%d%s)", _("Nbr.Counters"), _("current alloc"), NBR_COUNTERS, _("size"), NBR_COUNTERS*sizeof( StrCounter ), _("bytes") );
				sprintf( BuffValue, "%d", GeneralParamsMirror.SizesInfos.nbr_counters );
				break;
			case 7:
				sprintf( BuffLabel, "%s (%s=%d %s=%d%s)", _("Nbr.Timers IEC"), _("current alloc"), NBR_TIMERS_IEC, _("size"), NBR_TIMERS_IEC*sizeof( StrTimerIEC ), _("bytes") );
				sprintf( BuffValue, "%d", GeneralParamsMirror.SizesInfos.nbr_timers_iec );
				break;
			case 8:
				sprintf( BuffLabel, "%s (%s=%d %s=%d%s)", _("Nbr.Registers"), _("current alloc"), NBR_REGISTERS, _("size"), NBR_REGISTERS*sizeof( StrRegister ), _("bytes") );
				sprintf( BuffValue, "%d", GeneralParamsMirror.SizesInfos.nbr_registers );
				break;
			case 9:
				sprintf( BuffLabel, "%s (%s=%d %s=%d%s)", _("Register list size"), _("current alloc"), REGISTER_LIST_SIZE, _("size"), NBR_REGISTERS * REGISTER_LIST_SIZE * sizeof(int), _("bytes") );
				sprintf( BuffValue, "%d", GeneralParamsMirror.SizesInfos.register_list_size );
				break;
			case 10:
				CurrentSub += 2;
				sprintf( BuffLabel, "%s (%s=%d)", _("Nbr.Phys.Inputs"), _("current alloc"), NBR_PHYS_INPUTS );
				sprintf( BuffValue, "%d", GeneralParamsMirror.SizesInfos.nbr_phys_inputs );
				break;
			case 11:
				sprintf( BuffLabel, "%s (%s=%d)", _("Nbr.Phys.Outputs"), _("current alloc"), NBR_PHYS_OUTPUTS );
				sprintf( BuffValue, "%d", GeneralParamsMirror.SizesInfos.nbr_phys_outputs );
				break;
			case 12:
				sprintf( BuffLabel, "%s (%s=%d)", _("Nbr.Phys.Words.Inputs"), _("current alloc"), NBR_PHYS_WORDS_INPUTS );
				sprintf( BuffValue, "%d", GeneralParamsMirror.SizesInfos.nbr_phys_words_inputs );
				break;
			case 13:
				sprintf( BuffLabel, "%s (%s=%d)", _("Nbr.Phys.Words.Outputs"), _("current alloc"), NBR_PHYS_WORDS_OUTPUTS );
				sprintf( BuffValue, "%d", GeneralParamsMirror.SizesInfos.nbr_phys_words_outputs );
				break;
			case 14:
				sprintf( BuffLabel, "%s (%s=%d %s=%d%s)", _("Nbr.Arithm.Expr."), _("current alloc"), NBR_ARITHM_EXPR, _("size"), NBR_ARITHM_EXPR*sizeof( StrArithmExpr ), _("bytes") );
				sprintf( BuffValue, "%d", GeneralParamsMirror.SizesInfos.nbr_arithm_expr );
				break;
			case 15:
				InfoUsed = NbrSectionsDefined( )*100/InfosGene->GeneralParams.SizesInfos.nbr_sections;
				sprintf( BuffLabel, "%s (%d%c %s - %s=%d %s=%d%s)", _("Nbr.Sections"), InfoUsed,'%', _("used"), _("current alloc"), NBR_SECTIONS, _("size"), NBR_SECTIONS*sizeof( StrSection ), _("bytes") );
				sprintf( BuffValue, "%d", GeneralParamsMirror.SizesInfos.nbr_sections );
				break;
			case 16:
				sprintf( BuffLabel, "%s (%s=%d)", _("Nbr.Symbols"), _("current alloc"), NBR_SYMBOLS );
				sprintf( BuffValue, "%d", GeneralParamsMirror.SizesInfos.nbr_symbols );
				break;
#ifdef OLD_TIMERS_MONOS_SUPPORT
			case 17:
				sprintf( BuffLabel, "%s (%s=%d %s=%d%s)", _("Nbr.Timers"), _("current alloc"), NBR_TIMERS, _("size"), NBR_TIMERS*sizeof( StrTimer ), _("bytes") );
				sprintf( BuffValue, "%d", GeneralParamsMirror.SizesInfos.nbr_timers );
				break;
			case 18:
				sprintf( BuffLabel, "%s (%s=%d %s=%d%s)", _("Nbr.Monostables"), _("current alloc"), NBR_MONOSTABLES, _("size"), NBR_MONOSTABLES*sizeof( StrMonostable ), _("bytes") );
				sprintf( BuffValue, "%d", GeneralParamsMirror.SizesInfos.nbr_monostables );
				break;
#endif
			case NBR_OBJECTS_GENERAL-2:
				sprintf( BuffLabel, _("Current file project") );
				sprintf( BuffValue, "%s",InfosGene->CurrentProjectFileName);
				break;                                
			case NBR_OBJECTS_GENERAL-1:
				sprintf( BuffLabel, _("Default startup project") );
				sprintf( BuffValue, "%s",Preferences.DefaultProjectFileNameToLoadAtStartup);
				break;                                
			default:
				sprintf( BuffLabel, "???" );
				sprintf( BuffValue, "???" );
				break;
		}

		if ( NumObj==0 )
		{
			ValueParam[NumObj] = gtk_check_button_new_with_label( _("Use real physical & serial modbus inputs/outputs only on the embedded target (not for GTK simul interface)") );
			if ( GeneralParamsMirror.RealInputsOutputsOnlyOnTarget )
				gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( ValueParam[NumObj] ), TRUE );
			gtk_box_pack_start (GTK_BOX (hbox[NumObj]), ValueParam[NumObj], FALSE, FALSE, 0);

			gtk_container_add (GTK_CONTAINER(vbox_main), hbox[NumObj]);

			hbox_for_subs = gtk_hbox_new (FALSE, 0);
			gtk_container_add (GTK_CONTAINER(vbox_main), hbox_for_subs);
			for( ScanSub=0; ScanSub<NBR_SUBS_VBOX; ScanSub++ )
			{
				vbox_sub[ ScanSub ] = gtk_vbox_new (FALSE, 0);
				if ( ScanSub==1 )
				{
					GtkWidget * VertSep = gtk_vseparator_new( );
					gtk_widget_set_size_request( VertSep, 30, -1 );
					gtk_box_pack_start (GTK_BOX(vbox_sub[ScanSub]), VertSep, TRUE/*expand*/, TRUE/*fill*/, 0);
				}
				gtk_container_add (GTK_CONTAINER(hbox_for_subs), vbox_sub[ScanSub]);
			}
		}
		else
		{
			LabelParam[NumObj] = gtk_label_new(BuffLabel);
//ForGTK3			gtk_widget_set_usize(/*(GtkWidget *)*/LabelParam[NumObj],(NumObj<NBR_OBJECTS_GENERAL-2)?420:180,0);
			if( NumObj<NBR_OBJECTS_GENERAL-2 )
				gtk_widget_set_size_request(LabelParam[NumObj],440,-1);
			gtk_box_pack_start (GTK_BOX(hbox[NumObj]), LabelParam[NumObj], FALSE, FALSE, 0);

			/* For numbers */
			ValueParam[NumObj] = gtk_entry_new();
//ForGTK3			gtk_widget_set_usize(/*(GtkWidget *)*/ValueParam[NumObj],(NumObj<NBR_OBJECTS_GENERAL-2)?50:450,0);
			if (NumObj<NBR_OBJECTS_GENERAL-2)
				gtk_widget_set_size_request(/*(GtkWidget *)*/ValueParam[NumObj],45,-1);
			gtk_box_pack_start (GTK_BOX(hbox[NumObj]), ValueParam[NumObj], NumObj>=NBR_OBJECTS_GENERAL-2 /*expand*/, NumObj>=NBR_OBJECTS_GENERAL-2 /*fill*/, 0);
			gtk_entry_set_text( GTK_ENTRY(ValueParam[NumObj]), BuffValue );
			gtk_editable_set_editable( GTK_EDITABLE(ValueParam[NumObj]), (NumObj<NBR_OBJECTS_GENERAL-2)?TRUE:FALSE);

			if ( NumObj<NBR_OBJECTS_GENERAL-2 )
				gtk_container_add (GTK_CONTAINER(vbox_sub[CurrentSub]), hbox[NumObj]);
			else
				gtk_container_add (GTK_CONTAINER(vbox_main), hbox[NumObj]);
		}

		
		if ( NumObj==NBR_OBJECTS_GENERAL-2 )
		{
			GtkWidget * ButtonSetDefaultPrj = gtk_button_new_with_label ( _("Use as default project") );
			gtk_box_pack_start (GTK_BOX(hbox[NumObj]), ButtonSetDefaultPrj, FALSE, FALSE, 0);
			gtk_signal_connect(GTK_OBJECT (ButtonSetDefaultPrj), "clicked",
						GTK_SIGNAL_FUNC(ButtonSetDefaultProject_click), 0);
		}
		if ( NumObj==NBR_OBJECTS_GENERAL-1 )
		{
			GtkWidget * ButtonClearDefaultPrj = gtk_button_new_with_label ( _("No default project") );
			gtk_box_pack_start (GTK_BOX(hbox[NumObj]), ButtonClearDefaultPrj, FALSE, FALSE, 0);
			gtk_signal_connect(GTK_OBJECT (ButtonClearDefaultPrj), "clicked",
						GTK_SIGNAL_FUNC(ButtonClearDefaultProject_click), 0);
		}
	}
	gtk_widget_show_all(vbox_main);
	return vbox_main;
}
int GetOneGeneralInfo( int iNumber )
{
	char text[ 10 ];
	int value;
	strncpy( text, (char *)gtk_entry_get_text((GtkEntry *)ValueParam[ iNumber ]), 10 );
	text[ 9 ] = '\0';
	value = atoi( text );
	return value;
}
void GetGeneralParameters( void )
{
	int TheValue;

	GeneralParamsMirror.RealInputsOutputsOnlyOnTarget = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( ValueParam[0] ) );
printf("Real I/O choice:%d\n", GeneralParamsMirror.RealInputsOutputsOnlyOnTarget);

	TheValue = GetOneGeneralInfo( 1 );
	if ( TheValue<1 || TheValue>=1000 )
		TheValue = PERIODIC_REFRESH_MS_INPUTS_DEF;
	GeneralParamsMirror.PeriodMilliSecsTaskScanInputs = TheValue;
//NoMoreUsed	InfosGene->GeneralParams.PeriodMilliSecsTaskScanInputs = TheValue;

	TheValue = GetOneGeneralInfo( 2 );
	if ( TheValue<1 || TheValue>=1000 )
		TheValue = PERIODIC_REFRESH_MS_DEF;
	if ( TheValue<GeneralParamsMirror.PeriodMilliSecsTaskScanInputs )
		TheValue = GeneralParamsMirror.PeriodMilliSecsTaskScanInputs;
	GeneralParamsMirror.PeriodMilliSecsTaskLogic = TheValue;
//NoMoreUsed	InfosGene->GeneralParams.PeriodMilliSecsTaskLogic = TheValue;

	TheValue = GetOneGeneralInfo( 3 );
	GeneralParamsMirror.SizesInfos.nbr_rungs = TheValue;
	TheValue = GetOneGeneralInfo( 4 );
	GeneralParamsMirror.SizesInfos.nbr_bits = TheValue;
	TheValue = GetOneGeneralInfo( 5 );
	GeneralParamsMirror.SizesInfos.nbr_words = TheValue;
	TheValue = GetOneGeneralInfo( 6 );
	GeneralParamsMirror.SizesInfos.nbr_counters = TheValue;
	TheValue = GetOneGeneralInfo( 7 );
	GeneralParamsMirror.SizesInfos.nbr_timers_iec = TheValue;
	TheValue = GetOneGeneralInfo( 8 );
	GeneralParamsMirror.SizesInfos.nbr_registers = TheValue;
	TheValue = GetOneGeneralInfo( 9 );
	GeneralParamsMirror.SizesInfos.register_list_size = TheValue;
	TheValue = GetOneGeneralInfo( 10 );
	GeneralParamsMirror.SizesInfos.nbr_phys_inputs = TheValue;
	TheValue = GetOneGeneralInfo( 11 );
	GeneralParamsMirror.SizesInfos.nbr_phys_outputs = TheValue;
	TheValue = GetOneGeneralInfo( 12 );
	GeneralParamsMirror.SizesInfos.nbr_phys_words_inputs = TheValue;
	TheValue = GetOneGeneralInfo( 13 );
	GeneralParamsMirror.SizesInfos.nbr_phys_words_outputs = TheValue;
	TheValue = GetOneGeneralInfo( 14 );
	GeneralParamsMirror.SizesInfos.nbr_arithm_expr = TheValue;
	TheValue = GetOneGeneralInfo( 15 );
	GeneralParamsMirror.SizesInfos.nbr_sections = TheValue;
	TheValue = GetOneGeneralInfo( 16 );
	GeneralParamsMirror.SizesInfos.nbr_symbols = TheValue;
#ifdef OLD_TIMERS_MONOS_SUPPORT
	TheValue = GetOneGeneralInfo( 17 );
	GeneralParamsMirror.SizesInfos.nbr_timers = TheValue;
	TheValue = GetOneGeneralInfo( 18 );
	GeneralParamsMirror.SizesInfos.nbr_monostables = TheValue;
#endif
}

void AddDevicesListToComboBox( MyGtkComboBox * pComboBox )
{
	int ScanDev = 0;
	do
	{
		gtk_combo_box_append_text( pComboBox, gettext(Devices[ ScanDev++ ]) );
	}
	while( Devices[ ScanDev ] );
}

GtkWidget * CreateIOConfPage( char ForInputs )
{
	static char * Labels[] = { N_("First %"), N_("Type"), N_("PortAdr(0x)/SubDev"), N_("1stChannel/GPIO"), N_("NbrChannels/GPIOs"), N_("Logic"), N_("ConfigData") };
	static int LabelsSize[] = { 105, 130, 130, 120, 130, 105, 105 };
	GtkWidget *scrolled_win;
//////	GtkWidget *vbox;
//////	GtkWidget *hbox[ (ForInputs?NBR_INPUTS_CONF:NBR_OUTPUTS_CONF)+1   +30];
	GtkWidget *table;
	int NumObj;
	int NumLine;
//	GList * ItemsDevices = NULL;
//	int ScanDev = 0;
	StrIOConf * pConf;
	GtkWidget *InputParamLabel[ NBR_IO_PARAMS];
	GtkWidget *OutputParamLabel[ NBR_IO_PARAMS ];
//int testpack;

//	do
//	{
//		ItemsDevices = g_list_append( ItemsDevices, Devices[ ScanDev++ ] );
//	}
//	while( Devices[ ScanDev ] );

//////////	vbox = gtk_vbox_new (FALSE, 0);

	table = gtk_table_new( NBR_IO_PARAMS, 1+(ForInputs?NBR_INPUTS_CONF:NBR_OUTPUTS_CONF), FALSE );

	scrolled_win = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW (scrolled_win),
                                    GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
//////////	gtk_box_pack_start(GTK_BOX (vbox), scrolled_win, TRUE, TRUE, 0);

	// here we add the view to the scrolled !
//	gtk_container_add(GTK_CONTAINER(scrolled_win), table);
	gtk_scrolled_window_add_with_viewport ( GTK_SCROLLED_WINDOW (scrolled_win), table );

	for (NumLine=-1; NumLine<(ForInputs?NBR_INPUTS_CONF:NBR_OUTPUTS_CONF); NumLine++ )
	{
//////		hbox[NumLine+1] = gtk_hbox_new (FALSE, 0);
//////		gtk_container_add (GTK_CONTAINER (vbox), hbox[NumLine+1]);
		
		for (NumObj=0; NumObj<NBR_IO_PARAMS; NumObj++)
		{
			if ( NumLine==-1 )
			{
				GtkWidget **IOParamLabel = ForInputs?&InputParamLabel[ NumObj ]:&OutputParamLabel[ NumObj ];
				if ( NumObj==0 )
					*IOParamLabel = gtk_label_new( ForInputs?(_("1st %I mapped")):(_("1st %Q mapped")) );
				else
					*IOParamLabel = gtk_label_new( gettext(Labels[ NumObj ]) );
				gtk_widget_set_size_request( *IOParamLabel, LabelsSize[NumObj],-1 );
//////				gtk_box_pack_start(GTK_BOX (hbox[ NumLine+1 ]), *IOParamLabel, FALSE, FALSE, 0);
				gtk_table_attach_defaults (GTK_TABLE (table), *IOParamLabel, NumObj, NumObj+1, NumLine+1, NumLine+2);
			}
			else
			{
				char BuffValue[ 30 ];
				if ( ForInputs )
					pConf = &InfosGene->InputsConf[ NumLine ];
				else
					pConf = &InfosGene->OutputsConf[ NumLine ];

				switch( NumObj )
				{
					/* For devices */
					case 1:
					{
						int ComboValueToDisplay = 0;
//////						if ( pConf->FirstClassicLadderIO==-1 )
//////						{
//////							ValueToDisplay = 0;
//////						}
//////						else
						{
							if ( pConf->DeviceType==DEVICE_TYPE_DIRECT_ACCESS )
								ComboValueToDisplay = 1;
							else if ( pConf->DeviceType==DEVICE_TYPE_DIRECT_CONFIG )
								ComboValueToDisplay = 2;
							else if ( pConf->DeviceType==DEVICE_TYPE_RASPBERRY_GPIO )
								ComboValueToDisplay = 3;
							else if ( pConf->DeviceType==DEVICE_TYPE_ATMEL_SAM_GPIO )
								ComboValueToDisplay = 4;
							else if ( pConf->DeviceType>=DEVICE_TYPE_COMEDI )
								ComboValueToDisplay = pConf->DeviceType-DEVICE_TYPE_COMEDI+4;
						}
						{
							GtkWidget **IOParamDevice = ForInputs?&InputDeviceParam[ NumLine ]:&OutputDeviceParam[ NumLine ];
//							*IOParamDevice = gtk_combo_new( );
//							gtk_combo_set_value_in_list( GTK_COMBO(*IOParamDevice), TRUE /*val*/, FALSE /*ok_if_empty*/ );
//							gtk_combo_set_popdown_strings( GTK_COMBO(*IOParamDevice), ItemsDevices );
							*IOParamDevice = gtk_combo_box_new_text();
							AddDevicesListToComboBox( MY_GTK_COMBO_BOX( *IOParamDevice ) );
							gtk_widget_set_size_request( *IOParamDevice,LabelsSize[NumObj],-1 );
//////							gtk_box_pack_start ( GTK_BOX (hbox[NumLine+1]), *IOParamDevice, FALSE, FALSE, 0 );
							gtk_table_attach_defaults (GTK_TABLE (table), *IOParamDevice, NumObj, NumObj+1, NumLine+1, NumLine+2);
//							gtk_entry_set_text((GtkEntry*)((GtkCombo *)*IOParamDevice)->entry,Devices[ ValueToDisplay ]);
							gtk_combo_box_set_active( GTK_COMBO_BOX( *IOParamDevice ), ComboValueToDisplay );
						}
						break;
					}
					/* For flags */
					case 5:
					{
						GtkWidget **IOParamFlag = ForInputs?&InputFlagParam[ NumLine ]:&OutputFlagParam[ NumLine ];
						*IOParamFlag = gtk_check_button_new_with_label( _("Inverted") );
						gtk_widget_set_size_request( *IOParamFlag,LabelsSize[NumObj],-1 );
//////						gtk_box_pack_start( GTK_BOX (hbox[NumLine+1]), *IOParamFlag, FALSE, FALSE, 0 );
						gtk_table_attach_defaults (GTK_TABLE (table), *IOParamFlag, NumObj, NumObj+1, NumLine+1, NumLine+2);
						if ( pConf->FlagInverted )
							gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( *IOParamFlag ), TRUE );
						break;
					}
					/* For numbers */
					default:
					{
						switch( NumObj )
						{
							case 0:
								if ( pConf->FirstClassicLadderIO==-1 )
									strcpy( BuffValue, "" );
								else
									sprintf( BuffValue, "%d", pConf->FirstClassicLadderIO );
								break;
							case 2:
								if ( pConf->DeviceType==DEVICE_TYPE_DIRECT_ACCESS || pConf->DeviceType==DEVICE_TYPE_DIRECT_CONFIG )
									sprintf( BuffValue, "%X", pConf->SubDevOrAdr );
								else
									sprintf( BuffValue, "%d", pConf->SubDevOrAdr );
								break;
							case 3:
								sprintf( BuffValue, "%d", pConf->FirstChannel ); break;
							case 4:
								sprintf( BuffValue, "%d", pConf->NbrConsecutivesChannels ); break;
							case 6:
								sprintf( BuffValue, "%d", pConf->ConfigData ); break;
						}
						{
							GtkWidget **IOParamEntry = ForInputs?&InputParamEntry[ NumLine ][ NumObj ]:&OutputParamEntry[ NumLine ][ NumObj ];
							*IOParamEntry = gtk_entry_new( );
							gtk_widget_set_size_request( *IOParamEntry,LabelsSize[NumObj],-1 );
//////							gtk_box_pack_start( GTK_BOX (hbox[NumLine+1]), *IOParamEntry, FALSE, FALSE, 0 );
							gtk_table_attach_defaults (GTK_TABLE (table), *IOParamEntry, NumObj, NumObj+1, NumLine+1, NumLine+2);
							gtk_entry_set_text( GTK_ENTRY(*IOParamEntry), BuffValue );
						}
						break;
					}
				}
			}
		}//for (NumObj=0;
	}
//////////	gtk_widget_show_all( vbox );
	gtk_widget_show_all( scrolled_win );

//TODO: I've not found how to not have all the hbox vertically expanded...?
/*for(testpack=0; testpack<30; testpack++)
{		
		hbox[(ForInputs?NBR_INPUTS_CONF:NBR_OUTPUTS_CONF)+1+testpack] = gtk_hbox_new (FALSE, 0);
		gtk_container_add (GTK_CONTAINER (vbox), hbox[(ForInputs?NBR_INPUTS_CONF:NBR_OUTPUTS_CONF)+1+testpack]);
//gtk_box_pack_start(GTK_BOX(vbox), hbox[ (ForInputs?NBR_INPUTS_CONF:NBR_OUTPUTS_CONF)+1+testpack ], TRUE, TRUE, 0);
		gtk_widget_show (hbox[(ForInputs?NBR_INPUTS_CONF:NBR_OUTPUTS_CONF)+1+testpack]);
}
*/
	
//////////	return vbox;
	return scrolled_win;
}

// no more usefull with new widgets gtk_combo_box_new_text( )...
/*int ConvComboToNum( char * text, char ** list )
{
	int Value = 0;
	char Found = FALSE;
	while( !Found && list[ Value ]!=NULL )
	{
		if ( strcmp( list[ Value ], text )==0 )
			Found = TRUE;
		else
			Value++;
	}
	return Value;
}*/
void GetIOSettings( char ForInputs )
{
	int NumObj;
	int NumLine;
	StrIOConf * pConf;
	int ComboVal;
	GtkWidget **IOParamDevice;
	GtkWidget **IOParamEntry;
	GtkWidget **IOParamFlag;
	char * text;
	for (NumLine=0; NumLine<(ForInputs?NBR_INPUTS_CONF:NBR_OUTPUTS_CONF); NumLine++ )
	{
		if ( ForInputs )
			pConf = &InfosGene->InputsConf[ NumLine ];
		else
			pConf = &InfosGene->OutputsConf[ NumLine ];

		pConf->FirstClassicLadderIO = -1;
		pConf->FlagInverted = 0;

		IOParamDevice = ForInputs?&InputDeviceParam[ NumLine ]:&OutputDeviceParam[ NumLine ];
//		ComboVal = ConvComboToNum( (char *)gtk_entry_get_text((GtkEntry *)((GtkCombo *)*IOParamDevice)->entry), Devices );
		ComboVal = gtk_combo_box_get_active( GTK_COMBO_BOX( *IOParamDevice ) );
		if ( ComboVal>0 )
		{
			int FirstIO = -1;
			int DeviceTypeValue = DEVICE_TYPE_NONE;
			if ( ComboVal==1 )
				DeviceTypeValue = DEVICE_TYPE_DIRECT_ACCESS;
			else if ( ComboVal==2 )
				DeviceTypeValue = DEVICE_TYPE_DIRECT_CONFIG;
			else if ( ComboVal==3 )
				DeviceTypeValue = DEVICE_TYPE_RASPBERRY_GPIO;
			else if ( ComboVal==4 )
				DeviceTypeValue = DEVICE_TYPE_ATMEL_SAM_GPIO;
			else
				DeviceTypeValue = DEVICE_TYPE_COMEDI+ComboVal-4;
			IOParamFlag = ForInputs?&InputFlagParam[ NumLine ]:&OutputFlagParam[ NumLine ];
			if ( gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON( *IOParamFlag ) ) )
				pConf->FlagInverted = 1;
			for (NumObj=0; NumObj<NBR_IO_PARAMS; NumObj++)
			{
				IOParamEntry = ForInputs?&InputParamEntry[ NumLine ][ NumObj ]:&OutputParamEntry[ NumLine ][ NumObj ];
				switch( NumObj )
				{
					case 0:
						text = (char *)gtk_entry_get_text((GtkEntry *)*IOParamEntry);
						if ( text[0]!='\0' )
							FirstIO = atoi( text );
						break;
					case 2:
						text = (char *)gtk_entry_get_text((GtkEntry *)*IOParamEntry);
						if ( DeviceTypeValue==DEVICE_TYPE_DIRECT_ACCESS || DeviceTypeValue==DEVICE_TYPE_DIRECT_CONFIG )
							sscanf( text, "%X", &pConf->SubDevOrAdr );
						else
							pConf->SubDevOrAdr = atoi( text );
						break;
					case 3:
						text = (char *)gtk_entry_get_text((GtkEntry *)*IOParamEntry);
						pConf->FirstChannel = atoi( text );
						break;
					case 4:
						text = (char *)gtk_entry_get_text((GtkEntry *)*IOParamEntry);
						pConf->NbrConsecutivesChannels = atoi( text );
						break;
					case 6:
						text = (char *)gtk_entry_get_text((GtkEntry *)*IOParamEntry);
						pConf->ConfigData = (unsigned char)atoi( text );
						break;
				}
			}
			/* verify if not overflowing */
			if ( FirstIO+pConf->NbrConsecutivesChannels>( ForInputs?NBR_PHYS_INPUTS:NBR_PHYS_OUTPUTS ) )
			{
				printf("Error in I/O conf: overflow for Ixx or Qxx mapping detected...\n" );
				FirstIO = -1;
			}
			/* done at the end, do not forget multi-task ! */
			pConf->FirstClassicLadderIO = FirstIO;
			pConf->DeviceType = DeviceTypeValue;
		}//if ( ComboVal>0 )
	}
}

#ifdef MODBUS_IO_MASTER
void FillComboBoxConfigSlavesList( MyGtkComboBox * pComboBox, char ListForStatsSelect, char CleanUpBefore )
{
	int ScanSlave;
	char Buff[ 80 ];
	if ( CleanUpBefore )
	{
		for( ScanSlave=0; ScanSlave<NBR_MODBUS_SLAVES+1; ScanSlave++ )
			gtk_combo_box_remove_text( pComboBox, 0 );
	}
	if ( !ListForStatsSelect )
		gtk_combo_box_append_text( pComboBox, _("None") );
	for( ScanSlave=0; ScanSlave<NBR_MODBUS_SLAVES; ScanSlave++ )
	{
		/* take name from widgets (perhaps modified since displayed...) */
		char * text;
		if ( !ListForStatsSelect )
		{
			GtkWidget **IOParamEntry;
			IOParamEntry = &ModbusSlaveParamEntry[ ScanSlave ][ 1 ];
			text = (char *)gtk_entry_get_text((GtkEntry *)*IOParamEntry);
		}
		else
		{
			text = ModbusSlaveList[ ScanSlave ].SlaveAdr;
		}
		if ( text[0]=='\0' )
		{
			sprintf( Buff, "%d: %s", ScanSlave, _("not defined") );
		}
		else
		{
			// Modbus/RTU on serial used ?
			if ( ModbusConfig.ModbusSerialPortNameUsed[ 0 ]!='\0' )
				sprintf( Buff, "%d: %s%s", ScanSlave, "SerialAdr", text );
			else
				sprintf( Buff, "%d: %s", ScanSlave, text );
		}
		gtk_combo_box_append_text( pComboBox, Buff );
	}
}
void FillComboBoxReqType( MyGtkComboBox * pComboBox, char * ListTextsCombo[] )
{
	int ScanDev = 0;
	do
	{
		gtk_combo_box_append_text( pComboBox, gettext(ListTextsCombo[ ScanDev++ ]) );
	}
	while( ListTextsCombo[ ScanDev ] );
}
void UpdateSlaveListsOnRequestPage( void )
{
	int NumLine;
	for (NumLine=0; NumLine<NBR_MODBUS_MASTER_REQ; NumLine++ )
	{
		GtkWidget **IOParamEntry;
		StrModbusMasterReq * pConf = &ModbusMasterReq[ NumLine ];
		IOParamEntry = &ModbusReqParamEntry[ NumLine ][ 0/*for SlaveNum*/ ];
		FillComboBoxConfigSlavesList( MY_GTK_COMBO_BOX( *IOParamEntry ), FALSE/*ListForStatsSelect*/, TRUE/*CleanUpBefore*/ );
		gtk_combo_box_set_active( GTK_COMBO_BOX( *IOParamEntry ), pConf->SlaveListNum+1 );
	}
}
GtkWidget * CreateModbusModulesIO( void )
{
	static char * Labels[] = { N_("Slave No"), N_("Request Type"), N_("1st Modbus Ele."), N_("Nbr of Ele"), N_("Logic"), N_("1st I/Q/IW/QW mapped") };
//	GtkWidget *vbox;
//	GtkWidget *hbox[ NBR_MODBUS_MASTER_REQ+1 ];
	GtkWidget *table;
	GtkWidget *scrolled_win;
	int NumObj;
	int NumLine;
	GtkWidget *ModbusParamLabel[ NBR_MODBUS_REQ_PARAMS];

//	vbox = gtk_vbox_new (FALSE, 0);
	table = gtk_table_new( NBR_MODBUS_REQ_PARAMS, 1+NBR_MODBUS_MASTER_REQ, FALSE );
	scrolled_win = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW (scrolled_win),
                                    GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	gtk_scrolled_window_add_with_viewport ( GTK_SCROLLED_WINDOW (scrolled_win), table );

	for (NumLine=-1; NumLine<NBR_MODBUS_MASTER_REQ; NumLine++ )
	{
//		hbox[NumLine+1] = gtk_hbox_new (FALSE, 0);
//		gtk_container_add (GTK_CONTAINER (vbox), hbox[NumLine+1]);

		for (NumObj=0; NumObj<NBR_MODBUS_REQ_PARAMS; NumObj++)
		{
			GtkWidget **CurrentWidget;
/*GTK3			int PixelsLength = 120;
			switch( NumObj )
			{
				case 0:
					PixelsLength = 150;
					break;
				case 1:
					PixelsLength = 230;
					break;
				case 3:
					PixelsLength = 70;
					break;
				case 4:
					PixelsLength = 90;
					break;
				case 5:
					PixelsLength = 160;
					break;
			}*/
			/* labels line ? */
			if ( NumLine==-1 )
			{
				CurrentWidget = &ModbusParamLabel[ NumObj ];
				*CurrentWidget = gtk_label_new( gettext(Labels[ NumObj ]) );
			}
			else
			{
				StrModbusMasterReq * pConf = &ModbusMasterReq[ NumLine ];
				switch( NumObj )
				{
					/* For slave index + req type (combo-list) */
					case 0:
					case 1:
					{
						CurrentWidget = &ModbusReqParamEntry[ NumLine ][ NumObj ];
						*CurrentWidget = gtk_combo_box_new_text( );
						if ( NumObj==0 )
						{
							FillComboBoxConfigSlavesList( MY_GTK_COMBO_BOX( *CurrentWidget ), FALSE/*ListForStatsSelect*/, FALSE/*CleanUpBefore*/ );
							gtk_combo_box_set_active( GTK_COMBO_BOX( *CurrentWidget ), pConf->SlaveListNum+1 );
						}
						else
						{
							FillComboBoxReqType( MY_GTK_COMBO_BOX( *CurrentWidget ), ModbusReqType );
							gtk_combo_box_set_active( GTK_COMBO_BOX( *CurrentWidget ), pConf->TypeReq );
						}
						break;
					}
					/* For flags (checkbutton)*/
					case 4:
					{
						CurrentWidget = &ModbusReqParamEntry[ NumLine ][ NumObj ];
						*CurrentWidget = gtk_check_button_new_with_label( _("Inverted") );
						if ( pConf->LogicInverted )
							gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( *CurrentWidget ), TRUE );
						break;
					}
					/* For numbers/strings (edit widgets)*/
					default:
					{
						char BuffValue[ 30 ];
						switch( NumObj )
						{
							case 2:
								sprintf( BuffValue, "%d", pConf->FirstModbusElement );
								break;
							case 3:
								sprintf( BuffValue, "%d", pConf->NbrModbusElements );
								break;
							case 5:
								sprintf( BuffValue, "%d", pConf->OffsetVarMapped );
								break;
						}
						CurrentWidget = &ModbusReqParamEntry[ NumLine ][ NumObj ];
						*CurrentWidget = gtk_entry_new( );
						gtk_entry_set_text( GTK_ENTRY(*CurrentWidget), BuffValue );
						break;
					}
				}//switch( NumObj )
			}//!if ( NumLine==-1 )
//GTK3			gtk_widget_set_usize(*CurrentWidget,PixelsLength,0);
//			gtk_box_pack_start( GTK_BOX (hbox[NumLine+1]), *CurrentWidget, FALSE, FALSE, 0 );
			gtk_table_attach_defaults (GTK_TABLE (table), *CurrentWidget, NumObj, NumObj+1, NumLine+1, NumLine+2);
		}//for (NumObj
	}//for (NumLine
//	gtk_widget_show_all( vbox );
//	return vbox;
	gtk_widget_show_all( scrolled_win );
	return scrolled_win;
}
void GetModbusModulesIOSettings( void )
{
	int NumObj;
	int NumLine;
	int SlaveSelected;
	for (NumLine=0; NumLine<NBR_MODBUS_MASTER_REQ; NumLine++ )
	{
		int MaxVars = 0;
		char DoVerify = FALSE;
		SlaveSelected = -1;
		StrModbusMasterReq * pConf = &ModbusMasterReq[ NumLine ];

		for (NumObj=0; NumObj<NBR_MODBUS_REQ_PARAMS; NumObj++)
		{
			GtkWidget **IOParamEntry;
			char * text;
			IOParamEntry = &ModbusReqParamEntry[ NumLine ][ NumObj ];
			switch( NumObj )
			{
				case 0://slave address
					SlaveSelected = gtk_combo_box_get_active( GTK_COMBO_BOX(*IOParamEntry) )-1; 
					break;
				case 1://type of request
					pConf->TypeReq = gtk_combo_box_get_active( GTK_COMBO_BOX(*IOParamEntry) );
					break;
				case 2://first element address
					text = (char *)gtk_entry_get_text((GtkEntry *)*IOParamEntry);
					pConf->FirstModbusElement = atoi( text );
					break;
				case 3://number of requested items
					text = (char *)gtk_entry_get_text((GtkEntry *)*IOParamEntry);
					pConf->NbrModbusElements = atoi( text );
					break;
				case 4://invert logic (instead of thinking of that everywhere later...)
					pConf->LogicInverted = ( gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON( *IOParamEntry ) ) )?1:0;
					break;
				case 5:// first classicladder variable map location
					text = (char *)gtk_entry_get_text((GtkEntry *)*IOParamEntry);
					pConf->OffsetVarMapped = atoi( text );
					break;
			}
		}//for (NumObj=0;
		/* a slave is defined ? */
		if ( SlaveSelected!=-1 )
		{
			/* verify if not overflowing */
			switch( pConf->TypeReq )
			{
				case MODBUS_REQ_INPUTS_READ: MaxVars = GetSizeVarsForTypeVar( ModbusConfig.MapTypeForReadInputs ); DoVerify = TRUE; break;
				case MODBUS_REQ_COILS_WRITE: MaxVars = GetSizeVarsForTypeVar( ModbusConfig.MapTypeForWriteCoils ); DoVerify = TRUE; break;
				case MODBUS_REQ_COILS_READ: MaxVars = GetSizeVarsForTypeVar( ModbusConfig.MapTypeForReadCoils ); DoVerify = TRUE; break;
				case MODBUS_REQ_INPUT_REGS_READ: MaxVars = GetSizeVarsForTypeVar( ModbusConfig.MapTypeForReadInputRegs ); DoVerify = TRUE; break;
				case MODBUS_REQ_HOLD_REGS_WRITE: MaxVars = GetSizeVarsForTypeVar( ModbusConfig.MapTypeForWriteHoldRegs ); DoVerify = TRUE; break;
				case MODBUS_REQ_HOLD_REGS_READ: MaxVars = GetSizeVarsForTypeVar( ModbusConfig.MapTypeForReadHoldRegs ); DoVerify = TRUE; break;
			}
			if ( DoVerify )
			{
				if ( pConf->OffsetVarMapped+pConf->NbrModbusElements>MaxVars )
				{
					printf("Error in I/O modbus conf: overflow for I,Q,B,IQ,WQ or W mapping detected...ASKED=%i,MAX=%i\n",  pConf->OffsetVarMapped+pConf->NbrModbusElements,MaxVars);
					SlaveSelected = -1;
					ShowMessageBoxError( _("Overflow error for I,Q,B,IQ,WQ or W mapping detected...") );
				}
			}
		}
		/* done at the end, do not forget multi-task ! */
		pConf->SlaveListNum = SlaveSelected;
	}//for (NumLine=0; 
}
GtkWidget * CreateModbusSlavesPage( void )
{
	static char * Labels[] = { N_("Slave No"), N_("Slave Address"), N_("TCP/UDP mode"), N_("Module Informations") };
//	GtkWidget *vbox;
//	GtkWidget *hbox[ 1+NBR_MODBUS_SLAVES ];
	GtkWidget *table;
	int NumObj;
	int NumLine;
	GtkWidget *ModbusParamLabel[ NBR_MODBUS_SLAVES_PARAMS];

//ForGTK3, deprecated...	GtkTooltips * WidgetTooltipsAdr = gtk_tooltips_new();

//	vbox = gtk_vbox_new (FALSE, 0);
	table = gtk_table_new( NBR_MODBUS_SLAVES_PARAMS, 1+NBR_MODBUS_SLAVES, FALSE );

	for (NumLine=-1; NumLine<NBR_MODBUS_SLAVES; NumLine++ )
	{
//		hbox[NumLine+1] = gtk_hbox_new (FALSE, 0);
//		gtk_container_add (GTK_CONTAINER (vbox), hbox[NumLine+1]);

		for (NumObj=0; NumObj<NBR_MODBUS_SLAVES_PARAMS; NumObj++)
		{
			GtkWidget **CurrentWidget;
/*GTK3			int PixelsLength = 70;
			switch( NumObj )
			{
				case 1:
					PixelsLength = 180;
					break;
				case 2:
					PixelsLength = 160;
					break;
				case 3:
					PixelsLength = 240;
					break;
			}*/
			/* labels line ? */
			if ( NumLine==-1 )
			{
				CurrentWidget = &ModbusParamLabel[ NumObj ];
				*CurrentWidget = gtk_label_new( gettext(Labels[ NumObj ]) );
			}
			else
			{
				StrModbusSlave * pSlave = &ModbusSlaveList[ NumLine ];
				switch( NumObj )
				{
					/* For flags (checkbutton)*/
					case 2:
					{
						CurrentWidget = &ModbusSlaveParamEntry[ NumLine ][ NumObj ];
						*CurrentWidget = gtk_check_button_new_with_label( _("UDP instead of TCP") );
						if ( pSlave->UseUdpInsteadOfTcp )
							gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( *CurrentWidget ), TRUE );
						// can not be used for now...
						gtk_widget_set_sensitive( *CurrentWidget, FALSE );
						break;
					}
					/* For numbers/strings (edit widgets)*/
					default:
					{
						char BuffValue[ 30 ];
						switch( NumObj )
						{
							case 0:
								sprintf( BuffValue, "%d :", NumLine );
								break;
							case 1:
								strcpy( BuffValue, pSlave->SlaveAdr );
								break;
							case 3:
								strcpy( BuffValue, pSlave->NameInfo );
								break;
						}
						CurrentWidget = &ModbusSlaveParamEntry[ NumLine ][ NumObj ];
						if ( NumObj==0 )
						{
							*CurrentWidget = gtk_label_new( BuffValue );
							gtk_widget_set_size_request( *CurrentWidget, 80, -1 );
						}
						else
						{
							*CurrentWidget = gtk_entry_new( );
							gtk_entry_set_text( GTK_ENTRY(*CurrentWidget), BuffValue );
							if ( NumObj==1 )
							{
								//ForGTK3, deprecated... gtk_tooltips_set_tip( WidgetTooltipsAdr, *CurrentWidget, "SerialAdr -or- AdrIP -or- AdrIP:Port", NULL );
								gtk_widget_set_tooltip_text( *CurrentWidget, _("SerialAdr -or- AdrIP -or- AdrIP:Port") );
							}
						}
						break;
					}
				}//switch( NumObj )
			}//!if ( NumLine==-1 )
//			gtk_widget_set_usize(*CurrentWidget,PixelsLength,0);
//////			gtk_widget_set_size_request( *CurrentWidget,PixelsLength,-1 );
//			gtk_box_pack_start( GTK_BOX (hbox[NumLine+1]), *CurrentWidget, FALSE, FALSE, 0 );
			gtk_table_attach_defaults (GTK_TABLE (table), *CurrentWidget, NumObj, NumObj+1, NumLine+1, NumLine+2);
		}//for (NumObj
	}//for (NumLine
//	gtk_widget_show_all( vbox );
//	return vbox;
	gtk_widget_show_all( table );
	return table;
}
void GetModbusSlavesSettings( void )
{
	int NumObj;
	int NumLine;
	for (NumLine=0; NumLine<NBR_MODBUS_SLAVES; NumLine++ )
	{
		StrModbusSlave * pSlave = &ModbusSlaveList[ NumLine ];

		for (NumObj=0; NumObj<NBR_MODBUS_SLAVES_PARAMS; NumObj++)
		{
			GtkWidget **IOParamEntry;
			char * text;
			IOParamEntry = &ModbusSlaveParamEntry[ NumLine ][ NumObj ];
			switch( NumObj )
			{
				case 0://index, not a parameter !
					break;
				case 1://address
					text = (char *)gtk_entry_get_text((GtkEntry *)*IOParamEntry);
					strncpy( pSlave->SlaveAdr, text, LGT_SLAVE_ADR );
					pSlave->SlaveAdr[ LGT_SLAVE_ADR-1 ] = '\0';
					break;
				case 2://tdp/tcp mode
					pSlave->UseUdpInsteadOfTcp = ( gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON( *IOParamEntry ) ) )?1:0;
					break;
				case 3://information
					text = (char *)gtk_entry_get_text((GtkEntry *)*IOParamEntry);
					strncpy( pSlave->NameInfo, text, LGT_SLAVE_NAME );
					pSlave->NameInfo[ LGT_SLAVE_NAME-1 ] = '\0';
					break;
			}
		}//for (NumObj=0;
	}//for (NumLine=0; 
}

// return nbr of fields found
int SplitCommasFieldsInPointersArray( char * LineDatas, char * PtrFieldsDatasFound[], int NbrMaxFields )
{
	int ScanField;
	for( ScanField=0; ScanField<NbrMaxFields; ScanField++ )
		PtrFieldsDatasFound[ ScanField ] = NULL;
	ScanField = 0;
	PtrFieldsDatasFound[ ScanField++ ] = LineDatas;
	do
	{
		do
		{
			// comma ?
			if ( *LineDatas==',' && *(LineDatas+1)!='\0' )
			{
				// test if not an empty field...
				if ( *(LineDatas+1)!=',' )
				{
					PtrFieldsDatasFound[ ScanField ] = LineDatas+1;
					*LineDatas = '\0';
				}
				ScanField++;
			}
			LineDatas++;
		}
		while( ScanField<NbrMaxFields-1 && *LineDatas!='\0' );
	}
	while( ScanField<NbrMaxFields-1 && *LineDatas!='\0' );
	return ScanField;
}
#define MODBUS_COM_PARAMS_SIZE_X_TABLE 5
GtkWidget * CreateModbusComParametersPage( void )
{
//	GtkWidget *vbox;
//	GtkWidget *hbox[ NBR_COM_PARAMS ];
	GtkWidget *hbox;
	GtkWidget *table;
	GtkWidget * LabelComParam[ NBR_COM_PARAMS ];
	int NumLine;
	char BuffLabel[ 50 ];
	char BuffValue[ 100 ];

//	vbox = gtk_vbox_new (FALSE/*homogeneous*/, 0);
	hbox = gtk_hbox_new (FALSE/*homogeneous*/, 0);
//	gtk_widget_show (vbox);
	table = gtk_table_new( MODBUS_COM_PARAMS_SIZE_X_TABLE, NBR_COM_PARAMS, FALSE );
	gtk_box_pack_start (GTK_BOX(hbox), table, TRUE/*expand*/, FALSE/*fill*/, 0);
	for( NumLine=0; NumLine<NBR_COM_PARAMS; NumLine++ )
	{
//		hbox[NumLine] = gtk_hbox_new (FALSE, 0);
//		gtk_container_add (GTK_CONTAINER (vbox), hbox[NumLine]);
//		gtk_widget_show (hbox[NumLine]);
		switch( NumLine )
		{
			case 0:
				sprintf( BuffLabel, _("Modbus master Serial port (blank = IP mode)") );
				strcpy( BuffValue, ModbusConfig.ModbusSerialPortNameUsed );
				break;
			case 1:
				sprintf( BuffLabel, _("Serial baud rate") );
				sprintf( BuffValue, "%d", ModbusConfig.ModbusSerialSpeed );
				break;
			case 2:
				sprintf( BuffLabel, _("Serial nbr. data bits") );
				sprintf( BuffValue, "%d,5,6,7,8", ModbusConfig.ModbusSerialDataBits-5 );
				break;
			case 3:
				sprintf( BuffLabel, _("Serial parity") );
				sprintf( BuffValue, "%d,None,Odd,Even", ModbusConfig.ModbusSerialParity );
				break;
			case 4:
				sprintf( BuffLabel, _("Serial nbr. stops bits") );
				sprintf( BuffValue, "%d,1,2", ModbusConfig.ModbusSerialStopBits-1 );
				break;
			case 5:
				sprintf( BuffLabel, _("After transmit pause - milliseconds") );
				sprintf( BuffValue, "%d", ModbusConfig.ModbusTimeAfterTransmit );
				break;
			case 6:
				sprintf( BuffLabel, _("After receive pause - milliseconds") );
				sprintf( BuffValue, "%d", ModbusConfig.ModbusTimeInterFrame );
				break;
			case 7:
				sprintf( BuffLabel, _("Request Timeout length - milliseconds") );
				sprintf( BuffValue, "%d", ModbusConfig.ModbusTimeOutReceipt );
				break;
			case 8:
				sprintf( BuffLabel, _("Use RTS signal to send") );
				sprintf( BuffValue, "%d,NO,YES", ModbusConfig.ModbusSerialUseRtsToSend );
				break;
			case 9:
				sprintf( BuffLabel, _("Modbus element offset") );
				sprintf( BuffValue, "%d,0,1", ModbusConfig.ModbusEleOffset );
				break;
			case 10:
				sprintf( BuffLabel, _("Debug level") );
				sprintf( BuffValue, "%d,%s,%s 1,%s 2,%s", ModbusConfig.ModbusDebugLevel, _("QUIET"), _("LEVEL"), _("LEVEL"), _("VERBOSE") );
				break;
			case 11:
				sprintf( BuffLabel, _("Read inputs map to") );
				sprintf( BuffValue, "%d,\%%B,\%%Q,\%%I", ModbusConfig.MapTypeForReadInputs==VAR_MEM_BIT?0:(ModbusConfig.MapTypeForReadInputs==VAR_PHYS_OUTPUT?1:2) );
				break;
			case 12:
				sprintf( BuffLabel, _("Read coils map to") );
				sprintf( BuffValue, "%d,\%%B,\%%Q,\%%I", ModbusConfig.MapTypeForReadCoils==VAR_MEM_BIT?0:(ModbusConfig.MapTypeForReadCoils==VAR_PHYS_OUTPUT?1:2) );
				break;
			case 13:
				sprintf( BuffLabel, _("Write coils map from") );
				sprintf( BuffValue, "%d,\%%B,\%%Q,\%%I", ModbusConfig.MapTypeForWriteCoils==VAR_MEM_BIT?0:(ModbusConfig.MapTypeForWriteCoils==VAR_PHYS_OUTPUT?1:2) );
				break;
			case 14:
				sprintf( BuffLabel, _("Read input registers map to") );
				sprintf( BuffValue, "%d,\%%W,\%%QW,\%%IW", ModbusConfig.MapTypeForReadInputRegs==VAR_MEM_WORD?0:(ModbusConfig.MapTypeForReadInputRegs==VAR_PHYS_WORD_OUTPUT?1:2) );
				break;
			case 15:
				sprintf( BuffLabel, _("Read hold registers map to") );
				sprintf( BuffValue, "%d,\%%W,\%%QW,\%%IW", ModbusConfig.MapTypeForReadHoldRegs==VAR_MEM_WORD?0:(ModbusConfig.MapTypeForReadHoldRegs==VAR_PHYS_WORD_OUTPUT?1:2) );
				break;
			case 16:
				sprintf( BuffLabel, _("Write hold registers map from") );
				sprintf( BuffValue, "%d,\%%W,\%%QW,\%%IW", ModbusConfig.MapTypeForWriteHoldRegs==VAR_MEM_WORD?0:(ModbusConfig.MapTypeForWriteHoldRegs==VAR_PHYS_WORD_OUTPUT?1:2) );
				break;
		}

		/* Labels */
		LabelComParam[NumLine] = gtk_label_new(BuffLabel);
//GTK3		gtk_widget_set_usize( LabelComParam[NumLine],320,0 );
//		gtk_box_pack_start( GTK_BOX(hbox[NumLine]), LabelComParam[NumLine], FALSE, FALSE, 0 );
		gtk_table_attach_defaults (GTK_TABLE (table), LabelComParam[ NumLine ], 0, 1, NumLine, NumLine+1);
//		gtk_widget_show( LabelComParam[NumLine] );

		if ( NumLine<=1 || ( NumLine>=5 && NumLine<=7 ) )
		{
			/* Simple Integer Values */
			EntryComParam[NumLine] = gtk_entry_new();
//GTK3			gtk_widget_set_usize( EntryComParam[NumLine],125,0 );
//			gtk_box_pack_start( GTK_BOX(hbox[NumLine]), EntryComParam[NumLine], FALSE, FALSE, 0 );
			gtk_table_attach_defaults (GTK_TABLE (table), EntryComParam[ NumLine ], 1, MODBUS_COM_PARAMS_SIZE_X_TABLE, NumLine, NumLine+1);
//			gtk_widget_show( EntryComParam[NumLine] );
			gtk_entry_set_text( GTK_ENTRY(EntryComParam[NumLine]), BuffValue );
		}
		else
		{
			/* Radio buttons Values */
			/* BuffValue: first=n° selected , others=labels for each radio button */
			char * PtrArraysCsv[10];
			int CreateRadioBut;
			for( CreateRadioBut=0; CreateRadioBut<NBR_RADIO_BUT_COM_PARAMS; CreateRadioBut++ )
				RadioButComParams[ NumLine ][ CreateRadioBut ] = NULL;
			int NbrInfos = SplitCommasFieldsInPointersArray( BuffValue, PtrArraysCsv, 10 );
			if ( NbrInfos>2 )
			{
				int ValueSelected = atoi( PtrArraysCsv[0] );
				for( CreateRadioBut=0; CreateRadioBut<NbrInfos-1; CreateRadioBut++ )
				{
					char * label = PtrArraysCsv[1+CreateRadioBut];
					if ( CreateRadioBut==0 )
						RadioButComParams[ NumLine ][ CreateRadioBut ]= gtk_radio_button_new_with_label( NULL, label );
					else
						RadioButComParams[ NumLine ][ CreateRadioBut ]= gtk_radio_button_new_with_label_from_widget( GTK_RADIO_BUTTON(RadioButComParams[NumLine][0]), label );
//					gtk_box_pack_start (GTK_BOX (hbox[NumLine]), RadioButComParams[ NumLine ][ CreateRadioBut ], FALSE, TRUE, 0);
					gtk_table_attach_defaults (GTK_TABLE (table), RadioButComParams[ NumLine ][ CreateRadioBut ], 1+CreateRadioBut, 2+CreateRadioBut, NumLine, NumLine+1);
//					gtk_widget_show( RadioButComParams[ NumLine ][ CreateRadioBut ] );
					if ( CreateRadioBut==ValueSelected )
						gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( RadioButComParams[ NumLine ][ CreateRadioBut ] ), TRUE);
				}
			}
		}
	}
	gtk_widget_show_all(hbox);
//	return vbox;
	return hbox;
}
int GetRadioButValueSelected( int NumLineToSee )
{
	int Sel = 0;
	int ScanRadioBut;
	for( ScanRadioBut=0; ScanRadioBut<NBR_RADIO_BUT_COM_PARAMS; ScanRadioBut++ )
	{
		if ( RadioButComParams[ NumLineToSee ][ ScanRadioBut ]!=NULL )
		{
			if ( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( RadioButComParams[ NumLineToSee ][ ScanRadioBut ] ) ) )
				Sel = ScanRadioBut;
		}
	}
	return Sel;	
}
void GetModbusComParameters( void )
{
	strcpy( ModbusConfig.ModbusSerialPortNameUsed, gtk_entry_get_text(GTK_ENTRY( EntryComParam[ 0 ] )));
	ModbusConfig.ModbusSerialSpeed = atoi( gtk_entry_get_text(GTK_ENTRY( EntryComParam[ 1 ] )) );
	ModbusConfig.ModbusSerialDataBits = GetRadioButValueSelected( 2 )+5;
	ModbusConfig.ModbusSerialParity = GetRadioButValueSelected( 3 ); 
	ModbusConfig.ModbusSerialStopBits = GetRadioButValueSelected( 4 )+1;
	ModbusConfig.ModbusTimeAfterTransmit = atoi( gtk_entry_get_text(GTK_ENTRY( EntryComParam[ 5 ] )) );
	ModbusConfig.ModbusTimeInterFrame = atoi( gtk_entry_get_text(GTK_ENTRY( EntryComParam[ 6 ] )) );
	ModbusConfig.ModbusTimeOutReceipt = atoi( gtk_entry_get_text(GTK_ENTRY( EntryComParam[ 7 ] )) );
	ModbusConfig.ModbusSerialUseRtsToSend = GetRadioButValueSelected( 8 );
	ModbusConfig.ModbusEleOffset = GetRadioButValueSelected( 9 );
	ModbusConfig.ModbusDebugLevel = GetRadioButValueSelected( 10 );
	// ! after here, 2 tests per line... ( if a parameter is added before ! ;-) )
	ModbusConfig.MapTypeForReadInputs = GetRadioButValueSelected( 11 )==0?VAR_MEM_BIT:(GetRadioButValueSelected( 11 )==1?VAR_PHYS_OUTPUT:VAR_PHYS_INPUT);
	ModbusConfig.MapTypeForReadCoils = GetRadioButValueSelected( 12 )==0?VAR_MEM_BIT:(GetRadioButValueSelected( 12 )==1?VAR_PHYS_OUTPUT:VAR_PHYS_INPUT);
	ModbusConfig.MapTypeForWriteCoils = GetRadioButValueSelected( 13 )==0?VAR_MEM_BIT:(GetRadioButValueSelected( 13 )==1?VAR_PHYS_OUTPUT:VAR_PHYS_INPUT);
	ModbusConfig.MapTypeForReadInputRegs = GetRadioButValueSelected( 14 )==0?VAR_MEM_WORD:(GetRadioButValueSelected( 14 )==1?VAR_PHYS_WORD_OUTPUT:VAR_PHYS_WORD_INPUT);
	ModbusConfig.MapTypeForReadHoldRegs = GetRadioButValueSelected( 15 )==0?VAR_MEM_WORD:(GetRadioButValueSelected( 15 )==1?VAR_PHYS_WORD_OUTPUT:VAR_PHYS_WORD_INPUT);
	ModbusConfig.MapTypeForWriteHoldRegs = GetRadioButValueSelected( 16 )==0?VAR_MEM_WORD:(GetRadioButValueSelected( 16 )==1?VAR_PHYS_WORD_OUTPUT:VAR_PHYS_WORD_INPUT);
}
#endif


#ifdef COMPLETE_PLC
GtkWidget * CreateConfigEventsPage( void )
{
	static char * Labels[] = { N_("1st %Bxxxx"), N_("Nbr Of %B"), N_("Symbol"), N_("Text event"), N_("Level(>0=Def)"), N_("Forward Remote Alarms Slots") };
//	GtkWidget *vbox;
//	GtkWidget *hbox[ NBR_CONFIG_EVENTS_LOG+2 ];
	GtkWidget *table;
	GtkWidget *scrolled_win;
	int NumObj;
	int NumLine;
	StrConfigEventLog * pCfgEvtLog;
	char BuffValue[ 40 ];

//	vbox = gtk_vbox_new (FALSE, 0);
	table = gtk_table_new( NBR_CONFIG_EVENTS_PARAMS, 1+NBR_CONFIG_EVENTS_LOG, FALSE );
	scrolled_win = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW (scrolled_win),
                                    GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	gtk_scrolled_window_add_with_viewport ( GTK_SCROLLED_WINDOW (scrolled_win), table );

	for (NumLine=-1; NumLine<NBR_CONFIG_EVENTS_LOG; NumLine++ )
	{
//		hbox[NumLine+1] = gtk_hbox_new (FALSE, 0);
//		gtk_container_add (GTK_CONTAINER (vbox), hbox[NumLine+1]);

		for (NumObj=0; NumObj<NBR_CONFIG_EVENTS_PARAMS; NumObj++)
		{
			switch( NumLine )
			{
				case -1:
				{
					if ( NumObj<=5 )
					{
						GtkWidget * ParamTitleLabel;
/*GTK3						int PixelsLength = 100;
						switch( NumObj )
						{
							case 3:
								PixelsLength = 220;
								break;
						}*/
						ParamTitleLabel = gtk_label_new( gettext(Labels[ NumObj ]) );
//GTK3						if ( NumObj<5 )
//GTK3							gtk_widget_set_usize(ParamTitleLabel,PixelsLength,0);
//						gtk_box_pack_start(GTK_BOX (hbox[ NumLine+1 ]), ParamTitleLabel, FALSE, FALSE, 0);
						// remote alarm checkboxes on many columns...
						gtk_table_attach_defaults (GTK_TABLE (table), ParamTitleLabel, NumObj, ( NumObj<5 )?(NumObj+1):(NBR_CONFIG_EVENTS_PARAMS-1), NumLine+1, NumLine+2);
					}
					break;
				}
				default:
				{
					GtkWidget **ParamEntry = &EventConfigParamEntry[ NumLine ][ NumObj ];
					pCfgEvtLog = &ConfigEventLog[ NumLine ];
					if ( NumObj>=5 )
					{
						char Buff[ 20 ];
						sprintf(Buff, "'%d'", NumObj-5 );
						*ParamEntry = gtk_check_button_new_with_label( Buff );
//						gtk_box_pack_start( GTK_BOX (hbox[NumLine+1]), *ParamEntry, FALSE, FALSE, 0 );
						gtk_table_attach_defaults (GTK_TABLE (table), *ParamEntry, NumObj, NumObj+1, NumLine+1, NumLine+2);
						gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( *ParamEntry ), (pCfgEvtLog->RemoteAlarmsForwardSlot&(1<<(NumObj-5)))?TRUE:FALSE );
					}
					else
					{
//GTK3						int PixelsLength = 100;
						int MaxChars = 0;
						switch( NumObj )
						{
							case 0:
								if ( pCfgEvtLog->FirstVarNum!=-1 )
									sprintf( BuffValue, "%d", pCfgEvtLog->FirstVarNum );
								else
									BuffValue[ 0 ] = '\0';
								break;
							case 1:
								sprintf( BuffValue, "%d", pCfgEvtLog->NbrVars );
								break;
							case 2:
								strcpy( BuffValue, pCfgEvtLog->Symbol );
								MaxChars = EVENT_SYMBOL_LGT-1;
								break;
							case 3:
								strcpy( BuffValue, pCfgEvtLog->Text );
//GTK3								PixelsLength = 220;
								MaxChars = EVENT_TEXT_LGT-1;
								break;
							case 4:
								sprintf( BuffValue, "%d", pCfgEvtLog->EventLevel );
								break;
						}
						{
							*ParamEntry = gtk_entry_new( );
//GTK3							gtk_widget_set_usize( *ParamEntry,PixelsLength,0 );
							if ( MaxChars>0 )
								gtk_entry_set_max_length( GTK_ENTRY(*ParamEntry), MaxChars );
//							gtk_box_pack_start( GTK_BOX (hbox[NumLine+1]), *ParamEntry, FALSE, FALSE, 0 );
							gtk_table_attach_defaults (GTK_TABLE (table), *ParamEntry, NumObj, NumObj+1, NumLine+1, NumLine+2);
							gtk_entry_set_text( GTK_ENTRY(*ParamEntry), BuffValue );
						}
					}
				}//default:
			}
		}
	}
//	gtk_widget_show_all(vbox);
//	return vbox;
	gtk_widget_show_all(scrolled_win);
	return scrolled_win;
}
void GetConfigEventsSettings( void )
{
	int NumObj;
	int NumLine;
	StrConfigEventLog * pCfgEvtLog;
	GtkWidget *ParamEntry;
	char * text;
	for (NumLine=0; NumLine<NBR_CONFIG_EVENTS_LOG; NumLine++ )
	{
		int FirstVarEntered = -1;
		int NbrVarsEntered = 0;
		int ValueRemoteAlarmsForwardSlot = 0;
		pCfgEvtLog = &ConfigEventLog[ NumLine ];

		for (NumObj=0; NumObj<NBR_CONFIG_EVENTS_PARAMS; NumObj++)
		{
			ParamEntry = EventConfigParamEntry[ NumLine ][ NumObj ];
			switch( NumObj )
			{
				case 0:
					text = (char *)gtk_entry_get_text(GTK_ENTRY(ParamEntry));
					if ( text[0]!='\0' )
						FirstVarEntered = atoi( text );
					break;
				case 1:
					text = (char *)gtk_entry_get_text(GTK_ENTRY(ParamEntry));
					NbrVarsEntered = atoi( text );
					break;
				case 2:
					text = (char *)gtk_entry_get_text(GTK_ENTRY(ParamEntry));
					strcpy( pCfgEvtLog->Symbol, text );
					break;
				case 3:
					text = (char *)gtk_entry_get_text(GTK_ENTRY(ParamEntry));
					strcpy( pCfgEvtLog->Text, text );
					break;
				case 4:
					text = (char *)gtk_entry_get_text(GTK_ENTRY(ParamEntry));
					pCfgEvtLog->EventLevel = atoi( text );
					break;
				case 5:
				case 6:
				case 7:
				case 8:
				case 9:
				case 10:
				case 11:
				case 12:
					if ( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( ParamEntry ) ) )
						ValueRemoteAlarmsForwardSlot = ValueRemoteAlarmsForwardSlot|(1<<(NumObj-5));
					break;
			}
		}//for (NumObj=0;
		pCfgEvtLog->FirstVarNum = -1;
		if ( FirstVarEntered+NbrVarsEntered>GetSizeVarsForTypeVar( VAR_MEM_BIT ) )
		{
			ShowMessageBoxError( _("Overflow error for first/nbrs detected...") );
		}
		else
		{
			pCfgEvtLog->NbrVars = NbrVarsEntered;
			pCfgEvtLog->FirstVarNum = FirstVarEntered;
		}
		pCfgEvtLog->RemoteAlarmsForwardSlot = ValueRemoteAlarmsForwardSlot;
	}//for (NumLine=0; 
	// update the tags list of the variables that the user want to log !
	InitVarsArrayLogTags( );
}
#endif

GtkWidget * CreateModemConfigPage( void )
{
	char * NameParams[] = { N_("Modem on slave monitor"), N_("AT init sequence"), N_("AT config sequence"), N_("AT call sequence"), N_("Optional PIN Code") };
	GtkWidget *vbox;
	GtkWidget * hbox[ NBR_MODEM_CONF+1 ];
	GtkWidget * pLabel;
	int ScanParam;
	int IndexNameParam = 0;
	int ScanLine = 0;
	vbox = gtk_vbox_new (FALSE, 0);

	AutoAdjustTime = gtk_check_button_new_with_label( _("Automatically adjust summer/winter time.") );
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( AutoAdjustTime ), GeneralParamsMirror.AutomaticallyAdjustSummerWinterTime );
	gtk_box_pack_start(GTK_BOX(vbox), AutoAdjustTime, FALSE, FALSE, 0);

	for( ScanParam=0; ScanParam<NBR_MODEM_CONF; ScanParam++ )
	{
		hbox[ScanLine] = gtk_hbox_new (FALSE, 0);
		gtk_container_add (GTK_CONTAINER (vbox), hbox[ScanLine]);
		pLabel = gtk_label_new( gettext(NameParams[IndexNameParam]) );
		gtk_box_pack_start(GTK_BOX(hbox[ScanLine]), pLabel, FALSE, FALSE, 0);
		if ( ScanParam==0 )
		{
			ModemConfEntry[ ScanParam ] = gtk_check_button_new_with_label( _("Use") );
			gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( ModemConfEntry[ ScanParam ] ), Modem.ModemUsed );
		}
		else
		{
			char * CurrentParamValue = NULL;
			ModemConfEntry[ ScanParam ] = gtk_entry_new();
			switch( ScanParam )
			{
				case 1: CurrentParamValue = Modem.StrInitSequence; break;
				case 2: CurrentParamValue = Modem.StrConfigSequence; break;
				case 3: CurrentParamValue = Modem.StrCallSequence; break;
				case 4: CurrentParamValue = Modem.StrCodePIN; break;
				case 5: CurrentParamValue = Preferences.ModemForMasterMonitor.StrInitSequence; break;
				case 6: CurrentParamValue = Preferences.ModemForMasterMonitor.StrConfigSequence; break;
				case 7: CurrentParamValue = Preferences.ModemForMasterMonitor.StrCallSequence; break;
				case 8: CurrentParamValue = Preferences.ModemForMasterMonitor.StrCodePIN; break;
			}
			gtk_entry_set_text( GTK_ENTRY(ModemConfEntry[ ScanParam ]), CurrentParamValue );
		}
		gtk_box_pack_start(GTK_BOX(hbox[ScanLine]), ModemConfEntry[ ScanParam ], FALSE, FALSE, 0);
		if ( ScanParam==3 )
			gtk_widget_set_sensitive( ModemConfEntry[ ScanParam ], FALSE );
		ScanLine++;
		IndexNameParam++;
		if ( ScanParam==4 )
		{
			hbox[ScanLine] = gtk_hbox_new (FALSE, 0);
			gtk_container_add (GTK_CONTAINER (vbox), hbox[ScanLine]);
			pLabel = gtk_label_new( _("--- Monitor Master modem AT sequences ---") );
			gtk_box_pack_start(GTK_BOX(hbox[ScanLine]), pLabel, FALSE, FALSE, 0);
			ScanLine++;
			IndexNameParam = 1;
		}
	}
	gtk_widget_show_all(vbox);
	return vbox;
}
void GetModemConfigSettings( void )
{
	GeneralParamsMirror.AutomaticallyAdjustSummerWinterTime = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(AutoAdjustTime) );
	int ScanLine = 0;
	Modem.ModemUsed = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( ModemConfEntry[ ScanLine++ ] ) );
	strncpy( Modem.StrInitSequence, gtk_entry_get_text(GTK_ENTRY( ModemConfEntry[ ScanLine++ ] )), MODEM_STRING_LGT );
	Modem.StrInitSequence[ MODEM_STRING_LGT-1 ] = '\0';
	strncpy( Modem.StrConfigSequence, gtk_entry_get_text(GTK_ENTRY( ModemConfEntry[ ScanLine++ ] )), MODEM_STRING_LGT );
	Modem.StrConfigSequence[ MODEM_STRING_LGT-1 ] = '\0';
	strncpy( Modem.StrCallSequence, gtk_entry_get_text(GTK_ENTRY( ModemConfEntry[ ScanLine++ ] )), MODEM_STRING_LGT );
	Modem.StrCallSequence[ MODEM_STRING_LGT-1 ] = '\0';
	strncpy( Modem.StrCodePIN, gtk_entry_get_text(GTK_ENTRY( ModemConfEntry[ ScanLine++ ] )), MODEM_STRING_LGT );
	Modem.StrCodePIN[ TELEPHONE_NUMBER_LGT-1 ] = '\0';

	strncpy( Preferences.ModemForMasterMonitor.StrInitSequence, gtk_entry_get_text(GTK_ENTRY( ModemConfEntry[ ScanLine++ ] )), MODEM_STRING_LGT );
	Preferences.ModemForMasterMonitor.StrInitSequence[ MODEM_STRING_LGT-1 ] = '\0';
	strncpy( Preferences.ModemForMasterMonitor.StrConfigSequence, gtk_entry_get_text(GTK_ENTRY( ModemConfEntry[ ScanLine++ ] )), MODEM_STRING_LGT );
	Preferences.ModemForMasterMonitor.StrConfigSequence[ MODEM_STRING_LGT-1 ] = '\0';
	strncpy( Preferences.ModemForMasterMonitor.StrCallSequence, gtk_entry_get_text(GTK_ENTRY( ModemConfEntry[ ScanLine++ ] )), MODEM_STRING_LGT );
	Preferences.ModemForMasterMonitor.StrCallSequence[ MODEM_STRING_LGT-1 ] = '\0';
	strncpy( Preferences.ModemForMasterMonitor.StrCodePIN, gtk_entry_get_text(GTK_ENTRY( ModemConfEntry[ ScanLine++ ] )), MODEM_STRING_LGT );
	Preferences.ModemForMasterMonitor.StrCodePIN[ TELEPHONE_NUMBER_LGT-1 ] = '\0';
}

static gint RemoteAlarmType_changed_event( GtkWidget *widget, void * NumSlot )
{
	int Type = gtk_combo_box_get_active( GTK_COMBO_BOX(widget) );
//	printf("combo change type=%d (slot=%d)\n",Type,(int)NumSlot);
	gtk_widget_set_sensitive( RemoteAlarmConfEntry[ (int)NumSlot+2 ][ 2 ], (Type-1)==ALARMS_TYPE_SMS );
	gtk_widget_set_sensitive( RemoteAlarmConfEntry[ (int)NumSlot+2 ][ 3 ], (Type-1)==ALARMS_TYPE_EMAIL );
	return TRUE;
}
GtkWidget * CreateRemoteAlarmsConfigPage( void )
{
	static char * NameParams[] = { N_("Global remote alarms enable"), N_("Slot Alarms"), N_("Remote Slot '0': "), N_("Remote Slot '1': "), N_("Remote Slot '2': "), N_("Remote Slot '3': "), N_("Remote Slot '4': "), N_("Remote Slot '5': "), N_("Remote Slot '6': "), N_("Remote Slot '7': "), N_("Center SMS Server"), N_("Smtp Server For Emails"), N_("Smtp Server User Name"), N_("Smtp Server Password"), N_("Email Sender Address") };
	static char * NameCols[ ] = { N_("Name"), N_("Type"), N_("Telephone (SMS)"), N_("Email") };
	static char * ListComboTypeAlarm[ ] = { N_("None"), N_("SMS"), N_("Email") , NULL/*End*/ };
	static int ColWidgetSize[ ] = { 160, 120, 150, 240 };
	GtkWidget *vbox;
	GtkWidget * hbox[ NBR_LINES_REMOTE_ALARMS ];
	GtkWidget * pLabel;
	int ScanLine;
	int ScanCol;
	StrRemoteAlarms * pAlarms = &RemoteAlarmsConfig;
	vbox = gtk_vbox_new (FALSE, 0);

	for( ScanLine=0; ScanLine<NBR_LINES_REMOTE_ALARMS; ScanLine++ )
	{
		hbox[ScanLine] = gtk_hbox_new (FALSE, 0);
		gtk_container_add (GTK_CONTAINER (vbox), hbox[ScanLine]);
		pLabel = gtk_label_new( gettext(NameParams[ScanLine]) );
		gtk_box_pack_start(GTK_BOX(hbox[ScanLine]), pLabel, FALSE, FALSE, 0);
		if ( ScanLine==0 )
		{
			RemoteAlarmConfEntry[ ScanLine ][ 0 ] = gtk_check_button_new( );
			gtk_box_pack_start(GTK_BOX(hbox[ScanLine]), RemoteAlarmConfEntry[ ScanLine ][ 0 ], FALSE, FALSE, 0);
			gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( RemoteAlarmConfEntry[ ScanLine ][ 0 ] ), pAlarms->GlobalEnabled );
		}
		else if ( ScanLine==1 )
		{
			for( ScanCol=0; ScanCol<NBR_PARAMS_SLOTS_ALARMS; ScanCol++ )
			{
				pLabel = gtk_label_new( gettext(NameCols[ScanCol]) );
				gtk_widget_set_size_request( pLabel, ColWidgetSize[ ScanCol ] ,-1 );
				gtk_box_pack_start(GTK_BOX(hbox[ScanLine]), pLabel, FALSE, FALSE, 0);
			}
		}
		else if ( ScanLine>=2 && ScanLine<=9 )
		{
			for( ScanCol=0; ScanCol<NBR_PARAMS_SLOTS_ALARMS; ScanCol++ )
			{
				char * CurrentParamValue = NULL;
				if ( ScanCol==1 )
				{
					RemoteAlarmConfEntry[ ScanLine ][ ScanCol ] = gtk_combo_box_new_text( );
					FillComboBoxReqType( MY_GTK_COMBO_BOX( RemoteAlarmConfEntry[ ScanLine ][ ScanCol ] ), ListComboTypeAlarm );
//					gtk_combo_box_set_active( GTK_COMBO_BOX( RemoteAlarmConfEntry[ ScanLine ][ ScanCol ] ), pAlarms->AlarmType[ ScanLine-2 ]+1 );
					// can't be called (with a previous set_active() function here) if not already show...?
					gtk_signal_connect( GTK_OBJECT(RemoteAlarmConfEntry[ ScanLine ][ ScanCol ]), "changed", GTK_SIGNAL_FUNC(RemoteAlarmType_changed_event), (void *)(ScanLine-2) );
				}
				else
				{
					RemoteAlarmConfEntry[ ScanLine ][ ScanCol ] = gtk_entry_new();
				}
				gtk_widget_set_size_request( RemoteAlarmConfEntry[ ScanLine ][ ScanCol ], ColWidgetSize[ ScanCol ], -1 );
				gtk_box_pack_start(GTK_BOX(hbox[ScanLine]), RemoteAlarmConfEntry[ ScanLine ][ ScanCol ], FALSE, FALSE, 0);
				if ( ScanCol!=1 )
				{
					switch( ScanCol )
					{
						case 0: CurrentParamValue = pAlarms->SlotName[ ScanLine-2 ]; break;
						case 2: CurrentParamValue = pAlarms->TelephoneNumber[ ScanLine-2 ]; break;
						case 3: CurrentParamValue = pAlarms->EmailAddress[ ScanLine-2 ]; break;
					}
					gtk_entry_set_text( GTK_ENTRY(RemoteAlarmConfEntry[ ScanLine ][ ScanCol ]), CurrentParamValue );
				}
			}
		}
		else
		{
			char * CurrentParamValue = NULL;
			RemoteAlarmConfEntry[ ScanLine ][ 0 ] = gtk_entry_new();
			gtk_box_pack_start(GTK_BOX(hbox[ScanLine]), RemoteAlarmConfEntry[ ScanLine ][ 0 ], FALSE, FALSE, 0);
			switch( ScanLine )
			{
				case 10: CurrentParamValue = pAlarms->CenterServerSMS; break;
				case 11: CurrentParamValue = pAlarms->SmtpServerForEmails; break;
				case 12: CurrentParamValue = pAlarms->SmtpServerUserName; break;
				case 13: CurrentParamValue = pAlarms->SmtpServerPassword; break;
				case 14: CurrentParamValue = pAlarms->EmailSenderAddress; break;
			}
			gtk_entry_set_text( GTK_ENTRY(RemoteAlarmConfEntry[ ScanLine ][ 0 ]), CurrentParamValue );
		}
	}
	gtk_widget_show_all(vbox);
	for( ScanLine=2; ScanLine<2+NBR_ALARMS_SLOTS; ScanLine++ )
		gtk_combo_box_set_active( GTK_COMBO_BOX( RemoteAlarmConfEntry[ ScanLine ][ 1 ] ), pAlarms->AlarmType[ ScanLine-2 ]+1 );
	return vbox;
}
void GetRemoteAlarmsConfigSettings( void )
{
	int ScanLine = 0;
	int ScanCol;
	StrRemoteAlarms * pAlarms = &RemoteAlarmsConfig;
	for( ScanLine=0; ScanLine<NBR_LINES_REMOTE_ALARMS; ScanLine++ )
	{
		if ( ScanLine==0 )
		{
			pAlarms->GlobalEnabled = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( RemoteAlarmConfEntry[ ScanLine ][ 0 ] ) );
		}
		else if ( ScanLine>=2 && ScanLine<=9 )
		{
			for( ScanCol=0; ScanCol<NBR_PARAMS_SLOTS_ALARMS; ScanCol++ )
			{
				switch( ScanCol )
				{
					case 0:
						strncpy( pAlarms->SlotName[ ScanLine-2 ], gtk_entry_get_text(GTK_ENTRY( RemoteAlarmConfEntry[ ScanLine ][ ScanCol ] )), LGT_STR_INFO );
						pAlarms->SlotName[ ScanLine-2 ][ LGT_STR_INFO-1 ] = '\0';
						break;
					case 1:
						pAlarms->AlarmType[ ScanLine-2 ] = gtk_combo_box_get_active( GTK_COMBO_BOX(RemoteAlarmConfEntry[ ScanLine ][ ScanCol ]) )-1;
						break;
					case 2:
						strncpy( pAlarms->TelephoneNumber[ ScanLine-2 ], gtk_entry_get_text(GTK_ENTRY( RemoteAlarmConfEntry[ ScanLine ][ ScanCol ] )), TELEPHONE_NUMBER_LGT );
						pAlarms->TelephoneNumber[ ScanLine-2 ][ TELEPHONE_NUMBER_LGT-1 ] = '\0';
						break;
					case 3:
						strncpy( pAlarms->EmailAddress[ ScanLine-2 ], gtk_entry_get_text(GTK_ENTRY( RemoteAlarmConfEntry[ ScanLine ][ ScanCol ] )), EMAIL_LGT );
						pAlarms->EmailAddress[ ScanLine-2 ][ EMAIL_LGT-1 ] = '\0';
						break;
				}
			}
		}
		else if ( ScanLine==10 )
		{
			strncpy( pAlarms->CenterServerSMS, gtk_entry_get_text(GTK_ENTRY( RemoteAlarmConfEntry[ ScanLine ][ 0 ] )), TELEPHONE_NUMBER_LGT );
			pAlarms->CenterServerSMS[ TELEPHONE_NUMBER_LGT-1 ] = '\0';
			ScanLine++;
			strncpy( pAlarms->SmtpServerForEmails, gtk_entry_get_text(GTK_ENTRY( RemoteAlarmConfEntry[ ScanLine ][ 0 ] )), EMAIL_LGT );
			pAlarms->SmtpServerForEmails[ EMAIL_LGT-1 ] = '\0';
			ScanLine++;
			strncpy( pAlarms->SmtpServerUserName, gtk_entry_get_text(GTK_ENTRY( RemoteAlarmConfEntry[ ScanLine ][ 0 ] )), LGT_STR_INFO );
			pAlarms->SmtpServerUserName[ LGT_STR_INFO-1 ] = '\0';
			ScanLine++;
			strncpy( pAlarms->SmtpServerPassword, gtk_entry_get_text(GTK_ENTRY( RemoteAlarmConfEntry[ ScanLine ][ 0 ] )), LGT_STR_INFO );
			pAlarms->SmtpServerPassword[ LGT_STR_INFO-1 ] = '\0';
			ScanLine++;
			strncpy( pAlarms->EmailSenderAddress, gtk_entry_get_text(GTK_ENTRY( RemoteAlarmConfEntry[ ScanLine ][ 0 ] )), EMAIL_LGT );
			pAlarms->EmailSenderAddress[ EMAIL_LGT-1 ] = '\0';
			ScanLine++;
		}
	}	
}


void GetSettings( void )
{
	int Scan;
	GetGeneralParameters( );
	GetIOSettings( 1/*ForInputs*/ );
	GetIOSettings( 0/*ForInputs*/ );
#ifdef MODBUS_IO_MASTER
	// disable before all requests on slave (multi-task...)
	for (Scan=0; Scan<NBR_MODBUS_MASTER_REQ; Scan++ )
		ModbusMasterReq[ Scan ].SlaveListNum = -1;
	GetModbusModulesIOSettings( );
	GetModbusSlavesSettings( );
	GetModbusComParameters( );
#endif
#ifdef COMPLETE_PLC
	GetConfigEventsSettings( );
#endif
	GetRemoteAlarmsConfigSettings( );
	GetModemConfigSettings( );

/*#ifndef RT_SUPPORT
	InfosGene->AskToConfHardInputs = TRUE;
	InfosGene->AskToConfHardOutputs = TRUE;
#endif*/
	HardwareActionsAfterProjectLoaded( );
//v0.9.9 ConfigSerialModbusMaster( );
	AdjustPeriodicValuesForTasks( );
}

/* To update slave adress list of the widgets (perhaps modified) when we go on the requests page */
//ForGTK3 void SignalPageSelected( GtkNotebook * notebook, GtkNotebookPage * page, guint page_num, gpointer user_data )
void SignalPageSelected( GtkNotebook * notebook, GtkWidget * page, guint page_num, gpointer user_data )
{
	if ( page_num==6 )
		UpdateSlaveListsOnRequestPage( );
}

void OpenConfigWindowGtk()
{
	if ( InfosGUI->TargetMonitor.RemoteConnected )
	{
		ShowMessageBoxError( _("Not possible when connected to a remote target...") );
	}
	else
	{
		GtkWidget *nbook;
		GtkWidget *nbookio;

		ConfigWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
		gtk_window_set_title( GTK_WINDOW(ConfigWindow), "Config" );
		gtk_window_set_modal( GTK_WINDOW(ConfigWindow), TRUE );

		nbook = gtk_notebook_new( );
		gtk_notebook_append_page( GTK_NOTEBOOK(nbook), CreateGeneralParametersPage( ),
					 gtk_label_new ( _("Period/Sizes/Info")) );
#ifdef COMPLETE_PLC
		gtk_notebook_append_page( GTK_NOTEBOOK(nbook), CreateConfigEventsPage( ),
					 gtk_label_new ( _("Events Config")) );
#endif
		nbookio = gtk_notebook_new( );
		gtk_notebook_append_page( GTK_NOTEBOOK(nbookio), CreateIOConfPage( 1/*ForInputs*/ ),
					 gtk_label_new ( _("Physical Inputs %I")) );
		gtk_notebook_append_page( GTK_NOTEBOOK(nbookio), CreateIOConfPage( 0/*ForInputs*/ ),
					 gtk_label_new ( _("Physical Outputs %Q")) );
		gtk_notebook_append_page( GTK_NOTEBOOK(nbook), nbookio, gtk_label_new( _("Physical Inputs/Outputs" )) );
#ifdef MODBUS_IO_MASTER
		gtk_notebook_append_page( GTK_NOTEBOOK(nbook), CreateModbusComParametersPage( ),
					 gtk_label_new ( _("Modbus communication")) );
		gtk_notebook_append_page( GTK_NOTEBOOK(nbook), CreateModbusSlavesPage( ),
					 gtk_label_new ( _("Modbus slaves")) );
		gtk_notebook_append_page( GTK_NOTEBOOK(nbook), CreateModbusModulesIO( ),
					 gtk_label_new ( _("Modbus I/O")) );
#endif
		gtk_notebook_append_page( GTK_NOTEBOOK(nbook), CreateRemoteAlarmsConfigPage( ),
					 gtk_label_new ( _("Remote Alarms")) );
		gtk_notebook_append_page( GTK_NOTEBOOK(nbook), CreateModemConfigPage( ),
					 gtk_label_new ( _("Misc/Modem")) );

		gtk_container_add( GTK_CONTAINER (ConfigWindow), nbook );
		gtk_signal_connect( GTK_OBJECT(nbook), "switch-page",
											GTK_SIGNAL_FUNC(SignalPageSelected), (void *)NULL );

		gtk_window_set_position( GTK_WINDOW(ConfigWindow), GTK_WIN_POS_CENTER );
//		gtk_window_set_policy( GTK_WINDOW(ConfigWindow), FALSE, FALSE, TRUE );
		gtk_signal_connect ( GTK_OBJECT(ConfigWindow), "destroy",
							GTK_SIGNAL_FUNC(GetSettings), NULL );

		gtk_widget_show_all( ConfigWindow );
	}
}


