/* Classic Ladder Project */
/* Copyright (C) 2001-2012 Marc Le Douarain */
/* http://membres.lycos.fr/mavati/classicladder/ */
/* http://www.sourceforge.net/projects/classicladder */
/* September 2011 */
/* ----------------------------------------------- */
/* Search/GoTo module contribution by Heli Tejedor */
/* helitp At arrakis DoT es */
/* http://heli.xbot.es */
/* Highlight possibility, previous direction and search in sequential */
/* added by Marc Le Douarain */
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
#include <libintl.h> // i18n
#include <locale.h> // i18n

#include "classicladder.h"
#include "global.h"

#include "vars_names.h"
#include "classicladder_gtk.h"
#include "edit.h"
#include "manager_gtk.h"
#include "search.h"


int SearchTypes[NBR_SEARCH_TYPES]={ SEARCH_ALL, SEARCH_CONTACTS, SEARCH_COILS, SEARCH_COMPARE, SEARCH_OPERATE, SEARCH_TRANSITIONS, SEARCH_BLOCKS };
char *SearchTypesNames[NBR_SEARCH_TYPES]={ N_("All except blocks"), N_("Contacts"), N_("Coils"), N_("Compare"), N_("Operate"), N_("Transitions"), N_("Blocks") };

StrSearchDatas SearchDatas;
char PassCurrentOne;

void InitSearchDatas( void )
{
	SearchDatas.CurSearchSection = -1;
	SearchDatas.ElementOrSearchType = ELE_FREE;
}

void GoToFirstRung( void )
{
	if ( SectionArray[ InfosGene->CurrentSection ].Language==SECTION_IN_LADDER )
	{
		if ( InfosGene->CurrentRung!=InfosGene->FirstRung )
		{
//for move tests			InfosGene->CurrentRung = RungArray[ InfosGene->CurrentRung ].PrevRung;
			InfosGene->CurrentRung = InfosGene->FirstRung;
			UpdateVScrollBar( FALSE/*AutoSelectCurrentRung*/ );
		}
	}
}
void GoToLastRung( void )
{
	if ( SectionArray[ InfosGene->CurrentSection ].Language==SECTION_IN_LADDER )
	{
		if ( InfosGene->CurrentRung!=InfosGene->LastRung )
		{
//for move tests			InfosGene->CurrentRung = RungArray[ InfosGene->CurrentRung ].NextRung;
			InfosGene->CurrentRung = InfosGene->LastRung;
			UpdateVScrollBar( FALSE/*AutoSelectCurrentRung*/ );
		}
	}
}

void GoToSectionAndRung( int SectionToGo, int RungToGo )
{ 
	StrSection * pSection;
	
//	printf("goto Section:%d Rung:%d\n", InfosGene->CurrentSection, InfosGene->CurrentRung);
	if (SectionToGo!=-1 && InfosGene->CurrentSection!=SectionToGo )
	{
		InfosGene->CurrentSection = SectionToGo;
		pSection = &SectionArray[ InfosGene->CurrentSection ];
		InfosGene->FirstRung = pSection->FirstRung;
		InfosGene->LastRung = pSection->LastRung; 
		InfosGene->CurrentRung = pSection->FirstRung; 
		ChangeSectionSelectedInManager( InfosGene->CurrentSection );
	}
	if (RungToGo!=-1)
		InfosGene->CurrentRung = RungToGo;
	UpdateVScrollBar( FALSE/*AutoSelectCurrentRung*/ );
}


/* =========================================================================*/

char SearchInArithmExpr( char *Expr )
{
	char Found = FALSE;
	char Buff[ 50 ];
	sprintf( Buff, "@%d/%d@", SearchDatas.VarType, SearchDatas.VarNum );
//printf("Test Arithm, search=%s, expr=%s\n", Buff, Expr );
	if ( strstr( Expr,Buff )!=NULL )
		Found = TRUE;
	return Found;
}

