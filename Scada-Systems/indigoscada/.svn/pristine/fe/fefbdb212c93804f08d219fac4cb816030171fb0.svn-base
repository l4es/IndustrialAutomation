/*
* Copyright 1998, Michael van der Westhuizen.
* This package is under the LOLL (Less Obsessive Linux License).
* See LOLL for details.
*/
/*!
* \file datepopup.cpp
* \brief Month Selection Popup Menu Implementation.
*
* This file implements the datePopup class, as defined in the associated
* header file, datepopup.h.
*
* \internal
*/
#include "datepopup.h"
/*!
* This widget only has one constructor.
*
* The \c cMonth argument is the month currently being viewed in the dateNav.
*
* \warning You should never instantiate this object directly, but rather access the methods
* made available in dateNav.
* \internal
*/
datePopup::datePopup( class QWidget *parent, const char *name , QDate cMonth )
: QPopupMenu( parent, name )
{
	/*
	* It's a bit of a mission to write every thing ourselves, so we just use
	* a fairly common font for the menu items.
	*/
	setFont( QFont( "Helvetica", 10, QFont::Normal) );
	/*
	* Width of 10 is about right - pity we can't center the text ;-(
	*/
	setFixedWidth( 101 );
	/*
	* Set the clearing colour and the palette.
	*/
	setBackgroundColor( QColor( 255, 255, 255 ) );
	QColorGroup tmpColorGroup = QColorGroup( QColor( 255, 255, 255 ), QColor( 255, 255, 255 ), QColor( 0, 0, 0 ), QColor( 0, 0, 0 ), QColor( 0, 0, 0 ), QColor( 0, 0, 0 ), QColor( 255, 255, 255 ) );
	setPalette( QPalette( tmpColorGroup, tmpColorGroup, tmpColorGroup ) );
	/*
	* Get all the lines to look as we want them.
	*/
	setFrameStyle( Box | Raised );
	setLineWidth( 1 );
	/*
	* Connect our activated (clicked) signal to a slot that's eqipped to deal
	* with it...
	*/
	connect( this, SIGNAL( activated( int ) ), this, SLOT( sltItemClicked( int ) ) );
	/*
	* Set up the contents.
	*/
	setMiddleMonth( cMonth );
}
/*!
* \warning Don't <b>ever</b> forget to call this function before calling
* the popup method. If you do you'll either get stale data, or a crash!
*
* \warning You should never call this function directly, but rather access the methods
* made available in dateNav.
* \internal
*/
void datePopup::setMiddleMonth( QDate mMonth )
{
	clear();
	middleMonth = mMonth;
	makeDates( mMonth );
	idOne = insertItem( dateText( backThree ) );
	idTwo = insertItem( dateText( backTwo ) );
	idThree = insertItem( dateText( backOne ) );
	idFour = insertItem( dateText( middleMonth ) );
	idFive = insertItem( dateText( forwardOne ) );
	idSix = insertItem( dateText( forwardTwo ) );
	idSeven = insertItem( dateText( forwardThree ) );
	setActiveItem( idFour );
}
/*!
* Adjusts the three date objects that represtent the previous three months, 
* and the three date objects that represent the next three months to reflect
* accurate values.
*
* \warning You should never call this function directly, but rather access the methods
* made available in dateNav.
* \internal
*/
void datePopup::makeDates( QDate mMonth )
{
	QDate tmpDate = mMonth;
	/* Back three Months... */
	int newY = tmpDate.year();
	int newM = tmpDate.month();
	int newD = tmpDate.day();
	if( newM == 3 )
	{
		newM = 12;
		newY--;
	}
	else if( newM == 2 )
	{
		newM = 11; 
		newY--;
	}
	else if( newM == 1 )
	{
		newM = 10; 
		newY--;
	}
	else
	{
		newM = newM - 3;
	}
	newD = fixDays( newY, newM, newD );
	backThree = QDate( newY, newM, newD );
	/* End of back three months */
	/* Back two months... */
	newY = tmpDate.year();
	newM = tmpDate.month();
	newD = tmpDate.day();
	if( newM == 2 )
	{
		newM = 12; 
		newY--;
	}
	else if( newM == 1 )
	{
		newM = 11;
		newY--;
	}
	else
	{
		newM = newM - 2;
	}
	newD = fixDays( newY, newM, newD );
	backTwo = QDate( newY, newM, newD );
	/* End of back two months */
	/* Back one month... */
	newY = tmpDate.year();
	newM = tmpDate.month();
	newD = tmpDate.day();
	if( newM == 1 )
	{
		newM = 12; 
		newY--;
	}
	else
	{
		newM = newM - 1;
	}
	newD = fixDays( newY, newM, newD );
	backOne = QDate( newY, newM, newD );
	/* End of back one month */
	/* Forward one month... */
	newY = tmpDate.year();
	newM = tmpDate.month();
	newD = tmpDate.day();
	if( newM == 12 )
	{
		newM = 1; 
		newY++;
	}
	else
	{
		newM = newM + 1;
	}
	newD = fixDays( newY, newM, newD );
	forwardOne = QDate( newY, newM, newD );
	/* End of forward one month */
	/* Forward two months... */
	newY = tmpDate.year();
	newM = tmpDate.month();
	newD = tmpDate.day();
	if( newM == 11 )
	{
		newM = 1;
		newY++;
	}
	else if( newM == 12 )
	{
		newM = 2;
		newY++;
	}
	else
	{
		newM = newM + 2;
	}
	newD = fixDays( newY, newM, newD );
	forwardTwo = QDate( newY, newM, newD );
	/* End of forward two months */
	/* Forward three months... */
	newY = tmpDate.year();
	newM = tmpDate.month();
	newD = tmpDate.day();
	if( newM == 10 )
	{
		newM = 1;
		newY++;
	}
	else if( newM == 11 )
	{
		newM = 2;
		newY++;
	}
	else if( newM == 12 )
	{
		newM = 3;
		newY++;
	}
	else
	{
		newM = newM + 3;
	}
	newD = fixDays( newY, newM, newD );
	forwardThree = QDate( newY, newM, newD );
	/* End of forward three months */
}
/*!
* Ensures that the day we're wanting to represent is valid, by adjusting
* day integers that are past the end of their related months to the end
* of their related month.
*
* \warning You should never call this function directly, but rather access the methods
* made available in dateNav.
* \internal
* \bug For some reason February occasionaly gets the 23<sup>rd</sup> or 24<sup>th</sup>, not the
* 28<sup>th</sup>. It's not the end of the world, but I'd like to find the problem.
*/
int datePopup::fixDays( int tmpYear, int tmpMonth, int d )
{
	int tmpDay = d;
	/*
	* Ensure we're not trying to go to the 30th of a month that
	* has less than 30 days... (etc...)
	*/
	switch( tmpMonth )
	{
		/* February we'll be leap year safe... */
		case 2:
		{
			if( tmpDay >= 29 )
			{
				/* If it's not a leap year... */
				if( QDate::leapYear( tmpYear ) )
				{
					tmpDay = 29;
				}
				else
				/* it is a leap year */
				{
					tmpDay = 28;
				}
			}
			break;
		}
		/* April */
		case 4:
		{
			if( tmpDay == 31 )
			{
				tmpDay = 30;
			}
			break;
		}
		/* June */
		case 6:
		{
			if( tmpDay == 31 )
			{
				tmpDay = 30;
			}
			break;
		}
		/* September */
		case 9:
		{
			if( tmpDay == 31 )
			{
				tmpDay = 30;
			}
			break;
		}
		/* November */
		case 11:
		{
			if( tmpDay == 31 )
			{
				tmpDay = 30;
			}
			break;
		}
	} /* End of switch */
	return tmpDay;
}
/*!
* Finds a suitable Month/Year text string for the date passed as the argument.
*
* \warning You should never call this function directly, but rather access the methods
* made available in dateNav.
* \return A month/year string suited to the date passed as the argument.
* \internal
*/
QString datePopup::dateText( QDate cDate )
{
	QString strMonth = 0;
	switch( cDate.month() )
	{
		case 1:
		{
			strMonth = "January ";
			break;
		}
		case 2:
		{
			strMonth = "February ";
			break;
		}
		case 3:
		{
			strMonth = "March ";
			break;
		}
		case 4:
		{
			strMonth = "April ";
			break;
		}
		case 5:
		{
			strMonth = "May ";
			break;
		}
		case 6:
		{
			strMonth = "June ";
			break;
		}
		case 7:
		{
			strMonth = "July ";
			break;
		}
		case 8:
		{
			strMonth = "August ";
			break;
		}
		case 9:
		{
			strMonth = "September ";
			break;
		}
		case 10:
		{
			strMonth = "October ";
			break;
		}
		case 11:
		{
			strMonth = "November ";
			break;
		}
		case 12:
		{
			strMonth = "December ";
			break;
		}
	}
	QString strYear = 0;
	strYear.setNum( cDate.year() );
	return strMonth + strYear;
}
/*!
* This slot finds out which menu item was clicked, and emits a signal
* (newMonthSelected( QDate )) that carries the selected date to the
* dateNav widget.
*
* \warning You should never call this function directly, but rather access the methods
* made available in dateNav.
* \internal
*/
void datePopup::sltItemClicked( int itemId )
{
	if( itemId == idOne )
	{
		emit newMonthSelected( backThree );
	}
	if( itemId == idTwo )
	{
		emit newMonthSelected( backTwo );
	}
	if( itemId == idThree )
	{
		emit newMonthSelected( backOne );
	}
	if( itemId == idFour )
	{
		/*
		* Do nothing - why should we change when nothing's changed?
		*/
	}
	if( itemId == idFive )
	{
		emit newMonthSelected( forwardOne );
	}
	if( itemId == idSix )
	{
		emit newMonthSelected( forwardTwo );
	}
	if( itemId == idSeven )
	{
		emit newMonthSelected( forwardThree );
	}
	return;
}

