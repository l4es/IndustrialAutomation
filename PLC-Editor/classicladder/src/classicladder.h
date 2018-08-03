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

/* if GTK not included before */
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define CL_PRODUCT_NAME "ClassicLadder"
#define CL_RELEASE_VER_STRING "0.9.112"
#define CL_RELEASE_DATE_STRING "6 April 2018"
#define CL_RELEASE_COPYRIGHT_YEARS "2001-2018"


//to debug "monitor": usefull if PLC running... and connecting to ifself (same executable) only for tests purposes at the start, and after also!
//////#define MONITOR_TEST_ONLY_NO_RESPONSES_USE

// also available under Gtk, to allow to see "%QLEDx" variables !
#define NBR_USERS_LEDS 1

// include target files for hardware particularities of each embedded platform...
#ifdef RASPBERRY_GPIO_ACCESS
#include "target_embedded_raspberrypi.h"
#elif ATMEL_SAM_GPIO_ACCESS
#include "target_embedded_plc_arietta.h"
#elif EMBEDDED_PLC_486
#include "target_embedded_plc_486.h"
#else
// embedded filesystem used to store project, files datas (events, ...), and to get statistics on it
#define EMBEDDED_DISK_DEVICE "/"
#endif


// where to save the project file transfered with the monitor...
#define FILEPROJECT_TRANSFERED_TO_SAVE "/usr/local/classicladder/classicladder_project.clprjz"

//#define CL_ICON_FILE "/usr/share/pixmaps/gnome-gmush.png"

// defaults numbers values
#define NBR_RUNGS_DEF 300
#define NBR_BITS_DEF 500
#define NBR_WORDS_DEF 200
#define NBR_TIMERS_DEF 50
#define NBR_MONOSTABLES_DEF 50
#define NBR_COUNTERS_DEF 50
#define NBR_TIMERS_IEC_DEF 50
#define NBR_REGISTERS_DEF 10
#define REGISTER_LIST_SIZE_DEF 500
#define NBR_PHYS_INPUTS_DEF 50
#define NBR_PHYS_OUTPUTS_DEF 50
#define NBR_ARITHM_EXPR_DEF 200
#define NBR_SECTIONS_DEF 10
#define NBR_SYMBOLS_DEF 300
#define NBR_PHYS_WORDS_INPUTS_DEF 25
#define NBR_PHYS_WORDS_OUTPUTS_DEF 25

//much more for RaspberryPI as generally one line = one GPIO (not consecutive)
//#define NBR_INPUTS_CONF 5
//#define NBR_OUTPUTS_CONF 5
//#define NBR_INPUTS_CONF 15
//#define NBR_OUTPUTS_CONF 15
#define NBR_INPUTS_CONF 32
#define NBR_OUTPUTS_CONF 32

#define NBR_VARS_SYSTEM 50
#define NBR_VARS_WORDS_SYSTEM 20

typedef struct plc_sizeinfo_s {
	int	nbr_rungs;
	int	nbr_bits;
	int	nbr_words;
#ifdef OLD_TIMERS_MONOS_SUPPORT
	int	nbr_timers;
	int	nbr_monostables;
#endif
	int	nbr_counters;
	int	nbr_timers_iec;
	int nbr_registers;
	int register_list_size;
	int	nbr_phys_inputs;
	int	nbr_phys_outputs;
	int	nbr_arithm_expr;
	int	nbr_sections;
	int nbr_symbols;
	int	nbr_phys_words_inputs;
	int	nbr_phys_words_outputs;
}plc_sizeinfo_s;

