/* Classic Ladder Project */
/* Copyright (C) 2001-2015 Marc Le Douarain */
/* http://www.sourceforge.net/projects/classicladder */
/* http://sites.google.com/site/classicladder */
/* February 2001 */
/* -------------------------------------------------------------------------------------------- */
/* Load/Save Rungs, Timers, Monostables, Counters, Arithmetic expressions & Sections parameters */
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
#include <dirent.h>
#include <errno.h>
#include <stddef.h> // for replace_str()
// for mkdir( ) Linux + unistd.h for rmdir()
#if !defined(__WIN32__)
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#else
#include <dir.h>
#endif
#include "classicladder.h"
#include "global.h"
#include "calc.h"
#include "calc_sequential.h"
#include "files_sequential.h"
#include "files.h"
#include "vars_access.h"
#include "manager.h"
#ifdef COMPLETE_PLC
#include "log_events.h"
#endif
#include "files_project.h"

#ifdef debug
#define dbg_printf printf
#else
static inline int dbg_printf(char *f, ...) {return 0;}
#endif

StrDatasForBase CorresDatasForBase[3] = { {BASE_MINS , TIME_BASE_MINS , "%.1fmn" , "Mins" } ,
								   {BASE_SECS , TIME_BASE_SECS , "%.1fs" , "Secs" } ,
								   {BASE_100MS , TIME_BASE_100MS , "%.0f00ms" , "100msecs" } };
char * TimersModesStrings[ NBR_TIMERSMODES ] = { "TON", "TOF", "TP" };
char * RegistersModesStrings[ NBR_REGISTERS_MODES ] = { "UNDEF", "FIFO", "LIFO" };

char TmpDirectoryRoot[ 400 ] = "";
// where little files project are stored
char TmpDirectoryProject[ 400 ] = "";


char *cl_fgets(char *s, int size, FILE *stream)
{
	char * res;
	s[0] = '\0';
	
	res = fgets( s, size, stream );
	
	// While last character in string is either CR or LF, remove.
	while (strlen(s)>=1 && ((s[strlen(s)-1]=='\r') || (s[strlen(s)-1]=='\n')))
		s[strlen(s)-1] = '\0';

	if ( strlen( S_LINE )>0 && strlen(s)>strlen( S_LINE )+strlen( E_LINE ) )
	{
		strcpy( s, s+strlen( S_LINE ) );
		s[ strlen(s)-strlen(E_LINE) ] = '\0';
	}
	return res;
}

char ConvRawLineOfElements(char * RawLine,int y,StrRung * StorageRung)
{
	char * StartOfValue;
	char * EndOfValue;
	int x = 0;

	char EndOfLine;
	char IndexedVarFound;

	StartOfValue = RawLine;
	EndOfValue = RawLine;

	do
	{
		/* Extract Element Type */
		StartOfValue = EndOfValue;
		do
		{
			EndOfValue++;
		}
		while(*EndOfValue!='-');
		*EndOfValue++ = '\0';
		StorageRung->Element[x][y].Type = atoi(StartOfValue);

		/* Extract ConnectedWithTop */
		StartOfValue = EndOfValue;
		do
		{
			EndOfValue++;
		}
		while(*EndOfValue!='-');
		*EndOfValue++ = '\0';
		StorageRung->Element[x][y].ConnectedWithTop = atoi(StartOfValue);

		IndexedVarFound = FALSE;
		/* Extract Var Type */
		StartOfValue = EndOfValue;
		do
		{
			EndOfValue++;
		}
		while(*EndOfValue!='/');
		*EndOfValue++ = '\0';
		StorageRung->Element[x][y].VarType = atoi(StartOfValue);

		/* Extract Var Offset in the type table */
		StartOfValue = EndOfValue;
		do
		{
			EndOfValue++;
		}
		while( (*EndOfValue!=',') && (*EndOfValue!='[') && (*EndOfValue!=10) && (*EndOfValue!='\0') );
		EndOfLine = TRUE;
		if (*EndOfValue==',')
			EndOfLine = FALSE;
		if ( *EndOfValue=='[' )
			IndexedVarFound = TRUE;
		*EndOfValue++ = '\0';
		StorageRung->Element[x][y].VarNum = atoi(StartOfValue);
		if ( IndexedVarFound )
		{
			/* Extract Indexed Var Type */
			StartOfValue = EndOfValue;
			do
			{
				EndOfValue++;
			}
			while(*EndOfValue!='/');
			*EndOfValue++ = '\0';
			StorageRung->Element[x][y].IndexedVarType = atoi(StartOfValue);

			/* Extract Indexed Var Offset in the type table */
			StartOfValue = EndOfValue;
			do
			{
				EndOfValue++;
			}
			while( (*EndOfValue!=',') && (*EndOfValue!=10) && (*EndOfValue!='\0') );
			EndOfLine = TRUE;
			if (*EndOfValue==',')
				EndOfLine = FALSE;
			*EndOfValue++ = '\0';
			StorageRung->Element[x][y].IndexedVarNum = atoi(StartOfValue);
		}

		/* Next Element */
		x++;

	}
	while(!EndOfLine);
	//v0.9, to load old rungs with less columns...
	//last column of old file correspond to coils that must be moved to new last column!
	if ( x<RUNG_WIDTH )
	{
		if ( StorageRung->Element[x-1][y].Type!=ELE_FREE && StorageRung->Element[x-1][y].Type!=ELE_OUTPUT_OPERATE )
		{
			int ScanX;
			CopyElementToElement( &StorageRung->Element[x-1][y], &StorageRung->Element[RUNG_WIDTH-1][y] );
			// add horizontal cnx for the blank columns added...
			for( ScanX=x-1; ScanX<RUNG_WIDTH-1; ScanX++ )
			{
				StorageRung->Element[ScanX][y].Type = ELE_CONNECTION;
				StorageRung->Element[ScanX][y].ConnectedWithTop = 0;
				StorageRung->Element[ScanX][y].VarType = 0;
				StorageRung->Element[ScanX][y].VarNum = 0;
				StorageRung->Element[ScanX][y].IndexedVarType = -1; /* undefined */
				StorageRung->Element[ScanX][y].IndexedVarNum = 0;
			}
		}
		if ( StorageRung->Element[x-1][y].Type==ELE_OUTPUT_OPERATE )
		{
			int ScanX;
			for( ScanX=RUNG_WIDTH-1; ScanX>=x-1-2; ScanX-- )
			{
				if ( ScanX>=RUNG_WIDTH-1-2 )
				{
					CopyElementToElement( &StorageRung->Element[ScanX-(RUNG_WIDTH-x)][y], &StorageRung->Element[ScanX][y] );
				}
				else
				{
					StorageRung->Element[ScanX][y].Type = ELE_CONNECTION;
					StorageRung->Element[ScanX][y].ConnectedWithTop = 0;
					StorageRung->Element[ScanX][y].VarType = 0;
					StorageRung->Element[ScanX][y].VarNum = 0;
					StorageRung->Element[ScanX][y].IndexedVarType = -1;
					StorageRung->Element[ScanX][y].IndexedVarNum = 0;
				}
			}
		}
	}
	return (x);
}

/*void RemoveEndLine( char * line )
{
	if (strlen( line )>0 && line[ strlen(line)-1 ]=='\n')
		line[ strlen(line)-1 ]='\0';
	if (strlen( line )>0 && line[ strlen(line)-1 ]=='\r')
		line[ strlen(line)-1 ]='\0';
	if (strlen( line )>0 && line[ strlen(line)-1 ]=='\r')
		line[ strlen(line)-1 ]='\0';
}*/

char LoadRung(char * FileName,StrRung * BufRung)
{
	FILE * File;
	char Okay = FALSE;
	char Line[300];
	char * LineOk;
	int y = 0;
//printf("Trying to load rung file:%s\n",FileName);
	File = fopen(FileName,"rt");
	if (File)
	{
		do
		{
			LineOk = cl_fgets(Line,300,File);
			if (LineOk)
			{
				char * pParameter;
//printf("Line rung file loaded: %s\n",Line);
				switch(Line[0])
				{
					case ';':
						break;
					case '#':
						if(strncmp(&Line[1],"VER=",4)==0)
						{
							if (atoi(&Line[5])>3)
							{
								printf("Rung version not supported...\n");
								LineOk = FALSE;
							}
						}
						pParameter = "LABEL=";
						if(strncmp(&Line[1],pParameter,strlen(pParameter))==0)
						{
							strcpy(BufRung->Label,&Line[1+strlen(pParameter)]);
//WIN32PORT
//							RemoveEndLine( BufRung->Label );
						}
						pParameter = "COMMENT=";
						if(strncmp(&Line[1],pParameter,strlen(pParameter))==0)
						{
							strcpy(BufRung->Comment,&Line[1+strlen(pParameter)]);
//WIN32PORT
//							RemoveEndLine( BufRung->Comment );
						}
						pParameter = "COMMENT_LONG=";
						if(strncmp(&Line[1],pParameter,strlen(pParameter))==0)
						{
							strcpy(BufRung->Comment,&Line[1+strlen(pParameter)]);
//WIN32PORT
//							RemoveEndLine( BufRung->Comment );
						}
						pParameter = "PREVRUNG=";
						if(strncmp(&Line[1],pParameter,strlen(pParameter))==0)
							BufRung->PrevRung = atoi( &Line[1+strlen(pParameter)] );
						pParameter = "NEXTRUNG=";
						if(strncmp(&Line[1],pParameter,strlen(pParameter))==0)
							BufRung->NextRung = atoi( &Line[1+strlen(pParameter)] );
						break;
					default:
					if ( y<RUNG_HEIGHT )
						ConvRawLineOfElements(Line,y,BufRung);
					y++;
				}
			}
		}
		while(LineOk);
		fclose(File);
		UpdateNbrLinesUsedInRung( BufRung );
		Okay = TRUE;
	}
	return (Okay);
}
void UpdateNbrLinesUsedInRung( StrRung * BufRung )
{
	int x,y;
	// update info nbr lines really used in the rung
	for (y=0;y<RUNG_HEIGHT;y++)
	{
		for(x=0;x<RUNG_WIDTH;x++)
		{
			if ( BufRung->Element[x][y].Type!=ELE_FREE )
			{
				BufRung->NbrLinesUsed = y+1;
			}
		}
	}
}
char SaveRung(char * FileName,StrRung * BufRung)
{
	FILE * File;
	char Okay = FALSE;
	int x,y;
	File = fopen(FileName,"wt");
	if (File)
	{
//		fprintf(File,"; Rung :\n");
//		fprintf(File,"; all the blocks with the following format :\n");
//		fprintf(File,"; type (see classicladder.h) - ConnectedWithTop - VarType (see classicladder.h) / VarOffset\n");
		fprintf(File,S_LINE "#VER=3.0" E_LINE "\n");
		fprintf(File,S_LINE "#LABEL=%s" E_LINE "\n",BufRung->Label);
		if ( strlen( BufRung->Comment )>LGT_COMMENT_OLD_LITTLE-1 )
			fprintf(File,S_LINE "#COMMENT_LONG=%s" E_LINE "\n",BufRung->Comment);
		else
			fprintf(File,S_LINE "#COMMENT=%s" E_LINE "\n",BufRung->Comment);
		fprintf(File,S_LINE "#PREVRUNG=%d" E_LINE "\n",BufRung->PrevRung);
		fprintf(File,S_LINE "#NEXTRUNG=%d" E_LINE "\n",BufRung->NextRung);
		//TODO: use to optimise to save only lines where there is some elements defined...
		fprintf(File,S_LINE "#NBRLINES=%d" E_LINE "\n",RUNG_HEIGHT);
		for (y=0;y<RUNG_HEIGHT;y++)
		{
			fprintf(File, "%s", S_LINE );
			for(x=0;x<RUNG_WIDTH;x++)
			{
				fprintf(File,"%d-%d-%d/%d",BufRung->Element[x][y].Type, BufRung->Element[x][y].ConnectedWithTop ,
									BufRung->Element[x][y].VarType , BufRung->Element[x][y].VarNum);
				if ( BufRung->Element[x][y].IndexedVarType!=-1 )
					fprintf( File, "[%d/%d]", BufRung->Element[x][y].IndexedVarType, BufRung->Element[x][y].IndexedVarNum );
				if (x<RUNG_WIDTH-1)
					fprintf(File," , ");
			}
			fprintf(File,E_LINE "\n");
		}
		fclose(File);
		Okay = TRUE;
	}
	return (Okay);
}

