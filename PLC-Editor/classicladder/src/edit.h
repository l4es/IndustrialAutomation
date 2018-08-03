#define MODE_MODIFY 0
#define MODE_ADD 1
#define MODE_INSERT 2

char * GetElementPropertiesForStatusBar(StrElement * Element);
int TextToNumber(char * text,int ValMin,int ValMaxi,int *ValFound);
void SaveElementProperties(void);
char CheckForAllocatingArithmExpr(int NumTypeEle, int PosiX,int PosiY);
void SetUsedStateFunctionBlock( int Type, int Num, char Val );
int GetFreeNumberFunctionBlock( int Type );
void InitBufferRungEdited( StrRung * pRung );
int GetNbrRungsDefined(void);
int GetNbrRungsDefinedForASection( StrSection * pSection );
int FindFreeRung(void);
char AddRung(void);
char InsertRung(void);
void ModifyCurrentRung(void);
void DeleteCurrentRung(void);
void CancelRungEdited(void);
void ApplyRungEdited(void);
char GetSizesOfAnElement(short int NumTypeEle,int * pSizeX, int * pSizeY);
char ConvertDoublesToRungCoor( double coorx, double coory, int * pRungX, int * pRungY );
char PrepBeforeSettingTypeEleForComplexBlocsAndExpr( int NumTypeEle, int PosiX, int PosiY );
void FullDeleteElement( StrRung * pRung, int RungX, int RungY );
void EditElementInRung(double x,double y);
void EditElementInThePage(double x,double y);
void MouseMotionOnThePage( double x, double y );
void EditButtonReleaseEventOnThePage( void );
char * GetLadderElePropertiesForStatusBar(double x,double y);

