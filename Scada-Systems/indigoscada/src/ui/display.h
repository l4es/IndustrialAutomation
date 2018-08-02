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

#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <qt.h>

class StateListItem // alarm state aware list item
: public QListViewItem
{
	int State;
	bool AckState;
	bool FlashState;
	public:
	enum // various alarm states
	{
		None = 0,
		Ok, 
		Warning,
		Alarm,
		Fail
	};
	StateListItem(QListView *p, const QString &d, const QString &s, int state=None,const QString &s1="", const QString &s2= "")
	: QListViewItem(p,d,s,s1,s2) , State(state), AckState(0),FlashState(0){};
	//
	void paintCell(QPainter *p,const QColorGroup &,int,int,int);
	//
	int  GetState() const { return State;};
	bool GetAckState() { return AckState;};
	//
	void SetState(int s) { State = s;};
	void SetAckState(bool f) { AckState = f;};
	void Toggle() { FlashState = !FlashState;}; // toggle if the ack flag is set
};
#endif