void LoadAllRungs_V1(char * BaseName,StrRung * Rungs,int * TheFirst,int * TheLast,int * TheCurrent)
{
	int NumRung;
	StrRung * PrevRung = NULL;
	int PrevNumRung = 0;
	*TheFirst = -1;
	*TheCurrent = -1;
	for(NumRung=0; NumRung<NBR_RUNGS; NumRung++)
	{
		char RungFile[400];
		sprintf(RungFile,"%s%d.csv",BaseName,NumRung);
		dbg_printf("Loading file : %s",RungFile);
		if (LoadRung(RungFile,Rungs))
		{
			if (*TheFirst==-1)
			{
				*TheFirst = NumRung;
				*TheCurrent = NumRung;
			}
			if (PrevRung)
			{
				PrevRung->NextRung = NumRung;
			}
			Rungs->Used = TRUE;
			Rungs->PrevRung = PrevNumRung;
			*TheLast = NumRung;
			PrevNumRung = NumRung;
			PrevRung = Rungs;
			dbg_printf(" - ok.\n");
		}
		else
			dbg_printf(" - not found.\n");
		//DumpRung(Rungs);
		Rungs++;
	}
	/* no rungs loaded ? */
	/* we must keep at least one empty ! */
	if (*TheFirst<0)
	{
		*TheFirst = 0;
		*TheCurrent = 0;
	}
}
void LoadAllRungs(char * BaseName,StrRung * Rungs)
{
	int NumRung;
	for(NumRung=0; NumRung<NBR_RUNGS; NumRung++)
	{
		char RungFile[400];
		sprintf(RungFile,"%s%d.csv",BaseName,NumRung);
		dbg_printf("Loading file : %s",RungFile);
		if (LoadRung(RungFile,Rungs))
		{
			Rungs->Used = TRUE;
			dbg_printf(" - ok.\n");
		}
		else
			dbg_printf(" - not found.\n");
		//DumpRung(Rungs);
		Rungs++;
	}
}

void SaveAllRungs(char * BaseName)
{
	int NumRung;
	char RungFile[400];
	/* delete all before */
	for(NumRung=0;NumRung<NBR_RUNGS;NumRung++)
	{
		sprintf(RungFile,"%s%d.csv",BaseName,NumRung);
		remove(RungFile);
	}
	/* save rungs (only defined ones are saved) */
	/* Since v0.5.5, the number of the rungs do not change : */
	/* it's easier, and with the sections added it's indispensable */
	for(NumRung=0;NumRung<NBR_RUNGS;NumRung++)
	{
		if ( RungArray[ NumRung ].Used )
		{
			sprintf(RungFile,"%s%d.csv",BaseName,NumRung);
			dbg_printf("Saving file : %s",RungFile);
			if (SaveRung(RungFile,&RungArray[NumRung]))
			{
				dbg_printf(" - ok.\n");
				sprintf(RungFile,"rung_%d.csv",NumRung);
				AddFileToFilesProjectList( RungFile );
			}
			else
			{
				dbg_printf(" - failed.\n");
			}
		}
	}
}

void DumpRung(StrRung * TheRung)
{
	int x,y;
	printf("Used=%d\n",TheRung->Used);
	for (y=0;y<RUNG_HEIGHT;y++)
	{
		for(x=0;x<RUNG_WIDTH;x++)
		{
			printf("%d:%d:%d=%d , ",TheRung->Element[x][y].Type,TheRung->Element[x][y].ConnectedWithTop,TheRung->Element[x][y].VarNum,TheRung->Element[x][y].DynamicOutput);
		}
		printf("\n");
	}
}

char * ConvRawLineOfNumbers(char * RawLine,char NbrParams,int * ValuesFnd, int DefaultValueToComplete)
{
	char * StartOfValue;
	char * EndOfValue = RawLine; //aded init in v0.9.101
	char Num = 0;
	
	// added in v0.9.9, if empty line for user edit...
	if ( *RawLine!='\0' )
	{
		char EndOfLine;
		
		StartOfValue = RawLine;
		EndOfValue = RawLine;
		EndOfLine = FALSE;
		
		do
		{
			/* Extract Value */
			StartOfValue = EndOfValue;
			do
			{
				EndOfValue++;
			}
			while( (*EndOfValue!=',') && (*EndOfValue!=10) && (*EndOfValue!='\0') );
	//v0.9.9 more sure as now used for user edit...		if (*EndOfValue==10 || *EndOfValue=='\0')
			if (*EndOfValue!=',')
				EndOfLine = TRUE;
			*EndOfValue++ = '\0';
			*ValuesFnd++ = atoi(StartOfValue);
			Num++;
			StartOfValue = EndOfValue;
		}
		while( (!EndOfLine) && (Num<NbrParams) );
	}
	//v0.9.2, added by Heli
	while (Num<NbrParams){*ValuesFnd++=DefaultValueToComplete/*0*/; Num++;}  
	return EndOfValue;
}

int ConvBaseInMilliSecsToId(int NbrMilliSecs)
{
	switch(NbrMilliSecs)
	{
		case TIME_BASE_MINS:
		return BASE_MINS;
		case TIME_BASE_SECS:
		return BASE_SECS;
		case TIME_BASE_100MS:
		return BASE_100MS;
		default:
		printf("!!!Error in ConvBaseInMilliSecsToInt()\n");
		return BASE_SECS;
	}
}

/* Fields (strings, ints or chars) are separated by ',' */
/* When using ParamsStringsFnd[ ], (int)ParamsIntFnd[ ] corresponds to size max of the string... */ 
char * ConvRawLineOfStringsOrNumbers(char * RawLine,int NbrParams,char ** ParamsStringsFnd,int ** ParamsIntFnd,char ** ParamsCharFnd)
{
	char * StartOfValue;
	char * EndOfValue;
	int Num = 0;
	
	char EndOfLine;

	StartOfValue = RawLine;
	EndOfValue = RawLine;
	EndOfLine = FALSE;

//printf("%s(): Line=%s; PtrStr=%x; PtrInt=%x; PtrCh=%x\n", __FUNCTION__, RawLine, ParamsStringsFnd, ParamsIntFnd, ParamsCharFnd );  
	do
	{
		char IsAnIntForStrLgth = FALSE;
		
		/* Extract Value */
		StartOfValue = EndOfValue;
//april2014, added if null string...
		if ( *StartOfValue!=',' )
		{
			do
			{
				EndOfValue++;
			}
			while( (*EndOfValue!=',') && (*EndOfValue!=10) && (*EndOfValue!='\0') );
			if (*EndOfValue==10 || *EndOfValue=='\0')
				EndOfLine = TRUE;
		}
		*EndOfValue++ = '\0';
//printf("%s(): NumParam=%d Value=%s EndOfLine=%d\n", __FUNCTION__, Num, StartOfValue,EndOfLine );  
		if ( ParamsStringsFnd!=NULL )
		{
			if ( ParamsStringsFnd[Num]!=NULL )
			{
//printf("%s(): ForString\n", __FUNCTION__ );  
				if ( ParamsIntFnd[ Num ]!=NULL )
				{
					IsAnIntForStrLgth = TRUE;
					if ( strlen( StartOfValue )<(int)ParamsIntFnd[ Num ] )
						strcpy( ParamsStringsFnd[Num], StartOfValue );
				}
			}
		}
		if ( ParamsIntFnd!=NULL )
		{
//printf("%s(): ForInt\n", __FUNCTION__ );  
			if ( ParamsIntFnd[ Num ]!=NULL && !IsAnIntForStrLgth )
				*ParamsIntFnd[ Num ] = atoi( StartOfValue );
		}
		if ( ParamsCharFnd!=NULL )
		{
//printf("%s(): ForChar\n", __FUNCTION__ );  
			if ( ParamsCharFnd[ Num ]!=NULL )
				*ParamsCharFnd[ Num ] = (char)atoi( StartOfValue );
		}
		Num++;
//inutile...		StartOfValue = EndOfValue;
	}
	while( (!EndOfLine) && (Num<NbrParams) );
	return EndOfValue;
}

#ifdef OLD_TIMERS_MONOS_SUPPORT
char LoadTimersParams(char * FileName)
{
	FILE * File;
	char Okay = FALSE;
	char Line[300];
	char * LineOk;
	int Params[3];
	int OldFileScanTimer = 0;
	File = fopen(FileName,"rt");
	if (File)
	{
		do
		{
			LineOk = cl_fgets(Line,300,File);
			if (LineOk)
			{
				if (Line[0]!=';' && Line[0]!='#')
				{
					StrTimer * pTimer;
					//new format with timer number (instead of saving all ones like before...) ?
					if ( Line[0]=='T' )
					{
						ConvRawLineOfNumbers(Line+1,3,Params,0/*DefaultValueToComplete*/);
						pTimer = &TimerArray[ Params[0] ];
					}
					else
					{
						ConvRawLineOfNumbers(Line,2,&Params[1],0/*DefaultValueToComplete*/);
						pTimer = &TimerArray[ OldFileScanTimer ];
						OldFileScanTimer++;
					}
					switch(Params[1])
					{
						case BASE_MINS:
						case BASE_SECS:
						case BASE_100MS:
							pTimer->Base = CorresDatasForBase[Params[1]].ValueInMS;
							pTimer->Preset = Params[2] * pTimer->Base;
							strcpy(pTimer->DisplayFormat,CorresDatasForBase[Params[1]].DisplayFormat);
							break;
						default:
							pTimer->Base = 1;
							pTimer->Preset = 10;
							strcpy(pTimer->DisplayFormat,"%f?");
							printf("!!! Error loading parameter base in %s\n",FileName);
							break;
					}
dbg_printf("Timer => Base = %d , Preset = %d\n",pTimer->Base,pTimer->Preset);
				}
			}
		}
		while(LineOk);
		fclose(File);
		Okay = TRUE;
	}
	return (Okay);
}

char SaveTimersParams(char * FileName)
{
	FILE * File;
	char Okay = FALSE;
	int NumTimer;
	File = fopen(FileName,"wt");
	if (File)
	{
		fprintf(File,S_LINE "#VER=2.0" E_LINE "\n");
		for( NumTimer=0; NumTimer<NBR_TIMERS; NumTimer++ )
		{
			StrTimer * pTimer = &TimerArray[ NumTimer ];
			if ( pTimer->Preset>0 )
			{
				fprintf(File,S_LINE "T%d,%d,%d" E_LINE  "\n",NumTimer,ConvBaseInMilliSecsToId(pTimer->Base),pTimer->Preset/pTimer->Base);
printf("SaveTimer%d => Base = %d , Preset = %d\n",NumTimer,pTimer->Base,pTimer->Preset);
			}
		}
		fclose(File);
		Okay = TRUE;
	}
printf( " - result=%d\n", Okay );
	return (Okay);
}

