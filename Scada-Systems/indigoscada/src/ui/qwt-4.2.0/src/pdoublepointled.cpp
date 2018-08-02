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

#include <qwidget.h>
#include "pdoublepointled.h"

/*!
  \brief Constructor
  \param parent Parent Widget
  \param name Name
*/
PDoublePointLed::PDoublePointLed(QWidget *parent, const char *name)
        : QWidget(parent,name), timer(NULL)
{
    OnColor=QColor(white); // default on color is white
    OffColor=paletteBackgroundColor();
	s = Off;
    isFlashing = false;
}

/*!
  \brief Repaint the widget
*/
void PDoublePointLed::paintEvent(QPaintEvent *)
{
    QPainter paint( this );
    drawLed(&paint) ;
}

/*!
  \brief Repaint the widget
*/
void PDoublePointLed::drawLed(QPainter *painter)
{
    int w,h,m;
    QColor color;

    painter->save( );
    w=this->width();
    h=this->height();
    m=QMIN(w,h);

    if(s == On)
        color = OnColor;
    else
        color = OffColor;

    QBrush brush(color,Qt::SolidPattern);
    painter->setBrush(brush);
    painter->setPen(color);
    painter->drawPie(0,0,m-1,m-1,0,5760);
    painter->setPen(Qt::white);
    painter->drawArc(0,0,m-1,m-1,0,5760);
    painter->drawArc(2,2,m-5,m-5,0,5760);
    painter->setPen(Qt::darkGray);
    painter->drawArc(2,2,m-5,m-5,3600,2880);
    painter->drawArc(0,0,m-1,m-1,3600,2880);
    painter->setPen(Qt::white);
    painter->drawArc(m/4,m/4,m/2,m/2,1440,1440);
    painter->restore();
}

/*!
  \brief Set On Color
  \param newColor New Color
*/
void PDoublePointLed::setOnColor(QColor newColor)
{
    OnColor = newColor ;
    update();
}

/*!
  \brief Set Off Color
  \param newColor New Color
*/
void PDoublePointLed::setOffColor(QColor newColor)
{
    OffColor = newColor;
    update();
}

/*!
  \brief Mouse Release Event management
*/
void PDoublePointLed::mouseReleaseEvent(QMouseEvent * e)
{
	if(e->button() == Qt::RightButton)
	{
		QString name = this->name();

		emit RightClicked(QString("PDoublePointLed"), name);
	}
}

void PDoublePointLed::startFlash()
{
   if(timer == NULL)
   {
	    isFlashing = true;
		timer = new QTimer(this);
		timer->start(1000);
		connect(timer,SIGNAL(timeout()), this, SLOT(timerSlot()));
   }
}

void PDoublePointLed::stopFlash()
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

void PDoublePointLed::timerSlot()
{
	toggleState();
}
