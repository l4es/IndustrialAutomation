
void VerifyDirectorySelected( char * TheFileProject, char * NewDir );
void InitTempDir( void );
char LoadProjectFiles( char * FileProject );
char LoadGeneralParamsOnlyFromProject( char * FileProject );
char SaveProjectFiles( char * FileProject );
char JoinFiles( char * DirAndNameOfProject, char * TmpDirectoryFiles, char CompressedProject );
void AddFileToFilesProjectList( char * FileName );
void FreeFilesProjectList( void );
char SplitFiles( char * DirAndNameOfProject, char * TmpDirectoryFiles );

char RemountFileSystemRW( void );
char RemountFileSystemRO( void );

char CopyFile( const char * SrcFile, const char * DestFile );