char LoadMonostablesParams(char * FileName)
{
	FILE * File;
	char Okay = FALSE;
	char Line[300];
	char * LineOk;
	int Params[3];
	int ScanMonostable = 0;
	File = fopen(FileName,"rt");
	if (File)
	{
		do
		{
			LineOk = cl_fgets(Line,300,File);
			if (LineOk)
			{
				if (Line[0]!=';' && Line[0]!='#')
				{
					StrMonostable * pMonostable;
					if ( Line[0]=='M' )
					{
						ConvRawLineOfNumbers(Line+1,3,Params,0/*DefaultValueToComplete*/);
						ScanMonostable = Params[ 0 ];
					}
					else
					{
						ConvRawLineOfNumbers(Line,2,&Params[1],0/*DefaultValueToComplete*/);
					}
					pMonostable = &MonostableArray[ ScanMonostable ];
					switch(Params[1])
					{
						case BASE_MINS:
						case BASE_SECS:
						case BASE_100MS:
							pMonostable->Base = CorresDatasForBase[Params[1]].ValueInMS;
							pMonostable->Preset = Params[2] * pMonostable->Base;
							strcpy(pMonostable->DisplayFormat,CorresDatasForBase[Params[1]].DisplayFormat);
							break;
						default:
							pMonostable->Base = 1;
							pMonostable->Preset = 10;
							strcpy(pMonostable->DisplayFormat,"%f?");
							printf("!!! Error loading parameter base in %s\n",FileName);
							break;
					}
dbg_printf("Monostable => Base = %d , Preset = %d\n",pMonostable->Base,pMonostable->Preset);
					if ( Line[0]!='C' )
						ScanMonostable++;
				}
			}
		}
		while(LineOk);
		fclose(File);
		Okay = TRUE;
	}
	return (Okay);
}

char SaveMonostablesParams(char * FileName)
{
	FILE * File;
	char Okay = FALSE;
	int NumMonostable;
	File = fopen(FileName,"wt");
	if (File)
	{
		fprintf(File,S_LINE "#VER=2.0" E_LINE "\n");
		for( NumMonostable=0; NumMonostable<NBR_MONOSTABLES; NumMonostable++ )
		{
			StrMonostable *pMonostable = &MonostableArray[ NumMonostable ];
			if ( pMonostable->Preset>0 )
			{
				fprintf(File,S_LINE "M%d,%d,%d" E_LINE  "\n",NumMonostable,ConvBaseInMilliSecsToId(pMonostable->Base),pMonostable->Preset/pMonostable->Base);
printf("SaveMonostable%d => Base = %d , Preset = %d\n",NumMonostable,pMonostable->Base,pMonostable->Preset);
			}
		}
		fclose(File);
		Okay = TRUE;
	}
	return (Okay);
}
#endif

char LoadCountersParams(char * FileName)
{
	FILE * File;
	char Okay = FALSE;
	char Line[300];
	char * LineOk;
	int Params[2];
	int ScanCounter = 0;
	File = fopen(FileName,"rt");
	if (File)
	{
		do
		{
			LineOk = cl_fgets(Line,300,File);
			if (LineOk)
			{
				if (Line[0]!=';' && Line[0]!='#')
				{
					if ( Line[0]=='C' )
					{
						ConvRawLineOfNumbers(Line+1,2,Params,0/*DefaultValueToComplete*/);
						ScanCounter = Params[ 0 ];
					}
					else
					{
						ConvRawLineOfNumbers(Line,1,&Params[1],0/*DefaultValueToComplete*/);
					}
					WriteVar( VAR_COUNTER_PRESET, ScanCounter, Params[ 1 ] );
					if ( Line[0]!='C' )
						ScanCounter++;

				}
			}
		}
		while(LineOk);
		fclose(File);
		Okay = TRUE;
	}
	return (Okay);
}

char SaveCountersParams(char * FileName)
{
	FILE * File;
	char Okay = FALSE;
	int NumCounter;
	File = fopen(FileName,"wt");
	if (File)
	{
		fprintf(File,S_LINE "#VER=2.0" E_LINE "\n");
		for( NumCounter=0; NumCounter<NBR_COUNTERS; NumCounter++ )
		{
			StrCounter *pCounter = &CounterArray[ NumCounter ];
			if ( pCounter->Preset>0 )
				fprintf(File,S_LINE "C%d,%d" E_LINE "\n",NumCounter,pCounter->Preset);
		}
		fclose(File);
		Okay = TRUE;
	}
	return (Okay);
}

char LoadNewTimersParams(char * FileName)
{
	FILE * File;
	char Okay = FALSE;
	char Line[300];
	char * LineOk;
	int Params[4];
	int ScanTimerIEC = 0;
	File = fopen(FileName,"rt");
	if (File)
	{
		do
		{
			LineOk = cl_fgets(Line,300,File);
			if (LineOk)
			{
				if (Line[0]!=';' && Line[0]!='#')
				{
					StrTimerIEC * pTimerIEC;
					if ( Line[0]=='T' )
					{
						ConvRawLineOfNumbers(Line+2,4,Params,0/*DefaultValueToComplete*/);
						ScanTimerIEC = Params[ 0 ];
					}
					else
					{
						ConvRawLineOfNumbers(Line,3,&Params[1],0/*DefaultValueToComplete*/);
					}
//printf("Loaded TimerIEC%d, %d, %d, %d\n", ScanTimerIEC, Params[1], Params[2], Params[3] );
					pTimerIEC = &NewTimerArray[ ScanTimerIEC ];
					switch(Params[1])
					{
						case BASE_MINS:
						case BASE_SECS:
						case BASE_100MS:
							pTimerIEC->Base = CorresDatasForBase[Params[1]].ValueInMS;
							WriteVar( VAR_TIMER_IEC_PRESET, ScanTimerIEC, Params[2] );
							strcpy(pTimerIEC->DisplayFormat,CorresDatasForBase[Params[1]].DisplayFormat);
							break;
						default:
							pTimerIEC->Base = 1;
							WriteVar( VAR_TIMER_IEC_PRESET, ScanTimerIEC, 10 );
							strcpy(pTimerIEC->DisplayFormat,"%f?");
							printf("!!! Error loading parameter base in %s\n",FileName);
							break;
					}
					pTimerIEC->TimerMode = (char)Params[3];
					if ( Line[0]!='T' )
						ScanTimerIEC++;
				}
			}
		}
		while(LineOk);
		fclose(File);
		Okay = TRUE;
	}
	return (Okay);
}

char SaveNewTimersParams(char * FileName)
{
	FILE * File;
	char Okay = FALSE;
	int NumTimerIEC;
	File = fopen(FileName,"wt");
	if (File)
	{
		fprintf(File,S_LINE "#VER=2.0" E_LINE "\n");
		for( NumTimerIEC=0; NumTimerIEC<NBR_TIMERS_IEC; NumTimerIEC++ )
		{
			StrTimerIEC * pTimerIEC = &NewTimerArray[ NumTimerIEC ];
			if ( pTimerIEC->Preset>0 )
				fprintf(File,S_LINE "TM%d,%d,%d,%d" E_LINE "\n",NumTimerIEC, ConvBaseInMilliSecsToId(pTimerIEC->Base),pTimerIEC->Preset,pTimerIEC->TimerMode);
		}
		fclose(File);
		Okay = TRUE;
	}
	return (Okay);
}

char LoadRegistersParams(char * FileName)
{
	FILE * File;
	char Okay = FALSE;
	char Line[300];
	char * LineOk;
	int Params[2];
	File = fopen(FileName,"rt");
	if (File)
	{
		do
		{
			LineOk = cl_fgets(Line,300,File);
			if (LineOk)
			{
				if (Line[0]!=';' && Line[0]!='#')
				{
					StrRegister *pRegister;
					ConvRawLineOfNumbers(Line+1,2,Params,0/*DefaultValueToComplete*/);
					pRegister = &RegisterArray[ Params[0] ];
					pRegister->RegisterMode = (char)Params[1];
				}
			}
		}
		while(LineOk);
		fclose(File);
		Okay = TRUE;
	}
	return (Okay);
}

char SaveRegistersParams(char * FileName)
{
	FILE * File;
	char Okay = FALSE;
	int NumRegister;
	File = fopen(FileName,"wt");
	if (File)
	{
		fprintf(File,S_LINE "#VER=1.0" E_LINE "\n");
		for( NumRegister=0; NumRegister<NBR_REGISTERS; NumRegister++ )
		{
			StrRegister *pRegister = &RegisterArray[ NumRegister ];
			if ( pRegister->RegisterMode!=REGISTER_MODE_UNDEF )
				fprintf(File,S_LINE "R%d,%d" E_LINE "\n",NumRegister,pRegister->RegisterMode);
		}
		fclose(File);
		Okay = TRUE;
	}
	return (Okay);
}

char LoadArithmeticExpr(char * FileName)
{
	FILE * File;
	char Okay = FALSE;
	char Line[300];
	char * LineOk;
	int NumExpr = 0;
	File = fopen(FileName,"rt");
	if (File)
	{
		do
		{
			LineOk = cl_fgets(Line,300,File);
			if (LineOk)
			{
				if (Line[0]!=';' && Line[0]!='#')
				{
//WIN32PORT
//					RemoveEndLine( Line );
					// new format with number at the start ?
					// (only 'not blank' lines saved!)
					if ( Line[0]>='0' && Line[0]<='9' )
					{
						NumExpr = atoi(Line);
						strcpy(ArithmExpr[NumExpr].Expr,Line+strlen("xxxx,"));
					}
					else
					{
						strcpy(ArithmExpr[NumExpr].Expr,Line);
						NumExpr++;
					}
				}
			}
		}
		while(LineOk);
		fclose(File);
		Okay = TRUE;
	}
	return (Okay);
}

char SaveArithmeticExpr(char * FileName)
{
	FILE * File;
	char Okay = FALSE;
	int NumExpr;
	File = fopen(FileName,"wt");
	if (File)
	{
//		fprintf(File,"; Arithmetic expressions :\n");
//		fprintf(File,"; Compare or Operate ones\n");
		fprintf(File,S_LINE "#VER=2.0" E_LINE "\n");
		for(NumExpr=0; NumExpr<NBR_ARITHM_EXPR; NumExpr++)
		{
			if ( ArithmExpr[NumExpr].Expr[0]!='\0')
				fprintf(File,S_LINE "%04d,%s" E_LINE "\n",NumExpr,ArithmExpr[NumExpr].Expr);
		}
		fclose(File);
		Okay = TRUE;
	}
	return (Okay);
}

char LoadSectionsParams(char * FileName)
{
	FILE * File;
	char Okay = FALSE;
	char Line[300];
	char * LineOk;
	int NumSection;
	StrSection * pSection;
	int Params[10];
//printf("Trying to load sections datas from %s...\n",FileName);
	File = fopen(FileName,"rt");
	if (File)
	{
		printf("Loading sections datas from %s...\n",FileName);
		do
		{
			LineOk = cl_fgets(Line,300,File);
			if (LineOk)
			{
//printf("Line section file loaded: %s\n",Line);
				switch(Line[0])
				{
					case ';':
						break;
					case '#':
						if(strncmp(&Line[1],"VER=",4)==0)
						{
							if (atoi(&Line[5])>1)
							{
								printf("Sections file version not supported...!\n");
								LineOk = FALSE;
							}
						}
						// #NAMExxx=....
						if(strncmp(&Line[1],"NAME",4)==0)
						{
							Line[ 8 ] = '\0';
							NumSection = atoi( &Line[5] );
							strcpy(SectionArray[ NumSection ].Name, &Line[9]);
//WIN32PORT
//							RemoveEndLine( SectionArray[ NumSection ].Name );
						}
						break;
					default:
						ConvRawLineOfNumbers(Line,6,Params,0/*DefaultValueToComplete*/);
						NumSection = Params[ 0 ];
						pSection = &SectionArray[ NumSection ];
						pSection->Used = TRUE;
						pSection->Language = Params[ 1 ];
						pSection->SubRoutineNumber = Params[ 2 ];
						pSection->FirstRung = Params[ 3 ];
						pSection->LastRung = Params[ 4 ];
						pSection->SequentialPage = Params[ 5 ];
dbg_printf("Section %d => Name=%s, Language=%d, SRnbr=%d, FirstRung=%d, LastRung=%d, SequentialPage=%d\n", NumSection,
pSection->Name, pSection->Language, pSection->SubRoutineNumber, pSection->FirstRung, pSection->LastRung, pSection->SequentialPage);
						break;
				}
			}
		}
		while(LineOk);
		fclose(File);
		Okay = TRUE;
	}
printf("Sections loading result=%d\n",Okay);
	return (Okay);
}

