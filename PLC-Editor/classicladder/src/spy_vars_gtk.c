/* Classic Ladder Project */
/* Copyright (C) 2001-2018 Marc Le Douarain */
/* http://www.sourceforge.net/projects/classicladder */
/* http://sites.google.com/site/classicladder */
/* February 2001 */
/* -------------------------------------------------------------------------------------------- */
/* Spy variables windows (booleans with checkboxes, and any with entry widgets) - GTK interface */
/* + Modify current value of a spy variable + Set/Unset booleans inputs/outputs                 */
/* -------------------------------------------------------------------------------------------- */
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include <libintl.h> // i18n
#include <locale.h> // i18n

#include "classicladder.h"
#include "global.h"
#include "vars_access.h"
#include "drawing.h"
#include "edit.h"
#include "classicladder_gtk.h"
#include "vars_names.h"
#include "symbols.h"
#include "menu_and_toolbar_gtk.h"
#include "preferences.h"

#include "spy_vars_gtk.h"
#include "../icons/IconWindowSpyBoolVars.h"
#include "../icons/IconWindowSpyFreeVars.h"
#include "../icons/IconOutputOff.h"
#include "../icons/IconOutputOn.h"

//#define NBR_BOOLS_VAR_SPY 15
//#define NBR_TYPE_BOOLS_SPY 3
//#define NBR_FREE_VAR_SPY 10

GtkWidget *SpyBoolVarsWindow;
GtkWidget *SpyFreeVarsWindow;
GtkWidget *ModifyVarValueWindow;

GtkWidget *offsetboolvar[ NBR_TYPE_BOOLS_SPY ];
//new InfosGUI! int ValOffsetBoolVar[ NBR_TYPE_BOOLS_SPY ] = { 0, 0, 0 };
GtkWidget *CheckBoolVar[ NBR_TYPE_BOOLS_SPY-1 ][ NBR_BOOLS_VAR_SPY ];
GtkWidget * ButSetBoolVar[ NBR_TYPE_BOOLS_SPY-1 ][ NBR_BOOLS_VAR_SPY ];
GtkWidget * LabelBoolOuput[ NBR_BOOLS_VAR_SPY ];
GtkWidget * ImageBoolOutputOffOn[ NBR_BOOLS_VAR_SPY ];
//ForGTK3, deprecated... GtkTooltips * TooltipsBoolVar[ NBR_TYPE_BOOLS_SPY ][ NBR_BOOLS_VAR_SPY ];
GtkWidget *DisplaySymbolsForBoolVars;
// required to be able to use gtk_widget_modify_bg() else not working with widgets gtk_check_button/gtk_label (without background) !
GtkWidget *EventBoxInpOutVarSetted[ NBR_TYPE_BOOLS_SPY-1 ][ NBR_BOOLS_VAR_SPY ];
// if gtk_widget_modify_bg() called each time, seems to take a lot of CPU time...
// so only done when changing!
char OptimVarSettedMemory[ NBR_TYPE_BOOLS_SPY-1 ][ NBR_BOOLS_VAR_SPY ];
// optimization to refresh outputs images only when changed
char OptimBoolOutputStateMemory[ NBR_BOOLS_VAR_SPY ];
GdkPixbuf *PixBufOutputOff;
GdkPixbuf *PixBufOutputOn;


GtkWidget *SetUnsetDialog;
int TypeSetUnsetVar = -1;
int OffsetSetUnsetVar = -1;

GtkWidget * EntryDateTime;
enum {VAR_SPY_MODE_DEC_SIGNED, VAR_SPY_MODE_DEC_UNSIGNED, VAR_SPY_MODE_HEX, VAR_SPY_MODE_BIN, VAR_SPY_MODE_END};
GtkWidget * EntryVarSpy[NBR_FREE_VAR_SPY*2];
//ForGTK3, deprecated... GtkTooltips * TooltipsEntryVarSpy[ NBR_FREE_VAR_SPY ];
/* defaults vars to spy list */
//new InfosGUI! int VarSpy[NBR_FREE_VAR_SPY][2] = { {VAR_MEM_WORD,0}, {VAR_MEM_WORD,1}, {VAR_MEM_WORD,2}, {VAR_MEM_WORD,3}, {VAR_MEM_WORD,4}, {VAR_MEM_WORD,5}, {VAR_MEM_WORD,6}, {VAR_MEM_WORD,7}, {VAR_MEM_WORD,8}, {VAR_MEM_WORD,9} };
GtkWidget * DisplayFormatVarSpy[NBR_FREE_VAR_SPY];
//GtkWidget * ModifyVarSpy[NBR_FREE_VAR_SPY];
GtkWidget * EditVarSpyValue[NBR_FREE_VAR_SPY];
int LatestEditedVarValue[ NBR_FREE_VAR_SPY ];

GtkWidget * ModifyVariableNameEdit;
GtkWidget * ModifyVariableValueEdit;
int CurrentModifyVarType, CurrentModifyVarOffset;
int SaveModifyVarPosX = -1;
int SaveModifyVarPosY = -1;

#define NBR_TARGET_INFOS 5
GtkWidget * TargetInfoEntry[ NBR_TARGET_INFOS ];
#define NBR_PROJECT_INFOS 8
GtkWidget * ProjectInfoEntry[ NBR_PROJECT_INFOS ];

void DoFunctionSetOrUnsetVar( GtkWidget *widget, void * data_action )
{
	long DataAction = (int)data_action;
	gtk_widget_destroy(SetUnsetDialog);
	if ( InfosGUI->TargetMonitor.RemoteConnected )
	{
		InfosGUI->TargetMonitor.AskTargetToSetOrUnsetVarType = TypeSetUnsetVar;
		InfosGUI->TargetMonitor.AskTargetToSetOrUnsetVarNum = OffsetSetUnsetVar;
		InfosGUI->TargetMonitor.AskTargetToSetOrUnsetVarValue = DataAction;
	}
	else
	{
		switch( DataAction )
		{
			case 2: SetVar( TypeSetUnsetVar, OffsetSetUnsetVar, 0 ); break;
			case 1: SetVar( TypeSetUnsetVar, OffsetSetUnsetVar, 1 ); break;
			case 0: UnsetVar( TypeSetUnsetVar, OffsetSetUnsetVar ); break;
		}
	}
}
void ShowAskBoxToSetOrUnset( )
{
	/* From the example in gtkdialog help */
	GtkWidget *label, *Set1Button, *Set0Button, *UnsetButton;
	GtkWidget *DialogContentArea;
	GtkWidget *DialogActionArea;
	/* Create the widgets */
	SetUnsetDialog = gtk_dialog_new();
	//ForGTK3
	DialogContentArea = gtk_dialog_get_content_area(GTK_DIALOG(SetUnsetDialog));
	DialogActionArea = gtk_dialog_get_action_area(GTK_DIALOG(SetUnsetDialog));
	label = gtk_label_new ( CreateVarName(TypeSetUnsetVar,OffsetSetUnsetVar,Preferences.DisplaySymbolsInMainWindow/*InfosGene->DisplaySymbols*/) );
	Set1Button = gtk_button_new_with_label( _("Set to 1") );
	Set0Button = gtk_button_new_with_label( _("Set to 0") );
	UnsetButton = gtk_button_new_with_label( _("UnSet") );
	/* Ensure that the dialog box is destroyed when the user clicks ok. */
//	gtk_signal_connect_object (GTK_OBJECT (no_button), "clicked",
//							GTK_SIGNAL_FUNC (gtk_widget_destroy), GTK_OBJECT(SetUnsetDialog));
	gtk_signal_connect(GTK_OBJECT (Set1Button), "clicked",
							GTK_SIGNAL_FUNC (DoFunctionSetOrUnsetVar), (void *)1);
//ForGTK3	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(SetUnsetDialog)->action_area), Set1Button);
	gtk_container_add (GTK_CONTAINER(DialogActionArea), Set1Button);
	gtk_signal_connect(GTK_OBJECT (Set0Button), "clicked",
							GTK_SIGNAL_FUNC (DoFunctionSetOrUnsetVar), (void *)2);
//ForGTK3	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(SetUnsetDialog)->action_area), Set0Button);
	gtk_container_add (GTK_CONTAINER(DialogActionArea), Set0Button);
	gtk_signal_connect(GTK_OBJECT (UnsetButton), "clicked",
							GTK_SIGNAL_FUNC (DoFunctionSetOrUnsetVar), (void *)0);
//ForGTK3	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(SetUnsetDialog)->action_area), UnsetButton);
	gtk_container_add (GTK_CONTAINER(DialogActionArea), UnsetButton);
	/* Add the label, and show everything we've added to the dialog. */
//ForGTK3	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(SetUnsetDialog)->vbox), label);
	gtk_container_add (GTK_CONTAINER(DialogContentArea), label);
	gtk_widget_grab_focus(UnsetButton);
	gtk_window_set_modal(GTK_WINDOW(SetUnsetDialog),TRUE);
	gtk_window_set_title(GTK_WINDOW(SetUnsetDialog), _("Set/UnSet variable") );
	gtk_window_set_position(GTK_WINDOW(SetUnsetDialog),GTK_WIN_POS_CENTER);
	gtk_widget_show_all (SetUnsetDialog);
}

