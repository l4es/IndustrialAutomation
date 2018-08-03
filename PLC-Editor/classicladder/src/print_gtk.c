/* Classic Ladder Project */
/* Copyright (C) 2001-2017 Marc Le Douarain */
/* http://www.sourceforge.net/projects/classicladder */
/* http://sites.google.com/site/classicladder */
/* August 2008 */
/* ---------------------------------------------------- */
/* Printer output (using Gtk-print and Cairo rendering) */
/* ---------------------------------------------------- */
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
#include <math.h>
#include <gtk/gtk.h>
#include <pango/pango.h>
#include <libintl.h> // i18n
#include <locale.h> // i18n

#include "classicladder.h"
#include "global.h"
#include "drawing.h"
#include "drawing_sequential.h"
#include "classicladder_gtk.h"
#include "edit.h"

#define NBR_SYMBOLS_LINES_PER_PAGE 60

static GtkPrintSettings *settings = NULL;
static int OffsetPrintX = 20;
static int OffsetPrintY = 20;
int NbrRungsPerPage = 0;
int NbrPagesToPrint = 0;
int ScanRungToPrint = -1;
int ScanSectionToPrint = -1;
int NbrPagesForSymbolsList = 0;
int ScanSymbolToPrint = 0;

int PrintLadderBlockWidth = BLOCK_WIDTH_DEF;
int PrintLadderBlockHeight = BLOCK_HEIGHT_DEF;
int PrintSeqSize = 32;
int PrintHeaderLabelCommentHeight = BLOCK_HEIGHT_DEF/2;
int SpaceBetweenRungsY = 25;
int SymbolsLineHeight = 10;

// custom print options
GtkWidget * CustomOptionPrintSection;
GtkWidget * CustomOptionComboCurrentOrAllSections;
GtkWidget * CustomOptionPrintSymbolsList;
char OptionPrintSection = TRUE;
char OptionPrintAllSections = FALSE;
char OptionPrintSymbolsList = FALSE;

