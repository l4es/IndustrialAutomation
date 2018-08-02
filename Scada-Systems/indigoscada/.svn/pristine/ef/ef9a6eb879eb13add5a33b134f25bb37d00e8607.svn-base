#ifndef TABLE_H
#define TABLE_H
#include "common.h"

class QHeader;
class QLineEdit;
class TableItem
{
	public:
	TableItem( const QString &t, const QPixmap p )
	: txtColour(Qt::black), bkColour(Qt::white),txt( t ),pix( p ),state(0),flash(0) {}
	QPixmap pixmap() const { return pix; }
	QString text() const { return txt; }
	QString text1() const { return txt1; }
	QColor textColour() const { return txtColour;};
	QColor backColour() const { return bkColour;};
	void setPixmap( const QPixmap &p ) { pix = p; }
	void setText( const QString &t ) { txt = t; }
	void setText1( const QString &t ) { txt1 = t; }
	void setTextColour(const QColor &c) {txtColour = c;};
	void setBackColour(const QColor &c) {bkColour = c;};
	int GetState() const { return state;}; // internal state - extra data
	void SetState(int i) { state = i;}; 
	//
	void SetFlash(bool f) { flash = f;}; // flash state
	int GetFlash() const { return flash;};
	//
	private:
	QColor  txtColour; // text colour
	QColor  bkColour; // background colour
	QString txt; // text
	QString txt1; // second line of text
	QPixmap pix; // pix map
	int state; // extra data
	bool flash; // flash state - do we flash the cell
};
class QSEXPORT Table : public QWidget
{
	Q_OBJECT
	public:
	Table( int numRows, int numCols, QWidget* parent=0, const char* name=0 );
	~Table();
	TableItem *cellContent( int row, int col ) const;
	TableItem *currentItem() const
	{
		return cellContent( curRow, curCol );
	};
	TableItem * getCell(unsigned n) // get item based on linear index
	{   
		if(n < contents.count())
		{
			return (TableItem *) contents[n]; 
		};
		return 0;
	};
	unsigned count() const  // count of items in vector
	{
		return contents.count();
	};
	void setCellContent( int row, int col, TableItem *item );
	void setCellText( int row, int col, const QString &text );
	void setCellPixmap( int row, int col, const QPixmap &pix );
	void setCellTextColour(int row, int col, const QColor &c);
	void setCellBackColour(int row, int col, const QColor &c);
	QString cellText( int row, int col ) const;
	QPixmap cellPixmap( int row, int col ) const;
	QColor cellTextColour(int row, int col ) const;
	QColor cellBackColour(int row, int col ) const;
	QRect cellGeometry( int row, int col ) const;
	int columnWidth( int col ) const;
	int rowHeight( int row ) const;
	int columnPos( int col ) const;
	int rowPos( int row ) const;
	int columnAt( int pos ) const;
	int rowAt( int pos ) const;
	QSize tableSize() const;
	int rows() const;
	int cols() const;
	void updateCell( int row, int col );
	void clear();
	void resizeTable(int row, int col);
	void updateAll();
	protected:
	void drawContents( QPainter *p, int cx, int cy, int cw, int ch );
	void mousePressEvent( QMouseEvent* );
	void mouseMoveEvent( QMouseEvent* );
	void resizeEvent( QResizeEvent * );
	void paintEvent(QPaintEvent *e);
	private slots:
	void flash();
	signals:
	void CellChanged(int,int,TableItem *);
	void rightClicked(TableItem *,const QPoint &);
	void selectionChanged();
	private:
	void paintCell( QPainter *p, int row, int col, const QRect &cr );
	int indexOf( int row, int col ) const;
	void updateGeometries();
	private:
	QVector<TableItem> contents;
	int curRow;
	int curCol;
	bool fTextDrawable; // true if the font is valid
	bool fFlash; // flash on / off flag
	int colSize; // size of col 
	int rowSize; // size of row
	int nRows;
	int nCols;
};
#endif // TABLE_H