char SearchTestIfLadderEleMatch( StrElement * pEle )
{
	if ( SearchDatas.ElementOrSearchType==ELE_FREE )
		return FALSE;
	switch (SearchDatas.ElementOrSearchType)
	{
		case SEARCH_ALL:
		case SEARCH_CONTACTS:
		case ELE_INPUT:
		case ELE_INPUT_NOT:
		case ELE_RISING_INPUT:
		case ELE_FALLING_INPUT:
			if ( (pEle->Type==ELE_INPUT || pEle->Type==ELE_INPUT_NOT || pEle->Type==ELE_RISING_INPUT || pEle->Type==ELE_FALLING_INPUT)
				&& (pEle->VarType==SearchDatas.VarType)
				&& (pEle->VarNum==SearchDatas.VarNum) )
			{
				return TRUE;
			}
			
			if ( SearchDatas.ElementOrSearchType!=SEARCH_ALL) // go on with coil case ?
				break;
		
		case SEARCH_COILS:
		case ELE_OUTPUT:
		case ELE_OUTPUT_NOT:
		case ELE_OUTPUT_SET:
		case ELE_OUTPUT_RESET:
			if ( (pEle->Type==ELE_OUTPUT || pEle->Type==ELE_OUTPUT_NOT || pEle->Type==ELE_OUTPUT_SET || pEle->Type==ELE_OUTPUT_RESET)
				&& (pEle->VarType==SearchDatas.VarType)
				&& (pEle->VarNum==SearchDatas.VarNum) )
			{
				return TRUE;
			}
			if ( SearchDatas.ElementOrSearchType!=SEARCH_ALL) // go on with compare case ?
				break;
		
		case SEARCH_COMPARE:
		case ELE_COMPAR:
			if ( pEle->Type==ELE_COMPAR && SearchInArithmExpr( ArithmExpr[pEle->VarNum].Expr ) )
			{
				return TRUE;
			}
			if ( SearchDatas.ElementOrSearchType!=SEARCH_ALL) // go on with operate case ?
				break;
		
		case SEARCH_OPERATE:
		case ELE_OUTPUT_OPERATE:
			if ( pEle->Type==ELE_OUTPUT_OPERATE && SearchInArithmExpr( ArithmExpr[pEle->VarNum].Expr ) )
			{
				return TRUE;
			}
			break;
		
		case SEARCH_BLOCKS:
		case ELE_TIMER:
		case ELE_MONOSTABLE:
		case ELE_COUNTER:
		case ELE_TIMER_IEC:
			  if ( (pEle->Type==ELE_TIMER || pEle->Type==ELE_MONOSTABLE || pEle->Type==ELE_COUNTER || pEle->Type==ELE_TIMER_IEC )
			  	 && (pEle->VarNum==SearchDatas.VarNum) )
			{
				return TRUE;
			}
			break;
	}
	return FALSE;
}

char SearchTestIfSeqTransiMatch( StrTransition * pTransi )
{
	if ( SearchDatas.ElementOrSearchType==ELE_FREE )
		return FALSE;
	return ( pTransi->VarTypeCondi==SearchDatas.VarType && pTransi->VarNumCondi==SearchDatas.VarNum );
}

// Used to display current element on current rung with another color than the others marked (same element)
// DisplayedRung=-1 if test for sequential page
// Test on CurrentSection is necessary in the case of sequential.
char SearchTestIfOnCurrent( int DisplayedRung, int DisplayedPosX, int DisplayedPoxY )
{
	return ( InfosGene->CurrentSection==SearchDatas.CurSearchSection && ( DisplayedRung==SearchDatas.CurSearchRung || DisplayedRung==-1 ) && DisplayedPosX==SearchDatas.CurSearchPosX && DisplayedPoxY==SearchDatas.CurSearchPosY );
}

/* =========================================================================*/

