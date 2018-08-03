/* Classic Ladder Project */
/* Copyright (C) 2001-2017 Marc Le Douarain */
/* http://www.sourceforge.net/projects/classicladder */
/* http://sites.google.com/site/classicladder */
/* December 2010 */
/* ------------------------------------------------------ */
/* Menus & Toolbar - GTK window                           */
/* ------------------------------------------------------ */
/* Many thanks to Heli Tejedor for his modified version   */
/* with idea of possible menus and toolbar feature        */
/* ------------------------------------------------------ */
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
#include <stdlib.h>
#include <libintl.h> // i18n
#include <locale.h> // i18n

#include "classicladder.h"
#include "global.h"
#include "classicladder_gtk.h"
#include "print_gtk.h"
#include "edit_gtk.h"
#include "symbols_gtk.h"
#include "manager_gtk.h"
#include "config_gtk.h"
#include "spy_vars_gtk.h"
#include "log_events_gtk.h"
#include "monitor_threads.h"
#include "search.h"
#include "manager_gtk.h"
#include "monitor_windows_gtk.h"
#include "monitor_transfer.h"
#include "network_config_window_gtk.h"
#include "monitor_serial_config_window_gtk.h"
#include "calc.h"
#include "menu_and_toolbar_gtk.h"

GtkUIManager * uiManager;

