#define DRAW_NORMAL 0
#define DRAW_FOR_TOOLBAR 1
#define DRAW_FOR_PRINT 2

void CreateVarNameForElement( char * pBuffToWrite, StrElement * pElem, char SymbolsVarsNamesIfAvail );
char * DisplayArithmExpr(char * Expr, char SymbolsVarsNamesIfAvail);
void CreateFontPangoLayout( cairo_t *cr, int BlockPxHeight, char DrawingOption );
int DrawPangoTextOptions( cairo_t * cr, int BaseX, int BaseY, int Width, int Height, char * Text, char CenterAlignment );
int DrawPangoText( cairo_t * cr, int BaseX, int BaseY, int Width, int Height, char * Text );
void DrawCommonElementForToolbar( cairo_t * cr,int x,int y,int Size,int NumElement );
void my_cairo_draw_line( cairo_t *cr, double x1, double y1, double x2, double y2 );
void my_cairo_draw_color_line( cairo_t *cr, char cColor, double x1, double y1, double x2, double y2 );
void my_cairo_draw_black_rectangle( cairo_t *cr, double x, double y, double w, double h );
char GetDrawDisplayWithColorState( char DrawingOption );
void DrawElement( cairo_t * cr,int x,int y,int Width,int Height,StrElement * pTheElement,char DrawingOption );
void DrawLeftRightBars( cairo_t * cr, int OffX, int PosiY, int BlockWidth, int BlockHeight, int HeaderLabelAndCommentHeight, int LeftRightBarsWidth, int IsTheCurrentRung );
void DrawRung( cairo_t * cr, int NumRungToDraw, char cRungInEdit, int OffX, int PosiY, int BlockWidth, int BlockHeight, int HeaderLabelAndCommentHeight, char DrawingOption );
void DrawRungs( cairo_t * cr );
void DrawCurrentSection( cairo_t * cr );

