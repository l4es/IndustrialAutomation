/****************************************************************************
**
** Copyright (C) 1992-2000 Troll Tech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/
#include "table.h"

// minimum font size 
#define CELL_MIN_FS 4
// height at which we switch to one line
#define CELL_ONE_LINE_SIZE 8 
Table::Table( int numRows, int numCols, QWidget *parent, const char *name )
: QWidget( parent, name),
fTextDrawable(true),colSize(100),rowSize(10),nRows(numRows),
nCols(numCols)
{
	// Initialize headers
	resizeTable(numRows,numCols);
	// Enable clipper and set background mode
	// init variables
	curRow = curCol = 0;
	//
	// set up a two second timer to do the flashing
	//QTimer *pTimer = new QTimer(this);
	//connect(pTimer,SIGNAL(timeout()),this,SLOT(flash()));
	//pTimer->start(2000);
	//
}
Table::~Table()
{
}
void Table::updateAll()
{
	repaint(TRUE);
};
/****************************************************************************
Two drawing functions, 1 which finds out which cells to draw
and one which actually draws a cell.
Also one drawing function to darw empty areas.
*****************************************************************************/
void Table::paintEvent(QPaintEvent *e)
{
	QPainter p;
	p.begin(this);
	drawContents( &p, e->rect().left(), e->rect().top(),e->rect().width(),e->rect().height());
	p.end();
};
void Table::drawContents( QPainter *p, int cx, int cy, int cw, int ch )
{
	int colfirst = columnAt( cx );

	if(colfirst < 0) return; //APA added
	
	int collast = columnAt( cx + cw );
	int rowfirst = rowAt( cy );

	if(rowfirst < 0) return; //APA added

	int rowlast = rowAt( cy + ch );
	if ( rowlast == -1 )
	rowlast = rows() - 1;
	if ( collast == -1 )
	collast = cols() - 1;
	// go through the rows
	for ( int r = rowfirst; r <= rowlast; ++r ) 
	{
		// get row position and height
		int rowp = rowPos( r );
		int rowh = rowHeight( r );
		// go through the columns in the row r
		// if we know from where to where use this limits (colstart, colend), else go through all of them
		for ( int c = colfirst; c <= collast; ++c ) 
		{
			// get position and width of column c
			int colp, colw;
			colp = columnPos( c );
			colw = columnWidth( c );
			//  translate painter ad draw the cell
			p->saveWorldMatrix();
			p->translate( colp, rowp );
			paintCell( p, r, c, QRect( colp, rowp, colw, rowh ) );
			p->restoreWorldMatrix();
		}
	}
}
void Table::paintCell( QPainter* p, int row, int col, const QRect &cr )
{
	int w = cr.width();
	int h = cr.height();
	int x2 = w - 1;
	int y2 = h - 1;
	// draw cell background

	TableItem *pT = cellContent(row,col);
	if(pT && pT->GetFlash())	     
	{
		p->fillRect( 0, 0, w, h, Qt::cyan);
	}
	else
	{
		p->fillRect( 0, 0, w, h, cellBackColour(row, col ) );
	};
	// draw our lines
	QPen pen( p->pen() );
	p->setPen( gray );
	p->drawLine( x2, 0, x2, y2 );
	p->drawLine( 0, y2, x2, y2 );
	p->setPen( pen );
	if(fTextDrawable)
	{
		// supress drawing where the font is too small
		int x = 0;
		QPixmap pix( cellPixmap( row, col ) );
		if ( !pix.isNull() ) 
		{
			p->drawPixmap( 0, ( cr.height() - pix.height() ) / 2, pix );
			x = pix.width() + 2;
		}
		//
		if(pT)
		{
			if(pT->GetFlash())	     
			{
				p->setPen(Qt::black);
			}
			else
			{
				p->setPen(cellTextColour(row,col));
			};
			//
			// draw contents
			if(pT->text1().isEmpty() || (rowSize <  CELL_ONE_LINE_SIZE) ) // one or two lines - depends on space
			{
				p->drawText( x, 0, w - x, h,  AlignHCenter  | AlignVCenter, pT->text());
			}
			else
			{
				// second line is defined
				QString s = pT->text() + "\n" + pT->text1();
				p->drawText( x, 0, w - x, h,  AlignHCenter  | AlignVCenter, s);
			};
		};
	};
}
/****************************************************************************
Functions to access and set the cell contents
plus helper functions.
*****************************************************************************/
TableItem *Table::cellContent( int row, int col ) const
{
	return contents[ indexOf( row, col ) ];	// contents array lookup
}
void Table::setCellContent( int row, int col, TableItem *item )
{
	if ( contents[ indexOf( row, col ) ] )
	delete contents[ indexOf( row, col ) ];
	contents.insert( indexOf( row, col ), item ); // contents lookup and assign
	updateCell( row, col ); // repaint
}
int Table::indexOf( int row, int col ) const
{
	return ( row * cols() ) + col; // mapping from 2D table to 1D array
}
void Table::setCellText( int row, int col, const QString &text )
{
	TableItem *item = cellContent( row, col );
	if ( item ) {
		item->setText( text );
		updateCell( row, col );
		} else {
		TableItem *i = new TableItem( text, QPixmap() );
		setCellContent( row, col, i );
	}
}
void Table::setCellPixmap( int row, int col, const QPixmap &pix )
{
	TableItem *item = cellContent( row, col );
	if ( item ) {
		item->setPixmap( pix );
		updateCell( row, col );
		} else {
		TableItem *i = new TableItem( QString::null, pix );
		setCellContent( row, col, i );
	}
}
QString Table::cellText( int row, int col ) const
{
	TableItem *item = cellContent( row, col );
	if ( item )
	return item->text();
	return QString::null;
}
QPixmap Table::cellPixmap( int row, int col ) const
{
	TableItem *item = cellContent( row, col );
	if ( item )
	return item->pixmap();
	return QPixmap();
}
QColor Table::cellTextColour( int row, int col ) const
{
	TableItem *item = cellContent( row, col );
	if ( item )
	return item->textColour();
	return Qt::black;
}
QColor Table::cellBackColour( int row, int col ) const
{
	TableItem *item = cellContent( row, col );
	if ( item )
	return item->backColour();
	return Qt::white;
}
void Table::setCellTextColour(int row, int col, const QColor &c)
{
	TableItem *item = cellContent( row, col );
	if(item)
	{
		item->setTextColour(c); 
	};
};
void Table::setCellBackColour(int row, int col, const QColor &c)
{
	TableItem *item = cellContent( row, col );
	if(item)
	{
		item->setBackColour(c); 
	};
};
/****************************************************************************
Event handling to react on mouse, key and focus events.
*****************************************************************************/
void Table::mousePressEvent( QMouseEvent* e )
{
	// get rid of editor
	// remember old focus cell
	int oldRow = curRow;
	int oldCol = curCol;
	// get new focus cell
	curRow = rowAt( e->pos().y() );
	curCol = columnAt( e->pos().x() );
//	if ( curRow == -1 )//APA rem
//	curRow = oldRow;//APA rem
//	if ( curCol == -1 )//APA rem
//	curCol = oldCol;//APA rem
	if ( curRow < 0 ) //APA add
	curRow = oldRow;//APA add
	if ( curCol < 0 )//APA add
	curCol = oldCol;//APA add

	// of we have a new focus cell, repaint
	if ( curRow != oldRow || curCol != oldCol )
	{
		updateCell( oldRow, oldCol );
		updateCell( curRow, curCol );
		emit CellChanged(curRow,curCol,currentItem()); // say the cell selection has changed	 
		emit selectionChanged(); // general cell change
	}
	if(e->button() == Qt::RightButton)
	{
		emit rightClicked(currentItem(),e->pos());
	};
}
void Table::mouseMoveEvent( QMouseEvent *e )
{
	// do the same as in mouse press
	mousePressEvent( e ); 
}
void Table::resizeEvent( QResizeEvent *e )
{
	QWidget::resizeEvent( e );
	//
	//
	// make sure the cells fill the available space
	//
	// how big a font do we want now
	int h = (e->size().height()) / nRows; // two lines of text
	int w = (e->size().width()) / nCols;
	//
	int wf = w / 12; // assume we want 12 characters for a name (max)
	int fs = (h/2 > wf)?wf:h/2;
	//
	if(fs > 8)
	{
		fs -= 2;
		QFont f(font()); // set the font to the correct size - we hope
		#if QT_VERSION > 337
			f.setPixelSize(fs+120);
		#else
			f.setPixelSize(fs);
		#endif
		setFont(f);
		fTextDrawable = true;
	}
	else
	{
		fTextDrawable = false;
		fs = 0;
	};
	//
	rowSize = h;
	colSize = w;
}
/****************************************************************************
Functions to update cells, update geometries, etc.
*****************************************************************************/
void Table::updateCell( int row, int col )
{
	QRect r( cellGeometry( row, col ) );
	repaint( r );
}
/****************************************************************************
Following functions return the position/sizes of table cells. The
data is stored in the two headers anyway, so simply use it.
*****************************************************************************/
int Table::columnWidth( int col ) const
{
	return colSize;
}
int Table::rowHeight( int row ) const
{
	return rowSize;
}
int Table::columnPos( int col ) const
{
	return col * colSize;
}
int Table::rowPos( int row ) const
{
	return row * rowSize;
}
int Table::columnAt( int pos ) const
{
	int n = pos / colSize;
	return (n >= nCols)?(nCols - 1):n;
}
int Table::rowAt( int pos ) const
{
	int n = pos / rowSize;
	return (n >= nRows)?(nRows - 1):n;
}
QRect Table::cellGeometry( int row, int col ) const
{
	return QRect( columnPos( col ), rowPos( row ),
	columnWidth( col ), rowHeight( row ) );
}
QSize Table::tableSize() const
{
	return QSize( columnPos( cols() - 1 ) + columnWidth( cols() - 1 ),
	rowPos( rows() - 1 ) + rowHeight( rows() - 1 ) );
}
int Table::rows() const
{
	return nRows;
}
int Table::cols() const
{
	return nCols;
}
void Table::clear()
{
	for(int j = 0; j < nRows; j++)
	{
		for(int k = 0; k < nCols; k++)
		{
			setCellText(j,k,"");
		};
	};
};
void Table::resizeTable(int r, int c)
{
	curRow = curCol = 0;
	//
	nRows = (r > 0)?r:1;
	nCols = (c > 0)?c:1;
	//
	contents.resize( r * c);
	contents.fill(0);
	contents.setAutoDelete( TRUE );
	//
	for(int j = 0; j < r; j++)
	{
		for(int k = 0; k < c; k++)
		{
			setCellText(j,k,"");
		};
	};
	//
	int h = (size().height()) / nRows;
	int w = (size().width()) / nCols;
	//
	int wf = w / 12; // assume we want 12 characters for a name (max)
	int fs = (h/2 > wf)?wf:(h/2);
	//
	if(fs > CELL_MIN_FS)
	{
		QFont f(font()); // set the font to the correct size - we hope
		#if QT_VERSION > 337
			f.setPixelSize(fs-1+120);
		#else
			f.setPixelSize(fs-1);
		#endif
		setFont(f);
		fTextDrawable = true;
	}
	else
	{
		fTextDrawable = false;
		fs = 0;
	};
	//
	rowSize = h;
	colSize = w;
	//
};
void Table::flash()
{
	// flash cells with flash state set to 1
	for(int j = 0; j < rows(); j++)
	{
		for(int k = 0; k < cols(); k++)
		{
			TableItem *p = cellContent(j,k);
			if(p->GetFlash())	     
			{
				updateCell(j,k);
			};
		};
	};
	fFlash = !fFlash;
};

