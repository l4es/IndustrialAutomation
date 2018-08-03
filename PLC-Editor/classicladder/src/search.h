void InitSearchDatas( void );

void GoToFirstRung( void );
void GoToLastRung( void );

char SearchTestIfLadderEleMatch( StrElement * pEle );
char SearchTestIfSeqTransiMatch( StrTransition * pTransi );
char SearchTestIfOnCurrent( int DisplayedRung, int DisplayedPosX, int DisplayedPoxY );
void SearchAndGoToNextElement( void );
void SearchAndGoToPreviousElement( void );
void SearchNewElement( int IndexTypeSelected, char * StringEntered );
