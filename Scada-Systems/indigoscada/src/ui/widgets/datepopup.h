/*
* Copyright 1998, Michael van der Westhuizen.
* This package is under the LOLL (Less Obsessive Linux License).
* See LOLL for details.
*/
/*!
* \file datepopup.h
* \brief Month Selection Popup Menu Definition.
*
* This file defines the datePopup class, as implemented in the associated
* source file, datepopup.cpp.
*
* \internal
*/
#ifndef DATEPOPUP_H
#define DATEPOPUP_H
#include <qt.h>
/*!
* \class datePopup datepopup.h
* \brief Month Selection Popup Menu.
*
* The Month Selection popup menu is an internal class, and is a modified QPopupMenu
* that calculates its own contents, based on variables set at construction
* and prior to calling.
*
* \warning You should never instantiate this object directly, but rather access the methods
* made available in dateNav.
* \author Michael van der Westhuizen (michael@mudpuppies.co.za).
* \date 24/11/1998
* \internal
*/
class datePopup: public QPopupMenu
{
	Q_OBJECT
	public:
	datePopup( class QWidget *parent = 0, const char *name = 0, QDate cMonth = QDate() );
	void setMiddleMonth( QDate );
	signals:
	/*!
	* This signal carries a full QDate object for the dateNav to change to.
	* The signal is emitted whenever a new month is selected from the list.
	*/
	void newMonthSelected( QDate );
	private slots:
	void sltItemClicked( int );
	private:
	QString dateText( QDate );
	void makeDates( QDate );
	int fixDays( int, int, int );
	QDate middleMonth;
	QDate backThree;
	QDate backTwo;
	QDate backOne;
	QDate forwardOne;
	QDate forwardTwo;
	QDate forwardThree;
	int idOne;
	int idTwo;
	int idThree;
	int idFour;
	int idFive;
	int idSix;
	int idSeven;
};
#endif

