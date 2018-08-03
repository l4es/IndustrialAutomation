#ifndef _EDIT_GTK_H
#define _EDIT_GTK_H
void SelectToolPointer( void );
void SelectToolEraser( void );
void SelectToolDrawHLine( void );
void SelectToolDrawHLineToEnd( void );
void SelectToolConnectVLine( void );
void SelectToolOpenContact( void );
void SelectToolClosedContact( void );
void SelectToolRisingEdgeContact( void );
void SelectToolFallingEdgeContact( void );
void SelectToolCoil( void );
void SelectToolCoilNot( void );
void SelectToolSetCoil( void );
void SelectToolResetCoil( void );
void SelectToolJumpCoil( void );
void SelectToolCallCoil( void );
void SelectToolIECTimer( void );
void SelectToolCounter( void );
void SelectToolOldTimer( void );
void SelectToolOldMonostable( void );
void SelectToolCompare( void );
void SelectToolOperate( void );
void SelectToolInvertElement( void );
void SelectToolSelectBlock( void );
void SelectToolCopyBlock( void );
void SelectToolMoveBlock( void );

void SelectToolSeqStep( void );
void SelectToolSeqInitStep( void );
void SelectToolSeqTransi( void );
void SelectToolSeqStepAndTransi( void );
void SelectToolSeqTransisOr1( void );
void SelectToolSeqTransisOr2( void );
void SelectToolSeqStepsAnd1( void );
void SelectToolSeqStepsAnd2( void );
void SelectToolSeqLink( void );
void SelectToolSeqComment( void );

void EditorButtonsAccordingSectionType( );
void ButtonAddRung();
void ButtonInsertRung();
void ButtonDeleteCurrentRung();
void ButtonModifyCurrentRung();
void ButtonOkCurrentRung();
void ButtonCancelCurrentRung();
void OpenEditWindow( GtkAction * ActionOpen, gboolean OpenIt );
void RememberEditWindowPrefs( void );
char ConvertNumElementInToolbarPosisXY( int NumToolbar, int NumElementWanted, int *FoundX, int *FoundY );
void SelectAnElementInToolBar( int iNumToolbar, int iNumElementSelectedWithPopup );
void EditorInitGtk();
#endif
