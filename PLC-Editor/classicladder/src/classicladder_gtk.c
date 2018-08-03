/* Classic Ladder Project */
/* Copyright (C) 2001-2018 Marc Le Douarain */
/* http://www.sourceforge.net/projects/classicladder */
/* http://sites.google.com/site/classicladder */
/* February 2001 */
/* ---------------------------------- */
/* GTK Interface & Main */
/* Inspired (at the start) from the scribble example. */
/* ---------------------------------- */
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
#include <gdk/gdk.h>
#include <cairo.h>
#include <cairo-svg.h>
#include <zlib.h>
#include <gdk/gdkkeysyms.h>  // GDK keys codes
#include <gdk/gdktypes.h> // GDK keys masks
#include <libintl.h> // i18n
#include <locale.h> // i18n

#include "classicladder.h"
#include "global.h"
#include "classicladder_gtk.h"
#include "hardware.h"
#ifdef SEQUENTIAL_SUPPORT
#include "drawing_sequential.h"
#endif
#include "log_events.h"
#include "menu_and_toolbar_gtk.h"
#include "preferences.h"
#include "search.h"
#include "monitor_windows_gtk.h"
#include "monitor_serial_config_window_gtk.h"

//Cairo GdkPixmap *pixmap = NULL;
GtkWidget *drawing_area = NULL;
GtkWidget *entrylabel,*entrycomment;
GtkWidget *CheckDispSymbols;
//#if defined( RT_SUPPORT ) || defined( __XENO__ )
GtkWidget *WidgetDurationOfLastScan;
//#endif
//Since menu/toolbar... GtkWidget *ButtonRunStop;
GtkWidget *VScrollBar;
GtkWidget *HScrollBar;
GtkAdjustment * AdjustVScrollBar;
GtkAdjustment * AdjustHScrollBar;
GtkWidget *FileSelector;
GtkWidget *ConfirmDialog;
GtkWidget *MainSectionWindow;
GtkWidget *StatusBar;
gint StatusBarContextId;

GtkWidget *hBoxSearch;
GtkWidget *ButtonSearchClose;
GtkWidget *SearchEntry;
GtkWidget *SearchType;
GtkWidget *ButtonSearchNow;
GtkWidget *ButtonSearchNext;
GtkWidget *ButtonSearchPrev;

GtkWidget *hBoxFileTransfer;
GtkWidget *FileTransferBar;
GtkWidget *FileTransferLabel;
GtkWidget *FileTransferAbortButton;

#include "drawing.h"
#include "vars_access.h"
#include "calc.h"
#include "files_project.h"
#include "edit.h"
#include "edit_gtk.h"
#include "editproperties_gtk.h"
#include "manager_gtk.h"
#include "config_gtk.h"
#include "socket_server.h"
#include "socket_modbus_master.h"
#ifdef SEQUENTIAL_SUPPORT
#include "calc_sequential.h"
#endif
#include "symbols_gtk.h"
#include "spy_vars_gtk.h"
#include "print_gtk.h"
#include "vars_system.h"
#include "log_events_gtk.h"
//#include "tasks.h"
#include "time_and_rtc.h"
#include "icons_gtk.h"
#include "../icons/IconClassicLadderApplication.h"
#include "network_config_window_gtk.h"

extern GtkUIManager * uiManager;	 // in menu_and_toolbar_gtk.c

void CairoDrawCurrentSectionOnDrawingArea( cairo_t *cr )
{
	/* clean up */
	double w,h;
//ForGTK3
#if GTK_MAJOR_VERSION>=3
	w = gtk_widget_get_allocated_width( drawing_area );
	h = gtk_widget_get_allocated_height( drawing_area );
#else
	w = drawing_area->allocation.width;
	h = drawing_area->allocation.height;
#endif
	cairo_set_source_rgb( cr, 1, 1 ,1 );
	cairo_rectangle( cr, 0.0, 0.0, w, h );
	cairo_fill( cr );
	GetTheSizesForRung( );
	DrawCurrentSection( cr );
}

/* Create a new backing pixmap of the appropriate size */
/*static gint configure_event( GtkWidget		 *widget,
							GdkEventConfigure *event )
{
	if (pixmap)
		gdk_pixmap_unref(pixmap);

	pixmap = gdk_pixmap_new(widget->window,
							widget->allocation.width,
							widget->allocation.height,
							-1);
	gdk_draw_rectangle (pixmap,
						widget->style->white_gc,
						TRUE,
						0, 0,
						widget->allocation.width,
						widget->allocation.height);
	return TRUE;
}*/
/* Redraw the screen with Cairo */
#if GTK_MAJOR_VERSION>=3
void draw_callback( GtkWidget *widget, cairo_t *cr, gpointer data)
{
	CairoDrawCurrentSectionOnDrawingArea( cr );
}
#else
static gint expose_event( GtkWidget	  *widget,
						GdkEventExpose *event )
{
/*	gdk_draw_pixmap(widget->window,
					widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
					pixmap,
					event->area.x, event->area.y,
					event->area.x, event->area.y,
					event->area.width, event->area.height);
*/
	cairo_t *cr = gdk_cairo_create( drawing_area->window );
	CairoDrawCurrentSectionOnDrawingArea( cr );
	cairo_destroy( cr );
	return FALSE;
}
#endif

void GetTheSizesForRung( void )
{
	static int PageHeightBak = 0;
	static int BlockHeightBak = 0;
//ForGTK3
#if GTK_MAJOR_VERSION>=3
	InfosGene->BlockWidth = ((gtk_widget_get_allocated_width( drawing_area )*995/1000) / RUNG_WIDTH);
#else
	InfosGene->BlockWidth = ((GTK_WIDGET(drawing_area)->allocation.width*995/1000) / RUNG_WIDTH);
#endif
	// keep ratio aspect (if defaults values of size block not square)
	InfosGene->BlockHeight = InfosGene->BlockWidth*BLOCK_HEIGHT_DEF/BLOCK_WIDTH_DEF;

//ForGTK3
#if GTK_MAJOR_VERSION>=3
	InfosGene->PageHeight = gtk_widget_get_allocated_height( drawing_area );
#else
	InfosGene->PageHeight = GTK_WIDGET(drawing_area)->allocation.height;
#endif
	// used for sequential
//ForGTK3
#if GTK_MAJOR_VERSION>=3
	InfosGene->PageWidth = gtk_widget_get_allocated_width( drawing_area );
#else
	InfosGene->PageWidth = GTK_WIDGET(drawing_area)->allocation.width;
#endif

	// size of the page or block changed ?
	if ( InfosGene->PageHeight!=PageHeightBak || InfosGene->BlockHeight!=BlockHeightBak )
		UpdateVScrollBar( TRUE/*AutoSelectCurrentRung*/ );
	PageHeightBak = InfosGene->PageHeight;
	BlockHeightBak = InfosGene->BlockHeight;
}

// calc total nbr rungs in a section, and nbr of rungs before current rung.
void GetCurrentNumAndNbrRungsForCurrentSection( int * pCurrNumRung, int * pNbrRungs )
{
	int iSecurityBreak = 0;
	int NbrRungs = 1;
	int ScanRung = InfosGene->FirstRung;
	int NumCurrentRung = 0;
	while ( ScanRung!=InfosGene->LastRung && iSecurityBreak++<=NBR_RUNGS )
	{
		NbrRungs++;
		ScanRung = RungArray[ ScanRung ].NextRung;
	}
	ScanRung = InfosGene->FirstRung;
	iSecurityBreak = 0;
	while ( ScanRung!=InfosGene->CurrentRung && iSecurityBreak++<=NBR_RUNGS )
	{
		NumCurrentRung++;
		ScanRung = RungArray[ ScanRung ].NextRung;
	}
	if ( iSecurityBreak>=NBR_RUNGS )
		debug_printf("!!!error loop in %s()!\n",__FUNCTION__);
	if ( pCurrNumRung!=NULL )
		*pCurrNumRung = NumCurrentRung;
	if ( pNbrRungs!=NULL )
		*pNbrRungs = NbrRungs;
}
// flag used when signal event will be called on "value changed", to remember to not do the auto-select of the current rung.
// if current rung selected by software (search), and value adjusted by calling this function, so of course current rung is visible ! ;-)
char RememberNotToAutoSelectCurrentRung = FALSE;
void UpdateVScrollBar( char AutoSelectCurrentRung )
{
	//v0.9.20
	if( SectionArray==NULL )
		return;
		
	int iCurrentLanguage = SectionArray[ InfosGene->CurrentSection ].Language;
	RememberNotToAutoSelectCurrentRung = !AutoSelectCurrentRung;
	if ( iCurrentLanguage==SECTION_IN_LADDER )
	{
		int NbrTotalRungs = 1;
		int NbrRungsBeforeCurrent = 0;
		int AdjustUpper;
		int AdjustValue;
		GetCurrentNumAndNbrRungsForCurrentSection( &NbrRungsBeforeCurrent, &NbrTotalRungs );
//printf("=> in %s(); NbrTotalRungs=%d , NbrRungsBeforeCurrent=%d , AutoSelectCurrentRung=%d\n", __FUNCTION__, NbrTotalRungs, NbrRungsBeforeCurrent, AutoSelectCurrentRung);
		AdjustUpper = NbrTotalRungs * TOTAL_PX_RUNG_HEIGHT;
		AdjustValue = NbrRungsBeforeCurrent *  TOTAL_PX_RUNG_HEIGHT;
		gtk_adjustment_set_lower( AdjustVScrollBar, 0 );
		gtk_adjustment_set_upper( AdjustVScrollBar, AdjustUpper );
		// go up lift to take into account the fact that total height page is generally more than just one rung...
		while( AdjustValue+InfosGene->PageHeight > AdjustUpper )
		{
			AdjustValue = AdjustValue - TOTAL_PX_RUNG_HEIGHT;
		}
		gtk_adjustment_set_value( AdjustVScrollBar, AdjustValue );
		gtk_adjustment_set_step_increment( AdjustVScrollBar, InfosGene->BlockHeight );
		gtk_adjustment_set_page_increment( AdjustVScrollBar, TOTAL_PX_RUNG_HEIGHT );
		gtk_adjustment_set_page_size( AdjustVScrollBar, InfosGene->PageHeight );
//printf("=> in %s(); Value=%d , Upper=%d , BlockHeight=%d , RungHeight=%d , PageHeight=%d\n", __FUNCTION__, AdjustValue, AdjustUpper, InfosGene->BlockHeight, TOTAL_PX_RUNG_HEIGHT, InfosGene->PageHeight );
		gtk_adjustment_changed( AdjustVScrollBar );
		gtk_adjustment_value_changed( AdjustVScrollBar );
		gtk_widget_hide( HScrollBar );
	}
#ifdef SEQUENTIAL_SUPPORT
	if ( iCurrentLanguage==SECTION_IN_SEQUENTIAL )
	{
		gtk_widget_show( HScrollBar );
		RefreshLabelCommentEntries( );
		gtk_adjustment_set_lower( AdjustVScrollBar, 0 );
		gtk_adjustment_set_upper( AdjustVScrollBar, SEQ_PAGE_HEIGHT * SEQ_SIZE_DEF );
		gtk_adjustment_set_value( AdjustVScrollBar, 0 );
		gtk_adjustment_set_step_increment( AdjustVScrollBar, SEQ_SIZE_DEF );
		gtk_adjustment_set_page_increment( AdjustVScrollBar, InfosGene->PageHeight );
		gtk_adjustment_set_page_size( AdjustVScrollBar, InfosGene->PageHeight );
		gtk_adjustment_changed( AdjustVScrollBar );
		gtk_adjustment_value_changed( AdjustVScrollBar );
		gtk_adjustment_set_lower( AdjustHScrollBar, 0 );
		gtk_adjustment_set_upper( AdjustHScrollBar, SEQ_PAGE_WIDTH * SEQ_SIZE_DEF );
		gtk_adjustment_set_value( AdjustHScrollBar, 0 );
		gtk_adjustment_set_step_increment( AdjustHScrollBar, SEQ_SIZE_DEF );
		gtk_adjustment_set_page_increment( AdjustHScrollBar, InfosGene->PageWidth );
		gtk_adjustment_set_page_size( AdjustHScrollBar, InfosGene->PageWidth );
		gtk_adjustment_changed( AdjustHScrollBar );
		gtk_adjustment_value_changed( AdjustHScrollBar );
	}
#endif
}