// defines shortcuts for sizes
#define NBR_RUNGS 	 InfosGene->GeneralParams.SizesInfos.nbr_rungs
#define NBR_BITS 	 InfosGene->GeneralParams.SizesInfos.nbr_bits
#define NBR_WORDS	 InfosGene->GeneralParams.SizesInfos.nbr_words
#define NBR_TIMERS 	 InfosGene->GeneralParams.SizesInfos.nbr_timers
#define NBR_MONOSTABLES  InfosGene->GeneralParams.SizesInfos.nbr_monostables
#define NBR_COUNTERS	InfosGene->GeneralParams.SizesInfos.nbr_counters
#define NBR_TIMERS_IEC 	 InfosGene->GeneralParams.SizesInfos.nbr_timers_iec
#define NBR_REGISTERS	InfosGene->GeneralParams.SizesInfos.nbr_registers
#define REGISTER_LIST_SIZE	InfosGene->GeneralParams.SizesInfos.register_list_size
#define NBR_PHYS_INPUTS  InfosGene->GeneralParams.SizesInfos.nbr_phys_inputs
#define NBR_PHYS_OUTPUTS InfosGene->GeneralParams.SizesInfos.nbr_phys_outputs
#define NBR_ARITHM_EXPR  InfosGene->GeneralParams.SizesInfos.nbr_arithm_expr
#define NBR_SECTIONS 	 InfosGene->GeneralParams.SizesInfos.nbr_sections
#define NBR_SYMBOLS		InfosGene->GeneralParams.SizesInfos.nbr_symbols
#define NBR_PHYS_WORDS_INPUTS  InfosGene->GeneralParams.SizesInfos.nbr_phys_words_inputs
#define NBR_PHYS_WORDS_OUTPUTS InfosGene->GeneralParams.SizesInfos.nbr_phys_words_outputs


#define ARITHM_EXPR_SIZE 50

#ifdef MAT_CONNECTION
#define TYPE_FOR_BOOL_VAR plc_pt_t
#else
#define TYPE_FOR_BOOL_VAR char
#endif

// default period rung/sequential refresh
#define PERIODIC_REFRESH_MS_DEF 50
// default period to scan physical inputs
#define PERIODIC_REFRESH_MS_INPUTS_DEF 10

#define TIME_BASE_MINS 60000
#define TIME_BASE_SECS 1000
#define TIME_BASE_100MS 100

// IEC Timers available modes
#define TIMER_IEC_MODE_ON 0
#define TIMER_IEC_MODE_OFF 1
#define TIMER_IEC_MODE_PULSE 2

/* numbers of blocks in a rung */
//v0.9 #define RUNG_WIDTH 10
//v0.9 #define RUNG_HEIGHT 6
#define RUNG_WIDTH 12
#define RUNG_HEIGHT 8

/* size in pixels of rungs blocks (default) */
//#define BLOCK_WIDTH_DEF 32
#define BLOCK_WIDTH_DEF 48
#define BLOCK_HEIGHT_DEF 32

/* offsets in pixels */
//#define OFFSET_X 4
//#define OFFSET_Y 6
/* size of lines activated (comment to not use) */
#define THICK_LINE_ELE_ACTIVATED 3

/* convenient calcs used many time... */
#define TOTAL_PX_RUNG_HEIGHT ( InfosGene->HeaderLabelCommentHeight + InfosGene->BlockHeight*RUNG_HEIGHT )
#define TOTAL_PX_RUNG_WIDTH ( InfosGene->BlockWidth*RUNG_WIDTH )

/* elements lists for the rungs */
#define ELE_FREE 0
#define ELE_INPUT 1
#define ELE_INPUT_NOT 2
#define ELE_RISING_INPUT 3
#define ELE_FALLING_INPUT 4
#define ELE_CONNECTION 9
#define ELE_TIMER 10
#define ELE_MONOSTABLE 11
#define ELE_COUNTER 12
#define ELE_TIMER_IEC 13
#define ELE_REGISTER 14
#define ELE_COMPAR 20
#define ELE_OUTPUT 50
#define ELE_OUTPUT_NOT 51
#define ELE_OUTPUT_SET 52
#define ELE_OUTPUT_RESET 53
#define ELE_OUTPUT_JUMP 54
#define ELE_OUTPUT_CALL 55
#define ELE_OUTPUT_OPERATE 60
/* for complex elements using many blocks : only one block
   is "alive", others are marked as UNUSABLE */
#define ELE_UNUSABLE 99


/* used only for edit */
#define EDIT_CNX_WITH_TOP 100
#define EDIT_POINTER 101
#define EDIT_LONG_CONNECTION 102
#define EDIT_ERASER 103
#define EDIT_SELECTION 104
#define EDIT_COPY 105
#define EDIT_MOVE 106
#define EDIT_INVERT 107 // Heli patch ! */

