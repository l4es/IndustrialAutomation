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
*Header For: user interface common stuff
*Purpose:
*/

#ifndef include_main_hpp 
#define include_main_hpp
#include "realtimedb.h"
#include "common.h"
//

class QSApplication : public QApplication
{
	public:
	static bool fKeyMouseEvent;
	QSApplication(int &ac, char **av) : QApplication(ac,av)
	{
	};
	bool notify(QObject *o, QEvent *e);
};
#endif