// - used to auto-select current rung when lift moved
// - also used when user click on page to select its current rung (to edit later)
void ChoiceOfTheCurrentRung( int NbrOfRungsAfterTopRung )
{
	int DecptNbrRungs = NbrOfRungsAfterTopRung;

//printf("OffsetHiddenTopRungDisplayed=%d\n", InfosGene->OffsetHiddenTopRungDisplayed);
	// per default, the current rung, is the top one displayed...
	InfosGene->CurrentRung = InfosGene->TopRungDisplayed;

//printf("=> start choice of current rung, %d passes...(CurrentRung=%d)\n",NbrOfRungsAfterTopRung, InfosGene->CurrentRung);
	while( DecptNbrRungs>0 && InfosGene->CurrentRung!=InfosGene->LastRung )
	{
		InfosGene->CurrentRung = RungArray[ InfosGene->CurrentRung ].NextRung;
		DecptNbrRungs--;
	}
//printf("=> end choice of current rung. (CurrentRung=%d)\n", InfosGene->CurrentRung);
}
void CalcOffsetCurrentRungDisplayed( void )
{
	int ScanRung = InfosGene->TopRungDisplayed;
	int NbrOfRungsFnd = 0;
	// if OffsetHiddenTopRungDisplayed==0, vertical shift of the current rung is 0,
	// else this is the y value to substract to have the vertical shift... (we will add many full rungs heights after!)
	InfosGene->OffsetCurrentRungDisplayed = -1*InfosGene->OffsetHiddenTopRungDisplayed;
//	InfosGene->OffsetCurrentRungDisplayed += NbrOfRungsAfterTopRung*TOTAL_PX_RUNG_HEIGHT;
	while( ScanRung!=InfosGene->CurrentRung )
	{
		ScanRung = RungArray[ ScanRung ].NextRung;
		InfosGene->OffsetCurrentRungDisplayed += TOTAL_PX_RUNG_HEIGHT;
		NbrOfRungsFnd++;
	}
//printf("=> In %s, CurrentRung=%d , NbrOfRungsAfterTopRung=%d, OffsetCurrentRungDisplayed=%d\n", __FUNCTION__, InfosGene->CurrentRung, NbrOfRungsFnd, InfosGene->OffsetCurrentRungDisplayed);
//////test	if ( InfosGene->OffsetCurrentRungDisplayed<0 )
//////test		debug_printf( "Error in ChoiceOfTheCurrentRung( %d ) with OffsetCurrentRungDisplayed=%d\n", NbrOfRungsFnd, InfosGene->OffsetCurrentRungDisplayed );
	RefreshLabelCommentEntries( );
}

static gint VScrollBar_value_changed_event( GtkAdjustment * ScrollBar, void * not_used )
{
	int iCurrentLanguage = SectionArray[ InfosGene->CurrentSection ].Language;
	if ( iCurrentLanguage==SECTION_IN_LADDER )
	{
		int NbrRungsBeforeCurrent = ((int)gtk_adjustment_get_value( ScrollBar ))/TOTAL_PX_RUNG_HEIGHT;
		int ScanRung = 0;
		InfosGene->TopRungDisplayed = InfosGene->FirstRung;
		if ( NbrRungsBeforeCurrent<0 )
			NbrRungsBeforeCurrent = 0;
		while( ScanRung!=NbrRungsBeforeCurrent )
		{
			InfosGene->TopRungDisplayed = RungArray[ InfosGene->TopRungDisplayed ].NextRung;
			ScanRung++;
		}
		InfosGene->OffsetHiddenTopRungDisplayed = ((int)gtk_adjustment_get_value( ScrollBar ))%TOTAL_PX_RUNG_HEIGHT;
//printf("=> VScrollBar 'value changed' call ; NbrRungsBeforeCurrent=%d OffsetHiddenTopRungDisplayed=%d TopRungDisplayed=%d DoNotAutoSelectRung=%d\n",NbrRungsBeforeCurrent,InfosGene->OffsetHiddenTopRungDisplayed,InfosGene->TopRungDisplayed,RememberNotToAutoSelectCurrentRung);

		if ( RememberNotToAutoSelectCurrentRung==FALSE )
		{
			// auto select current rung which is the first completely displayed on the page.
			// if top rung displayed entirely (no vertical offset), it's the current rung => give '0'.
			// else, search the next one => give '1'.
			ChoiceOfTheCurrentRung( (InfosGene->OffsetHiddenTopRungDisplayed>0)?1:0 );
		}
		RememberNotToAutoSelectCurrentRung = FALSE;
		CalcOffsetCurrentRungDisplayed( );
	}
	InfosGene->VScrollValue = (int)gtk_adjustment_get_value( ScrollBar );
	return TRUE;
}
static gint HScrollBar_value_changed_event( GtkAdjustment * ScrollBar, void * not_used )
{
	InfosGene->HScrollValue = (int)gtk_adjustment_get_value( ScrollBar );
	return TRUE;
}

// function called for keys up/down and mouse scroll with increment height (positive or negative)...
// if increment=0, just update display with new value modified before.
static void IncrementVScrollBar( int IncrementValue )
{
//printf("%s(): inc=%d\n", __FUNCTION__, IncrementValue );
	if ( IncrementValue!=0 )
	{
		gtk_adjustment_set_value( AdjustVScrollBar, gtk_adjustment_get_value(AdjustVScrollBar)+IncrementValue );
		if ( IncrementValue>0 )
		{
			if ( gtk_adjustment_get_value(AdjustVScrollBar) > (gtk_adjustment_get_upper(AdjustVScrollBar)-InfosGene->PageHeight) )
				gtk_adjustment_set_value( AdjustVScrollBar, gtk_adjustment_get_upper(AdjustVScrollBar)-InfosGene->PageHeight );
		}
		else
		{
			if ( gtk_adjustment_get_value(AdjustVScrollBar) < gtk_adjustment_get_lower(AdjustVScrollBar) )
				gtk_adjustment_set_value( AdjustVScrollBar, gtk_adjustment_get_lower(AdjustVScrollBar) );
		}
	}
	gtk_adjustment_changed( AdjustVScrollBar );
	InfosGene->OffsetHiddenTopRungDisplayed	= gtk_adjustment_get_value(AdjustVScrollBar);
	VScrollBar_value_changed_event( AdjustVScrollBar, 0 );
}

static gboolean mouse_scroll_event( GtkWidget *widget, GdkEventScroll *event )
{
	int iCurrentLanguage = SectionArray[ InfosGene->CurrentSection ].Language;
	if ( iCurrentLanguage==SECTION_IN_LADDER )
	{
		if (event->direction == GDK_SCROLL_DOWN)
			IncrementVScrollBar( gtk_adjustment_get_step_increment(AdjustVScrollBar) );
		else  if (event->direction == GDK_SCROLL_UP )
			IncrementVScrollBar( -1*gtk_adjustment_get_step_increment(AdjustVScrollBar) );
	}
	return TRUE;
}
static gboolean key_press_event( GtkWidget *widget, GdkEventKey *event )
{
	GtkWindow *window = GTK_WINDOW (widget);
	gboolean handled = FALSE;

	int iCurrentLanguage = SectionArray[ InfosGene->CurrentSection ].Language;
	if ( iCurrentLanguage==SECTION_IN_LADDER )
	{
		int Increment = gtk_adjustment_get_page_increment(AdjustVScrollBar);
		switch (event->keyval)
		{
			case GDK_KEY_Down:
				Increment = gtk_adjustment_get_step_increment(AdjustVScrollBar); //no break here!
			case GDK_KEY_Page_Down:
				IncrementVScrollBar( Increment );
			return TRUE;
			case GDK_KEY_End:
				gtk_adjustment_set_value( AdjustVScrollBar, gtk_adjustment_get_upper(AdjustVScrollBar)-InfosGene->PageHeight );
				IncrementVScrollBar( 0 );
			return TRUE;
			case GDK_KEY_Up:
				Increment = gtk_adjustment_get_step_increment(AdjustVScrollBar); //no break here!
			case GDK_KEY_Page_Up:
				Increment = Increment *-1;
				IncrementVScrollBar( Increment );
			return TRUE;
			case GDK_KEY_Home:
				gtk_adjustment_set_value( AdjustVScrollBar, gtk_adjustment_get_lower(AdjustVScrollBar) );
				IncrementVScrollBar( 0 );
			return TRUE;
		}
		// beware that keys choosed must not be used in "edit" cells...
		if (EditDatas.ModeEdit)
		{
			// 'Alt'-xx not used, and when focus on Properties window and 'Alt' pressed, give focus back to main window for here!
			if (event->state & GDK_MOD1_MASK) //ALT+key
			{
				switch (event->keyval)
				{
					case GDK_KEY_i:  // Input, contact
							SelectToolOpenContact( );
					return TRUE;
					case GDK_KEY_o:  // Output, coil
						SelectToolCoil( );
					return TRUE;
					case GDK_KEY_h:
						SelectToolDrawHLine( );
					return TRUE;
					case GDK_KEY_l:
						SelectToolDrawHLineToEnd( );
					return TRUE;
					case GDK_KEY_v:
						SelectToolConnectVLine( );
					return TRUE;
					case GDK_KEY_c:
						ButtonCancelCurrentRung( );
					return TRUE;
					case GDK_KEY_x:
						SelectToolEraser( );
					return TRUE;
					case GDK_KEY_Return:
						ButtonOkCurrentRung( );
					return TRUE;
					case GDK_KEY_p:
						SelectToolPointer( );
					return TRUE;
				}
			}
		}
		else
		{
			if (event->state & GDK_MOD1_MASK) //ALT+key
			{
				switch (event->keyval)
				{
					case GDK_KEY_m:
						ButtonModifyCurrentRung( );
					return TRUE;
					case GDK_KEY_a:
						ButtonAddRung( );
					return TRUE;
					case GDK_KEY_i:
						ButtonInsertRung( );
					return TRUE;
					case GDK_KEY_x:
						ButtonDeleteCurrentRung( );
					return TRUE;
				}
			}
		}
	}
	/* handle mnemonics and accelerators */
	if (!handled) handled = gtk_window_activate_key (window, event);
	/* handle focus widget key events */
	if (!handled) handled = gtk_window_propagate_key_event (window, event);

	printf("KEY PRESS MAIN %x %d\n", event->keyval, handled); 
	return TRUE;
}

