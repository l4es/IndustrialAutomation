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
#ifndef DateNavigator_included
#define DateNavigator_included
//
#include "DateNavigatorData.h"
//
#include "common.h"
//
class QSEXPORT DateNavigator : public DateNavigatorData
{
	Q_OBJECT
	public:
	DateNavigator
	(
	QWidget* parent = NULL,
	const char* name = NULL
	);
	virtual ~DateNavigator();
	QString GetDate()
	{
		QString s;
		QDate d = TheDate->selectedDate();
		s.sprintf("%04d-%02d-%02d",d.year(),d.month(),d.day());
		return s;
	};
	void SetDate(const QString &s)
	{
		//
		char p[16];
		memset(p,0,sizeof(p));
		strncpy(p,(const char *)s,sizeof(p));
		//
		p[4] = p[7] = 0;
		//
		int day  = atoi(p + 8);
		int mon  = atoi(p + 5);
		int year = atoi(p);
		//
		QDate d(year,mon,day);
		TheDate->setDate(d);
		//
	};
	friend class DATEEDIT;
};
#endif // DateNavigator_included

