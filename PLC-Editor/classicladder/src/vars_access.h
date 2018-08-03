void InitVars( char DoLogEvents );
void InitSetsVars( void );
int GetNbrVarsForType( int TypeVar );
int ReadVar(int TypeVar,int Offset);
void WriteVar(int TypeVar,int NumVar,int Value);
void WriteBoolVarWithoutLog( int NumVar,int Value );

void SetVar( int TypeVar, int NumVar, int SetValue );
void UnsetVar( int TypeVar, int NumVar );
char IsVarSet( int TypeVar, int NumVar );

/* these are only useful for the MAT-connected version */
void DoneVars(void);
void CycleStart(void);
void CycleEnd(void);
