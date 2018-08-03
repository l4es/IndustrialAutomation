/* Classic Ladder Project */
/* Copyright (C) 2001-2015 Marc Le Douarain */
/* http://www.sourceforge.net/projects/classicladder */
/* http://sites.google.com/site/classicladder */
/* February 2001 */
/* ------------------------ */
/* Load/Save projects files */
/* ------------------------ */
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
#include <dirent.h>
#include <errno.h>
// for mkdir( ) Linux
#if !defined(__WIN32__)
#include <sys/stat.h>
#include <sys/types.h>
#endif
#include <zlib.h>
#include "classicladder.h"
#include "global.h"
#include "edit.h"
#include "calc.h"
#include "calc_sequential.h"
#include "files.h"
#include "files_sequential.h"
#include "files_project.h"

#if defined(EMBEDDED_MOUNT_START_PARAMS_CMD) || defined(EMBEDDED_MOUNT_END_PARAMS_CMD)
#include <sys/mount.h>
#endif

#ifdef debug
#define dbg_printf printf
#else
static inline int dbg_printf(char *f, ...) {return 0;}
#endif


//v0.9.5, replace scan temp dir, with file list to always have same order of little file
//(usefull in diff between 2 projects files...)
#define NBR_PROJECT_FILES_LIST 2000
char * pFilesProjectList[ NBR_PROJECT_FILES_LIST ];
int NbrFilesProjectList = 0;

//#ifdef GTK_INTERFACE
//char CurrentProjectFileName[400] = "projects_examples/example.clp";
//#else
//char CurrentProjectFileName[400] = "projects_examples/parallel_port_direct.clp";
//#endif


#ifdef __WIN32__
#define CAR_SEP '\\'
#else
#define CAR_SEP '/'
#endif
void VerifyDirectorySelected( char * TheFileProject, char * NewDir )
{
	strcpy( TheFileProject, NewDir );
	if (strlen(TheFileProject)>1)
	{
		if ( strcmp( &NewDir[ strlen( NewDir ) -4 ], ".clp" )!=0 && strcmp( &NewDir[ strlen( NewDir ) -6 ], ".clprj" )!=0
			&& strcmp( &NewDir[ strlen( NewDir ) -7 ], ".clprjz" )!=0 )
		{
			// verify if path given is really a directory (not a file in it)
			DIR *pDir;
			pDir = opendir(TheFileProject);
			if (pDir==NULL && errno==ENOTDIR)
			{
				int Lgt = strlen(TheFileProject);
				char * End = &TheFileProject[Lgt-1];
				do
				{
					End--;
				}
				while(*End!=CAR_SEP && --Lgt>0);
				End++;
				if ( Lgt>0 )
				{
					*End = '\0';
				}
				else
				{
					debug_printf("ERROR with path directory given for project !!!\n");
					TheFileProject[ 0 ] = '\0';
				}
			}
			else
			{
				if (TheFileProject[strlen(TheFileProject)-1]!=CAR_SEP)
					strcat( TheFileProject, "/" );
			}
			debug_printf("DIRECTORY PROJECT = %s\n",TheFileProject);
		}
	}
}



void InitTempDir( void )
{
	char * TmpEnv = getenv("TMP");
	if ( TmpEnv==NULL )
		TmpEnv = "/tmp";

	strcpy( TmpDirectoryRoot, TmpEnv );

	// get a single name directory
	sprintf( TmpDirectoryProject, "%s/classicladder_tmp_XXXXXX", TmpEnv );
#ifndef __WIN32__
	if ( mkdtemp( TmpDirectoryProject )==NULL )
#else
	if ( mktemp( TmpDirectoryProject )==NULL )
#endif
	{
		sprintf( TmpDirectoryProject, "%s/classicladder_tmp", TmpEnv );
#ifndef __WIN32__
		mkdir( TmpDirectoryProject, S_IRWXU );
#else
		mkdir( TmpDirectoryProject );
#endif
	}
#ifdef __WIN32__
	else
	{
		mkdir( TmpDirectoryProject );
	}
#endif
printf("Init tmps directories: root=%s project_dir=%s\n", TmpDirectoryRoot, TmpDirectoryProject);
}

