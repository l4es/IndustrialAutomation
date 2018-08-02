/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2013 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */

#include <qwidget.h>
#include <qfontmetrics.h>
#include "elswitch.h"

/*!
  \brief Constructor
  \param parent Parent Widget
  \param name Name
*/
Breaker::Breaker(QWidget *parent, const char *name)
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
void Breaker::paintEvent(QPaintEvent *)
{
    drawSwitch();
}

/*!
  \brief Mouse Release Event management
*/
void Breaker::mouseReleaseEvent(QMouseEvent * e)
{
	if(e->button() == Qt::RightButton)
	{
		//emit RightClicked();
		QString name = this->name();

		emit RightClicked(QString("Breaker"), name);
	}
	//else
	//{
	//	toggleValueManually();
	//	update();
	//}
}

/*!
  \brief Key Release Event management
*/
void Breaker::keyReleaseEvent(QKeyEvent *e)
{
//    int keyPressed=e->key();

//    if(keyPressed==Qt::Key_Return || keyPressed==Qt::Key_Space)
//    {
//        toggleValueManually();
//        update();
//    }
}

#include "breaker_closed.xpm"
#include "breaker_opened.xpm"

/*!
  \brief Repaint the widget
*/
void Breaker::drawSwitch()
{
	QPainter painter(this);
	
	if(value)
	{
		painter.begin(this);
		painter.drawPixmap(0, 0, QPixmap((const char **)breaker_closed_xpm));
		painter.setFont(QFont("helvetica", 10));
		painter.setPen(black);
		//painter.drawText(0, 0, width(), height()-2,
        //       AlignLeft|AlignBottom,
        //         "State is:");
                 
		//painter.drawText(0, 0, width(), height()-2,
        //         AlignRight|AlignBottom,
        //         "Closed");
		painter.end();
	}
	else
	{
		painter.begin(this);
		painter.drawPixmap(0, 0, QPixmap((const char **)breaker_opened_xpm));
		painter.setFont(QFont("helvetica", 10));
		painter.setPen(black);
		//painter.drawText(0, 0, width(), height()-2,
        //       AlignLeft|AlignBottom,
        //         "State is:");
                 
		//painter.drawText(0, 0, width(), height()-2,
        //         AlignRight|AlignBottom,
        //         "Opened");
		painter.end();
	}
}

/*!
  \brief Toggle the switch value
*/
void Breaker::toggleValue()
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
void Breaker::toggleValueManually()
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


void Breaker::undoToggle()
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
void Breaker::setBreakerValue(bool newValue)
{
	setEnabled(true);

    if(getBreakerValue()!=newValue)
    {
        toggleValue();
        update();
    }
}

/*!
  \brief Set the switch value as invalid
*/
void Breaker::setBreakerValueInvalid(bool newValue)
{
	setEnabled(false);
}

/*!
  \brief Set the On label
*/
void Breaker::setOnLabel(QString onstring)
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
void Breaker::setOffLabel(QString offstring)
{
    QRect rect;
    OffString=offstring;
    rect= geometry();
    QFontMetrics fm( this->font() );
    int maxwidth=QMAX( fm.size(SingleLine,OnString).width(), fm.size(SingleLine,OffString).width());
    setGeometry(rect.x(),rect.y(),maxwidth+70,80);
    update();
}