// on which type of section to search: ladder or/and sequential ?
int GetMaskSectionsTypesSearched( )
{
	int MaskSectionsSearched = 0;
	if ( SearchDatas.ElementOrSearchType==SEARCH_ALL )
	{
		MaskSectionsSearched = (1<<SECTION_IN_LADDER) | (1<<SECTION_IN_SEQUENTIAL); 
	}
	else if ( SearchDatas.ElementOrSearchType==SEARCH_TRANSITIONS )
	{
		MaskSectionsSearched = (1<<SECTION_IN_SEQUENTIAL); 
	}
	else
	{
		MaskSectionsSearched = (1<<SECTION_IN_LADDER); 
	}
	return MaskSectionsSearched;
}

// search next ladder/sequential section...
void ScanSection( int Direction )
{
	StrSection * pScanSection;
	char SectionFound = FALSE;
printf( "%s( ): starting with Section=%d Rung=%d\n", __FUNCTION__, SearchDatas.CurSearchSection, SearchDatas.CurSearchRung );
	while( ( ( Direction>0 && SearchDatas.CurSearchSection<NBR_SECTIONS ) || ( Direction<0 && SearchDatas.CurSearchSection>=0 ) ) && !SectionFound )
	{ 
		pScanSection = &SectionArray[ SearchDatas.CurSearchSection ];
printf( "%s( ): test if section %d to check\n", __FUNCTION__, SearchDatas.CurSearchSection );
		if ( pScanSection->Used && (1<<pScanSection->Language)&GetMaskSectionsTypesSearched( ) )
		{
			SectionFound = TRUE;
		}
		else
		{
			SearchDatas.CurSearchSection += Direction;
		}
	}
	if ( SectionFound )
	{
		if ( pScanSection->Language==SECTION_IN_LADDER )
		{
			SearchDatas.CurSearchRung = (Direction>0)?pScanSection->FirstRung:pScanSection->LastRung;
			SearchDatas.CurSearchPosX = (Direction>0)?0:(RUNG_WIDTH-1);
			SearchDatas.CurSearchPosY = (Direction>0)?0:(RUNG_HEIGHT-1);
		}
#ifdef SEQUENTIAL_SUPPORT
		if ( pScanSection->Language==SECTION_IN_SEQUENTIAL )
		{
			SearchDatas.CurSearchRung = -1;
			SearchDatas.CurSearchPosX = (Direction>0)?0:(SEQ_PAGE_WIDTH-1);
			SearchDatas.CurSearchPosY = (Direction>0)?0:(SEQ_PAGE_HEIGHT-1);
		}
#endif
	}
	else
	{
		SearchDatas.CurSearchSection = -1; // no more section...
		pScanSection = NULL;
	}
printf( "%s( ): FoundAnotherSection=%s Section=%d(%s) Rung=%d\n", __FUNCTION__, SectionFound?"yes":"no", SearchDatas.CurSearchSection, (pScanSection!=NULL)?((pScanSection->Language==SECTION_IN_LADDER)?"ladder":"seq"):"---", SearchDatas.CurSearchRung );
}