char FileNameToLoad[500];
//v0.9.20 before loading project, will be 'stopped' if running... in 'loading' during load, and in 'stop' after loaded.
char LoadProjectFiles( char * FileProject )
{
	char Result = FALSE;
	char OldProjectFound = TRUE;
	//moved here in v0.9.20 to avoid to add that everywhere...
	StopRunIfRunning( );
	InfosGene->LadderState = STATE_LOADING;
	
//v0.9.20, no more necessary...	if ( TmpDirectoryProject[ 0 ]=='\0' )
//v0.9.20, no more necessary...		InitTempDir( );
	CleanTmpLadderDirectory( FALSE/*DestroyDir*/ );
	if ( FileProject[0]!='\0' )
	{
		/* if it is an old project, read directly from the directory selected... */
		if ( strcmp( &FileProject[ strlen( FileProject ) -4 ], ".clp" )==0 || strcmp( &FileProject[ strlen( FileProject ) -6 ], ".clprj" )==0
			|| strcmp( &FileProject[ strlen( FileProject ) -7 ], ".clprjz" )==0 )
		{
			OldProjectFound = FALSE;
		}
		if ( OldProjectFound )
		{
//v0.9.20			printf("Loading an old project (many files in the directory %s) !\n",FileProject);
//v0.9.20			LoadAllLadderDatas( FileProject );
			printf("Loading a very very old format directory project no more supported !\n");
		}
		else
		{
			//v0.9.20
//////			ClassicLadder_FreeProjectDatas( );
			
printf("Load project '%s' in tmp dir=%s\n", FileProject, TmpDirectoryProject);
			// split files of the project in the temp directory
			Result = SplitFiles( FileProject, TmpDirectoryProject );
			
			// v0.9.8, do not load at all files project if previous split failed...
			if ( Result )
			{
				
				//v0.9.20, first load general to know sizes defined in the project we have to alloc
				InitGeneralParamsMirror( );
				sprintf(FileNameToLoad,"%s/general.txt",TmpDirectoryProject);
				LoadGeneralParameters( FileNameToLoad );
				if ( !ClassicLadder_FreeAllocInitProjectDatas( ) )
				{
					printf("Failed to alloc memory required for project !!!\n" );
					exit( 1 );
				}
				LoadAllLadderDatas( TmpDirectoryProject );
			}
		}
	}
	//moved here in v0.9.20 to avoid to add that everywhere...
	InfosGene->LadderState = STATE_STOP;
	return Result;
}

/*v0.9.20
char FileName[500];
char LoadGeneralParamsOnlyFromProject( char * FileProject )
{
	char Result = FALSE;
//v0.9.20, no more necessary...	if ( TmpDirectoryProject[ 0 ]=='\0' )
//v0.9.20, no more necessary...		InitTempDir( );
	CleanTmpLadderDirectory( FALSE );
	if ( FileProject[0]!='\0' )
	{
		if ( strcmp( &FileProject[ strlen( FileProject ) -4 ], ".clp" )==0 || strcmp( &FileProject[ strlen( FileProject ) -6 ], ".clprj" )==0
			 || strcmp( &FileProject[ strlen( FileProject ) -7 ], ".clprjz" )==0 )
		{
			// split files of the project in the temp directory
			Result = SplitFiles( FileProject, TmpDirectoryProject );
			sprintf(FileName,"%s/general.txt",TmpDirectoryProject);
			LoadGeneralParameters( FileName );
		}
	}
	return Result;
}
*/