static gint chkvar_press_event( GtkWidget * widget, void * numcheck )
{
	long NumCheckWidget = (long)numcheck;
	int Type = VAR_MEM_BIT;
	int Offset = InfosGUI->ValOffsetBoolVar[ 0 ];
	int NumCheck = NumCheckWidget;
	if( NumCheckWidget>=NBR_BOOLS_VAR_SPY && NumCheckWidget<2*NBR_BOOLS_VAR_SPY )
	{
		Type = VAR_PHYS_INPUT;
		Offset = InfosGUI->ValOffsetBoolVar[ 1 ];
		NumCheck -= NBR_BOOLS_VAR_SPY;
	} 
	if( NumCheckWidget>=2*NBR_BOOLS_VAR_SPY && NumCheckWidget<3*NBR_BOOLS_VAR_SPY )
	{
		Type = VAR_PHYS_OUTPUT;
		Offset = InfosGUI->ValOffsetBoolVar[ 2 ];
		NumCheck -= 2*NBR_BOOLS_VAR_SPY;
	} 
	if (gtk_toggle_button_get_active((GtkToggleButton *)widget))
	{
//Using InputStateForLogic at -1 now...		if ( Type==VAR_PHYS_INPUT )
//			InputFilterAndStateArray[ Offset+NumCheck ].InputStateForLogic = 1;
//		else
//TODO: test, if not -1, do not do anything?!
			WriteVar(Type,Offset+NumCheck,1);
//printf("check var=%d/%d to on\n",Type,Offset+NumCheck);
	}
	else
	{
//Using InputStateForLogic at -1 now...		if ( Type==VAR_PHYS_INPUT )
//			InputFilterAndStateArray[ Offset+NumCheck ].InputStateForLogic = 0;
//		else
//TODO: test, if not -1, do not do anything?!
			WriteVar(Type,Offset+NumCheck,0);
//printf("check var=%d/%d to off\n",Type,Offset+NumCheck);
	}
	return TRUE;
}
static gint buttoninpout_clicked_event( GtkWidget * widget, void * numcheck )
{
	long NumCheckWidget = (long)numcheck;
	OffsetSetUnsetVar = NumCheckWidget;
	if( NumCheckWidget>=NBR_BOOLS_VAR_SPY && NumCheckWidget<2*NBR_BOOLS_VAR_SPY )
	{
		TypeSetUnsetVar = VAR_PHYS_INPUT;
		OffsetSetUnsetVar -= NBR_BOOLS_VAR_SPY;
		OffsetSetUnsetVar += InfosGUI->ValOffsetBoolVar[ 1 ];
	} 
	if( NumCheckWidget>=2*NBR_BOOLS_VAR_SPY && NumCheckWidget<3*NBR_BOOLS_VAR_SPY )
	{
		TypeSetUnsetVar = VAR_PHYS_OUTPUT;
		OffsetSetUnsetVar -= 2*NBR_BOOLS_VAR_SPY;
		OffsetSetUnsetVar += InfosGUI->ValOffsetBoolVar[ 2 ];
	}
	ShowAskBoxToSetOrUnset( );
	return TRUE;
}

/*void RefreshOneBoolVar( int Type, int Num, int Val )
{
	int Col = -1;
	switch( Type )
	{
		case VAR_MEM_BIT: Col = 0; break;
		case VAR_PHYS_INPUT: Col = 1; break;
		case VAR_PHYS_OUTPUT: Col = 2; break;
	}
	if ( Num>=InfosGUI->ValOffsetBoolVar[ Col ] && Num<InfosGUI->ValOffsetBoolVar[ Col ]+NBR_BOOLS_VAR_SPY )
		gtk_toggle_button_set_active((GtkToggleButton *)CheckBoolVar[Col][Num-InfosGUI->ValOffsetBoolVar[ Col ]],(Val!=0)?TRUE:FALSE);
}*/

void RefreshAllBoolsVars( )
{
	int NumVar;
	// background color used to show var that are setted...
	GdkColor color_red;
	gdk_color_parse( "lightblue", &color_red );
	for (NumVar=0; NumVar<NBR_BOOLS_VAR_SPY; NumVar++)
	{
		char SetVarState;
		char OutputState;
		// bools
		gtk_toggle_button_set_active( (GtkToggleButton *)CheckBoolVar[0][NumVar], ReadVar(VAR_MEM_BIT,NumVar+InfosGUI->ValOffsetBoolVar[ 0 ])?TRUE:FALSE );
		
		// inputs
		gtk_toggle_button_set_active( (GtkToggleButton *)CheckBoolVar[1][NumVar], ReadVar(VAR_PHYS_INPUT,NumVar+InfosGUI->ValOffsetBoolVar[ 1 ])?TRUE:FALSE );
		SetVarState = IsVarSet( VAR_PHYS_INPUT, NumVar+InfosGUI->ValOffsetBoolVar[ 1 ] );
		if ( SetVarState!=OptimVarSettedMemory[ 0 ][ NumVar ] )
		{
			gtk_widget_modify_bg( EventBoxInpOutVarSetted[ 0 ][ NumVar ], GTK_STATE_NORMAL, SetVarState?&color_red:NULL );
			OptimVarSettedMemory[ 0 ][ NumVar ] = SetVarState;
		}
		
		// outputs
		OutputState = ReadVar(VAR_PHYS_OUTPUT,NumVar+InfosGUI->ValOffsetBoolVar[ 2 ]);
		if ( OutputState!=OptimBoolOutputStateMemory[ NumVar ] )
		{
			gtk_image_set_from_pixbuf( GTK_IMAGE( ImageBoolOutputOffOn[ NumVar ] ), OutputState?PixBufOutputOn:PixBufOutputOff );
			OptimBoolOutputStateMemory[ NumVar ] = OutputState;
		}
		SetVarState = IsVarSet( VAR_PHYS_OUTPUT, NumVar+InfosGUI->ValOffsetBoolVar[ 2 ] );
		if ( SetVarState!=OptimVarSettedMemory[ 1 ][ NumVar ] )
		{
			gtk_widget_modify_bg( EventBoxInpOutVarSetted[ 1 ][ NumVar ], GTK_STATE_NORMAL, SetVarState?&color_red:NULL );
			OptimVarSettedMemory[ 1][ NumVar ] = SetVarState;
		}
	}
}

void UpdateAllLabelsBoolsVars( int OnlyThisColumn )
{
	int ColumnVar, OffVar;
	char DisplaySymbol = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON( DisplaySymbolsForBoolVars ) );
	for(ColumnVar=0; ColumnVar<NBR_TYPE_BOOLS_SPY; ColumnVar++)
	{
		char Buffer[20];
		if ( OnlyThisColumn==-1 || OnlyThisColumn==ColumnVar )
		{
			for(OffVar=0; OffVar<NBR_BOOLS_VAR_SPY; OffVar++)
			{
				char * OtherVarName;
				switch( ColumnVar )
				{
					case 0: sprintf(Buffer, "%cB%d",'%', OffVar+InfosGUI->ValOffsetBoolVar[ ColumnVar ]); break;
					case 1: sprintf(Buffer, "%cI%d",'%', OffVar+InfosGUI->ValOffsetBoolVar[ ColumnVar ]); break;
					case 2: sprintf(Buffer, "%cQ%d",'%', OffVar+InfosGUI->ValOffsetBoolVar[ ColumnVar ]); break;
				}
				OtherVarName = ConvVarNameToSymbol( Buffer );
				if ( OtherVarName==NULL )
					OtherVarName = "";
				if ( ColumnVar==2 )
				{
					gtk_label_set_text(GTK_LABEL( LabelBoolOuput[ OffVar ] ), (DisplaySymbol && OtherVarName[0]!='\0')?OtherVarName:Buffer);
					gtk_widget_set_tooltip_text( LabelBoolOuput[ OffVar ], (DisplaySymbol && OtherVarName[0]!='\0')?Buffer:OtherVarName );
				}
				else
				{
//ForGTK3				gtk_label_set_text(GTK_LABEL(GTK_BIN( CheckBoolVar[ ColumnVar ][ OffVar ] )->child), (DisplaySymbol && OtherVarName[0]!='\0')?OtherVarName:Buffer);
					gtk_label_set_text(GTK_LABEL( gtk_bin_get_child( GTK_BIN( CheckBoolVar[ ColumnVar ][ OffVar ] ) ) ), (DisplaySymbol && OtherVarName[0]!='\0')?OtherVarName:Buffer);
//ForGTK3, deprecated...				gtk_tooltips_set_tip ( TooltipsBoolVar[ ColumnVar ][ OffVar ], CheckBoolVar[ ColumnVar ][ OffVar ], (DisplaySymbol && OtherVarName[0]!='\0')?Buffer:OtherVarName, NULL );
					gtk_widget_set_tooltip_text( CheckBoolVar[ ColumnVar ][ OffVar ], (DisplaySymbol && OtherVarName[0]!='\0')?Buffer:OtherVarName );
				}
			}
		}
		if ( OnlyThisColumn==-1 )
		{
			sprintf( Buffer, "%d", InfosGUI->ValOffsetBoolVar[ ColumnVar ] );
			gtk_entry_set_text( GTK_ENTRY(offsetboolvar[ ColumnVar ]), Buffer );
		}
	}
}