/* =========================================================================*/
char SearchInLadderSection( StrSection * pThisSection, int Direction )
{
	char CurrentSectionDone = FALSE;
	do						  // Rungs loop ScanRung
	{
		StrRung * Rung = &RungArray[ SearchDatas.CurSearchRung ];
printf("Searching in ladder Section=%d (fr=%d,lr=%d) Rung=%d Direction=%d\n",SearchDatas.CurSearchSection, pThisSection->FirstRung, pThisSection->LastRung, SearchDatas.CurSearchRung, Direction);
		do						// Columns loop ScanY
		{		 			  	
			do					  // Lines loop ScanX
			{
				StrElement * pEle = &Rung->Element[SearchDatas.CurSearchPosX][SearchDatas.CurSearchPosY];
//printf("search Rung=%d x=%d/y=%d direction=%d\n",SearchDatas.CurSearchRung, SearchDatas.CurSearchPosX,SearchDatas.CurSearchPosY,Direction);
				if ( SearchTestIfLadderEleMatch( pEle ) )
				{
					if ( !PassCurrentOne )
					{
						// Jump to section/rung of the current searched element
						GoToSectionAndRung( SearchDatas.CurSearchSection, SearchDatas.CurSearchRung );
						printf("### FOUND %s Section=%d LadderRung=%d X=%d Y=%d ###\n", GetElementPropertiesForStatusBar(pEle), 
							SearchDatas.CurSearchSection, SearchDatas.CurSearchRung, SearchDatas.CurSearchPosX, SearchDatas.CurSearchPosY);
						MessageInStatusBar( "" );
						return TRUE;
					}
					PassCurrentOne = FALSE;
				}
				SearchDatas.CurSearchPosX += Direction;				// Lines loop ScanX
			}
			while ( ( Direction>0 && SearchDatas.CurSearchPosX<RUNG_WIDTH ) || ( Direction<0 && SearchDatas.CurSearchPosX>=0 ) );
			SearchDatas.CurSearchPosX = (Direction>0)?0:(RUNG_WIDTH-1);
			SearchDatas.CurSearchPosY += Direction;				  // Columns loop ScanY
		}
		while ( ( Direction>0 && SearchDatas.CurSearchPosY<RUNG_HEIGHT ) || ( Direction<0 && SearchDatas.CurSearchPosY>=0 ) );
		SearchDatas.CurSearchPosY = (Direction>0)?0:(RUNG_HEIGHT-1);
		if ( Direction>0 )
		{
			if ( SearchDatas.CurSearchRung==pThisSection->LastRung )
				CurrentSectionDone = TRUE;
			else
				SearchDatas.CurSearchRung = RungArray[ SearchDatas.CurSearchRung ].NextRung;
		}
		else
		{
			if ( SearchDatas.CurSearchRung==pThisSection->FirstRung )
				CurrentSectionDone = TRUE;
			else
				SearchDatas.CurSearchRung = RungArray[ SearchDatas.CurSearchRung ].PrevRung;
		}
	}
	while ( !CurrentSectionDone );   // Rungs loop ScanRung
if ( CurrentSectionDone )
printf("Searching, end of this ladder section\n");
	return FALSE;
}

#ifdef SEQUENTIAL_SUPPORT
StrTransition * SearchTransiForCoord( int SearchPageNbr, int SearchCoordX, int SearchCoordY )
{
	int ScanTransi;
	StrTransition * pTransi;
	for( ScanTransi=0; ScanTransi<NBR_TRANSITIONS; ScanTransi++ )
	{
		pTransi = &Sequential->Transition[ ScanTransi ];
		if ( pTransi->NumPage==SearchPageNbr )
		{
			if ( pTransi->PosiX==SearchCoordX &&  pTransi->PosiY==SearchCoordY )
				return pTransi;
		}
	}
	return NULL;
}
char SearchInSequentialSection( StrSection * pThisSection, int Direction )
{
printf("Searching in sequential Section=%d SeqPage=%d Direction=%d\n",SearchDatas.CurSearchSection,  pThisSection->SequentialPage, Direction);
	do						// Columns loop ScanY
	{		 			  	
		do					  // Lines loop ScanX
		{
			StrTransition * pTransi = SearchTransiForCoord( pThisSection->SequentialPage, SearchDatas.CurSearchPosX, SearchDatas.CurSearchPosY );
			if ( pTransi )
			{
//printf("verify transition in SeqPage=%d x=%d/y=%d direction=%d\n",pThisSection->SequentialPage, SearchDatas.CurSearchPosX,SearchDatas.CurSearchPosY,Direction);
				if ( SearchTestIfSeqTransiMatch( pTransi ) )
				{
					if ( !PassCurrentOne )
					{
						// Jump to section/rung of the current searched element
						GoToSectionAndRung( SearchDatas.CurSearchSection, -1 );
						printf("### FOUND transition in Section=%d SeqPage=%d X=%d Y=%d ###\n", 
							SearchDatas.CurSearchSection, pThisSection->SequentialPage, SearchDatas.CurSearchPosX, SearchDatas.CurSearchPosY);
						MessageInStatusBar( "" );
						return TRUE;
					}
					PassCurrentOne = FALSE;
				}
			}
			SearchDatas.CurSearchPosX += Direction;				// Lines loop ScanX
		}
		while ( ( Direction>0 && SearchDatas.CurSearchPosX<SEQ_PAGE_WIDTH ) || ( Direction<0 && SearchDatas.CurSearchPosX>=0 ) );
		SearchDatas.CurSearchPosX = (Direction>0)?0:(SEQ_PAGE_WIDTH-1);
		SearchDatas.CurSearchPosY += Direction;				  // Columns loop ScanY
	}
	while ( ( Direction>0 && SearchDatas.CurSearchPosY<SEQ_PAGE_HEIGHT ) || ( Direction<0 && SearchDatas.CurSearchPosY>=0 ) );
	SearchDatas.CurSearchPosY = (Direction>0)?0:(SEQ_PAGE_HEIGHT-1);
	return FALSE;
}
#endif

