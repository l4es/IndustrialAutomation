

int GetSizeVarsForTypeVar( int iTypeVarToSearch );
char * CreateVarName(int Type, int Offset, char SymbolNameIfAvail);
char TextParserForAVar( char * TextToParse, int * VarTypeFound, int * VarOffsetFound, int * pNumberOfChars, char PartialNames );
char TestVarIsReadWrite( int TypeVarTested, int OffsetVarTested );