static gint OffsetBoolVar_activate_event( GtkWidget *widget, void * NumVarSpy )
{
	int Maxi = 0;
	long NumType = (long)NumVarSpy;
	int ValOffset = atoi( gtk_entry_get_text((GtkEntry *)widget) );
	switch( NumType )
	{
		case 0: Maxi = NBR_BITS; break;
		case 1: Maxi = NBR_PHYS_INPUTS; break;
		case 2: Maxi = NBR_PHYS_OUTPUTS; break;
	}
	if ( ValOffset+NBR_BOOLS_VAR_SPY>Maxi || ValOffset<0 )
		ValOffset = 0;
	InfosGUI->ValOffsetBoolVar[ NumType ] = ValOffset;
	UpdateAllLabelsBoolsVars( NumType/*OnlyThisColumn*/ );
	RefreshAllBoolsVars( );
	return TRUE;
}

static gint ChangeDisplaySymbolsForBoolVars_event( GtkWidget *widget, void * NotUsed )
{
	Preferences.DisplaySymbolsInBoolsVarsWindows = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON( DisplaySymbolsForBoolVars ) );
	UpdateAllLabelsBoolsVars( -1/*ALL Columns*/ );
	return TRUE;
}

gint BoolVarsWindowDeleteEvent( GtkWidget * widget, GdkEvent * event, gpointer data )
{
// Here, we must only toggle the menu check that will call itself the function below to close the window ...
//	gtk_widget_hide( SpyBoolVarsWindow );
	SetToggleMenuForBoolVarsWindow( FALSE/*OpenedWin*/ );
	// we do not want that the window be destroyed.
	return TRUE;
}

// called per toggle action menu, or at startup (if window saved open or not)...
void OpenSpyBoolVarsWindow( GtkAction * ActionOpen, gboolean OpenIt )
{
	if ( ActionOpen!=NULL )
		OpenIt = gtk_toggle_action_get_active( GTK_TOGGLE_ACTION(ActionOpen) );
	if ( OpenIt )
	{
		RestoreWindowPosiPrefs( "BoolVars", SpyBoolVarsWindow );
		gtk_widget_show( SpyBoolVarsWindow );
		gtk_window_present( GTK_WINDOW(SpyBoolVarsWindow) );
	}
	else
	{
		RememberWindowPosiPrefs( "BoolVars", SpyBoolVarsWindow, FALSE/*SaveWindowSize*/ );
		gtk_widget_hide( SpyBoolVarsWindow );
	}
}
void RememberBoolVarsWindowPrefs( void )
{
//ForGTK3	char WindowIsOpened = GTK_WIDGET_VISIBLE( GTK_WINDOW(SpyBoolVarsWindow) );
	char WindowIsOpened = MY_GTK_WIDGET_VISIBLE( SpyBoolVarsWindow );
		RememberWindowOpenPrefs( "BoolVars", WindowIsOpened );
	if ( WindowIsOpened )
		RememberWindowPosiPrefs( "BoolVars", SpyBoolVarsWindow, FALSE/*SaveWindowSize*/ );
}

void BoolVarsWindowInitGtk()
{
//	GtkWidget *vboxboolvars[ NBR_TYPE_BOOLS_SPY ],*vboxmain,*hboxvars;
//	GtkWidget *Hbox1;
	long NumCheckWidget,ColumnVar;
//ForGTK3, deprecated...	GtkTooltips * WidgetTooltips[ NBR_TYPE_BOOLS_SPY ];
	GtkWidget *hboxinpoutvars[ NBR_TYPE_BOOLS_SPY-1 ][ NBR_BOOLS_VAR_SPY ];
	GtkWidget *table;
//	GdkColor color_red;
//	gdk_color_parse( "red", &color_red );

	PixBufOutputOff = gdk_pixbuf_new_from_inline (-1, IconOutputOff, FALSE, NULL);
	PixBufOutputOn = gdk_pixbuf_new_from_inline (-1, IconOutputOn, FALSE, NULL);

	SpyBoolVarsWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title ( (GtkWindow *)SpyBoolVarsWindow, _("Spy bools vars") );
//	vboxmain = gtk_vbox_new (FALSE, 0);
//	gtk_container_add (GTK_CONTAINER (SpyBoolVarsWindow), vboxmain);
	table = gtk_table_new( NBR_TYPE_BOOLS_SPY, 2+NBR_BOOLS_VAR_SPY, FALSE );
	gtk_container_add (GTK_CONTAINER (SpyBoolVarsWindow), table);
	
//	Hbox1 = gtk_hbox_new (FALSE, 0);
//	gtk_container_add (GTK_CONTAINER (vboxmain), Hbox1);
	DisplaySymbolsForBoolVars = gtk_check_button_new_with_label( _("Display symbols") );
//	gtk_box_pack_start (GTK_BOX(Hbox1), DisplaySymbolsForBoolVars, FALSE, FALSE, 0);
	gtk_table_attach_defaults (GTK_TABLE (table), DisplaySymbolsForBoolVars, 0, NBR_TYPE_BOOLS_SPY, 0, 1 );
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( DisplaySymbolsForBoolVars ), Preferences.DisplaySymbolsInBoolsVarsWindows );
	gtk_signal_connect(GTK_OBJECT (DisplaySymbolsForBoolVars), "toggled",
					GTK_SIGNAL_FUNC(ChangeDisplaySymbolsForBoolVars_event), NULL );

//	hboxvars = gtk_hbox_new (FALSE, 0);
//	gtk_container_add (GTK_CONTAINER (vboxmain), hboxvars);

