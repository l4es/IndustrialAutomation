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

/*
*Header For: Validated edit control - lifted from the Qt widget archives
*Purpose:
*/

#ifndef include_valedit_hpp 
#define include_valedit_hpp 
#include <qt.h>
#include "common.h"

class QSEXPORT VALEDIT : public QLineEdit
{
	QString CharValid; 
	protected:
	void keyPressEvent(QKeyEvent *); 
	public:
	VALEDIT(QWidget *parent = 0, const char *name = 0) :
	QLineEdit(parent,name) {};
	void setCharValid(const QString &s) {CharValid = s;};
};
class QSEXPORT NAMEEDIT : public QLineEdit 
{
	void keyPressEvent(QKeyEvent *);
	public:
	NAMEEDIT(QWidget *p, const char *name = 0) : QLineEdit(p,name) 
	{
	};
};
class QSEXPORT INTEDIT : public VALEDIT
{
	protected:
	void keyPressEvent(QKeyEvent *); // [0-9-]
	public:
	INTEDIT(QWidget *parent = 0, const char *name = 0) :
	VALEDIT(parent,name) 
	{
		setCharValid("0123456789"); 
		setText("0");
	};
};
class QSEXPORT FPEDIT : public VALEDIT
{
	protected:
	int dot_counter;
	void keyPressEvent(QKeyEvent *); // [0-9-]
	public:
	FPEDIT(QWidget *parent = 0, const char *name = 0) :
	VALEDIT(parent,name),dot_counter(0) 
	{
		setCharValid("0123456789.-"); 
		setText("0.0");
	};
};
class QSEXPORT TIMEEDIT : public QLineEdit 
{
	protected:
	void keyPressEvent(QKeyEvent *); // key press event
	void mousePressEvent(QMouseEvent *); 
	public:
	TIMEEDIT(QWidget *parent = 0, const char *name = 0) :
	QLineEdit(parent,name) 
	{
		setMaxLength(8); 
		setText("00:00:00"); 
	};
	~TIMEEDIT()
	{
	};
};
class QSEXPORT DATEEDIT : public QLineEdit // ISO date entry
{
	protected:
	void keyPressEvent(QKeyEvent *); 
	void mousePressEvent(QMouseEvent *); 
	public:
	DATEEDIT(QWidget *parent = 0, const char *name = 0) :
	QLineEdit(parent,name) 
	{
		setMaxLength(10); 
		Default();
		QToolTip::add(this,tr("Click with Right Mouse Button For Calendar")); 
		setReadOnly(true);
	};
	void SetDate(const QDate &d)
	{
		QString s;
		s.sprintf("%04d-%02d-%02d",d.year(),d.month(),d.day()); 
		setText(s);
	};
	QDate GetDate() // get the date as a QDate
	{
		//
		QString s = text(); // get the string
		s[4] = ' ';
		s[7] = ' ';
		//
		int y,m,d;
		sscanf((const char *)s,"%d%d%d",&y,&m,&d);
		QDate dt(y,m,d);
		return dt;
		//
	};
	void Default() // default to today
	{
		SetDate(QDate::currentDate());
	};
	void LineValidate(); 
};
//
// Decimal Spin Box
//
//
class QSEXPORT DecimalSpinBox : public QSpinBox
{
	QDoubleValidator *pVal;
	public:
	DecimalSpinBox(QWidget *parent, const char *name = 0)
	: QSpinBox(parent,name),pVal( new QDoubleValidator(0.0,10.0,1,this))
	{
		setMinValue(0);
		setMaxValue(100); 
		setValidator(pVal);
	};
	QString mapValueToText( int value)
	{
		return QString("%1.%2").arg(value/10).arg(value % 10);
	};
	int mapTextToValue(bool *ok)
	{
		int ret = (int)(currentValueText().toDouble() * 10.0 + 0.6);
		return ret;
	};
	void SetValue(double v)
	{
		int i = (int)(v * 10.0 + 0.6);
		setValue(i);
	};
	void SetMinMax(double Min, double Max)
	{
		pVal->setRange(Min,Max,1);
		setMinValue((int)(Min * 10));
		setMaxValue((int)(Max * 10)); // number of steps needed
	};
};
#endif

