/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2009 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */
#include <qt.h>
#include "dclock.h"
//
// Constructs a DigitalClock widget with a parent and a name.
//
DigitalClock::DigitalClock( QWidget *parent, const char *name )
: QLCDNumber( parent, name )
{
	showingColon = FALSE;
	setLineWidth( 2 );				// set frame line width
	showTime();					// display the current time
	setFrameStyle(QFrame::Panel | QFrame::Plain);
	normalTimer = startTimer( 500 );		// 1/2 second timer events
	setSegmentStyle(QLCDNumber::Filled);
	showDateTimer = -1;				// not showing dat
	{
		QColorGroup normal( QColor( 0,0,0 ), QColor( 0,0,0), QColor( 0,255,0 ), QColor( 0,255,0), QColor(0,0,128), 
		QColor( 0,0,0 ), QColor( 0,0,0 ) );
		QPalette palette( normal, normal, normal );
		setPalette( palette );
	}
};
//
// Handles timer events for the digital clock widget.
// There are two different timers; one timer for updating the clock
// and another one for switching back from date mode to time mode.
//
void DigitalClock::timerEvent( QTimerEvent *e )
{
	if ( e->timerId() == showDateTimer )	// stop showing date
	stopDate();
	else {					// normal timer
		if ( showDateTimer == -1 )		// not showing date
		showTime();
	}
}
//
// Enters date mode when the left mouse button is pressed.
//
void DigitalClock::mousePressEvent( QMouseEvent *e )
{
	if ( e->button() == QMouseEvent::LeftButton )		// left button pressed
	showDate();
}
//
// Shows the current date in the internal lcd widget.
// Fires a timer to stop showing the date.
//
void DigitalClock::showDate()
{
	if ( showDateTimer != -1 )			// already showing date
	return;
	QDate d = QDate::currentDate();
	QString s;
	s.sprintf( "%2d %2d", d.month(), d.day() );
	display( s );				// sets the LCD number/text
	showDateTimer = startTimer( 2000 );		// keep this state for 2 secs
}
//
// Stops showing the date.
//
void DigitalClock::stopDate()
{
	killTimer( showDateTimer );
	showDateTimer = -1;
	showTime();
}
//
// Shows the current time in the internal lcd widget.
//
void DigitalClock::showTime()
{
	showingColon = !showingColon;		// toggle/blink colon
	QString s = QTime::currentTime().toString().left(5);
	if ( !showingColon )
	s[2] = ' ';
	if ( s[0] == '0' )
	s[0] = ' ';
	display( s );				// set LCD number/text
}