/*	for( ColumnVar=0; ColumnVar<NBR_TYPE_BOOLS_SPY; ColumnVar++ )
	{
		vboxboolvars[ ColumnVar ] = gtk_vbox_new (FALSE, 0);
		gtk_container_add (GTK_CONTAINER (hboxvars), vboxboolvars[ ColumnVar ]);
	}*/

	NumCheckWidget = 0;
	for(ColumnVar=0; ColumnVar<NBR_TYPE_BOOLS_SPY; ColumnVar++)
	{
		int OffVar;
		offsetboolvar[ ColumnVar ]  = gtk_entry_new();
//ForGTK3, deprecated...		WidgetTooltips[ ColumnVar ] = gtk_tooltips_new();
//ForGTK3, deprecated...		gtk_tooltips_set_tip ( WidgetTooltips[ ColumnVar ], offsetboolvar[ ColumnVar ], "Offset for vars displayed below (press return to apply)", NULL );
		gtk_widget_set_tooltip_text( offsetboolvar[ ColumnVar ], _("Offset for vars displayed below (press return to apply)") );
//GTK3		gtk_widget_set_usize((GtkWidget *)offsetboolvar[ ColumnVar ],40,0);
//////TEST!!!		gtk_widget_set_size_request( offsetboolvar[ ColumnVar ], 40, -1 );
//////gtk_widget_set_size_request( offsetboolvar[ ColumnVar ], 20, -1);
// Changes the size request of the entry to be about the right size for n_chars characters !
gtk_entry_set_width_chars( GTK_ENTRY(offsetboolvar[ ColumnVar ]), 4 );
//		gtk_box_pack_start (GTK_BOX(vboxboolvars[ ColumnVar ]),  offsetboolvar[ ColumnVar ] , FALSE, FALSE, 0);
		gtk_table_attach_defaults (GTK_TABLE (table), offsetboolvar[ ColumnVar ], ColumnVar, ColumnVar+1, 1, 2 );
		gtk_entry_set_text((GtkEntry *)offsetboolvar[ ColumnVar ],"0");
		gtk_signal_connect(GTK_OBJECT (offsetboolvar[ ColumnVar ]), "activate",
					GTK_SIGNAL_FUNC(OffsetBoolVar_activate_event), (void *)ColumnVar);
		
		for(OffVar=0; OffVar<NBR_BOOLS_VAR_SPY; OffVar++)
		{
			// for inputs & outputs columns, add "set" button
			if ( ColumnVar>=1 )
			{
				OptimVarSettedMemory[ ColumnVar-1 ][ OffVar ] = -1;
				hboxinpoutvars[ ColumnVar-1 ][ OffVar ] = gtk_hbox_new (FALSE, 0);
//				gtk_container_add (GTK_CONTAINER (vboxboolvars[ ColumnVar ]), hboxinpoutvars[ ColumnVar-1 ][ OffVar ]);

				ButSetBoolVar[ ColumnVar-1 ][ OffVar ] = gtk_button_new();
				gtk_button_set_image( GTK_BUTTON( ButSetBoolVar[ ColumnVar-1 ][ OffVar ] ), gtk_image_new_from_stock( GTK_STOCK_GOTO_LAST, /*GTK_ICON_SIZE_MENU*/GTK_ICON_SIZE_BUTTON ) );
				gtk_widget_set_tooltip_text( ButSetBoolVar[ ColumnVar-1 ][ OffVar ], (ColumnVar>=2)?(_("Set/Unset output")):(_("Set/Unset input")) );
				gtk_box_pack_start( GTK_BOX(hboxinpoutvars[ ColumnVar-1 ][ OffVar ]), ButSetBoolVar[ ColumnVar-1 ][ OffVar ], FALSE, FALSE, 0 );
				gtk_signal_connect( GTK_OBJECT(ButSetBoolVar[ ColumnVar-1 ][ OffVar ]), "clicked", GTK_SIGNAL_FUNC(buttoninpout_clicked_event), (void*)NumCheckWidget );

				// for outputs column, add "image"
				if ( ColumnVar==2 )
				{
					OptimBoolOutputStateMemory[ OffVar ] = -1;
					ImageBoolOutputOffOn[ OffVar ] = gtk_image_new_from_pixbuf( PixBufOutputOff );
					gtk_box_pack_start( GTK_BOX(hboxinpoutvars[ ColumnVar-1 ][ OffVar ]), ImageBoolOutputOffOn[ OffVar ], FALSE, FALSE, 0 );
				}

// add a gtk_event_box, so that gtk_widget_modify_bg() works with gtk_check_button/gtk_label widgets...
EventBoxInpOutVarSetted[ ColumnVar-1 ][ OffVar ] = gtk_event_box_new( );
//gtk_container_add (GTK_CONTAINER(EventBoxInpOutVarSetted[ ColumnVar-1 ][ OffVar ]), hboxinpoutvars[ ColumnVar-1 ][ OffVar ]);
//gtk_container_add (GTK_CONTAINER (vboxboolvars[ ColumnVar ]), EventBoxInpOutVarSetted[ ColumnVar-1 ][ OffVar ]);
gtk_box_pack_start( GTK_BOX(hboxinpoutvars[ ColumnVar-1 ][ OffVar ]), EventBoxInpOutVarSetted[ ColumnVar-1 ][ OffVar ], FALSE, FALSE, 0 );

			}
			if ( ColumnVar==2 )
			{
				LabelBoolOuput[ OffVar ] = gtk_label_new( "xxx" );
			}
			else
			{
				CheckBoolVar[ ColumnVar ][ OffVar ] = gtk_check_button_new_with_label("xxxx");
				gtk_signal_connect(GTK_OBJECT (CheckBoolVar[ ColumnVar ][ OffVar ]), "toggled",
					GTK_SIGNAL_FUNC(chkvar_press_event), (void*)NumCheckWidget);
			}
			if ( ColumnVar>=1 )
			{
				if ( ColumnVar==2 )
				{
//					gtk_box_pack_start (GTK_BOX(hboxinpoutvars[ ColumnVar-1 ][ OffVar ]), LabelBoolOuput[ OffVar ], FALSE, FALSE, 0);
					gtk_container_add( GTK_CONTAINER(EventBoxInpOutVarSetted[ ColumnVar-1 ][ OffVar ]), LabelBoolOuput[ OffVar ] );
				}
				else
				{
//					gtk_box_pack_start (GTK_BOX(hboxinpoutvars[ ColumnVar-1 ][ OffVar ]), CheckBoolVar[ ColumnVar ][ OffVar ], FALSE, FALSE, 0);
					gtk_container_add( GTK_CONTAINER(EventBoxInpOutVarSetted[ ColumnVar-1 ][ OffVar ]), CheckBoolVar[ ColumnVar ][ OffVar ] );
				}
gtk_table_attach_defaults (GTK_TABLE (table), hboxinpoutvars[ ColumnVar-1 ][ OffVar ], ColumnVar, ColumnVar+1, 2+OffVar, 2+OffVar+1 );
			}
			else
			{
//				gtk_box_pack_start (GTK_BOX(vboxboolvars[ ColumnVar ]), CheckBoolVar[ ColumnVar ][ OffVar ], FALSE, FALSE, 0);
gtk_table_attach_defaults (GTK_TABLE (table), CheckBoolVar[ ColumnVar ][ OffVar ], ColumnVar, ColumnVar+1, 2+OffVar, 2+OffVar+1 );
			}
//ForGTK3, deprecated...			TooltipsBoolVar[ ColumnVar ][ OffVar ] = gtk_tooltips_new();
			NumCheckWidget++;
		}
	}
//v0.9.20	UpdateAllLabelsBoolsVars( -1/*OnlyThisColumn*/ );
	
	gtk_window_set_icon(GTK_WINDOW(SpyBoolVarsWindow), gdk_pixbuf_new_from_inline (-1, IconWindowSpyBoolVars, FALSE, NULL));
	gtk_widget_show_all(SpyBoolVarsWindow);

	gtk_signal_connect( GTK_OBJECT(SpyBoolVarsWindow), "delete_event",
		GTK_SIGNAL_FUNC(BoolVarsWindowDeleteEvent), 0 );

//	gtk_window_set_policy( GTK_WINDOW(SpyBoolVarsWindow), FALSE/*allow_shrink*/, FALSE/*allow_grow*/, TRUE/*auto_shrink*/ );
}



//NO MORE USED, SINCE EDIT WIDGET AT THE RIGHT OF EACH SPY...
#ifdef AAAAAAAAAAAAAAAAAAAA
static gint OpenModifyVarWindow_clicked_event(GtkWidget *widget, int NumSpy)
{
	char BuffValue[ 30 ];
	CurrentModifyVarType = InfosGUI->FreeVarSpy[NumSpy].VarType;
	CurrentModifyVarOffset = InfosGUI->FreeVarSpy[NumSpy].VarNum;

	gtk_entry_set_text( GTK_ENTRY(ModifyVariableNameEdit), CreateVarName(CurrentModifyVarType,CurrentModifyVarOffset,InfosGene->DisplaySymbols) );
	sprintf( BuffValue, "%d", ReadVar(CurrentModifyVarType, CurrentModifyVarOffset) );
	gtk_entry_set_text( GTK_ENTRY(ModifyVariableValueEdit), BuffValue );
	gtk_widget_grab_focus( ModifyVariableValueEdit );

	gtk_widget_show( ModifyVarValueWindow );
	if ( SaveModifyVarPosX!=-1 && SaveModifyVarPosY!=-1 )
		gtk_window_move( GTK_WINDOW(ModifyVarValueWindow), SaveModifyVarPosX, SaveModifyVarPosY );

	return TRUE;
}
gint ModifyVarWindowDeleteEvent( GtkWidget * widget, GdkEvent * event, gpointer data )
{
	gtk_window_get_position( GTK_WINDOW(ModifyVarValueWindow), &SaveModifyVarPosX, &SaveModifyVarPosY );
	gtk_widget_hide( ModifyVarValueWindow );
	// we do not want that the window be destroyed.
	return TRUE;
}
gint ApplyModifiedVar( GtkWidget * widget, GdkEvent * event, gpointer data )
{
	int NewValue = atoi( gtk_entry_get_text( GTK_ENTRY(ModifyVariableValueEdit) ) );
	WriteVar( CurrentModifyVarType, CurrentModifyVarOffset, NewValue );
	gtk_window_get_position( GTK_WINDOW(ModifyVarValueWindow), &SaveModifyVarPosX, &SaveModifyVarPosY );
	gtk_widget_hide( ModifyVarValueWindow );
	// we do not want that the window be destroyed.
	return TRUE;
}
void ModifyVarWindowInitGtk( )
{
	GtkWidget *vboxMain,*hboxOkCancel;
	GtkWidget *ButtonOk,*ButtonCancel;
	ModifyVarValueWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title ((GtkWindow *)ModifyVarValueWindow, "Modify variable value");
	vboxMain = gtk_vbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER (ModifyVarValueWindow), vboxMain);
	gtk_widget_show (vboxMain);

	ModifyVariableNameEdit = gtk_entry_new();
//	gtk_widget_set_usize( GTK_WIDGET(ModifyVariableValueEdit),110,0);
	gtk_box_pack_start (GTK_BOX( vboxMain ), ModifyVariableNameEdit, TRUE, TRUE, 0);
	gtk_editable_set_editable( GTK_EDITABLE(ModifyVariableNameEdit), FALSE );
	gtk_widget_show( ModifyVariableNameEdit );

	ModifyVariableValueEdit = gtk_entry_new();
