/* Classic Ladder Project */
/* Copyright (C) 2001-2015 Marc Le Douarain */
/* http://www.sourceforge.net/projects/classicladder */
/* http://sites.google.com/site/classicladder */
/* August 2002 */
/* -------------------------- */
/* Sections manager (GTK part)*/
/* -------------------------- */
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
#include "classicladder.h"
#include "global.h"
#include "manager.h"
#include "classicladder_gtk.h"
#include "manager_gtk.h"
#include "edit_gtk.h"
#include "menu_and_toolbar_gtk.h"
#include "preferences.h"

GtkWidget *ManagerWindow;

GtkWidget *SectionsList;
GtkWidget * ToolBarWidget;

GtkWidget *AddSectionWindow;
GtkWidget * EditName;
GtkWidget * CycleLanguage;
GtkWidget * CycleSubRoutineNbr;

char * pNameSectionSelected;
int RowSectionSelected;

char ModifyNotAdding = FALSE;

void ManagerDisplaySections( )
{
	StrSection * pSection;
	int NumSec;
	char * RowList[ ] = {"---", "---", "---", "---" };
	char BufferForSRx[ 10 ];
	int OneSectionExists = FALSE;
char buffer_debug[ 50 ];
	pNameSectionSelected = NULL;
	gtk_clist_clear( GTK_CLIST(SectionsList) );
	for ( NumSec=0; NumSec<NBR_SECTIONS; NumSec++ )
	{
		pSection = &SectionArray[ NumSec ];
		if ( pSection->Used )
		{
			RowList[ 0 ] = pSection->Name;
			if ( pSection->Language == SECTION_IN_LADDER )
			{
				RowList[ 1 ] = "Ladder";
				RowList[ 2 ] = "Main";
			}
			if ( pSection->Language == SECTION_IN_SEQUENTIAL )
			{
				RowList[ 1 ] = "Sequential";
				RowList[ 2 ] = "---";
			}
			if ( pSection->SubRoutineNumber>=0 )
			{
				sprintf( BufferForSRx, "SR%d", pSection->SubRoutineNumber );
				RowList[ 2 ] = BufferForSRx;
			}
sprintf( buffer_debug, "F=%d, L=%d, P=%d", pSection->FirstRung, pSection->LastRung, pSection->SequentialPage );
RowList[ 3 ] = buffer_debug;
			gtk_clist_append( GTK_CLIST(SectionsList), RowList );
			OneSectionExists = TRUE;
		}
	}
	if ( OneSectionExists )
		gtk_clist_select_row( GTK_CLIST(SectionsList), 0, 0 );
}

void SelectRowSignal( GtkCList *clist, gint row, gint column, GdkEventButton *event, gpointer user_data)
{
	if ( gtk_clist_get_text( GTK_CLIST(SectionsList), row, 0, (gchar **)&pNameSectionSelected ) )
	{
		RowSectionSelected = row;
		SectionSelected( pNameSectionSelected );
		EditorButtonsAccordingSectionType( );
		EnableDisableMenusAccordingSectionType( );
		UpdateVScrollBar( TRUE/*AutoSelectCurrentRung*/ );
	}
}

//added by Heli for search
void ChangeSectionSelectedInManager( int SectionToSelect )
{
	int Idx;
	StrSection * pSection = &SectionArray[ SectionToSelect ];

	for (Idx=0; Idx<NBR_SECTIONS; Idx++)
	{
		if ( gtk_clist_get_text(GTK_CLIST(SectionsList), Idx, 0, (gchar **)&pNameSectionSelected ) )
		{
			if (strcmp (pNameSectionSelected, pSection->Name)==0)
			{
				gtk_clist_select_row( GTK_CLIST(SectionsList), Idx, 0 ); // will do a signal like 'user click' !
				return;
			}
		}
	}
}
void SelectNextSectionInManager(void)
{
	if (gtk_clist_get_text(GTK_CLIST(SectionsList), RowSectionSelected+1, 0, (gchar **)&pNameSectionSelected ) )
	{
		RowSectionSelected++;
		gtk_clist_select_row( GTK_CLIST(SectionsList), RowSectionSelected, 0 ); // will do a signal like 'user click' !
	}
}

void SelectPreviousSectionInManager(void)
{
	if ( gtk_clist_get_text(GTK_CLIST(SectionsList), RowSectionSelected-1, 0, (gchar **)&pNameSectionSelected ) )
	{
		RowSectionSelected--;
		gtk_clist_select_row( GTK_CLIST(SectionsList), RowSectionSelected, 0 ); // will do a signal like 'user click' !
	}
}

