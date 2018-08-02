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
*Header For: Alarm Display Window
*Purpose: this displays current alarm groups and alarm states
The alarm display shows on the left the alarm groups. On the right the alarm status of points
belonging to the selected alarm group (or All)
*/

#ifndef include_alarmdisplay_hpp 
#define include_alarmdisplay_hpp 
#include <qt.h>
#include "statuspane.h"
class AlarmDisplay : public QSplitter
{
	Q_OBJECT
	QListView *pAlarmGroups; // pane with alarm group status
	QListView *pAlarmPoints; // pane with alarm points status 
	QTimer *pTimer;
	//
	QString CurrentGroup;
	//
	StatusPane Status;
	//
	enum { tList = 1,tGroup,tItem,tTags,tSamplePoint,tTagLimits};
	//
	public:
	AlarmDisplay(QWidget *parent);

	public slots:
	void SelChanged(); // handle the selection
	void PointSelChanged(); // handle the selection
	void ReceivedNotify(int, const char *); 
	void QueryResponse (QObject *, const QString &, int, QObject*);
	void CurrentQueryResponse (QObject *, const QString &, int, QObject*);
	void PointMenu(QListViewItem *, const QPoint &, int); // an item has been right clicked - open the point menu
	void GroupMenu(QListViewItem *, const QPoint &, int); // a group alarm has been right clicked - open its menu
	void DoSelChange(); //action the selection change
	void Restart();
	void Flash();
};
#endif

