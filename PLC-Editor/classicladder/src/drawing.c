/* Classic Ladder Project */
/* Copyright (C) 2001-2016 Marc Le Douarain */
/* http://www.sourceforge.net/projects/classicladder */
/* http://sites.google.com/site/classicladder */
/* February 2001 */
/* ---------------------------------------- */
/* Draw the ladder rungs                    */
/* + current section draw function          */
/* ======================================== */
/* Switched to CAIRO library in august 2008 */
/* ---------------------------------------- */
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
#include <math.h>
#include <gtk/gtk.h>
#include <pango/pango.h>

#include "classicladder.h"
#include "global.h"
#include "arithm_eval.h"
#include "classicladder_gtk.h"
#include "edit.h"
#ifdef SEQUENTIAL_SUPPORT
#include "drawing_sequential.h"
#endif
#include "symbols.h"
#include "vars_names.h"
#include "vars_access.h"
#include "search.h"
#include "drawing.h"

PangoLayout * pFontLayout = NULL;
int BlockPxHeightUsedForFont = -1;

int PrintRightMarginExprPosiY = 0;
int PrintRightMarginNumExpr = 0;
int PrintRightMarginPosiX;
int PrintRightMarginWidth;

//for monitor of functions blocks
int NbrFunctionsBlocksVarsDisplayed = 0;

StrMarkColor ColorElementSelected = { .ColorR = 0.99, .ColorG = 0.19, .ColorB = 0.19 };
//Heli,better? StrMarkColor ColorElementSearched = { .ColorR = 0.10, .ColorG = 0.39, .ColorB = 0.10 };
//Heli,better? StrMarkColor ColorElementHighlighted = { .ColorR = 0.0, .ColorG = 0.95, .ColorB = 0.0 };
StrMarkColor ColorElementSearched =    { .ColorR = 0.00, .ColorG = 0.95, .ColorB = 0.00 };
StrMarkColor ColorElementHighlighted = { .ColorR = 0.15, .ColorG = 0.15, .ColorB = 0.15 };

void CreateVarNameForElement( char * pBuffToWrite, StrElement * pElem, char SymbolsVarsNamesIfAvail )
{
	char VarIndexBuffer[20];
	if ( pElem->IndexedVarType!=-1 )
	{
		// buffer for index required as CreateVarName() returns on a static buffer !
		strcpy( VarIndexBuffer, CreateVarName(pElem->IndexedVarType,pElem->IndexedVarNum,SymbolsVarsNamesIfAvail) );
		sprintf( pBuffToWrite, "%s[%s]", CreateVarName(pElem->VarType,pElem->VarNum,SymbolsVarsNamesIfAvail), VarIndexBuffer );
	}
	else
	{
		strcpy( pBuffToWrite, CreateVarName(pElem->VarType,pElem->VarNum,SymbolsVarsNamesIfAvail) );
	}
}

char * DisplayArithmExpr(char * Expr, char SymbolsVarsNamesIfAvail)
{
	static char Buffer[ARITHM_EXPR_SIZE+30];
	char * Ptr = Expr;
	int Fill = 0;
	Buffer[0] = '\0';
	/* null expression ? */
	if (Expr[0]=='\0')
		return Buffer;
	do
	{
		/* start of a variable ? */
		if (*Ptr=='@')
		{
			int NumVar,TypeVar;
			int IndexNumVar,IndexTypeVar;
			char VarBuffer[20];
			char VarIndexBuffer[20];
			if ( IdentifyVarIndexedOrNot( Ptr, &TypeVar, &NumVar, &IndexTypeVar, &IndexNumVar ) )
			{
				if ( IndexTypeVar!=-1 && IndexNumVar!=-1 )
				{
					// buffer for index required as CreateVarName() returns on a static buffer !
					strcpy( VarIndexBuffer, CreateVarName(IndexTypeVar,IndexNumVar,SymbolsVarsNamesIfAvail) );
					sprintf(VarBuffer, "%s[%s]", CreateVarName(TypeVar,NumVar,SymbolsVarsNamesIfAvail), VarIndexBuffer );
				}
				else
				{
					strcpy(VarBuffer,CreateVarName(TypeVar,NumVar,SymbolsVarsNamesIfAvail));
				}
			}
			else
				strcpy(VarBuffer,"??");
			strcpy(&Buffer[Fill],VarBuffer);
			/* flush until end of a variable */
			do
			{
				Ptr++;
			}
			while(*Ptr!='@');
			Ptr++;
			Fill = Fill+strlen(VarBuffer);
		}
		else
		{
			Buffer[Fill++] = *Ptr++;
		}
	}
	while(*Ptr!='\0');
	Buffer[Fill] = '\0';
	return Buffer;
}

void CreateFontPangoLayout( cairo_t *cr, int BlockPxHeight, char DrawingOption )
{
	if ( pFontLayout==NULL || BlockPxHeightUsedForFont!=BlockPxHeight )
	{
		char BuffFontDesc[ 25 ];
		PangoFontDescription *FontDesc;
		const char * FontName = "Lucida Sans";
		int FontHeight = 8;
		if ( DrawingOption==DRAW_FOR_PRINT )
			FontHeight = BlockPxHeight*8/BLOCK_HEIGHT_DEF;
//TODO: cleanup also needed on exit?
		if ( pFontLayout!=NULL )
			g_object_unref( pFontLayout );
		pFontLayout = pango_cairo_create_layout( cr );

//	FontDesc = pango_font_description_from_string( "Andale Mono 8" );
//Cairo		FontDesc = pango_font_description_from_string( "Courier New 8" );
//	FontDesc = pango_font_description_from_string( "Lucida Bright 8" );
		sprintf( BuffFontDesc, "%s %d", FontName, FontHeight );
printf("Pango Layout Font:'%s' Height:%d BlockPxHeight:%d, BLOCK_HEIGHT_DEF:%d\n", FontName, FontHeight, BlockPxHeight, BLOCK_HEIGHT_DEF );
		FontDesc = pango_font_description_from_string( BuffFontDesc );
		pango_layout_set_font_description( pFontLayout, FontDesc );
		pango_font_description_free( FontDesc );
		BlockPxHeightUsedForFont = BlockPxHeight;
		
		pango_layout_set_wrap( pFontLayout, PANGO_WRAP_CHAR );
	}
}

/* Drawing text with Pango. */
/* if Height is -1, then drawing bottom text on top of BaseY given... */
/* if Height is 0, then drawing directly on BaseY */
/* return pixels height required if more than one line should be displayed (if height space available)
	else 0 (used to print) */
int DrawPangoTextOptions( cairo_t * cr, int BaseX, int BaseY, int Width, int Height, char * Text, char CenterAlignment ) 
{
	int SizeX, SizeY;
	int PosiY = BaseY;
	int TotalHeightRequired = 0;

	if ( pFontLayout==NULL )
		return 0;

	pango_layout_set_width( pFontLayout, Width>0?Width*PANGO_SCALE:-1 );
	pango_layout_set_alignment( pFontLayout, CenterAlignment?PANGO_ALIGN_CENTER:PANGO_ALIGN_LEFT );
	pango_layout_set_text( pFontLayout, Text, -1 );

	pango_layout_get_pixel_size( pFontLayout, &SizeX, &SizeY );
//printf("Pango sizeX=%d,sizeY=%d - width=%d,height=%d text=%s\n",SizeX,SizeY,Width,Height,Text);
	if ( Height<=0 )
	{
		if ( Height==-1 )
			PosiY = BaseY - SizeY; //BaseY is the bottom if Height is -1.
		TotalHeightRequired = SizeY;
	}
	else
	{
		if ( SizeY>Height )
		{
			TotalHeightRequired = SizeY;
			// just display the first line
			PangoLayoutLine * playout_line = pango_layout_get_line( pFontLayout, 0 );
			if ( playout_line!=NULL )
			{
				pango_layout_set_text( pFontLayout, Text, playout_line->length );
				pango_layout_get_pixel_size( pFontLayout, &SizeX, &SizeY );
			}
		}
		// vertical centering
		if ( SizeY<Height )
			PosiY = BaseY + (Height-SizeY)/2;
	}
	cairo_move_to( cr, BaseX, PosiY );
	pango_cairo_show_layout( cr, pFontLayout );
	return TotalHeightRequired;
}
// draw a text centered
int DrawPangoText( cairo_t * cr, int BaseX, int BaseY, int Width, int Height, char * Text ) 
{
	return DrawPangoTextOptions( cr, BaseX, BaseY, Width, Height, Text, /*CenterAlignment*/Width>0?TRUE:FALSE );
}