char SaveSectionsParams(char * FileName)
{
	FILE * File;
	char Okay = FALSE;
	int NumSection;
	StrSection * pSection;
	File = fopen(FileName,"wt");
	if (File)
	{
//		fprintf(File,"; Sections\n");
		fprintf(File,S_LINE "#VER=1.0" E_LINE "\n");
		NumSection = 0;
		do
		{
			pSection = &SectionArray[ NumSection ];
			if ( pSection->Used )
				fprintf(File,S_LINE "#NAME%03d=%s" E_LINE "\n", NumSection, pSection->Name);
			NumSection++;
		}
		while(NumSection<NBR_SECTIONS);
		NumSection = 0;
		do
		{
			pSection = &SectionArray[ NumSection ];
			if ( pSection->Used )
				fprintf(File,S_LINE "%03d,%d,%d,%d,%d,%d" E_LINE "\n", NumSection, pSection->Language, pSection->SubRoutineNumber, pSection->FirstRung, pSection->LastRung, pSection->SequentialPage );
			NumSection++;
		}
		while(NumSection<NBR_SECTIONS);
		fclose(File);
		Okay = TRUE;
	}
	return (Okay);
}



char LoadIOConfParams(char * FileName)
{
	FILE * File;
	char Okay = FALSE;
	char Line[300];
	char * LineOk;
	int ScanLineInput = 0;
	int ScanLineOutput = 0;
	File = fopen(FileName,"rt");
	if (File)
	{
		do
		{
			LineOk = cl_fgets(Line,300,File);
			if (LineOk)
			{
				if (Line[0]!=';' && Line[0]!='#')
				{
					/* input/output depending of the first caracter */
					if ( Line[0]=='0' && ScanLineInput<NBR_INPUTS_CONF )
					{
						StrIOConf * pConfInput = &InfosGene->InputsConf[ ScanLineInput++ ];
						ConvRawLineOfNumbers(Line+2,7,(int*)pConfInput++,0/*DefaultValueToComplete*/);
					}
					if ( Line[0]=='1' && ScanLineOutput<NBR_OUTPUTS_CONF )
					{
						StrIOConf * pConfOutput = &InfosGene->OutputsConf[ ScanLineOutput++ ];
						ConvRawLineOfNumbers(Line+2,7,(int*)pConfOutput++,0/*DefaultValueToComplete*/);
					}
				}
			}
		}
		while(LineOk);
		fclose(File);
		Okay = TRUE;
	}
	return (Okay);
}

char SaveIOConfParams(char * FileName)
{
	FILE * File;
	char Okay = FALSE;
	int NumConf;
	int NbrConf;
	int Pass;
	StrIOConf * pConf;
	File = fopen(FileName,"wt");
	if (File)
	{
//		fprintf(File,"; I/O Configuration\n");
		fprintf(File,S_LINE "#VER=1.0" E_LINE "\n");
		for( Pass=0; Pass<2; Pass++)
		{
			NbrConf = (Pass==0)?NBR_INPUTS_CONF:NBR_OUTPUTS_CONF;
			for( NumConf=0; NumConf<NbrConf; NumConf++ )
			{
				pConf = (Pass==0)?&InfosGene->InputsConf[ NumConf ]:&InfosGene->OutputsConf[ NumConf ];
				/* valid mapping ? */
//				if ( pConf->FirstClassicLadderIO!=-1 )
				if ( pConf->DeviceType!=DEVICE_TYPE_NONE )
				{
					fprintf(File,S_LINE "%d,%d,%d,%d,%d,%d,%d,%d" E_LINE "\n", Pass, pConf->FirstClassicLadderIO,
						pConf->DeviceType, pConf->SubDevOrAdr, pConf->FirstChannel,
						pConf->NbrConsecutivesChannels, pConf->FlagInverted, pConf->ConfigData );
				}
			}
		}
		fclose(File);
		Okay = TRUE;
	}
	return (Okay);
}


#ifdef MODBUS_IO_MASTER
char LoadModbusIOConfParams(char * FileName)
{
	FILE * File;
	char Okay = FALSE;
	char Line[300];
	char * LineOk;
	char *PtrStrings[ 4 ];
	int * PtrInts[ 6 ];
	char * PtrChars[ 6 ];
	int FileVersion = 0;
	StrModbusMasterReq * pReq = &ModbusMasterReq[ 0 ];
	File = fopen(FileName,"rt");
	if (File)
	{
		do
		{
			LineOk = cl_fgets(Line,300,File);
			if (LineOk)
			{
				switch(Line[0])
				{
					case ';':
						break;
					case '#':
						if(strncmp(&Line[1],"VER=",4)==0)
						{
							FileVersion = atoi(&Line[5]);
							if (FileVersion>2)
							{
								printf("Modbus I/O config version not supported...\n");
								LineOk = FALSE;
							}
						}
						break;
					case 'S':
						{
							char NumSlave = (char)atoi( &Line[1] );
							if ( NumSlave<NBR_MODBUS_SLAVES )
							{
								StrModbusSlave * pSlave = &ModbusSlaveList[ (int)NumSlave ];
								PtrStrings[ 0 ] = NULL; PtrInts[ 0 ] = NULL; PtrChars[ 0 ] = &NumSlave; //first field already read in previous atoi()
								PtrStrings[ 1 ] = pSlave->SlaveAdr; PtrInts[ 1 ] = (int *)LGT_SLAVE_ADR; PtrChars[ 1 ] = NULL;
								PtrStrings[ 2 ] = NULL; PtrInts[ 2 ] = NULL; PtrChars[ 2 ] = &pSlave->UseUdpInsteadOfTcp;
								PtrStrings[ 3 ] = pSlave->NameInfo; PtrInts[ 3 ] = (int *)LGT_SLAVE_NAME; PtrChars[ 3 ] = NULL;
								ConvRawLineOfStringsOrNumbers( &Line[1], 4, PtrStrings, PtrInts, PtrChars );
//printf("ModbusSlave%d: adr:%s udp:%d name:%s\n", NumSlave, pSlave->SlaveAdr, pSlave->UseUdpInsteadOfTcp, pSlave->NameInfo );
							}
						}
						break;
					case 'R':
					default:
						if ( FileVersion==1 || ( FileVersion==2 && Line[0]=='R' ) )
						{
							PtrInts[ 0 ] = &pReq->SlaveListNum; PtrChars[ 0 ] = NULL;
							PtrInts[ 1 ] = NULL; PtrChars[ 1 ] = &pReq->TypeReq;
							PtrInts[ 2 ] = &pReq->FirstModbusElement; PtrChars[ 2 ] = NULL;
							PtrInts[ 3 ] = &pReq->NbrModbusElements; PtrChars[ 3 ] = NULL;
							PtrInts[ 4 ] = NULL; PtrChars[ 4 ] = &pReq->LogicInverted;
							PtrInts[ 5 ] = &pReq->OffsetVarMapped; PtrChars[ 5 ] = NULL;
							ConvRawLineOfStringsOrNumbers( &Line[ (FileVersion==1)?0:2 ], 6, NULL, PtrInts, PtrChars );
/*printf("ModbusReq: sl:%d ty:%d first:%d nbr:%d inv:%d offset:%d\n", pReq->SlaveListNum, pReq->TypeReq,
								pReq->FirstModbusElement, pReq->NbrModbusElements, pReq->LogicInverted, pReq->OffsetVarMapped);*/
							if ( FileVersion==1 )
								pReq->SlaveListNum = -1; // people will have to correct it manually... otherwise a little more complicated!
							pReq++;
						}
						break;
				}
			}
		}
		while(LineOk);
		fclose(File);
		Okay = TRUE;
	}
	return (Okay);
}

char SaveModbusIOConfParams(char * FileName)
{
	FILE * File;
	char Okay = FALSE;
	int NumLine;
	File = fopen(FileName,"wt");
	if (File)
	{
//		fprintf(File,"; Modbus Distributed I/O Configuration\n");
		fprintf(File,S_LINE "#VER=2.0" E_LINE "\n");
		for (NumLine=0; NumLine<NBR_MODBUS_SLAVES; NumLine++ )
		{
			StrModbusSlave * pSlave = &ModbusSlaveList[ NumLine ];
			/* valid slave ? */
			if ( pSlave->SlaveAdr[0]!='\0' )
			{
				fprintf(File, S_LINE "S%d,%s,%d,%s" E_LINE "\n", NumLine, pSlave->SlaveAdr, pSlave->UseUdpInsteadOfTcp, pSlave->NameInfo);
			}
		}
		for (NumLine=0; NumLine<NBR_MODBUS_MASTER_REQ; NumLine++ )
		{
			StrModbusMasterReq * pConf = &ModbusMasterReq[ NumLine ];
			/* valid request ? */
			if ( pConf->SlaveListNum!=-1 )
			{
				fprintf(File,S_LINE "R,%d,%d,%d,%d,%d,%d" E_LINE "\n", pConf->SlaveListNum, pConf->TypeReq,
					pConf->FirstModbusElement, pConf->NbrModbusElements, pConf->LogicInverted, pConf->OffsetVarMapped );
			}
		}
		fclose(File);
		Okay = TRUE;
	}
	return (Okay);
}
#endif

char LoadSymbols(char * FileName)
{
	FILE * File;
	char Okay = FALSE;
	char Line[300];
	char * LineOk;
	int NumSymbol = 0;
	char *PtrStrings[ 3 ];
	int * PtrIntsLength[ 3 ];
	StrSymbol * pSymbol;
	File = fopen(FileName,"rt");
	if (File)
	{
		do
		{
			LineOk = cl_fgets(Line,300,File);
			if (LineOk)
			{
				switch(Line[0])
				{
					case ';':
						break;
					case '#':
						if(strncmp(&Line[1],"VER=",4)==0)
						{
							if (atoi(&Line[5])>1)
							{
								printf("Symbols file version not supported...\n");
								LineOk = FALSE;
							}
						}
						break;
					default:
						pSymbol = &SymbolArray[ NumSymbol ];
						PtrStrings[ 0 ] = pSymbol->VarName; PtrIntsLength[ 0 ] = (int *)LGT_VAR_NAME;
						PtrStrings[ 1 ] = pSymbol->Symbol; PtrIntsLength[ 1 ] = (int *)LGT_SYMBOL_STRING;
						PtrStrings[ 2 ] = pSymbol->Comment; PtrIntsLength[ 2 ] = (int *)LGT_SYMBOL_COMMENT;
						ConvRawLineOfStringsOrNumbers( Line, 3, PtrStrings, PtrIntsLength, NULL );
//						RemoveEndLine( pSymbol->Comment );
//printf("LoadedSymbol: %s - %s - %s\n", pSymbol->VarName, pSymbol->Symbol, pSymbol->Comment);
						NumSymbol++;
						break;
				}
			}
		}
		while(LineOk);
		fclose(File);
		Okay = TRUE;
	}
	return (Okay);
}