static GtkActionEntry ActionEntriesArray[ ] =
{	{ "FileMenuAction", NULL, N_("File") },
	{ "NewAction", GTK_STOCK_NEW, N_("New"), NULL, N_("Create a new project"), G_CALLBACK( DoActionConfirmNewProject ) },
	{ "LoadAction", GTK_STOCK_OPEN, N_("Load"), NULL, N_("Load an existing project"), G_CALLBACK( DoActionLoadProject ) },
	{ "SaveAction", GTK_STOCK_SAVE, N_("Save"), NULL, N_("Save current project"), G_CALLBACK( DoActionSave ) },
	{ "SaveAsAction", GTK_STOCK_SAVE_AS, N_("Save As..."), "<Shift><Control>S", N_("Save project to another file"), G_CALLBACK( DoActionSaveAs ) },
	{ "ExportMenuAction", GTK_STOCK_CONVERT, N_("Export to"), NULL, NULL, NULL },
	{ "ExportSvgAction", GTK_STOCK_DND, "Svg", NULL, NULL, G_CALLBACK( DoActionExportSvg ) },
	{ "ExportPngAction", GTK_STOCK_SELECT_COLOR, "Png", NULL, NULL, G_CALLBACK( DoActionExportPng ) },
	{ "CopyToClipboardAction", GTK_STOCK_PASTE, N_("Clipboard"), "<Control>C", NULL, G_CALLBACK( DoActionCopyToClipboard ) },
	{ "PreviewAction", GTK_STOCK_PRINT_PREVIEW, N_("Preview"), "<Shift><Control>P", NULL, G_CALLBACK( PrintPreviewGtk ) },
	{ "PrintAction", GTK_STOCK_PRINT, N_("Print"), "<Control>P", N_("Print current section"), G_CALLBACK( PrintGtk ) },
	{ "QuitAction", GTK_STOCK_QUIT, N_("Quit"), NULL, NULL, G_CALLBACK( ConfirmQuit ) },

	{ "ViewMenuAction", NULL, N_("View") },

	{ "ViewRegisterBlockContent", NULL, N_("Register block content"), NULL, N_("View register block content"), G_CALLBACK( MenuViewRegisterBlockContent ) },
	{ "ViewFramesLogWindowAction", NULL, N_("Frames log windows") },
	{ "ViewLinuxSysLogDebug", NULL, N_("Linux SysLog debug"), NULL, N_("View Linux SysLog debug"), G_CALLBACK( MenuViewLinuxSysLogDebug ) },

	{ "FindMenuAction", NULL, N_("Search") },
	{ "FindOpenAction", GTK_STOCK_FIND, N_("Find"), NULL, N_("Find First"), G_CALLBACK( FunctionSearchOpenBox ) },
	{ "FindNextAction", GTK_STOCK_GO_FORWARD, N_("Find Next"), "<Control>G", N_("Find Next"), G_CALLBACK( SearchAndGoToNextElement ) },
	{ "FindPrevAction", GTK_STOCK_GO_BACK, N_("Find Previous"), "<Shift><Control>G", N_("Find Down"), G_CALLBACK( SearchAndGoToPreviousElement ) },
	{ "GoToFirstRungAction", GTK_STOCK_GOTO_TOP, N_("Go to First Rung"), "<Control>T", N_("Go to First Rung"), G_CALLBACK( GoToFirstRung ) },
	{ "GoToLastRungAction", GTK_STOCK_GOTO_BOTTOM, N_("Go to Last Rung"), "<Shift><Control>T", N_("Go to Last Rung"), G_CALLBACK( GoToLastRung ) },
	{ "GoToPrevSectionAction", GTK_STOCK_MEDIA_REWIND, N_("Go to Previous Section"), "<Control>R", N_("Go to Previous Section"), G_CALLBACK( SelectPreviousSectionInManager ) },
	{ "GoToNextSectionAction", GTK_STOCK_MEDIA_FORWARD, N_("Go to Next Section"), "<Shift><Control>R", N_("Go to Next Section"), G_CALLBACK( SelectNextSectionInManager ) },

	{ "PLCAction", NULL, N_("PLC") },
#ifdef COMPLETE_PLC
	{ "ConnectDisconnectAction", GTK_STOCK_CONNECT, "ConnectDisconnect", NULL, "Connect/Disconnect", G_CALLBACK( DoFlipFlopConnectDisconnectTargetInGtk ) },
#endif
	{ "RunStopAction", GTK_STOCK_EXECUTE, N_("Run logic"), NULL, N_("Start/stop logic"), G_CALLBACK( DoFlipFlopRunStop ) },
	{ "RunFreezeAction", GTK_STOCK_EXECUTE, N_("Run logic only one cycle"), NULL, N_("Run logic one cycle/freeze logic"), G_CALLBACK( DoFlipFlopRunOneCycleFreeze ) },
	{ "ResetAction", GTK_STOCK_REFRESH, N_("Reset logic"), NULL, N_("Reset logic"), G_CALLBACK( DoActionResetAndConfirmIfRunning ) },
	{ "ConfigurationAction", GTK_STOCK_PREFERENCES, N_("Configuration"), NULL, N_("Configuration (sizes, i/o, ...)"), G_CALLBACK( OpenConfigWindowGtk ) },
#ifdef COMPLETE_PLC
	{ "SetTargetClockTimeAction", GTK_STOCK_JUMP_TO, N_("Set Target Clock Time"), NULL, N_("Set Clock Time of the Target with PC Time"), G_CALLBACK( MenuToSetTargetClockTime ) },
	{ "RebootOrHaltTargetAction", GTK_STOCK_CAPS_LOCK_WARNING, N_("Reboot/Halt Target"), NULL, NULL, NULL },
	{ "RebootTargetAction", GTK_STOCK_CAPS_LOCK_WARNING, N_("Reboot Target"), NULL, N_("Ask to reboot the target"), G_CALLBACK( MenuRebootTarget ) },
	{ "HaltTargetAction", GTK_STOCK_CAPS_LOCK_WARNING, N_("Halt Target"), NULL, N_("Ask to halt the target"), G_CALLBACK( MenuHaltTarget ) },
	{ "TargetNetworkConfigAction", GTK_STOCK_NETWORK, N_("Target network config"), NULL, N_("See and modify target IP network parameters"), G_CALLBACK( OpenNetworkConfigDialog ) },
	{ "TargetMonitorSerialConfigAction", GTK_STOCK_NETWORK, N_("Target monitor serial config"), NULL, N_("See and modify target monitor serial config"), G_CALLBACK( OpenMonitorSerialConfigDialog ) },
	{ "TransferMenuAction", GTK_STOCK_NETWORK, N_("File Transfer"), NULL, NULL, NULL },
	{ "SendProjectToTargetAction", GTK_STOCK_REDO, N_("Send current project to Target"), NULL, NULL, G_CALLBACK( MenuSendProjectToTargetAction ) },
	{ "ReceiveProjectFromTargetAction", GTK_STOCK_UNDO, N_("Receive project of Target"), NULL, NULL, G_CALLBACK( MenuReceiveProjectFromTargetAction ) },
	{ "SendUpdateSoftToTargetAction", GTK_STOCK_REDO, N_("Send update soft archive to Target"), NULL, NULL, G_CALLBACK( MenuSendUpdateSoftToTargetAction ) },
#endif

	{ "HelpMenuAction", NULL, N_("Help") },
	{ "AboutAction", GTK_STOCK_ABOUT, N_("About"), "F1", NULL, G_CALLBACK( DoActionAboutClassicLadder ) },

	{ "SectionsManagerAction", GTK_STOCK_DND_MULTIPLE, N_("Sections Manager"), NULL, N_("Open Sections Manager Window"), G_CALLBACK( OpenManagerWindowFromPopup ) },

	{ "EditAddRungAction", GTK_STOCK_ADD, N_("Add rung (alt-a)"), NULL, N_("Add rung"), G_CALLBACK( ButtonAddRung ) },
	{ "EditInsertRungAction", GTK_STOCK_INDENT, N_("Insert rung (alt-i)"), NULL, N_("Insert rung"), G_CALLBACK( ButtonInsertRung ) },
	{ "EditDeleteRungAction", GTK_STOCK_DELETE, N_("Delete rung (alt-x)"), NULL, N_("Delete rung"), G_CALLBACK( ButtonDeleteCurrentRung ) },
	{ "EditModifyAction", GTK_STOCK_EDIT, N_("Modify (alt-m)"), NULL, N_("Modify"), G_CALLBACK( ButtonModifyCurrentRung ) },

	{ "PointerAction", GTK_STOCK_OK, N_("Pointer (alt-p)"), NULL, N_("Pointer"), G_CALLBACK( SelectToolPointer ) },
	{ "EraserAction", "IconEraser", N_("Eraser (alt-x)"), NULL, N_("Erase"), G_CALLBACK( SelectToolEraser ) },
	{ "DrawHLineAction", "IconWire", N_("Draw H line (alt-h)"), NULL, N_("Draw H line"), G_CALLBACK( SelectToolDrawHLine ) },
	{ "DrawHLineToEndAction", "IconWireLong", N_("Draw H line to end (alt-l)"), NULL, N_("Draw H line to end"), G_CALLBACK( SelectToolDrawHLineToEnd ) },
	{ "ConnectVLineAction", "IconConnection", N_("Connect V line (alt-v)"), NULL, N_("Connect V line"), G_CALLBACK( SelectToolConnectVLine ) },

	{ "Contacts", "IconOpenContact", N_("Contacts"), NULL, N_("Contacts"), NULL },
	{ "OpenContactSelection", "IconOpenContact", N_("Open contact (alt-i)"), NULL, N_("Open contact"), G_CALLBACK( SelectToolOpenContact ) },
	{ "ClosedContactSelection", "IconClosedContact", N_("Closed contact"), NULL, N_("Closed contact"), G_CALLBACK( SelectToolClosedContact ) },
	{ "RisingEdgeSelection", "IconRisingEdge", N_("Rising edge"), NULL, N_("Rising edge"), G_CALLBACK( SelectToolRisingEdgeContact ) },
	{ "FallingEdgeSelection", "IconFallingEdge", N_("Falling edge"), NULL, N_("Falling edge"), G_CALLBACK( SelectToolFallingEdgeContact ) },

	{ "Coils", "IconCoil", N_("Coils"), NULL, N_("Coils"), NULL },
	{ "CoilSelection", "IconCoil", N_("Coil (alt-o)"), NULL, N_("Coil"), G_CALLBACK( SelectToolCoil ) },
	{ "CoilNotSelection", "IconCoilNot", N_("Inverted coil"), NULL, N_("Inverted coil"), G_CALLBACK( SelectToolCoilNot ) },
	{ "SetCoilSelection", "IconSetCoil", N_("Set"), NULL, N_("Set"), G_CALLBACK( SelectToolSetCoil ) },
	{ "ResetCoilSelection", "IconResetCoil", N_("Reset"), NULL, N_("Reset"), G_CALLBACK( SelectToolResetCoil ) },
	{ "JumpCoilSelection", "IconJumpCoil", N_("Jump"), NULL, N_("Jump"), G_CALLBACK( SelectToolJumpCoil ) },
	{ "CallCoilSelection", "IconCallCoil", N_("Call"), NULL, N_("Call"), G_CALLBACK( SelectToolCallCoil ) },

	{ "Boxes", "IconCounterBox", N_("Boxes"), NULL, N_("Boxes"), NULL },
	{ "IECTimerSelection", "IconIECTimerBox", N_("IEC timer"), NULL, N_("IEC timer"), G_CALLBACK( SelectToolIECTimer ) },
	{ "CounterSelection", "IconCounterBox", N_("Counter"), NULL, N_("Counter"), G_CALLBACK( SelectToolCounter ) },
#ifdef OLD_TIMERS_MONOS_SUPPORT
	{ "OldTimerSelection", "IconOldTimerBox", N_("Old timer"), NULL, N_("Old timer"), G_CALLBACK( SelectToolOldTimer ) },
	{ "OldMonostableSelection", "IconOldMonoBox", N_("Old mono"), NULL, N_("Old mono"), G_CALLBACK( SelectToolOldMonostable ) },
#endif
	{ "CompareSelection", "IconCompareBox", N_("Compare"), NULL, N_("Compare"), G_CALLBACK( SelectToolCompare ) },
	{ "OperateSelection", "IconOperateBox", N_("Operate"), NULL, N_("Operate"), G_CALLBACK( SelectToolOperate ) },

	{ "Actions", GTK_STOCK_PAGE_SETUP, N_("Actions"), NULL, N_("Actions"), NULL },
	{ "InvertAction", GTK_STOCK_CONVERT, N_("Element invert"), NULL, N_("Element invert"), G_CALLBACK( SelectToolInvertElement ) },
	{ "SelectBlockAction", GTK_STOCK_COPY, N_("Block Select"), NULL, N_("Select"), G_CALLBACK( SelectToolSelectBlock ) },
	{ "CopyBlockAction", GTK_STOCK_PASTE, N_("Block Copy"), NULL, N_("Copy"), G_CALLBACK( SelectToolCopyBlock ) },
	{ "MoveBlockAction", GTK_STOCK_PASTE, N_("Block Move"), NULL, N_("Move"), G_CALLBACK( SelectToolMoveBlock ) },

	{ "EditOkAction", GTK_STOCK_APPLY, N_("Save (alt-Return)"), NULL, N_("Save"), G_CALLBACK( ButtonOkCurrentRung ) },
	{ "EditCancelAction", GTK_STOCK_CANCEL, N_("Cancel (alt-c)"), NULL, N_("Cancel"), G_CALLBACK( ButtonCancelCurrentRung ) },

#ifdef SEQUENTIAL_SUPPORT
	{ "SeqStepSelection", "IconSeqStep", N_("Step"), NULL, N_("Step"), G_CALLBACK( SelectToolSeqStep ) },
	{ "SeqInitStepSelection", "IconSeqInitStep", N_("Init. Step"), NULL, N_("Init. Step"), G_CALLBACK( SelectToolSeqInitStep ) },
	{ "SeqTransiSelection", "IconSeqTransi", N_("Transition"), NULL, N_("Transition"), G_CALLBACK( SelectToolSeqTransi ) },
	{ "SeqStepAndTransiSelection", "IconSeqStepAndTransi", N_("Step And Transition"), NULL, N_("Step And Transition"), G_CALLBACK( SelectToolSeqStepAndTransi ) },
	{ "SeqTransisOr1Selection", "IconSeqTransisOr1", N_("Transitions Or Start"), NULL, N_("Transitions Or Start"), G_CALLBACK( SelectToolSeqTransisOr1 ) },
	{ "SeqTransisOr2Selection", "IconSeqTransisOr2", N_("Transitions Or End"), NULL, N_("Transitions Or End"), G_CALLBACK( SelectToolSeqTransisOr2 ) },
	{ "SeqStepsAnd1Selection", "IconSeqStepsAnd1", N_("Steps And Start"), NULL, N_("Steps And Start"), G_CALLBACK( SelectToolSeqStepsAnd1 ) },
	{ "SeqStepsAnd2Selection", "IconSeqStepsAnd2", N_("Steps And End"), NULL, N_("Steps And End"), G_CALLBACK( SelectToolSeqStepsAnd2 ) },
	{ "SeqLinkSelection", "IconSeqLink", N_("Link"), NULL, N_("Link"), G_CALLBACK( SelectToolSeqLink ) },
	{ "SeqCommentSelection", "IconSeqComment", N_("Comment"), NULL, N_("Comment"), G_CALLBACK( SelectToolSeqComment ) }
#endif
};
static GtkToggleActionEntry ToggleActionEntriesArray[ ] =
{	{ "ViewSectionsAction", GTK_STOCK_DND_MULTIPLE, N_("Sections window"), "F2", N_("View sections manager window"), G_CALLBACK( OpenManagerWindow ), TRUE },
	{ "ViewEditorAction", GTK_STOCK_EDIT, N_("Editor window"), "F3", N_("View editor window"), G_CALLBACK( OpenEditWindow ), FALSE },
	{ "ViewSymbolsAction", GTK_STOCK_SELECT_FONT, N_("Symbols window"), "F4", N_("View symbols window"), G_CALLBACK( OpenSymbolsWindow ), FALSE },
	{ "ViewBoolVarsAction", NULL, N_("Bools vars window"), "F5", NULL, G_CALLBACK( OpenSpyBoolVarsWindow ), FALSE },
	{ "ViewFreeVarsAction", NULL, N_("Free vars window"), "F6", NULL, G_CALLBACK( OpenSpyFreeVarsWindow ), FALSE },
#ifdef COMPLETE_PLC
	{ "ViewLogAction", NULL, N_("Events log window"), "F7", NULL, G_CALLBACK( OpenLogBookWindow ), FALSE },
#endif
	{ "ViewMonitor4Action", NULL, N_("Monitor master frames with target"), NULL, NULL, G_CALLBACK( OpenMonitorWindow4 ), FALSE },
	{ "ViewMonitor0Action", NULL, N_("Modbus master frames"), NULL, NULL, G_CALLBACK( OpenMonitorWindow0 ), FALSE },
	{ "ViewMonitor1Action", NULL, N_("Target monitor slave (IP) frames"), NULL, NULL, G_CALLBACK( OpenMonitorWindow1 ), FALSE },
	{ "ViewMonitor2Action", NULL, N_("Target monitor slave (Serial) frames"), NULL, NULL, G_CALLBACK( OpenMonitorWindow2 ), FALSE },
	{ "ViewMonitor3Action", NULL, N_("Modbus slave frames"), NULL, NULL, G_CALLBACK( OpenMonitorWindow3 ), FALSE },
};