//	gtk_widget_set_usize( GTK_WIDGET(ModifyVariableValueEdit),110,0);
	gtk_box_pack_start (GTK_BOX( vboxMain ), ModifyVariableValueEdit, TRUE, TRUE, 0);
	gtk_widget_show( ModifyVariableValueEdit );
	gtk_signal_connect( GTK_OBJECT(ModifyVariableValueEdit), "activate",
                                        GTK_SIGNAL_FUNC(ApplyModifiedVar), (void *)NULL );

	hboxOkCancel = gtk_hbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER (vboxMain), hboxOkCancel);
	gtk_widget_show (hboxOkCancel);

	ButtonOk = gtk_button_new_from_stock( GTK_STOCK_OK );
	gtk_box_pack_start( GTK_BOX(hboxOkCancel), ButtonOk, FALSE, FALSE, 0 );
	gtk_widget_show( ButtonOk );
	gtk_signal_connect( GTK_OBJECT(ButtonOk), "clicked",
                                        GTK_SIGNAL_FUNC(ApplyModifiedVar), (void *)NULL );
	ButtonCancel = gtk_button_new_from_stock( GTK_STOCK_CANCEL );
	gtk_box_pack_start( GTK_BOX(hboxOkCancel), ButtonCancel, FALSE, FALSE, 0 );
	gtk_widget_show( ButtonCancel );
	gtk_signal_connect( GTK_OBJECT(ButtonCancel), "clicked",
                                        GTK_SIGNAL_FUNC(ModifyVarWindowDeleteEvent), NULL );

	gtk_signal_connect( GTK_OBJECT(ModifyVarValueWindow), "delete_event",
		GTK_SIGNAL_FUNC(ModifyVarWindowDeleteEvent), 0 );
}
#endif



static int ConvBinToInt (const char *Ascii)
{
	int Pos, Value, Len;

  Value=0;
  Len=strlen(Ascii);
  for (Pos=0; Pos<Len; Pos++)
  {
  	Value*=2;
    if (Ascii[Pos]!='0') Value+=1;
  }	
  return Value;
}

char * ConvIntToBin( unsigned int Val )
{
	static char TabBin[ 33 ];
	int Pos;
	unsigned int Mask = 0x80000000;
	char First1 = FALSE;
	strcpy( TabBin, "" );
	for ( Pos = 0; Pos<32; Pos++ )
	{
		if ( Val & Mask )
			First1 = TRUE;
		if ( First1 )
		{
			if ( Val & Mask )
				strcat( TabBin, "1" );
			else
				strcat( TabBin, "0" );
		}
		Mask = Mask>>1;
	}
	if ( Val==0 )
		strcpy( TabBin,"0" );
	return TabBin;
}

char * FormatVariableValue( char *BufferValue, int Format, int Value )
{	
	switch(Format)
	{
		case VAR_SPY_MODE_DEC_SIGNED: 
			sprintf(BufferValue,"%d",Value); break;
		case VAR_SPY_MODE_DEC_UNSIGNED: 
			sprintf(BufferValue,"%u",Value); break;	
		case VAR_SPY_MODE_HEX: 
			sprintf(BufferValue,"%X",Value); break;
		case VAR_SPY_MODE_BIN: 
			strcpy( BufferValue, ConvIntToBin( Value ) ); break;
	}
	return BufferValue;
}

static gint EntryVarSpy_activate_event(GtkWidget *widget, int NumSpy)
{
	int NewVarType,NewVarOffset;
//	int * NumVarSpy = &InfosGUI->VarSpy[NumSpy].VarType;
	char BufferVar[30];
	strcpy(BufferVar, gtk_entry_get_text((GtkEntry *)widget) );
	if (TextParserForAVar(BufferVar , &NewVarType, &NewVarOffset, NULL, FALSE/*PartialNames*/))
	{
//		*NumVarSpy++ = NewVarType;
//		*NumVarSpy = NewVarOffset;
		InfosGUI->FreeVarSpy[NumSpy].VarType = NewVarType;
		InfosGUI->FreeVarSpy[NumSpy].VarNum = NewVarOffset;
		UpdateAllLabelsFreeVars( NumSpy, BufferVar );
	}
	else
	{
		int OldType,OldOffset;
		/* Error Message */
		if (ErrorMessageVarParser)
			ShowMessageBoxError( ErrorMessageVarParser );
		else
			ShowMessageBoxError( _("Unknown variable...") );
//		OldType = *NumVarSpy++;
//		OldOffset = *NumVarSpy;
		OldType = InfosGUI->FreeVarSpy[NumSpy].VarType;
		OldOffset = InfosGUI->FreeVarSpy[NumSpy].VarNum;
		/* put back old correct var */
		gtk_entry_set_text((GtkEntry *)widget,CreateVarName(OldType,OldOffset,Preferences.DisplaySymbolsInMainWindow/*InfosGene->DisplaySymbols*/));
	}
	return TRUE;
}

static gint ComboBox_changed_event(GtkWidget *widget, int NumVarSpy)
{
	char BufferValue[50];

	InfosGUI->FreeVarSpyDisplayFormat[NumVarSpy] = gtk_combo_box_get_active( GTK_COMBO_BOX( DisplayFormatVarSpy[NumVarSpy] ) );
	// display latest value entered with new display format selected
	gtk_entry_set_text((GtkEntry *)EditVarSpyValue[NumVarSpy], FormatVariableValue(BufferValue, InfosGUI->FreeVarSpyDisplayFormat[NumVarSpy],
						LatestEditedVarValue[NumVarSpy] ));
	return TRUE;
}	

static gint EditVarSpyValue_changed_event(GtkWidget *widget, int NumVarSpy)
{
	int NewValue;
	char BufferValue[50];

	CurrentModifyVarType = InfosGUI->FreeVarSpy[NumVarSpy].VarType;
	CurrentModifyVarOffset = InfosGUI->FreeVarSpy[NumVarSpy].VarNum;

	switch(InfosGUI->FreeVarSpyDisplayFormat[NumVarSpy])
	{
		case VAR_SPY_MODE_DEC_SIGNED: 
				sscanf (gtk_entry_get_text( GTK_ENTRY(widget) ),"%d", &NewValue); break;
		case VAR_SPY_MODE_DEC_UNSIGNED: 
				sscanf (gtk_entry_get_text( GTK_ENTRY(widget) ),"%u", &NewValue); break;
		case VAR_SPY_MODE_HEX: 
				sscanf (gtk_entry_get_text( GTK_ENTRY(widget) ),"%x", &NewValue); break;
		case VAR_SPY_MODE_BIN: 
				NewValue = ConvBinToInt( gtk_entry_get_text( GTK_ENTRY(widget) ) ); break;
	}

	if ( InfosGUI->TargetMonitor.RemoteConnected )
	{
		InfosGUI->TargetMonitor.AskTargetToWriteVarType = CurrentModifyVarType;
		InfosGUI->TargetMonitor.AskTargetToWriteVarNum = CurrentModifyVarOffset;
		InfosGUI->TargetMonitor.AskTargetToWriteVarNewValue = NewValue;
	}
	else
	{
		WriteVar( CurrentModifyVarType, CurrentModifyVarOffset, NewValue );
	}
	// show the converted value entered we have understand!
	FormatVariableValue( BufferValue, InfosGUI->FreeVarSpyDisplayFormat[NumVarSpy], NewValue );
	gtk_entry_set_text( GTK_ENTRY(widget), BufferValue );
	LatestEditedVarValue[NumVarSpy] = NewValue;

	return TRUE;
}