char SaveSymbols(char * FileName)
{
	FILE * File;
	char Okay = FALSE;
	int NumSymbol = 0;
	StrSymbol * pSymbol;
	File = fopen(FileName,"wt");
	if (File)
	{
//		fprintf(File,"; Symbols\n");
		fprintf(File,S_LINE "#VER=1.0" E_LINE "\n");
		do
		{
			pSymbol = &SymbolArray[ NumSymbol ];
			if ( pSymbol->VarName[0]!='\0')
				fprintf(File,S_LINE "%s,%s,%s" E_LINE "\n", pSymbol->VarName, pSymbol->Symbol, pSymbol->Comment );
			NumSymbol++;
		}
		while(NumSymbol<NBR_SYMBOLS);
		fclose(File);
		Okay = TRUE;
	}
	return (Okay);
}


char LoadGeneralParameters(char * FileName)
{
	FILE * File;
	char Okay = FALSE;
	char Line[300];
	char * LineOk;
	File = fopen(FileName,"rt");
	if (File)
	{
		do
		{
			LineOk = cl_fgets(Line,300,File);
			if (LineOk)
			{
				char * pParameter;
				pParameter = "PERIODIC_REFRESH=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					GeneralParamsMirror.PeriodMilliSecsTaskLogic = atoi( &Line[ strlen( pParameter) ] );
				pParameter = "PERIODIC_INPUTS_REFRESH=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					GeneralParamsMirror.PeriodMilliSecsTaskScanInputs = atoi( &Line[ strlen( pParameter) ] );
				pParameter = "REAL_INPUTS_OUTPUTS_ONLY_ON_TARGET=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
				{
					GeneralParamsMirror.RealInputsOutputsOnlyOnTarget = atoi( &Line[ strlen( pParameter) ] );
printf("Real I/O choice (loaded):%d\n", GeneralParamsMirror.RealInputsOutputsOnlyOnTarget);
				}
				pParameter = "AUTO_ADJUST_SUMMER_WINTER_TIME=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					GeneralParamsMirror.AutomaticallyAdjustSummerWinterTime = atoi( &Line[ strlen( pParameter) ] );
				pParameter = "SIZE_NBR_RUNGS=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					GeneralParamsMirror.SizesInfos.nbr_rungs = atoi( &Line[ strlen( pParameter) ] );
				pParameter = "SIZE_NBR_BITS=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					GeneralParamsMirror.SizesInfos.nbr_bits = atoi( &Line[ strlen( pParameter) ] );
				pParameter = "SIZE_NBR_WORDS=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					GeneralParamsMirror.SizesInfos.nbr_words = atoi( &Line[ strlen( pParameter) ] );
#ifdef OLD_TIMERS_MONOS_SUPPORT
				pParameter = "SIZE_NBR_TIMERS=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					GeneralParamsMirror.SizesInfos.nbr_timers = atoi( &Line[ strlen( pParameter) ] );
				pParameter = "SIZE_NBR_MONOSTABLES=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					GeneralParamsMirror.SizesInfos.nbr_monostables = atoi( &Line[ strlen( pParameter) ] );
#endif
				pParameter = "SIZE_NBR_COUNTERS=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					GeneralParamsMirror.SizesInfos.nbr_counters = atoi( &Line[ strlen( pParameter) ] );
				pParameter = "SIZE_NBR_TIMERS_IEC=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					GeneralParamsMirror.SizesInfos.nbr_timers_iec = atoi( &Line[ strlen( pParameter) ] );
				pParameter = "SIZE_NBR_REGISTERS=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					GeneralParamsMirror.SizesInfos.nbr_registers = atoi( &Line[ strlen( pParameter) ] );
				pParameter = "SIZE_REGISTER_LIST=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					GeneralParamsMirror.SizesInfos.register_list_size = atoi( &Line[ strlen( pParameter) ] );
				pParameter = "SIZE_NBR_PHYS_INPUTS=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					GeneralParamsMirror.SizesInfos.nbr_phys_inputs = atoi( &Line[ strlen( pParameter) ] );
				pParameter = "SIZE_NBR_PHYS_OUTPUTS=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					GeneralParamsMirror.SizesInfos.nbr_phys_outputs = atoi( &Line[ strlen( pParameter) ] );
				pParameter = "SIZE_NBR_ARITHM_EXPR=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					GeneralParamsMirror.SizesInfos.nbr_arithm_expr = atoi( &Line[ strlen( pParameter) ] );
				pParameter = "SIZE_NBR_SECTIONS=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					GeneralParamsMirror.SizesInfos.nbr_sections = atoi( &Line[ strlen( pParameter) ] );
				pParameter = "SIZE_NBR_SYMBOLS=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					GeneralParamsMirror.SizesInfos.nbr_symbols = atoi( &Line[ strlen( pParameter) ] );
				pParameter = "SIZE_NBR_PHYS_WORDS_INPUTS=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					GeneralParamsMirror.SizesInfos.nbr_phys_words_inputs = atoi( &Line[ strlen( pParameter) ] );
				pParameter = "SIZE_NBR_PHYS_WORDS_OUTPUTS=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					GeneralParamsMirror.SizesInfos.nbr_phys_words_outputs = atoi( &Line[ strlen( pParameter) ] );
				pParameter = "MODBUS_MASTER_SERIAL_PORT=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					strcpy( ModbusConfig.ModbusSerialPortNameUsed, &Line[ strlen( pParameter) ] );
				pParameter = "MODBUS_MASTER_SERIAL_SPEED=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					ModbusConfig.ModbusSerialSpeed = atoi( &Line[ strlen( pParameter) ] );
				pParameter = "MODBUS_MASTER_SERIAL_DATABITS=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					ModbusConfig.ModbusSerialDataBits = atoi( &Line[ strlen( pParameter) ] );
				pParameter = "MODBUS_MASTER_SERIAL_PARITY=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					ModbusConfig.ModbusSerialParity = atoi( &Line[ strlen( pParameter) ] );
				pParameter = "MODBUS_MASTER_SERIAL_STOPBITS=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					ModbusConfig.ModbusSerialStopBits = atoi( &Line[ strlen( pParameter) ] );
			}
		}
		while(LineOk);
		fclose(File);
		Okay = TRUE;
	}
	return (Okay);
}

char SaveGeneralParameters(char * FileName)
{
	FILE * File;
	char Okay = FALSE;
	File = fopen(FileName,"wt");
	if (File)
	{
		fprintf( File,S_LINE "PERIODIC_REFRESH=%d" E_LINE "\n", GeneralParamsMirror.PeriodMilliSecsTaskLogic );
		fprintf( File,S_LINE "PERIODIC_INPUTS_REFRESH=%d" E_LINE "\n", GeneralParamsMirror.PeriodMilliSecsTaskScanInputs );
printf("Real I/O choice (save):%d\n", GeneralParamsMirror.RealInputsOutputsOnlyOnTarget);
		fprintf( File,S_LINE "REAL_INPUTS_OUTPUTS_ONLY_ON_TARGET=%d" E_LINE "\n", GeneralParamsMirror.RealInputsOutputsOnlyOnTarget );
		fprintf( File,S_LINE "AUTO_ADJUST_SUMMER_WINTER_TIME=%d" E_LINE "\n", GeneralParamsMirror.AutomaticallyAdjustSummerWinterTime );
		fprintf( File,S_LINE "SIZE_NBR_RUNGS=%d" E_LINE "\n", GeneralParamsMirror.SizesInfos.nbr_rungs );
		fprintf( File,S_LINE "SIZE_NBR_BITS=%d" E_LINE "\n", GeneralParamsMirror.SizesInfos.nbr_bits );
		fprintf( File,S_LINE "SIZE_NBR_WORDS=%d" E_LINE "\n", GeneralParamsMirror.SizesInfos.nbr_words );
#ifdef OLD_TIMERS_MONOS_SUPPORT
		fprintf( File,S_LINE "SIZE_NBR_TIMERS=%d" E_LINE "\n", GeneralParamsMirror.SizesInfos.nbr_timers );
		fprintf( File,S_LINE "SIZE_NBR_MONOSTABLES=%d" E_LINE "\n", GeneralParamsMirror.SizesInfos.nbr_monostables );
#endif
		fprintf( File,S_LINE "SIZE_NBR_COUNTERS=%d" E_LINE "\n", GeneralParamsMirror.SizesInfos.nbr_counters );
		fprintf( File,S_LINE "SIZE_NBR_TIMERS_IEC=%d" E_LINE "\n", GeneralParamsMirror.SizesInfos.nbr_timers_iec );
		fprintf( File,S_LINE "SIZE_NBR_REGISTERS=%d" E_LINE "\n", GeneralParamsMirror.SizesInfos.nbr_registers );
		fprintf( File,S_LINE "SIZE_REGISTER_LIST=%d" E_LINE "\n", GeneralParamsMirror.SizesInfos.register_list_size );
		fprintf( File,S_LINE "SIZE_NBR_PHYS_INPUTS=%d" E_LINE "\n", GeneralParamsMirror.SizesInfos.nbr_phys_inputs );
		fprintf( File,S_LINE "SIZE_NBR_PHYS_OUTPUTS=%d" E_LINE "\n", GeneralParamsMirror.SizesInfos.nbr_phys_outputs );
		fprintf( File,S_LINE "SIZE_NBR_ARITHM_EXPR=%d" E_LINE "\n", GeneralParamsMirror.SizesInfos.nbr_arithm_expr );
		fprintf( File,S_LINE "SIZE_NBR_SECTIONS=%d" E_LINE "\n", GeneralParamsMirror.SizesInfos.nbr_sections );
		fprintf( File,S_LINE "SIZE_NBR_SYMBOLS=%d" E_LINE "\n", GeneralParamsMirror.SizesInfos.nbr_symbols );
		fprintf( File,S_LINE "SIZE_NBR_PHYS_WORDS_INPUTS=%d" E_LINE "\n", GeneralParamsMirror.SizesInfos.nbr_phys_words_inputs );
		fprintf( File,S_LINE "SIZE_NBR_PHYS_WORDS_OUTPUTS=%d" E_LINE "\n", GeneralParamsMirror.SizesInfos.nbr_phys_words_outputs );
		fprintf( File,S_LINE "MODBUS_MASTER_SERIAL_PORT=%s" E_LINE "\n", ModbusConfig.ModbusSerialPortNameUsed );
		fprintf( File,S_LINE "MODBUS_MASTER_SERIAL_SPEED=%d" E_LINE "\n", ModbusConfig.ModbusSerialSpeed );
		fprintf( File,S_LINE "MODBUS_MASTER_SERIAL_DATABITS=%d" E_LINE "\n", ModbusConfig.ModbusSerialDataBits );
		fprintf( File,S_LINE "MODBUS_MASTER_SERIAL_PARITY=%d" E_LINE "\n", ModbusConfig.ModbusSerialParity );
		fprintf( File,S_LINE "MODBUS_MASTER_SERIAL_STOPBITS=%d" E_LINE "\n", ModbusConfig.ModbusSerialStopBits );
		fclose(File);
		Okay = TRUE;
	}
	return (Okay);
}