char SaveProjectFiles( char * FileProject )
{
	char CompressPrj = FALSE;
	char ProjectFileOk;
//v0.9.20, no more necessary...	if ( TmpDirectoryProject[ 0 ]=='\0' )
//v0.9.20, no more necessary...		InitTempDir( );
	SaveAllLadderDatas( TmpDirectoryProject );
	if ( strcmp( &FileProject[ strlen( FileProject ) -7 ], ".clprjz" )==0 )
	{
		CompressPrj = TRUE;
	}
	else
	{
		if ( strcmp( &FileProject[ strlen( FileProject ) -6 ], ".clprj" )!=0 )
		{
			if ( strcmp( &FileProject[ strlen( FileProject ) -4 ], ".clp" )==0 )
				FileProject[ strlen( FileProject ) -4 ] = '\0';
			strcat( FileProject, ".clprj" );
		}
	}
printf("Save project '%s' (compress=%s) from tmp dir=%s\n", FileProject, CompressPrj?"yes":"no", TmpDirectoryProject);
	// join files for the project in one file
	ProjectFileOk = JoinFiles( FileProject, TmpDirectoryProject, CompressPrj );
	FreeFilesProjectList( );
	return ProjectFileOk;
}


#define FILE_HEAD "_FILE-"
#define STR_LEN_FILE_HEAD strlen(FILE_HEAD)
// Join many parameters files in a project file
char JoinFiles( char * DirAndNameOfProject, char * TmpDirectoryFiles, char CompressedProject )
{
	char ProjectFileOk = FALSE;
	FILE * pProjectFile = NULL;
	gzFile pProjectFileZ = NULL;
	char Buff[300];
	char BuffTemp[300];
//v0.9.5	DIR *pDir;
//v0.9.5	struct dirent *pEnt;

	if ( CompressedProject )
		pProjectFileZ = gzopen( DirAndNameOfProject, "wt" );
	else
		pProjectFile = fopen( DirAndNameOfProject, "wt" );
	if ( pProjectFileZ || pProjectFile )
	{
		int ScanFileList;

		/* start line of project */
		if ( CompressedProject )
			gzputs( pProjectFileZ, "_FILES_CLASSICLADDER\n" );
		else
			fputs( "_FILES_CLASSICLADDER\n", pProjectFile );

//v0.9.5		/* read directory of the parameters files */
//v0.9.5		pDir = opendir( TmpDirectoryFiles );
//v0.9.5		if (pDir)
//v0.9.5		{
//v0.9.5			while ((pEnt = readdir(pDir)) != NULL)
			for( ScanFileList=0; ScanFileList<NbrFilesProjectList; ScanFileList++ )
			{
//v0.9.5				if ( strcmp(pEnt->d_name,".") && strcmp(pEnt->d_name,"..") )
//v0.9.5				{
					FILE * pParametersFile;
////WIN32PORT added /
					sprintf(Buff, "%s/%s", TmpDirectoryFiles, pFilesProjectList[ ScanFileList ]/*pEnt->d_name*/);
					pParametersFile = fopen( Buff, "rt" );
					if (pParametersFile)
					{
						sprintf( BuffTemp, FILE_HEAD "%s\n", pFilesProjectList[ ScanFileList ]/*pEnt->d_name*/ );
						if ( CompressedProject )
							gzputs( pProjectFileZ, BuffTemp );
						else
							fputs( BuffTemp, pProjectFile );
						while( !feof( pParametersFile ) )
						{
							char Buff[ 300 ];
							fgets( Buff, 300, pParametersFile );
							if (!feof(pParametersFile))
							{
								if ( CompressedProject )
									gzputs( pProjectFileZ, Buff );
								else
									fputs( Buff, pProjectFile );
							}
						}
						fclose( pParametersFile );
						sprintf( BuffTemp, "_/FILE-%s\n", pFilesProjectList[ ScanFileList ]/*pEnt->d_name*/ );
						if ( CompressedProject )
							gzputs( pProjectFileZ, BuffTemp );
						else
							fputs( BuffTemp, pProjectFile );
					}
//v0.9.5				}
			}
//v0.9.5			closedir(pDir);

//v0.9.5		}

		/* end line of project */
		if ( CompressedProject )
		{
			gzputs( pProjectFileZ, "_/FILES_CLASSICLADDER\n" );
			gzclose( pProjectFileZ );
		}
		else
		{
			fputs( "_/FILES_CLASSICLADDER\n", pProjectFile );
			fclose( pProjectFile );
		}

		ProjectFileOk = TRUE;
	}

	return ProjectFileOk;
}

