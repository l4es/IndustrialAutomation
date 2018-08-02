/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2012 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */

#include "double_point_led.h"

DoublePointLed::DoublePointLed(QWidget *parent, const char *name) : QFrame(parent, name),
  lwidth( 17 ), lheight( 10 ), dx( 4 ), timer(NULL)
{
   s = Off;
//   setFrameStyle(Sunken | Box);
   setGeometry(0,0,lwidth,lheight);
   color = yellow;
   isFlashing = false;

   setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum ) );
}

void DoublePointLed::drawContents(QPainter *painter)
{
   int w,h,x,y;
   QBrush lightBrush(color);
   QBrush darkBrush(QColor(60,60,0));
   QPen pen(QColor(40,40,0));
//   QPen pen(QColor(140,140,140));
   
   w=width();
   h=height();
   y=(h-lheight)/2;
   x=0;
  
   qDrawShadeRect(painter,x,y,lwidth,lheight,colorGroup(),TRUE,1,1);
   painter->drawText(7+lwidth,0,w-lwidth-20,h,AlignVCenter|AlignLeft,text);
   
   switch(s) {
    case On:
      painter->setBrush(lightBrush);
      painter->drawRect(x+1,y+1,lwidth-2, lheight-2);
      break;
    case Off:
      painter->setBrush(darkBrush);
      painter->drawRect(x+1,y+1,lwidth-2, lheight-2);
      painter->setPen(pen);
      painter->drawLine(x+2,y+2,x+lwidth-2, y+2);
      painter->drawLine(x+2,y+lheight-2,x+lwidth-2,y+lheight-2);
      // Draw verticals
//      int i;
//      for (i= 2; i < width-1; i+= dx)
//	   painter->drawLine(i,2,i,height-2);
      break;
    default:
	  break;
      
   }
}

DoublePointLed::~DoublePointLed()
{
   if(timer)
   {
	  timer->stop();
	  delete timer;
   }
}

void DoublePointLed::timerSlot()
{
	toggleState();
}

QSize DoublePointLed::sizeHint() const
{
    return QSize( 10 + 9 * 10 , 23 );
}

QSizePolicy DoublePointLed::sizePolicy() const
{
    return QWidget::sizePolicy();
}

void DoublePointLed::startFlash()
{
   if(timer == NULL)
   {
	    isFlashing = true;
		timer = new QTimer(this);
		timer->start(1000);
		connect(timer,SIGNAL(timeout()), this, SLOT(timerSlot()));
   }
}

void DoublePointLed::stopFlash()
{
   if(timer)
   {
	  isFlashing = false;
	  timer->stop();
	  delete timer;
	  timer = NULL;
	  on();
   }
}

/*!
  \brief Mouse Release Event management
*/
void DoublePointLed::mouseReleaseEvent(QMouseEvent * e)
{
	if(e->button() == Qt::RightButton)
	{
		QString name = this->name();
		emit RightClicked(QString("DoublePointLed"), name);
	}
}