void DisplayFreeVarSpy()
{
	int NumVarSpy;
	char BufferValue[50];
	char * Days[] = { _("Sunday"), _("Monday"), _("Tuesday"), _("Wednesday"), _("Thursday"), _("Friday"), _("Saturday") };
	int Time = ReadVar( VAR_WORD_SYSTEM, 0 );
	int Date = ReadVar( VAR_WORD_SYSTEM, 1 );
	if ( Date==0 )
	{
		strcpy( BufferValue, "" );
	}
	else
	{
		int DayVarValue = ReadVar( VAR_WORD_SYSTEM,2 );
		char * StringDayValue = "---";
		if( DayVarValue>=0 && DayVarValue<=6 )
			StringDayValue = gettext(Days[ DayVarValue ]);
		else
			printf("!!! error for DayVarValue (%d,t=%d,d=%d) in %s\n", DayVarValue, Time, Date, __FUNCTION__);
		sprintf( BufferValue, "%s %d/%d/%d %d:%02d:%02d", StringDayValue,
					2000+Date/10000, Date/100%100, Date%100, Time/10000, Time/100%100, Time%100 );
	}
	gtk_entry_set_text(GTK_ENTRY(EntryDateTime),BufferValue);
	for (NumVarSpy=0; NumVarSpy<NBR_FREE_VAR_SPY; NumVarSpy++)
	{
		FormatVariableValue( BufferValue, InfosGUI->FreeVarSpyDisplayFormat[NumVarSpy], 
								ReadVar(InfosGUI->FreeVarSpy[NumVarSpy].VarType,InfosGUI->FreeVarSpy[NumVarSpy].VarNum) );
		gtk_entry_set_text( GTK_ENTRY(EntryVarSpy[NBR_FREE_VAR_SPY+NumVarSpy]), BufferValue );
	}
}
/* if OnlyThisOne = -1 => all */
void UpdateAllLabelsFreeVars( int OnlyThisOne, char * VarName )
{
	int NumVarSpy;
	char BufferValue[50];
	for (NumVarSpy=0; NumVarSpy<NBR_FREE_VAR_SPY; NumVarSpy++)
	{
		if ( OnlyThisOne==-1 || NumVarSpy==OnlyThisOne )
		{
			char * OtherVarName = NULL;
			char TestVarIsABool = FALSE;
			StrIdVar * pIdVar = &InfosGUI->FreeVarSpy[NumVarSpy];
			if ( OnlyThisOne==-1 )
				VarName = CreateVarName(pIdVar->VarType,pIdVar->VarNum,Preferences.DisplaySymbolsInMainWindow/*InfosGene->DisplaySymbols*/);
			// first column = variable name
			gtk_entry_set_text((GtkEntry *)EntryVarSpy[ NumVarSpy ],VarName);
			if ( VarName[ 0 ]=='%' )
				OtherVarName = ConvVarNameToSymbol( VarName );
			else
				OtherVarName = ConvSymbolToVarName( VarName );
			if ( OtherVarName==NULL )
				OtherVarName = "";
//ForGTK3, deprecated...			gtk_tooltips_set_tip( TooltipsEntryVarSpy[ NumVarSpy ], EntryVarSpy[ NumVarSpy ], OtherVarName, NULL );
			gtk_widget_set_tooltip_text( EntryVarSpy[ NumVarSpy ], OtherVarName );
			// second column = current value
			gtk_entry_set_text( GTK_ENTRY(EntryVarSpy[NBR_FREE_VAR_SPY+NumVarSpy]), "");
			// format display combo => set to "bin" if boolean variable
			TestVarIsABool = TEST_VAR_IS_A_BOOL( pIdVar->VarType, pIdVar->VarNum );
			gtk_combo_box_set_active( GTK_COMBO_BOX( DisplayFormatVarSpy[NumVarSpy] ), TestVarIsABool?VAR_SPY_MODE_BIN:InfosGUI->FreeVarSpyDisplayFormat[NumVarSpy] );
			gtk_widget_set_sensitive( DisplayFormatVarSpy[NumVarSpy], !TestVarIsABool );
			// latest edited value entry
			gtk_entry_set_text( GTK_ENTRY(EditVarSpyValue[NumVarSpy]), FormatVariableValue(BufferValue, InfosGUI->FreeVarSpyDisplayFormat[NumVarSpy], 
				LatestEditedVarValue[NumVarSpy]));    
			gtk_widget_set_sensitive( EditVarSpyValue[ NumVarSpy ], TestVarIsReadWrite( pIdVar->VarType, pIdVar->VarNum ) );
		}
	}
}

gint FreeVarsWindowDeleteEvent( GtkWidget * widget, GdkEvent * event, gpointer data )
{
// Here, we must only toggle the menu check that will call itself the function below to close the window ...
//	gtk_widget_hide( SpyFreeVarsWindow );
	SetToggleMenuForFreeVarsWindow( FALSE/*OpenedWin*/ );
	// we do not want that the window be destroyed.
	return TRUE;
}

// called per toggle action menu, or at startup (if window saved open or not)...
void OpenSpyFreeVarsWindow( GtkAction * ActionOpen, gboolean OpenIt )
{
	if ( ActionOpen!=NULL )
		OpenIt = gtk_toggle_action_get_active( GTK_TOGGLE_ACTION(ActionOpen) );
	if ( OpenIt )
	{
		RestoreWindowPosiPrefs( "FreeVars", SpyFreeVarsWindow );
		gtk_widget_show( SpyFreeVarsWindow );
		gtk_window_present( GTK_WINDOW(SpyFreeVarsWindow) );
	}
	else
	{
		RememberWindowPosiPrefs( "FreeVars", SpyFreeVarsWindow, FALSE/*SaveWindowSize*/ );
		gtk_widget_hide( SpyFreeVarsWindow );
	}
}
void RememberFreeVarsWindowPrefs( void )
{
//ForGTK3	char WindowIsOpened = GTK_WIDGET_VISIBLE( GTK_WINDOW(SpyFreeVarsWindow) );
	char WindowIsOpened = MY_GTK_WIDGET_VISIBLE( SpyFreeVarsWindow );
	RememberWindowOpenPrefs( "FreeVars", WindowIsOpened );
	if ( WindowIsOpened )
		RememberWindowPosiPrefs( "FreeVars", SpyFreeVarsWindow, FALSE/*SaveWindowSize*/ );
}

void AddDisplayFormatItems( MyGtkComboBox * pComboBox )
{
//////printf("traduc:%s\n", _("Dec +/-") );
	gtk_combo_box_append_text( pComboBox, _("Dec +/-") );
	gtk_combo_box_append_text( pComboBox, _("Dec +") );
	gtk_combo_box_append_text( pComboBox, _("Hex") );
	gtk_combo_box_append_text( pComboBox, _("Bin") );
	gtk_combo_box_set_active( GTK_COMBO_BOX( pComboBox ), 0 );
}