/* used for search function (Heli) */
#define NBR_SEARCH_TYPES 7
#define SEARCH_ALL 200
#define SEARCH_CONTACTS 201
#define SEARCH_COILS 202
#define SEARCH_TRANSITIONS 203
#define SEARCH_BLOCKS 204
#define SEARCH_COMPARE 205
#define SEARCH_OPERATE 206


// for link with another project where all variables IDs are only an offset in
// a giant array (absolutely no type part existance)...
#define IDVAR_IS_TYPE_AND_OFFSET


/* Type of vars */
/* booleans */
#define VAR_MEM_BIT 00
#define VAR_TIMER_DONE 10
#define VAR_TIMER_RUNNING 11
#define VAR_TIMER_IEC_DONE 15
#define VAR_MONOSTABLE_RUNNING 20
#define VAR_COUNTER_DONE 25
#define VAR_COUNTER_EMPTY 26
#define VAR_COUNTER_FULL 27
#define VAR_STEP_ACTIVITY 30
#define VAR_PHYS_INPUT 50
#define VAR_PHYS_OUTPUT 60
#define VAR_USER_LED 65
#define VAR_SYSTEM 70
#define VAR_REGISTER_EMPTY 80
#define VAR_REGISTER_FULL 81
#define VAR_ARE_WORD 199    /* after it, all vars are no more booleans */
/* integers */
#define VAR_MEM_WORD 200
#define VAR_STEP_TIME 220
#define VAR_TIMER_PRESET 230
#define VAR_TIMER_VALUE 231
#define VAR_MONOSTABLE_PRESET 240
#define VAR_MONOSTABLE_VALUE 241
#define VAR_COUNTER_PRESET 250
#define VAR_COUNTER_VALUE 251
#define VAR_TIMER_IEC_PRESET 260
#define VAR_TIMER_IEC_VALUE 261
#define VAR_PHYS_WORD_INPUT 270
#define VAR_PHYS_WORD_OUTPUT 280
#define VAR_WORD_SYSTEM 290
#define VAR_REGISTER_IN_VALUE 300
#define VAR_REGISTER_OUT_VALUE 301
#define VAR_REGISTER_NBR_VALUES 302

#define TEST_VAR_IS_A_BOOL( type, offset ) (type<VAR_ARE_WORD)
#define VAR_DEFAULT_TYPE VAR_MEM_BIT
#define BASE_MINS 0
#define BASE_SECS 1
#define BASE_100MS 2

//default variables depending of the element placed
#define DEFAULT_VAR_FOR_CONTACT VAR_PHYS_INPUT
#define DEFAULT_VAR_FOR_COIL VAR_PHYS_OUTPUT

#define TIME_UPDATE_GTK_DISPLAY_MS 100

// attribute separator for variable names.
//#define VAR_ATTRIBUTE_SEP ','
#define VAR_ATTRIBUTE_SEP '.'

typedef struct StrElement
{
	short int Type;
	char ConnectedWithTop;
	int VarType;
	int VarNum;     /* or NumRung (for jump), NumTimer, NumMonostable,... */
	int IndexedVarType;
	int IndexedVarNum;
	
	char DynamicInput;
	char DynamicState;
	char DynamicVarBak; /* used for rising/falling edges */
	char DynamicOutput;
	char DynamicVarSetted;
}StrElement;

#define LGT_LABEL 10
#define LGT_COMMENT_OLD_LITTLE 30
#define LGT_COMMENT 100
typedef struct StrRung
{
	int Used;
	int PrevRung;
	int NextRung;
	int NbrLinesUsed; // TODO: also use this for drawing in the future !!! to see more rungs simultaneously vertically...
	char Label[LGT_LABEL];
	char Comment[LGT_COMMENT];
	StrElement Element[RUNG_WIDTH][RUNG_HEIGHT];
}StrRung;

#ifdef OLD_TIMERS_MONOS_SUPPORT
typedef struct StrTimer
{
	int Preset;
	int Value;
	int Base;
	char DisplayFormat[10];
	char InputEnable;
	char InputControl;
	char OutputDone;
	char OutputRunning;
}StrTimer;
typedef struct StrMonostable
{
	int Preset;
	int Value;
	int Base;
	char DisplayFormat[10];
	char Input;
	char InputBak;
	char OutputRunning;
}StrMonostable;
#endif

