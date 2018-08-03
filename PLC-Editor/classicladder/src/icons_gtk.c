/* Classic Ladder Project */
/* Copyright (C) 2001-2016 Marc Le Douarain */
/* http://www.sourceforge.net/projects/classicladder */
/* http://sites.google.com/site/classicladder */
/* February 2013 */
/* ----------------------------------------------- */
/* "embedded in code" Gtk+ icons */
/* module contribution by Heli Tejedor */
/* helitp At arrakis DoT es */
/* http://heli.xbot.es */
/* ----------------------------------------------- */
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
#include <gtk/gtk.h>

#include "classicladder.h"
#include "global.h"
#include "icons_gtk.h"

#include "../icons/IconOpenContact.h"
#include "../icons/IconClosedContact.h"
#include "../icons/IconRisingEdge.h"
#include "../icons/IconFallingEdge.h"
#include "../icons/IconCoil.h"
#include "../icons/IconCoilNot.h"
#include "../icons/IconSetCoil.h"
#include "../icons/IconResetCoil.h"
#include "../icons/IconJumpCoil.h"
#include "../icons/IconCallCoil.h"
#include "../icons/IconIECTimerBox.h"
#include "../icons/IconCounterBox.h"
#include "../icons/IconRegisterBox.h"
#include "../icons/IconOldTimerBox.h"
#include "../icons/IconOldMonoBox.h"
#include "../icons/IconCompareBox.h"
#include "../icons/IconOperateBox.h"
#include "../icons/IconWire.h"
#include "../icons/IconConnection.h"
#include "../icons/IconWireLong.h"
#include "../icons/IconEraser.h"

#include "../icons/IconSeqStep.h"
#include "../icons/IconSeqInitStep.h"
#include "../icons/IconSeqTransi.h"
#include "../icons/IconSeqStepAndTransi.h"
#include "../icons/IconSeqTransisOr1.h"
#include "../icons/IconSeqTransisOr2.h"
#include "../icons/IconSeqStepsAnd1.h"
#include "../icons/IconSeqStepsAnd2.h"
#include "../icons/IconSeqLink.h"
#include "../icons/IconSeqComment.h"

GtkIconFactory *factory;

#define N_ICONS 31
GtkStockItem Item[N_ICONS];
GdkPixbuf *PixBufIcon[N_ICONS];
gchar *id[N_ICONS] = {"IconOpenContact", "IconClosedContact", "IconRisingEdge", "IconFallingEdge", "IconCoil", "IconCoilNot",
                      "IconSetCoil", "IconResetCoil", "IconJumpCoil", "IconCallCoil", "IconIECTimerBox", "IconCounterBox", "IconRegisterBox",
                      "IconOldTimerBox", "IconOldMonoBox",  "IconCompareBox", "IconOperateBox", "IconWire", "IconConnection",
                      "IconWireLong","IconEraser",
                      "IconSeqStep", "IconSeqInitStep", "IconSeqTransi", "IconSeqStepAndTransi",
                      "IconSeqTransisOr1","IconSeqTransisOr2","IconSeqStepsAnd1","IconSeqStepsAnd2",
                      "IconSeqLink","IconSeqComment"};
const guint8 *IconHeaderPtr[N_ICONS] = {IconOpenContact, IconClosedContact, IconRisingEdge, IconFallingEdge, IconCoil, IconCoilNot,
                                  IconSetCoil, IconResetCoil, IconJumpCoil, IconCallCoil, IconIECTimerBox, IconCounterBox, IconRegisterBox,
                                  IconOldTimerBox, IconOldMonoBox,  IconCompareBox, IconOperateBox, IconWire, IconConnection,
                                  IconWireLong,IconEraser,
                                  IconSeqStep,IconSeqInitStep,IconSeqTransi,IconSeqStepAndTransi,
                                  IconSeqTransisOr1,IconSeqTransisOr2,IconSeqStepsAnd1,IconSeqStepsAnd2,
                                  IconSeqLink,IconSeqComment};

// Custom Cursors
GdkCursor *MyCursor[N_ICONS];
extern GtkWidget *drawing_area;

void AddCustomIconsAndCursors (void)
{
	int cnt;

	factory = gtk_icon_factory_new();
	gtk_icon_factory_add_default( factory );

	for (cnt=0; cnt<N_ICONS; cnt++)
	{
//printf("Icon n°%d\n", cnt);
		Item[cnt].stock_id = id[cnt];
		Item[cnt].label = id[cnt];
		Item[cnt].modifier = 0;
		Item[cnt].keyval = 0;
//printf("Add static n°%d\n", cnt);
		gtk_stock_add_static( &Item[cnt], 1 );
//printf( "Loading pixbuf from inline for icon n° %d\n", cnt );
		PixBufIcon[cnt] = gdk_pixbuf_new_from_inline (-1, IconHeaderPtr[cnt], FALSE, NULL);
		if ( PixBufIcon[cnt] )
		{
//printf( "Factory adding pixbuf for icon n° %d\n", cnt );
			gtk_icon_factory_add( factory, id[cnt], gtk_icon_set_new_from_pixbuf (PixBufIcon[cnt])); 
		}
		else
		{
			printf( "FAILED IN PIXBUF FROM INLINE FOR ICON N°%d\n", cnt );
		}
	} 
	for (cnt=0; cnt<N_ICONS; cnt++)
	{
//		MyCursor = gdk_cursor_new_for_display(gdk_display_get_default(), GDK_HAND2);
		if ( PixBufIcon[cnt] )
		{
//printf( "Adding cursor n°%d from pixbuf\n", cnt );
			MyCursor[cnt] = gdk_cursor_new_from_pixbuf (gdk_display_get_default(), PixBufIcon[cnt] , 8, 8 );  // 16 * 16 cursor
		}
	}
 // gdk_cursor_new_for_display(gdk_display_get_default(), GDK_HAND2 /*GDK_ARROW*/);  // Default cursor use NULL
}

void UseCustomIcon (char * CursorName)
{                                        
	GdkWindow *win = gtk_widget_get_window( GTK_WIDGET( drawing_area ) );
	if (CursorName==NULL)
	{
		gdk_window_set_cursor( win , NULL);  // Default cursor
	}
	else
	{
		int ScanIconIndex = 0;
		char Found = FALSE;
printf("Want to use custom cursor %s\n", CursorName );
		do
		{
			if ( strcmp( id[ ScanIconIndex ], CursorName )==0 )
				Found = TRUE;
			else
				ScanIconIndex++;
		}
		while( !Found && ScanIconIndex<N_ICONS );
printf( "ScanIconIndex=%d\n", ScanIconIndex );
		if ( Found )
		{
			gdk_window_set_cursor( win , MyCursor[ScanIconIndex] );
		}
		else
		{
			gdk_window_set_cursor( win , NULL);  // Default cursor
			printf("ERROR CURSOR LABEL '%s' NOT FOUND !!!!!\n",CursorName);
		}
	}
//	printf ("CURSOR!\n"); // TEST
}

void DestroyCustomIconsAndCursors (void)
{
	int cnt;

	for (cnt=0; cnt<N_ICONS; cnt++)
	{
		if( MyCursor[cnt]!=NULL )
			gdk_cursor_unref(MyCursor[cnt]);
//ForGTK3		if( MyCursor[cnt]->ref_count <= 0 )
			MyCursor[cnt] = NULL;
			
		g_object_unref( PixBufIcon[cnt] );
		PixBufIcon[cnt] = NULL;
	}
}

