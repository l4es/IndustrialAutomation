void StartOrMotionPartSelection(double x,double y, char StartToClick);
void EndPartSelection( );
void GetSizesOfTheSelectionToCopy( int * pSizeX, int * pSizeY );
char GetIsOutputEleLastColumnSelection( );
void CopyRungPartSrcToDest( StrRung *RungSrc, StrRung *pRungDest, int PosiDestX, int PosiDestY, char JustRungCopyInBuffer );
void CleanUpSrcWhenStartMoving( );
void CopyNowPartSelected( double x,double y );