// if expr not drawed entirely, draw it complete in the right margin
// return report number full expression for margin (added after word OPERATE/COMPARE)
char * DrawExprForCompareOperate( cairo_t * cr, int BaseX, int BaseY, int Width, int Height, char * Text,char DrawingOption )
{
	static char tcBuffNumExpr[10];
	tcBuffNumExpr[ 0 ] = '\0';
	int NbrPixHeight = DrawPangoTextOptions( cr, BaseX, BaseY, Width, Height, Text, FALSE/*CenterAlignment*/ );
	// expression not printed entirely ?
	if ( NbrPixHeight>0 )
	{
		if ( DrawingOption==DRAW_FOR_PRINT )
		{
			// print the expression entirely in the right margin
			char * pReportNumAndText = malloc( strlen(Text)+10 );
			if ( pReportNumAndText )
			{
				int Hgt;
				sprintf( pReportNumAndText, "(*%d) ", PrintRightMarginNumExpr );
				strcat( pReportNumAndText, Text );
				Hgt = DrawPangoTextOptions( cr, PrintRightMarginPosiX, PrintRightMarginExprPosiY, PrintRightMarginWidth, 0/*Height*/, pReportNumAndText, FALSE/*CenterAlignment*/ );
				PrintRightMarginExprPosiY += Hgt;
				free( pReportNumAndText );
			}
			sprintf( tcBuffNumExpr, "(*%d)", PrintRightMarginNumExpr++ );
		}
		else
		{
			strcpy( tcBuffNumExpr, "(...)" );
		}
	}
	return tcBuffNumExpr;
}

void DrawCommonElementForToolbar( cairo_t * cr,int x,int y,int Size,int NumElement )
{
	int SizeDiv2 = Size/2;
	int SizeDiv3 = Size/3;
	int SizeDiv4 = Size/4;
	cairo_set_source_rgb( cr, 0.0, 0.0, 0.0 );
	const double lgt_dashes[] = { 2.0 };
	switch(NumElement)
	{
		case EDIT_POINTER:
			my_cairo_draw_line(cr,
				x+SizeDiv4,y+SizeDiv4, x+Size-SizeDiv4,y+Size-SizeDiv4); /* \ */
			my_cairo_draw_line(cr,
				x+SizeDiv4,y+SizeDiv4, x+SizeDiv3,y+SizeDiv2);  /* | */
			my_cairo_draw_line(cr,
				x+SizeDiv4,y+SizeDiv4, x+SizeDiv2,y+SizeDiv3);  /* _ */
			my_cairo_draw_line(cr,
				x+SizeDiv3,y+SizeDiv2, x+SizeDiv2,y+SizeDiv3);
			break;
		case EDIT_ERASER:
			my_cairo_draw_line(cr,
				x+SizeDiv3,y+SizeDiv4, x+Size-SizeDiv4,y+SizeDiv4);
			my_cairo_draw_line(cr,
				x+SizeDiv4,y+Size-SizeDiv4, x+Size-SizeDiv3,y+Size-SizeDiv4);
			my_cairo_draw_line(cr,
				x+SizeDiv3,y+SizeDiv4, x+SizeDiv4,y+Size-SizeDiv4);
			my_cairo_draw_line(cr,
				x+Size-SizeDiv4,y+SizeDiv4, x+Size-SizeDiv3,y+Size-SizeDiv4);
			my_cairo_draw_line(cr,
				x+SizeDiv4 +2,y+Size-SizeDiv4 +2, x+Size-SizeDiv3 +2,y+Size-SizeDiv4 +2);
			my_cairo_draw_line(cr,
				x+Size-SizeDiv4 +2,y+SizeDiv4 +2, x+Size-SizeDiv3 +2,y+Size-SizeDiv4 +2);
			break;
		case EDIT_SELECTION:
			cairo_set_dash( cr, lgt_dashes, 1, 0);
			cairo_rectangle( cr, x+SizeDiv4, y+SizeDiv4, Size-2*SizeDiv4, Size-2*SizeDiv4 );
			cairo_stroke( cr );
			break;
		case EDIT_COPY:
		case EDIT_MOVE:
			cairo_rectangle( cr, x+SizeDiv4, y+SizeDiv4, SizeDiv2, SizeDiv2 );
			cairo_stroke( cr );
			if ( NumElement==EDIT_MOVE )
			{
				my_cairo_draw_line(cr,
					x+SizeDiv3,y+SizeDiv3, x+Size-SizeDiv3,y+SizeDiv2); /* \ */
				my_cairo_draw_line(cr,
					x+SizeDiv3,y+Size-SizeDiv3, x+Size-SizeDiv3,y+SizeDiv2);  /* / */
			}
			cairo_set_dash( cr, lgt_dashes, 1, 0);
			cairo_rectangle( cr, x+SizeDiv3, y+SizeDiv3, SizeDiv2, SizeDiv2 );
			cairo_stroke( cr );
			break;
		case EDIT_INVERT:
			my_cairo_draw_line(cr,
				x+SizeDiv4,y+SizeDiv4, x+SizeDiv2,y+Size-SizeDiv4); /* \ */
			cairo_set_dash( cr, lgt_dashes, 1, 0);
			my_cairo_draw_line(cr,
				x+Size-SizeDiv4,y+SizeDiv4, x+SizeDiv2,y+Size-SizeDiv4);  /* / */
			break;
	}
}

void my_cairo_line( cairo_t *cr, double x1, double y1, double x2, double y2 )
{
	cairo_move_to( cr, x1, y1 );
	cairo_line_to( cr, x2, y2 );
}
void my_cairo_draw_line( cairo_t *cr, double x1, double y1, double x2, double y2 )
{
	cairo_move_to( cr, x1, y1 );
	cairo_line_to( cr, x2, y2 );
	cairo_stroke( cr );
}
void my_cairo_draw_color_line( cairo_t *cr, char cColor, double x1, double y1, double x2, double y2 )
{
	cairo_save( cr );
	if ( cColor )
		cairo_set_source_rgb( cr, 1.0, 0.13, 1.0 );
	else
		cairo_set_source_rgb( cr, 0.0, 0.0, 0.0 );
	my_cairo_draw_line( cr, x1, y1, x2, y2 );
	cairo_restore( cr );
}
void my_cairo_draw_black_rectangle( cairo_t *cr, double x, double y, double w, double h )
{
	cairo_save( cr );
	cairo_set_source_rgb( cr, 0.0, 0.0, 0.0 );
	cairo_rectangle( cr, x, y, w, h );
	cairo_stroke( cr );
	cairo_restore( cr );
}

char CharUnitForTimer( int BaseTimeValue )
{
	if ( BaseTimeValue==TIME_BASE_MINS)
		return 'm';
	else if ( BaseTimeValue==TIME_BASE_SECS )
		return 's';
	else if ( BaseTimeValue==TIME_BASE_100MS )
		return 't';
	else
		return '?';
}

char GetDrawDisplayWithColorState( char DrawingOption )
{
	return ( DrawingOption==DRAW_NORMAL && !EditDatas.ModeEdit && (InfosGene->LadderState==STATE_RUN || InfosGene->LadderState==STATE_RUN_FOR_ONE_CYCLE || InfosGene->LadderState==STATE_RUN_FREEZE || InfosGUI->TargetMonitor.RemoteConnected) );
}