void ButtonAddSectionDoneClickSignal( )
{
	char * pSectionNameEntered = (char *)gtk_entry_get_text( GTK_ENTRY(EditName) );
	// verify if name already exists...
	if ( VerifyIfSectionNameAlreadyExist( pSectionNameEntered ) )
	{
		ShowMessageBox( "Error", "This section name already exists or is incorrect !!!", "Ok" );
	}
	else
	{
		if ( ModifyNotAdding )
		{
			ModifySectionProperties( pNameSectionSelected, pSectionNameEntered );
			gtk_widget_hide( AddSectionWindow );
			ManagerDisplaySections( );
		}
		else
		{
//			char SubNbrValue[ 10 ];
			int SubNbr = -1;
//			char BuffLanguage[ 30 ];
			int Language = SECTION_IN_LADDER;
			// get language type
//			strcpy( BuffLanguage , (char *)gtk_entry_get_text((GtkEntry *)((GtkCombo *)CycleLanguage)->entry) );
//			if ( strcmp( BuffLanguage, "Sequential" )==0 )
//				Language = SECTION_IN_SEQUENTIAL;
			Language = gtk_combo_box_get_active( GTK_COMBO_BOX( CycleLanguage ) );
			// get if main or sub-routine (and which number if sub, used in the 'C'all coils)
//			strcpy( SubNbrValue , (char *)gtk_entry_get_text((GtkEntry *)((GtkCombo *)CycleSubRoutineNbr)->entry) );
//			if ( SubNbrValue[ 0 ]=='S' && SubNbrValue[ 1 ]=='R' )
//				SubNbr = atoi( &SubNbrValue[2] );
			SubNbr = gtk_combo_box_get_active( GTK_COMBO_BOX( CycleSubRoutineNbr ) )-1;
		
			if ( SubNbr>=0 && VerifyIfSubRoutineNumberExist( SubNbr ))
			{
				ShowMessageBox( "Error", "This sub-routine number for calls is already defined !!!", "Ok" );
			}
			else
			{
				// create the new section
				if ( !AddSection( pSectionNameEntered , Language , SubNbr ) )
					ShowMessageBox( "Error", "Failed to add a new section. Full?", "Ok" );
				gtk_widget_hide( AddSectionWindow );
				ManagerDisplaySections( );
			}
		}
	}
}
void ButtonAddClickSignal( )
{
	// we open the requester to add a new section...
	gtk_entry_set_text( GTK_ENTRY(EditName), "" );
	gtk_combo_box_set_active( GTK_COMBO_BOX( CycleLanguage ), 0 );
	gtk_widget_set_sensitive( CycleLanguage, TRUE );
	gtk_combo_box_set_active( GTK_COMBO_BOX( CycleSubRoutineNbr ), 0 );
	gtk_widget_set_sensitive( CycleSubRoutineNbr, TRUE );
	gtk_widget_grab_focus( EditName );
	gtk_window_set_title( GTK_WINDOW(AddSectionWindow), "Add a new section...");
	gtk_widget_show( AddSectionWindow );
	ModifyNotAdding = FALSE;
}
void ButtonPropertiesClickSignal( )
{
	// we open the requester to modify some properties of the current section...
	gtk_entry_set_text( GTK_ENTRY(EditName), pNameSectionSelected );
	int NumSec = SearchSectionWithName( pNameSectionSelected );
	if ( NumSec>=0 )
	{
//		char Buff[10];
		StrSection * pSection = &SectionArray[ NumSec ];
//		gtk_entry_set_text((GtkEntry*)((GtkCombo *)CycleLanguage)->entry,pSection->Language== SECTION_IN_LADDER?"Ladder":"Sequential");
		gtk_combo_box_set_active( GTK_COMBO_BOX( CycleLanguage ), pSection->Language );
//		strcpy( Buff, "Main");
//		if ( pSection->SubRoutineNumber>=0 )
//			sprintf( Buff, "SR%d", pSection->SubRoutineNumber );
//		gtk_entry_set_text((GtkEntry*)((GtkCombo *)CycleSubRoutineNbr)->entry,Buff);
		gtk_combo_box_set_active( GTK_COMBO_BOX( CycleSubRoutineNbr ), pSection->SubRoutineNumber+1 );
	}
	gtk_widget_set_sensitive( CycleLanguage, FALSE );
	gtk_widget_set_sensitive( CycleSubRoutineNbr, FALSE );
	gtk_widget_grab_focus( EditName );
	gtk_window_set_title( GTK_WINDOW(AddSectionWindow), "Modify current section");
	gtk_widget_show( AddSectionWindow );
	ModifyNotAdding = TRUE;
}
gint AddSectionWindowDeleteEvent( GtkWidget * widget, GdkEvent * event, gpointer data )
{
	// we just want to hide the window
	gtk_widget_hide( AddSectionWindow );
	// we do not want that the window be destroyed.
	return TRUE;
}

