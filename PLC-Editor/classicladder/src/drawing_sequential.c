/* Classic Ladder Project */
/* Copyright (C) 2001-2018 Marc Le Douarain */
/* http://www.sourceforge.net/projects/classicladder */
/* http://sites.google.com/site/classicladder */
/* October 2002 */
/* ---------------------------------------- */
/* Draw sequential pages                    */
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
#include <gtk/gtk.h>
#include <pango/pango.h>

#include "classicladder.h"
#include "global.h"
#include "drawing.h"
#include "vars_names.h"
#include "search.h"
#include "edit_sequential.h"
#include "drawing_sequential.h"

extern StrMarkColor ColorElementSelected;
extern StrMarkColor ColorElementSearched;
extern StrMarkColor ColorElementHighlighted;

void DrawSeqStep( cairo_t * cr,int x,int y,int Size,StrStep * pStep,char DrawingOption )
{
	char BufTxt[50];
	int PxOffsetActiveStep = Size/6;

	/* State with color */
	char DisplayColorState = GetDrawDisplayWithColorState( DrawingOption );
	if ( DisplayColorState && pStep->Activated )
		cairo_set_source_rgb( cr, 1.0, 0.13, 1.0 );
	else
		cairo_set_source_rgb( cr, 0.0, 0.0, 0.0 );

	cairo_rectangle(cr,
			x+3, y+3,
			Size-5, Size-5);
	cairo_stroke( cr );
	// init step ? (double square)
	if ( pStep->InitStep )
	{
		cairo_set_source_rgb( cr, 0.0, 0.0, 0.0 );
		cairo_rectangle(cr,
				x+PxOffsetActiveStep, y+PxOffsetActiveStep,
				Size-2*PxOffsetActiveStep, Size-2*PxOffsetActiveStep);
		cairo_stroke( cr );
	}
	// step number
	sprintf(BufTxt,"%d",pStep->StepNumber);
	DrawPangoText( cr, x, y, Size, Size, BufTxt );
}