static gboolean button_press_event( GtkWidget *widget, GdkEventButton *event )
{
	char SelectCurrentRung = FALSE;
	char DisplayPopup = FALSE;
	int iCurrentLanguage = SectionArray[ InfosGene->CurrentSection ].Language;
	if (event->button == 1 /*Cairo && pixmap != NULL*/)
	{
		if (EditDatas.ModeEdit)
		{
			EditElementInThePage(event->x,event->y);
		}
		else
		{
			SelectCurrentRung = TRUE;
		}
	}
	else  if (event->button == 3 /*right click*/)
	{
		if (!EditDatas.ModeEdit)
			SelectCurrentRung = TRUE;
		DisplayPopup = TRUE;
	}

	if ( SelectCurrentRung )
	{
		// we can select the current rung by clicking on one.
		// the current rung is the one that will be modified...
		if ( iCurrentLanguage==SECTION_IN_LADDER )
		{
			char DoSelection = TRUE;
			if ( InfosGene->OffsetHiddenTopRungDisplayed>0 )
			{
//////test					if ( event->y<TOTAL_PX_RUNG_HEIGHT-InfosGene->OffsetHiddenTopRungDisplayed )
//////test						DoSelection = FALSE;
			}
			if ( DoSelection )
			{
				int NbrRungsShift =  (event->y+InfosGene->OffsetHiddenTopRungDisplayed)/TOTAL_PX_RUNG_HEIGHT;
//printf("=> Select the current rung clicked, with a shift of rungs=%d\n", NbrRungsShift );
				ChoiceOfTheCurrentRung( NbrRungsShift );
				CalcOffsetCurrentRungDisplayed( );
//DisplayedNowDirectlyOnMotion					MessageInStatusBar( GetLadderElePropertiesForStatusBar( event->x,event->y ) );
			}
		}
	}
	if ( DisplayPopup )
	{
		if (EditDatas.ModeEdit)
		{
			if ( iCurrentLanguage==SECTION_IN_LADDER )
				gtk_menu_popup (GTK_MENU(gtk_ui_manager_get_widget( uiManager, "/PopUpEditLadder")), NULL, NULL, NULL, NULL, event->button, event->time);
			else
				gtk_menu_popup (GTK_MENU(gtk_ui_manager_get_widget( uiManager, "/PopUpEditSequential")), NULL, NULL, NULL, NULL, event->button, event->time);
		}
		else
		{
			if ( iCurrentLanguage==SECTION_IN_LADDER )
				gtk_menu_popup (GTK_MENU(gtk_ui_manager_get_widget( uiManager, "/PopUpMenuLadder")), NULL, NULL, NULL, NULL, event->button, event->time);
			else
				gtk_menu_popup (GTK_MENU(gtk_ui_manager_get_widget( uiManager, "/PopUpMenuSequential")), NULL, NULL, NULL, NULL, event->button, event->time);
		}
	}
	return TRUE;
}
static gboolean motion_notify_event( GtkWidget *widget, GdkEventMotion *event, gpointer user_data )
{
	//v0.9.20
	if( SectionArray==NULL )
		return TRUE;
		
	if (EditDatas.ModeEdit)
	{
		MouseMotionOnThePage( event->x, event->y );
	}
	else
	{
		if ( SectionArray[ InfosGene->CurrentSection ].Language==SECTION_IN_LADDER )
		{
			char * pLadderProperties = GetLadderElePropertiesForStatusBar( event->x,event->y );
			if ( pLadderProperties )
				MessageInStatusBar( pLadderProperties );
		}
	}
	return TRUE;
}
static gboolean button_release_event( GtkWidget *widget, GdkEventButton *event )
{
	if (event->button == 1 /*Cairo && pixmap != NULL*/)
	{
		if (EditDatas.ModeEdit)
			EditButtonReleaseEventOnThePage( );
	}
	return TRUE;
}

void RefreshLabelCommentEntries( void )
{
	int iCurrentLanguage = SectionArray[ InfosGene->CurrentSection ].Language;
	if ( iCurrentLanguage==SECTION_IN_LADDER )
	{
		StrRung * RfhRung = &RungArray[InfosGene->CurrentRung];
		gtk_entry_set_text(GTK_ENTRY(entrylabel),RfhRung->Label);
		gtk_widget_show( entrylabel );
		gtk_entry_set_text(GTK_ENTRY(entrycomment),RfhRung->Comment);
	}
#ifdef SEQUENTIAL_SUPPORT
	else if ( iCurrentLanguage==SECTION_IN_SEQUENTIAL )
	{
		int CurrentSeqPage = SectionArray[ InfosGene->CurrentSection ].SequentialPage;
		gtk_widget_hide( entrylabel );
		gtk_entry_set_text(GTK_ENTRY(entrylabel),"");
		gtk_entry_set_text(GTK_ENTRY(entrycomment),Sequential->PageComment[CurrentSeqPage]);
	}
#endif
}
void ClearLabelCommentEntries()
{
	gtk_entry_set_text(GTK_ENTRY(entrylabel),"");
	gtk_entry_set_text(GTK_ENTRY(entrycomment),"");
}
void SaveLabelCommentEntriesEdited()
{
	int iCurrentLanguage = SectionArray[ InfosGene->CurrentSection ].Language;
	if ( iCurrentLanguage==SECTION_IN_LADDER )
	{
		strncpy(EditDatas.Rung.Label,gtk_entry_get_text(GTK_ENTRY(entrylabel)),LGT_LABEL-1);
		EditDatas.Rung.Label[ LGT_LABEL-1 ] = '\0';
		strncpy(EditDatas.Rung.Comment,gtk_entry_get_text(GTK_ENTRY(entrycomment)),LGT_COMMENT-1);
		EditDatas.Rung.Comment[ LGT_COMMENT-1 ] = '\0';
	}
#ifdef SEQUENTIAL_SUPPORT
	else if ( iCurrentLanguage==SECTION_IN_SEQUENTIAL )
	{
		int CurrentSeqPage = SectionArray[ InfosGene->CurrentSection ].SequentialPage;
		strncpy(Sequential->PageComment[CurrentSeqPage],gtk_entry_get_text(GTK_ENTRY(entrycomment)),LGT_COMMENT-1);
		Sequential->PageComment[CurrentSeqPage][ LGT_COMMENT-1 ] = '\0';
	}
#endif
}
void AdjustLabelCommentEntriesToSection( int SectionLanguage )
{
printf("===>AdjustLabelCommentEntriesToSection %d\n", SectionLanguage);
	if (SectionLanguage==-1)
	{
		gtk_widget_set_sensitive(VScrollBar, TRUE);
		gtk_widget_set_sensitive(entrylabel, FALSE);
		gtk_widget_set_sensitive(entrycomment, FALSE);
	}
	else
	{
		gtk_widget_set_sensitive(VScrollBar, FALSE);
		gtk_widget_set_sensitive(entrylabel, SectionLanguage!=SECTION_IN_SEQUENTIAL);
		if( SectionLanguage!=SECTION_IN_SEQUENTIAL )
			gtk_widget_show( entrylabel );
		else
			gtk_widget_hide( entrylabel );
		gtk_widget_set_sensitive(entrycomment, TRUE);
	}
}

void CheckDispSymbols_toggled( )
{
	Preferences.DisplaySymbolsInMainWindow/*InfosGene->DisplaySymbols*/ = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( CheckDispSymbols ) );
}


void StoreDirectorySelected( GtkFileChooser *selector, char cForLoadingProject)
{
	char * TempDir;

	TempDir = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER(FileSelector));
	if ( cForLoadingProject )
		VerifyDirectorySelected( InfosGene->CurrentProjectFileName, TempDir );
	else
		strcpy( InfosGene->CurrentProjectFileName, TempDir );
}


void LoadNewLadder()
{
	char ProjectLoadedOk;
	StoreDirectorySelected( GTK_FILE_CHOOSER(FileSelector) , TRUE/*cForLoadingProject*/);
	
	if (InfosGene->LadderState==STATE_RUN)
	{
		DoFlipFlopRunStop( );
	}
//v0.9.20 moved in LoadProjectFiles()	InfosGene->LadderState = STATE_LOADING;
	ProjectLoadedOk = LoadProjectFiles( InfosGene->CurrentProjectFileName );
	if ( !ProjectLoadedOk )
		ShowMessageBox( _("Load Error"), _("Failed to load the project file..."), _("Ok") );

	UpdateAllGtkWindows( );
	UpdateWindowTitleWithProjectName( );
	MessageInStatusBar( ProjectLoadedOk?_("Project loaded (stopped)."):_("Project failed to load..."));
//v0.9.20 moved in LoadProjectFiles()	InfosGene->LadderState = STATE_STOP;
	if ( ProjectLoadedOk )
		HardwareActionsAfterProjectLoaded( );
//added here in 0.9.10???, really right place?
//removed in v0.9.9 ConfigSerialModbusMaster( );
}
void DoTheSave()
{
	if ( InfosGene->CurrentProjectFileName[0]=='\0' )
	{
		DoActionSaveAs( );
	}
	else
	{
		if ( InfosGene->AskConfirmationToQuit )
		{
			char BuffCurrTime[ 30 ];
			GetCurrentAscTime( BuffCurrTime );
			strcpy( InfosGene->ProjectProperties.ParamModifDate, BuffCurrTime );
			if ( InfosGene->ProjectProperties.ParamVersion[ 0 ]>='0' && InfosGene->ProjectProperties.ParamVersion[ 0 ]<='9' )
			{
				int version = 0;
				version = atoi( InfosGene->ProjectProperties.ParamVersion );
				version++;
				sprintf( InfosGene->ProjectProperties.ParamVersion, "%d", version );
			}
		}
		if ( !SaveProjectFiles( InfosGene->CurrentProjectFileName ) )
		{
			ShowMessageBox( _("Save Error"), _("Failed to save the project file..."), _("Ok") );
		}
		else
		{
			UpdateWindowTitleWithProjectName( );
			DisplayProjectProperties( );
		}
	}
}
void DoActionSave()
{
	if (EditDatas.ModeEdit==TRUE )
		ShowConfirmationBox( _("Warning!"), _("You are currently under edit.\nYou should apply current modifications before...\nDo you really want to save now ?\n"), DoTheSave );
	else
		DoTheSave( );
}

void SaveAsLadder(void)
{
	StoreDirectorySelected( GTK_FILE_CHOOSER(FileSelector), FALSE/*cForLoadingProject*/);
	DoActionSave( );
//	if ( !SaveProjectFiles( InfosGene->CurrentProjectFileName ) )
//		ShowMessageBox( "Save Error", "Failed to save the project file...", "Ok" );
//	UpdateWindowTitleWithProjectName( );
}