void DrawElement( cairo_t * cr,int x,int y,int Width,int Height,StrElement * pTheElement,char DrawingOption )
{
	char BufTxt[50];
	int WidDiv2 = Width/2;
	int WidDiv3 = Width/3;
	int WidDiv4 = Width/4;
	int HeiDiv2 = Height/2;
	int HeiDiv3 = Height/3;
	int HeiDiv4 = Height/4;
	char DisplayColorState = GetDrawDisplayWithColorState( DrawingOption );

//Cairo....
#ifdef AAAAAAAAAAAAA
	GdkGC * DynaGcOff;
	GdkGC * TheGc;
	GdkColor DynaGdkColor;
	GdkGC * DynaGcOn;
	DynaGdkColor.pixel = 0xFF22FF;
	DynaGdkColor.red = 0xFF;
	DynaGdkColor.green = 0x22;
	DynaGdkColor.blue = 0xFF;

	DynaGcOn = gdk_gc_new(DrawPixmap);
	gdk_gc_set_foreground(DynaGcOn,&DynaGdkColor);
	#ifdef THICK_LINE_ELE_ACTIVATED
	gdk_gc_set_line_attributes(DynaGcOn, THICK_LINE_ELE_ACTIVATED,
		GDK_LINE_SOLID, GDK_CAP_BUTT, GDK_JOIN_MITER);
	#endif
	DynaGcOff = drawing_area->style->black_gc;
	/* State with color */
	TheGc = drawing_area->style->black_gc;
	if ( (DrawingOption==DRAW_NORMAL) && (!EditDatas.ModeEdit) && (pTheElement->DynamicState) )
		TheGc = DynaGcOn;
	if (EditDatas.ModeEdit || DrawingOption==DRAW_FOR_PRINT)
	{
		gdk_gc_unref(DynaGcOn);
		DynaGcOn = drawing_area->style->black_gc;
	}
#endif

	// draw rectangle to see elements with their var "setted" !
	if ( DisplayColorState )
	{
		switch(pTheElement->Type)
		{
			case ELE_INPUT:
			case ELE_INPUT_NOT:
			case ELE_RISING_INPUT:
			case ELE_FALLING_INPUT:
			case ELE_OUTPUT:
			case ELE_OUTPUT_NOT:
			if ( pTheElement->DynamicVarSetted )
			{
				cairo_set_source_rgb( cr, 0.3, 0.3, 1.0 );
				cairo_rectangle( cr, x+2, y+HeiDiv4+2, Width-3, Height-2*HeiDiv4-5 );
				cairo_stroke( cr );
			}
		}
	}

//	cairo_save( cr );
	if ( DisplayColorState && pTheElement->DynamicState )
	{
		cairo_set_source_rgb( cr, 1.0, 0.13, 1.0 );
//		cairo_set_line_width( cr, 1.5*cairo_get_line_width( cr ) );
	}
	else
	{
		cairo_set_source_rgb( cr, 0.0, 0.0, 0.0 );
//		cairo_set_line_width( cr, 0.1*cairo_get_line_width( cr ) );
	}

	/* modified: drawing this ones first (before -  -) because trouble under Windows printing... */
	/* Drawing / */
	switch(pTheElement->Type)
	{
		case ELE_INPUT_NOT:
		case ELE_OUTPUT_NOT:
				my_cairo_draw_line( cr,
								x+WidDiv3,y+Height-HeiDiv4, x+WidDiv3*2,y+HeiDiv4 );
	}
	/* Drawing ^ or \/  */
	switch(pTheElement->Type)
	{
		case ELE_RISING_INPUT:
				my_cairo_draw_line( cr,
								x+WidDiv3,y+HeiDiv3*2, x+WidDiv4*2,y+HeiDiv3 );
				my_cairo_draw_line( cr,
								x+WidDiv4*2,y+HeiDiv3, x+WidDiv3*2,y+HeiDiv3*2 );
				break;
		case ELE_FALLING_INPUT:
				my_cairo_draw_line( cr,
								x+WidDiv3,y+HeiDiv3, x+WidDiv4*2,y+HeiDiv3*2 );
				my_cairo_draw_line( cr,
								x+WidDiv4*2,y+HeiDiv3*2, x+WidDiv3*2,y+HeiDiv3 );
				break;
	}

	/* Drawing - - */
	switch(pTheElement->Type)
	{
		case ELE_INPUT:
		case ELE_INPUT_NOT:
		case ELE_RISING_INPUT:
		case ELE_FALLING_INPUT:
		case ELE_OUTPUT:
		case ELE_OUTPUT_NOT:
		case ELE_OUTPUT_SET:
		case ELE_OUTPUT_RESET:
		case ELE_OUTPUT_JUMP:
		case ELE_OUTPUT_CALL:
				my_cairo_draw_line( cr,
								x,y+HeiDiv2, x+WidDiv3,y+HeiDiv2 );
				my_cairo_draw_line( cr,
								x+WidDiv3*2,y+HeiDiv2, x+Width,y+HeiDiv2 );
	}
	/* Drawing || or () or --- */
	switch(pTheElement->Type)
	{
		case ELE_INPUT:
		case ELE_INPUT_NOT:
		case ELE_RISING_INPUT:
		case ELE_FALLING_INPUT:
				my_cairo_draw_line( cr,
								x+WidDiv3,y+HeiDiv4, x+WidDiv3,y+Height-HeiDiv4);
				my_cairo_draw_line( cr,
								x+WidDiv3*2,y+HeiDiv4, x+WidDiv3*2,y+Height-HeiDiv4 );
				break;
		case ELE_CONNECTION:
				my_cairo_draw_line( cr,
								x,y+HeiDiv2, x+Width,y+HeiDiv2 );
				break;
		case ELE_OUTPUT:
		case ELE_OUTPUT_NOT:
		case ELE_OUTPUT_SET:
		case ELE_OUTPUT_RESET:
		case ELE_OUTPUT_JUMP:
		case ELE_OUTPUT_CALL:
				/* hide the too much of lines - - before drawing arcs */
//Cairo				gdk_draw_rectangle(DrawPixmap, (DrawingOption!=DRAW_FOR_TOOLBAR)?drawing_area->style->white_gc:drawing_area->style->bg_gc[0], TRUE,
//Cairo								x+WidDiv4,y+HeiDiv2-1, WidDiv2,3);
				/* draw the 2 arcs of the outputs */
//Cairo				gdk_draw_arc (DrawPixmap, TheGc, FALSE,
//Cairo								x+WidDiv4, y+HeiDiv4, WidDiv2, HeiDiv2, (90+20)*64, 150*64);
//Cairo				gdk_draw_arc (DrawPixmap, TheGc, FALSE,
//Cairo								x+WidDiv4, y+HeiDiv4, WidDiv2, HeiDiv2, (270+20)*64, 150*64);
cairo_save (cr);
cairo_translate (cr, x + WidDiv2, y + HeiDiv2);
cairo_scale (cr, WidDiv4-4, HeiDiv4);
//cairo_arc (cr, 0, 0, 1., 0., 2 * M_PI);
cairo_new_sub_path( cr );
cairo_arc (cr, 0, 0, 1., M_PI/2.+0.3, M_PI+M_PI/2.-0.3);
cairo_new_sub_path( cr );
cairo_arc (cr, 0, 0, 1., M_PI+M_PI/2.+0.3, M_PI/2-0.3 );
cairo_restore (cr);
cairo_stroke( cr );
				break;
	}

	/* Drawing 'S'et or 'R'eset or 'J'ump or 'C'all for outputs */
	switch(pTheElement->Type)
	{
		case ELE_OUTPUT_SET:
			DrawPangoText( cr, x, y, Width, Height, "S" );
			break;
		case ELE_OUTPUT_RESET:
			DrawPangoText( cr, x, y, Width, Height, "R" );
			break;
		case ELE_OUTPUT_JUMP:
			DrawPangoText( cr, x, y, Width, Height, "J" );
			break;
		case ELE_OUTPUT_CALL:
			DrawPangoText( cr, x, y, Width, Height, "C" );
			break;
	}

	/* Drawing Var */
	if (DrawingOption!=DRAW_FOR_TOOLBAR)
	{
		switch(pTheElement->Type)
		{
			case ELE_INPUT:
			case ELE_INPUT_NOT:
			case ELE_RISING_INPUT:
			case ELE_FALLING_INPUT:
			case ELE_OUTPUT:
			case ELE_OUTPUT_NOT:
			case ELE_OUTPUT_SET:
			case ELE_OUTPUT_RESET:
//				strcpy(BufTxt,CreateVarName(Element.VarType,Element.VarNum));
				CreateVarNameForElement( BufTxt, pTheElement, Preferences.DisplaySymbolsInMainWindow/*InfosGene->DisplaySymbols*/ );
				DrawPangoText( cr, x, y+HeiDiv4+1, Width, -1, BufTxt );
				break;
			case ELE_OUTPUT_JUMP:
				DrawPangoText( cr, x, y+HeiDiv4+1, Width, -1, RungArray[pTheElement->VarNum].Label );
				break;
			case ELE_OUTPUT_CALL:
				sprintf( BufTxt, "SR%d", pTheElement->VarNum );
				DrawPangoText( cr, x, y+HeiDiv4+1, Width, -1, BufTxt );
				break;
		}
	}
	/* Drawing cnx with top */
	if (pTheElement->ConnectedWithTop)
	{
		if (pTheElement->DynamicInput && DisplayColorState )
			my_cairo_draw_color_line( cr, 1,
				x,y+HeiDiv2 +1, x,y-HeiDiv2 );
		else
			my_cairo_draw_color_line( cr, 0,
				x,y+HeiDiv2 +1, x,y-HeiDiv2 );
	}

	/* specials used for Editor */
	if (DrawingOption==DRAW_FOR_TOOLBAR)
	{
		switch(pTheElement->Type)
		{
			case EDIT_CNX_WITH_TOP:
				my_cairo_draw_color_line( cr, 0,
					x+WidDiv2,y+HeiDiv4, x+WidDiv2,y+Height-HeiDiv4 );
				break;
			case EDIT_LONG_CONNECTION:
				my_cairo_draw_line( cr,
								x,y+HeiDiv2, x+Width-1,y+HeiDiv2 );
				my_cairo_draw_line( cr,
								x+3*WidDiv4-1,y+HeiDiv4, x+Width-1,y+HeiDiv2 );
				my_cairo_draw_line( cr,
								x+3*WidDiv4-1,y+3*HeiDiv4, x+Width-1,y+HeiDiv2 );
				my_cairo_draw_line( cr,
								x+3*WidDiv4-1,y+HeiDiv4, x+3*WidDiv4 +3,y+HeiDiv2 );
				my_cairo_draw_line( cr,
								x+3*WidDiv4 +3,y+HeiDiv2, x+3*WidDiv4-1,y+3*HeiDiv4 );
				break;
			/* little display used for the toolbar */
			case ELE_TIMER:
			case ELE_MONOSTABLE:
			case ELE_COUNTER:
			case ELE_TIMER_IEC:
			case ELE_REGISTER:
				{
					char * Letter = "T";
					if ( pTheElement->Type==ELE_MONOSTABLE )
						Letter = "M";
					if ( pTheElement->Type==ELE_COUNTER )
						Letter = "C";
					if ( pTheElement->Type==ELE_TIMER_IEC )
						Letter = "TM";
					if ( pTheElement->Type==ELE_REGISTER )
						Letter = "R";
					my_cairo_draw_black_rectangle( cr,
									x/*+WidDiv4*/, y+HeiDiv4,
									Width-2/**WidDiv4*/, Height-2*HeiDiv4 );
					DrawPangoText( cr, x, y, Width, Height, Letter );
				}
				break;
			case ELE_COMPAR:
				my_cairo_draw_black_rectangle( cr,
									x+WidDiv4, y+HeiDiv4,
									Width-2*WidDiv4, Height-2*HeiDiv4 );
				DrawPangoText( cr, x, y, Width, Height, ">" );
				break;
			case ELE_OUTPUT_OPERATE:
				my_cairo_draw_black_rectangle( cr,
									x+WidDiv4, y+HeiDiv4,
									Width-2*WidDiv4, Height-2*HeiDiv4 );
				DrawPangoText( cr, x, y, Width, Height, "=" );
				break;
			default:
				DrawCommonElementForToolbar( cr, x, y, Width, pTheElement->Type );
				break;
		}
	}
	else
	{
		// only usefull to see abnormal elements in a rung (eraser bug in versions < 0.7.124)
		if ( pTheElement->Type>=EDIT_CNX_WITH_TOP )
		{
			sprintf( BufTxt, "(%d)", pTheElement->Type );
			DrawPangoText( cr, x, y, Width, Height, BufTxt );
		}
	}
//Cairo	if (!EditDatas.ModeEdit && DrawingOption!=DRAW_FOR_PRINT )
//Cairo		gdk_gc_unref(DynaGcOn);
//	cairo_restore( cr );
}