void DrawSeqTransition( cairo_t * cr,int PageNumber, int x,int y,int Size,int NumTransi/*StrTransition * pTransi*/,int TheHeaderSeqCommentHeight,char DrawingOption )
{
	int SizeDiv2 = Size/2;
	int SizeDiv3 = Size/3;
	int SizeDiv4 = Size/4;
	int SizeLittle = Size/16;
	char BufTxt[50];

	int ScanSwitch,ScanStep;
	int StepPosiX,StepPosiY;
	int TransiX;
	int NumDirectStepOnTop = -1;
	int NumDirectStepOnBottom = -1;
	int MoreLeftStepOnTop = -1;
	int MoreLeftStepOnBottom = -1;
	char DisplayColorState = GetDrawDisplayWithColorState( DrawingOption );

	StrSequential * Seq = Sequential;
	StrTransition * pTransi;
	// if in edit, use datas in edit...
	if ( EditDatas.ModeEdit )
		Seq = &EditSeqDatas;
	pTransi = &Seq->Transition[ NumTransi ];
	// lifts positions (not used if for print!)
	int ShiftX = InfosGene->HScrollValue;
	int ShiftY = InfosGene->VScrollValue;
	if ( DrawingOption==DRAW_FOR_PRINT )
	{
		ShiftX = 0;
		ShiftY = -1*TheHeaderSeqCommentHeight;
	}

	/* State with color */
	if ( DisplayColorState && pTransi->Activated )
		cairo_set_source_rgb( cr, 1.0, 0.13, 1.0 );
	else
		cairo_set_source_rgb( cr, 0.0, 0.0, 0.0 );

	//v0.9.100, search and memorize direct top/bottom step + more left, for usages later...
	for ( ScanStep=0; ScanStep<NBR_SWITCHS_MAX; ScanStep++ )
	{
		int NumStep;
		// search a direct step on top ?
		NumStep = pTransi->NumStepToDesactiv[ ScanStep ];
		if ( NumStep!=-1 )
		{
			StrStep * pStep = &Seq->Step[ NumStep ];
			if ( pStep->PosiX==pTransi->PosiX && pStep->PosiY==pTransi->PosiY-1 )
				NumDirectStepOnTop = ScanStep;
			if ( MoreLeftStepOnTop==-1 )
			{
				MoreLeftStepOnTop = NumStep;
			}
			else
			{
				StrStep * pMoreLeftStep = &Seq->Step[ MoreLeftStepOnTop ];
				if ( pStep->PosiX<pMoreLeftStep->PosiX && pStep->PosiY==pTransi->PosiY-1 )
					MoreLeftStepOnTop = NumStep;
			}
		}
		// search a direct step on bottom ?
		NumStep = pTransi->NumStepToActiv[ ScanStep ];
		if ( NumStep!=-1 )
		{
			StrStep * pStep = &Seq->Step[ NumStep ];
			if ( pStep->PosiX==pTransi->PosiX && pStep->PosiY==pTransi->PosiY+1 )
				NumDirectStepOnBottom = ScanStep;
			if ( MoreLeftStepOnBottom==-1 )
			{
				MoreLeftStepOnBottom = NumStep;
			}
			else
			{
				StrStep * pMoreLeftStep = &Seq->Step[ MoreLeftStepOnBottom ];
				if ( pStep->PosiX<pMoreLeftStep->PosiX && pStep->PosiY==pTransi->PosiY+1 )
					MoreLeftStepOnBottom = NumStep;
			}
		}
	}
	
	// -
	my_cairo_draw_line(cr,
			x+SizeDiv3+2, y+SizeDiv2,
			x+Size-SizeDiv3-2, y+SizeDiv2);

	// variable for transition
//to try to debug sequential...	strcpy(BufTxt, CreateVarName( pTransi->VarTypeCondi, pTransi->VarNumCondi, InfosGene->DisplaySymbols ) );
	BufTxt[0] = '\0';
	if ( EditDatas.ModeEdit )
		sprintf( BufTxt, "t%d:", NumTransi );
	strcpy(&BufTxt[strlen(BufTxt)], CreateVarName( pTransi->VarTypeCondi, pTransi->VarNumCondi, Preferences.DisplaySymbolsInMainWindow/*InfosGene->DisplaySymbols*/ ) );
	DrawPangoText( cr, x+Size-SizeDiv3-1, y+1, -1, Size, BufTxt );

	// | (top and/or bottom shorten if no direct step)
	my_cairo_draw_color_line(cr, 0,
			x+SizeDiv2, ((NumDirectStepOnTop==-1)?y+SizeDiv3:y)  -1,
			x+SizeDiv2, (NumDirectStepOnBottom==-1)?y+Size-SizeDiv3:y+Size+2);

	// multiple steps activation (more than 1 defined) ?
	if ( pTransi->NumStepToActiv[ 1 ]!=-1 )
	{
		char MultiStepsFound = FALSE;
		for ( ScanSwitch=0; ScanSwitch<NBR_SWITCHS_MAX; ScanSwitch++ )
		{
			int NumStep;
			NumStep = pTransi->NumStepToActiv[ ScanSwitch ];
			if ( NumStep!=-1 && NumStep!=NumDirectStepOnBottom )
			{
				int ValueX;
				MultiStepsFound = TRUE;
				StepPosiX = Seq->Step[ NumStep ].PosiX * Size - ShiftX;
				ValueX = StepPosiX+Size-SizeDiv3;
				if (  MoreLeftStepOnBottom!=-1 )
				{
					if ( NumStep==MoreLeftStepOnBottom )
						ValueX = StepPosiX+SizeDiv3;
				}
				// ===
				my_cairo_draw_color_line(cr, 0,
					x+SizeDiv3, y+Size-SizeDiv4+SizeLittle, ValueX/*StepPosiX+Size-SizeDiv3*/, y+Size-SizeDiv4+SizeLittle);
				my_cairo_draw_color_line(cr, 0,
					x+SizeDiv3, y+Size-SizeDiv4+2*SizeLittle+1, ValueX/*StepPosiX+Size-SizeDiv3*/, y+Size-SizeDiv4+2*SizeLittle+1);
//				my_cairo_draw_color_line(cr, 1,
//					x+SizeDiv3, y+Size-SizeDiv4+3, StepPosiX+Size-SizeDiv3, y+Size-SizeDiv4+3);
				// | with the step
				my_cairo_draw_color_line(cr, 0,
					StepPosiX+SizeDiv2, y+Size-SizeDiv4+2*SizeLittle, StepPosiX+SizeDiv2, y+Size+2);
			}
		}
		if ( MultiStepsFound )
		{
			// | (little on bottom, if no direct bottom step)
			if ( NumDirectStepOnBottom==-1 )
				my_cairo_draw_color_line(cr, 0,
					x+SizeDiv2, y+SizeDiv2, x+SizeDiv2, y+Size-SizeDiv4+SizeLittle);
		}
	}

	// multiple steps desactivation (more than 1 defined) ?
	if ( pTransi->NumStepToDesactiv[ 1 ]!=-1 )
	{
		char MultiStepsFound = FALSE;
		for ( ScanSwitch=0; ScanSwitch<NBR_SWITCHS_MAX; ScanSwitch++ )
		{
			int NumStep;
			NumStep = pTransi->NumStepToDesactiv[ ScanSwitch ];
			if ( NumStep!=-1 && NumStep!=NumDirectStepOnTop )
			{
				int ValueX;
				MultiStepsFound = TRUE;
				StepPosiX = Seq->Step[ NumStep ].PosiX * Size - ShiftX;
				ValueX = StepPosiX+Size-SizeDiv3;
				if (  MoreLeftStepOnTop!=-1 )
				{
					if ( NumStep==MoreLeftStepOnTop )
						ValueX = StepPosiX+SizeDiv3;
				}
				// ===
				my_cairo_draw_color_line(cr, 0,
					x+SizeDiv3, y+SizeDiv4-SizeLittle, ValueX /*StepPosiX+Size-SizeDiv3*/, y+SizeDiv4-SizeLittle);
				my_cairo_draw_color_line(cr, 0,
					x+SizeDiv3, y+SizeDiv4-2*SizeLittle-1, ValueX /*StepPosiX+Size-SizeDiv3*/, y+SizeDiv4-2*SizeLittle-1);
//				my_cairo_draw_color_line(cr, 1,
//					x+SizeDiv3, y+SizeDiv4-3, StepPosiX+Size-SizeDiv3, y+SizeDiv4-3);
				// | with the step
				my_cairo_draw_color_line(cr, 0,
					StepPosiX+SizeDiv2, y  -1, StepPosiX+SizeDiv2, y+SizeDiv4-2*SizeLittle);
			}
		}
		if ( MultiStepsFound )
		{
			// | (little on top, if no direct top step)
			if ( NumDirectStepOnTop==-1 )
				my_cairo_draw_color_line(cr, 0,
					x+SizeDiv2, y+SizeDiv4-SizeLittle, x+SizeDiv2, y+SizeDiv2);
		}
	}

	// transitions linked (start of OR)
	if ( pTransi->NumTransLinkedForStart[ 0 ]!=-1 )
	{
		for ( ScanSwitch=0; ScanSwitch<NBR_SWITCHS_MAX; ScanSwitch++ )
		{
			int NumScanTransi;
			NumScanTransi = pTransi->NumTransLinkedForStart[ ScanSwitch ];
			if ( NumScanTransi!=-1 )
			{
				TransiX = Seq->Transition[ NumScanTransi ].PosiX * Size - ShiftX;
				my_cairo_draw_color_line(cr, 0,
					x+SizeDiv2, y+SizeDiv3, TransiX+SizeDiv2, y+SizeDiv3);
			}
		}
		// | (little on top, under bottom-step desactivated, if no direct top step)
		if ( NumDirectStepOnTop==-1 )
		{
			int NumStep;
			NumStep = pTransi->NumStepToDesactiv[ 0 ];
			if ( NumStep!=-1 )
			{
				StepPosiX = Seq->Step[ NumStep ].PosiX * Size - ShiftX;
				my_cairo_draw_color_line(cr, 0,
					StepPosiX+SizeDiv2, y-1, StepPosiX+SizeDiv2, y+SizeDiv3);
			}
		}

	}

	// transitions linked (end of OR)
	if ( pTransi->NumTransLinkedForEnd[ 0 ]!=-1 )
	{
		for ( ScanSwitch=0; ScanSwitch<NBR_SWITCHS_MAX; ScanSwitch++ )
		{
			int NumScanTransi;
			NumScanTransi = pTransi->NumTransLinkedForEnd[ ScanSwitch ];
			if ( NumScanTransi!=-1 )
			{
				TransiX = Seq->Transition[ NumScanTransi ].PosiX * Size - ShiftX;
				my_cairo_draw_color_line(cr, 0,
					x+SizeDiv2, y+Size-SizeDiv3, TransiX+SizeDiv2, y+Size-SizeDiv3);
			}
		}
		// | (little on bottom, before bottom-step activated, if no direct top step)
		if ( NumDirectStepOnBottom==-1 )
		{
			int NumStep;
			NumStep = pTransi->NumStepToActiv[ 0 ];
			if ( NumStep!=-1 )
			{
				StepPosiX = Seq->Step[ NumStep ].PosiX * Size - ShiftX;
				my_cairo_draw_color_line(cr, 0,
					StepPosiX+SizeDiv2, y+Size-SizeDiv3, StepPosiX+SizeDiv2, y+Size+2);
			}
		}
	}

	// cross step ?
	if ( pTransi->NumStepToActiv[ 0 ]!=-1 && pTransi->NumStepToDesactiv[ 0 ]!=-1 )
	{
		StrStep * pStepActiv = &Seq->Step[ pTransi->NumStepToActiv[ 0 ] ];
		StrStep * pStepDesactiv = &Seq->Step[ pTransi->NumStepToDesactiv[ 0 ] ];
		StepPosiX = pStepActiv->PosiX;
		StepPosiY = pStepActiv->PosiY;
		if ( pTransi->PosiX!=StepPosiX || pTransi->PosiY+1!=StepPosiY )
		{
//v0.9.100			if ( pTransi->NumTransLinkedForEnd[ 0 ]==-1 )
			if ( pTransi->NumTransLinkedForEnd[ 0 ]==-1 && pTransi->NumStepToActiv[ 1 ]==-1 )
			{
				// draw v with active step number (below this transition)
				my_cairo_draw_color_line(cr, 0,
					x+SizeDiv3, y+Size-SizeDiv3, x+SizeDiv2, y+Size-SizeDiv3+SizeDiv4);
				my_cairo_draw_color_line(cr, 0,
					x+Size-SizeDiv3, y+Size-SizeDiv3, x+SizeDiv2, y+Size-SizeDiv3+SizeDiv4);
				my_cairo_draw_color_line(cr, 0,
					x+SizeDiv2, y+Size-SizeDiv3, x+SizeDiv2, y+Size-SizeDiv3+SizeDiv4);
				sprintf( BufTxt, "%d", pStepActiv->StepNumber );
				cairo_set_source_rgb( cr, 0.0, 0.0, 0.0 );
				DrawPangoText( cr, x,y+Size+SizeDiv2, Size, -1, BufTxt );


				// draw desactiv step number with v (above the cross activ step)
				StepPosiX = StepPosiX*Size - ShiftX;
				StepPosiY = StepPosiY*Size - ShiftY;
				// for first one, verify if not already a transition under it...
				if ( pStepActiv->OffDrawCrossStep==-1 )
				{
					int NumTransiExist = SearchTransiElement( Sequential, PageNumber, pStepActiv->PosiX, pStepActiv->PosiY-1 );
//printf("verify if transi exists: %d,%d = %d\n", pStepActiv->PosiX, pStepActiv->PosiY-1, NumTransiExist );
					if ( NumTransiExist==-1 )
					{
						pStepActiv->OffDrawCrossStep = 0;
						pStepActiv->OffDrawCrossStepOffsetY = SizeDiv2;
						// draw v
						my_cairo_draw_color_line(cr, 0,
							StepPosiX+SizeDiv3, StepPosiY-SizeDiv2, StepPosiX+SizeDiv2, StepPosiY-SizeDiv4);
						my_cairo_draw_color_line(cr, 0,
							StepPosiX+Size-SizeDiv3, StepPosiY-SizeDiv2, StepPosiX+SizeDiv2, StepPosiY-SizeDiv4);
						my_cairo_draw_color_line(cr, 0,
							StepPosiX+SizeDiv2, StepPosiY-SizeDiv4, StepPosiX+SizeDiv2, StepPosiY+2);
					}
					else
					{
						pStepActiv->OffDrawCrossStep = Size-SizeDiv4+2;
						pStepActiv->OffDrawCrossStepOffsetY = 0;
						// draw -< on right
						my_cairo_draw_color_line(cr, 0,
							StepPosiX+Size-SizeDiv4, StepPosiY-SizeDiv4, StepPosiX+Size, StepPosiY-SizeDiv4-SizeLittle);
						my_cairo_draw_color_line(cr, 0,
							StepPosiX+Size-SizeDiv4, StepPosiY-SizeDiv4, StepPosiX+Size, StepPosiY-SizeDiv4+SizeLittle);
						my_cairo_draw_color_line(cr, 0,
							StepPosiX+SizeDiv2, StepPosiY-SizeDiv4, StepPosiX+Size-SizeDiv4, StepPosiY-SizeDiv4);
					}
					sprintf( BufTxt, "%d", pStepDesactiv->StepNumber );
				}
				else
				{
					sprintf( BufTxt, ";%d", pStepDesactiv->StepNumber );
				}
				DrawPangoText( cr, StepPosiX+pStepActiv->OffDrawCrossStep,StepPosiY-pStepActiv->OffDrawCrossStepOffsetY, Size, -1, BufTxt );
//printf("CrossStep nbr=%d, offX=%d\n", pStepActiv->StepNumber, pStepActiv->OffDrawCrossStep );
				pStepActiv->OffDrawCrossStep = pStepActiv->OffDrawCrossStep+((pStepActiv->OffDrawCrossStep!=0)?3:0)+15; // TODO: add length in pixels of the text written
			}
		}
	}
//TODO: cross step for pTransi->NumStepToDesactiv[ 0 ]

}

