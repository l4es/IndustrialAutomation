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
#include <qpainter.h>
#include "ptank.h"

/*!
  \brief Constructor
  \param parent Parent Widget
  \param name Name
*/
PTank::PTank(QWidget *parent, const char *name)
        : QWidget(parent,name)
{
    // set default values
    dMin=-10.0;
    dThreshold=0.0;
    dMax=10.0;
    precision=2;
    dValue=0.0;
    numTicks=4 ;
    borderStyle=Frame;
    indicatorColor=Qt::blue;
    resize(minimumSizeHint());
}

/*!
  \brief set minimum value
*/
int PTank::setMinValue(double newMinValue)
{
    dMin=newMinValue;
    if(dThreshold<dMin)
        dThreshold=dMin;
    update();
    return 0;
}

/*!
  \brief set widget value
*/
int PTank::setValue(double newValue)
{
    int retCode=0;
//    if(newValue<dMin || newValue>dMax)
//    {
//        emit OutOfRange(newValue);
//        retCode=-1;
//    }
//    else
//    {
        if(dValue>dThreshold && newValue<=dThreshold)
            emit ThresholdEvent(newValue);
        dValue=newValue;
        update();
//    }
    return (retCode);
}

/*!
  \brief set maximum value
*/
int PTank::setMaxValue(double newMaxValue)
{
    int retCode=0;
    if(newMaxValue<dMin)
    {
        retCode=-1;
    }
    else
    {
        dMax=newMaxValue;
        if(dValue>dMax)
            emit OutOfRange(dValue);
        if(dThreshold>dMax)
            dThreshold=dMax;
        update();
    }
    return retCode;
}

/*!
  \brief set widget border style.
  \sa BorderStyle
*/
int PTank::setBorderStyle(BorderStyle newBorderStyle)
{
    //check input??
    borderStyle=newBorderStyle;
    update();
    return 0;
}

/*!
  \brief set Precision (digits after zero)
*/
int PTank::setPrecision(int newPrecision)
{
    int retCode=0;
    if(newPrecision<0 || newPrecision>6)
    {
        retCode=-1;
    }
    else
    {
        precision=newPrecision;
        update();
    }
    return retCode;
}

/*!
  \brief set Number of Ticks
*/
int PTank::setNumTicks(int newNumTicks)
{
    int retCode=0;
    if(newNumTicks<0)
    {
        retCode=-1;
    }
    else
    {
        numTicks=newNumTicks;
        update();
    }
    return retCode;
}

/*!
  \brief set Threshold (Min Value <= Threshold <= Max Value)
*/
int PTank::setThreshold(double newThreshold)
{
    int retCode=0;
    if(newThreshold<dMin || newThreshold>dMax)
        retCode=-1;
    else
    {
        dThreshold=newThreshold;
        if(dValue<=dThreshold)
            emit ThresholdEvent(dValue);
        update() ;
    }
    return retCode;
}

/*!
  \brief Repaint the widget
*/
void PTank::paintEvent(QPaintEvent *event)
{

    static QPixmap  pixmap;
    QRect rect = event->rect();

    QSize newSize = rect.size().expandedTo(pixmap.size());
    pixmap.resize(newSize);
    pixmap.fill(this,rect.topLeft());
    QPainter painter(&pixmap,this);
    painter.translate(-rect.x(),-rect.y());
    drawTank(&painter);
    bitBlt(this,rect.x(),rect.y(),&pixmap,0,0,rect.width(),rect.height());
}