typedef struct StrCounter
{
	int Preset;
	int Value;
	int ValueBak;
	char InputReset;
	char InputPreset;
	char InputCountUp;
	char InputCountUpBak;
	char InputCountDown;
	char InputCountDownBak;
	char OutputDone;
	char OutputEmpty;
	char OutputFull;
}StrCounter;

typedef struct StrTimerIEC
{
	int Preset; /* value in number of base units */
	int Value; /* value in number of base units */
	int Base;
	char TimerMode;
	char DisplayFormat[10];
	char Input;
	char InputBak;
	char Output;
	char TimerStarted;
	int ValueToReachOneBaseUnit;
}StrTimerIEC;

#define REGISTER_MODE_UNDEF 0
#define REGISTER_MODE_FIFO 1
#define REGISTER_MODE_LIFO 2
#define NBR_REGISTERS_MODES 3
typedef struct StrRegister
{
	char RegisterMode; /* FIFO/LIFO or not defined... */
	int ValueIn;
	int ValueOut;
	char InputReset;
	char InputIn;
	char InputInBak;
	char InputOut;
	char InputOutBak;
	char OutputEmpty;
	char OutputFull;
	int NbrValuesSaved;
	int FirstValueIndex;
	int LatestValueIndex;
}StrRegister;

typedef struct StrArithmExpr
{
	char Expr[ARITHM_EXPR_SIZE];
}StrArithmExpr;

#define DEVICE_TYPE_NONE -1 //added in 0.9.4 because now we can have DEVICE_TYPE_DIRECT_CONFIG and FirstClassicLadderIO at -1 !!!
#define DEVICE_TYPE_DIRECT_ACCESS 0	/* use inb( ) and outb( ) calls to read/write local inputs/outputs */
#define DEVICE_TYPE_DIRECT_CONFIG 1	/* use outb( ) calls to write config data for local inputs/outputs */
#define DEVICE_TYPE_RASPBERRY_GPIO 2 /* use wiringPi library */
#define DEVICE_TYPE_ATMEL_SAM_GPIO 3 /* use wiringSam library */
#define DEVICE_TYPE_COMEDI 100	/* /dev/comedi0 and following */

typedef struct StrIOConf
{
	int FirstClassicLadderIO;	/* -1 : not used : NO MORE TRUE... */
	int DeviceType;		/* a comedi device or type direct I/O access OR NOTHING */
	int SubDevOrAdr;	/* comedi sub-device, or I/O port address, or ControllerPortA/B/C/D for AtmelSAM */
	int FirstChannel; /* = GPIO directly for RaspberryPI */
	int NbrConsecutivesChannels;
	int FlagInverted;
	int ConfigData; // Data to write for DEVICE_TYPE_DIRECT_CONFIG
}StrIOConf;

typedef struct StrGeneralParams
{
	plc_sizeinfo_s SizesInfos;
	int PeriodMilliSecsTaskLogic;
	int PeriodMilliSecsTaskScanInputs;
	char RealInputsOutputsOnlyOnTarget; //to not scan physical i/o on Gtk simul + do not alloc modbus serial port !
	char AutomaticallyAdjustSummerWinterTime;
}StrGeneralParams;

#define LGT_STR_INFO 30
#define LGT_STR_INFO_COMMENT 300
typedef struct StrProjectProperties
{
	char ProjectName[ LGT_STR_INFO ];
	char ProjectSite[ LGT_STR_INFO ];
	char ParamVersion[ LGT_STR_INFO ];
	char ParamAuthor[ LGT_STR_INFO ];
	char ParamCompany[ LGT_STR_INFO ];
	char ParamCreaDate[ LGT_STR_INFO ];
	char ParamModifDate[ LGT_STR_INFO ];
	char ParamComment[ LGT_STR_INFO_COMMENT ];
}StrProjectProperties;