static const gchar *ClassicLadder_ui_strings = 
"<ui>"
"	<menubar name='MenuBar'>"
"		<menu action='FileMenuAction'>"
"			<menuitem action='NewAction' />"
"			<menuitem action='LoadAction' />"
"			<menuitem action='SaveAction' />"
"			<menuitem action='SaveAsAction' />"
"			<separator />"
"			<menu action='ExportMenuAction'>"
"				<menuitem action='ExportSvgAction' />"
"				<menuitem action='ExportPngAction' />"
"				<menuitem action='CopyToClipboardAction' />"
"			</menu>"
"			<separator />"
"			<menuitem action='PreviewAction' />"
"			<menuitem action='PrintAction' />"
"			<separator />"
"			<menuitem action='QuitAction' />"
"		</menu>"
"		<menu action='FindMenuAction'>"
"			<menuitem action='FindOpenAction' />"
"			<menuitem action='FindNextAction' />"
"			<menuitem action='FindPrevAction' />"
"			<separator />"
"			<menuitem action='GoToFirstRungAction' />"
"			<menuitem action='GoToLastRungAction' />"
"			<separator />"
"			<menuitem action='GoToPrevSectionAction' />"
"			<menuitem action='GoToNextSectionAction' />"
"		</menu>"
"		<menu action='ViewMenuAction'>"
"			<menuitem action='ViewSectionsAction' />"
"			<menuitem action='ViewEditorAction' />"
"			<menuitem action='ViewSymbolsAction' />"
"			<menuitem action='ViewBoolVarsAction' />"
"			<menuitem action='ViewFreeVarsAction' />"
"           <menuitem action='ViewRegisterBlockContent' />"
#ifdef COMPLETE_PLC
"			<menuitem action='ViewLogAction' />"
#endif
"           <menu action='ViewFramesLogWindowAction'>"
"				<menuitem action='ViewMonitor4Action' />"
"				<menuitem action='ViewMonitor0Action' />"
"				<menuitem action='ViewMonitor1Action' />"
"				<menuitem action='ViewMonitor2Action' />"
"				<menuitem action='ViewMonitor3Action' />"
"			</menu>"
"           <menuitem action='ViewLinuxSysLogDebug' />"
"		</menu>"
"		<menu action='PLCAction'>"
#ifdef COMPLETE_PLC
"			<menuitem action='ConnectDisconnectAction' />"
#endif
"			<separator />"
"			<menuitem action='RunStopAction' />"
"			<menuitem action='RunFreezeAction' />"
"			<menuitem action='ResetAction' />"
"			<separator />"
"			<menuitem action='ConfigurationAction' />"
"			<separator />"
#ifdef COMPLETE_PLC
"			<menuitem action='SetTargetClockTimeAction' />"
"			<menu action='RebootOrHaltTargetAction'>"
"				<menuitem action='RebootTargetAction' />"
"				<menuitem action='HaltTargetAction' />"
"			</menu>"	
"			<menuitem action='TargetNetworkConfigAction' />"
"			<menuitem action='TargetMonitorSerialConfigAction' />"
#endif
"			<separator />"
"			<menu action='TransferMenuAction'>"
"				<menuitem action='SendProjectToTargetAction' />"
"				<menuitem action='ReceiveProjectFromTargetAction' />"
"				<menuitem action='SendUpdateSoftToTargetAction' />"
"			</menu>"
"		</menu>"
"		<menu action='HelpMenuAction'>"
"			<menuitem action='AboutAction' />"
"		</menu>"
"	</menubar>"
"	<toolbar name='ToolBar'>"
"		<toolitem action='NewAction' />"
"		<toolitem action='LoadAction' />"
"		<toolitem action='SaveAction' />"
"		<separator />"
"		<toolitem action='PrintAction' />"
"		<separator />"
"		<toolitem action='RunStopAction' />"
"		<toolitem action='ResetAction' />"
"		<toolitem action='ConfigurationAction' />"
"		<separator />"
"		<toolitem action='ViewSectionsAction' />"
"		<toolitem action='ViewEditorAction' />"
"		<toolitem action='ViewSymbolsAction' />"
"		<separator />"
"	</toolbar>"
"	<popup action='PopUpMenuLadder'>"
"		<menuitem action='EditModifyAction' />"
"		<menuitem action='EditAddRungAction' />"
"		<menuitem action='EditInsertRungAction' />"
"		<menuitem action='EditDeleteRungAction' />"
"		<separator />"
" 		<menuitem action='GoToFirstRungAction' />"
" 		<menuitem action='GoToLastRungAction' />"
"		<separator />"
" 		<menuitem action='GoToPrevSectionAction' />"
" 		<menuitem action='GoToNextSectionAction' />"
"		<menuitem action='SectionsManagerAction' />"
"	</popup>"
"	<popup action='PopUpMenuSequential'>"
"		<menuitem action='EditModifyAction' />"
"		<separator />"
" 		<menuitem action='GoToPrevSectionAction' />"
" 		<menuitem action='GoToNextSectionAction' />"
"		<menuitem action='SectionsManagerAction' />"
"	</popup>"
"	<popup action='PopUpEditLadder'>"
"		<menuitem action='PointerAction'/>"
"		<menuitem action='EraserAction'/>"
"		<menuitem action='DrawHLineAction'/>"
"		<menuitem action='DrawHLineToEndAction'/>"
"		<menuitem action='ConnectVLineAction'/>"
"	 	<separator />"
"		<menu action='Contacts'>"
"			<menuitem action='OpenContactSelection'/>"
"			<menuitem action='ClosedContactSelection'/>"
"			<menuitem action='RisingEdgeSelection'/>"
"			<menuitem action='FallingEdgeSelection'/>"
"		</menu>"
"		<menu action='Coils'>"
"			<menuitem action='CoilSelection'/>"
"			<menuitem action='CoilNotSelection'/>"
"			<menuitem action='SetCoilSelection'/>"
"			<menuitem action='ResetCoilSelection'/>"
"			<menuitem action='JumpCoilSelection'/>"
"			<menuitem action='CallCoilSelection'/>"
"		</menu>"
"		<menu action='Boxes'>"
"			<menuitem action='IECTimerSelection'/>"
"			<menuitem action='CounterSelection'/>"
#ifdef OLD_TIMERS_MONOS_SUPPORT
"			<menuitem action='OldTimerSelection'/>"
"			<menuitem action='OldMonostableSelection'/>"
#endif
"			<menuitem action='CompareSelection'/>"
"			<menuitem action='OperateSelection'/>"
"		</menu>"
"	 	<separator />"
"		<menu action='Actions'>"
"			<menuitem action='InvertAction'/>"
"			<menuitem action='SelectBlockAction'/>"
"			<menuitem action='CopyBlockAction'/>"
"			<menuitem action='MoveBlockAction'/>"
"		</menu>"
"	 	<separator />"
" 		<menuitem action='EditOkAction' />"
" 		<menuitem action='EditCancelAction' />"
"	</popup>"
"	<popup action='PopUpEditSequential'>"
"		<menuitem action='PointerAction'/>"
"		<menuitem action='EraserAction'/>"
"	 	<separator />"
"		<menuitem action='SeqStepSelection'/>"
"		<menuitem action='SeqInitStepSelection'/>"
"		<menuitem action='SeqTransiSelection'/>"
"		<menuitem action='SeqStepAndTransiSelection'/>"
"	 	<separator />"
"		<menuitem action='SeqTransisOr1Selection'/>"
"		<menuitem action='SeqTransisOr2Selection'/>"
"		<menuitem action='SeqStepsAnd1Selection'/>"
"		<menuitem action='SeqStepsAnd2Selection'/>"
"	 	<separator />"
"		<menuitem action='SeqLinkSelection'/>"
"		<menuitem action='SeqCommentSelection'/>"
"	 	<separator />"
" 		<menuitem action='EditOkAction' />"
" 		<menuitem action='EditCancelAction' />"
"	 	<separator />"
"	</popup>"
"</ui>";