void DeleteCurrentSection( )
{
	DelSection( pNameSectionSelected );
	ManagerDisplaySections( );
}

void ButtonDelClickSignal( )
{
	if (pNameSectionSelected )
	{
		if ( NbrSectionsDefined( )>1 )
		{
			ShowConfirmationBox("New","Do you really want to delete the section ?", DeleteCurrentSection);	
		}
		else
		{
			ShowMessageBox( "Error", "You can not delete the last section...", "Ok" );
		}
	}
}

void ButtonMoveUpClickSignal( )
{
	char *pNameSectionToSwapWith;
	if ( RowSectionSelected>0 )
	{
		if ( gtk_clist_get_text( GTK_CLIST(SectionsList), RowSectionSelected-1, 0, (gchar **)&pNameSectionToSwapWith ) )
		{
			SwapSections( pNameSectionSelected, pNameSectionToSwapWith );
		}
	}
	else
	{
		ShowMessageBox( "Error", "This section is already executed the first !", "Ok" );
	}
	ManagerDisplaySections( );
}
void ButtonMoveDownClickSignal( )
{
	char *pNameSectionToSwapWith;
//	if ( RowSectionSelected<   )
	{
		if ( gtk_clist_get_text( GTK_CLIST(SectionsList), RowSectionSelected+1, 0, (gchar **)&pNameSectionToSwapWith ) )
		{
			SwapSections( pNameSectionSelected, pNameSectionToSwapWith );
		}
	}
//	else
//	{
//		ShowMessageBox( "Error", "This section is already executed the last !", "Ok" );
//	}
	ManagerDisplaySections( );
}

gint ManagerWindowDeleteEvent( GtkWidget * widget, GdkEvent * event, gpointer data )
{
// Here, we must only toggle the menu check that will call itself the function below to close the window ...
//	gtk_widget_hide( ManagerWindow );
	SetToogleMenuForSectionsManagerWindow( FALSE/*OpenedWin*/ );
	// we do not want that the window be destroyed.
	return TRUE;
}
// called per toggle action menu, or at startup (if window saved open or not)...
void OpenManagerWindow( GtkAction * ActionOpen, gboolean OpenIt )
{
	if ( ActionOpen!=NULL )
		OpenIt = gtk_toggle_action_get_active( GTK_TOGGLE_ACTION(ActionOpen) );
	if ( OpenIt )
	{
		RestoreWindowPosiPrefs( "Manager", ManagerWindow );
		gtk_widget_show( ManagerWindow );
		gtk_window_present( GTK_WINDOW(ManagerWindow) );
	}
	else
	{
		RememberWindowPosiPrefs( "Manager", ManagerWindow, FALSE/*SaveWindowSize*/ );
		gtk_widget_hide( ManagerWindow );
	}
}
void RememberManagerWindowPrefs( void )
{
//ForGTK3	char WindowIsOpened = GTK_WIDGET_VISIBLE( GTK_WINDOW(ManagerWindow) );
	char WindowIsOpened = MY_GTK_WIDGET_VISIBLE( ManagerWindow );
//TODO: when project opened (and we see that project have many sections), open this window !!! so for now just always opened per default...
//	RememberWindowOpenPrefs( "Manager", WindowIsOpened );
	if ( WindowIsOpened )
		RememberWindowPosiPrefs( "Manager", ManagerWindow, FALSE/*SaveWindowSize*/ );
}
void OpenManagerWindowFromPopup( void )
{
	SetToogleMenuForSectionsManagerWindow( TRUE );
	gtk_window_present( GTK_WINDOW(ManagerWindow) );
}
void ManagerEnableActionsSectionsList( char cState )
{
	gtk_widget_set_sensitive( SectionsList, cState );
	gtk_widget_set_sensitive( ToolBarWidget , cState );
}