char LoadComParameters(char * FileName)
{
	FILE * File;
	char Okay = FALSE;
	char Line[300];
	char * LineOk;
	File = fopen(FileName,"rt");
	if (File)
	{
		do
		{
			LineOk = cl_fgets(Line,300,File);
			if (LineOk)
			{
				char * pParameter;
				pParameter = "MODBUS_ELEMENT_OFFSET=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					ModbusConfig.ModbusEleOffset = atoi( &Line[ strlen( pParameter) ] );
				pParameter = "MODBUS_MASTER_SERIAL_USE_RTS_TO_SEND=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					ModbusConfig.ModbusSerialUseRtsToSend = atoi( &Line[ strlen( pParameter) ] );
				pParameter = "MODBUS_MASTER_TIME_INTER_FRAME=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					ModbusConfig.ModbusTimeInterFrame = atoi( &Line[ strlen( pParameter) ] );
				pParameter = "MODBUS_MASTER_TIME_OUT_RECEIPT=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					ModbusConfig.ModbusTimeOutReceipt = atoi( &Line[ strlen( pParameter) ] );
				pParameter = "MODBUS_MASTER_TIME_AFTER_TRANSMIT=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					ModbusConfig.ModbusTimeAfterTransmit = atoi( &Line[ strlen( pParameter) ] );
				pParameter = "MODBUS_DEBUG_LEVEL=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					ModbusConfig.ModbusDebugLevel = atoi( &Line[ strlen( pParameter) ] );
				pParameter = "MODBUS_MAP_TYPE_FOR_READ_INPUTS=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					ModbusConfig.MapTypeForReadInputs = atoi( &Line[ strlen( pParameter) ] );
				pParameter = "MODBUS_MAP_TYPE_FOR_READ_COILS=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					ModbusConfig.MapTypeForReadCoils = atoi( &Line[ strlen( pParameter) ] );
				pParameter = "MODBUS_MAP_TYPE_FOR_WRITE_COILS=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					ModbusConfig.MapTypeForWriteCoils = atoi( &Line[ strlen( pParameter) ] );
				pParameter = "MODBUS_MAP_TYPE_FOR_READ_INPUT_REGS=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					ModbusConfig.MapTypeForReadInputRegs = atoi( &Line[ strlen( pParameter) ] );
				pParameter = "MODBUS_MAP_TYPE_FOR_READ_HOLD_REGS=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					ModbusConfig.MapTypeForReadHoldRegs = atoi( &Line[ strlen( pParameter) ] );
				pParameter = "MODBUS_MAP_TYPE_FOR_WRITE_HOLD_REGS=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					ModbusConfig.MapTypeForWriteHoldRegs = atoi( &Line[ strlen( pParameter) ] );
			}
		}
		while(LineOk);
		fclose(File);
		Okay = TRUE;
	}
	return (Okay);
}

char SaveComParameters(char * FileName)
{
	FILE * File;
	char Okay = FALSE;
	File = fopen(FileName,"wt");
	if (File)
	{
		fprintf( File,S_LINE "MODBUS_ELEMENT_OFFSET=%d" E_LINE "\n", ModbusConfig.ModbusEleOffset );
		fprintf( File,S_LINE "MODBUS_MASTER_SERIAL_USE_RTS_TO_SEND=%d" E_LINE "\n", ModbusConfig.ModbusSerialUseRtsToSend );
		fprintf( File,S_LINE "MODBUS_MASTER_TIME_INTER_FRAME=%d" E_LINE "\n", ModbusConfig.ModbusTimeInterFrame );
		fprintf( File,S_LINE "MODBUS_MASTER_TIME_OUT_RECEIPT=%d" E_LINE "\n", ModbusConfig.ModbusTimeOutReceipt );
		fprintf( File,S_LINE "MODBUS_MASTER_TIME_AFTER_TRANSMIT=%d" E_LINE "\n", ModbusConfig.ModbusTimeAfterTransmit );
		fprintf( File,S_LINE "MODBUS_DEBUG_LEVEL=%d" E_LINE "\n", ModbusConfig.ModbusDebugLevel );
		fprintf( File,S_LINE "MODBUS_MAP_TYPE_FOR_READ_INPUTS=%d" E_LINE "\n", ModbusConfig.MapTypeForReadInputs );
		fprintf( File,S_LINE "MODBUS_MAP_TYPE_FOR_READ_COILS=%d" E_LINE "\n", ModbusConfig.MapTypeForReadCoils );
		fprintf( File,S_LINE "MODBUS_MAP_TYPE_FOR_WRITE_COILS=%d" E_LINE "\n", ModbusConfig.MapTypeForWriteCoils );
		fprintf( File,S_LINE "MODBUS_MAP_TYPE_FOR_READ_INPUT_REGS=%d" E_LINE "\n", ModbusConfig.MapTypeForReadInputRegs );
		fprintf( File,S_LINE "MODBUS_MAP_TYPE_FOR_READ_HOLD_REGS=%d" E_LINE "\n", ModbusConfig.MapTypeForReadHoldRegs );
		fprintf( File,S_LINE "MODBUS_MAP_TYPE_FOR_WRITE_HOLD_REGS=%d" E_LINE "\n", ModbusConfig.MapTypeForWriteHoldRegs );
		fclose(File);
		Okay = TRUE;
	}
	return (Okay);
}

#ifdef COMPLETE_PLC
char LoadConfigEventsLog(char * FileName)
{
	FILE * File;
	char Okay = FALSE;
	char Line[300];
	char * LineOk;
	int NumConfigEvtLog = 0;
	char *PtrStrings[ 7 ];
	int *PtrInts[ 7 ];
	StrConfigEventLog * pCfgEvtLog;
	File = fopen(FileName,"rt");
	if (File)
	{
		do
		{
			LineOk = cl_fgets(Line,300,File);
			if (LineOk)
			{
				switch(Line[0])
				{
					case ';':
						break;
					case '#':
						if(strncmp(&Line[1],"VER=",4)==0)
						{
							if (atoi(&Line[5])>1)
							{
								printf("Config Events file version not supported...\n");
								LineOk = FALSE;
							}
						}
						break;
					default:
						NumConfigEvtLog = atoi( Line );
						pCfgEvtLog = &ConfigEventLog[ NumConfigEvtLog ];
						PtrStrings[ 0 ] = NULL; PtrInts[ 0 ] = &NumConfigEvtLog; //first field already read in previous atoi()
						PtrStrings[ 1 ] = NULL; PtrInts[ 1 ] = &pCfgEvtLog->FirstVarNum;
						PtrStrings[ 2 ] = NULL; PtrInts[ 2 ] = &pCfgEvtLog->NbrVars;
						PtrStrings[ 3 ] = NULL; PtrInts[ 3 ] = &pCfgEvtLog->EventLevel; 
						PtrStrings[ 4 ] = pCfgEvtLog->Symbol; PtrInts[ 4 ] = (int *)EVENT_SYMBOL_LGT;
						PtrStrings[ 5 ] = pCfgEvtLog->Text; PtrInts[ 5 ] = (int *)EVENT_TEXT_LGT;
						PtrStrings[ 6 ] = NULL; PtrInts[ 6 ] = &pCfgEvtLog->RemoteAlarmsForwardSlot;
//						ConvRawLineOfStringsOrNumbers( Line, 6, PtrStrings, PtrInts, NULL );
						ConvRawLineOfStringsOrNumbers( Line, 7, PtrStrings, PtrInts, NULL );
//printf("LoadedCfgEvt%d: %d - %d - %s - %s\n", NumConfigEvtLog, pCfgEvtLog->FirstVarNum, pCfgEvtLog->NbrVars, pCfgEvtLog->Symbol, pCfgEvtLog->Text);
						break;
				}
			}
		}
		while(LineOk);
		fclose(File);
		Okay = TRUE;
	}
	return (Okay);
}
char SaveConfigEventsLog(char * FileName)
{
	FILE * File;
	char Okay = FALSE;
	int Scan = 0;
	StrConfigEventLog * pCfgEvtLog;
	File = fopen(FileName,"wt");
	if (File)
	{
		fprintf(File,S_LINE "#VER=1.0" E_LINE "\n");
		do
		{
			pCfgEvtLog = &ConfigEventLog[ Scan ];
			if ( pCfgEvtLog->FirstVarNum!=-1 && pCfgEvtLog->NbrVars!=0 )
				fprintf(File,S_LINE "%d,%d,%d,%d,%s,%s,%d" E_LINE "\n", Scan, pCfgEvtLog->FirstVarNum, pCfgEvtLog->NbrVars, pCfgEvtLog->EventLevel, pCfgEvtLog->Symbol, pCfgEvtLog->Text, pCfgEvtLog->RemoteAlarmsForwardSlot );
			Scan++;
		}
		while(Scan<NBR_CONFIG_EVENTS_LOG);
		fclose(File);
		Okay = TRUE;
	}
	return (Okay);
}
#endif

char LoadBoolFreeVarsSpyList(char * FileName)
{
	char Okay = FALSE;
#ifdef GTK_INTERFACE
	FILE * File;
	char Line[300];
	char * LineOk;
	int NumVarSpy;
	//final datas not in an array, so we convert line values in this one...
	int Params[NBR_FREE_VAR_SPY];
	File = fopen(FileName,"rt");
	if (File)
	{
		LineOk = cl_fgets(Line,300,File);
		if (LineOk)
		{
			ConvRawLineOfNumbers(Line,NBR_FREE_VAR_SPY,Params,0/*DefaultValueToComplete*/);
			for (NumVarSpy=0; NumVarSpy<NBR_FREE_VAR_SPY; NumVarSpy++)
				InfosGUI->FreeVarSpy[NumVarSpy].VarType = Params[ NumVarSpy ];
			LineOk = cl_fgets(Line,300,File);
			if (LineOk)
			{
				ConvRawLineOfNumbers(Line,NBR_FREE_VAR_SPY,Params,0/*DefaultValueToComplete*/);
				for (NumVarSpy=0; NumVarSpy<NBR_FREE_VAR_SPY; NumVarSpy++)
					InfosGUI->FreeVarSpy[NumVarSpy].VarNum = Params[ NumVarSpy ];
				LineOk = cl_fgets(Line,300,File);
				if (LineOk)
				{
					ConvRawLineOfNumbers(Line,NBR_TYPE_BOOLS_SPY,InfosGUI->ValOffsetBoolVar,0/*DefaultValueToComplete*/);
					LineOk = cl_fgets(Line,300,File);
					if (LineOk)
					{
						ConvRawLineOfNumbers(Line,NBR_FREE_VAR_SPY,InfosGUI->FreeVarSpyDisplayFormat,0/*DefaultValueToComplete*/);
					}
				}
			}
		}
		fclose(File);
		Okay = TRUE;
	}
#endif
	return (Okay);
}

char SaveBoolFreeVarsSpyList(char * FileName)
{
	char Okay = FALSE;
#ifdef GTK_INTERFACE
	FILE * File;
	File = fopen(FileName,"wt");
	if (File)
	{
		int ScanIt;
		for (ScanIt=0; ScanIt<NBR_FREE_VAR_SPY; ScanIt++)
			fprintf( File, "%s%d%s", (ScanIt==0)?S_LINE:"", InfosGUI->FreeVarSpy[ScanIt].VarType, (ScanIt<NBR_FREE_VAR_SPY-1)?",":E_LINE"\n" );
		for (ScanIt=0; ScanIt<NBR_FREE_VAR_SPY; ScanIt++)
			fprintf( File, "%s%d%s", (ScanIt==0)?S_LINE:"", InfosGUI->FreeVarSpy[ScanIt].VarNum, (ScanIt<NBR_FREE_VAR_SPY-1)?",":E_LINE"\n" );
		for (ScanIt=0; ScanIt<NBR_TYPE_BOOLS_SPY; ScanIt++)
			fprintf( File, "%s%d%s", (ScanIt==0)?S_LINE:"", InfosGUI->ValOffsetBoolVar[ScanIt], (ScanIt<NBR_TYPE_BOOLS_SPY-1)?",":E_LINE"\n" );
		for (ScanIt=0; ScanIt<NBR_FREE_VAR_SPY; ScanIt++)
			fprintf( File, "%s%d%s", (ScanIt==0)?S_LINE:"", InfosGUI->FreeVarSpyDisplayFormat[ScanIt], (ScanIt<NBR_FREE_VAR_SPY-1)?",":E_LINE"\n" );
		fclose(File);
		Okay = TRUE;
	}
#endif
	return (Okay);
}

