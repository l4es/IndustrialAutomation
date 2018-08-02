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
#include "pswitch.h"

/*!
  \brief Constructor
  \param parent Parent Widget
  \param name Name
*/
PSwitch::PSwitch(QWidget *parent, const char *name)
        : QWidget(parent,name)
{
    OnString="on";
    OffString="off";
    QFontMetrics fm( this->font() );
    int maxwidth=QMAX( fm.size(SingleLine,OnString).width(), fm.size(SingleLine,OffString).width());
    setGeometry(0,0,maxwidth+70,80);
    value=false;
    setFocusPolicy(QWidget::StrongFocus);
}

/*!
  \brief Repaint the widget
*/
void PSwitch::paintEvent(QPaintEvent *)
{
    drawSwitch();
}

/*!
  \brief Mouse Release Event management
*/
void PSwitch::mouseReleaseEvent(QMouseEvent * e)
{
	if(e->button() == Qt::RightButton)
	{
		//emit RightClicked();
		QString name = this->name();

		emit RightClicked(QString("PSwitch"), name);
	}
	else
	{
		toggleValueManually();
		update();
	}
}

/*!
  \brief Key Release Event management
*/
void PSwitch::keyReleaseEvent(QKeyEvent *e)
{
    int keyPressed=e->key();

    if(keyPressed==Qt::Key_Return || keyPressed==Qt::Key_Space)
    {
        toggleValueManually();
        update();
    }
}

/*!
  \brief Repaint the widget
*/
void PSwitch::drawSwitch()
{
    QPainter painter(this);
    int w=this->width();
    int h=this->height();
    int m=QMIN(w,h);
    int m_1_4 =(int)m/4;
    int m_1_3 =(int)m/3;
    int m_1_2=(int)m/2;
    int m_6_8=(int)m*(int)6/8;
    int m_4_9=(int)m*(int)4/9;
    int m_5_9=(int)m*(int)5/9;
    int m_3_9=(int)m*(int)3/9;
    int m_2_9=(int)m*(int)2/9;
    int m_7_9=(int)m*(int)7/9;
    int m_1_11=(int)m*(int)1/11;
    int m_2_11=(int)m*(int)2/11;
    int m_9_11=(int)m*(int)9/11;
    int m_10_11=(int)m*(int)10/11;
    painter.eraseRect(0,0,w-1,h-1);

    if(hasFocus())
    {
        QPen pen(Qt::lightGray,Qt::DotLine);
        painter.setPen(pen);
        painter.drawWinFocusRect(0,0,w-1,h-1,Qt::lightGray);
    }

    painter.setPen(Qt::white);
    painter.drawArc(m_1_4,m_1_4,m_1_2,m_1_2,0,5670);
    painter.drawArc(m_1_3+1,m_1_3,m_1_3,m_1_3,0,5670);
    painter.setPen(Qt::black);
    painter.drawArc(m_1_4,m_1_4,m_1_2,m_1_2,3400,3000);
    painter.drawArc(m_1_3+1,m_1_3,m_1_3,m_1_3,3400,3000);

    if(isEnabled())
        painter.setPen(Qt::black);
    else
        painter.setPen(Qt::darkGray);

    painter.drawText(m_7_9-2,m_3_9,OnString);
    painter.drawText(m_7_9-2,m_7_9,OffString);

    QPointArray a(4);
    if(value)
    {
        painter.setPen(Qt::darkGray);
        a.setPoint(0,m_4_9,m_1_2);
        a.setPoint(1,m_5_9,m_1_2);
        a.setPoint(2,m_7_9,m_2_11);
        a.setPoint(3,m_2_9,m_2_11);
        QBrush brush3(Qt::lightGray,Qt::SolidPattern);
        painter.setBrush(brush3);
        painter.drawPolygon(a,false);
        a.setPoint(0,m_1_4,m_1_11);
        a.setPoint(1,m_6_8,m_1_11);
        a.setPoint(2,m_7_9,m_2_11);
        a.setPoint(3,m_2_9,m_2_11);
        QBrush brush4(Qt::darkGray,Qt::SolidPattern);
        painter.setBrush(brush4);
        painter.drawPolygon(a,true);
    }
    else
    {
        painter.setPen(Qt::darkGray);
        a.setPoint(0,m_4_9,m_1_2);
        a.setPoint(1,m_5_9,m_1_2);
        a.setPoint(2,m_7_9,m_9_11);
        a.setPoint(3,m_2_9,m_9_11);
        QBrush brush(Qt::lightGray,Qt::SolidPattern);
        painter.setBrush(brush);
        painter.drawPolygon(a,false);
        a.setPoint(0,m_1_4,m_10_11);
        a.setPoint(1,m_6_8,m_10_11);
        a.setPoint(2,m_7_9,m_9_11);
        a.setPoint(3,m_2_9,m_9_11);
        QBrush brush2(Qt::darkGray,Qt::SolidPattern);
        painter.setBrush(brush2);
        painter.drawPolygon(a,true);
    }
}

/*!
  \brief Toggle the switch value
*/
void PSwitch::toggleValue()
{
    if(isEnabled())
    {
        if (value)
            value=false;
        else
            value=true;
        //emit valueChanged(value);
    }
}

/*!
  \brief Toggle the switch value manually
*/
void PSwitch::toggleValueManually()
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


void PSwitch::undoToggle()
{
    if(isEnabled())
    {
        if (value)
            value=false;
        else
            value=true;
    }
}


/*!
  \brief Set the switch value
*/
void PSwitch::setPSwitchValue(bool newValue)
{
	setEnabled(true);

    if(getPSwitchValue()!=newValue)
    {
        toggleValue();
        update();
    }
}

/*!
  \brief Set the switch value as invalid
*/
void PSwitch::setPSwitchValueInvalid(bool newValue)
{
	setEnabled(false);
}

/*!
  \brief Set the On label
*/
void PSwitch::setOnLabel(QString onstring)
{
    QRect rect;
    OnString=onstring;
    rect= geometry();
    QFontMetrics fm( this->font() );
    int maxwidth=QMAX( fm.size(SingleLine,OnString).width(), fm.size(SingleLine,OffString).width());
    setGeometry(rect.x(),rect.y(),maxwidth+70,80);
    update();
}

/*!
  \brief Set the Off label
*/
void PSwitch::setOffLabel(QString offstring)
{
    QRect rect;
    OffString=offstring;
    rect= geometry();
    QFontMetrics fm( this->font() );
    int maxwidth=QMAX( fm.size(SingleLine,OnString).width(), fm.size(SingleLine,OffString).width());
    setGeometry(rect.x(),rect.y(),maxwidth+70,80);
    update();
}