void AddSectionWindowInit( )
{
	GtkWidget *vbox;
	GtkWidget * ButtonOk;
	GtkWidget * hbox[ 3 ];
	GtkWidget * Lbl[ 3 ];
//	GList *LangageItems = NULL;
//	GList *SubRoutinesNbrItems = NULL;
	int NumSub;
	char BuffNumSub[ 10 ];
//	char * ArrayNumSub[ ] = { "SR0", "SR1", "SR2", "SR3", "SR4", "SR5", "SR6", "SR7", "SR8", "SR9" };
	int Line;
//	LangageItems = g_list_append( LangageItems, "Ladder" );
//#ifdef SEQUENTIAL_SUPPORT
//	LangageItems = g_list_append( LangageItems, "Sequential" );
//#endif

//	SubRoutinesNbrItems = g_list_append( SubRoutinesNbrItems, "Main" );
//	for ( NumSub=0; NumSub<10; NumSub++ )
//	{
//		SubRoutinesNbrItems = g_list_append( SubRoutinesNbrItems, ArrayNumSub[ NumSub ] );
//	}

	AddSectionWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);

	vbox = gtk_vbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER (AddSectionWindow), vbox);
	gtk_widget_show (vbox);

	for ( Line = 0; Line<3; Line++ )
	{
		char * text;
		hbox[ Line ] = gtk_hbox_new (FALSE, 0);
		gtk_container_add (GTK_CONTAINER (vbox), hbox[ Line ]);
		gtk_widget_show (hbox[ Line ]);

		switch( Line )
		{
			case 1 : text = "Language"; break;
			case 2 : text = "Main/Sub-Routine"; break;
			default: text = "Name"; break;
		}
		Lbl[ Line ] = gtk_label_new( text );
		gtk_box_pack_start (GTK_BOX (hbox[ Line ]), Lbl[ Line ], FALSE, FALSE, 0);
		gtk_widget_show ( Lbl[ Line ]);

		switch( Line )
		{
			case 0:
				EditName = gtk_entry_new();
				gtk_entry_set_max_length( GTK_ENTRY(EditName),LGT_SECTION_NAME-1 );
				gtk_box_pack_start( GTK_BOX (hbox[Line]), EditName, TRUE, TRUE, 0 );
				gtk_widget_show( EditName );
				break;
			case 1:
//				CycleLanguage = gtk_combo_new();
//				gtk_combo_set_value_in_list(GTK_COMBO(CycleLanguage), TRUE /*val*/, FALSE /*ok_if_empty*/);
//				gtk_combo_set_popdown_strings(GTK_COMBO(CycleLanguage),LangageItems);
				CycleLanguage = gtk_combo_box_new_text( );
				gtk_combo_box_append_text( GTK_COMBO_BOX(CycleLanguage), "Ladder" );
#ifdef SEQUENTIAL_SUPPORT
				gtk_combo_box_append_text( GTK_COMBO_BOX(CycleLanguage), "Sequential" );
#endif
				gtk_box_pack_start( GTK_BOX (hbox[Line]), CycleLanguage, TRUE, TRUE, 0 );
				gtk_widget_show( CycleLanguage );
				break;
			case 2:
//				CycleSubRoutineNbr = gtk_combo_new();
//				gtk_combo_set_value_in_list(GTK_COMBO(CycleSubRoutineNbr), TRUE /*val*/, FALSE /*ok_if_empty*/);
//				gtk_combo_set_popdown_strings(GTK_COMBO(CycleSubRoutineNbr),SubRoutinesNbrItems);
				CycleSubRoutineNbr = gtk_combo_box_new_text( );
				gtk_combo_box_append_text( GTK_COMBO_BOX(CycleSubRoutineNbr), "Main" );
				for ( NumSub=0; NumSub<10; NumSub++ )
				{
					sprintf( BuffNumSub, "SR%d", NumSub );
					gtk_combo_box_append_text( GTK_COMBO_BOX(CycleSubRoutineNbr), BuffNumSub );
				}
				gtk_box_pack_start( GTK_BOX (hbox[Line]), CycleSubRoutineNbr, TRUE, TRUE, 0 );
				gtk_widget_show( CycleSubRoutineNbr );
				break;
		}
	}

	ButtonOk = gtk_button_new_with_label("Ok");
	gtk_box_pack_start (GTK_BOX (vbox), ButtonOk, TRUE, FALSE, 0);
	gtk_signal_connect(GTK_OBJECT (ButtonOk), "clicked",
		GTK_SIGNAL_FUNC(ButtonAddSectionDoneClickSignal), 0);
	gtk_widget_show (ButtonOk);
	gtk_window_set_modal(GTK_WINDOW(AddSectionWindow),TRUE);
	gtk_window_set_position(GTK_WINDOW(AddSectionWindow),GTK_WIN_POS_CENTER);
	gtk_signal_connect( GTK_OBJECT(AddSectionWindow), "delete_event",
		GTK_SIGNAL_FUNC(AddSectionWindowDeleteEvent), 0 );
}