typedef struct StrInfosGene
{
	StrProjectProperties ProjectProperties;
	
	StrGeneralParams GeneralParams; // variable GeneralParamsMirror contains current project values (not values allocated...)
	StrIOConf InputsConf[ NBR_INPUTS_CONF ];
	StrIOConf OutputsConf[ NBR_OUTPUTS_CONF ];

	int FirstRung;
	int CurrentRung;
	int LastRung;
	int LadderState;
	int UnderCalculationPleaseWait;
	int LadderStoppedToRunBack;
	char CmdRefreshVarsBits;
	
	//TODO: put all this in another structure, or in the new StrInfosGUI?
	int BlockWidth;
	int BlockHeight;
	int PageWidth;
	int PageHeight;
	int TopRungDisplayed;
	int OffsetHiddenTopRungDisplayed;
	int OffsetCurrentRungDisplayed;
	int HScrollValue;
	int VScrollValue;
	int HeaderLabelCommentHeight;
	
	/* how time for the last scan of the rungs in ns if real-time used (if calc on RTLinux side or Xenomai) */
	int DurationOfLastScan;
	int MaxScanDuration;
	/*long*/ unsigned int NbrTicksMissed;
	
	int CurrentSection;

	char AskConfirmationToQuit;
	char AskConfirmationToCancel;
	char HasBeenModifiedForExitCode;
	char ErrorMsgStringToDisplay[ 120 ];
	char ErrorTitleStringToDisplay[ 60 ];
//	char DisplaySymbols;
	char CurrentProjectFileName[ 400 ];
	// splitted, because now two differents threads (inputs read, and outputs write done in logic)
	char AskToConfHardInputs;
	char AskToConfHardOutputs;

	char LogContentModified;
	char DefaultLogListModified;
	char BackgroundSaveLogEventsData;
	
	char BackgroundSaveSetVarsList;
	char DoNotSaveDuringLoadingSetVarsList;
	
	// life led state and users leds states
	char LifeLedState;
	char UserLedVar[ NBR_USERS_LEDS ];
	
	// flags used to point that an alarm will have to be created...
	char TransmitAlarmsGlobalFlagForSms;
	char TransmitAlarmsGlobalFlagForEmails;
}StrInfosGene;

/* Differents states of Ladder */
#define STATE_LOADING 0
#define STATE_STOP 1
#define STATE_RUN 2
#define STATE_RUN_FREEZE 3
#define STATE_RUN_FOR_ONE_CYCLE 4
#define _ConvStateInString( state ) ( (state==STATE_LOADING)?"loading":(state==STATE_STOP)?"stop":(state==STATE_RUN)?"run":(state==STATE_RUN_FREEZE)?"freeze":"run_one" )

typedef struct StrInputFilterAndState
{
	unsigned int FilterMask;
	unsigned int BuffRawInput;
	int InputStateForLogic; // = -1 if "inputs" task never stored any thing for this input (to avoid battle with gtk window simul)
}StrInputFilterAndState;

typedef struct StrTargetMonitor
{
	char RemoteConnected;
	char PreviousRemoteConnected; // used with serial, to open/close the port...
	char RemoteAdrIP[ 50 ];
	char RemoteWithSerialPort[ 30 ]; // connected with a slave on a serial link instead of udp network
	int RemoteWithSerialSpeed; // serial speed to connect
	int TimeOutWaitReply;
	char RemoteWithSerialModem; // connect using a modem on serial...
	char RemoteTelephoneNumber[ 50 ]; // number to call when using modem

	// under file transfer (exclusively) with target
	char RemoteFileTransfer;
	char PreviousRemoteFileTransfer;
	// file tranfer to/from target (-1 if none)
	// possibility to be connected, and transfer a file at the same time...
	int TransferFileNum;
	int TransferFileSubNum;
	char TransferFileSelectedName[ 400 ];
	char TransferFileIsSend;

	// latest target run/stop state
	int LatestTargetState;
	// actions wanted to the target (-1 value after done)
	int AskTargetToSetState;
	int AskTargetToReset;
	int AskTargetToSetClockTime;
	int AskTargetToReboot; // 1=reboot ; 2=halt
	int AskTargetToGetTargetInfos;
	int AskTargetToGetProjectProperties;
	int AskTargetToWriteVarType;
	int AskTargetToWriteVarNum;
	int AskTargetToWriteVarNewValue;
	int AskTargetToSetOrUnsetVarType;
	int AskTargetToSetOrUnsetVarNum;
	int AskTargetToSetOrUnsetVarValue;
	int AskTargetToGetNetworkConfig;
	int AskTargetToWriteNetworkConfig;
	int AskTargetToGetMonitorSerialConfig;
	int AskTargetToWriteMonitorSerialConfig;
	int AskTargetToCleanUp;
		
	char * AskTargetForThisManualFrame; // NULL if nothing requested! to free after use...
	
	char InfoSoftVersion[ 80 ];
	char InfoKernelVersion[ 30 ];
	char InfoLinuxDistribVersion[ 50 ];
	char InfoXenomaiVersion[ 30 ];
	char InfoDiskStats[ 200 ];
	char CmdDisplayTargetInfosVersion;
	char CmdDisplayTargetProjectProperties;
	char CmdDisplayTargetNetworkConfig;

	// for target remote monitor serial config
	char CmdDisplayTargetMonitorSerialConfig;
	char TargetSlaveOnSerialPort[ 30 ];
	int TargetSlaveSerialSpeed;
}StrTargetMonitor;