void on_filechooserdialog_save_response(GtkDialog  *dialog,gint response_id,gpointer user_data)
{
	debug_printf("SAVE %s %d\n",gtk_file_chooser_get_filename (GTK_FILE_CHOOSER(FileSelector)),response_id);

	if(response_id==GTK_RESPONSE_ACCEPT || response_id==GTK_RESPONSE_OK)
		SaveAsLadder();
	gtk_widget_destroy(GTK_WIDGET(dialog));
}
void on_filechooserdialog_load_response(GtkDialog  *dialog,gint response_id,gpointer user_data)
{
	debug_printf("LOAD %s %d\n",gtk_file_chooser_get_filename (GTK_FILE_CHOOSER(FileSelector)),response_id);

	if(response_id==GTK_RESPONSE_ACCEPT || response_id==GTK_RESPONSE_OK)
		LoadNewLadder();
	gtk_widget_destroy(GTK_WIDGET(dialog));
}
void on_filechooserdialog_selected_soft_response(GtkDialog  *dialog,gint response_id,gpointer user_data)
{
	char GoOn = FALSE;
	debug_printf("SOFT SELECTED %s %d\n",gtk_file_chooser_get_filename (GTK_FILE_CHOOSER(FileSelector)),response_id);

	if(response_id==GTK_RESPONSE_ACCEPT || response_id==GTK_RESPONSE_OK)
	{
		GoOn = TRUE;
		strcpy( InfosGUI->TargetMonitor.TransferFileSelectedName, gtk_file_chooser_get_filename (GTK_FILE_CHOOSER(FileSelector)) );
	}
	gtk_widget_destroy(GTK_WIDGET(dialog));
	if ( GoOn )
		NextStepBeforeSendingUpdateSoftToTarget();
}