void SearchAndGoToNextOrPrevElement( int Direction )
{
	StrSection * pScanSection;
	PassCurrentOne = !SearchDatas.NewSearchStarting;
	if ( SearchDatas.CurSearchSection==-1 )
		return;
	SearchDatas.NewSearchStarting = FALSE;
	do							// Sections loop ScanSection
	{
		pScanSection = &SectionArray[ SearchDatas.CurSearchSection ];

		if ( pScanSection->Language==SECTION_IN_LADDER )
		{
			if ( SearchInLadderSection( pScanSection, Direction ) )
				return;
		}
#ifdef SEQUENTIAL_SUPPORT
		if ( pScanSection->Language==SECTION_IN_SEQUENTIAL )
		{
			if ( SearchInSequentialSection( pScanSection, Direction ) )
				return;
		}
#endif

		SearchDatas.CurSearchSection += Direction;
		ScanSection( Direction );
	}
	while( SearchDatas.CurSearchSection!=-1 ); // Sections loop

	MessageInStatusBar( _("### NOT FOUND ###") );
	printf ("### NOT FOUND ###\n");
}

void SearchAndGoToNextElement( void )
{
	SearchAndGoToNextOrPrevElement( 1/*Direction*/ );
}
void SearchAndGoToPreviousElement( void )
{
	SearchAndGoToNextOrPrevElement( -1/*Direction*/ );
}
void SearchNewElement( int IndexTypeSelected, char * StringEntered )
{
	int TypeToSearch = ELE_FREE;
	SearchDatas.ElementOrSearchType = TypeToSearch;
	if ( IndexTypeSelected>=0 && IndexTypeSelected<NBR_SEARCH_TYPES )
	{
		TypeToSearch = SearchTypes[ IndexTypeSelected ];
		printf("New search: SearchTypeIndex=%d, Var/NbrBlock=%s\n",TypeToSearch, StringEntered);

		if ( StringEntered[ 0 ]!='\0' )
		{
			if ( TypeToSearch==SEARCH_BLOCKS )
			{
				SearchDatas.VarNum = atoi( StringEntered );
				SearchDatas.ElementOrSearchType = TypeToSearch;
			}
			else
			{
				// as it is not a box (number), it is a variable (to parse) !
				if ( TextParserForAVar( StringEntered, &SearchDatas.VarType, &SearchDatas.VarNum, NULL, FALSE/*PartialNames*/))
				{
					SearchDatas.ElementOrSearchType = TypeToSearch;
				}
				else
				{
					SearchDatas.ElementOrSearchType = ELE_FREE;
					if (ErrorMessageVarParser)
						ShowMessageBoxError( ErrorMessageVarParser );
					else
						ShowMessageBoxError( _("Unknown variable...") );
				}
			}
		}
		if ( SearchDatas.ElementOrSearchType!=ELE_FREE )
		{
			SearchDatas.CurSearchSection = 0;
			SearchDatas.NewSearchStarting = TRUE;
			ScanSection( 1/*Direction*/ );
			if ( SearchDatas.CurSearchSection!=-1 )
				SearchAndGoToNextElement( );
		}
	}
}