GtkUIManager * InitMenusAndToolBar( GtkWidget *vbox )
{
	GtkActionGroup * ActionGroup;
	GError *error = NULL;
	uiManager = gtk_ui_manager_new( );
	
	ActionGroup = gtk_action_group_new( "ClassicLadderActionGroup" );

	// for i18n menus !
	gtk_action_group_set_translation_domain( ActionGroup, "classicladder" );

	gtk_action_group_add_actions( ActionGroup, ActionEntriesArray, G_N_ELEMENTS( ActionEntriesArray ), NULL );
	gtk_action_group_add_toggle_actions( ActionGroup, ToggleActionEntriesArray, G_N_ELEMENTS( ToggleActionEntriesArray ), NULL/*user_data*/ );
	

	gtk_ui_manager_insert_action_group( uiManager, ActionGroup, 0 );
	if (!gtk_ui_manager_add_ui_from_string( uiManager, ClassicLadder_ui_strings, -1/*length*/, &error ))
	{
		g_message ("Failed to build gtk menus: %s", error->message);
		g_error_free (error);
	}
	else
	{
		GtkWidget * ToolBarWidget = gtk_ui_manager_get_widget( uiManager, "/ToolBar" );
		gtk_box_pack_start( GTK_BOX(vbox), gtk_ui_manager_get_widget( uiManager, "/MenuBar" ), FALSE, FALSE, 0 );
		//do not display text under icons (seems to be the case per default under Windows...)
		gtk_toolbar_set_style( GTK_TOOLBAR(ToolBarWidget), GTK_TOOLBAR_ICONS );
//		gtk_toolbar_set_style( GTK_TOOLBAR(ToolBarWidget), GTK_TOOLBAR_BOTH );
		gtk_box_pack_start( GTK_BOX(vbox), ToolBarWidget, FALSE, FALSE, 0 );
	}
	return uiManager;
}