/*!
  \brief Repaint the widget
*/
void PTank::drawTank(QPainter *painter)
{
    // double buffer to avoid flikering...
    int w,h,strLen,strHeight;

    painter->save();
    w=this->width();
    h=this->height();

    switch(borderStyle)
    {
    case Flat:
        break;

    case Frame: //to be modified
        painter->setPen(Qt::white);
        painter->drawLine(0,0,w-1,0);
        painter->drawLine(0,0,0,h-1);
        painter->drawLine(2,h-2,w-2,h-2);
        painter->drawLine(w-2,h-2,w-2,1);
        painter->setPen(Qt::darkGray);
        painter->drawLine(2,2,w-4,2);
        painter->drawLine(2,2,2,h-4);
        painter->drawLine(2,h-1,w-2,h-1);
        painter->drawLine(w-1,h-1,w-1,0);
        break;


    case Raised:
        painter->setPen(Qt::darkGray);
        painter->drawRect(0,0,w-1,h-1);
        painter->setPen(Qt::white) ;
        painter->drawLine(0,0,w-1,0);
        painter->drawLine(0,0,0,h-1) ;
        break;

    case Sunken:
        painter->setPen(Qt::white);
        painter->drawRect(0,0,w-1,h-1);
        painter->setPen(Qt::darkGray);
        painter->drawLine(0,0,w-1,0);
        painter->drawLine(0,0,0,h-1);
        break;

    default:
        break;
    }

    int deltaPixel=h*3/5-1;
    //upper side
    painter->setPen(Qt::darkGray) ;
    painter->drawLine(w/2+w/4,h/10,w/2+w/4,h*3/5+h/10);
    painter->drawArc(w/2-w/4,h/10-w/16,w/2,w/8,0*16,45*16);

    //painter->drawLine(w/2-w/4,h*3/5+h/10,w/2+w/4,h*3/5+h/10); //cancel??

    painter->setPen(Qt::white);
    painter->drawArc(w/2-w/4,h/10-w/16,w/2,w/8,45*16,135*16);
    painter->drawLine(w/2-w/4,h/10,w/2-w/4,h*3/5+h/10);


    //down side
    QBrush brush=QBrush(indicatorColor, SolidPattern);
    painter->setBrush(brush);
    painter->setPen(indicatorColor);
    painter->drawPie(w/2-w/4+1,h/10+(h*3/5)-w/16,w/2-1,w/8,180*16,180*16);


    //indicator
    int valPixel=(int)( (double)deltaPixel/(dMax-dMin) *(dValue-dMin) );
    if(valPixel>h*3/5+h/10)
        valPixel=h*3/5+h/10;
    painter->fillRect(w/2-(w/4)+1,h*3/5+h/10-valPixel,w/2-1,valPixel,brush);

    //ticks
    //top value
    QFontMetrics fm( this->font() );
    painter->setPen(Qt::white);
    painter->drawLine(w/2-(w/4)-(w/20),h/10-1,w/2-(w/4)-1,h/10-1) ;
    painter->setPen(Qt::black);
    painter->drawLine(w/2-(w/4)-(w/20),h/10,w/2-(w/4)-1,h/10);
    QString Max = QString("%1") .arg(dMax,0,'f',precision);
    strLen=fm.size(SingleLine,Max).width();
    painter->drawText(w/2-(w/4)-(w/20)-strLen,h/10,Max);


    //for each tick in numTicks
    int tick;
    double step;
    QString val;

    int stepPixel=deltaPixel/numTicks;

    for(tick=1;tick<numTicks;tick++)
    {
        step=(dMax-dMin)/numTicks;
        painter->setPen(Qt::white);
        painter->drawLine(w/2-(w/4)-(w/20),h*3/5+h/10-1-(stepPixel*tick)-1,w/2-(w/4)-1,h*3/5+h/10-1-(stepPixel*tick)-1);
        painter->setPen(Qt::black);
        painter->drawLine(w/2-(w/4)-(w/20),h*3/5+h/10-1-(stepPixel*tick),w/2-(w/4)-1,h*3/5+h/10-1-(stepPixel*tick) );
        val.setNum((dMin+(step)*tick),'f',precision);
        strLen=fm.size(SingleLine,val).width();
        painter->drawText(w/2-(w/4)-(w/20)-strLen,h*3/5+h/10-1-(stepPixel*tick),val);
    }

    //bottom value
    painter->drawLine(w/2-(w/4)-(w/20),h*3/5+h/10-1,w/2-(w/4)-1,h*3/5+h/10-1);
    painter->setPen(Qt::white);
    painter->drawLine(w/2-(w/4)-(w/20),h*3/5+h/10-2,w/2-(w/4)-1,h*3/5+h/10-2);
    painter->setPen(Qt::black);
    QString Min = QString("%1") .arg(dMin,0,'f',precision);
    strLen=fm.size(SingleLine,Min).width();
    painter->drawText(w/2-(w/4)-(w/20)-strLen,h*3/5+h/10-1,Min);

    // scale
    painter->setPen(Qt::white);
    painter->drawLine(w/2-(w/4)-(w/40)-1,h/10,w/2-(w/4)-(w/40)-1,h*3/5+h/10-1);
    painter->setPen(Qt::black);
    painter->drawLine(w/2-(w/4)-(w/40),h/10,w/2-(w/4)-(w/40),h*3/5+h/10-1); //shadow

    // to be modified threshold
    painter->setPen(Qt::red);
    int thresholdPixel=(int)( (double)deltaPixel/(dMax-dMin) *(dThreshold-dMin) );
    if(thresholdPixel>h*3/5+h/10)
        thresholdPixel=h*3/5+h/10;
    painter->drawLine(w/2+(w/4)+1,h*3/5+h/10-thresholdPixel,w/2+(w/4)+(w/20),h*3/5+h/10-thresholdPixel);
    QString strThreshold = QString("%1") .arg(dThreshold,0,'f',precision);
    strLen=fm.size(SingleLine,strThreshold).width();
    painter->drawText(w/2+(w/4)+(w/20),h*3/5+h/10-thresholdPixel,strThreshold);

    // numeric
    QString strValue = QString("%1") .arg(dValue,0,'f',precision);
    strLen=fm.size(SingleLine,strValue).width();
    strHeight=fm.size(SingleLine,strValue).height();

    //draw a double rectangle with shadow
    painter->setPen(Qt::darkGray);
    painter->drawLine(w/2-w/4,h/10*9-strHeight/2-3,w/2+w/4,h/10*9-strHeight/2-3);
    painter->drawLine(w/2-w/4,h/10*9-strHeight/2-3,w/2-w/4,h/10*9+strHeight/2+3);

    painter->setPen(Qt::white);
    painter->drawLine(w/2-w/4,h/10*9+strHeight/2+3,w/2+w/4,h/10*9+strHeight/2+3);
    painter->drawLine(w/2+w/4,h/10*9-strHeight/2-3,w/2+w/4,h/10*9+strHeight/2+3);

    // write value
    painter->setPen(Qt::black);
    painter->drawText(w/2-strLen/2,h/10*9+strHeight/2-2,strValue);
    painter->restore();
}


QSize PTank::sizeHint()
{
    return minimumSizeHint();
}


QSize PTank::minimumSizeHint()
{
    return QSize(130,160);
}

/*!
  \brief Set indicator Color
  \param newColor New Color
*/
void PTank::setColor(QColor newColor)
{
    indicatorColor=newColor ;
    update();
}


/*!
  \brief Mouse Release Event management
*/
void PTank::mouseReleaseEvent(QMouseEvent * e)
{
	if(e->button() == Qt::RightButton)
	{
		QString name = this->name();

		emit RightClicked(QString("PTank"), name);
	}
}