void AddFileToFilesProjectList( char * FileName )
{
	if ( NbrFilesProjectList<NBR_PROJECT_FILES_LIST )
	{
		pFilesProjectList[ NbrFilesProjectList ] = malloc( strlen( FileName )+1 );
		if ( pFilesProjectList[ NbrFilesProjectList ] )
			strcpy( pFilesProjectList[ NbrFilesProjectList++ ], FileName );
		else
			printf("FilesProject: error to alloc string to the list...\n");
	}
	else
	{
		printf("FilesProject: error too much file to add to the list...\n");
	}
}
void FreeFilesProjectList( void )
{
	int ScanList;
	for( ScanList=0; ScanList<NbrFilesProjectList; ScanList++ )
	{
		free( pFilesProjectList[ ScanList ] );
		pFilesProjectList[ ScanList ] = NULL;
	}
}

// Split a project file in many parameters files
char SplitFiles( char * DirAndNameOfProject, char * TmpDirectoryFiles )
{
	char ProjectFileOk = TRUE;
	char Buff[ 300 ];
//	FILE * pProjectFile;
	gzFile pProjectFileZ;
	FILE * pParametersFile;
	char ParametersFile[300];
	int zerr;
	char ProjectFileIsComplete = FALSE;
	strcpy(ParametersFile,"");

printf("%s(): starting...\n", __FUNCTION__);
//	pProjectFile = fopen( DirAndNameOfProject, "rb" );
	pProjectFileZ = gzopen( DirAndNameOfProject, "rt" );
//	if ( pProjectFile )
	if ( pProjectFileZ )
	{

		/* start line of project ?*/
//		fgets( Buff, 300, pProjectFile );
		gzgets( pProjectFileZ, Buff, 300 );
		if ( strncmp( Buff, "_FILES_CLASSICLADDER", strlen( "_FILES_CLASSICLADDER" ) )==0 )
		{

//			while( !feof( pProjectFile ) )
			while( !gzeof( pProjectFileZ ) && ProjectFileOk && !ProjectFileIsComplete )
			{
//				fgets( Buff, 300, pProjectFile );
				gzgets( pProjectFileZ, Buff, 300 );
//////printf("%s(): gzgets lineS=%s\n", __FUNCTION__, Buff);
				/* seen end line of project ? */
				if ( strncmp( Buff, "_/FILES_CLASSICLADDER", strlen("_/FILES_CLASSICLADDER") )==0 )
				{
printf("%s(): project is complete.\n", __FUNCTION__);
					ProjectFileIsComplete = TRUE;
				}
//				if ( !feof( pProjectFile ) )
				if ( !gzeof( pProjectFileZ ) && ProjectFileOk && !ProjectFileIsComplete )
				{
					// header line for a file parameter ?
					if (strncmp(Buff,FILE_HEAD,STR_LEN_FILE_HEAD) ==0)
					{
////WIN32PORT added /
						sprintf(ParametersFile, "%s/%s", TmpDirectoryFiles, &Buff[STR_LEN_FILE_HEAD]);
						ParametersFile[ strlen( ParametersFile )-1 ] = '\0';
//WIN32PORT
if ( ParametersFile[ strlen(ParametersFile)-1 ]=='\r' )
ParametersFile[ strlen(ParametersFile)-1 ] = '\0';
					}
					else
					{
						/* seen end line of project ? */
//before						if ( strncmp( Buff, "_/FILES_CLASSICLADDER", strlen("_/FILES_CLASSICLADDER") )==0 )
//before...							ProjectFileIsComplete = TRUE;
						if ( !ProjectFileIsComplete && Buff[ 0 ]!='\n' )
						{
							char cEndOfFile = FALSE;
							/* file parameter */

							pParametersFile = fopen( ParametersFile, "wt" );
							if (pParametersFile)
							{
								fputs( Buff, pParametersFile );
//								while( !feof( pProjectFile ) && !cEndOfFile )
								while( /*!gzeof( pProjectFileZ ) &&*/ !cEndOfFile && ProjectFileOk )
								{
//									fgets( Buff, 300, pProjectFile );
									if ( gzgets( pProjectFileZ, Buff, 300 )==Z_NULL )
									{
										printf( "%s(): gzgets error: %s !!!\n", __FUNCTION__, gzerror(pProjectFileZ, &zerr) );
										ProjectFileOk = FALSE;
										printf("Abnormal project file not complete in %s() !!!\n",__FUNCTION__);
									}
									else
									{
//////printf("%s(): gzgets lineC=%s\n", __FUNCTION__, Buff);
										if (strncmp(Buff,"_/FILE-",strlen("_/FILE-")) !=0)
										{
//											if (!feof(pProjectFile))
//////										if (!gzeof(pProjectFileZ))
												fputs( Buff, pParametersFile );
										}
										else
										{
											cEndOfFile = TRUE;
//printf("%s(): end of this file %s\n", __FUNCTION__, ParametersFile);
										}
									}
								}
								fclose(pParametersFile);
							}
						}
					}
				}
			}
			if ( !ProjectFileIsComplete )
			{
				printf("Project file is not complete (missing end line) in %s() !!!\n",__FUNCTION__);
			}
		}
		else
		{
			ProjectFileOk = FALSE;
			printf("Failed to load project (bad first line) in %s() !!!\n",__FUNCTION__);
		}
//		fclose(pProjectFile);
		gzclose(pProjectFileZ);
	}
	else
	{
		ProjectFileOk = FALSE;
		printf("Failed to load project in %s() !!!\n",__FUNCTION__);
	}
	return ProjectFileOk && ProjectFileIsComplete;
}