static void begin_print(GtkPrintOperation *operation, GtkPrintContext   *context, gpointer           user_data)
{
	NbrPagesToPrint = 0;
	// reset variables used between each page drawed...
	ScanRungToPrint = -1;
	ScanSectionToPrint = -1;
	ScanSymbolToPrint = 0;
	int NumSec;
	int PageWidth = gtk_print_context_get_width(context);
	int PageHeight = gtk_print_context_get_height(context);

	int FinalLadderPageWidth = (PageWidth*75/100) - (OffsetPrintX*2);

	// symbols list pages
	if ( OptionPrintSymbolsList )
	{
		int NbrSymbolsDefined = 0;
		int ScanSymb;
		for ( ScanSymb=0; ScanSymb<NBR_SYMBOLS; ScanSymb++ )
		{
			if ( SymbolArray[ ScanSymb ].Symbol[0]!='\0' )
				NbrSymbolsDefined++;
		}
		NbrPagesForSymbolsList = 1 + (NbrSymbolsDefined-1)/NBR_SYMBOLS_LINES_PER_PAGE;
		NbrPagesToPrint += NbrPagesForSymbolsList;
printf("print NbrSymbols=%d, NbrPagesForSymbol=%d\n", NbrSymbolsDefined, NbrPagesForSymbolsList);
	}
	
	// sizes elements calculation...
	PrintLadderBlockWidth = FinalLadderPageWidth/RUNG_WIDTH;
	PrintLadderBlockHeight = PrintLadderBlockWidth*BLOCK_HEIGHT_DEF/BLOCK_WIDTH_DEF;
	PrintHeaderLabelCommentHeight = PrintLadderBlockWidth/2;
	SpaceBetweenRungsY = PrintLadderBlockHeight*80/100;
	PrintSeqSize = PageWidth/SEQ_PAGE_WIDTH;
	SymbolsLineHeight = (PageHeight-OffsetPrintY)/NBR_SYMBOLS_LINES_PER_PAGE;
	
	// right margin for ladder pages
	PrintRightMarginPosiX = FinalLadderPageWidth + OffsetPrintX;
	PrintRightMarginWidth = PageWidth-FinalLadderPageWidth - (OffsetPrintX*2);
	
	if ( OptionPrintSection )
	{

printf("<<<< CALC %s SECTION(S) >>>\n", OptionPrintAllSections?"ALL":"CURRENT");
printf("print general infos: width=%d, height=%d, ladder_width_used=%d\n", PageWidth, PageHeight, FinalLadderPageWidth );
		for ( NumSec=0; NumSec<NBR_SECTIONS; NumSec++ )
		{
			StrSection * pSection = &SectionArray[ NumSec ];
			if ( pSection->Used )
			{
				if ( ( !OptionPrintAllSections && InfosGene->CurrentSection==NumSec ) || OptionPrintAllSections )
				{
					int iCurrentLanguage = SectionArray[ NumSec ].Language;
					if ( iCurrentLanguage==SECTION_IN_LADDER )
					{
						int the_width = RUNG_WIDTH*PrintLadderBlockWidth;
						int the_height = PrintHeaderLabelCommentHeight + RUNG_HEIGHT*PrintLadderBlockHeight;

						int NbrTotalRungs = 0;
						NbrTotalRungs = GetNbrRungsDefinedForASection( pSection );
						NbrRungsPerPage = PageHeight/(the_height+SpaceBetweenRungsY);
						NbrPagesToPrint = NbrPagesToPrint + (NbrTotalRungs+NbrRungsPerPage-1)/NbrRungsPerPage;
printf( "section: %s, rung page: w=%d, h=%d, PageWidth=%d, NbrTotalRungs=%d NbrRungsPerPage=%d  NbrPagesToPrint=%d\n",SectionArray[ NumSec ].Name, the_width,the_height,PageWidth,NbrTotalRungs,NbrRungsPerPage,NbrPagesToPrint );
printf( "print sizes: PrintLadderBlockWidth=%d, PrintLadderBlockHeight=%d, PrintSeqSize=%d\n", PrintLadderBlockWidth, PrintLadderBlockHeight, PrintSeqSize );
	//					ScanRungToPrint = InfosGene->FirstRung;
					}
#ifdef SEQUENTIAL_SUPPORT
					if ( iCurrentLanguage==SECTION_IN_SEQUENTIAL )
					{
						NbrPagesToPrint = NbrPagesToPrint+1;
printf("section: %s, SeqSize=%d\n", SectionArray[ NumSec ].Name, PrintSeqSize );
					}
#endif
					if ( ScanSectionToPrint==-1 )
					{
						ScanSectionToPrint = NumSec;
						if ( iCurrentLanguage==SECTION_IN_LADDER )
							ScanRungToPrint = pSection->FirstRung;
					}
				}
			}
		}
		
	}//if ( OptionPrintSection )
	gtk_print_operation_set_n_pages( operation, NbrPagesToPrint );
}

static void DrawSymbolsListPage( GtkPrintContext   *context, int page_nr )
{
	cairo_t *cr;
	char Buffer[ 80 ];
	int ScanPosiY = OffsetPrintY;
	int PosiX;
	cr = gtk_print_context_get_cairo_context( context );
	CreateFontPangoLayout( cr, PrintLadderBlockHeight, DRAW_FOR_PRINT );
	sprintf( Buffer, CL_PRODUCT_NAME CL_RELEASE_VER_STRING "  -  %s  -  %s:%d/%d", _("Symbols list"), _("Page"), page_nr+1, NbrPagesToPrint );
	DrawPangoText( cr, OffsetPrintX, gtk_print_context_get_height(context)-10, -1, -1, Buffer );

	PosiX = gtk_print_context_get_width(context)/6;
	int NbrSymbolsPrinted = 0;
	do
	{
		StrSymbol *pScanSymbol = &SymbolArray[ ScanSymbolToPrint ];
		if ( pScanSymbol->Symbol[0]!='\0' )
		{
			DrawPangoText( cr, OffsetPrintX, ScanPosiY, -1, -1, pScanSymbol->Symbol );
			DrawPangoText( cr, OffsetPrintX+PosiX, ScanPosiY, -1, -1, pScanSymbol->VarName );
			DrawPangoText( cr, OffsetPrintX+PosiX*2, ScanPosiY, -1, -1, pScanSymbol->Comment );
			NbrSymbolsPrinted++;
		}
		ScanSymbolToPrint++;
		ScanPosiY = ScanPosiY+SymbolsLineHeight;
	}
	while( ScanSymbolToPrint<NBR_SYMBOLS && NbrSymbolsPrinted<NBR_SYMBOLS_LINES_PER_PAGE );
}