void EnableDisableMenusAccordingSectionType( )
{
	int iCurrentLanguage = SectionArray[ InfosGene->CurrentSection ].Language;
	GtkWidget * MenuElement;
	MenuElement = gtk_ui_manager_get_widget( uiManager, "/MenuBar/FindMenuAction/GoToFirstRungAction" );
	gtk_widget_set_sensitive( MenuElement, !(iCurrentLanguage==SECTION_IN_SEQUENTIAL) );
	MenuElement = gtk_ui_manager_get_widget( uiManager, "/MenuBar/FindMenuAction/GoToLastRungAction" );
	gtk_widget_set_sensitive( MenuElement, !(iCurrentLanguage==SECTION_IN_SEQUENTIAL) );
}

// called at startup (if window saved open or not), and when window closed...
void SetToogleMenuForSectionsManagerWindow( gboolean OpenedWin )
{
	GtkWidget *ToggleElement = gtk_ui_manager_get_widget( uiManager, "/MenuBar/ViewMenuAction/ViewSectionsAction" );
//ForGTK3	gtk_check_menu_item_set_state( GTK_CHECK_MENU_ITEM(ToggleElement), OpenedWin );
	gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM(ToggleElement), OpenedWin );
}
void SetToggleMenuForEditorWindow( gboolean OpenedWin )
{
//printf("SetToggleMenuForEditorWindow function called with state %d!\n",OpenedWin);
	GtkWidget *ToggleElement = gtk_ui_manager_get_widget( uiManager, "/MenuBar/ViewMenuAction/ViewEditorAction" );
//ForGTK3	gtk_check_menu_item_set_state( GTK_CHECK_MENU_ITEM(ToggleElement), OpenedWin );
	gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM(ToggleElement), OpenedWin );
}
void SetToggleMenuForSymbolsWindow( gboolean OpenedWin )
{
//printf("%s get pointer on toggle element\n", __FUNCTION__ );
	GtkWidget *ToggleElement = gtk_ui_manager_get_widget( uiManager, "/MenuBar/ViewMenuAction/ViewSymbolsAction" );
//ForGTK3	gtk_check_menu_item_set_state( GTK_CHECK_MENU_ITEM(ToggleElement), OpenedWin );
//printf("%s set toggle element %d\n", __FUNCTION__, OpenedWin );
	gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM(ToggleElement), OpenedWin );
}
void SetToggleMenuForBoolVarsWindow( gboolean OpenedWin )
{
	GtkWidget *ToggleElement = gtk_ui_manager_get_widget( uiManager, "/MenuBar/ViewMenuAction/ViewBoolVarsAction" );
//ForGTK3	gtk_check_menu_item_set_state( GTK_CHECK_MENU_ITEM(ToggleElement), OpenedWin );
	gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM(ToggleElement), OpenedWin );
}
void SetToggleMenuForFreeVarsWindow( gboolean OpenedWin )
{
	GtkWidget *ToggleElement = gtk_ui_manager_get_widget( uiManager, "/MenuBar/ViewMenuAction/ViewFreeVarsAction" );
//ForGTK3	gtk_check_menu_item_set_state( GTK_CHECK_MENU_ITEM(ToggleElement), OpenedWin );
	gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM(ToggleElement), OpenedWin );
}
#ifdef COMPLETE_PLC
void SetToggleMenuForLogWindow( gboolean OpenedWin )
{
	GtkWidget *ToggleElement = gtk_ui_manager_get_widget( uiManager, "/MenuBar/ViewMenuAction/ViewLogAction" );
//ForGTK3	gtk_check_menu_item_set_state( GTK_CHECK_MENU_ITEM(ToggleElement), OpenedWin );
	gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM(ToggleElement), OpenedWin );
}
#endif
void SetToggleMenuForMonitorWindow( int NumFramesLogWindow, gboolean OpenedWin )
{
	char Buff[ 100 ];
	sprintf(Buff, "/MenuBar/ViewMenuAction/ViewFramesLogWindowAction/ViewMonitor%dAction", NumFramesLogWindow );
	GtkWidget *ToggleElement = gtk_ui_manager_get_widget( uiManager, Buff );
//ForGTK3	gtk_check_menu_item_set_state( GTK_CHECK_MENU_ITEM(ToggleElement), OpenedWin );
	gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM(ToggleElement), OpenedWin );
}