void DrawSeqComment( cairo_t * cr,int x,int y,int Size,StrSeqComment * pSeqComment )
{
	char * BufTxt = pSeqComment->Comment;
	int Margin = Size/16;
	int Oblique = Size/5;
	cairo_set_source_rgb( cr, 0.58, 0.58, 0.95 );
	my_cairo_draw_line(cr, x+Margin +Oblique, y+Margin, x+Margin, y+Margin +Oblique);
	my_cairo_draw_line(cr, x+4*Size-Oblique, y+Size, x+4*Size, y+Size-Oblique);
	cairo_set_source_rgb( cr, 0.0, 0.0, 0.0 );
	cairo_rectangle(cr,
			x+Margin, y+Margin,
			4*Size-Margin, Size-Margin);
	cairo_stroke( cr );
//	DrawPangoTextWithOffset( cr, x, y, 4*Size, Size, BufTxt, 3 );
	DrawPangoText( cr, x, y, 4*Size, Size, BufTxt );
}

void DrawSequentialGrid( cairo_t * cr, int SeqPxSize )
{
	int x,y,z;
	cairo_set_source_rgb( cr, 0.96, 0.96, 0.96 );
	z = 0;
	for(y=0; y<=SeqPxSize*SEQ_PAGE_HEIGHT; y=y+SeqPxSize)
	{
		my_cairo_draw_line(cr, 0, y-InfosGene->VScrollValue, SeqPxSize*NBR_STEPS, y-InfosGene->VScrollValue );
		if ( z&1 )
		{
			for(x=0; x<=SeqPxSize*SEQ_PAGE_WIDTH; x=x+SeqPxSize)
				my_cairo_draw_line(cr, x-InfosGene->HScrollValue, y-InfosGene->VScrollValue, x-InfosGene->HScrollValue, y-InfosGene->VScrollValue+SeqPxSize );
		}
		z++;
	}
}