typedef struct StrIdVar
{
	int VarType;
	int VarNum;
}StrIdVar;

#define NBR_TYPE_BOOLS_SPY 3
#define NBR_BOOLS_VAR_SPY 15
#define NBR_FREE_VAR_SPY 10
#define NBR_FUNCTIONS_BLOCKS_VARS_TO_MONITOR 50
// infos not used with embedded version (without GTK_INTERFACE defined)
typedef struct StrInfosGUI
{
	StrTargetMonitor TargetMonitor;

//TODO: move following in StrInfosGene, so that always present even if project in embedded without GUI and saved back...
	int ValOffsetBoolVar[ NBR_TYPE_BOOLS_SPY ];
	StrIdVar FreeVarSpy[ NBR_FREE_VAR_SPY ];
	int FreeVarSpyDisplayFormat[ NBR_FREE_VAR_SPY ];

	// for target remote monitor of the rungs
	int NbrRungsDisplayedToMonitor;
	int NumRungDisplayedToMonitor[ 20 ];
	// for target remote monitor of functions blocks displayed on rungs
	int NbrFunctionsBlocksVarsToMonitor;
	StrIdVar FunctionsBlocksVarsToMonitor[ NBR_FUNCTIONS_BLOCKS_VARS_TO_MONITOR ];
}
StrInfosGUI;

typedef struct StrElePosiSize
{
	int PosiX;
	int PosiY;
	int SizeX;
	int SizeY;  
}StrElePosiSize;
typedef struct StrMarkColor
{
	float ColorR;
	float ColorG;
	float ColorB;
}StrMarkColor;

typedef struct StrEditRung
{
	StrRung Rung;
	char ModeEdit;
	int NumRung;
	char DoBeforeFinalCopy;
	short int NumElementSelectedInToolBar;
	StrElement * ElementUnderEdit;
/*	int CurrentElementPosiX;
	int CurrentElementPosiY;
	int CurrentElementSizeX;
	int CurrentElementSizeY;*/
	StrElePosiSize CurrentElementPosiSize;
	/* used to see size of element type selected, or selected zone for copy function */
	int GhostZonePosiX;
	int GhostZonePosiY;
	int GhostZoneSizeX;
	int GhostZoneSizeY;
}StrEditRung;

#define NBR_PARAMS_PER_OBJ 5

#define NBR_TIMEBASES 3
typedef struct StrDatasForBases
{
	int Id;
	int ValueInMS;
	char * DisplayFormat;
	char * ParamSelect;
}StrDatasForBase;

#define NBR_TIMERSMODES 3

#define SECTION_IN_LADDER 0
#define SECTION_IN_SEQUENTIAL 1

#define LGT_SECTION_NAME 20
typedef struct StrSection
{
	char Used;
	char Name[ LGT_SECTION_NAME ];
	int Language; /* SECTION_IN_ */
	/* -1 if not a sub-routine, else sub-routine number used for the calls */
	int SubRoutineNumber;
	/* if section is in Ladder */
	int FirstRung;
	int LastRung;
	/* if section is in Sequential */
	int SequentialPage;
}StrSection;

#define LGT_VAR_NAME 10
#define LGT_SYMBOL_STRING 10
#define LGT_SYMBOL_COMMENT 30
typedef struct StrSymbol
{
	char VarName[ LGT_VAR_NAME ];
	char Symbol[ LGT_SYMBOL_STRING ];
	char Comment[ LGT_SYMBOL_COMMENT ];
}StrSymbol;

