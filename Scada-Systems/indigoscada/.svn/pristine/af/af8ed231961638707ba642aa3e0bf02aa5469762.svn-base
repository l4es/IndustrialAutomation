/*
* Copyright 1998, Michael van der Westhuizen.
* This package is under the LOLL (Less Obsessive Linux License).
* See LOLL for details.
*/
/*!
* \file dateobj.cpp
* \brief Date Item Widget Implementation.
*
* This file implements the dateObj class, as defined in the associated
* header file, dateobj.h.
*
* \internal
*/
#include "dateobj.h"
/*!
* The two special arguments the constructor takes are:
* <ul>
* <li>The date to display.
* <li>Whether or not the date is in the currently selected month (in the dateNav).
* </ul>
*
* \warning No error checking is done on the validity of the date passed through
* the constructor, so \b don't get it wrong.
* \warning You should never instantiate this object directly, but rather access the methods
* made available in dateNav.
* \internal
*/
dateObj::dateObj( class QWidget *parent, const char *name , QDate dateRepresented, int selectedMonth  )
: QWidget( parent, name )
{
	/*
	* Minimise flicker - set the background colour to the most common
	* colour in the widget, in this case white.
	*/
	setBackgroundColor( QColor( 255, 255, 255) );
	thisDate = &dateRepresented;
	inCurrent = false;
	paintSelected = false;
	/*
	* Check if we actually have a date to work with.
	*/
	if( !thisDate == 0 )
	{
		dateString.setNum( thisDate->day() );
		if( dateString.length() == 1 )
		{
			numTwo = dateString.toInt();
		}
		if( dateString.length() == 2 )
		{
			numOne = dateString.mid( 0, 1 ).toInt();
			numTwo = dateString.mid( 1, 1 ).toInt();
		}
		yr = thisDate->year();
		mo = thisDate->month();
		da = thisDate->day();
	}
	setSelectedMonth( selectedMonth );
	setFixedSize( 11, 8 );
	resize( 11, 8 );
}
/*!
* Called by the constructor to set values on how to paint this date object.
*
* \warning You should never call this function directly, but rather access the methods
* made available in dateNav.
* \internal
*/
void dateObj::setSelectedMonth( int monthNumber )
{
	sMonth = monthNumber;
	if( sMonth == thisDate->month() )
	{
		inCurrent = true;
	}
}
/*!
* Sets this date object to paint itself as selected.<br>
*
* \warning You should never call this function directly, but rather access the methods
* made available in dateNav.
* \internal
*/
void dateObj::setSelectedDay( bool tf )
{
	paintSelected = tf;
	repaint();
}
/*!
* Used by dateNav.
*
* \warning You should never call this function directly, but rather access the methods
* made available in dateNav.
* \return True if this date object paints itself as selected, false if not.
* \internal
*/
bool dateObj::selectedDay()
{
	return paintSelected;
}
/*!
* Reimplemented to provide the custom widget look and feel.
*
* \internal
*/
void dateObj::paintEvent( QPaintEvent * )
{
	/*
	* Paint the background white if we're not the selected day, otherwise
	* paint a grey background.
	*/
	setUpdatesEnabled( false );
	QPainter paint;
	paint.begin( this );
	if( !paintSelected )
	{
		paint.fillRect( 0, 0, 11, 8, QColor( 255, 255, 255 ) );
	}
	else
	{
		paint.fillRect( 0, 0, 11, 8, QColor( 192, 192, 192 ) );
	}
	paint.end();
	/*
	* Paint the numbers.
	*/
	if( !thisDate == 0 )
	{
		if( dateString.length() == 1 )
		{
			paintNumber( numTwo, true, inCurrent );
		}
		if( dateString.length() == 2 )
		{
			paintNumber( numOne, false, inCurrent );
			paintNumber( numTwo, true, inCurrent );
		}
	}
	setUpdatesEnabled( true );
}
/*!
* Used internally by the widget Paint Event.
*
* \internal
*/
void dateObj::paintNumber( int number, bool alignment, bool current )
{
	/*
	* Set the offset, which gives us the alignment of the number.
	*/
	if( number >= 4 && !alignment )
	{
		/* Die without another word... */
		return;
	}
	int offset = 0;
	if( alignment )
	{
		offset = 6;
	}
	QPainter paint;
	paint.begin( this );
	/*
	* Set the pen colour based on whether or not we are the current month.
	*/
	if( current )
	{
		if( !paintSelected )
		{
			paint.setPen( QColor( 0, 0, 0 ) );
		}
		else
		{
			paint.setPen( QColor( 255, 255, 255 ) );
		}
	}
	else
	{
		paint.setPen( QColor( 127, 127, 127 ) );
	}
	/*
	* Paint the number.
	*/
	switch( number )
	{
		case 0:
		{
			paint.drawLine( offset + 1, 0, offset + 3, 0 );
			paint.drawLine( offset + 0, 1, offset + 0, 6 );
			paint.drawLine( offset + 1, 7, offset + 3, 7 );
			paint.drawLine( offset + 4, 1, offset + 4, 6 );
			break;
		}
		case 1:
		{
			paint.drawLine( offset + 1, 1, offset + 1, 1 );
			paint.drawLine( offset + 2, 0, offset + 2, 6 );
			paint.drawLine( offset + 1, 7, offset + 3, 7 );
			break;
		}
		case 2:
		{
			paint.drawLine( offset + 0, 1, offset + 0, 1 );
			paint.drawLine( offset + 1, 0, offset + 3, 0 );
			paint.drawLine( offset + 4, 1, offset + 4, 2 );
			paint.drawLine( offset + 3, 3, offset + 3, 3 );
			paint.drawLine( offset + 2, 4, offset + 2, 4 );
			paint.drawLine( offset + 1, 5, offset + 1, 5 );
			paint.drawLine( offset + 0, 6, offset + 0, 6 );
			paint.drawLine( offset + 0, 7, offset + 4, 7 );
			break;
		}
		case 3:
		{
			paint.drawLine( offset + 0, 1, offset + 0, 1 );
			paint.drawLine( offset + 1, 0, offset + 3, 0 );
			paint.drawLine( offset + 4, 1, offset + 4, 2 );
			paint.drawLine( offset + 2, 3, offset + 3, 3 );
			paint.drawLine( offset + 4, 4, offset + 4, 6 );
			paint.drawLine( offset + 1, 7, offset + 3, 7 );
			paint.drawLine( offset + 0, 6, offset + 0, 6 );
			break;
		}
		case 4:
		{
			paint.drawLine( offset + 3, 0, offset + 3, 7 );
			paint.drawLine( offset + 2, 1, offset + 2, 1 );
			paint.drawLine( offset + 1, 2, offset + 1, 2 );
			paint.drawLine( offset + 0, 3, offset + 0, 3 );
			paint.drawLine( offset + 0, 4, offset + 4, 4 );
			break;
		}
		case 5:
		{
			paint.drawLine( offset + 0, 0, offset + 4, 0 );
			paint.drawLine( offset + 0, 1, offset + 0, 2 );
			paint.drawLine( offset + 0, 3, offset + 3, 3 );
			paint.drawLine( offset + 4, 4, offset + 4, 6 );
			paint.drawLine( offset + 1, 7, offset + 3, 7 );
			paint.drawLine( offset + 0, 6, offset + 0, 6 );
			break;
		}
		case 6:
		{
			paint.drawLine( offset + 2, 0, offset + 3, 0 );
			paint.drawLine( offset + 1, 1, offset + 1, 1 );
			paint.drawLine( offset + 0, 2, offset + 0, 6 );
			paint.drawLine( offset + 1, 7, offset + 3, 7 );
			paint.drawLine( offset + 4, 6, offset + 4, 4 );
			paint.drawLine( offset + 3, 3, offset + 1, 3 );
			break;
		}
		case 7:
		{
			paint.drawLine( offset + 0, 0, offset + 4, 0 );
			paint.drawLine( offset + 4, 1, offset + 4, 1 );
			paint.drawLine( offset + 3, 2, offset + 3, 3 );
			paint.drawLine( offset + 2, 4, offset + 2, 5 );
			paint.drawLine( offset + 1, 6, offset + 1, 7 );
			break;
		}
		case 8:
		{
			paint.drawLine( offset + 1, 0, offset + 3, 0 );
			paint.drawLine( offset + 0, 1, offset + 0, 2 );
			paint.drawLine( offset + 4, 1, offset + 4, 2 );
			paint.drawLine( offset + 1, 3, offset + 3, 3 );
			paint.drawLine( offset + 0, 4, offset + 0, 6 );
			paint.drawLine( offset + 4, 4, offset + 4, 6 );
			paint.drawLine( offset + 1, 7, offset + 3, 7 );
			break;
		}
		case 9:
		{
			paint.drawLine( offset + 1, 0, offset + 3, 0 );
			paint.drawLine( offset + 0, 1, offset + 0, 3 );
			paint.drawLine( offset + 1, 4, offset + 3, 4 );
			paint.drawLine( offset + 4, 1, offset + 4, 5 );
			paint.drawLine( offset + 3, 6, offset + 3, 6 );
			paint.drawLine( offset + 1, 7, offset + 2, 7 );
			break;
		}
	}
	/*
	* Close the paint device.
	*/
	paint.end();
}
/*!
* Reimplemented to allow us to emit a clicked signal for use in dateNav.
*
* \internal
*/
void dateObj::mousePressEvent( QMouseEvent * mpe )
{
	/*
	* Don't react to a right-click at the moment, just a normal click...
	*/
	if( mpe->button() == LeftButton )
	{
		emit clicked( *this, x(), y() );
	}
}
/*!
* Facilitates queries on the date represented by this object.<br>
* 
* \warning You should never call this function directly, but rather access the methods
* made available in dateNav.
* \return The date represented by this object.
* \internal
*/
QDate dateObj::date()
{
	return QDate( yr, mo, da );
}