/* Drawing complex ones : Timer, Monostable, Compar, Operate - "alive" not for toolbar ! */
void DrawComplexElement( cairo_t * cr,int x,int y,int Width,int Height,int EleX,int EleY,StrRung * pTheRung,char DrawingOption )
{
	char BufTxt[50];
    char BufTxt2[50];
#ifdef OLD_TIMERS_MONOS_SUPPORT
	StrTimer * Timer;
	StrMonostable * Monostable;
#endif
//	StrCounter * Counter;
	StrTimerIEC * TimerIEC;
	int ModeValue;
//	int WidDiv2 = Width/2;
	int WidDiv3 = Width/3;
	int WidDiv4 = Width/4;
	int HeiDiv2 = Height/2;
	int HeiDiv3 = Height/3;
	int HeiDiv4 = Height/4;
	int Thickness = Width/16;
	char * ptcBuffNumExprMargin;
	char DisplayColorState = GetDrawDisplayWithColorState( DrawingOption );

	StrElement * pTheElement = &pTheRung->Element[EleX][EleY];

	switch(pTheElement->Type)
	{
#ifdef OLD_TIMERS_MONOS_SUPPORT
		case ELE_TIMER:
			Timer = &TimerArray[pTheElement->VarNum];
			/* the box */
//Cairo			gdk_draw_rectangle(DrawPixmap, drawing_area->style->white_gc, TRUE,
//								x+WidDiv3-Width, y+HeiDiv3,
//								Width+1*WidDiv3, Height+1*HeiDiv3);
			my_cairo_draw_black_rectangle( cr,
								x+WidDiv3-Width, y+HeiDiv3,
								Width+1*WidDiv3, Height+1*HeiDiv3 );
			/* input : enable */
			my_cairo_draw_color_line( cr, pTheRung->Element[EleX-1][EleY].DynamicInput/*Timer->InputEnable*/ && DisplayColorState,
								x-Width,y+HeiDiv2, x-Width+WidDiv3,y+HeiDiv2 );
			DrawPangoText( cr, x-Width+2, y+HeiDiv2-1, -1, -1, "E" );
			/* input : control */
			my_cairo_draw_color_line( cr, pTheRung->Element[EleX-1][EleY+1].DynamicInput/*Timer->InputControl*/ && DisplayColorState,
								x-Width,y+HeiDiv2+Height, x-Width+WidDiv3,y+HeiDiv2+Height );
			DrawPangoText( cr, x-Width+2, y+HeiDiv2-1+Height, -1, -1, "C" );
			/* output : done */
			my_cairo_draw_color_line( cr, pTheRung->Element[EleX][EleY].DynamicOutput/*Timer->OutputDone*/ && DisplayColorState,
								x+WidDiv3*2,y+HeiDiv2, x+Width,y+HeiDiv2 );
			DrawPangoText( cr, x+Width-WidDiv4,y+HeiDiv2-1, -1, -1, "D" );
			/* output : running */
			my_cairo_draw_color_line( cr, pTheRung->Element[EleX][EleY+1].DynamicOutput/*Timer->OutputRunning*/ && DisplayColorState,
								x+WidDiv3*2,y+HeiDiv2+Height, x+Width,y+HeiDiv2+Height );
			DrawPangoText( cr, x+Width-WidDiv4,y+HeiDiv2-1+Height, -1, -1, "R" );
			/* Timer Number */
			sprintf( BufTxt,"%cT%d", '%', pTheElement->VarNum );
			if ( Preferences.DisplaySymbolsInMainWindow/*InfosGene->DisplaySymbols*/ )
			{
				StrSymbol * SymbolName = ConvVarNameInSymbolPtr( BufTxt );
				if ( SymbolName )
					strcpy( BufTxt, SymbolName->Symbol );
			}
			DrawPangoText( cr, x+WidDiv3-Width,y+HeiDiv4+2, (Width-WidDiv3)*2, -1, BufTxt );
			/* Current Value (or Preset if print/edit) */
			if ( DrawingOption!=DRAW_FOR_PRINT && !EditDatas.ModeEdit )
				sprintf(BufTxt,Timer->DisplayFormat,(float)Timer->Value/(float)Timer->Base);
			else
				sprintf(BufTxt,Timer->DisplayFormat,(float)Timer->Preset/(float)Timer->Base);
			DrawPangoText( cr, x-Width, y, Width*2, Height*2, BufTxt );
			break;

		case ELE_MONOSTABLE:
			Monostable = &MonostableArray[pTheElement->VarNum];
			/* the box */
//Cairo			gdk_draw_rectangle(DrawPixmap, drawing_area->style->white_gc, TRUE,
//								x+WidDiv3-Width, y+HeiDiv3,
//								Width+1*WidDiv3, Height+1*HeiDiv3);
			my_cairo_draw_black_rectangle( cr,
								x+WidDiv3-Width, y+HeiDiv3,
								Width+1*WidDiv3, Height+1*HeiDiv3 );
			/* input */
			my_cairo_draw_color_line( cr, pTheRung->Element[EleX-1][EleY].DynamicInput/*Monostable->Input*/ && DisplayColorState,
								x-Width,y+HeiDiv2, x-Width+WidDiv3,y+HeiDiv2 );
			DrawPangoText( cr, x-Width,y+HeiDiv2-1, -1, -1, "I^" );
			/* output : running */
			my_cairo_draw_color_line( cr, pTheRung->Element[EleX][EleY].DynamicOutput/*Monostable->OutputRunning*/ && DisplayColorState,
								x+WidDiv3*2,y+HeiDiv2, x+Width,y+HeiDiv2 );

			DrawPangoText( cr, x+Width-WidDiv4,y+HeiDiv2-1, -1, -1, "R" );
			/* Monostable Number */
			sprintf( BufTxt,"%cM%d", '%', pTheElement->VarNum );
			if ( Preferences.DisplaySymbolsInMainWindow/*InfosGene->DisplaySymbols*/ )
			{
				StrSymbol * SymbolName = ConvVarNameInSymbolPtr( BufTxt );
				if ( SymbolName )
					strcpy( BufTxt, SymbolName->Symbol );
			}
			DrawPangoText( cr, x+WidDiv3-Width,y+HeiDiv4+2, (Width-WidDiv3)*2, -1, BufTxt );
			/* Current Value (or Preset if print/edit) */
			if ( DrawingOption!=DRAW_FOR_PRINT && !EditDatas.ModeEdit )
				sprintf(BufTxt,Monostable->DisplayFormat,(float)Monostable->Value/(float)Monostable->Base);
			else
				sprintf(BufTxt,Monostable->DisplayFormat,(float)Monostable->Preset/(float)Monostable->Base);
			DrawPangoText( cr, x-Width, y, Width*2, Height*2, BufTxt );
			break;
#endif

		case ELE_COUNTER:
//			Counter = &CounterArray[pTheElement->VarNum];
			/* the box */
//Cairo			gdk_draw_rectangle(DrawPixmap, drawing_area->style->white_gc, TRUE,
//								x+WidDiv3-Width, y+HeiDiv3,
//								Width+1*WidDiv3, 3*Height+1*HeiDiv3);
			my_cairo_draw_black_rectangle( cr, 
								x+WidDiv3-Width, y+HeiDiv3,
								Width+1*WidDiv3, 3*Height+1*HeiDiv3 );
			/* input : reset */
			my_cairo_draw_color_line( cr, pTheRung->Element[EleX-1][EleY].DynamicInput/*Counter->InputReset*/ && DisplayColorState,
								x-Width,y+HeiDiv2, x-Width+WidDiv3,y+HeiDiv2 );
			DrawPangoText( cr, x-Width+WidDiv4/2, y+HeiDiv2-1, -1, -1, "R" );
			/* input : preset */
			my_cairo_draw_color_line( cr, pTheRung->Element[EleX-1][EleY+1].DynamicInput/*Counter->InputPreset*/ && DisplayColorState,
								x-Width,y+HeiDiv2+Height, x-Width+WidDiv3,y+HeiDiv2+Height );
			DrawPangoText( cr, x-Width+WidDiv4/2, y+HeiDiv2-1+Height, -1, -1, "P" );
			/* input : count up */
			my_cairo_draw_color_line( cr, pTheRung->Element[EleX-1][EleY+2].DynamicInput/*Counter->InputCountUp*/ && DisplayColorState,
								x-Width,y+HeiDiv2+Height*2, x-Width+WidDiv3,y+HeiDiv2+Height*2 );
			DrawPangoText( cr, x-Width+WidDiv4/2, y+HeiDiv2-1+Height*2, -1, -1, "U" );
			/* input : count down */
			my_cairo_draw_color_line( cr, pTheRung->Element[EleX-1][EleY+3].DynamicInput/*Counter->InputCountDown*/ && DisplayColorState,
								x-Width,y+HeiDiv2+Height*3, x-Width+WidDiv3,y+HeiDiv2+Height*3 );
			DrawPangoText( cr, x-Width+WidDiv4/2, y+HeiDiv2-1+Height*3, -1, -1, "D" );
			/* output : empty */
			my_cairo_draw_color_line( cr, pTheRung->Element[EleX][EleY].DynamicOutput/*Counter->OutputEmpty*/ && DisplayColorState,
								x+WidDiv3*2,y+HeiDiv2, x+Width,y+HeiDiv2 );
			DrawPangoText( cr, x+Width-WidDiv4,y+HeiDiv2-1, -1, -1, "E" );
			/* output : done */
			my_cairo_draw_color_line( cr, pTheRung->Element[EleX][EleY+1].DynamicOutput/*Counter->OutputDone*/ && DisplayColorState,
								x+WidDiv3*2,y+HeiDiv2+Height, x+Width,y+HeiDiv2+Height );
			DrawPangoText( cr, x+Width-WidDiv4,y+HeiDiv2-1+Height, -1, -1, "D" );
			/* output : full */
			my_cairo_draw_color_line( cr, pTheRung->Element[EleX][EleY+2].DynamicOutput/*Counter->OutputFull*/ && DisplayColorState,
								x+WidDiv3*2,y+HeiDiv2+Height*2, x+Width,y+HeiDiv2+Height*2 );
			DrawPangoText( cr, x+Width-WidDiv4,y+HeiDiv2-1+Height*2, -1, -1, "F" );
			/* Counter Number */
			sprintf( BufTxt,"%cC%d", '%', pTheElement->VarNum );
			if ( Preferences.DisplaySymbolsInMainWindow/*InfosGene->DisplaySymbols*/ )
			{
				StrSymbol * SymbolName = ConvVarNameInSymbolPtr( BufTxt );
				if ( SymbolName )
					strcpy( BufTxt, SymbolName->Symbol );
			}
			DrawPangoText( cr, x+WidDiv3-Width,y+HeiDiv4+2, (Width-WidDiv3)*2, -1, BufTxt );
            /* Current and Preset values */
			sprintf(BufTxt,"%d", ReadVar(VAR_COUNTER_VALUE,pTheElement->VarNum) /*Counter->Value*/);
            sprintf(BufTxt2,"%d",ReadVar(VAR_COUNTER_PRESET,pTheElement->VarNum) /*Counter->Preset*/);
			if ( DisplayColorState )
				DrawPangoText( cr, x-Width, y+HeiDiv2, Width*2, Height*2, BufTxt );
			DrawPangoText( cr, x-Width, y+HeiDiv2+HeiDiv3, Width*2, Height*2, BufTxt2 );
			break;

		case ELE_TIMER_IEC:
			TimerIEC = &NewTimerArray[pTheElement->VarNum];
			/* the box */
//Cairo			gdk_draw_rectangle(DrawPixmap, drawing_area->style->white_gc, TRUE,
//								x+WidDiv3-Width, y+HeiDiv3,
//								Width+1*WidDiv3, Height+1*HeiDiv3);
			my_cairo_draw_black_rectangle( cr,
								x+WidDiv3-Width, y+HeiDiv3,
								Width+1*WidDiv3, Height+1*HeiDiv3 );
			/* input : enable */
			my_cairo_draw_color_line( cr, pTheRung->Element[EleX-1][EleY].DynamicInput/*TimerIEC->Input*/ && DisplayColorState,
								x-Width,y+HeiDiv2, x-Width+WidDiv3,y+HeiDiv2 );
			DrawPangoText( cr, x-Width+WidDiv4/1.5, y+HeiDiv2-1, -1, -1, "I" );
			/* output : done */
			my_cairo_draw_color_line( cr, pTheRung->Element[EleX][EleY].DynamicOutput/*TimerIEC->Output*/ && DisplayColorState,
								x+WidDiv3*2,y+HeiDiv2, x+Width,y+HeiDiv2 );
			DrawPangoText( cr, x+Width-WidDiv4,y+HeiDiv2-1, -1, -1, "Q" );
			/* Timer IEC Number */
			sprintf( BufTxt,"%cTM%d", '%', pTheElement->VarNum );
			if ( Preferences.DisplaySymbolsInMainWindow/*InfosGene->DisplaySymbols*/ )
			{
				StrSymbol * SymbolName = ConvVarNameInSymbolPtr( BufTxt );
				if ( SymbolName )
					strcpy( BufTxt, SymbolName->Symbol );
			}
			DrawPangoText( cr, x+WidDiv3-Width,y+HeiDiv4+2, (Width-WidDiv3)*2, -1, BufTxt );
			/* Timer mode */
			ModeValue = TimerIEC->TimerMode;
			if ( ModeValue<NBR_TIMERSMODES )
				sprintf( BufTxt, "%s", TimersModesStrings[ ModeValue ] );
			else
				strcpy(BufTxt, "???" );
			DrawPangoText( cr, x-Width, y+HeiDiv3, Width*2, Height-HeiDiv3, BufTxt );

            /* Current and Preset values */
			sprintf(BufTxt,/*TimerIEC->DisplayFormat*/"%d%c", ReadVar(VAR_TIMER_IEC_VALUE,pTheElement->VarNum) /*TimerIEC->Value*/, CharUnitForTimer(TimerIEC->Base ));
            sprintf(BufTxt2,/*TimerIEC->DisplayFormat*/"%d%c", ReadVar(VAR_TIMER_IEC_PRESET,pTheElement->VarNum) /*TimerIEC->Preset*/, CharUnitForTimer(TimerIEC->Base));
			if ( DisplayColorState )
				DrawPangoText( cr, x-Width, y+Height+HeiDiv3, Width*2, -1/*Height-HeiDiv3*/, BufTxt );
			DrawPangoText( cr, x-Width, y+Height+2*HeiDiv3, Width*2, -1/*Height-HeiDiv3*/, BufTxt2 );
			break;

		case ELE_REGISTER:
			my_cairo_draw_black_rectangle( cr, 
								x+WidDiv3-Width, y+HeiDiv3,
								Width+1*WidDiv3, 2*Height+1*HeiDiv3 );
			/* input : reset */
			my_cairo_draw_color_line( cr, pTheRung->Element[EleX-1][EleY].DynamicInput && DisplayColorState,
								x-Width,y+HeiDiv2, x-Width+WidDiv3,y+HeiDiv2 );
			DrawPangoText( cr, x-Width+WidDiv4/2, y+HeiDiv2-1, -1, -1, "R" );
			/* input : put in */
			my_cairo_draw_color_line( cr, pTheRung->Element[EleX-1][EleY+1].DynamicInput && DisplayColorState,
								x-Width,y+HeiDiv2+Height, x-Width+WidDiv3,y+HeiDiv2+Height );
			DrawPangoText( cr, x-Width+WidDiv4/2, y+HeiDiv2-1+Height, -1, -1, "I" );
			/* input : put out */
			my_cairo_draw_color_line( cr, pTheRung->Element[EleX-1][EleY+2].DynamicInput && DisplayColorState,
								x-Width,y+HeiDiv2+Height*2, x-Width+WidDiv3,y+HeiDiv2+Height*2 );
			DrawPangoText( cr, x-Width+WidDiv4/2, y+HeiDiv2-1+Height*2, -1, -1, "O" );
			/* output : empty */
			my_cairo_draw_color_line( cr, pTheRung->Element[EleX][EleY].DynamicOutput && DisplayColorState,
								x+WidDiv3*2,y+HeiDiv2, x+Width,y+HeiDiv2 );
			DrawPangoText( cr, x+Width-WidDiv4,y+HeiDiv2-1, -1, -1, "E" );
			/* output : full */
			my_cairo_draw_color_line( cr, pTheRung->Element[EleX][EleY+1].DynamicOutput && DisplayColorState,
								x+WidDiv3*2,y+HeiDiv2+Height, x+Width,y+HeiDiv2+Height );
			DrawPangoText( cr, x+Width-WidDiv4,y+HeiDiv2-1+Height, -1, -1, "F" );
			/* Register Number */
			sprintf( BufTxt,"%cR%d", '%', pTheElement->VarNum );
			if ( Preferences.DisplaySymbolsInMainWindow/*InfosGene->DisplaySymbols*/ )
			{
				StrSymbol * SymbolName = ConvVarNameInSymbolPtr( BufTxt );
				if ( SymbolName )
					strcpy( BufTxt, SymbolName->Symbol );
			}
			DrawPangoText( cr, x+WidDiv3-Width,y+HeiDiv4+2, (Width-WidDiv3)*2, -1, BufTxt );
			/* Mode */
			ModeValue = RegisterArray[ pTheElement->VarNum ].RegisterMode;
			if ( ModeValue<NBR_REGISTERS_MODES )
				sprintf(BufTxt,"%s", RegistersModesStrings[ ModeValue ]);
			else
				strcpy(BufTxt, "???" );
			DrawPangoText( cr, x-Width, y+HeiDiv2+HeiDiv3, Width*2, Height*2, BufTxt );
            /* Current and Preset values */
			sprintf(BufTxt,"<%d>", ReadVar(VAR_REGISTER_NBR_VALUES,pTheElement->VarNum));
			DrawPangoText( cr, x-Width, y+Height+HeiDiv3, Width*2, -1/*Height-HeiDiv3*/, BufTxt );
			break;

		case ELE_COMPAR:
			/* the box */
//Cairo			gdk_draw_rectangle(DrawPixmap, drawing_area->style->white_gc, TRUE,
//								x+WidDiv3-(Width*2), y+HeiDiv4,
//								Width*2+1*WidDiv3, 2*HeiDiv4);
			my_cairo_draw_black_rectangle( cr, 
								x+WidDiv3-(Width*2), y+HeiDiv4,
								Width*2+1*WidDiv3, 2*HeiDiv4 );
			/* input */
			my_cairo_draw_color_line( cr, DisplayColorState && pTheElement->DynamicState,
								x-Width*2,y+HeiDiv2, x-Width*2+WidDiv3,y+HeiDiv2 );
			/* output */
			my_cairo_draw_color_line( cr, DisplayColorState && pTheElement->DynamicState,
								x+WidDiv3*2,y+HeiDiv2, x+Width,y+HeiDiv2 );
			/* arithmetic expression */
			if (!EditDatas.ModeEdit)
				strcpy(BufTxt,DisplayArithmExpr(ArithmExpr[pTheElement->VarNum].Expr,Preferences.DisplaySymbolsInMainWindow/*InfosGene->DisplaySymbols*/));
			else
				strcpy(BufTxt,DisplayArithmExpr(EditArithmExpr[pTheElement->VarNum].Expr,Preferences.DisplaySymbolsInMainWindow/*InfosGene->DisplaySymbols*/));
			ptcBuffNumExprMargin = DrawExprForCompareOperate( cr, x+WidDiv3-(Width*2)+2,y+HeiDiv4, Width*2+1*WidDiv3-Thickness, 2*HeiDiv4, BufTxt, DrawingOption );
			cairo_set_source_rgb( cr, 0.0, 0.0, 0.0 );
			sprintf( BufTxt, "COMPARE%s", ptcBuffNumExprMargin );
			DrawPangoText( cr, x+WidDiv4-(Width*2)+3, y+HeiDiv4+1, -1, -1, BufTxt );
			break;
		case ELE_OUTPUT_OPERATE:
			/* the box */
//Cairo			gdk_draw_rectangle(DrawPixmap, drawing_area->style->white_gc, TRUE,
//								x+WidDiv3-(Width*2), y+HeiDiv4,
//								Width*2+1*WidDiv3, 2*HeiDiv4);
			my_cairo_draw_black_rectangle( cr,
								x+WidDiv3-(Width*2), y+HeiDiv4,
								Width*2+1*WidDiv3, 2*HeiDiv4 );
			/* input */
			my_cairo_draw_color_line( cr, DisplayColorState && pTheElement->DynamicState,
								x-Width*2,y+HeiDiv2, x-Width*2+WidDiv3,y+HeiDiv2 );
			/* output */
			my_cairo_draw_color_line( cr, DisplayColorState && pTheElement->DynamicState,
								x+WidDiv3*2,y+HeiDiv2, x+Width,y+HeiDiv2 );
			/* arithmetic expression */
			if (!EditDatas.ModeEdit)
				strcpy(BufTxt,DisplayArithmExpr(ArithmExpr[pTheElement->VarNum].Expr,Preferences.DisplaySymbolsInMainWindow/*InfosGene->DisplaySymbols*/));
			else
				strcpy(BufTxt,DisplayArithmExpr(EditArithmExpr[pTheElement->VarNum].Expr,Preferences.DisplaySymbolsInMainWindow/*InfosGene->DisplaySymbols*/));
			ptcBuffNumExprMargin = DrawExprForCompareOperate( cr,x+WidDiv3-(Width*2)+2,y+HeiDiv4, Width*2+1*WidDiv3-Thickness, 2*HeiDiv4, BufTxt, DrawingOption );
			cairo_set_source_rgb( cr, 0.0, 0.0, 0.0 );
			sprintf( BufTxt, "OPERATE%s", ptcBuffNumExprMargin );
			DrawPangoText( cr, x+WidDiv4-(Width*2)+3, y+HeiDiv4+1, -1, -1, BufTxt );
			break;
	}
}

