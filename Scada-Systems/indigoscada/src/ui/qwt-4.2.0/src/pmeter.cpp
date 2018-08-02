/***************************************************************************
 *   Qt4Lab Plugins Library                                                *
 *   Copyright (C) 2004 by paolo sereno                                    *
 *   http://www.qt4lab.org                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <qwidget.h>
#include <qfontmetrics.h>
#include <math.h>
#include <qpixmap.h>
#include "pmeter.h"

/*!
  \brief Constructor
  \param parent Parent Widget
  \param name Name
*/
PMeter::PMeter(QWidget *parent, const char *name)
        : QWidget(parent,name)
{
    minValue=0.0;
    maxValue=10.0;
    value=0.0;
    precision=0;
    resize(minimumSizeHint());
}

/*!
  \brief Repaint the widget
*/
void PMeter::paintEvent(QPaintEvent *event)
{
    static QPixmap  pixmap;
    QRect rect = event->rect();

    QSize newSize = rect.size().expandedTo(pixmap.size());
    pixmap.resize(newSize);
    pixmap.fill(this,rect.topLeft());
    QPainter painter(&pixmap,this);
    painter.translate(-rect.x(),-rect.y());
    drawMeter(&painter);
    bitBlt(this,rect.x(),rect.y(),&pixmap,0,0,rect.width(),rect.height());
}

/*!
  \brief Repaint the widget
*/
void PMeter::drawMeter(QPainter *paint)
{
    paint->save();
    QRect v = paint->viewport();
    int w=this->width();
    int h=this->height();
    int d = QMIN( w, h );
    paint->setWindow( -d/2,-d/2,d,d);
    paint->setViewport( 0 + (this->width()-d)/2, 0 + (this->height()-d)/2, d, d );
    QBrush brush(Qt::black,Qt::SolidPattern);
    paint->setPen(Qt::black);
    paint->setBrush(brush);
    paint->drawPie(-d/2,-d/2,d,d,0,360*16);
    QBrush brush2(Qt::white,Qt::SolidPattern) ;
    paint->setBrush(brush2);
    paint->setPen(Qt::white);
    paint->save();
    int nSteps=10;
    paint->rotate(-270);
    int i=0;
    for ( i=0; i<=nSteps;i++ )
    {
        paint->drawLine(d/4 ,0,d/3 ,0 );
        paint->rotate( 30 );
    }
    paint->restore() ;
    double sina,cosa;
    for ( i=0; i<=nSteps; i++ )
    {
        sina=-sin((double)i*0.5235988);
        cosa=cos((double)i*0.5235988);
        double tmpVal=i*((maxValue-minValue)/nSteps);
        QString str = QString( "%1" )
                      .arg(tmpVal,0,'f',precision);
        QFontMetrics fm( this->font() );
        int w=fm.size(SingleLine,str).width();
        int h=fm.size(SingleLine,str).height();
        paint->drawText((int)(d*0.4*sina)-w/2,(int)(d*0.4*cosa)+h/2,str);
    }

    QPointArray pts;
    pts.setPoints( 4, -3,0,  0,-3, d/4,0, 0,3 );

    // display current value
    int degRotate = (int)(-270 + (300/(maxValue - minValue)) * (value - minValue));
    paint->rotate( degRotate );
    paint->drawConvexPolygon( pts );
    paint->restore();
}

/*!
  \brief set widget value (int)
*/
int PMeter::setValue(int newValue)
{
    int retCode=0;

    if((double)newValue>=minValue && newValue<=maxValue)
    {
        value=newValue;
        update();
    }
    else
        retCode=-1;
    return retCode;
}

/*!
  \brief set widget value (double)
*/
int PMeter::setValue(double newValue)
{
    int retCode=0;

    if(newValue>=minValue && newValue<=maxValue)
    {
        value=newValue;
        update();
    }
    else
        retCode=-1;
    return retCode;
}

/*!
  \brief set minimum value (int)
*/
int PMeter::setMinValue(int newValue)
{
    minValue=(double)newValue;
    update();
    return 0;
}

/*!
  \brief set minimum value (double)
*/
int PMeter::setMinValue(double newValue)
{
    minValue=newValue;
    update();
    return 0;
}

/*!
  \brief set maximum value (int)
*/
int PMeter::setMaxValue(int newValue)
{
    maxValue=(double)newValue;
    update();
    return 0;
}

/*!
  \brief set maximum value (double)
*/
int PMeter::setMaxValue(double newValue)
{
    maxValue=newValue;
    update();
    return 0;
}

int PMeter::setPrecision(int newPrecision)
{
    int retCode=0;

    if(newPrecision>=0 && newPrecision <=2)
        precision=newPrecision;
    else
        retCode=-1;

    return retCode;
}
