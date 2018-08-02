/*
* Copyright 1998, Michael van der Westhuizen.
* This package is under the LOLL (Less Obsessive Linux License).
* See LOLL for details.
*/
/*!
* \file datenav.cpp
* \brief Date Navigator Widget Implementation.
*
* This file implements the dateNav class, as described in the associated
* header file, datenav.h.
*
* See the class documentation for details on usage.
*
* \sa dateNav
*/
#include "datenav.h"
/*!
* The default constructor creates a date navigator based on todays date.
*
* This is the standard way to use this widget, and provides a widget with
* an initial view on this month, with todays date selected.
*
* An example of usage would be:
* \code
* // MyHeader.h
* ...
* private:
* 	dateNav *myDateNav;
* ...
* // end MyHeader.h
* // MyFile.cpp
* ...
* 	myDateNav = new dateNav( this, "myDateNav" );
* ...
* // end MyFile.cpp
* \endcode
*
* This is the most common constructor, and the simplest to use. Don't forget
* to connect to the dateChanged() signal to actually make the widget useful!
*/
dateNav::dateNav( class QWidget * parent, const char * name )
: QWidget( parent, name )
{
	setFixedSize( 145, 127 );
	resize( 145, 127 );
	isThisMonth = false;
	/*
	* This constructor creates a date navigator based on todays date.
	*/
	currentMonth = QDate( QDate::currentDate() );
	setupDates();
	sevenMonthPopup = new datePopup( 0, "sevenMonthPopup", currentMonth );
	connect( sevenMonthPopup, SIGNAL( newMonthSelected( QDate ) ), this, SLOT( monthSelected( QDate ) ) );
}
/*!
* This constructor creates a date navigator based on the date provided.
*
* This is the alternative way to constuct the widget, and provides a widget
* with an initial view on the month provided in the supplied QDate, with
* the day provided by the supplied QDate selected. If the supplied date is
* not valid, the date to be displayed is set to today.
*
* An example of usage would be:
* \code
* // MyHeader.h
* ...
* private:
* 	dateNav *myDateNav;
* ...
* // end MyHeader.h
* // MyFile.cpp
* ...
* 	QDate * bob = new QDate( 1975, 1, 6 ); // My Birthday ;-)
* 	myDateNav = new dateNav( bob, this, "myDateNav" );
* ...
* // end MyFile.cpp
* \endcode
*
* Don't forget to connect to the dateChanged() signal to actually make the
* widget useful!
*
* \overload
*/
dateNav::dateNav( const QDate * pd , class QWidget * parent, const char * name )
: QWidget( parent, name )
{
	setFixedSize( 145, 127 );
	resize( 145, 127 );
	isThisMonth = false;
	/*
	* This constructor creates a date navigator based on the date passed
	* through the constructor.
	*/
	currentMonth = QDate( pd->year(), pd->month(), pd->day() );
	/*
	* We must check if the date passed is valid...
	*/
	if( currentMonth.isValid() )
	{
		setupDates();
		sevenMonthPopup = new datePopup( 0, "sevenMonthPopup", currentMonth );
		connect( sevenMonthPopup, SIGNAL( newMonthSelected( QDate ) ), this, SLOT( monthSelected( QDate ) ) );
	}
	/*
	* If it's not, we set it to today...
	*/
	else
	{
		currentMonth = QDate( QDate::currentDate() );
		setupDates();
		sevenMonthPopup = new datePopup( 0, "sevenMonthPopup", currentMonth );
		connect( sevenMonthPopup, SIGNAL( newMonthSelected( QDate ) ), this, SLOT( monthSelected( QDate ) ) );
	}
}
/*!
* Use this function sets the date displayed.
*
* \return True if the new date is valid, false if not (and therefore no
* changes are made).
* \sa selectedDate()
* \sa dateChanged()
*/
bool dateNav::setDate( QDate nDate )
{
	if( nDate.isValid() )
	{
		currentMonth = nDate;
		setupDates();
		return true;
	}
	return false;
}
/*!
* This member function calculates which days should be shown, and creates
* a dateObj corresponding to each date. As each dateObj is created, it is
* also placed on this widget, and various tests are performed as how
* best to display it (selected or not, todays date or not).
*
* \internal
*/
void dateNav::setupDates()
{
	isChangingMonth = true;
	setUpdatesEnabled( false );
	lstDateObjects.setAutoDelete( true );
	lstDateObjects.clear();
	todaysDate = QDate( QDate::currentDate() );
	/*
	* Is the diplayed month the current month?
	* (don't forget, it's got to be in this year too - I did!)
	*/
	if( currentMonth.month() == todaysDate.month() && currentMonth.year() == todaysDate.year() )
	{
		isThisMonth = true;
	}
	else
	{
		isThisMonth = false;
	}
	/*
	* We need to find out what day the first of this month was.
	*/
	QDate *fdom = new QDate( currentMonth.year(), currentMonth.month(), 1 );
	/*
	* That value is assigned to the start position of our matrix.
	* The problem here is that Qt returns monday as 1, not 0, and Sunday
	* as 7! We count our matrix from 0, so we just turn a 7 into 0
	*/
	int matrixStartPos = fdom->dayOfWeek();
	if( matrixStartPos == 7 )
	{
		matrixStartPos = 0;
	}
	/*
	* If we are starting the month before thursday, we give the previous month
	* a little more space.
	*/
	if( matrixStartPos < 4 )
	{
		matrixStartPos = matrixStartPos + 7;
	}
	/*
	* We create a last month, and a next month, both relative to current
	* month.
	*/
	if( !( currentMonth.month() == 1 ) )
	{
		previousMonth = new QDate( currentMonth.year(), currentMonth.month() - 1, 1 );
	}
	else
	{
		previousMonth = new QDate( currentMonth.year() - 1, 12, 1 );
	}
	if( !( currentMonth.month() == 12 ) )
	{
		nextMonth = new QDate( currentMonth.year(), currentMonth.month() + 1, 1 );
	}
	else
	{
		nextMonth = new QDate( currentMonth.year() + 1, 1, 1 );
	}
	/*
	* All the yukky little temporaries used for layout and keeping track of
	* what exactly we've done.
	*/
	int leftBase = 16;
	int topBase = 40;
	int lineCounter = 1;
	int dayCounter = 1;
	int nextMonthDayCounter = 1;
	dateObj *insertedDateObject;
	/*
	* Insert the actual date objects, and lay them out in our date matrix.
	*
	* The grid is 7 days x 6 weeks ( == 42 blocks )
	* First we add last months trailing dates, when those are done, we add
	* this months dates, then the forward dates for next month.
	* With each date added, we use the temporaries above to track the layout
	* management, line numbers and decision conditions.
	*/
	for( int i = 0; i <= 41; i++ )
	{
		if( !matrixStartPos == 0 )
		{
			lstDateObjects.append( new dateObj( this, "aDateObj", QDate::QDate( previousMonth->year(), previousMonth->month(), previousMonth->daysInMonth() - matrixStartPos + 1 ), currentMonth.month() ) );
			matrixStartPos--;
		}
		else
		{
			if( !( dayCounter == ( currentMonth.daysInMonth() + 1 ) ) )
			{
				lstDateObjects.append( new dateObj( this, "aDateObj", QDate::QDate( currentMonth.year(), currentMonth.month(), dayCounter ), currentMonth.month() ) );
				dayCounter++;
			}
			else
			{
				lstDateObjects.append( new dateObj( this, "aDateObj", QDate::QDate( nextMonth->year(), nextMonth->month(), nextMonthDayCounter ), currentMonth.month() ) );
				nextMonthDayCounter++;
			}
		}
		/*
		* The object is now added, connect it, then we lay it out...
		*/
		insertedDateObject = lstDateObjects.last();
		connect( insertedDateObject, SIGNAL( clicked( dateObj &, int, int ) ), this, SLOT( sltDateClicked( dateObj &, int, int ) ) );
		insertedDateObject->move( leftBase, topBase );
		insertedDateObject->show();
		/*
		* If the most recently added dateObj represents today, we'll have to
		* know where it so we can draw the today indicator (a red box) around
		* it.
		*/
		if( isThisMonth )
		{
			if( insertedDateObject->date() == todaysDate )
			{
				tBoxX = leftBase;
				tBoxY = topBase;
			}
		}
		/*
		* If the most recently added dateObj represents the date that was
		* passed to us, it needs to be selected.
		*/
		if( insertedDateObject->date() == currentMonth )
		{
			/*
			* We store modified X and Y coords of the newly selected box, so
			* that we can erase any selection cursor later.
			*/
			sBoxX = insertedDateObject->x() - 3;
			sBoxY = insertedDateObject->y() - 3;
			insertedDateObject->setSelectedDay( true );
		}
		/*
		* All the ugly positional stuff...
		* This keeps track of where the next item should go...
		* We go from left to right, returning to our left position (new line)
		* after even items have been layed out.
		*/
		/*
		* We're not as far right as we can go, so we add to the left base...
		*/
		if( !( leftBase == 118 ) )
		{
			leftBase = leftBase + 17;
		}
		/*
		* We can't go further right...
		*/
		else
		{
			/*
			* Reset left base to 16
			*/
			leftBase = 16;
			/*
			* Based on the line we're on, we update the top base and increment
			* the line counter. Note that we do this in reverse so we don't
			* stumble across an untrue truth (!).
			* For efficiency this could be turned into a switch, in which
			* order doesn't really matter.
			*/
			if( lineCounter == 6 )
			{
				topBase = 130;
				lineCounter++;
			}
			if( lineCounter == 5 )
			{
				topBase = 115;
				lineCounter++;
			}
			if( lineCounter == 4 )
			{
				topBase = 100;
				lineCounter++;
			}
			if( lineCounter == 3 )
			{
				topBase = 85;
				lineCounter++;
			}
			if( lineCounter == 2 )
			{
				topBase = 70;
				lineCounter++;
			}
			if( lineCounter == 1 )
			{
				topBase = 55;
				lineCounter++;
			}
		}/* End of geometry management */
	}/* End of for loop */
	setUpdatesEnabled( true );
	isChangingMonth = false;
	repaint(true);
}
/*!
* Calculates width of the month/year caption, and then
* calls drawCaption( strCaption ) to paint it.
*
* \internal
* \sa drawCaption( QString )
*/
void dateNav::paintMonth()
{
	//16-06-2003 TODO:correct the path fo language support
	QString strMonth = 0;
	switch( currentMonth.month() )
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
	strYear.setNum( currentMonth.year() );
	QString strCaption = strMonth + strYear;
	drawCaption( strCaption );
}
/*!
* Paints the month/year caption.
* 
* This code is a little messy at the moment, but will be cleaned up soon
* enough.
*
* \internal
*/
void dateNav::drawCaption( QString ca )
{
	setUpdatesEnabled( false );
	/*
	* Initialise the local variable to calculate the length of the string.
	*/
	int lenthOfca = 0;
	/*
	* Calculate the width of the resultant paint area.
	*/
	for( unsigned int ii = 0 ; ii <= ca.length() ; ii++ )
	{
		lenthOfca += widthOfLetter( ca.mid( ii, 1 ) );
	}
	lenthOfca--;
	int leftBase = ( width() - lenthOfca ) / 2;
	int topBase = 6;
	QPainter paint;
	paint.begin( this );
	paint.setPen( QColor( 0, 0, 0 ) );
	for( unsigned int i = 0 ; i <= ca.length() ; i++ )
	{
		if( ca.mid( i, 1 ) == "A" )
		{
			paint.drawLine( leftBase + 0, 11, leftBase + 0, 13 );
			paint.drawLine( leftBase + 1, 8, leftBase + 1, 10 );
			paint.drawLine( leftBase + 2, 6, leftBase + 2, 7 );
			paint.drawLine( leftBase + 3, 6, leftBase + 3, 7 );
			paint.drawLine( leftBase + 4, 8, leftBase + 4, 10 );
			paint.drawLine( leftBase + 5, 11, leftBase + 5, 13 );
			paint.drawLine( leftBase + 1, 11, leftBase + 4, 11 );
			/* Adjust the left base */
			leftBase += widthOfLetter( "A" );
		}
		if( ca.mid( i, 1 ) == "D" )
		{
			paint.drawLine( leftBase + 0, 6, leftBase + 0, 13 );
			paint.drawLine( leftBase + 1, 6, leftBase + 3, 6 );
			paint.drawLine( leftBase + 1, 13, leftBase + 3, 13 );
			paint.drawLine( leftBase + 4, 7, leftBase + 4, 7 );
			paint.drawLine( leftBase + 4, 12, leftBase + 4, 12 );
			paint.drawLine( leftBase + 5, 8, leftBase + 5, 11 );
			/* Adjust the left base */
			leftBase += widthOfLetter( "D" );
		}
		if( ca.mid( i, 1 ) == "F" )
		{
			paint.drawLine( leftBase + 0, 6, leftBase + 0, 13 );
			paint.drawLine( leftBase + 1, 6, leftBase + 4, 6 );
			paint.drawLine( leftBase + 1, 9, leftBase + 4, 9 );
			/* Adjust the left base */
			leftBase += widthOfLetter( "F" );
		}
		if( ca.mid( i, 1 ) == "J" )
		{
			paint.drawLine( leftBase + 1, 6, leftBase + 2, 6 );
			paint.drawLine( leftBase + 3, 6, leftBase + 3, 13 );
			paint.drawLine( leftBase + 0, 13, leftBase + 2, 13 );
			/* Adjust the left base */
			leftBase += widthOfLetter( "J" );
		}
		if( ca.mid( i, 1 ) == "G" ) //APA added copied from "J"
		{
			paint.drawLine( leftBase + 1, 6, leftBase + 2, 6 );
			paint.drawLine( leftBase + 3, 6, leftBase + 3, 13 );
			paint.drawLine( leftBase + 0, 13, leftBase + 2, 13 );
			/* Adjust the left base */
			leftBase += widthOfLetter( "G" );
		}
		if( ca.mid( i, 1 ) == "M" )
		{
			paint.drawLine( leftBase + 0, 6, leftBase + 0, 13 );
			paint.drawLine( leftBase + 1, 6, leftBase + 1, 7 );
			paint.drawLine( leftBase + 2, 8, leftBase + 2, 9 );
			paint.drawLine( leftBase + 3, 10, leftBase + 3, 11 );
			paint.drawLine( leftBase + 4, 8, leftBase + 4, 9 );
			paint.drawLine( leftBase + 5, 6, leftBase + 5, 7 );
			paint.drawLine( leftBase + 6, 6, leftBase + 6, 13 );
			/* Adjust the left base */
			leftBase += widthOfLetter( "M" );
		}
		if( ca.mid( i, 1 ) == "N" )
		{
			paint.drawLine( leftBase + 0, 6, leftBase + 0, 13 );
			paint.drawLine( leftBase + 1, 6, leftBase + 1, 7 );
			paint.drawLine( leftBase + 2, 8, leftBase + 2, 9 );
			paint.drawLine( leftBase + 3, 10, leftBase + 3, 11 );
			paint.drawLine( leftBase + 4, 12, leftBase + 4, 13 );
			paint.drawLine( leftBase + 5, 6, leftBase + 5, 13 );
			/* Adjust the left base */
			leftBase += widthOfLetter( "N" );
		}
		if( ca.mid( i, 1 ) == "O" )
		{
			paint.drawLine( leftBase + 0, 8, leftBase + 0, 11 );
			paint.drawLine( leftBase + 1, 7, leftBase + 1, 7 );
			paint.drawLine( leftBase + 1, 12, leftBase + 1, 12 );
			paint.drawLine( leftBase + 2, 6, leftBase + 4, 6 );
			paint.drawLine( leftBase + 2, 13, leftBase + 4, 13 );
			paint.drawLine( leftBase + 5, 12, leftBase + 5, 12 );
			paint.drawLine( leftBase + 5, 7, leftBase + 5, 7 );
			paint.drawLine( leftBase + 6, 8, leftBase + 6, 11 );
			/* Adjust the left base */
			leftBase += widthOfLetter( "O" );
		}
		if( ca.mid( i, 1 ) == "S" )
		{
			paint.drawLine( leftBase + 1, 6, leftBase + 4, 6 );
			paint.drawLine( leftBase + 0, 7, leftBase + 0, 8 );
			paint.drawLine( leftBase + 1, 9, leftBase + 3, 9 );
			paint.drawLine( leftBase + 4, 10, leftBase + 4, 12 );
			paint.drawLine( leftBase + 0, 13, leftBase + 3, 13 );
			/* Adjust the left base */
			leftBase += widthOfLetter( "S" );
		}
		if( ca.mid( i, 1 ) == "a" )
		{
			paint.drawLine( leftBase + 0, 11, leftBase + 0, 12 );
			paint.drawLine( leftBase + 1, 13, leftBase + 3, 13 );
			paint.drawLine( leftBase + 1, 10, leftBase + 3, 10 );
			paint.drawLine( leftBase + 1, 8, leftBase + 3, 8 );
			paint.drawLine( leftBase + 4, 9, leftBase + 4, 13 );
			/* Adjust the left base */
			leftBase += widthOfLetter( "a" );
		}
		if( ca.mid( i, 1 ) == "b" )
		{
			paint.drawLine( leftBase + 0, 5, leftBase + 0, 13 );
			paint.drawLine( leftBase + 1, 13, leftBase + 3, 13 );
			paint.drawLine( leftBase + 4, 9, leftBase + 4, 12 );
			paint.drawLine( leftBase + 1, 8, leftBase + 3, 8 );
			/* Adjust the left base */
			leftBase += widthOfLetter( "b" );
		}
		if( ca.mid( i, 1 ) == "c" )
		{
			paint.drawLine( leftBase + 0, 9, leftBase + 0, 12 );
			paint.drawLine( leftBase + 1, 13, leftBase + 3, 13 );
			paint.drawLine( leftBase + 1, 8, leftBase + 3, 8 );
			/* Adjust the left base */
			leftBase += widthOfLetter( "c" );
		}
		if( ca.mid( i, 1 ) == "e" )
		{
			paint.drawLine( leftBase + 0, 9, leftBase + 0, 12 );
			paint.drawLine( leftBase + 1, 13, leftBase + 3, 13 );
			paint.drawLine( leftBase + 4, 12, leftBase + 4, 12 );
			paint.drawLine( leftBase + 1, 10, leftBase + 4, 10 );
			paint.drawLine( leftBase + 4, 9, leftBase + 4, 9 );
			paint.drawLine( leftBase + 1, 8, leftBase + 3, 8 );
			/* Adjust the left base */
			leftBase += widthOfLetter( "e" );
		}
		if( ca.mid( i, 1 ) == "g" )
		{
			paint.drawLine( leftBase + 0, 9, leftBase + 0, 12 );
			paint.drawLine( leftBase + 1, 13, leftBase + 3, 13 );
			paint.drawLine( leftBase + 1, 8, leftBase + 3, 8 );
			paint.drawLine( leftBase + 4, 8, leftBase + 4, 14 );
			paint.drawLine( leftBase + 1, 15, leftBase + 3, 15 );
			/* Adjust the left base */
			leftBase += widthOfLetter( "g" );
		}
		if( ca.mid( i, 1 ) == "h" )
		{
			paint.drawLine( leftBase + 0, 5, leftBase + 0, 13 );
			paint.drawLine( leftBase + 1, 8, leftBase + 3, 8 );
			paint.drawLine( leftBase + 4, 9, leftBase + 4, 13 );
			/* Adjust the left base */
			leftBase += widthOfLetter( "h" );
		}
		if( ca.mid( i, 1 ) == "i" )
		{
			paint.drawLine( leftBase + 0, 6, leftBase + 0, 6 );
			paint.drawLine( leftBase + 0, 8, leftBase + 0, 13 );
			/* Adjust the left base */
			leftBase += widthOfLetter( "i" );
		}
		if( ca.mid( i, 1 ) == "l" )
		{
			paint.drawLine( leftBase + 0, 5, leftBase + 0, 13 );
			/* Adjust the left base */
			leftBase += widthOfLetter( "l" );
		}
		if( ca.mid( i, 1 ) == "m" )
		{
			paint.drawLine( leftBase + 0, 8, leftBase + 0, 13 );
			paint.drawLine( leftBase + 1, 8, leftBase + 2, 8 );
			paint.drawLine( leftBase + 3, 9, leftBase + 3, 13 );
			paint.drawLine( leftBase + 4, 8, leftBase + 5, 8 );
			paint.drawLine( leftBase + 6, 9, leftBase + 6, 13 );
			/* Adjust the left base */
			leftBase += widthOfLetter( "m" );
		}
		if( ca.mid( i, 1 ) == "n" )
		{
			paint.drawLine( leftBase + 0, 8, leftBase + 0, 13 );
			paint.drawLine( leftBase + 1, 8, leftBase + 3, 8 );
			paint.drawLine( leftBase + 4, 9, leftBase + 4, 13 );
			/* Adjust the left base */
			leftBase += widthOfLetter( "n" );
		}
		if( ca.mid( i, 1 ) == "o" )
		{
			paint.drawLine( leftBase + 0, 9, leftBase + 0, 12 );
			paint.drawLine( leftBase + 1, 13, leftBase + 3, 13 );
			paint.drawLine( leftBase + 4, 9, leftBase + 4, 12 );
			paint.drawLine( leftBase + 1, 8, leftBase + 3, 8 );
			/* Adjust the left base */
			leftBase += widthOfLetter( "o" );
		}
		if( ca.mid( i, 1 ) == "p" )
		{
			paint.drawLine( leftBase + 0, 8, leftBase + 0, 15 );
			paint.drawLine( leftBase + 1, 13, leftBase + 3, 13 );
			paint.drawLine( leftBase + 1, 8, leftBase + 3, 8 );
			paint.drawLine( leftBase + 4, 9, leftBase + 4, 12 );
			/* Adjust the left base */
			leftBase += widthOfLetter( "p" );
		}
		if( ca.mid( i, 1 ) == "r" )
		{
			paint.drawLine( leftBase + 0, 8, leftBase + 0, 13 );
			paint.drawLine( leftBase + 1, 9, leftBase + 1, 9 );
			paint.drawLine( leftBase + 2, 8, leftBase + 2, 8 );
			/* Adjust the left base */
			leftBase += widthOfLetter( "r" );
		}
		if( ca.mid( i, 1 ) == "s" )
		{
			paint.drawLine( leftBase + 1, 8, leftBase + 3, 8 );
			paint.drawLine( leftBase + 0, 9, leftBase + 0, 10 );
			paint.drawLine( leftBase + 1, 10, leftBase + 1, 10 );
			paint.drawLine( leftBase + 2, 11, leftBase + 3, 11 );
			paint.drawLine( leftBase + 3, 12, leftBase + 3, 12 );
			paint.drawLine( leftBase + 0, 13, leftBase + 2, 13 );
			/* Adjust the left base */
			leftBase += widthOfLetter( "s" );
		}
		if( ca.mid( i, 1 ) == "t" )
		{
			paint.drawLine( leftBase + 0, 6, leftBase + 0, 12 );
			paint.drawLine( leftBase + 1, 8, leftBase + 2, 8 );
			paint.drawLine( leftBase + 1, 13, leftBase + 2, 13 );
			/* Adjust the left base */
			leftBase += widthOfLetter( "t" );
		}
		if( ca.mid( i, 1 ) == "u" )
		{
			paint.drawLine( leftBase + 0, 8, leftBase + 0, 12 );
			paint.drawLine( leftBase + 1, 13, leftBase + 3, 13 );
			paint.drawLine( leftBase + 4, 8, leftBase + 4, 13 );
			/* Adjust the left base */
			leftBase += widthOfLetter( "u" );
		}
		if( ca.mid( i, 1 ) == "v" )
		{
			paint.drawLine( leftBase + 0, 8, leftBase + 0, 9 );
			paint.drawLine( leftBase + 1, 10, leftBase + 1, 11 );
			paint.drawLine( leftBase + 2, 12, leftBase + 2, 13 );
			paint.drawLine( leftBase + 3, 10, leftBase + 3, 11 );
			paint.drawLine( leftBase + 4, 8, leftBase + 4, 9 );
			/* Adjust the left base */
			leftBase += widthOfLetter( "v" );
		}
		if( ca.mid( i, 1 ) == "z" ) //APA added cpied from "v"
		{
			paint.drawLine( leftBase + 0, 8, leftBase + 0, 9 );
			paint.drawLine( leftBase + 1, 10, leftBase + 1, 11 );
			paint.drawLine( leftBase + 2, 12, leftBase + 2, 13 );
			paint.drawLine( leftBase + 3, 10, leftBase + 3, 11 );
			paint.drawLine( leftBase + 4, 8, leftBase + 4, 9 );
			/* Adjust the left base */
			leftBase += widthOfLetter( "z" );
		}
		if( ca.mid( i, 1 ) == "y" )
		{
			paint.drawLine( leftBase + 0, 8, leftBase + 0, 9 );
			paint.drawLine( leftBase + 1, 10, leftBase + 1, 11 );
			paint.drawLine( leftBase + 2, 12, leftBase + 2, 13 );
			paint.drawLine( leftBase + 1, 14, leftBase + 1, 15 );
			paint.drawLine( leftBase + 3, 10, leftBase + 3, 11 );
			paint.drawLine( leftBase + 4, 8, leftBase + 4, 9 );
			/* Adjust the left base */
			leftBase += widthOfLetter( "y" );
		}
		if( ca.mid( i, 1 ) == "0" )
		{
			paint.drawLine( leftBase + 1, topBase + 0, leftBase + 3, topBase + 0 );
			paint.drawLine( leftBase + 0, topBase + 1, leftBase + 0, topBase + 6 );
			paint.drawLine( leftBase + 1, topBase + 7, leftBase + 3, topBase + 7 );
			paint.drawLine( leftBase + 4, topBase + 1, leftBase + 4, topBase + 6 );
			/* Adjust the left base */
			leftBase += widthOfLetter( "0" );
		}
		if( ca.mid( i, 1 ) == "1" )
		{
			paint.drawLine( leftBase + 1, topBase + 1, leftBase + 1, topBase + 1 );
			paint.drawLine( leftBase + 2, topBase + 0, leftBase + 2, topBase + 6 );
			paint.drawLine( leftBase + 1, topBase + 7, leftBase + 3, topBase + 7 );
			/* Adjust the left base */
			leftBase += widthOfLetter( "1" );
		}
		if( ca.mid( i, 1 ) == "2" )
		{
			paint.drawLine( leftBase + 0, topBase + 1, leftBase + 0, topBase + 1 );
			paint.drawLine( leftBase + 1, topBase + 0, leftBase + 3, topBase + 0 );
			paint.drawLine( leftBase + 4, topBase + 1, leftBase + 4, topBase + 2 );
			paint.drawLine( leftBase + 3, topBase + 3, leftBase + 3, topBase + 3 );
			paint.drawLine( leftBase + 2, topBase + 4, leftBase + 2, topBase + 4 );
			paint.drawLine( leftBase + 1, topBase + 5, leftBase + 1, topBase + 5 );
			paint.drawLine( leftBase + 0, topBase + 6, leftBase + 0, topBase + 6 );
			paint.drawLine( leftBase + 0, topBase + 7, leftBase + 4, topBase + 7 );
			/* Adjust the left base */
			leftBase += widthOfLetter( "2" );
		}
		if( ca.mid( i, 1 ) == "3" )
		{
			paint.drawLine( leftBase + 0, topBase + 1, leftBase + 0, topBase + 1 );
			paint.drawLine( leftBase + 1, topBase + 0, leftBase + 3, topBase + 0 );
			paint.drawLine( leftBase + 4, topBase + 1, leftBase + 4, topBase + 2 );
			paint.drawLine( leftBase + 2, topBase + 3, leftBase + 3, topBase + 3 );
			paint.drawLine( leftBase + 4, topBase + 4, leftBase + 4, topBase + 6 );
			paint.drawLine( leftBase + 1, topBase + 7, leftBase + 3, topBase + 7 );
			paint.drawLine( leftBase + 0, topBase + 6, leftBase + 0, topBase + 6 );
			/* Adjust the left base */
			leftBase += widthOfLetter( "3" );
		}
		if( ca.mid( i, 1 ) == "4" )
		{
			paint.drawLine( leftBase + 3, topBase + 0, leftBase + 3, topBase + 7 );
			paint.drawLine( leftBase + 2, topBase + 1, leftBase + 2, topBase + 1 );
			paint.drawLine( leftBase + 1, topBase + 2, leftBase + 1, topBase + 2 );
			paint.drawLine( leftBase + 0, topBase + 3, leftBase + 0, topBase + 3 );
			paint.drawLine( leftBase + 0, topBase + 4, leftBase + 4, topBase + 4 );
			/* Adjust the left base */
			leftBase += widthOfLetter( "4" );
		}
		if( ca.mid( i, 1 ) == "5" )
		{
			paint.drawLine( leftBase + 0, topBase + 0, leftBase + 4, topBase + 0 );
			paint.drawLine( leftBase + 0, topBase + 1, leftBase + 0, topBase + 2 );
			paint.drawLine( leftBase + 0, topBase + 3, leftBase + 3, topBase + 3 );
			paint.drawLine( leftBase + 4, topBase + 4, leftBase + 4, topBase + 6 );
			paint.drawLine( leftBase + 1, topBase + 7, leftBase + 3, topBase + 7 );
			paint.drawLine( leftBase + 0, topBase + 6, leftBase + 0, topBase + 6 );
			/* Adjust the left base */
			leftBase += widthOfLetter( "5" );
		}
		if( ca.mid( i, 1 ) == "6" )
		{
			paint.drawLine( leftBase + 2, topBase + 0, leftBase + 3, topBase + 0 );
			paint.drawLine( leftBase + 1, topBase + 1, leftBase + 1, topBase + 1 );
			paint.drawLine( leftBase + 0, topBase + 2, leftBase + 0, topBase + 6 );
			paint.drawLine( leftBase + 1, topBase + 7, leftBase + 3, topBase + 7 );
			paint.drawLine( leftBase + 4, topBase + 6, leftBase + 4, topBase + 4 );
			paint.drawLine( leftBase + 3, topBase + 3, leftBase + 1, topBase + 3 );
			/* Adjust the left base */
			leftBase += widthOfLetter( "6" );
		}
		if( ca.mid( i, 1 ) == "7" )
		{
			paint.drawLine( leftBase + 0, topBase + 0, leftBase + 4, topBase + 0 );
			paint.drawLine( leftBase + 4, topBase + 1, leftBase + 4, topBase + 1 );
			paint.drawLine( leftBase + 3, topBase + 2, leftBase + 3, topBase + 3 );
			paint.drawLine( leftBase + 2, topBase + 4, leftBase + 2, topBase + 5 );
			paint.drawLine( leftBase + 1, topBase + 6, leftBase + 1, topBase + 7 );
			/* Adjust the left base */
			leftBase += widthOfLetter( "7" );
		}
		if( ca.mid( i, 1 ) == "8" )
		{
			paint.drawLine( leftBase + 1, topBase + 0, leftBase + 3, topBase + 0 );
			paint.drawLine( leftBase + 0, topBase + 1, leftBase + 0, topBase + 2 );
			paint.drawLine( leftBase + 4, topBase + 1, leftBase + 4, topBase + 2 );
			paint.drawLine( leftBase + 1, topBase + 3, leftBase + 3, topBase + 3 );
			paint.drawLine( leftBase + 0, topBase + 4, leftBase + 0, topBase + 6 );
			paint.drawLine( leftBase + 4, topBase + 4, leftBase + 4, topBase + 6 );
			paint.drawLine( leftBase + 1, topBase + 7, leftBase + 3, topBase + 7 );
			/* Adjust the left base */
			leftBase += widthOfLetter( "8" );
		}
		if( ca.mid( i, 1 ) == "9" )
		{
			paint.drawLine( leftBase + 1, topBase + 0, leftBase + 3, topBase + 0 );
			paint.drawLine( leftBase + 0, topBase + 1, leftBase + 0, topBase + 3 );
			paint.drawLine( leftBase + 1, topBase + 4, leftBase + 3, topBase + 4 );
			paint.drawLine( leftBase + 4, topBase + 1, leftBase + 4, topBase + 5 );
			paint.drawLine( leftBase + 3, topBase + 6, leftBase + 3, topBase + 6 );
			paint.drawLine( leftBase + 1, topBase + 7, leftBase + 2, topBase + 7 );
			/* Adjust the left base */
			leftBase += widthOfLetter( "9" );
		}
		if( ca.mid( i, 1 ) == " " )
		{
			leftBase += widthOfLetter( " " );
		}
	}
	paint.end();
	setUpdatesEnabled( true );
	return;
}
/*!
* Reimplemented to draw the custom widget interface and control drawing and
* redrawing of the today indicator, initial selection indicator and
* month/year caption as necessary.
*
* \internal
*/
void dateNav::paintEvent( QPaintEvent * )
{
	setUpdatesEnabled( false );
	QPainter paint;
	paint.begin( this );
	/*
	* Layout format is:
	* start-right, start-top, end-right, end-top
	*/
	/* set the pen colour to gray */
	paint.setPen( QColor( 127, 127, 127 ) );
	/* the top line */
	paint.drawLine( 0, 0, 143, 0 );
	/* the left line */
	paint.drawLine( 0, 0, 0, 125 );
	/* the mid line */
	paint.drawLine( 0, 18, 143, 18 );
	/* the linking line */
	paint.drawLine( 143, 0, 143, 18 );
	/* draw the rectangle the calender will be shown on */
	paint.fillRect( 1, 19, 143, 125, QColor( 255, 255, 255 ) );
	/* the gray line below the days */
	paint.drawLine( 12, 33, 130, 33 );
	/* set the pen to white */
	paint.setPen( QColor( 255, 255, 255 ) );
	/* the bottom line */
	paint.drawLine( 0, 126, 144, 126 );
	/* the right hand line */
	paint.drawLine( 144, 0, 144, 126 );
	/* the 3d lines on the title */
	paint.drawLine( 1, 1, 142, 1 );
	paint.drawLine( 1, 2, 1, 17 );
	/*
	* start-right, start-top, end-right, end-top
	*/
	/* set the pen to black */
	paint.setPen( QColor( 0, 0, 0 ) );
	/* the left hand triangle */
	paint.drawLine( 8, 10, 12, 6 );
	paint.drawLine( 12, 7, 12, 13 );
	paint.drawLine( 8, 10, 12, 14 );
	/* fill the left hand triangle */
	paint.drawLine( 11, 8, 11, 12 );
	paint.drawLine( 10, 9, 10, 11 );
	paint.drawLine( 9, 10, 9, 10 );
	/* the right hand triangle */
	paint.drawLine( 133, 7, 133, 13 );
	paint.drawLine( 133, 6, 137, 10 );
	paint.drawLine( 133, 14, 137, 10 );
	/* fill the right hand triangle */
	paint.drawLine( 134, 8, 134, 12 );
	paint.drawLine( 135, 9, 135, 11 );
	paint.drawLine( 136, 10, 136, 10 );
	/* the caption space */
	paint.fillRect( 13, 5, 120, 11, QColor( 192, 192, 192 ) );
	/*
	* The days. These are column headers for the dates.
	*/
	/*
	* Sunday
	*/
	paint.drawLine( 22, 22, 25, 22 );
	paint.drawLine( 21, 23, 21, 24 );
	paint.drawLine( 22, 25, 24, 25 );
	paint.drawLine( 25, 26, 25, 28 );
	paint.drawLine( 21, 29, 24, 29 );
	/*
	* Monday
	*/
	paint.drawLine( 36, 22, 36, 29 );
	paint.drawLine( 37, 22, 37, 23 );
	paint.drawLine( 38, 24, 38, 25 );
	paint.drawLine( 39, 26, 39, 27 );
	paint.drawLine( 40, 24, 40, 25 );
	paint.drawLine( 41, 22, 41, 23 );
	paint.drawLine( 42, 22, 42, 29 );
	/*
	* Tuesday
	*/
	paint.drawLine( 55, 22, 59, 22 );
	paint.drawLine( 57, 23, 57, 29 );
	/*
	* Wednesday
	*/
	paint.drawLine( 68, 22, 68, 24 );
	paint.drawLine( 69, 25, 69, 27 );
	paint.drawLine( 70, 28, 70, 29 );
	paint.drawLine( 71, 25, 71, 27 );
	paint.drawLine( 72, 22, 72, 24 );
	paint.drawLine( 73, 25, 73, 27 );
	paint.drawLine( 74, 28, 74, 29 );
	paint.drawLine( 75, 25, 75, 27 );
	paint.drawLine( 76, 22, 76, 24 );
	/*
	* Thursday
	*/
	paint.drawLine( 89, 22, 93, 22 );
	paint.drawLine( 91, 23, 91, 29 );
	/*
	* Friday
	*/
	paint.drawLine( 106, 22, 106, 29 );
	paint.drawLine( 107, 22, 110, 22 );
	paint.drawLine( 107, 25, 110, 25 );
	/*
	* Saturday
	*/
	paint.drawLine( 124, 22, 127, 22 );
	paint.drawLine( 123, 23, 123, 24 );
	paint.drawLine( 124, 25, 126, 25 );
	paint.drawLine( 127, 26, 127, 28 );
	paint.drawLine( 123, 29, 126, 29 );
	/*
	* Is there a selected dateObj? There should always be one, so we paint it
	* as selected (grey cursor).
	*/
	dateObj *checkItem;
	for( unsigned int i = 0; i < lstDateObjects.count(); i++ )
	{
		checkItem = lstDateObjects.at( i );
		if( checkItem->selectedDay() )
		{
			/*
			* Paint a selection "cursor" box.
			*/
			paint.fillRect( sBoxX, sBoxY, 17, 14, QColor( 192, 192, 192 ) );
			continue;
		}
	}
	/*
	* If the displayed month is the current month, draw a red box
	* around today.
	*/
	if( isThisMonth )
	{
		paint.setPen( QColor( 255, 0, 0 ) );
		paint.drawLine( tBoxX - 3, tBoxY - 3, tBoxX + 13, tBoxY - 3 );
		paint.drawLine( tBoxX - 3, tBoxY - 3, tBoxX - 3, tBoxY + 10 );
		paint.drawLine( tBoxX - 3, tBoxY + 10, tBoxX + 13, tBoxY + 10 );
		paint.drawLine( tBoxX + 13, tBoxY - 3, tBoxX + 13, tBoxY + 10 );
	}
	/*
	* Close the paint device
	*/
	paint.end();
	/*
	* Paint the month (a sort of label).
	*/
	paintMonth();
	setUpdatesEnabled( true );
}
/*!
* This slot handles click events from any of the date objects.
*
* This is one of the ways in which the selected date is changed, and
* also controls painting of the selection cursor, and can affect the
* today cursor. Depending on the selection, this slot can emit a signal
* containing the newly selected date.
* \sa dateObj::date()
* \sa setupDates()
* \sa dateChanged( QDate )
* 
* \internal
*/
void dateNav::sltDateClicked( dateObj & cDate, int cx, int cy )
{
	/*
	* If it's already selected, do nothing.
	*/
	if( cDate.selectedDay() )
	{
		return;
	}
	/*
	* If it's not in the current month, we need to reconstruct the date
	* matrix.
	*/
	if( !( cDate.date().month() == currentMonth.month() ) )
	{
		currentMonth = cDate.date();
		setupDates();
		emit dateChanged( currentMonth );
		return;
	}
	QPainter paint;
	/*
	* Iterate the list, looking for an item that's already selected, when we
	* find that item, deselect it, and continue with the function.
	*/
	dateObj *checkItem;
	for( unsigned int i = 0; i < lstDateObjects.count(); i++ )
	{
		checkItem = lstDateObjects.at( i );
		if( checkItem->selectedDay() )
		{
			paint.begin( this );
			/* Remove the grey boxing */
			paint.fillRect( sBoxX, sBoxY, 17, 14, QColor( 255, 255, 255 ) );
			/*
			* If the displayed month is the current month, draw a red box
			* around today.
			*/
			if( isThisMonth )
			{
				paint.setPen( QColor( 255, 0, 0 ) );
				paint.drawLine( tBoxX - 3, tBoxY - 3, tBoxX + 13, tBoxY - 3 );
				paint.drawLine( tBoxX - 3, tBoxY - 3, tBoxX - 3, tBoxY + 10 );
				paint.drawLine( tBoxX - 3, tBoxY + 10, tBoxX + 13, tBoxY + 10 );
				paint.drawLine( tBoxX + 13, tBoxY - 3, tBoxX + 13, tBoxY + 10 );
			}
			paint.end();
			/* Set the item found to unselected */
			checkItem->setSelectedDay( false );
			continue;
		}
	}    
	/*
	* The dateObj that was selected is now deselected, so we set the
	* clicked dateObj to selected.
	*/
	/*
	* We store modified X and Y coords of the newly selected box, so
	* that we can erase any selection cursor later.
	*/
	sBoxX = cx - 3;
	sBoxY = cy - 3;
	/*
	* Paint a selection "cursor" box.
	*/
	paint.begin( this );
	paint.fillRect( sBoxX, sBoxY, 17, 14, QColor( 192, 192, 192 ) );
	/*
	* If the displayed month is the current month, draw a red box
	* around today.
	*/
	if( isThisMonth )
	{
		paint.setPen( QColor( 255, 0, 0 ) );
		paint.drawLine( tBoxX - 3, tBoxY - 3, tBoxX + 13, tBoxY - 3 );
		paint.drawLine( tBoxX - 3, tBoxY - 3, tBoxX - 3, tBoxY + 10 );
		paint.drawLine( tBoxX - 3, tBoxY + 10, tBoxX + 13, tBoxY + 10 );
		paint.drawLine( tBoxX + 13, tBoxY - 3, tBoxX + 13, tBoxY + 10 );
	}
	paint.end();
	/*
	* And finally, set the newly selected widget to selected.
	* Note that this kicks the widgets repaint member.
	*/
	cDate.setSelectedDay( true );
	emit dateChanged( cDate.date() );
}
/*!
* Reimplemented to handle click events on the forward button (move a
* month forward), back button (move a month backwards) and caption (pop
* up the seven month selection menu).
*
* Date Objects handle their own mouse events.
*
* \sa dateObj
* \sa datePopup
* \sa dateObj::::mousePressEvent()
* \internal
*/
void dateNav::mousePressEvent( QMouseEvent * mpe )
{
	/* If the click was from the left button */
	if( mpe->button() == LeftButton )
	{
		/* If the y() is consistent with the arrows */
		if( mpe->y() >= 6 && mpe->y() <= 14 )
		{
			/* If the x() is consistent with the back arrow */
			if( mpe->x() >= 8 && mpe->x() <= 12 )
			{
				int tmpYear = currentMonth.year();
				int tmpMonth = currentMonth.month();
				int tmpDay = 0;
				/*
				* If the current month is January, make the month we're going
				* to December and the year the year prior to the current,
				* otherwise make the month one month prior to this month.
				*/
				if( tmpMonth == 1 )
				{
					tmpMonth = 12;
					tmpYear--;
				}
				else
				{
					tmpMonth--;
				}
				/* Find out which day is selected */
				dateObj *checkItem;
				for( unsigned int i = 0; i < lstDateObjects.count(); i++ )
				{
					checkItem = lstDateObjects.at( i );
					if( checkItem->selectedDay() )
					{
						tmpDay = checkItem->date().day();
					}
				}
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
							/* it is not a leap year */
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
				currentMonth = QDate( tmpYear, tmpMonth, tmpDay );
				setupDates();
				emit dateChanged( currentMonth );
				return;
			}
			/* If the x() is consistent with the forward arrow */
			if( mpe->x() >= 133 && mpe->x() <= 137 )
			{
				int tmpYear = currentMonth.year();
				int tmpMonth = currentMonth.month();
				int tmpDay = 0;
				/*
				* If the current month is December, make the month we're going
				* to January and the year the year after the current,
				* otherwise make the month one month after this month.
				*/
				if( tmpMonth == 12 )
				{
					tmpMonth = 1;
					tmpYear++;
				}
				else
				{
					tmpMonth++;
				}
				/* Find out which day is selected */
				dateObj *checkItem;
				for( unsigned int i = 0; i < lstDateObjects.count(); i++ )
				{
					checkItem = lstDateObjects.at( i );
					if( checkItem->selectedDay() )
					{
						tmpDay = checkItem->date().day();
					}
				}
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
				currentMonth = QDate( tmpYear, tmpMonth, tmpDay );
				setupDates();
				emit dateChanged( currentMonth );
				return;
			} /* End of forward movement */
		} /* End of y() check */
	} /* End of x() check */
	/* Again, a left button action */
	if( mpe->button() == LeftButton )
	{
		/* If it's in the "caption" area */
		if( mpe->y() >= 5 && mpe->y() <= 16 )
		{
			if( mpe->x() >= 23 && mpe->x() <= 121 )
			{
				/* Pop up the seven months selected menu... */
				int tmpInt = ( sevenMonthPopup->height() / 7 ) * 3;
				QPoint tmpPoint = mapToGlobal( QPoint( 22, 0 ) );
				tmpPoint.setY( ( tmpPoint.y() - tmpInt ) - 3 );
				sevenMonthPopup->setMiddleMonth( currentMonth );
				sevenMonthPopup->popup( tmpPoint, -1 );
				return;
			}
		}
	}
}
/*!
* Use this function to directly query the date navigator widget for the
* date the currently selected item represents.
*
* This provides you with a way to find out the selected day without having
* to wait for a dateChanged() signal.
*
* \warning In the (supposedly) impossible case that there is no selected date,
* this function will return an invalid QDate object.
* \return The currently selected date.
* \sa setDate()
* \sa dateChanged( QDate )
* \sa dateObj::selectedDay()
*/
QDate dateNav::selectedDate()
{
	/* Find out which day is selected */
	dateObj *checkItem = 0;
	QDate selDate;
	for( unsigned int i = 0; i < lstDateObjects.count(); i++ )
	{
		checkItem = lstDateObjects.at( i );
		if( checkItem->selectedDay() )
		{
			selDate = checkItem->date();
			continue;
		}
	}
	return selDate;
}
/*!
* Calculates the width of the letter (in our internal font) we have to paint.
*
* \return The width of the letter passed as the argument.
* \internal
*/
int dateNav::widthOfLetter( QString letter )
{
	if( letter == "M" || letter == "O" || letter == "m" )
	{
		return 7 + 1;
	}
	else if( letter == "A" || letter == "D" || letter == "N" )
	{
		return 6 + 1;
	}
	else if( letter == "0" || letter == "1" || letter == "2" || letter == "3" || letter == "4" || letter == "5" || letter == "6" || letter == "7" || letter == "8" || letter == "9" )
	{
		return 6;
	}
	else if( letter == "F" || letter == "S" || letter == "a" || letter == "b" || letter == "e" || letter == "g" || letter == "h" || letter == "n" || letter == "o" || letter == "p" || letter == "u" || letter == "v" || letter == "y")
	{
		return 5 + 1;
	}
	else if( letter == "J" || letter == "c" || letter == "s" )
	{
		return 4 + 1;
	}
	else if( letter == "r" || letter == "t" || letter == "r" )
	{
		return 3 + 1;
	}
	else if( letter == "i" || letter == "l" )
	{
		return 1 + 1;
	}
	else if( letter == " " )
	{
		return 3;
	}
	else
	{
		return 0;
	}
}
/*!
* This slot handles date change events from the popup menu.
* 
* \internal
*/
void dateNav::monthSelected( QDate nMo )
{
	setUpdatesEnabled( false );
	currentMonth = QDate( nMo );
	emit dateChanged( currentMonth );
	setupDates();
	setUpdatesEnabled( true );
}

