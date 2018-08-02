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
#ifndef DCLOCK_H
#define DCLOCK_H
#include <qt.h>
#include "common.h"
class QSEXPORT DigitalClock : public QLCDNumber		// digital clock widget
{
	Q_OBJECT
	public:
	DigitalClock( QWidget *parent=0, const char *name=0 );
	protected:					// event handlers
	void	timerEvent( QTimerEvent * );
	void	mousePressEvent( QMouseEvent * );
	private slots:					// internal slots
	void	stopDate();
	void	showTime();
	private:					// internal data
	void	showDate();
	bool	showingColon;
	int		normalTimer;
	int		showDateTimer;
};
#endif // DCLOCK_H