void DrawLeftRightBars( cairo_t * cr, int OffX, int PosiY, int BlockWidth, int BlockHeight, int HeaderLabelAndCommentHeight, int LeftRightBarsWidth, int IsTheCurrentRung )
{
	int LeftRightBarsHeight = HeaderLabelAndCommentHeight+BlockHeight*RUNG_HEIGHT;

	if (!IsTheCurrentRung )
	{
		cairo_set_source_rgb( cr, 0.0, 0.0, 0.0 );
	}
	else
	{
		// light yellow background for current rung
		cairo_set_source_rgb( cr, 1.0, 1.0, 0.85 );
		cairo_rectangle( cr, OffX+LeftRightBarsWidth, PosiY, BlockWidth*RUNG_WIDTH, LeftRightBarsHeight );
		cairo_fill( cr );
		cairo_set_source_rgb( cr, 0.93, 0.33, 0.33 );
	}
	cairo_rectangle( cr, OffX, PosiY, LeftRightBarsWidth, LeftRightBarsHeight );
	cairo_rectangle( cr, OffX+BlockWidth*RUNG_WIDTH+LeftRightBarsWidth, PosiY, LeftRightBarsWidth, LeftRightBarsHeight );
	cairo_fill( cr );
}

void DrawGrid( cairo_t * cr, int OffX, int PosiY )
{
	int x,y;
	PosiY += InfosGene->HeaderLabelCommentHeight;
	cairo_save( cr );
	cairo_set_line_width( cr, 0.25*cairo_get_line_width( cr ) );
	cairo_set_source_rgb( cr, 0.63, 0.82, 0.82 );
	for(x=InfosGene->BlockWidth; x<RUNG_WIDTH*InfosGene->BlockWidth; x=x+InfosGene->BlockWidth)
	{
		cairo_move_to( cr, x+OffX, PosiY );
		cairo_line_to( cr, x+OffX, TOTAL_PX_RUNG_HEIGHT+PosiY );
		cairo_stroke( cr );
	}
	for(y=InfosGene->BlockHeight; y<TOTAL_PX_RUNG_HEIGHT; y=y+InfosGene->BlockHeight)
	{
		cairo_move_to( cr, OffX, y+PosiY );
		cairo_line_to( cr, RUNG_WIDTH*InfosGene->BlockWidth+OffX, y+PosiY );
		cairo_stroke( cr );
	}
	cairo_restore( cr );
}

