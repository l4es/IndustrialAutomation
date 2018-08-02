/*
* Copyright 1998, Michael van der Westhuizen.
* This package is under the LOLL (Less Obsessive Linux License).
* See LOLL for details.
*/
/*!
* \file datenav.h
* \brief Date Navigator Widget Definition.
*
* This file defines the dateNav class, as implemented in the associated
* source file, datenav.cpp.
*
* This is the header file you'll want to include in your projects.
*
* See the class documentation for details on usage.
*
* \sa dateNav
*/
#ifndef DATENAV_H
#define DATENAV_H
#include <qt.h>
#include "dateobj.h"
#include "datepopup.h"
/*!
* \class dateNav datenav.h
* \brief Date Navigator Widget.
*
* The Date Navigator Widget is the "top level" widget, and the one you
* want to instantiate directly in your applications.
*
* Typical usage would be:
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
*	connect( myDateNav, SIGNAL( dateChanged( QDate ) ), this, SLOT( mySlotThatHandlesDateChanges( QDate ) ) );
* ...
* // end MyFile.cpp
* \endcode
*
* Don't forget to connect to the dateChanged() signal to get the most out
* of the widget.
*
* You can also use:<br>
* \c setDate( QDate );
* <br>to set a date you want selected after construction, and:<br>
* \c selectedDate()
* <br>to find the currently selected date without waiting for a dateChanged()
* signal.
*
* \author Michael van der Westhuizen (michael@mudpuppies.co.za).
* \date 24/11/1998
* \sa dateObj
* \sa datePopup
* \sa dateChanged()
* \sa setDate()
* \sa selectedDate()
*/
class dateNav: public QWidget
{
	Q_OBJECT
	public:
	dateNav( class QWidget * parent=0, const char * name=0 );
	dateNav( const QDate * pd , class QWidget * parent=0, const char * name=0 );
	QDate selectedDate();
	bool setDate( QDate );
	signals:
	/*!
	* This signal carries a QDate representing the selected date, and is
	* emitted whenever the selected date changes.
	*
	* \sa selectedDate()
	* \sa setDate()
	*/
	void dateChanged( QDate );
	protected:
	void paintEvent( QPaintEvent * );
	void mousePressEvent( QMouseEvent * );
	private slots:
	void sltDateClicked( dateObj &, int, int );
	void monthSelected( QDate );
	private:
	void setupDates();
	void paintMonth();
	void drawCaption( QString );
	int widthOfLetter( QString );
	QList<dateObj> lstDateObjects;
	QDate todaysDate;
	QDate currentMonth;
	QDate *previousMonth;
	QDate *nextMonth;
	int sBoxX;
	int sBoxY;
	int tBoxX;
	int tBoxY;
	bool isThisMonth;
	bool isChangingMonth;
	datePopup *sevenMonthPopup;
};
#endif