void DrawSequentialMarkedElement( cairo_t * cr, int SeqPxSize, int OffsetX, int OffsetY, StrElePosiSize * pMarked, StrMarkColor *pColor )
{
	if ( pMarked->SizeX>0 && pMarked->SizeY>0 )
	{
		cairo_set_source_rgb( cr, pColor->ColorR, pColor->ColorG, pColor->ColorB );
		cairo_rectangle( cr, pMarked->PosiX*SeqPxSize-OffsetX, pMarked->PosiY*SeqPxSize-OffsetY, pMarked->SizeX*SeqPxSize, pMarked->SizeY*SeqPxSize );
		cairo_clip( cr );
		cairo_paint_with_alpha( cr, 0.3 );
		cairo_reset_clip( cr );
	}
}

void DrawPathCommentHeaderSequential( cairo_t * cr, char * SeqComment, int OffX, int PosiY, int SeqPxSize, int HeaderHeight, char DrawingOption )
{
	int HeaderBarHeight = HeaderHeight*70/100;
	int OffsetStartY = HeaderHeight*15/100;
	int Margin = SeqPxSize/4;
	int Oblique = SeqPxSize/2;
//printf("Called %s\n",__FUNCTION__);
	PosiY = PosiY+OffsetStartY;
//	if ( !Background )
//		my_cairo_draw_line( cr, OffX+Margin+Oblique+(BlockWidth*2), PosiY, OffX+Margin+(BlockWidth*2), PosiY+HeaderBarHeight );
	cairo_move_to( cr, OffX+Margin+Oblique, PosiY  );
	cairo_line_to( cr, OffX+SeqPxSize*SEQ_PAGE_WIDTH-Margin, PosiY );
	cairo_line_to( cr, OffX+SeqPxSize*SEQ_PAGE_WIDTH-Margin-Oblique, PosiY+HeaderBarHeight );
	cairo_line_to( cr, OffX+Margin, PosiY+HeaderBarHeight );
	cairo_line_to( cr, OffX+Margin+Oblique, PosiY  );
//	if ( Background )
//	{
//		cairo_fill( cr );
//	}
//	else
	{
		cairo_stroke( cr );
		DrawPangoTextOptions( cr, OffX+Margin+Oblique, PosiY, -1, HeaderBarHeight, SeqComment, FALSE/*CenterAlignment*/ );
	}
}