void DrawRungPartition( cairo_t * cr, int OffX, int PosiY )
{
	cairo_set_source_rgb( cr, 0.8, 0.8, 0.8 );
	cairo_move_to( cr, 0, PosiY );
	cairo_line_to( cr, RUNG_WIDTH*InfosGene->BlockWidth+OffX, PosiY );
	cairo_stroke( cr );
}

void DrawMarkedElement( cairo_t * cr, int OffX, int AddPosiY, int BlockWidth, int BlockHeight, int HeaderLabelAndCommentHeight, StrElePosiSize * pMarked, StrMarkColor *pColor)
{
	int Left, Top, Width, Height;
	if ( pMarked->SizeX>0 && pMarked->SizeY>0 )
	{
		Left = (pMarked->PosiX-pMarked->SizeX+1)*BlockWidth +OffX;
		Top = pMarked->PosiY*BlockHeight +AddPosiY +HeaderLabelAndCommentHeight;
		Width = pMarked->SizeX*BlockWidth;
		Height = pMarked->SizeY*BlockHeight;
		cairo_set_source_rgb( cr, pColor->ColorR, pColor->ColorG, pColor->ColorB );
		cairo_rectangle( cr, Left, Top, Width, Height );
		cairo_clip( cr );
		cairo_paint_with_alpha( cr, 0.3 );
		cairo_reset_clip( cr );
	}
}
/*void DrawCurrentElementEdited( cairo_t * cr, int OffX, int AddPosiY )
{
		int Left = (EditDatas.CurrentElementPosiX-EditDatas.CurrentElementSizeX+1)*InfosGene->BlockWidth +OffX;
		int Top = EditDatas.CurrentElementPosiY*InfosGene->BlockHeight +AddPosiY +InfosGene->HeaderLabelCommentHeight;
		int Width = EditDatas.CurrentElementSizeX*InfosGene->BlockWidth;
		int Height = EditDatas.CurrentElementSizeY*InfosGene->BlockHeight;
		cairo_set_source_rgb( cr, 0.99, 0.19, 0.19 );
//		cairo_rectangle( cr, Left, Top, Width, Height );
//		cairo_stroke( cr );
		cairo_rectangle( cr, Left, Top, Width, Height );
		cairo_clip( cr );
		cairo_paint_with_alpha( cr, 0.3 );
		cairo_reset_clip( cr );
	}
}*/
void DrawGhostZone( cairo_t * cr, int OffX, int AddPosiY )
{
	int Left = EditDatas.GhostZonePosiX*InfosGene->BlockWidth +OffX;
	int Top = EditDatas.GhostZonePosiY*InfosGene->BlockHeight +AddPosiY +InfosGene->HeaderLabelCommentHeight;
	int Width = EditDatas.GhostZoneSizeX*InfosGene->BlockWidth;
	int Height = EditDatas.GhostZoneSizeY*InfosGene->BlockHeight;
	cairo_save( cr );
	cairo_set_source_rgb( cr, 0.50, 0.20, 0.90 );
	if ( EditDatas.NumElementSelectedInToolBar==EDIT_SELECTION )
	{
		const double lgt_dashes[] = { 5.0 };
		cairo_set_dash( cr, lgt_dashes, 1, 0);
	}
	cairo_rectangle( cr, Left, Top, Width, Height );
	cairo_stroke( cr );
	cairo_restore( cr );
}