//toggle function ourself, depends on "status running/stopped..." 
gboolean SetGtkMenuStateForRunStopSwitch( gboolean * Running )
{
	printf("-> %s() called with %s\n",__FUNCTION__,(*Running)?"Running":"Stopped");
	GtkWidget *Element = gtk_ui_manager_get_widget( uiManager, "/MenuBar/PLCAction/RunStopAction" );
	gtk_menu_item_set_label( GTK_MENU_ITEM(Element), (*Running)?(_("Stop logic")):(_("Run logic")) );
	Element = gtk_ui_manager_get_widget( uiManager, "/MenuBar/PLCAction/RunFreezeAction" );
	gtk_menu_item_set_label( GTK_MENU_ITEM(Element), (*Running)?(_("Freeze logic")):(_("Run logic one cycle")) );
	
	Element = gtk_ui_manager_get_widget( uiManager, "/ToolBar/RunStopAction" );
	gtk_tool_button_set_stock_id(GTK_TOOL_BUTTON(Element), (*Running)?GTK_STOCK_STOP:GTK_STOCK_EXECUTE );
	gtk_tool_button_set_label( GTK_TOOL_BUTTON(Element), (*Running)?(_("Stop")):(_("Run")) );
	gtk_tool_item_set_tooltip_text( GTK_TOOL_ITEM(Element), (*Running)?(_("Stop logic")):(_("Run logic")) );
	return FALSE; //usefull when called with g_idle_add (just one time)
}