void DrawSequentialPage( cairo_t * cr, int PageNbr, int SeqPxSize, char DrawingOption )
{
	int ScanStep;
	StrStep * pStep;
	int ScanTransi;
	StrTransition * pTransi;
	int ScanSeqComment;
	StrSequential * Seq = Sequential;
	// lifts positions (not used if for print!)
	int ShiftX = InfosGene->HScrollValue;
	int ShiftY = InfosGene->VScrollValue;
	int HeaderSeqCommentHeight = 0;
	if ( DrawingOption==DRAW_FOR_PRINT )
	{
		HeaderSeqCommentHeight = SeqPxSize*150/100;
		ShiftX = 0;
		ShiftY = 0;
		DrawPathCommentHeaderSequential( cr, Sequential->PageComment[PageNbr], 0, 0, SeqPxSize, HeaderSeqCommentHeight, DrawingOption );
		ShiftY = -1*HeaderSeqCommentHeight;
	}
	// if in edit, use datas in edit...
	if ( EditDatas.ModeEdit )
		Seq = &EditSeqDatas;
	CreateFontPangoLayout( cr, SeqPxSize, DrawingOption );
	// clean all
//	gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE /*filled*/,
//		0, 0, GTK_WIDGET(drawing_area)->allocation.width,GTK_WIDGET(drawing_area)->allocation.height);
	if ( EditDatas.ModeEdit )
		DrawSequentialGrid( cr, SeqPxSize );
	for( ScanStep=0; ScanStep<NBR_STEPS; ScanStep++ )
	{
		pStep = &Seq->Step[ ScanStep ];
		if ( pStep->NumPage==PageNbr )
		{
			pStep->OffDrawCrossStep = -1; //0;
			pStep->OffDrawCrossStepOffsetY = 0;
		}
	}
	// draw the steps of this page
	for( ScanStep=0; ScanStep<NBR_STEPS; ScanStep++ )
	{
		pStep = &Seq->Step[ ScanStep ];
		if ( pStep->NumPage==PageNbr )
			DrawSeqStep( cr, pStep->PosiX*SeqPxSize-ShiftX, pStep->PosiY*SeqPxSize-ShiftY, SeqPxSize, pStep, DrawingOption );
	}
	// draw the transitions of this page
	for( ScanTransi=0; ScanTransi<NBR_TRANSITIONS; ScanTransi++ )
	{
		pTransi = &Seq->Transition[ ScanTransi ];
		if ( pTransi->NumPage==PageNbr )
		{
			DrawSeqTransition( cr, PageNbr, pTransi->PosiX*SeqPxSize-ShiftX, pTransi->PosiY*SeqPxSize-ShiftY, SeqPxSize, ScanTransi/*pTransi*/, HeaderSeqCommentHeight, DrawingOption );
			// show highlight all elements searched and with another color current one searched
			if ( SearchTestIfSeqTransiMatch( pTransi ) )
			{
				StrElePosiSize PosiSizeMatch;
				PosiSizeMatch.PosiX = pTransi->PosiX;
				PosiSizeMatch.PosiY = pTransi->PosiY;
				PosiSizeMatch.SizeX = 1;
				PosiSizeMatch.SizeY = 1;
				if ( DrawingOption!=DRAW_FOR_PRINT && SearchTestIfOnCurrent( -1, pTransi->PosiX, pTransi->PosiY ) )
					DrawSequentialMarkedElement( cr, SeqPxSize, ShiftX, ShiftY, &PosiSizeMatch, &ColorElementSearched );
				else
					DrawSequentialMarkedElement( cr, SeqPxSize, ShiftX, ShiftY, &PosiSizeMatch, &ColorElementHighlighted );
			}
		}
	}
	// draw the comments
	for( ScanSeqComment=0; ScanSeqComment<NBR_SEQ_COMMENTS; ScanSeqComment++ )
	{
		StrSeqComment * pSeqComment = &Seq->SeqComment[ ScanSeqComment ];
		if ( pSeqComment->NumPage==PageNbr )
			DrawSeqComment( cr, pSeqComment->PosiX*SeqPxSize-ShiftX,
							pSeqComment->PosiY*SeqPxSize-ShiftY, SeqPxSize, pSeqComment);
	}
	if ( EditDatas.ModeEdit )
		DrawSequentialMarkedElement( cr, SeqPxSize, ShiftX, ShiftY, &EditDatas.CurrentElementPosiSize, &ColorElementSelected );
}

