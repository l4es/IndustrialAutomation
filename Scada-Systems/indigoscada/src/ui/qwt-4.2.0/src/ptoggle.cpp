/**************************************************************************
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
#include "ptoggle.h"

/*!
  \brief Constructor
  \param parent Parent Widget
  \param name Name
*/
PToggle::PToggle(QWidget *parent, const char *name)
        : QWidget(parent,name)
{
    value=false;
    setFocusPolicy(QWidget::StrongFocus);
}

/*!
  \brief Repaint the widget
*/
void PToggle::paintEvent(QPaintEvent *)
{
    drawSwitch();
}

/*!
  \brief Mouse Release Event management
*/
void PToggle::mouseReleaseEvent(QMouseEvent *)
{
    toggleValue();
    update();
}

/*!
  \brief Key Release Event management
*/
void PToggle::keyReleaseEvent(QKeyEvent *e)
{
    int keyPressed=e->key();

    if(keyPressed==Qt::Key_Return || keyPressed==Qt::Key_Space)
    {
        toggleValue();
        update();
    }
}

/*!
  \brief Repaint the widget
*/
void PToggle::drawSwitch()
{
    QPainter painter(this);
    int w=this->width();
    int h=this->height();
    int min=QMIN(w,h);
    int m=min*4/5;
    int mm=min/5;
    int wm=min/16;
    int hm=wm;

    painter.eraseRect(0,0,m-1,m-1);

    painter.setPen(Qt::white);
    painter.drawLine(mm,mm,m-1,mm);
    painter.drawLine(mm,mm,mm,m-1);
    painter.setPen(Qt::darkGray);
    painter.drawLine(m-1,mm,m-1,m-1);
    painter.drawLine(mm,m-1,m-1,m-1);

    if(hasFocus())
    {
        QPen pen(Qt::lightGray,Qt::DotLine);
        painter.setPen(pen);
        painter.drawWinFocusRect(0,0,w-1,h-1,Qt::lightGray);
    }

    QPointArray a(4);
    if(value)
    {
        painter.setPen(Qt::white);
        painter.drawLine(wm+mm,hm+mm,m-wm-1,hm+mm);
        painter.drawLine(wm+mm,hm+mm,wm+mm,m-hm-1);
        painter.drawLine(wm+1+mm,hm+1+mm,m-wm-2,hm+1+mm);
        painter.drawLine(wm+1+mm,hm+1+mm,wm+1+mm,m-hm-2);
        painter.setPen(Qt::darkGray);
        painter.drawLine(m-wm-1,hm+mm,m-wm-1,m-hm-1);
        painter.drawLine(wm+mm,m-hm-1,m-wm-1,m-hm-1);
        painter.drawLine(m-wm-2,hm+mm+1,m-wm-2,m-hm-2);
        painter.drawLine(wm+mm+1,m-hm-2,m-wm-2,m-hm-2);
    }
    else
    {
        int deep=m/6;
        painter.setPen(Qt::white);
        QBrush brush(Qt::white,Qt::SolidPattern);
        painter.setBrush(brush);
        a.setPoint(0,wm+mm,hm+mm);
        a.setPoint(1,m-wm-1,hm+mm);
        a.setPoint(2,m-wm-deep,hm+mm+deep);
        a.setPoint(3,wm+mm-deep,hm+mm+deep);
        painter.drawPolygon(a);
        painter.setPen(Qt::white);
        brush.setColor(paletteBackgroundColor());
        painter.setBrush(brush);
        a.setPoint(0,m-wm-deep,hm+mm+deep);
        a.setPoint(1,wm+mm-deep,hm+mm+deep);
        a.setPoint(2,wm+mm-deep,m-hm+deep);
        a.setPoint(3,m-wm-deep,m-hm+deep);
        painter.drawPolygon(a);
        painter.setPen(Qt::darkGray);
        painter.drawLine(wm+mm-deep,m-hm+deep,m-wm-deep-1,m-hm+deep);
        brush.setColor(Qt::darkGray);
        painter.setBrush(brush);
        a.setPoint(0,m-wm-1,hm+mm);
        a.setPoint(1,m-wm-deep-1,hm+mm+deep);
        a.setPoint(2,m-wm-deep-1,m-hm+deep);
        a.setPoint(3,m-wm-1,m-hm);
        painter.drawPolygon(a);
    }
}

/*!
  \brief Toggle the switch value
*/
void PToggle::toggleValue()
{
    if(isEnabled())
    {
        if (value)
            value=false;
        else
            value=true;
        emit valueChanged(value);
    }
}

/*!
  \brief Set the switch value
*/
void PToggle::setPToggleValue(bool newValue)
{
    if(getPToggleValue()!=newValue)
    {
        toggleValue();
        update();
    }
}