#ifdef COMPLETE_PLC
void SetGtkMenuStateForConnectDisconnectSwitch( gboolean TargetConnected )
{
	GtkWidget *Element = gtk_ui_manager_get_widget( uiManager, "/MenuBar/PLCAction/ConnectDisconnectAction" );
	gtk_menu_item_set_label( GTK_MENU_ITEM(Element), TargetConnected?(_("Disconnect")):(_("Connect")) );
}
#endif


#ifdef COMPLETE_PLC
void MenuToSetTargetClockTime( void )
{
	if ( !InfosGUI->TargetMonitor.RemoteConnected )
		ShowMessageBoxError( _("You are not currently connected to a remote target...") );
	else
		InfosGUI->TargetMonitor.AskTargetToSetClockTime = 1;
}
void MenuRebootTarget( void )
{
//test LaunchExternalCommand( "/bin/cp" );
//test LaunchExternalCommand( "/etc/lsb-release" );
//test LaunchExternalCommand( "/tmp/toto.txt\t" ); // '\t' = end of command list !
	if ( !InfosGUI->TargetMonitor.RemoteConnected )
		ShowMessageBoxError( _("You are not currently connected to a remote target...") );
	else
		InfosGUI->TargetMonitor.AskTargetToReboot = 1; //reboot
}
void MenuHaltTarget( void )
{
	if ( !InfosGUI->TargetMonitor.RemoteConnected )
		ShowMessageBoxError( _("You are not currently connected to a remote target...") );
	else
		InfosGUI->TargetMonitor.AskTargetToReboot = 2; //halt
}
void MenuSendProjectToTargetAction( void )
{
	if ( OpenDialogMonitorConnect( ) )
	{
printf("*** HERE SET REMOTE_FILE_TRANSFER ---PROJECT--- TO SEND!\n");
		InfosGUI->TargetMonitor.TransferFileNum = 0;
		InfosGUI->TargetMonitor.TransferFileIsSend = TRUE;
		TransferMasterStart( );
	}
}
void MenuReceiveProjectFromTargetAction( void )
{
	if ( OpenDialogMonitorConnect( ) )
	{
printf("*** HERE SET REMOTE_FILE_TRANSFER ---PROJECT--- TO RECEIVE!\n");
		InfosGUI->TargetMonitor.TransferFileNum = 0;
		InfosGUI->TargetMonitor.TransferFileIsSend = FALSE;
		TransferMasterStart( );
	}
}
void MenuSendUpdateSoftToTargetAction( void )
{
	CreateFileSelection( _("Please select the update soft archive to send"), CREATE_FILE_SELECTION_TO_SELECT_UPDATE_SOFT );
}
void NextStepBeforeSendingUpdateSoftToTarget( void )
{
	if ( OpenDialogMonitorConnect( ) )
	{
printf("*** HERE SET REMOTE_FILE_TRANSFER ---SOFT_UPDATE--- TO SEND!\n");
		InfosGUI->TargetMonitor.TransferFileNum = 1;
		InfosGUI->TargetMonitor.TransferFileIsSend = TRUE;
		TransferMasterStart( );
	}
}
#endif

