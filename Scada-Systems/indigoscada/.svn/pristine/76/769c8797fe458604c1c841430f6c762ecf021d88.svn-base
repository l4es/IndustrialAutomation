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

#include "display.h"
#include "common.h"
#include "IndentedTrace.h"
/*
*Function:paintCell
*Inputs:DC bits
*Outputs:none
*Returns:none
*/
void StateListItem::paintCell(QPainter *p,const QColorGroup &c,int col,int width,int align)
{
//	IT_IT("StateListItem::paintCell");
	
	QColor b = Qt::white; // set the default colours
	QColor f = Qt::black;
	if(!AckState)
	{
		switch(State) // select colours based on state
		{
			case Ok: 
			b = Qt::green;
			break;
			case Warning:
			b = Qt::yellow;
			break;
			case Alarm:
			b = Qt::red;
			break;
			case Fail:
			b = Qt::blue;
			f = Qt::white;
			break;
			default:
			break;
		};
	}
	else
	{
		b = Qt::cyan;
		f = Qt::black;
	};
	// draw the cell
	p->setBrush(b);
	p->setBackgroundColor(b);
	p->setPen(b);
	//
	QRect r(0,0,width,height());
	p->drawRect(r); 
	//
	p->setPen(f);
	p->drawText(r,Qt::AlignVCenter,text(col));
	//
};

