/*
* Copyright 1998, Michael van der Westhuizen.
* This package is under the LOLL (Less Obsessive Linux License).
* See LOLL for details.
*/
/*!
* \file dateobj.h
* \brief Date Item Widget Definition.
*
* This file defines the dateObj class, as implemented in the associated
* source file, dateobj.cpp.
*
* \internal
*/
#ifndef DATEOBJ_H
#define DATEOBJ_H
#include <qt.h>
/*!
* \class dateObj dateobj.h
* \brief Date Item Widget.
*
* The Date Item Widget is an internal class.
*
* \warning You should never instantiate this object directly, but rather access the methods
* made available in dateNav.
* \author Michael van der Westhuizen (michael@mudpuppies.co.za).
* \date 24/11/1998
* \internal
* \sa dateNav
*/
class dateObj: public QWidget
{
	Q_OBJECT
	public:
	dateObj( class QWidget *parent = 0, const char *name = 0, QDate dateRepresented = QDate(), int selectedMonth = 0 );
	void setSelectedDay( bool );
	bool selectedDay();
	QDate date();
	protected:
	void paintEvent( QPaintEvent * );
	void mousePressEvent( QMouseEvent * );
	signals:
	/*!
	* This signal carries the following:
	* <ul>
	* <li>A pointer to a (always this) date object.
	* <li>The x() position of this object.
	* <li>The y() position of this object.
	* </ul>
	* This signal is used by dateNav.
	* \internal
	*/
	void clicked( dateObj &, int, int );
	private:
	void paintNumber( int number, bool alignment, bool current );
	void setSelectedMonth( int monthNumber );
	QDate *thisDate;
	int sMonth;
	QString dateString;
	int numOne;
	int numTwo;
	bool inCurrent;
	bool paintSelected;
	int yr;
	int mo;
	int da;
};
#endif