static void draw_page( GtkPrintOperation *operation, GtkPrintContext   *context, int page_nr)
{
	cairo_t *cr;
	char Buffer[ 80 ];
//	int the_width = RUNG_WIDTH*bl_width;
//	int the_height = RUNG_HEIGHT*bl_height;

	if ( NbrPagesForSymbolsList>0 )
	{
		DrawSymbolsListPage( context, page_nr );
		NbrPagesForSymbolsList--;
	}
	else
	{

		if ( ScanSectionToPrint==-1 )
		{
			printf("ERROR print page without section index !!!\n");
			return;
		}

		int iCurrentLanguage = SectionArray[ ScanSectionToPrint ].Language;
			
		cr = gtk_print_context_get_cairo_context( context );
		CreateFontPangoLayout( cr, (iCurrentLanguage==SECTION_IN_SEQUENTIAL)?PrintSeqSize:PrintLadderBlockHeight, DRAW_FOR_PRINT );
		sprintf( Buffer, CL_PRODUCT_NAME CL_RELEASE_VER_STRING "  -  %s:%s  -  %s:%d/%d", _("Section"), SectionArray[ ScanSectionToPrint ].Name, _("Page"), page_nr+1, NbrPagesToPrint );
		DrawPangoText( cr, OffsetPrintX, gtk_print_context_get_height(context)-10, -1, -1, Buffer );

		char TheEndForThisSection = FALSE;
		if ( iCurrentLanguage==SECTION_IN_LADDER )
		{
			int NumRung = 0;
			int ScanPosiY = OffsetPrintY;
			int PrintLeftRightBarsWidth = PrintLadderBlockWidth/16;
			do
			{
				DrawLeftRightBars( cr, OffsetPrintX, ScanPosiY, PrintLadderBlockWidth, PrintLadderBlockHeight, PrintHeaderLabelCommentHeight, PrintLeftRightBarsWidth, FALSE );
				DrawRung( cr, ScanRungToPrint, FALSE/*InEdit*/, OffsetPrintX+PrintLeftRightBarsWidth, ScanPosiY, PrintLadderBlockWidth, PrintLadderBlockHeight, PrintHeaderLabelCommentHeight, DRAW_FOR_PRINT );
				if ( ScanRungToPrint!=SectionArray[ ScanSectionToPrint ].LastRung )
				{
					ScanRungToPrint = RungArray[ ScanRungToPrint ].NextRung;
					int the_height = PrintHeaderLabelCommentHeight + RUNG_HEIGHT*PrintLadderBlockHeight;
					ScanPosiY = ScanPosiY+the_height+SpaceBetweenRungsY;
					NumRung++;
				}
				else
				{
					TheEndForThisSection = TRUE;
				}
			}
			while( NumRung<NbrRungsPerPage && !TheEndForThisSection );
		}
#ifdef SEQUENTIAL_SUPPORT
		if ( iCurrentLanguage==SECTION_IN_SEQUENTIAL )
		{
//cairo_rectangle( cr, 0, 0, PrintSeqSize*SEQ_PAGE_WIDTH, PrintSeqSize*SEQ_PAGE_HEIGHT );
			DrawSequentialPage( cr, SectionArray[ ScanSectionToPrint ].SequentialPage, PrintSeqSize, DRAW_FOR_PRINT );
			TheEndForThisSection = TRUE;
		}
#endif
		if ( TheEndForThisSection && page_nr<NbrPagesToPrint )
		{
			do
			{
				ScanSectionToPrint++;
			}
			while( !SectionArray[ ScanSectionToPrint ].Used );
			if ( SectionArray[ ScanSectionToPrint ].Used )
			{
				int iCurrentLanguage = SectionArray[ ScanSectionToPrint ].Language;
				if ( iCurrentLanguage==SECTION_IN_LADDER )
					ScanRungToPrint = SectionArray[ ScanSectionToPrint ].FirstRung;
			}
			else
			{
				printf("ERROR print for no more section !!!\n");
				ScanSectionToPrint = -1; // should not...
			}
		}
		
	}
}