GtkUIManager * ManageruiManager;

static GtkActionEntry ManagerEntriesArray[ ] =
{	{ "AddSection", GTK_STOCK_ADD, "Add section",  NULL, "Add Section", G_CALLBACK( ButtonAddClickSignal ) },
	{ "DelSection", GTK_STOCK_DELETE, "Delete section",  NULL, "Delete Section", G_CALLBACK( ButtonDelClickSignal ) },
	{ "MoveUpSection", GTK_STOCK_GO_UP,  "Move up",  NULL, "Move up", G_CALLBACK( ButtonMoveUpClickSignal ) },
	{ "MoveDownSection", GTK_STOCK_GO_DOWN, "Move down",  NULL, "Move down", G_CALLBACK( ButtonMoveDownClickSignal ) },
	{ "PropertiesSection", GTK_STOCK_PROPERTIES, "Properties", NULL, "Properties", G_CALLBACK( ButtonPropertiesClickSignal ) } 
};
static const gchar *Manager_ui_strings = 
"<ui>"
"  <toolbar name='ToolBar'>"
"		<toolitem action='AddSection' />"
"		<toolitem action='DelSection' />"
"		<separator />"
"		<toolitem action='MoveUpSection' />"
"		<toolitem action='MoveDownSection' />"
"		<separator />"
"		<toolitem action='PropertiesSection' />"
"	</toolbar>"
"</ui>";

void ManagerInitGtk()
{
	GtkWidget *vbox;
//	GtkWidget *hbox;
	char * List[ ] = {"Section Name   ", "Language    ", "Type   ", "debug" };

	pNameSectionSelected = NULL;

	ManagerWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title ((GtkWindow *)ManagerWindow, "Sections Manager");

	vbox = gtk_vbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER (ManagerWindow), vbox);
	gtk_widget_show (vbox);

	GError *error = NULL;
	GtkActionGroup * MgrActionGroup;
	ManageruiManager = gtk_ui_manager_new( );
	
	MgrActionGroup = gtk_action_group_new( "ManagerActionGroup" );
	gtk_action_group_add_actions( MgrActionGroup, ManagerEntriesArray, G_N_ELEMENTS( ManagerEntriesArray ), NULL );
	
	gtk_ui_manager_insert_action_group( ManageruiManager, MgrActionGroup, 0 );
	if (!gtk_ui_manager_add_ui_from_string( ManageruiManager, Manager_ui_strings, -1/*length*/, &error ))
	{
		g_message ("Manager failed to build gtk menus: %s", error->message);
		g_error_free (error);
	}
	ToolBarWidget = gtk_ui_manager_get_widget( ManageruiManager, "/ToolBar" );
//		gtk_toolbar_set_style( GTK_TOOLBAR(ToolBarWidget), GTK_TOOLBAR_ICONS );
	gtk_toolbar_set_style( GTK_TOOLBAR(ToolBarWidget), GTK_TOOLBAR_BOTH );
	gtk_box_pack_start( GTK_BOX(vbox), ToolBarWidget, FALSE, FALSE, 0 );
gtk_widget_set_usize((GtkWidget *)ToolBarWidget,475/*ToSeeAllButtons! 367*/,0);

	SectionsList = gtk_clist_new_with_titles( /*3*/ 4, List );
	gtk_box_pack_start (GTK_BOX(vbox), SectionsList, TRUE, TRUE, 0);
	gtk_signal_connect(GTK_OBJECT (SectionsList), "select-row",
		GTK_SIGNAL_FUNC(SelectRowSignal), 0);
	gtk_widget_show( SectionsList );

//v0.9.20	ManagerDisplaySections( );
	gtk_signal_connect( GTK_OBJECT(ManagerWindow), "delete_event",
		GTK_SIGNAL_FUNC(ManagerWindowDeleteEvent), 0 );
	gtk_window_set_icon_name (GTK_WINDOW( ManagerWindow ), GTK_STOCK_DND_MULTIPLE);
RestoreWindowPosiPrefs( "Manager", ManagerWindow );
gtk_widget_show (ManagerWindow);

	AddSectionWindowInit( );
}