#define TypeTime unsigned long
#define NBR_EVENTS_IN_LOG 100
#define NBR_CONFIG_EVENTS_LOG 20
//for current defaults list
#define NBR_CURRENT_DEFS_MAX 30
typedef struct StrEventLog
{
	int iIdEvent;
	TypeTime StartTime;
	TypeTime EndTime;
	short int StartTimeMilliSecs; // not used for now
	short int EndTimeMilliSecs; // not used for now...
	int ConfigArrayNum;
	int Parameter;
	char OldEventThatCouldNotBeDestroyed; //very old, but still not finished...
	unsigned char StartTransmitAlarmsFlags; // bits flags for each link to transmit on (app.event)
	unsigned char EndTransmitAlarmsFlags; // idem (disap.event)
}StrEventLog;
typedef struct StrLog
{
	StrEventLog Event[ NBR_EVENTS_IN_LOG ];
	int LastEvent;
	int NbrEvents;
}StrLog;
#define EVENT_SYMBOL_LGT 10
#define EVENT_TEXT_LGT 40
typedef struct StrConfigEventLog
{
//	int VarType; //only %Bxxx type, else difficult in WriteVar() to know which vars have to be logged
	int FirstVarNum;
	int NbrVars;
	int EventLevel; //(idea here: 0=simple event, 1=default, 2=big default, 3=very big, ...???)
	char Symbol[ EVENT_SYMBOL_LGT ];
	char Text[ EVENT_TEXT_LGT ];
	int RemoteAlarmsForwardSlot; // 1 bit per alarm slot (limited to 8 bits because of 'unsigned char' StartEndTransmitAlarmsFlags in each events!)
}StrConfigEventLog;

#define NBR_ALARMS_SLOTS 8 // depend of Start/End/TransmitAlarmsFlags & RemoteAlarmsForwardSlot on unsigned char
#define ALARMS_TYPE_NONE -1
#define ALARMS_TYPE_SMS 0
#define ALARMS_TYPE_EMAIL 1
#define TELEPHONE_NUMBER_LGT 30
#define EMAIL_LGT 60
typedef  struct StrRemoteAlarms
{
	char GlobalEnabled;
	char SlotName[ NBR_ALARMS_SLOTS ][ LGT_STR_INFO ];
	char AlarmType[ NBR_ALARMS_SLOTS ]; //0:none, 1:SMS, 2:emails
	char TelephoneNumber[ NBR_ALARMS_SLOTS ][ TELEPHONE_NUMBER_LGT ];
	char CenterServerSMS[ TELEPHONE_NUMBER_LGT ];
	char EmailAddress[ NBR_ALARMS_SLOTS ][ EMAIL_LGT ];
	char SmtpServerForEmails[ EMAIL_LGT ];
	char SmtpServerUserName[ LGT_STR_INFO ];
	char SmtpServerPassword[ LGT_STR_INFO ];
	char EmailSenderAddress[ EMAIL_LGT ];
}StrRemoteAlarms;

#define NBR_WINDOWS_PREFS 16
typedef struct StrWindowPosisOpenPrefs
{
	char WindowName[ 20 ]; //'\0' = free...
	int PosX;
	int PosY;
	int SizeX;
	int SizeY;
	char Opened;
}StrWindowPosisOpenPrefs;

#define MODEM_STRING_LGT 50
typedef struct StrModem
{
	char ModemUsed; //this one is not used for master preferences (choosed in the Gtk connect window)
	char StrInitSequence[ MODEM_STRING_LGT ]; // if contains ';' = many commands sends separately
	char StrConfigSequence[ MODEM_STRING_LGT ]; // idem
	char StrCallSequence[ MODEM_STRING_LGT ];
	char StrCodePIN[ TELEPHONE_NUMBER_LGT ];
}StrModem;

typedef struct StrPreferences
{
	char DefaultProjectFileNameToLoadAtStartup[ 400 ];
	int LatestStateSaved;
	char MonitorSlaveOnSerialPort[ 30 ]; // listening monitor requests on the serial link instead of udp network
	int MonitorSlaveSerialSpeed;
	char UseSysLog;
	char Daemonize;
	char UseRtcDevice;
	StrModem ModemForMasterMonitor;
	StrWindowPosisOpenPrefs WindowPosisOpenPrefs[ NBR_WINDOWS_PREFS ];
	char DisplaySymbolsInMainWindow;
	char DisplaySymbolsInBoolsVarsWindows;
}StrPreferences;