void CreateFileSelection(char * Prompt,int CreateFileSelectionType)
{
	/* Create the selector */
	GtkFileFilter *FilterOldProjects, *FilterProjects;
	if(CreateFileSelectionType==CREATE_FILE_SELECTION_TO_SAVE_PROJECT)
	{
		FileSelector = gtk_file_chooser_dialog_new (Prompt, NULL, GTK_FILE_CHOOSER_ACTION_SAVE,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
		gtk_file_chooser_set_do_overwrite_confirmation( GTK_FILE_CHOOSER(FileSelector), TRUE );
	}
	else
	{
		FileSelector = gtk_file_chooser_dialog_new (Prompt, NULL, GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
	}
	gtk_window_set_type_hint (GTK_WINDOW (FileSelector), GDK_WINDOW_TYPE_HINT_DIALOG);

	if( CreateFileSelectionType==CREATE_FILE_SELECTION_TO_SELECT_UPDATE_SOFT )
	{
		FilterProjects = gtk_file_filter_new( );
		gtk_file_filter_set_name( FilterProjects, _("ClassicLadder softs archives") );
		gtk_file_filter_add_pattern( FilterProjects, "classicladder_embedded*.tar.gz" );
		gtk_file_chooser_add_filter( GTK_FILE_CHOOSER(FileSelector), FilterProjects );
		gtk_file_chooser_set_filter( GTK_FILE_CHOOSER(FileSelector), FilterProjects );
	}
	else
	{
		FilterOldProjects = gtk_file_filter_new( );
		gtk_file_filter_set_name( FilterOldProjects, _("Old directories projects") );
		gtk_file_filter_add_pattern( FilterOldProjects, "*.csv" ); // old dir projects
		gtk_file_chooser_add_filter( GTK_FILE_CHOOSER(FileSelector), FilterOldProjects );
		FilterProjects = gtk_file_filter_new( );
		gtk_file_filter_set_name( FilterProjects, _("ClassicLadder projects") );
		gtk_file_filter_add_pattern( FilterProjects, "*.clprj" );
		gtk_file_filter_add_pattern( FilterProjects, "*.clp" );
		gtk_file_filter_add_pattern( FilterProjects, "*.clprjz" );
		gtk_file_chooser_add_filter( GTK_FILE_CHOOSER(FileSelector), FilterProjects );
		gtk_file_chooser_set_filter( GTK_FILE_CHOOSER(FileSelector), FilterProjects );
	}

	gtk_window_set_modal(GTK_WINDOW(FileSelector), TRUE );

/*
  g_signal_connect ((gpointer) filechooserdialog, "file_activated",
					G_CALLBACK (on_filechooserdialog_file_activated),
					NULL);
					*/

	if( CreateFileSelectionType==CREATE_FILE_SELECTION_TO_SELECT_UPDATE_SOFT )
		g_signal_connect ((gpointer) FileSelector, "response",
					G_CALLBACK (on_filechooserdialog_selected_soft_response),
					NULL);
	else if( CreateFileSelectionType==CREATE_FILE_SELECTION_TO_SAVE_PROJECT )
		g_signal_connect ((gpointer) FileSelector, "response",
					G_CALLBACK (on_filechooserdialog_save_response),
					NULL);
	else
		g_signal_connect ((gpointer) FileSelector, "response",
					G_CALLBACK (on_filechooserdialog_load_response),
					NULL);

	g_signal_connect_swapped ((gpointer) FileSelector, "close",
							G_CALLBACK (gtk_widget_destroy),
							GTK_OBJECT (FileSelector));

	/* Display that dialog */
	gtk_widget_show (FileSelector);
}

void DoNewProject( void )
{
	if (InfosGene->LadderState==STATE_RUN)
	{
		DoFlipFlopRunStop( );
	}
//v0.9.20	ClassicLadder_InitProjectDatas( );
	ClassicLadder_AllocNewProjectDatas( );
	UpdateAllGtkWindows( );
	UpdateWindowTitleWithProjectName( );
}

void DoActionConfirmNewProject()
{
	if ( InfosGUI->TargetMonitor.RemoteConnected )
		ShowMessageBoxError( _("Not possible when connected to a remote target...") );
	else
		ShowConfirmationBox(_("New"),_("Do you really want to clear all datas ?"),DoNewProject);
}
void DoLoadProject()
{
	CreateFileSelection(_("Please select the project to load"),CREATE_FILE_SELECTION_TO_LOAD_PROJECT);
}

void DoActionLoadProject()
{
	if ( InfosGUI->TargetMonitor.RemoteConnected )
		ShowMessageBoxError( _("Not possible when connected to a remote target...") );
	else if ( InfosGene->AskConfirmationToQuit )
		ShowConfirmationBox( _("Sure?"), _("Do you really want to load another project ?\nIf not saved, all modifications on the current project will be lost  \n"), DoLoadProject );
	else
		DoLoadProject( );
}

void DoActionSaveAs()
{
	CreateFileSelection(_("Please select the project to save"),CREATE_FILE_SELECTION_TO_SAVE_PROJECT);
}

void DoActionResetAndConfirmIfRunning( )
{
	if ( ( InfosGene->LadderState==STATE_RUN && !InfosGUI->TargetMonitor.RemoteConnected ) || ( InfosGUI->TargetMonitor.LatestTargetState==STATE_RUN && InfosGUI->TargetMonitor.RemoteConnected ) )
		ShowConfirmationBox(_("Warning!"),_("Resetting a running program\ncan cause unexpected behavior\n Do you really want to reset?"),DoReset);
	else
		DoReset();
}

void DoActionAboutClassicLadder()
{
	/*
	// From the example in gtkdialog help
	GtkWidget *dialog, *label, *okay_button;
	// Create the widgets
	dialog = gtk_dialog_new();
	label = gtk_label_new ( CL_PRODUCT_NAME " v" CL_RELEASE_VER_STRING "\n" CL_RELEASE_DATE_STRING "\n"
						"Copyright (C) " CL_RELEASE_COPYRIGHT_YEARS " Marc Le Douarain\nmarc . le - douarain /At\\ laposte \\DoT/ net\n"
						"http://www.sourceforge.net/projects/classicladder\n"
						"http://membres.lycos.fr/mavati/classicladder\n"
						"Released under the terms of the\nGNU Lesser General Public License v3");
	gtk_label_set_justify( GTK_LABEL(label), GTK_JUSTIFY_CENTER );
	okay_button = gtk_button_new_with_label("Okay");
	// Ensure that the dialog box is destroyed when the user clicks ok.
	gtk_signal_connect_object (GTK_OBJECT (okay_button), "clicked",
							GTK_SIGNAL_FUNC (gtk_widget_destroy), GTK_OBJECT(dialog));
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->action_area),
					okay_button);
	gtk_widget_grab_focus(okay_button);
	// Add the label, and show everything we've added to the dialog.
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox),
					label);
	gtk_window_set_modal(GTK_WINDOW(dialog),TRUE);
	gtk_window_set_position(GTK_WINDOW(dialog),GTK_WIN_POS_CENTER);
	gtk_widget_show_all (dialog);
*/
	char * static_comments = _("Released under the terms of the\nGNU Lesser General Public License v3\n\n"
							"Written by Marc Le Douarain\n"
							"and including contributions made by Chris Morley, Heli Tejedor, Dave Gamble (cJSON), Bernard Chardonneau (base64 transfer) and others\n\n"
							"Latest software version available at:\n");
	char * comments = malloc( strlen( static_comments )+150 );
	if ( comments )
	{
		char GtkVersionString[ 30 ];
		sprintf( GtkVersionString, "GTK+ version %d.%d.%d\n\n", gtk_major_version, gtk_minor_version, gtk_micro_version );
		strcpy( comments, "("CL_RELEASE_DATE_STRING")\n\n" );
		strcat( comments, GtkVersionString );
		strcat( comments, static_comments );
		strcat( comments, "http://www.sourceforge.net/projects/classicladder\n" );
		gtk_show_about_dialog ( GTK_WINDOW( MainSectionWindow ),
							"program-name", CL_PRODUCT_NAME ,
							"version", CL_RELEASE_VER_STRING ,
							"copyright", "Copyright (C) " CL_RELEASE_COPYRIGHT_YEARS " Marc Le Douarain\nmarc . le - douarain /At\\ laposte \\DoT/ net" ,
//							"logo", example_logo,
							"title", _("About ClassicLadder"),
							"website", "http://sites.google.com/site/classicladder" ,
							"comments", comments ,
					   NULL );
		free( comments );
	}
}


cairo_surface_t *ExportSurface;
cairo_t * InitExportSurface( int SurfaceWidth, int SurfaceHeight, char * SvgFileToCreate )
{
	cairo_t *cr;
	if ( SvgFileToCreate )
	{
		ExportSurface = cairo_svg_surface_create( SvgFileToCreate, SurfaceWidth, SurfaceHeight );
		cr = cairo_create( ExportSurface );
	}
	else
	{
		ExportSurface = cairo_image_surface_create( CAIRO_FORMAT_RGB24, SurfaceWidth, SurfaceHeight );
		cr = cairo_create( ExportSurface );
		//cleanup
		cairo_set_source_rgb( cr, 1.0, 1.0, 1.0 );
		cairo_paint( cr );
	}
	return cr;	
}

void ExportSvgOrPngFile( char * FileToCreate, char GoForSvgExport )
{
	cairo_t *cr;
	int iCurrentLanguage = SectionArray[ InfosGene->CurrentSection ].Language;
	if ( iCurrentLanguage==SECTION_IN_LADDER )
	{
		int SvgWidthTotal = RUNG_WIDTH*BLOCK_WIDTH_DEF+30;
		int SvgHeightTotal = RUNG_HEIGHT*BLOCK_HEIGHT_DEF+BLOCK_HEIGHT_DEF/2;
		int LeftRightBarsWidth = BLOCK_WIDTH_DEF/16;
		cr = InitExportSurface( SvgWidthTotal, SvgHeightTotal, GoForSvgExport?FileToCreate:NULL );
		DrawLeftRightBars( cr, 0, 0, BLOCK_WIDTH_DEF, BLOCK_HEIGHT_DEF, BLOCK_HEIGHT_DEF/2, LeftRightBarsWidth, FALSE );
		DrawRung( cr, InfosGene->CurrentRung, FALSE/*InEdit*/, LeftRightBarsWidth, 0, BLOCK_WIDTH_DEF, BLOCK_HEIGHT_DEF, BLOCK_HEIGHT_DEF/2, DRAW_FOR_PRINT );
	}
	if ( iCurrentLanguage==SECTION_IN_SEQUENTIAL )
	{
		int SvgWidthTotal = SEQ_PAGE_WIDTH*SEQ_SIZE_DEF;
		int SvgHeightTotal = SEQ_PAGE_HEIGHT*SEQ_SIZE_DEF;
		cr = InitExportSurface(  SvgWidthTotal, SvgHeightTotal, GoForSvgExport?FileToCreate:NULL );
		DrawSequentialPage( cr, SectionArray[ InfosGene->CurrentSection ].SequentialPage, SEQ_SIZE_DEF, DRAW_FOR_PRINT );
	}
	if ( !GoForSvgExport )
		cairo_surface_write_to_png( ExportSurface, FileToCreate );
	cairo_surface_destroy( ExportSurface );
	cairo_destroy( cr );
}
void FileRequestToExportSvgOrPng(char GoForSvg)
{
	GtkWidget *dialog;
	dialog = gtk_file_chooser_dialog_new (GoForSvg?_("Save SVG File"):_("Save PNG File"),
										GTK_WINDOW(MainSectionWindow),
										GTK_FILE_CHOOSER_ACTION_SAVE,
										GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
										GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
										NULL);
	gtk_file_chooser_set_do_overwrite_confirmation( GTK_FILE_CHOOSER (dialog), TRUE );
	gtk_file_chooser_set_current_name( GTK_FILE_CHOOSER (dialog), GoForSvg?"classicladder_export.svg":"classicladder_export.png" );
	if ( gtk_dialog_run( GTK_DIALOG (dialog) ) == GTK_RESPONSE_ACCEPT )
	{
		char *filename;
		filename = gtk_file_chooser_get_filename( GTK_FILE_CHOOSER(dialog) );
		ExportSvgOrPngFile( filename, GoForSvg );
		g_free( filename );
	}
	gtk_widget_destroy( dialog );
}
void DoActionExportSvg( void )
{
	FileRequestToExportSvgOrPng( TRUE );
}
void DoActionExportPng( void )
{
	FileRequestToExportSvgOrPng( FALSE );
}
// use a temp .png file to make it... (Cairo Surface -> png file -> pixbuf -> clipboard)
void DoActionCopyToClipboard( void )
{
	GError * err = NULL;
	GtkClipboard * pClipboard = gtk_clipboard_get( GDK_SELECTION_CLIPBOARD );
	ExportSvgOrPngFile( "cl_clipboard_tmp.png", FALSE/*GoForSvgExport*/ );
printf("Creating gdk pixpuf from tmp png file\n");
	GdkPixbuf * pPixBuf = gdk_pixbuf_new_from_file( "cl_clipboard_tmp.png", &err );
	if ( pPixBuf )
	{
		remove( "cl_clipboard_tmp.png" );
printf("Set pixbuf image to clipboard\n");
		gtk_clipboard_set_image( pClipboard, pPixBuf );
		g_object_unref( pPixBuf );
	}
	else
	{
		printf("Error clipboard_set_image() : %s\n", err->message);
	}
}


void ShowMessageBox(const char * title, const char * text, const char * button)
{
	/* From the example in gtkdialog help */
	GtkWidget *dialog, *label, *okay_button;
	/* Create the widgets */
	dialog = gtk_dialog_new();
	label = gtk_label_new (text);
	okay_button = gtk_button_new_with_label(button);
	/* Ensure that the dialog box is destroyed when the user clicks ok. */
//ForGTK3	gtk_signal_connect_object (GTK_OBJECT (okay_button), "clicked",
//ForGTK3							GTK_SIGNAL_FUNC (gtk_widget_destroy), GTK_OBJECT(dialog));
	g_signal_connect_swapped(GTK_OBJECT (okay_button), "clicked",
							GTK_SIGNAL_FUNC (gtk_widget_destroy), GTK_OBJECT(dialog));
//ForGTK3	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->action_area), okay_button);
	gtk_container_add (GTK_CONTAINER (gtk_dialog_get_action_area(GTK_DIALOG(dialog))), okay_button);
	gtk_widget_grab_focus(okay_button);
	/* Add the label, and show everything we've added to the dialog. */
//ForGTK3	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), label);
	gtk_container_add (GTK_CONTAINER (gtk_dialog_get_content_area(GTK_DIALOG(dialog))), label);
	gtk_window_set_modal(GTK_WINDOW(dialog),TRUE);
	gtk_window_set_title(GTK_WINDOW(dialog),title);
	gtk_window_set_position(GTK_WINDOW(dialog),GTK_WIN_POS_CENTER);
	gtk_widget_show_all (dialog);
}
void ShowMessageBoxError( const char * text )
{
	ShowMessageBox( _("Error"), text, _("Ok") );
}

void DoFunctionOfConfirmationBox( GtkWidget *widget, void * (*function_to_do)(void *) )
{
	gtk_widget_destroy(ConfirmDialog);
	(function_to_do)(NULL);
}
void ShowConfirmationBoxWithChoiceOrNot(const char * title,const char * text,void * function_if_yes, char HaveTheChoice)
{
	/* From the example in gtkdialog help */
	GtkWidget *label, *yes_button, *no_button;
	/* Create the widgets */
	ConfirmDialog = gtk_dialog_new();
	label = gtk_label_new (text);
	if ( HaveTheChoice )
	{
		yes_button = gtk_button_new_with_label(_("Yes"));
		no_button = gtk_button_new_with_label(_("No"));
	}
	else
	{
		yes_button = gtk_button_new_with_label(_("Ok"));
	}
	/* Ensure that the dialog box is destroyed when the user clicks ok. */
	if ( HaveTheChoice )
	{
//ForGTK3		gtk_signal_connect_object (GTK_OBJECT (no_button), "clicked",
//ForGTK3							GTK_SIGNAL_FUNC (gtk_widget_destroy), GTK_OBJECT(ConfirmDialog));
		g_signal_connect_swapped(GTK_OBJECT (no_button), "clicked",
							GTK_SIGNAL_FUNC (gtk_widget_destroy), GTK_OBJECT(ConfirmDialog));
//ForGTK3		gtk_container_add (GTK_CONTAINER (GTK_DIALOG(ConfirmDialog)->action_area), no_button);
		gtk_container_add (GTK_CONTAINER (gtk_dialog_get_action_area(GTK_DIALOG(ConfirmDialog))), no_button);
		gtk_widget_grab_focus(no_button);
	}
	gtk_signal_connect(GTK_OBJECT (yes_button), "clicked",
							GTK_SIGNAL_FUNC (DoFunctionOfConfirmationBox), function_if_yes);
//ForGTK3	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(ConfirmDialog)->action_area), yes_button);
	gtk_container_add (GTK_CONTAINER (gtk_dialog_get_action_area(GTK_DIALOG(ConfirmDialog))), yes_button);
	/* Add the label, and show everything we've added to the dialog. */
//ForGTK3	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(ConfirmDialog)->vbox), label);
	gtk_container_add (GTK_CONTAINER (gtk_dialog_get_content_area(GTK_DIALOG(ConfirmDialog))), label);
	gtk_window_set_modal(GTK_WINDOW(ConfirmDialog),TRUE);
	gtk_window_set_title(GTK_WINDOW(ConfirmDialog),title);
	gtk_window_set_position(GTK_WINDOW(ConfirmDialog),GTK_WIN_POS_CENTER);
	gtk_widget_show_all (ConfirmDialog);
}
void ShowConfirmationBox(const char * title,const char * text,void * function_if_yes)
{
	ShowConfirmationBoxWithChoiceOrNot( title, text, function_if_yes, TRUE );
}

// StoreStringResult must be initialized (content will be displayed at startup, can be "") !
char ShowEnterMessageBox( const char * title, const char * text, char * StoreStringResult, int LengthStringResult )
{
	GtkWidget* pDialogBox;
	GtkWidget* pEntry;
	char OkDone = FALSE;

	pDialogBox = gtk_dialog_new_with_buttons(title,
		GTK_WINDOW( MainSectionWindow ),
		GTK_DIALOG_MODAL,
		GTK_STOCK_OK,GTK_RESPONSE_OK,
		GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,
		NULL);
	
	if ( text )
	{
		GtkWidget * pLabel = gtk_label_new( text );
//ForGTK3		gtk_box_pack_start(GTK_BOX(GTK_DIALOG(pDialogBox)->vbox), pLabel, TRUE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(pDialogBox))), pLabel, TRUE, FALSE, 0);
	}
	pEntry = gtk_entry_new();
	gtk_entry_set_text( GTK_ENTRY(pEntry), StoreStringResult );
//ForGTK3	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(pDialogBox)->vbox), pEntry, TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(pDialogBox))), pEntry, TRUE, FALSE, 0);
//ForGTK3	gtk_widget_show_all(GTK_DIALOG(pDialogBox)->vbox);
	gtk_widget_show_all(gtk_dialog_get_content_area(GTK_DIALOG(pDialogBox)));

	switch (gtk_dialog_run(GTK_DIALOG(pDialogBox)))
	{
		case GTK_RESPONSE_OK:
			strncpy( StoreStringResult, gtk_entry_get_text(GTK_ENTRY(pEntry) ), LengthStringResult );
			StoreStringResult[ LengthStringResult-1 ] = '\0';
			OkDone = TRUE;
			break;
	}

	gtk_widget_destroy(pDialogBox);
	return OkDone;
}

void ShowMessageBoxInBackground( char * Title, char * Message )
{
	if ( Title )
		strcpy( InfosGene->ErrorTitleStringToDisplay, Title );
	strcpy( &InfosGene->ErrorMsgStringToDisplay[1], &Message[1] );
	// First char tested in multitask, so copied the latest to be sure !
	InfosGene->ErrorMsgStringToDisplay[ 0 ] = Message[ 0 ];
}

void QuitAppliGtk()
{
	int NumFramesLogWindow;
	printf("Quit GTK application...\n");
	// look all window open/closed states + posis/sizes if opened (to restore them at next startup !)
RememberManagerWindowPrefs( );//not open-close state for this window, see TODO note in function...
	RememberEditWindowPrefs( );
	RememberSymbolsWindowPrefs( );
	RememberBoolVarsWindowPrefs( );
	RememberFreeVarsWindowPrefs( );
	RememberLogBookWindowPrefs( );
	for( NumFramesLogWindow=0; NumFramesLogWindow<NBR_FRAMES_LOG_WINDOWS; NumFramesLogWindow++ )
	{
		char Buff[ 30 ];
		sprintf( Buff, "FramesLogWindow%d", NumFramesLogWindow );
		RememberMonitorWindowPrefs( NumFramesLogWindow );
	}
	
	SaveSetsVarsList( );
	SaveLogEventsData( );
	
	//v0.9.20, save return code before freeing memory allocated !!!
	int ReturnCodeValue = InfosGene->HasBeenModifiedForExitCode?1:0;
	ClassicLadderEndOfAppli( );
	DestroyCustomIconsAndCursors( );
//ForGTK3	gtk_exit( ReturnCodeValue );
	exit( ReturnCodeValue );
}

void DoQuitGtkApplication( void )
{
	RememberWindowPosiPrefs( "Main", MainSectionWindow, TRUE/*SaveWindowSize*/ );
	gtk_widget_destroy( MainSectionWindow ); //sends signal "destroy" that will call QuitAppliGtk()...
}
void ConfirmQuit( void )
{
	if ( InfosGene->AskConfirmationToQuit )
		ShowConfirmationBox( _("Warning!"), _("If not saved, all modifications will be lost.\nDo you really want to quit ?\n"), DoQuitGtkApplication );
	else if (EditDatas.ModeEdit==TRUE )
		ShowConfirmationBox( _("Warning!"), _("You are currently under edit.\nDo you really want to quit ?\n"), DoQuitGtkApplication );
	else if ( InfosGUI->TargetMonitor.RemoteConnected )
		ShowConfirmationBox( _("Warning!"), _("You are currently connected to a target.\nDo you really want to quit ?\n"), DoQuitGtkApplication );	
	else
		DoQuitGtkApplication( );
}
gint MainSectionWindowDeleteEvent( GtkWidget * widget, GdkEvent * event, gpointer data )
{
	ConfirmQuit( );
	// we do not want that the window be destroyed.
	return TRUE;
}

gboolean MessageInStatusBar( char * msg )
{
	gtk_statusbar_pop(GTK_STATUSBAR(StatusBar), StatusBarContextId);
	if ( msg )
		gtk_statusbar_push(GTK_STATUSBAR(StatusBar), StatusBarContextId, msg);
	else
		gtk_statusbar_push(GTK_STATUSBAR(StatusBar), StatusBarContextId, "---");
	return FALSE; //usefull when called with g_idle_add (just one time)
}

/* =========================================================================*/
/* GTK SEARCH BOX ... */
/* =========================================================================*/
extern char *SearchTypesNames[NBR_SEARCH_TYPES];
void FunctionNewSearchData( void )
{
	char * VarOrNbr;
	int Idx;
	VarOrNbr = (char *)gtk_entry_get_text(GTK_ENTRY(SearchEntry));
	Idx = gtk_combo_box_get_active (GTK_COMBO_BOX(SearchType));
	SearchNewElement( Idx, VarOrNbr );
}
void FunctionSearchCloseBox( void )
{
	gtk_widget_hide(hBoxSearch);
	SearchDatas.ElementOrSearchType = ELE_FREE;
}
void FunctionSearchOpenBox( void )
{
	if ( EditDatas.ModeEdit )
	{
		ShowMessageBox( _("Search..."), _("Not available during edit..."), "Ok" );
	}
	else
	{
		gtk_widget_show(hBoxSearch);
		gtk_widget_grab_focus( SearchEntry );
	}
}
void SearchInitGtk(GtkBox *vbox)
{
	int Scan;
//ForGTK3, deprecated...	GtkTooltips * SearchEntryTooltip;

	hBoxSearch = gtk_hbox_new (FALSE,0);
	gtk_container_add (GTK_CONTAINER (vbox), hBoxSearch);
	gtk_box_set_child_packing(GTK_BOX(vbox), hBoxSearch,
		/*expand*/ FALSE, /*fill*/ FALSE, /*pad*/ 0, GTK_PACK_START);

	ButtonSearchClose = gtk_button_new();
	gtk_button_set_image( GTK_BUTTON( ButtonSearchClose ), gtk_image_new_from_stock( GTK_STOCK_CLOSE, GTK_ICON_SIZE_MENU/*GTK_ICON_SIZE_BUTTON*/ ) );
	gtk_box_pack_start( GTK_BOX (hBoxSearch), ButtonSearchClose, FALSE, FALSE, 0 );
	gtk_signal_connect( GTK_OBJECT (ButtonSearchClose), "clicked", GTK_SIGNAL_FUNC(FunctionSearchCloseBox), 0 );
	gtk_widget_show( ButtonSearchClose );

	SearchEntry = gtk_entry_new();
//	gtk_widget_set_usize((GtkWidget *)SearchEntry,65,0);
	gtk_box_pack_start (GTK_BOX (hBoxSearch), SearchEntry, FALSE, FALSE, 4);
	gtk_widget_show (SearchEntry);
	gtk_signal_connect(GTK_OBJECT (SearchEntry), "activate", GTK_SIGNAL_FUNC(FunctionNewSearchData), NULL);
//ForGTK3, deprecated...	SearchEntryTooltip = gtk_tooltips_new();
//ForGTK3, deprecated...	gtk_tooltips_set_tip( SearchEntryTooltip, SearchEntry, "Variable to search or block number", NULL );
	gtk_widget_set_tooltip_text( SearchEntry, _("Variable to search or block number") );

	SearchType = gtk_combo_box_new_text();
	for( Scan=0; Scan<NBR_SEARCH_TYPES; Scan++ )
		gtk_combo_box_append_text( MY_GTK_COMBO_BOX(SearchType), gettext(SearchTypesNames[Scan]) );
	gtk_box_pack_start(GTK_BOX (hBoxSearch), SearchType, FALSE, FALSE, 0);
	gtk_combo_box_set_active (GTK_COMBO_BOX(SearchType), 0);
	gtk_widget_show (SearchType);
	gtk_signal_connect(GTK_OBJECT (SearchType), "changed", GTK_SIGNAL_FUNC(FunctionNewSearchData), NULL);

	ButtonSearchNow = gtk_button_new();
	gtk_button_set_image( GTK_BUTTON( ButtonSearchNow ), gtk_image_new_from_stock( GTK_STOCK_FIND, GTK_ICON_SIZE_MENU/*GTK_ICON_SIZE_BUTTON*/ ) );
	gtk_box_pack_start (GTK_BOX (hBoxSearch), ButtonSearchNow, FALSE, FALSE, 0);
	gtk_signal_connect(GTK_OBJECT (ButtonSearchNow), "clicked", GTK_SIGNAL_FUNC(FunctionNewSearchData), 0);
	gtk_widget_set_sensitive( ButtonSearchNow, TRUE );
	gtk_widget_show( ButtonSearchNow );

	ButtonSearchNext = gtk_button_new();
	gtk_button_set_image( GTK_BUTTON( ButtonSearchNext ), gtk_image_new_from_stock( GTK_STOCK_GO_FORWARD, GTK_ICON_SIZE_MENU/*GTK_ICON_SIZE_BUTTON*/ ) );
	gtk_box_pack_start( GTK_BOX (hBoxSearch), ButtonSearchNext, FALSE, FALSE, 0 );
	gtk_signal_connect( GTK_OBJECT (ButtonSearchNext), "clicked", GTK_SIGNAL_FUNC(SearchAndGoToNextElement), 0 );
	gtk_widget_show( ButtonSearchNext );
	ButtonSearchPrev = gtk_button_new();
	gtk_button_set_image( GTK_BUTTON( ButtonSearchPrev ), gtk_image_new_from_stock( GTK_STOCK_GO_BACK, GTK_ICON_SIZE_MENU/*GTK_ICON_SIZE_BUTTON*/ ) );
	gtk_box_pack_start( GTK_BOX (hBoxSearch), ButtonSearchPrev, FALSE, FALSE, 0 );
	gtk_signal_connect( GTK_OBJECT (ButtonSearchPrev), "clicked", GTK_SIGNAL_FUNC(SearchAndGoToPreviousElement), 0 );
	gtk_widget_show( ButtonSearchPrev );
}

/* =========================================================================*/
/* GTK FILE TRANSFER BOX ... */
/* =========================================================================*/
gboolean FileTransferUpdateInfosGtk( void )
{
	if ( InfosGUI->TargetMonitor.RemoteFileTransfer )
	{
		char Buff[ 20 ];
		double BarValue = 0;
		char *FileWord = _("File");
		StrFileTransfer * pFileTrans = GetPtrFileTransForMaster( );
		gtk_widget_show( hBoxFileTransfer );
		if ( InfosGUI->TargetMonitor.TransferFileSubNum!=-1 )
			sprintf( Buff, "%s%d(%d)%sX", FileWord, InfosGUI->TargetMonitor.TransferFileNum, InfosGUI->TargetMonitor.TransferFileSubNum, InfosGUI->TargetMonitor.TransferFileIsSend?"->":"<-");
		else
			sprintf( Buff, "%s%d%sX", FileWord, InfosGUI->TargetMonitor.TransferFileNum, InfosGUI->TargetMonitor.TransferFileIsSend?"->":"<-");
		gtk_label_set_text( GTK_LABEL(FileTransferLabel), Buff );
		if ( pFileTrans->NumBlock>=0 )
		{
			if ( pFileTrans->NbrTotalBlocks>0 )
				BarValue = (double)pFileTrans->NumBlock/(double)(pFileTrans->NbrTotalBlocks+1.0);
printf("calc progress bar (file transfer), num=%d, nbr=%d, calc=%f\n", pFileTrans->NumBlock, pFileTrans->NbrTotalBlocks, BarValue );
			if ( BarValue>=1.0 )
				BarValue = 1.0;
			gtk_progress_bar_set_fraction( GTK_PROGRESS_BAR(FileTransferBar), BarValue );
		}
	}
	else
	{
printf("ask to hide progress bar...\n");
		gtk_widget_hide( hBoxFileTransfer );
	}
	return FALSE; //usefull when called with g_idle_add (just one time)
}
void FileTransferInitGtk(GtkBox *vbox)
{
	hBoxFileTransfer = gtk_hbox_new (FALSE,0);
	gtk_container_add (GTK_CONTAINER (vbox), hBoxFileTransfer);
	gtk_box_set_child_packing(GTK_BOX(vbox), hBoxFileTransfer,
		/*expand*/ FALSE, /*fill*/ FALSE, /*pad*/ 0, GTK_PACK_START);

	FileTransferLabel = gtk_label_new( "-" );
	gtk_box_pack_start (GTK_BOX (hBoxFileTransfer), FileTransferLabel, FALSE, FALSE, 4);
	gtk_widget_show( FileTransferLabel );

	FileTransferBar = gtk_progress_bar_new( );
	gtk_box_pack_start (GTK_BOX (hBoxFileTransfer), FileTransferBar, FALSE, FALSE, 4);
	gtk_widget_show( FileTransferBar );
	
	FileTransferAbortButton = gtk_button_new();
	gtk_button_set_image( GTK_BUTTON( FileTransferAbortButton ), gtk_image_new_from_stock( GTK_STOCK_STOP, GTK_ICON_SIZE_BUTTON ) );
	gtk_box_pack_start( GTK_BOX (hBoxFileTransfer), FileTransferAbortButton, FALSE, FALSE, 0 );
	gtk_signal_connect( GTK_OBJECT (FileTransferAbortButton), "clicked", GTK_SIGNAL_FUNC(TransferMasterAskToAbort), 0 );
	gtk_widget_show( FileTransferAbortButton );
}

void MainSectionWindowInitGtk()
{
	GtkWidget *vbox,*hboxtop; //,*hboxbottom,*hboxbottom2;
	GtkWidget *hboxmiddle;
//	GtkWidget *ButtonQuit;
//	GtkWidget *ButtonNew,*ButtonLoad,*ButtonSave,*ButtonSaveAs,*ButtonReset,*ButtonConfig,*ButtonAbout;
//	GtkWidget *ButtonEdit,*ButtonSymbols,*ButtonSpyVars,*ButtonLogBook;
//#ifdef GNOME_PRINT_USE
//	GtkWidget *ButtonPrint,*ButtonPrintPreview,*ButtonExportSVG,*ButtonExportPNG,*ButtonCopyToClipboard;
//#endif
//ForGTK3, deprecated...	GtkTooltips * TooltipsEntryLabel, * TooltipsEntryComment;
	GtkUIManager * PtrUIManager;

	MainSectionWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title ( GTK_WINDOW(MainSectionWindow), _("ClassicLadder Section Display"));
RestoreWindowPosiPrefs( "Main", MainSectionWindow );

	vbox = gtk_vbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER (MainSectionWindow), vbox);
	gtk_widget_show (vbox);

	gtk_signal_connect (GTK_OBJECT (MainSectionWindow), "destroy",
						GTK_SIGNAL_FUNC (QuitAppliGtk), NULL);

	PtrUIManager = InitMenusAndToolBar( vbox );
	gtk_window_add_accel_group( GTK_WINDOW( MainSectionWindow ), 
				  gtk_ui_manager_get_accel_group(PtrUIManager) );

	hboxtop = gtk_hbox_new (FALSE,0);
	gtk_container_add (GTK_CONTAINER (vbox), hboxtop);
	gtk_widget_show(hboxtop);
	gtk_box_set_child_packing(GTK_BOX(vbox), hboxtop,
		/*expand*/ FALSE, /*fill*/ FALSE, /*pad*/ 0, GTK_PACK_START);

//ForGTK3, deprecated...	TooltipsEntryLabel = gtk_tooltips_new();
	entrylabel = gtk_entry_new();
//GTK3	gtk_widget_set_usize((GtkWidget *)entrylabel,80,0);
	gtk_widget_set_size_request( entrylabel, 80, -1 );
	gtk_entry_set_max_length(GTK_ENTRY(entrylabel),LGT_LABEL-1);
//ForGTK3	gtk_entry_prepend_text((GtkEntry *)entrylabel,"");
	gtk_entry_set_text(GTK_ENTRY(entrylabel),"");
	gtk_box_pack_start (GTK_BOX (hboxtop), entrylabel, FALSE, FALSE, 0);
//ForGTK3, deprecated...	gtk_tooltips_set_tip ( TooltipsEntryLabel, entrylabel, "Label of the current selected rung", NULL );
	gtk_widget_set_tooltip_text( entrylabel, _("Label of the current selected rung") );
	gtk_widget_show(entrylabel);
//ForGTK3, deprecated...	TooltipsEntryComment = gtk_tooltips_new();
	entrycomment = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(entrycomment),LGT_COMMENT-1);
//ForGTK3	gtk_entry_prepend_text((GtkEntry *)entrycomment,"");
	gtk_entry_set_text(GTK_ENTRY(entrycomment),"");
	gtk_box_pack_start (GTK_BOX (hboxtop), entrycomment, TRUE, TRUE, 0);
//ForGTK3, deprecated...	gtk_tooltips_set_tip ( TooltipsEntryComment, entrycomment, "Comment of the current selected rung", NULL );
	gtk_widget_set_tooltip_text( entrycomment, _("Comment of the current selected ladder rung or sequential page") );
	gtk_widget_show(entrycomment);

	CheckDispSymbols = gtk_check_button_new_with_label(_("Display symbols"));
	gtk_box_pack_start( GTK_BOX (hboxtop), CheckDispSymbols, FALSE, FALSE, 0 );
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( CheckDispSymbols ), Preferences.DisplaySymbolsInMainWindow/*InfosGene->DisplaySymbols*/ );
	gtk_signal_connect( GTK_OBJECT(CheckDispSymbols), "toggled",
				GTK_SIGNAL_FUNC(CheckDispSymbols_toggled), NULL );
	gtk_widget_show( CheckDispSymbols );

	WidgetDurationOfLastScan = gtk_entry_new();