void DrawSeqElementForToolBar( cairo_t * cr, int x, int y, int Size, int NumElement )
{
	int SizeDiv2 = Size/2;
	int SizeDiv3 = Size/3;
	int SizeDiv4 = Size/4;
	cairo_set_source_rgb( cr, 0.0, 0.0, 0.0 );
	switch( NumElement )
	{
		case ELE_SEQ_STEP:
		case EDIT_SEQ_INIT_STEP:
			cairo_rectangle(cr,
				x+SizeDiv4, y+SizeDiv4,
				Size-2*SizeDiv4, Size-2*SizeDiv4);
			if( NumElement==EDIT_SEQ_INIT_STEP )
				cairo_rectangle(cr,
					x+SizeDiv4+2, y+SizeDiv4+2,
					Size-2*SizeDiv4-4, Size-2*SizeDiv4-4);
			cairo_stroke( cr );
			break;
		case ELE_SEQ_TRANSITION:
			my_cairo_draw_line(cr,
				x+SizeDiv2,y+SizeDiv4, x+SizeDiv2,y+Size-SizeDiv4);
			my_cairo_draw_line(cr,
				x+SizeDiv3,y+SizeDiv2, x+Size-SizeDiv3,y+SizeDiv2);
			break;
		case EDIT_SEQ_STEP_AND_TRANS:
			cairo_rectangle(cr,
				x+SizeDiv4, y+2,
				Size-2*SizeDiv4, SizeDiv2-2);
			cairo_stroke( cr );
			my_cairo_draw_line(cr,
				x+SizeDiv2,y+SizeDiv2, x+SizeDiv2,y+Size-2);
			my_cairo_draw_line(cr,
				x+SizeDiv3,y+Size-SizeDiv4, x+Size-SizeDiv3,y+Size-SizeDiv4);
			break;
		case EDIT_SEQ_START_MANY_TRANS:
			my_cairo_draw_line(cr,
				x+2,y+SizeDiv4, x+Size-2,y+SizeDiv4);
			my_cairo_draw_line(cr,
				x+SizeDiv4,y+SizeDiv4-4, x+SizeDiv4,y+SizeDiv4+4);
			my_cairo_draw_line(cr,
				x+Size-SizeDiv4,y+SizeDiv4, x+Size-SizeDiv4,y+SizeDiv4+4);
			break;
		case EDIT_SEQ_END_MANY_TRANS:
			my_cairo_draw_line(cr,
				x+2,y+Size-SizeDiv4, x+Size-2,y+Size-SizeDiv4);
			my_cairo_draw_line(cr,
				x+SizeDiv4,y+Size-SizeDiv4-4, x+SizeDiv4,y+Size-SizeDiv4+4);
			my_cairo_draw_line(cr,
				x+Size-SizeDiv4,y+Size-SizeDiv4, x+Size-SizeDiv4,y+Size-SizeDiv4-4);
			break;
		case EDIT_SEQ_START_MANY_STEPS:
			my_cairo_draw_line(cr,
				x+2,y+SizeDiv4, x+Size-2,y+SizeDiv4);
			my_cairo_draw_line(cr,
				x+2,y+SizeDiv4+2, x+Size-2,y+SizeDiv4+2);
			my_cairo_draw_line(cr,
				x+SizeDiv4,y+SizeDiv4-4, x+SizeDiv4,y+SizeDiv4+4);
			my_cairo_draw_line(cr,
				x+Size-SizeDiv4,y+SizeDiv4+2, x+Size-SizeDiv4,y+SizeDiv4+4);
			break;
		case EDIT_SEQ_END_MANY_STEPS:
			my_cairo_draw_line(cr,
				x+2,y+Size-SizeDiv4, x+Size-2,y+Size-SizeDiv4);
			my_cairo_draw_line(cr,
				x+2,y+Size-SizeDiv4-2, x+Size-2,y+Size-SizeDiv4-2);
			my_cairo_draw_line(cr,
				x+SizeDiv4,y+Size-SizeDiv4-4, x+SizeDiv4,y+Size-SizeDiv4+4);
			my_cairo_draw_line(cr,
				x+Size-SizeDiv4,y+Size-SizeDiv4-2, x+Size-SizeDiv4,y+Size-SizeDiv4-4);
			break;
		case EDIT_SEQ_LINK:
			my_cairo_draw_line(cr,
				x+SizeDiv3,y+SizeDiv4, x+Size-SizeDiv3,y+Size-SizeDiv4);
//Cairo			gdk_draw_arc(DrawPixmap, drawing_area->style->black_gc, FALSE/*filled*/,
//				x+SizeDiv3-2,y+SizeDiv4-2, 4, 4, 0/*angle1*/, 23040/*angle2*/);
//			gdk_draw_arc(DrawPixmap, drawing_area->style->black_gc, FALSE/*filled*/,
//				x+Size-SizeDiv3-2,y+Size-SizeDiv4-2, 4, 4, 0/*angle1*/, 23040/*angle2*/);
			break;
		case ELE_SEQ_COMMENT:
			cairo_rectangle(cr,
				x+SizeDiv4, y+SizeDiv4,
				Size-2*SizeDiv4, Size-2*SizeDiv4);
			cairo_stroke( cr );
			DrawPangoText( cr, x, y, Size, Size, "xx" );
			break;
		default:
			DrawCommonElementForToolbar( cr, x, y, Size, NumElement );
			break;
	}
}