static void end_print(GtkPrintOperation *operation, GtkPrintContext   *context, gpointer           user_data)
{
}

static GObject * create_custom_widget_cb( GtkPrintOperation *operation, gpointer user_data )
{
	GtkWidget *vbox;
	vbox = gtk_vbox_new (FALSE, 0);
	CustomOptionPrintSection = gtk_check_button_new_with_label( _("Print section") );
	gtk_box_pack_start( GTK_BOX (vbox), CustomOptionPrintSection, FALSE, FALSE, 0 );
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( CustomOptionPrintSection ), TRUE );
	CustomOptionComboCurrentOrAllSections = gtk_combo_box_new_text();
	gtk_combo_box_append_text( MY_GTK_COMBO_BOX(CustomOptionComboCurrentOrAllSections), _("Print only current section") );
	gtk_combo_box_append_text( MY_GTK_COMBO_BOX(CustomOptionComboCurrentOrAllSections), _("Print all the sections") );
	gtk_combo_box_set_active( GTK_COMBO_BOX(CustomOptionComboCurrentOrAllSections), 0 );
	gtk_box_pack_start( GTK_BOX (vbox), CustomOptionComboCurrentOrAllSections, FALSE, FALSE, 0 );
	CustomOptionPrintSymbolsList = gtk_check_button_new_with_label( _("Print symbols list") );
	gtk_box_pack_start( GTK_BOX (vbox), CustomOptionPrintSymbolsList, FALSE, FALSE, 0 );
	gtk_widget_show_all( vbox );
	return G_OBJECT( vbox );
}
static void custom_widget_apply_cb( GtkPrintOperation *operation, GtkWidget *widget, gpointer user_data )
{
	OptionPrintSection = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( CustomOptionPrintSection ) );
	OptionPrintAllSections = gtk_combo_box_get_active( GTK_COMBO_BOX( CustomOptionComboCurrentOrAllSections ) );
	OptionPrintSymbolsList = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( CustomOptionPrintSymbolsList ) );
printf("<<<< CUSTOM OPTIONS = sections:%d / all:%d / symbols:%d >>>\n", OptionPrintSection, OptionPrintAllSections, OptionPrintSymbolsList);
}

void DoPrint( char DoPreview )
{
	GtkPrintOperation *print;
	GtkPrintOperationResult res;
	
	print = gtk_print_operation_new ();
	gtk_print_operation_set_job_name( print, CL_PRODUCT_NAME );
	
	gtk_print_operation_set_custom_tab_label(print, _("ClassicLadder Options") );

	if (settings != NULL) 
		gtk_print_operation_set_print_settings (print, settings);
	
	g_signal_connect (print, "begin_print", G_CALLBACK (begin_print), NULL);
	g_signal_connect (print, "draw_page", G_CALLBACK (draw_page), NULL);
	g_signal_connect (print, "end_print", G_CALLBACK (end_print), NULL);
	g_signal_connect (print, "create-custom-widget", G_CALLBACK (create_custom_widget_cb), NULL);
	g_signal_connect (print, "custom-widget-apply", G_CALLBACK (custom_widget_apply_cb), NULL);
	
	res = gtk_print_operation_run (print, DoPreview?GTK_PRINT_OPERATION_ACTION_PREVIEW:GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG,
									GTK_WINDOW (MainSectionWindow), NULL);
	
	if (res == GTK_PRINT_OPERATION_RESULT_ERROR)
	{
		ShowMessageBox( _("Print"), _("Failed to print..."), _("Ok") );
	}
	else if (res == GTK_PRINT_OPERATION_RESULT_APPLY)
	{
		if (settings != NULL)
			g_object_unref (settings);
		settings = g_object_ref (gtk_print_operation_get_print_settings (print));
	}
	
	g_object_unref (print);
}

void PrintGtk( )
{
	DoPrint( FALSE/*DoPreview*/ );
}
void PrintPreviewGtk( void )
{
	DoPrint( TRUE/*DoPreview*/ );
}