void DrawPathLabelCommentHeader( cairo_t * cr, StrRung * Rung, int OffX, int PosiY, int BlockWidth, int HeaderHeight, char Background, char DrawingOption )
{
	int HeaderBarHeight = HeaderHeight*70/100;
	int OffsetStartY = HeaderHeight*15/100;
	int Margin = BlockWidth/4;
	int Oblique = BlockWidth/2;
	PosiY = PosiY+OffsetStartY;
	if ( !Background )
		my_cairo_draw_line( cr, OffX+Margin+Oblique+(BlockWidth*2), PosiY, OffX+Margin+(BlockWidth*2), PosiY+HeaderBarHeight );
	cairo_move_to( cr, OffX+Margin+Oblique, PosiY  );
	cairo_line_to( cr, OffX+BlockWidth*RUNG_WIDTH-Margin, PosiY );
	cairo_line_to( cr, OffX+BlockWidth*RUNG_WIDTH-Margin-Oblique, PosiY+HeaderBarHeight );
	cairo_line_to( cr, OffX+Margin, PosiY+HeaderBarHeight );
	cairo_line_to( cr, OffX+Margin+Oblique, PosiY  );
	if ( Background )
	{
		cairo_fill( cr );
	}
	else
	{
		cairo_stroke( cr );
		DrawPangoTextOptions( cr, OffX+Margin+Oblique, PosiY, -1, HeaderBarHeight, Rung->Label, FALSE/*CenterAlignment*/ );
		DrawPangoTextOptions( cr, OffX+BlockWidth*3, PosiY, -1, HeaderBarHeight, Rung->Comment, FALSE/*CenterAlignment*/ );
	}
}
// called for each functions blocks displayed on screen (to eventually monitor later)
void AddFunctionBlockVarToMonitor( int VarTypeToMonitor, int VarNumToMonitor )
{
	InfosGUI->FunctionsBlocksVarsToMonitor[ NbrFunctionsBlocksVarsDisplayed ].VarType = VarTypeToMonitor;
	InfosGUI->FunctionsBlocksVarsToMonitor[ NbrFunctionsBlocksVarsDisplayed ].VarNum = VarNumToMonitor;
	NbrFunctionsBlocksVarsDisplayed++;
}
void DrawRung( cairo_t * cr, int NumRungToDraw, char cRungInEdit, int OffX, int PosiY, int BlockWidth, int BlockHeight, int HeaderLabelAndCommentHeight, char DrawingOption )
{
	int x,y;
	StrRung * PtrRung;
	PtrRung = &RungArray[ NumRungToDraw ];
	if( cRungInEdit )
		PtrRung = &EditDatas.Rung;
	PrintRightMarginExprPosiY = PosiY;
	PrintRightMarginNumExpr = 1;
	CreateFontPangoLayout( cr, BlockHeight, DrawingOption );
	// drawing label & comment at the top of this rung in a box
	if ( DrawingOption!=DRAW_FOR_PRINT )
	{
		cairo_set_source_rgb( cr, 0.92, 0.92, 0.92 );
		DrawPathLabelCommentHeader( cr, PtrRung, OffX, PosiY, BlockWidth, HeaderLabelAndCommentHeight, 1/*Background*/, DrawingOption );
	}
	cairo_set_source_rgb( cr, 0.0, 0.0, 0.0 );
	DrawPathLabelCommentHeader( cr, PtrRung, OffX, PosiY, BlockWidth, HeaderLabelAndCommentHeight, 0/*Background*/, DrawingOption );
	// drawing elements of the rung
	for (y=0;y<RUNG_HEIGHT;y++)
	{
		for(x=0;x<RUNG_WIDTH;x++)
		{
			StrElement * pElementToDraw = &PtrRung->Element[x][y];
			int TypeEle = pElementToDraw->Type;
			if ( TypeEle==ELE_TIMER || TypeEle==ELE_MONOSTABLE || TypeEle==ELE_COUNTER || TypeEle==ELE_TIMER_IEC || TypeEle==ELE_REGISTER || TypeEle==ELE_COMPAR || TypeEle==ELE_OUTPUT_OPERATE )
				DrawComplexElement( cr, x*BlockWidth+OffX, HeaderLabelAndCommentHeight+y*BlockHeight+PosiY, BlockWidth,BlockHeight, x, y, PtrRung, DrawingOption );
			else
				DrawElement( cr, x*BlockWidth+OffX, HeaderLabelAndCommentHeight+y*BlockHeight+PosiY, BlockWidth,BlockHeight, pElementToDraw, DrawingOption );
			//for monitor the values of functions blocks...
			switch(pElementToDraw->Type)
			{
#ifdef OLD_TIMERS_MONOS_SUPPORT
				case ELE_TIMER:
					AddFunctionBlockVarToMonitor( VAR_TIMER_PRESET, pElementToDraw->VarNum );
					AddFunctionBlockVarToMonitor( VAR_TIMER_VALUE, pElementToDraw->VarNum );
					break;
				case ELE_MONOSTABLE:
					AddFunctionBlockVarToMonitor( VAR_MONOSTABLE_PRESET, pElementToDraw->VarNum );
					AddFunctionBlockVarToMonitor( VAR_MONOSTABLE_VALUE, pElementToDraw->VarNum );
					break;
#endif
				case ELE_COUNTER:
					AddFunctionBlockVarToMonitor( VAR_COUNTER_PRESET, pElementToDraw->VarNum );
					AddFunctionBlockVarToMonitor( VAR_COUNTER_VALUE, pElementToDraw->VarNum );
					break;
				case ELE_TIMER_IEC:
					AddFunctionBlockVarToMonitor( VAR_TIMER_IEC_PRESET, pElementToDraw->VarNum );
					AddFunctionBlockVarToMonitor( VAR_TIMER_IEC_VALUE, pElementToDraw->VarNum );
					break;
				case ELE_REGISTER:
					AddFunctionBlockVarToMonitor( VAR_REGISTER_IN_VALUE, pElementToDraw->VarNum );
					AddFunctionBlockVarToMonitor( VAR_REGISTER_OUT_VALUE, pElementToDraw->VarNum );
					AddFunctionBlockVarToMonitor( VAR_REGISTER_NBR_VALUES, pElementToDraw->VarNum );
			}
			// show highlight all elements searched and with another color current one searched
			if ( SearchTestIfLadderEleMatch( pElementToDraw ) )
			{
				StrElePosiSize PosiSizeMatch;
				PosiSizeMatch.PosiX = x;
				PosiSizeMatch.PosiY = y;
				GetSizesOfAnElement( pElementToDraw->Type, &PosiSizeMatch.SizeX, &PosiSizeMatch.SizeY);
				if ( DrawingOption!=DRAW_FOR_PRINT && SearchTestIfOnCurrent( NumRungToDraw, x, y ) )
					DrawMarkedElement( cr, OffX, PosiY, BlockWidth, BlockHeight, HeaderLabelAndCommentHeight, &PosiSizeMatch, &ColorElementSearched );
				else
					DrawMarkedElement( cr, OffX, PosiY, BlockWidth, BlockHeight, HeaderLabelAndCommentHeight, &PosiSizeMatch, &ColorElementHighlighted );
			}
		}
	}
}