// A C str_replace() function by Laird Shaw, with assistance and inspiration from comp.lang.c and stackoverflow
char *replace_str(const char *str, const char *old, const char *new)
{
	char *ret, *r;
	const char *p, *q;
	size_t oldlen = strlen(old);
	size_t count, retlen, newlen = strlen(new);
	int samesize = (oldlen == newlen);

	if (!samesize) {
		for (count = 0, p = str; (q = strstr(p, old)) != NULL; p = q + oldlen)
			count++;
		/* This is undefined if p - str > PTRDIFF_MAX */
		retlen = p - str + strlen(p) + count * (newlen - oldlen);
	} else
		retlen = strlen(str);

	if ((ret = malloc(retlen + 1)) == NULL)
		return NULL;

	r = ret, p = str;
	while (1) {
		/* If the old and new strings are different lengths - in other
		 * words we have already iterated through with strstr above,
		 * and thus we know how many times we need to call it - then we
		 * can avoid the final (potentially lengthy) call to strstr,
		 * which we already know is going to return NULL, by
		 * decrementing and checking count.
		 */
		if (!samesize && !count--)
			break;
		/* Otherwise i.e. when the old and new strings are the same
		 * length, and we don't know how many times to call strstr,
		 * we must check for a NULL return here (we check it in any
		 * event, to avoid further conditions, and because there's
		 * no harm done with the check even when the old and new
		 * strings are different lengths).
		 */
		if ((q = strstr(p, old)) == NULL)
			break;
		/* This is undefined if q - p > PTRDIFF_MAX */
		ptrdiff_t l = q - p;
		memcpy(r, p, l);
		r += l;
		memcpy(r, new, newlen);
		r += newlen;
		p = q + oldlen;
	}
	strcpy(r, p);

	return ret;
}

char LoadProjectProperties(char * FileName)
{
	FILE * File;
	char Okay = FALSE;
	char Line[300];
	char * LineOk;
	File = fopen(FileName,"rt");
	if (File)
	{
		do
		{
			LineOk = cl_fgets(Line,300,File);
			if (LineOk)
			{
				char * pParameter;
				pParameter = "PROJECT_NAME=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					strcpy( InfosGene->ProjectProperties.ProjectName, &Line[ strlen( pParameter) ] );
				pParameter = "PROJECT_SITE=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					strcpy( InfosGene->ProjectProperties.ProjectSite, &Line[ strlen( pParameter) ] );
				pParameter = "PARAM_VERSION=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					strcpy( InfosGene->ProjectProperties.ParamVersion, &Line[ strlen( pParameter) ] );
				pParameter = "PARAM_AUTHOR=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					strcpy( InfosGene->ProjectProperties.ParamAuthor, &Line[ strlen( pParameter) ] );
				pParameter = "PARAM_COMPANY=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					strcpy( InfosGene->ProjectProperties.ParamCompany, &Line[ strlen( pParameter) ] );
				pParameter = "CREA_DATE=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					strcpy( InfosGene->ProjectProperties.ParamCreaDate, &Line[ strlen( pParameter) ] );
				pParameter = "MODIF_DATE=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					strcpy( InfosGene->ProjectProperties.ParamModifDate, &Line[ strlen( pParameter) ] );
				pParameter = "PARAM_COMMENT=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
				{
					char * pCommentWithoutNewLines = replace_str( &Line[ strlen( pParameter) ], "\\n", "\n" );
					strcpy( InfosGene->ProjectProperties.ParamComment, pCommentWithoutNewLines );
					free( pCommentWithoutNewLines );
				}
			}
		}
		while(LineOk);
		fclose(File);
		Okay = TRUE;
	}
	return (Okay);
}

char SaveProjectProperties(char * FileName)
{
	FILE * File;
	char Okay = FALSE;
	File = fopen(FileName,"wt");
	if (File)
	{
		char * pCommentWithoutNewLines;
		fprintf( File,S_LINE "PROJECT_NAME=%s" E_LINE "\n", InfosGene->ProjectProperties.ProjectName );
		fprintf( File,S_LINE "PROJECT_SITE=%s" E_LINE "\n", InfosGene->ProjectProperties.ProjectSite );
		fprintf( File,S_LINE "PARAM_VERSION=%s" E_LINE "\n", InfosGene->ProjectProperties.ParamVersion );
		fprintf( File,S_LINE "PARAM_AUTHOR=%s" E_LINE "\n", InfosGene->ProjectProperties.ParamAuthor );
		fprintf( File,S_LINE "PARAM_COMPANY=%s" E_LINE "\n", InfosGene->ProjectProperties.ParamCompany );
		fprintf( File,S_LINE "CREA_DATE=%s" E_LINE "\n", InfosGene->ProjectProperties.ParamCreaDate );
		fprintf( File,S_LINE "MODIF_DATE=%s" E_LINE "\n", InfosGene->ProjectProperties.ParamModifDate );
		pCommentWithoutNewLines = replace_str( InfosGene->ProjectProperties.ParamComment, "\n", "\\n" );
		fprintf( File,S_LINE "PARAM_COMMENT=%s" E_LINE "\n", pCommentWithoutNewLines );
		free( pCommentWithoutNewLines );
		fclose(File);
		Okay = TRUE;
	}
	return (Okay);
}

char LoadModemConfig(char * FileName)
{
	FILE * File;
	char Okay = FALSE;
	char Line[300];
	char * LineOk;
	File = fopen(FileName,"rt");
	if (File)
	{
		do
		{
			LineOk = cl_fgets(Line,300,File);
			if (LineOk)
			{
				char * pParameter;
				pParameter = "MODEM_USED=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					Modem.ModemUsed = atoi( &Line[ strlen( pParameter) ] )?TRUE:FALSE;
				pParameter = "INIT_SEQ=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					strcpy( Modem.StrInitSequence, &Line[ strlen( pParameter) ] );
				pParameter = "CONFIG_SEQ=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					strcpy( Modem.StrConfigSequence, &Line[ strlen( pParameter) ] );
				pParameter = "CALL_SEQ=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					strcpy( Modem.StrCallSequence, &Line[ strlen( pParameter) ] );
				pParameter = "PIN_CODE=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					strcpy( Modem.StrCodePIN, &Line[ strlen( pParameter) ] );
			}
		}
		while(LineOk);
		fclose(File);
		Okay = TRUE;
	}
	return (Okay);
}

char SaveModemConfig(char * FileName)
{
	FILE * File;
	char Okay = FALSE;
//printf("WRITE MODEM CONFIG FILE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	File = fopen(FileName,"wt");
	if (File)
	{
		fprintf( File,S_LINE "MODEM_USED=%d" E_LINE "\n", Modem.ModemUsed?1:0 );
		fprintf( File,S_LINE "INIT_SEQ=%s" E_LINE "\n", Modem.StrInitSequence );
		fprintf( File,S_LINE "CONFIG_SEQ=%s" E_LINE "\n", Modem.StrConfigSequence );
		fprintf( File,S_LINE "CALL_SEQ=%s" E_LINE "\n", Modem.StrCallSequence );
		fprintf( File,S_LINE "PIN_CODE=%s" E_LINE "\n", Modem.StrCodePIN );
		fclose(File);
		Okay = TRUE;
//printf("WRITTEN MODEM CONFIG FILE %s !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n",FileName);
	}
	return (Okay);
}

char LoadRemoteAlarmsConfig(char * FileName)
{
	FILE * File;
	char Okay = FALSE;
	char Line[300];
	char * LineOk;
	StrRemoteAlarms * pAlarms = &RemoteAlarmsConfig;
	File = fopen(FileName,"rt");
	if (File)
	{
		do
		{
			LineOk = cl_fgets(Line,300,File);
			if (LineOk)
			{
				char * pParameter;
				int LgtPartialKey;
				pParameter = "GLOBAL_ENABLED=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					pAlarms->GlobalEnabled = atoi( &Line[ strlen( pParameter) ] )?TRUE:FALSE;
				// partial keyword (without "XX=")
				pParameter = "SLOT_NAME_";
				LgtPartialKey = strlen( pParameter);
				if ( strncmp( Line, pParameter, LgtPartialKey )==0 )
				{
					int iAlarmSlot = atoi( &Line[LgtPartialKey] );
					if ( iAlarmSlot<NBR_ALARMS_SLOTS )
						strcpy( pAlarms->SlotName[ iAlarmSlot ], &Line[ LgtPartialKey+3 ] );
				}
				// partial keyword (without "XX=")
				pParameter = "ALARM_TYPE_";
				LgtPartialKey = strlen( pParameter);
				if ( strncmp( Line, pParameter, LgtPartialKey )==0 )
				{
					int iAlarmSlot = atoi( &Line[LgtPartialKey] );
					if ( iAlarmSlot<NBR_ALARMS_SLOTS )
						pAlarms->AlarmType[ iAlarmSlot ] =  atoi( &Line[ LgtPartialKey+3 ] );
				}
				// partial keyword (without "XX=")
				pParameter = "TELEPHONE_NUMBER_";
				LgtPartialKey = strlen( pParameter);
				if ( strncmp( Line, pParameter, LgtPartialKey )==0 )
				{
					int iAlarmSlot = atoi( &Line[LgtPartialKey] );
					if ( iAlarmSlot<NBR_ALARMS_SLOTS )
						strcpy( pAlarms->TelephoneNumber[ iAlarmSlot ], &Line[ LgtPartialKey+3 ] );
				}
				// partial keyword (without "XX=")
				pParameter = "EMAIL_ADDRESS_";
				LgtPartialKey = strlen( pParameter);
				if ( strncmp( Line, pParameter, LgtPartialKey )==0 )
				{
					int iAlarmSlot = atoi( &Line[LgtPartialKey] );
					if ( iAlarmSlot<NBR_ALARMS_SLOTS )
						strcpy( pAlarms->EmailAddress[ iAlarmSlot ], &Line[ LgtPartialKey+3 ] );
				}
				pParameter = "CENTER_SERVER_SMS=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					strcpy( pAlarms->CenterServerSMS, &Line[ strlen( pParameter) ] );
				pParameter = "SMTP_SERVER_FOR_EMAILS=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					strcpy( pAlarms->SmtpServerForEmails, &Line[ strlen( pParameter) ] );
				pParameter = "SMTP_SERVER_USERNAME=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					strcpy( pAlarms->SmtpServerUserName, &Line[ strlen( pParameter) ] );
				pParameter = "SMTP_SERVER_PASSWORD=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					strcpy( pAlarms->SmtpServerPassword, &Line[ strlen( pParameter) ] );
				pParameter = "EMAIL_SENDER_ADDRESS=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					strcpy( pAlarms->EmailSenderAddress, &Line[ strlen( pParameter) ] );
			}
		}
		while(LineOk);
		fclose(File);
		Okay = TRUE;
	}
	return (Okay);
}

char SaveRemoteAlarmsConfig(char * FileName)
{
	FILE * File;
	char Okay = FALSE;
	StrRemoteAlarms * pAlarms = &RemoteAlarmsConfig;
	File = fopen(FileName,"wt");
	if (File)
	{
		int ScanAlarmSlot;
		fprintf( File,S_LINE "GLOBAL_ENABLED=%d" E_LINE "\n", pAlarms->GlobalEnabled?1:0 );
		for( ScanAlarmSlot=0; ScanAlarmSlot<NBR_ALARMS_SLOTS; ScanAlarmSlot++ )
		{
			fprintf( File,S_LINE "SLOT_NAME_%02d=%s" E_LINE "\n", ScanAlarmSlot, pAlarms->SlotName[ ScanAlarmSlot ] );
			fprintf( File,S_LINE "ALARM_TYPE_%02d=%d" E_LINE "\n", ScanAlarmSlot, pAlarms->AlarmType[ ScanAlarmSlot ] );
			fprintf( File,S_LINE "TELEPHONE_NUMBER_%02d=%s" E_LINE "\n", ScanAlarmSlot, pAlarms->TelephoneNumber[ ScanAlarmSlot ] );
			fprintf( File,S_LINE "EMAIL_ADDRESS_%02d=%s" E_LINE "\n", ScanAlarmSlot, pAlarms->EmailAddress[ ScanAlarmSlot ] );
		}
		fprintf( File,S_LINE "CENTER_SERVER_SMS=%s" E_LINE "\n", pAlarms->CenterServerSMS );
		fprintf( File,S_LINE "SMTP_SERVER_FOR_EMAILS=%s" E_LINE "\n", pAlarms->SmtpServerForEmails );
		fprintf( File,S_LINE "SMTP_SERVER_USERNAME=%s" E_LINE "\n", pAlarms->SmtpServerUserName );
		fprintf( File,S_LINE "SMTP_SERVER_PASSWORD=%s" E_LINE "\n", pAlarms->SmtpServerPassword );
		fprintf( File,S_LINE "EMAIL_SENDER_ADDRESS=%s" E_LINE "\n", pAlarms->EmailSenderAddress );
		fclose(File);
		Okay = TRUE;
	}
	return (Okay);
}