//GTK3	gtk_widget_set_usize(WidgetDurationOfLastScan,150,0);
	gtk_widget_set_size_request( WidgetDurationOfLastScan, 150, -1 );
//	gtk_entry_set_max_length((GtkEntry *)WidgetDurationOfLastScan,10);
//ForGTK3	gtk_entry_prepend_text((GtkEntry *)WidgetDurationOfLastScan,"---");
	gtk_entry_set_text(GTK_ENTRY(WidgetDurationOfLastScan),"---");
	gtk_box_pack_start (GTK_BOX (hboxtop), WidgetDurationOfLastScan, FALSE, FALSE, 0);
	gtk_widget_set_sensitive(WidgetDurationOfLastScan, FALSE);


	hboxmiddle = gtk_hbox_new (FALSE,0);
	gtk_container_add (GTK_CONTAINER (vbox), hboxmiddle);
	gtk_widget_show(hboxmiddle);
	gtk_box_set_child_packing(GTK_BOX(vbox), hboxmiddle,
		/*expand*/ TRUE, /*fill*/ TRUE, /*pad*/ 0, GTK_PACK_START);

	/* Create the drawing area */
	drawing_area = gtk_drawing_area_new ();
//ForGTK3	gtk_drawing_area_size (GTK_DRAWING_AREA (drawing_area) ,
//ForGTK3							BLOCK_WIDTH_DEF*RUNG_WIDTH+20 ,
//ForGTK3							BLOCK_HEIGHT_DEF*RUNG_HEIGHT+45);
	gtk_widget_set_size_request(drawing_area,
							BLOCK_WIDTH_DEF*RUNG_WIDTH+20 ,
							BLOCK_HEIGHT_DEF*RUNG_HEIGHT+45);
	gtk_box_pack_start (GTK_BOX (hboxmiddle), drawing_area, TRUE, TRUE, 0);
	gtk_widget_show (drawing_area);

	AdjustVScrollBar = (GtkAdjustment *)gtk_adjustment_new( 0, 0, 0, 0, 0, 0);
	VScrollBar = gtk_vscrollbar_new( AdjustVScrollBar );
	gtk_box_pack_start (GTK_BOX (hboxmiddle), VScrollBar, FALSE, FALSE, 0);
	gtk_widget_show (VScrollBar);

	AdjustHScrollBar = (GtkAdjustment *)gtk_adjustment_new( 0, 0, 0, 0, 0, 0);
	HScrollBar = gtk_hscrollbar_new( AdjustHScrollBar );
	gtk_box_pack_start (GTK_BOX (vbox), HScrollBar, FALSE, FALSE, 0);
	gtk_widget_show (HScrollBar);