typedef struct StrSearchDatas
{
	int ElementOrSearchType; // ELE_FREE is no current search
	int VarType;
	int VarNum;
	// for move search
	int CurSearchSection;
	int CurSearchRung; // not used if sequential section
	int CurSearchPosX;
	int CurSearchPosY;
	// to pass current element
	char NewSearchStarting;
}StrSearchDatas;

typedef struct StrNetworkConfigDatas
{
	char IpAddr[ 30 ];
	char NetMask[ 30 ];
	char Route[ 30 ];
	char ServerDNS1[ 80 ];
	char ServerDNS2[ 80 ];
	char HostName[ 80 ];
}StrNetworkConfigDatas;

#define CREATE_FILE_SELECTION_TO_LOAD_PROJECT 0
#define CREATE_FILE_SELECTION_TO_SAVE_PROJECT 1
#define CREATE_FILE_SELECTION_TO_SELECT_UPDATE_SOFT 2

//Sizes of vars arrays to alloc, used for many vars types
#ifdef SEQUENTIAL_SUPPORT
#include "sequential.h"
#define SIZE_VAR_ARRAY (NBR_BITS+NBR_PHYS_INPUTS+NBR_PHYS_OUTPUTS+NBR_VARS_SYSTEM +NBR_STEPS)
#define SIZE_VAR_WORD_ARRAY (NBR_WORDS+NBR_PHYS_WORDS_INPUTS+NBR_PHYS_WORDS_OUTPUTS+NBR_VARS_WORDS_SYSTEM +NBR_STEPS)
#else
#define SIZE_VAR_ARRAY (NBR_BITS+NBR_PHYS_INPUTS+NBR_PHYS_OUTPUTS+NBR_VARS_SYSTEM)
#define SIZE_VAR_WORD_ARRAY (NBR_WORDS+NBR_PHYS_WORDS_INPUTS+NBR_PHYS_WORDS_OUTPUTS+NBR_VARS_WORDS_SYSTEM)
#endif
#define SIZE_VAR_SET_ARRAY (NBR_PHYS_INPUTS+NBR_PHYS_OUTPUTS)

void ClassicLadderEndOfAppli( void );
void DoPauseMilliSecs( int MilliSecsTime );
void DoFlipFlopRunStop( void );
void DoFlipFlopRunOneCycleFreeze( void );
void StopRunIfRunning( void );
void RunBackIfStopped( void );
void DoReset( void );
void HardwareActionsAfterProjectLoaded( void );
void AdjustPeriodicValuesForTasks( void );

void InitGeneralParamsMirror( void );
int ClassicLadder_AllocAllInfos( void );
void ClassicLadder_FreeAllInfos( char CleanAndRemoveTmpDir );
int ClassicLadder_AllocNewProjectDatas( void );
int ClassicLadder_AllocProjectDatas( void );
void ClassicLadder_FreeProjectDatas( void );
void ClassicLadder_InitProjectDatas( void );
void InitTargetMonitorAsks( void );
int ClassicLadder_FreeAllocInitProjectDatas( void );

void UpdateSizesOfConvVarNameTable( void );


// rt_debug_printf => real-time (if Linux = normal printf !)
// debug_printf => normal printf
#ifdef __RTL__
#include <rtl_printf.h>
#define rt_debug_printf rtl_printf
#define debug_printf printf
#endif

#if defined( RTAI ) && defined( MODULE )
#define rt_debug_printf rt_printk
#define debug_printf printf
#endif

#ifndef MODULE
#define rt_debug_printf printf
#define debug_printf printf
#endif

// for EMC to be able to have specific headers in the many (rt or not) debug printf
//#define DBG_HEADER_INFO "ClassicLadder Info --- "
//#define DBG_HEADER_ERR "ClassicLadder Error --- "
#define DBG_HEADER_INFO ""
#define DBG_HEADER_ERR ""

#define FIFO_CLASSICLAUNCHER "/tmp/clrunext"