// for embedded system, with filesystem read-only per default...
// functions tu use before/after writing file on it!
char RemountFileSystemRW( void )
{
#ifdef EMBEDDED_MOUNT_START_PARAMS_CMD
	printf("Remount fs 'rw'.\n");
	if ( mount( EMBEDDED_MOUNT_START_PARAMS_CMD )<0 )
	{
		printf("FAILED to remount fs 'rw' !!!\n");
		return FALSE;
	}
#endif
	return TRUE;
}
char RemountFileSystemRO( void )
{
#ifdef EMBEDDED_MOUNT_END_PARAMS_CMD
	printf("Remount fs 'ro'.\n");
	mount( EMBEDDED_MOUNT_END_PARAMS_CMD );
#endif
	return TRUE;
}

// finally function never used, so never tested...
#define BUFFCOPY 512
char CopyFile( const char * SrcFile, const char * DestFile )
{
	FILE * pFileNew, * pFileBase;
	char FilesError = FALSE;
	char Buff[ BUFFCOPY ];
	if ( NULL == ( pFileBase = fopen( SrcFile, "r" ) ) )
	{
		return 0;
	}
	if ( NULL == ( pFileNew = fopen( DestFile, "w" ) ) )
	{
		fclose( pFileBase );
		return 0;
	}
	
	while( !FilesError && !feof( pFileBase ) )
	{
		ssize_t l;
		l = fread( Buff, sizeof(char), BUFFCOPY, pFileBase );
		if ( ferror(pFileBase ) )
		{
			FilesError = TRUE;
		}
		else
		{
			fwrite( Buff, sizeof(char), l, pFileNew );
			if ( ferror( pFileNew ) )
				FilesError = TRUE;
		}
	}
	fclose( pFileBase );
	fclose( pFileNew );
	if ( FilesError )
	{
		unlink( DestFile );
		return 0;
	}
	return 1;
}