void MenuViewRegisterBlockContent( void )
{
	int RegisterSelect = -1;
	char RegisterString[ 10 ];
	RegisterString[ 0 ] = '\0';
	if ( ShowEnterMessageBox( _("Register content"),_("Select register number to view"), RegisterString, 10 ) )
	{
printf("SELECTION STRING = %s\n", RegisterString);
		RegisterSelect= atoi( RegisterString );
		if ( RegisterSelect>=0 && RegisterSelect<NBR_REGISTERS )
		{
			if ( InfosGUI->TargetMonitor.RemoteConnected )
			{
printf("*** HERE SET REMOTE_FILE_TRANSFER ---REGISTER CONTENT--- TO RECEIVE!\n");
				InfosGUI->TargetMonitor.TransferFileNum = 50; // register number is stored in the 'sub-num'...
				InfosGUI->TargetMonitor.TransferFileSubNum = RegisterSelect;
				InfosGUI->TargetMonitor.TransferFileIsSend = FALSE;
				TransferMasterStart( );
			}
			else
			{
				char RegisterContentFileName[ 400 ];
				GError *error = NULL;
				sprintf( RegisterContentFileName, "%s/register_block_content.csv", TmpDirectoryRoot );
				SaveRegisterFunctionBlockContent( RegisterSelect, RegisterContentFileName, FALSE/*Compressed*/ );
				sprintf( RegisterContentFileName, "file://%s/register_block_content.csv", TmpDirectoryRoot );
printf("REGISTER FILE FOR SHOW_URI = %s\n", RegisterContentFileName);
				gtk_show_uri( NULL, RegisterContentFileName, GDK_CURRENT_TIME, &error);
//printf("TEST SHOW_URI for Windows...\n");
//gtk_show_uri( NULL, "file://C:\\essai.txt", GDK_CURRENT_TIME, &error);
			}
		}
		else
		{
			ShowMessageBox( _("Register selection error"), _("This register is not defined..."), _("Ok") );
		}
	}
}

void MenuViewLinuxSysLogDebug( void )
{
	if ( InfosGUI->TargetMonitor.RemoteConnected )
	{
printf("*** HERE SET REMOTE_FILE_TRANSFER ---LINUX SYSLOG DEBUG--- TO RECEIVE!\n");
		InfosGUI->TargetMonitor.TransferFileNum = 1000;
		InfosGUI->TargetMonitor.TransferFileIsSend = FALSE;
		TransferMasterStart( );
	}
	else
	{
		GError *error = NULL;
//		gtk_show_uri( NULL, "file:///var/log/debug", GDK_CURRENT_TIME, &error);
		gtk_show_uri( NULL, "file:///tmp/debug.txt", GDK_CURRENT_TIME, &error);
	}
}