void DrawRungs( cairo_t * cr )
{
	int ScanRung = InfosGene->TopRungDisplayed;
	int ScanY = InfosGene->OffsetHiddenTopRungDisplayed;
	int FlagAddOrInsertRung = FALSE;
	int TheEnd = FALSE;
	int LeftRightBarsWidth = InfosGene->BlockWidth/16;
	int NbrRungsToMonitor = 0;

	// If we are editing and adding a rung, we set a flag to indicate
	// that there is an extra rung to add (before or after current rung)
	if ( EditDatas.ModeEdit && EditDatas.DoBeforeFinalCopy == MODE_ADD )
	{
		// beware not the first rung displayed at the top...
		if ( InfosGene->FirstRung!=InfosGene->CurrentRung )
		{
			if ( ScanRung!=InfosGene->LastRung )
				ScanRung = RungArray[ ScanRung ].NextRung;
			FlagAddOrInsertRung = TRUE;
		}
	}
	if ( EditDatas.ModeEdit && EditDatas.DoBeforeFinalCopy == MODE_INSERT )
		FlagAddOrInsertRung = TRUE;

	// Clean all
//Cairo	gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE,
//Cairo						0, 0, InfosGene->BlockWidth*RUNG_WIDTH+50, InfosGene->PageHeight+50);

	for (ScanY = InfosGene->OffsetHiddenTopRungDisplayed*-1; ScanY<InfosGene->PageHeight && !TheEnd; ScanY=ScanY+(TOTAL_PX_RUNG_HEIGHT))
	{
		char cInEdit = FALSE;
		// save datas of each rungs displayed for monitor (if used)
		if ( NbrRungsToMonitor<20 )
			InfosGUI->NumRungDisplayedToMonitor[ NbrRungsToMonitor++ ] = ScanRung;
		DrawLeftRightBars( cr, 0, ScanY, InfosGene->BlockWidth, InfosGene->BlockHeight, InfosGene->HeaderLabelCommentHeight, LeftRightBarsWidth, ScanRung==InfosGene->CurrentRung );
		/* displaying the current rung - in edit ? */
		if ( ScanRung==InfosGene->CurrentRung && EditDatas.ModeEdit )
		{
			if ( ( FlagAddOrInsertRung && EditDatas.DoBeforeFinalCopy==MODE_INSERT )
				|| ( !FlagAddOrInsertRung && EditDatas.DoBeforeFinalCopy==MODE_ADD )
				|| EditDatas.DoBeforeFinalCopy==MODE_MODIFY )
			{
				/* grid for edit mode and display the rung under edition */
				DrawGrid( cr, LeftRightBarsWidth, ScanY );
				cInEdit = TRUE;
			}
		}
		DrawRung( cr, ScanRung, cInEdit, LeftRightBarsWidth, ScanY, InfosGene->BlockWidth, InfosGene->BlockHeight, InfosGene->HeaderLabelCommentHeight, DRAW_NORMAL );
		// "alpha" draw background of the current selected element.
		if ( cInEdit && EditDatas.ElementUnderEdit!=NULL )
		{
			if ( EditDatas.CurrentElementPosiSize.SizeX>0 && EditDatas.CurrentElementPosiSize.SizeY>0 )
				//DrawCurrentElementEdited( cr, LeftRightBarsWidth, ScanY );
				DrawMarkedElement( cr, LeftRightBarsWidth, ScanY, InfosGene->BlockWidth, InfosGene->BlockHeight, InfosGene->HeaderLabelCommentHeight, &EditDatas.CurrentElementPosiSize, &ColorElementSelected );
		}
		// ghost zone
		if ( cInEdit && EditDatas.GhostZonePosiX!=-1 && EditDatas.GhostZonePosiY!=-1 && EditDatas.GhostZoneSizeX>0 && EditDatas.GhostZoneSizeY>0 )
			DrawGhostZone( cr, LeftRightBarsWidth, ScanY );

		// if we are adding or inserting a rung, it is as if we have 2 current rungs...
		// If inserting : display edited one, then the current.
		// if adding : display current, then the edited one.
		// Edited one is displayed at the current position (current before... well I hope
		// you still follow, because I'm not sure for me ;-) )
		if ( ! (ScanRung==InfosGene->CurrentRung  && FlagAddOrInsertRung) )
		{
			if ( ScanRung!=InfosGene->LastRung )
				ScanRung = RungArray[ ScanRung ].NextRung;
			else
				TheEnd = TRUE;
		}
		else
		{
			FlagAddOrInsertRung = FALSE;
		}
		DrawRungPartition( cr, LeftRightBarsWidth, ScanY );
		DrawRungPartition( cr, LeftRightBarsWidth, ScanY + TOTAL_PX_RUNG_HEIGHT );
	}
	// for monitor
	InfosGUI->NbrRungsDisplayedToMonitor = NbrRungsToMonitor;
}

void DrawCurrentSection( cairo_t * cr )
{
	//v0.9.20
	if( SectionArray==NULL )
		return;
		
//Cairo		GdkRectangle update_rect;
	int iCurrentLanguage = SectionArray[ InfosGene->CurrentSection ].Language;
	// for monitor
	NbrFunctionsBlocksVarsDisplayed = 0;
	if ( iCurrentLanguage==SECTION_IN_LADDER )
		DrawRungs( cr );
#ifdef SEQUENTIAL_SUPPORT
	if ( iCurrentLanguage==SECTION_IN_SEQUENTIAL )
		DrawSequentialPage( cr, SectionArray[ InfosGene->CurrentSection ].SequentialPage, SEQ_SIZE_DEF, 0 );
#endif
//Cairo	update_rect.x = 0;
//Cairo	update_rect.y = 0;
//Cairo	update_rect.width = GTK_WIDGET(drawing_area)->allocation.width;
//Cairo	update_rect.height = GTK_WIDGET(drawing_area)->allocation.height;
//Cairo	gtk_widget_draw (drawing_area, &update_rect);
	// for monitor
	InfosGUI->NbrFunctionsBlocksVarsToMonitor = NbrFunctionsBlocksVarsDisplayed;
}