GtkWidget * CreateFreeVarsPage( )
{
	GtkWidget * hboxfreevars[ 1+NBR_FREE_VAR_SPY ], *vboxMain;
	long ColumnVar;
	int NumLine,NumVarSpy,NumEntry;
//	GList *DisplayFormatItems = NULL;

//	DisplayFormatItems = g_list_append(DisplayFormatItems,"Dec");
//	DisplayFormatItems = g_list_append(DisplayFormatItems,"Hex");
//	DisplayFormatItems = g_list_append(DisplayFormatItems,"Bin");
	
	vboxMain = gtk_vbox_new (FALSE, 0);
	for(NumLine=0; NumLine<1+NBR_FREE_VAR_SPY; NumLine++)
	{
		hboxfreevars[ NumLine ] = gtk_hbox_new (FALSE, 0);
		gtk_container_add (GTK_CONTAINER (vboxMain), hboxfreevars[ NumLine ]);
//////printf("traduc ligne:%d\n", NumLine );
//		gtk_widget_show (hboxfreevars[ NumLine ]);
		if ( NumLine==0 )
		{
			GtkWidget * ModifyLabelInfo;
			EntryDateTime = gtk_entry_new( );
			gtk_box_pack_start (GTK_BOX( hboxfreevars[ NumLine ] ), EntryDateTime, TRUE, TRUE, 0);
			gtk_editable_set_editable( GTK_EDITABLE(EntryDateTime), FALSE);
//GTK3			gtk_widget_set_usize(EntryDateTime,110*2+80,0);
//			gtk_widget_show( EntryDateTime );
//////printf("traduc:%s\n", _("Modify Value :") );
			ModifyLabelInfo = gtk_label_new( _("Modify Value :") );
			gtk_label_set_justify ( GTK_LABEL(ModifyLabelInfo), GTK_JUSTIFY_CENTER );
			gtk_box_pack_start (GTK_BOX( hboxfreevars[ NumLine ] ), ModifyLabelInfo, TRUE, TRUE, 0);
//			gtk_widget_show( ModifyLabelInfo );
		}
		else
		{
			NumVarSpy = NumLine-1;
			//First column: variable name, Second column: current value
			for(ColumnVar=0; ColumnVar<2; ColumnVar++)
			{
				NumEntry = NumVarSpy+ColumnVar*NBR_FREE_VAR_SPY;
				EntryVarSpy[ NumEntry ] = gtk_entry_new();
//GTK3				gtk_widget_set_usize((GtkWidget *)EntryVarSpy[ NumEntry ],110,0);
// Changes the size request of the entry to be about the right size for n_chars characters !
gtk_entry_set_width_chars( GTK_ENTRY(EntryVarSpy[ NumEntry ]), (ColumnVar==0)?15:20 );
				gtk_entry_set_alignment ((GtkEntry *)EntryVarSpy[ NumEntry ], (ColumnVar==1)?1:0);
				gtk_box_pack_start (GTK_BOX( hboxfreevars[ NumLine ] ), EntryVarSpy[ NumEntry ], TRUE, TRUE, 0);
//				gtk_widget_show(EntryVarSpy[NumEntry]);
				if ( ColumnVar==0 )
				{
//ForGTK3, deprecated...					TooltipsEntryVarSpy[ NumVarSpy ] = gtk_tooltips_new();
					gtk_signal_connect(GTK_OBJECT (EntryVarSpy[ NumEntry ]), "activate",
	                                        GTK_SIGNAL_FUNC(EntryVarSpy_activate_event), (void *)NumVarSpy);
				}
				else
				{
					gtk_editable_set_editable( GTK_EDITABLE(EntryVarSpy[ NumEntry ]), FALSE );
				}
			}

//			DisplayFormatVarSpy[NumVarSpy] = gtk_combo_new();
//			gtk_combo_set_value_in_list(GTK_COMBO(DisplayFormatVarSpy[NumVarSpy]), TRUE /*val*/, FALSE /*ok_if_empty*/);
//			gtk_combo_set_popdown_strings(GTK_COMBO(DisplayFormatVarSpy[NumVarSpy]), DisplayFormatItems);
//			gtk_widget_set_usize((GtkWidget *)DisplayFormatVarSpy[NumVarSpy],65,0);
//			gtk_box_pack_start (GTK_BOX(hboxfreevars[ NumLine ]), DisplayFormatVarSpy[NumVarSpy], FALSE, FALSE, 0);
//			gtk_widget_show(DisplayFormatVarSpy[NumVarSpy]);
			DisplayFormatVarSpy[NumVarSpy] = gtk_combo_box_new_text();
			AddDisplayFormatItems( MY_GTK_COMBO_BOX( DisplayFormatVarSpy[NumVarSpy] ) );
//GTK3			gtk_widget_set_usize((GtkWidget *)DisplayFormatVarSpy[NumVarSpy],80,0);
			gtk_box_pack_start (GTK_BOX(hboxfreevars[ NumLine ]), DisplayFormatVarSpy[NumVarSpy], FALSE, FALSE, 0);
//			gtk_widget_show(DisplayFormatVarSpy[NumVarSpy]);
	
// Modified by helitp AT arrakis DOT es 19/7/2011 to replace popup window to modify value by new edit widget column on right.
//			ModifyVarSpy[NumVarSpy] = gtk_button_new();
//			gtk_button_set_image( GTK_BUTTON ( ModifyVarSpy[NumVarSpy] ),
//	                        gtk_image_new_from_stock (GTK_STOCK_EDIT, GTK_ICON_SIZE_SMALL_TOOLBAR) );
//			gtk_box_pack_start (GTK_BOX(hboxfreevars[ NumLine ]), ModifyVarSpy[NumVarSpy], FALSE, FALSE, 0);
//			gtk_widget_show(ModifyVarSpy[NumVarSpy]);
//			gtk_signal_connect( GTK_OBJECT(ModifyVarSpy[ NumVarSpy ]), "clicked",
//	                                        (GtkSignalFunc)OpenModifyVarWindow_clicked_event, (void *)NumVarSpy );

			gtk_signal_connect(GTK_OBJECT( DisplayFormatVarSpy[NumVarSpy] ), "changed", 
									GTK_SIGNAL_FUNC(ComboBox_changed_event), (void *)NumVarSpy );
			gtk_signal_connect(GTK_OBJECT( DisplayFormatVarSpy[NumVarSpy] ), "popup", 
									GTK_SIGNAL_FUNC(ComboBox_changed_event), (void *)NumVarSpy );

			NumEntry = NumVarSpy;
			EditVarSpyValue[ NumEntry ] = gtk_entry_new();
//GTK3			gtk_widget_set_usize(EditVarSpyValue[ NumEntry ],110,0);
// Changes the size request of the entry to be about the right size for n_chars characters !
gtk_entry_set_width_chars( GTK_ENTRY(EditVarSpyValue[ NumEntry ]), 20 );
			gtk_entry_set_text( GTK_ENTRY(EditVarSpyValue[ NumEntry ]),"0" );
			LatestEditedVarValue[NumVarSpy] = 0;
			gtk_entry_set_alignment ((GtkEntry *)EditVarSpyValue[ NumEntry ], 1);
			gtk_box_pack_start (GTK_BOX( hboxfreevars[ NumLine ] ), EditVarSpyValue[ NumEntry ], TRUE, TRUE, 0);
//			gtk_widget_show(EditVarSpyValue[NumEntry]);

			gtk_editable_set_editable( GTK_EDITABLE(EditVarSpyValue[ NumEntry ]), TRUE );
			gtk_signal_connect(GTK_OBJECT (EditVarSpyValue[ NumEntry ]), "activate", 
	                                        GTK_SIGNAL_FUNC(EditVarSpyValue_changed_event), (void *)NumVarSpy );
        }
	}
	
//v0.9.20	UpdateAllLabelsFreeVars( -1/*OnlyThisOne*/, NULL );

	return vboxMain;
}
//ForGTK3 void SignalTargetInfosPageSelected( GtkNotebook * notebook, GtkNotebookPage * page, guint page_num, gpointer user_data )
void SignalTargetInfosPageSelected( GtkNotebook * notebook, GtkWidget * page, guint page_num, gpointer user_data )
{
	if ( page_num==1 )
		DisplayProjectProperties( );
	if ( page_num==2 )
	{
		int NumLine;
		for(NumLine=0; NumLine<NBR_TARGET_INFOS; NumLine++)
		{
			gtk_entry_set_text( GTK_ENTRY(TargetInfoEntry[ NumLine ]), !InfosGUI->TargetMonitor.RemoteConnected?(_("Not connected")):"---" );
		}
		InfosGUI->TargetMonitor.AskTargetToGetTargetInfos = 1;
	}
}
// Translated words not directly done in protocol to avoid i18n dependancy for an embedded target...
// So here a little less easier of course!
void ConvertInfoDiskStats( char * BuffConv )
{
	int ScanChar;
	int ScanNew = 0;
	char PrevChar = 0;
	BuffConv[ 0 ] = '\0';
	for( ScanChar = 0; ScanChar<strlen( InfosGUI->TargetMonitor.InfoDiskStats ); ScanChar++ )
	{
		char CurChar = InfosGUI->TargetMonitor.InfoDiskStats[ ScanChar ];
		if ( CurChar=='=' )
		{
			char * ConvText = NULL;
			switch( PrevChar )
			{
				case 'U': ConvText = _("Used"); break;
				case 'F': ConvText = _("Free"); break;
				case 'S': ConvText = _("Size"); break;
			}
			if ( ConvText!=NULL )
			{
				ScanNew--;
				strcpy( &BuffConv[ScanNew], ConvText );
				ScanNew = ScanNew+strlen( ConvText );
				BuffConv[ScanNew++] = ':';
				BuffConv[ScanNew] = '\0';
			}
			else
			{
				BuffConv[ScanNew++] = CurChar;
				BuffConv[ScanNew] = '\0';
			}
		}
		else
		{
			BuffConv[ScanNew++] = CurChar;
			BuffConv[ScanNew] = '\0';
		}
//printf("CurChar=%c - BuffConv(%d)=%s\n", CurChar, ScanNew, BuffConv );
		PrevChar = CurChar;
	}
}
void DisplayTargetInfosVersion( void )
{
	gtk_entry_set_text( GTK_ENTRY(TargetInfoEntry[ 0 ]), InfosGUI->TargetMonitor.InfoSoftVersion );
	gtk_entry_set_text( GTK_ENTRY(TargetInfoEntry[ 1 ]), InfosGUI->TargetMonitor.InfoKernelVersion );
	gtk_entry_set_text( GTK_ENTRY(TargetInfoEntry[ 2 ]), (InfosGUI->TargetMonitor.InfoXenomaiVersion[0]!='\0')?InfosGUI->TargetMonitor.InfoXenomaiVersion:"---" );
	gtk_entry_set_text( GTK_ENTRY(TargetInfoEntry[ 3 ]), InfosGUI->TargetMonitor.InfoLinuxDistribVersion );
	char * InfoStats = (char *)malloc( 300 );
	if ( InfoStats )
	{
		ConvertInfoDiskStats( InfoStats );
		gtk_entry_set_text( GTK_ENTRY(TargetInfoEntry[ 4 ]), InfoStats );
		free( InfoStats );
	}
}
GtkWidget * CreateTargetInfosPage( )
{
	GtkWidget * hbox[ NBR_TARGET_INFOS ], *vboxMain;
	int NumLine;
	GtkWidget * pLabel;
	char * Text[ NBR_TARGET_INFOS ] = { N_("ClassicLadder Soft.Version"), N_("Kernel Version"), N_("Xenomai version"), N_("Linux Distribution"), N_("Disk statistics") };
	vboxMain = gtk_vbox_new (FALSE, 0);
	for(NumLine=0; NumLine<NBR_TARGET_INFOS; NumLine++)
	{
		hbox[ NumLine ] = gtk_hbox_new (FALSE, 0);
		gtk_container_add (GTK_CONTAINER (vboxMain), hbox[ NumLine ]);

		pLabel = gtk_label_new( gettext(Text[NumLine]) );
//		gtk_label_set_justify ( GTK_LABEL(pLabel), GTK_JUSTIFY_CENTER );
		gtk_box_pack_start (GTK_BOX( hbox[ NumLine ] ), pLabel, TRUE, TRUE, 0);

		TargetInfoEntry[ NumLine ] = gtk_entry_new( );
		gtk_box_pack_start (GTK_BOX( hbox[ NumLine ] ), TargetInfoEntry[ NumLine ], TRUE, TRUE, 0);
		gtk_editable_set_editable( GTK_EDITABLE(TargetInfoEntry[ NumLine ]), FALSE);
	}
	return vboxMain;
}
void DisplayProjectProperties( void )
{
	GtkTextBuffer * TextBufferForGtk = gtk_text_view_get_buffer( GTK_TEXT_VIEW( ProjectInfoEntry[ 7 ] ) );
	gtk_entry_set_text( GTK_ENTRY(ProjectInfoEntry[ 0 ]), InfosGene->ProjectProperties.ProjectName );
	gtk_entry_set_text( GTK_ENTRY(ProjectInfoEntry[ 1 ]), InfosGene->ProjectProperties.ProjectSite );
	gtk_entry_set_text( GTK_ENTRY(ProjectInfoEntry[ 2 ]), InfosGene->ProjectProperties.ParamAuthor );
	gtk_entry_set_text( GTK_ENTRY(ProjectInfoEntry[ 3 ]), InfosGene->ProjectProperties.ParamCompany );
	gtk_entry_set_text( GTK_ENTRY(ProjectInfoEntry[ 4 ]), InfosGene->ProjectProperties.ParamVersion );
	gtk_entry_set_text( GTK_ENTRY(ProjectInfoEntry[ 5 ]), InfosGene->ProjectProperties.ParamCreaDate );
	gtk_entry_set_text( GTK_ENTRY(ProjectInfoEntry[ 6 ]), InfosGene->ProjectProperties.ParamModifDate );
	gtk_text_buffer_set_text( TextBufferForGtk, InfosGene->ProjectProperties.ParamComment, -1 );
}
void ButtonApplyProjectProperties( )
{
	if ( InfosGUI->TargetMonitor.RemoteConnected )
	{
		ShowMessageBoxError( _("Not possible when connected to a remote target...") );
	}
	else
	{
		GtkTextIter start, end;
		GtkTextBuffer * TextBufferForGtk = gtk_text_view_get_buffer( GTK_TEXT_VIEW( ProjectInfoEntry[ 7 ] ) );
		strncpy( InfosGene->ProjectProperties.ProjectName, gtk_entry_get_text( GTK_ENTRY( ProjectInfoEntry[ 0 ] ) ), LGT_STR_INFO-1 );
		InfosGene->ProjectProperties.ProjectName[ LGT_STR_INFO-1 ] = '\0';
		strncpy( InfosGene->ProjectProperties.ProjectSite, gtk_entry_get_text( GTK_ENTRY( ProjectInfoEntry[ 1 ] ) ), LGT_STR_INFO-1 );
		InfosGene->ProjectProperties.ProjectSite[ LGT_STR_INFO-1 ] = '\0';
		strncpy( InfosGene->ProjectProperties.ParamAuthor, gtk_entry_get_text( GTK_ENTRY( ProjectInfoEntry[ 2 ] ) ), LGT_STR_INFO-1 );
		InfosGene->ProjectProperties.ParamAuthor[ LGT_STR_INFO-1 ] = '\0';
		strncpy( InfosGene->ProjectProperties.ParamCompany, gtk_entry_get_text( GTK_ENTRY( ProjectInfoEntry[ 3 ] ) ), LGT_STR_INFO-1 );
		InfosGene->ProjectProperties.ParamCompany[ LGT_STR_INFO-1 ] = '\0';
		strncpy( InfosGene->ProjectProperties.ParamVersion, gtk_entry_get_text( GTK_ENTRY( ProjectInfoEntry[ 4 ] ) ), LGT_STR_INFO-1 );
		InfosGene->ProjectProperties.ParamVersion[ LGT_STR_INFO-1 ] = '\0';
		gtk_text_buffer_get_start_iter(TextBufferForGtk, &start);
		gtk_text_buffer_get_end_iter(TextBufferForGtk, &end);
		strncpy( InfosGene->ProjectProperties.ParamComment, gtk_text_buffer_get_text( TextBufferForGtk, &start, &end, TRUE), LGT_STR_INFO_COMMENT-1 );
		InfosGene->ProjectProperties.ParamComment[ LGT_STR_INFO_COMMENT-1 ] = '\0';
	}
}
GtkWidget * CreateProjectPropertiesPage( )
{
	GtkWidget * hbox[ NBR_PROJECT_INFOS+1 ], *vboxMain;
	int NumLine;
	GtkWidget * pLabel;
	GtkWidget * pApplyButton;
	char * Text[ NBR_PROJECT_INFOS-1 ] = { N_("Project Name"), N_("Project Site"), N_("Author"), N_("Company"), N_("Param.Version"), N_("Creation Date"), N_("Modify Date") };
	vboxMain = gtk_vbox_new (FALSE, 0);
	for(NumLine=0; NumLine<NBR_PROJECT_INFOS+1; NumLine++)
	{
		hbox[ NumLine ] = gtk_hbox_new (FALSE, 0);
		gtk_container_add (GTK_CONTAINER (vboxMain), hbox[ NumLine ]);

		if ( NumLine<NBR_PROJECT_INFOS-1 )
		{
			pLabel = gtk_label_new( gettext(Text[NumLine]) );
//			gtk_label_set_justify ( GTK_LABEL(pLabel), GTK_JUSTIFY_CENTER );
			gtk_box_pack_start (GTK_BOX( hbox[ NumLine ] ), pLabel, TRUE, TRUE, 0);

			ProjectInfoEntry[ NumLine ] = gtk_entry_new( );
			gtk_entry_set_max_length( GTK_ENTRY(ProjectInfoEntry[ NumLine ]),LGT_STR_INFO-1 );
			gtk_box_pack_start (GTK_BOX( hbox[ NumLine ] ), ProjectInfoEntry[ NumLine ], TRUE, TRUE, 0);
			if ( NumLine>=5 )
				gtk_editable_set_editable( GTK_EDITABLE(ProjectInfoEntry[ NumLine ]), FALSE);
		}
		else if ( NumLine<NBR_PROJECT_INFOS )
		{
			ProjectInfoEntry[ NumLine ] = gtk_text_view_new( );
			gtk_box_pack_start (GTK_BOX( hbox[ NumLine ] ), ProjectInfoEntry[ NumLine ], TRUE, TRUE, 0);
		}
		else
		{
			pApplyButton = gtk_button_new_with_label( _("Apply") );
			gtk_box_pack_start(GTK_BOX(hbox[ NumLine ]),pApplyButton,TRUE,FALSE,0);
			gtk_signal_connect(GTK_OBJECT(pApplyButton), "clicked", 
				GTK_SIGNAL_FUNC(ButtonApplyProjectProperties), 0);
		}
	}
	return vboxMain;
}
void FreeVarsWindowInitGtk( )
{
	GtkWidget *nbook;

	SpyFreeVarsWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title ((GtkWindow *)SpyFreeVarsWindow, _("Spy free vars") );
	
	nbook = gtk_notebook_new( );
	gtk_notebook_append_page( GTK_NOTEBOOK(nbook), CreateFreeVarsPage( ),
				 gtk_label_new ( _("Vars & Time") ) );
	gtk_notebook_append_page( GTK_NOTEBOOK(nbook), CreateProjectPropertiesPage( ),
				 gtk_label_new ( _("Project properties") ) );
	gtk_notebook_append_page( GTK_NOTEBOOK(nbook), CreateTargetInfosPage( ),
				 gtk_label_new ( _("Target infos") ) );
	gtk_container_add( GTK_CONTAINER (SpyFreeVarsWindow), nbook );
	gtk_signal_connect( GTK_OBJECT(nbook), "switch-page",
                                        GTK_SIGNAL_FUNC(SignalTargetInfosPageSelected), (void *)NULL );
	gtk_window_set_icon(GTK_WINDOW(SpyFreeVarsWindow), gdk_pixbuf_new_from_inline (-1, IconWindowSpyFreeVars, FALSE, NULL));
	gtk_widget_show_all( SpyFreeVarsWindow );

	gtk_signal_connect( GTK_OBJECT(SpyFreeVarsWindow), "delete_event",
		GTK_SIGNAL_FUNC(FreeVarsWindowDeleteEvent), 0 );
}


void VarsWindowInitGtk()
{
	FreeVarsWindowInitGtk( );
	BoolVarsWindowInitGtk( );
//	ModifyVarWindowInitGtk( );
}