//v0.9.20	UpdateVScrollBar( TRUE/*AutoSelectCurrentRung*/ );

	gtk_signal_connect(GTK_OBJECT (AdjustVScrollBar), "value-changed",
						GTK_SIGNAL_FUNC(VScrollBar_value_changed_event), 0);
	gtk_signal_connect(GTK_OBJECT (AdjustHScrollBar), "value-changed",
						GTK_SIGNAL_FUNC(HScrollBar_value_changed_event), 0);

	/* Search function */
	SearchInitGtk( GTK_BOX(vbox) );
	
	FileTransferInitGtk( GTK_BOX(vbox) );

	/* Create the status bar */
	StatusBar = gtk_statusbar_new ();
//	gtk_statusbar_set_has_resize_grip( GTK_STATUSBAR(StatusBar), FALSE );
	gtk_box_pack_start (GTK_BOX(vbox), StatusBar, FALSE, FALSE, 0);
	gtk_widget_show (StatusBar);
	StatusBarContextId = gtk_statusbar_get_context_id( GTK_STATUSBAR(StatusBar), "Statusbar" );


	/* Signal used to redraw */
#if GTK_MAJOR_VERSION>=3
	gtk_signal_connect (GTK_OBJECT (drawing_area), "draw",
						GTK_SIGNAL_FUNC(draw_callback), NULL);
#else
	gtk_signal_connect (GTK_OBJECT (drawing_area), "expose_event",
						GTK_SIGNAL_FUNC(expose_event), NULL);
#endif
//Cairo	gtk_signal_connect (GTK_OBJECT(drawing_area),"configure_event",
//Cairo						(GtkSignalFunc) configure_event, NULL);

	/* Event signals */
	gtk_signal_connect (GTK_OBJECT (drawing_area), "button_press_event",
						GTK_SIGNAL_FUNC(button_press_event), NULL);
	gtk_signal_connect (GTK_OBJECT (drawing_area), "motion_notify_event",
						GTK_SIGNAL_FUNC(motion_notify_event), NULL);
	gtk_signal_connect (GTK_OBJECT (drawing_area), "button_release_event",
						GTK_SIGNAL_FUNC(button_release_event), NULL);
	gtk_signal_connect (GTK_OBJECT (drawing_area), "scroll_event",
						GTK_SIGNAL_FUNC(mouse_scroll_event), NULL);
	gtk_signal_connect (GTK_OBJECT (MainSectionWindow), "key_press_event",
						GTK_SIGNAL_FUNC(key_press_event), NULL);
	gtk_widget_set_events (drawing_area, GDK_EXPOSURE_MASK
							| GDK_LEAVE_NOTIFY_MASK
							| GDK_BUTTON_PRESS_MASK
							| GDK_BUTTON_RELEASE_MASK
							| GDK_SCROLL_MASK  // mouse scroll
							| GDK_POINTER_MOTION_MASK
							| GDK_POINTER_MOTION_HINT_MASK);

	gtk_signal_connect( GTK_OBJECT(MainSectionWindow), "delete_event",
		GTK_SIGNAL_FUNC(MainSectionWindowDeleteEvent), 0 );

//	gtk_window_set_icon_from_file( GTK_WINDOW(MainSectionWindow), CL_ICON_FILE, NULL );
	gtk_window_set_icon(GTK_WINDOW(MainSectionWindow), gdk_pixbuf_new_from_inline (-1, IconClassicLadderApplication, FALSE, NULL));

	gtk_widget_show (MainSectionWindow);

	GetTheSizesForRung();
	// added in v0.9.10, usefull if no project loaded to have label/comment entries disabled...
	AdjustLabelCommentEntriesToSection( -1/*SectionLanguage*/ );
}

void MainSectionWindowTakeFocus( )
{
	gtk_window_present( GTK_WINDOW(MainSectionWindow) );
}

void RedrawSignalDrawingArea( void )
{
#if GTK_MAJOR_VERSION>=3
	gtk_widget_queue_draw( drawing_area );
#else
	GdkRegion * region = gdk_drawable_get_clip_region( drawing_area->window );
	// redraw completely by exposing it
	gdk_window_invalidate_region( drawing_area->window, region, TRUE );
	gdk_window_process_updates( drawing_area->window, TRUE );
	gdk_region_destroy( region );
#endif
}