void DeleteTheDefaultSection( )
{
	RungArray[ 0 ].Used = FALSE;
	SectionArray[ 0 ].Used = FALSE;
}

char FileName[500];
void LoadAllLadderDatas(char * DatasDirectory)
{
//moved up in LoadProject() function... well no more now in AllocProjectDatas()! 	ClassicLadder_InitProjectDatas( );
	// not necessary to have the default section, as we will load a working project
	// and annoying if the section (with internal number 0) has been deleted in this project !
	DeleteTheDefaultSection( );

	printf("Loading datas from %s...\n", DatasDirectory);

	sprintf(FileName,"%s/"FILE_PREFIX"project_infos.txt",DatasDirectory);
	LoadProjectProperties( FileName );
//v0.9.20	sprintf(FileName,"%s/"FILE_PREFIX"general.txt",DatasDirectory);
//v0.9.20	LoadGeneralParameters( FileName );
	sprintf(FileName,"%s/"FILE_PREFIX"com_params.txt",DatasDirectory);
	LoadComParameters( FileName );
#ifdef OLD_TIMERS_MONOS_SUPPORT
	sprintf(FileName,"%s/"FILE_PREFIX"timers.csv",DatasDirectory);
//	printf("Loading timers datas from %s\n",FileName);
	LoadTimersParams(FileName);
	sprintf(FileName,"%s/"FILE_PREFIX"monostables.csv",DatasDirectory);
//	printf("Loading monostables datas from %s\n",FileName);
	LoadMonostablesParams(FileName);
#endif
	sprintf(FileName,"%s/"FILE_PREFIX"counters.csv",DatasDirectory);
//	printf("Loading counters datas from %s\n",FileName);
	LoadCountersParams(FileName);
	sprintf(FileName,"%s/"FILE_PREFIX"timers_iec.csv",DatasDirectory);
	LoadNewTimersParams(FileName);
	sprintf(FileName,"%s/"FILE_PREFIX"registers.csv",DatasDirectory);
	LoadRegistersParams(FileName);

	sprintf(FileName,"%s/"FILE_PREFIX"arithmetic_expressions.csv",DatasDirectory);
//	printf("Loading arithmetic expressions from %s\n",FileName);
	LoadArithmeticExpr(FileName);

	// Sections added since v0.5.5, the format of files has a little changed :
	// before the prev/next rungs were not saved in each rung...
	// and the number of rungs changed when saved...
	sprintf(FileName,"%s/"FILE_PREFIX"sections.csv",DatasDirectory);
//	printf("Loading sections datas from %s\n",FileName);
	if ( LoadSectionsParams(FileName) )
	{
		sprintf(FileName,"%s/"FILE_PREFIX"rung_",DatasDirectory);
		LoadAllRungs(FileName,RungArray);
	}
	else
	{
		printf("Rungs with old format found (no sections)\n");
		sprintf(FileName,"%s/"FILE_PREFIX"rung_",DatasDirectory);
		LoadAllRungs_V1(FileName,RungArray,&InfosGene->FirstRung,&InfosGene->LastRung,&InfosGene->CurrentRung);
		// if we load old format files, sections wasn't created, so we must write theses infos...
		SectionArray[ 0 ].FirstRung = InfosGene->FirstRung;
		SectionArray[ 0 ].LastRung = InfosGene->LastRung;
	}
#ifdef SEQUENTIAL_SUPPORT
	sprintf(FileName,"%s/"FILE_PREFIX"sequential.csv",DatasDirectory);
//	printf("Loading sequential datas from %s\n",FileName);
	LoadSequential( FileName );
#endif
	sprintf(FileName,"%s/"FILE_PREFIX"ioconf.csv",DatasDirectory);
//	printf("Loading I/O configuration datas from %s\n",FileName);
	LoadIOConfParams( FileName );
#ifdef MODBUS_IO_MASTER
	sprintf(FileName,"%s/"FILE_PREFIX"modbusioconf.csv",DatasDirectory);
//	printf("Loading modbus distributed I/O configuration datas from %s\n",FileName);
	LoadModbusIOConfParams( FileName );
#endif
	sprintf(FileName,"%s/"FILE_PREFIX"symbols.csv",DatasDirectory);
//	printf("Loading symbols datas from %s\n",FileName);
	LoadSymbols(FileName);
#ifdef COMPLETE_PLC
	sprintf(FileName,"%s/"FILE_PREFIX"config_events.csv",DatasDirectory);
	LoadConfigEventsLog(FileName);
	sprintf(FileName,"%s/"FILE_PREFIX"modem_config.txt",DatasDirectory);
	LoadModemConfig( FileName );
	sprintf(FileName,"%s/"FILE_PREFIX"remote_alarms.txt",DatasDirectory);
	LoadRemoteAlarmsConfig( FileName );
#endif
	sprintf(FileName,"%s/"FILE_PREFIX"spy_vars.csv",DatasDirectory);
	LoadBoolFreeVarsSpyList(FileName);

	// security if empty file...
#ifdef GTK_INTERFACE
	if ( NbrSectionsDefined()==0 )
		AddSection("Prog1", SECTION_IN_LADDER, -1 );
#endif	

//printf("Prepare all datas before run...\n");
	PrepareAllDatasBeforeRun( );
#ifdef COMPLETE_PLC
	// update the tags list of the variables that the user want to log (after to have load the config file...)
	InitVarsArrayLogTags( );
#endif
}

void SaveAllLadderDatas(char * DatasDirectory)
{
	CleanTmpLadderDirectory( FALSE/*DestroyDir*/ );
	sprintf(FileName,"%s/"FILE_PREFIX"project_infos.txt",DatasDirectory);
	SaveProjectProperties( FileName );
	AddFileToFilesProjectList( "project_infos.txt" );
	sprintf(FileName,"%s/"FILE_PREFIX"general.txt",DatasDirectory);
	SaveGeneralParameters( FileName );
	AddFileToFilesProjectList( "general.txt" );
#ifdef OLD_TIMERS_MONOS_SUPPORT
	sprintf(FileName,"%s/"FILE_PREFIX"timers.csv",DatasDirectory);
	SaveTimersParams(FileName);
	AddFileToFilesProjectList( "timers.csv" );
	sprintf(FileName,"%s/"FILE_PREFIX"monostables.csv",DatasDirectory);
	SaveMonostablesParams(FileName);
	AddFileToFilesProjectList( "monostables.csv" );
#endif
	sprintf(FileName,"%s/"FILE_PREFIX"counters.csv",DatasDirectory);
	SaveCountersParams(FileName);
	AddFileToFilesProjectList( "counters.csv" );
	sprintf(FileName,"%s/"FILE_PREFIX"timers_iec.csv",DatasDirectory);
	SaveNewTimersParams(FileName);
	AddFileToFilesProjectList( "timers_iec.csv" );
	sprintf(FileName,"%s/"FILE_PREFIX"registers.csv",DatasDirectory);
	SaveRegistersParams(FileName);
	AddFileToFilesProjectList( "registers.csv" );

	sprintf(FileName,"%s/"FILE_PREFIX"arithmetic_expressions.csv",DatasDirectory);
	SaveArithmeticExpr(FileName);
	AddFileToFilesProjectList( "arithmetic_expressions.csv" );
	sprintf(FileName,"%s/"FILE_PREFIX"rung_",DatasDirectory);
	SaveAllRungs(FileName);
	sprintf(FileName,"%s/"FILE_PREFIX"sections.csv",DatasDirectory);
	SaveSectionsParams( FileName );
	AddFileToFilesProjectList( "sections.csv" );
#ifdef SEQUENTIAL_SUPPORT
	sprintf(FileName,"%s/"FILE_PREFIX"sequential.csv",DatasDirectory);
	SaveSequential( FileName );
	AddFileToFilesProjectList( "sequential.csv" );
#endif
	sprintf(FileName,"%s/"FILE_PREFIX"symbols.csv",DatasDirectory);
	SaveSymbols( FileName );
	AddFileToFilesProjectList( "symbols.csv" );
	sprintf(FileName,"%s/"FILE_PREFIX"ioconf.csv",DatasDirectory);
	SaveIOConfParams( FileName );
	AddFileToFilesProjectList( "ioconf.csv" );
	sprintf(FileName,"%s/"FILE_PREFIX"com_params.txt",DatasDirectory);
	SaveComParameters( FileName );
	AddFileToFilesProjectList( "com_params.txt" );
#ifdef MODBUS_IO_MASTER
	sprintf(FileName,"%s/"FILE_PREFIX"modbusioconf.csv",DatasDirectory);
	SaveModbusIOConfParams( FileName );
	AddFileToFilesProjectList( "modbusioconf.csv" );
#endif
#ifdef COMPLETE_PLC
	sprintf(FileName,"%s/"FILE_PREFIX"config_events.csv",DatasDirectory);
	SaveConfigEventsLog( FileName );
	AddFileToFilesProjectList( "config_events.csv" );
	sprintf(FileName,"%s/"FILE_PREFIX"modem_config.txt",DatasDirectory);
	SaveModemConfig( FileName );
	AddFileToFilesProjectList( "modem_config.txt" );
	sprintf(FileName,"%s/"FILE_PREFIX"remote_alarms.txt",DatasDirectory);
	SaveRemoteAlarmsConfig( FileName );
	AddFileToFilesProjectList( "remote_alarms.txt" );
#endif
	sprintf(FileName,"%s/"FILE_PREFIX"spy_vars.csv",DatasDirectory);
	SaveBoolFreeVarsSpyList( FileName );
	AddFileToFilesProjectList( "spy_vars.csv" );
	InfosGene->AskConfirmationToQuit = FALSE;
}


/* clean the tmp directory of the parameters files */
void CleanTmpLadderDirectory( char DestroyDir )
{
	DIR *pDir;
	struct dirent *pEnt;
	char Buff[400];

	if (TmpDirectoryProject[0]!='\0')
	{
		pDir = opendir(TmpDirectoryProject);
		if (pDir)
		{
			while ((pEnt = readdir(pDir)) != NULL)
			{
				if ( strcmp(pEnt->d_name,".") && strcmp(pEnt->d_name,"..") )
				{
					char cRemoveIt = TRUE;
					// if a file prefix defined, only remove the classicladder files...
					if ( strlen(FILE_PREFIX)>0 )
					{
						if ( strncmp( pEnt->d_name, FILE_PREFIX, strlen(FILE_PREFIX) )!=0 )
							cRemoveIt = FALSE;
					}
					if ( cRemoveIt )
					{
						sprintf(Buff, "%s/%s", TmpDirectoryProject,pEnt->d_name);
						remove(Buff);
					}
				}
			}
		}
		closedir(pDir);
		/* delete the temp directory if wanted */
#ifndef __WIN32__
		if ( DestroyDir )
			rmdir(TmpDirectoryProject);
#else
		if ( DestroyDir )
			_rmdir(TmpDirectoryProject);
#endif
	}
}