static gint PeriodicUpdateDisplay(gpointer data)
{
if ( InfosGene==NULL )
	return 1;
//Dec.2016, for MSYS2/Win32!!! gdk_threads_enter();
	if ( InfosGene->LadderState==STATE_RUN || InfosGene->LadderState==STATE_RUN_FOR_ONE_CYCLE || InfosGene->LadderState==STATE_RUN_FREEZE )
	{
		if (InfosGene->CmdRefreshVarsBits)
		{
			RefreshAllBoolsVars();
			InfosGene->CmdRefreshVarsBits = FALSE;
		}
		DisplayFreeVarSpy();
#ifdef COMPLETE_PLC
		if ( InfosGene->LogContentModified )
		{
			DisplayLogBookEvents( TRUE/*OnLogContentModified*/ );
			InfosGene->LogContentModified = FALSE;
		}
		if ( InfosGene->DefaultLogListModified )
		{
//printf("$$$DefaultLogListModified flag !\n");
			int NbrDefs = FindCurrentDefaults( );
			InfosGene->DefaultLogListModified = FALSE;
			if ( NbrDefs>0 )
			{
				char * ListDefaultsText = (char *)malloc( NbrDefs*(EVENT_SYMBOL_LGT+10)+10 );
				if ( ListDefaultsText )
				{
					StrConfigEventLog * pCfgEvtLog;
					int ScanList;
					char OneEventText[ EVENT_SYMBOL_LGT+10 ];
					if ( NbrDefs>1 )
						strcpy( ListDefaultsText, _("DEFAULTS") );
					else
						strcpy( ListDefaultsText, _("DEFAULT") );
					strcat( ListDefaultsText, " : ");
//printf("nbr defaults=%d\n", NbrDefs);
					for( ScanList=0; ScanList<NbrDefs; ScanList++ )
					{
						pCfgEvtLog = &ConfigEventLog[ ListCurrentDefType[ ScanList ] ];
						//display value parameter after symbol name if many variables configured for the same event !
						if ( ListCurrentDefParam[ ScanList ]!=-1 )
							sprintf( OneEventText, "%s%d ", pCfgEvtLog->Symbol, ListCurrentDefParam[ ScanList ] );
						else
							sprintf( OneEventText, "%s ", pCfgEvtLog->Symbol );
						strcat( ListDefaultsText, OneEventText );
					}
					MessageInStatusBar( ListDefaultsText );
					free( ListDefaultsText );
				}
			}
			else
			{
				MessageInStatusBar( _("No default.") );
			}
		}
#endif
	}

	if (InfosGene->LadderState!=STATE_LOADING )
	{
//		DrawCurrentSection( );
//		CairoDrawCurrentSection( );
		RedrawSignalDrawingArea( );
	}
	if ( InfosGene->ErrorMsgStringToDisplay[ 0 ]!='\0' )
	{
		ShowMessageBox( InfosGene->ErrorTitleStringToDisplay[0]!='\0'?InfosGene->ErrorTitleStringToDisplay:(_("An error occurred!")), InfosGene->ErrorMsgStringToDisplay, _("Ok") );
		InfosGene->ErrorMsgStringToDisplay[ 0 ] = '\0';
		InfosGene->ErrorTitleStringToDisplay[ 0 ] = '\0';
	}

	if( InfosGUI->TargetMonitor.RemoteConnected )
	{
		RefreshAllBoolsVars();
		DisplayFreeVarSpy();
		RedrawSignalDrawingArea( );
		if ( InfosGUI->TargetMonitor.CmdDisplayTargetInfosVersion )
		{
			DisplayTargetInfosVersion( );
			InfosGUI->TargetMonitor.CmdDisplayTargetInfosVersion = FALSE;
		}
		if ( InfosGUI->TargetMonitor.CmdDisplayTargetNetworkConfig )
		{
			UpdateNetworkConfigValues( );
			InfosGUI->TargetMonitor.CmdDisplayTargetNetworkConfig = FALSE;
		}
		if ( InfosGUI->TargetMonitor.CmdDisplayTargetMonitorSerialConfig )
		{
			UpdateMonitorSerialConfigValues( );
			InfosGUI->TargetMonitor.CmdDisplayTargetMonitorSerialConfig = FALSE;
		}
	}
	if ( InfosGene->DurationOfLastScan!=-1 || InfosGene->NbrTicksMissed!=-1 )
	{
		char TempBuffer[ 60 ];
		char TextBuffer[ 100 ];
		TextBuffer[ 0 ] = '\0';
		if ( InfosGene->DurationOfLastScan!=-1 )
		{
			sprintf(TempBuffer , "%d %s (%s=%d)", InfosGene->DurationOfLastScan/1000, _("us"), _("max"), InfosGene->MaxScanDuration/1000);
			strcat( TextBuffer, TempBuffer );
		}
		if ( InfosGene->NbrTicksMissed!=-1 )
		{
			if ( InfosGene->DurationOfLastScan!=-1 )
				strcat( TextBuffer, " / " );
			sprintf( TempBuffer, "%d %s",InfosGene->NbrTicksMissed, _("missed") );
			strcat( TextBuffer, TempBuffer );
		}
		gtk_entry_set_text(GTK_ENTRY(WidgetDurationOfLastScan),TextBuffer);
		gtk_widget_show( WidgetDurationOfLastScan );
	}
	else
	{
		gtk_widget_hide( WidgetDurationOfLastScan );
	}
//Dec.2016, for MSYS2/Win32!!! gdk_threads_leave();
	return 1;
}


void InitGtkWindows( int argc, char *argv[] )
{
	int NumFramesLogWindow;
//	char * ReturnDomainDir;
printf("<<<<<<<<<<========== INIT GTK WINDOWS ==========>>>>>>>>>>\n");
	//For i18n
	setlocale( LC_ALL, "" );
	// works if ClassicLadder started from the directory where is the executable !
	/*ReturnDomainDir =*/ bindtextdomain( "classicladder", "po" /*"./po"*/ );
/*	if ( ReturnDomainDir )
		printf("Successfully bind text domain for i18n : %s\n", ReturnDomainDir );
	else
		printf("Error bind text domain for i18n. Default english used!\n");*/
	textdomain( "classicladder" );
	//Dec.2016, required for MSYS2/Win32!!! (else codeset seems null per default...)
	bind_textdomain_codeset( "classicladder", "UTF-8" );
	
	debug_printf( "Your GTK+ version is: %d.%d.%d, glib is: %d.%d.%d and zlib is: %s\n", gtk_major_version, gtk_minor_version,
			gtk_micro_version, glib_major_version, glib_minor_version, glib_micro_version, ZLIB_VERSION );
//ProblemWithPrint	g_thread_init (NULL);
//ProblemWithPrint	gdk_threads_init ();
//set back for monitor window...
//ForGTK3
#if ( GTK_MAJOR_VERSION<=2 )
	g_thread_init (NULL);
#endif
//Dec.2016, for MSYS2/Win32!!!	gdk_threads_init( );
//Dec.2016, for MSYS2/Win32!!!	gdk_threads_enter();
	gtk_init (&argc, &argv);

	AddCustomIconsAndCursors( );

//printf("====> InitVarsWin\n");
	VarsWindowInitGtk();
//printf("====> InitMainWin\n");
	MainSectionWindowInitGtk();
//moved before, else crashing when adding tooltips...?
//	VarsWindowInitGtk();
//printf("====> InitEditWin\n");
	EditorInitGtk();
//printf("====> InitPropWin\n");
	PropertiesInitGtk();
//printf("====> InitManagerWin\n");
	ManagerInitGtk( );
//printf("====> InitSymbolsWin\n");
	SymbolsInitGtk( );
//printf("====> InitLogBookWin\n");
#ifdef COMPLETE_PLC
	LogBookInitGtk( );
#endif
//printf("====> InitFramesLogWin\n");
	FramesLogWindowsInitGtk( );
	// restore each window open/closed state at startup !
	if ( GetWindowOpenPrefs( "Edit" ) )
		SetToggleMenuForEditorWindow( TRUE );
	if ( GetWindowOpenPrefs( "Symbols" ) )
		SetToggleMenuForSymbolsWindow( TRUE );
	if ( GetWindowOpenPrefs( "BoolVars" ) )
		SetToggleMenuForBoolVarsWindow( TRUE );
	if ( GetWindowOpenPrefs( "FreeVars" ) )
		SetToggleMenuForFreeVarsWindow( TRUE );
	if ( GetWindowOpenPrefs( "LogBook" ) )
		SetToggleMenuForLogWindow( TRUE );
	for( NumFramesLogWindow=0; NumFramesLogWindow<NBR_FRAMES_LOG_WINDOWS; NumFramesLogWindow++ )
	{
		char Buff[ 30 ];
		sprintf( Buff, "FramesLogWindow%d", NumFramesLogWindow );
		if ( GetWindowOpenPrefs( Buff ) )
			SetToggleMenuForMonitorWindow( NumFramesLogWindow, TRUE );
	}
	MainSectionWindowTakeFocus( );
	
//->moved in classicladder.c SetMenuStateForRunStopSwitch( TRUE );
#ifdef COMPLETE_PLC
SetGtkMenuStateForConnectDisconnectSwitch( FALSE );
#endif
//deprecated	gtk_timeout_add( TIME_UPDATE_GTK_DISPLAY_MS, PeriodicUpdateDisplay, NULL );
	g_timeout_add( TIME_UPDATE_GTK_DISPLAY_MS, PeriodicUpdateDisplay, NULL );
}

gboolean UpdateAllGtkWindows( void )
{
	ManagerDisplaySections( TRUE/*ForgetSectionSelected*/ );
//	DrawCurrentSection( );
	RedrawSignalDrawingArea( );
	RefreshLabelCommentEntries( );
	AdjustLabelCommentEntriesToSection( -1/*SectionLanguage*/ );
	UpdateVScrollBar( TRUE/*AutoSelectCurrentRung*/ );
//moved at top in v0.9.7	ManagerDisplaySections( );
	DisplaySymbols( );
	UpdateAllLabelsBoolsVars( -1/*OnlyThisColumn*/ );
	UpdateAllLabelsFreeVars( -1/*OnlyThisOne*/, NULL );
	DisplayProjectProperties( );
	DisplayLogBookEvents( FALSE/*OnLogContentModified*/ );
	UpdateMonitorModbusSlaveListStats( );
	return FALSE; //usefull when called with g_idle_add (just one time)
}

gboolean UpdateWindowTitleWithProjectName( void )
{
	char Buff[ 250 ];
	int ScanFileNameOnly = 0;
	int LgtProjectFileName = strlen(InfosGene->CurrentProjectFileName);
	char * WindowTitle = _("ClassicLadder Section Display");
	if ( LgtProjectFileName==0 )
	{
		sprintf( Buff, "%s (%s)", WindowTitle, _("No project") );
		if ( InfosGUI->TargetMonitor.RemoteConnected )
		{
			strcat( Buff, " - ");
			strcat( Buff, _("CONNECTED") );
		}
	}
	else 
	{
		if ( LgtProjectFileName>2 )
		{
			ScanFileNameOnly = LgtProjectFileName-1;
			while( ScanFileNameOnly>0 && InfosGene->CurrentProjectFileName[ScanFileNameOnly-1]!='/' && InfosGene->CurrentProjectFileName[ScanFileNameOnly-1]!='\\')
				ScanFileNameOnly--;
		}
		sprintf( Buff, "%s (%s)", &InfosGene->CurrentProjectFileName[ScanFileNameOnly], WindowTitle );
		if ( InfosGUI->TargetMonitor.RemoteConnected )
		{
			strcat( Buff, " - ");
			strcat( Buff, _("CONNECTED") );
		}
	}
	gtk_window_set_title( GTK_WINDOW(MainSectionWindow), Buff );
	return FALSE; //usefull when called with g_idle_add (just one time)
}
