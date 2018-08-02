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

#ifndef PDOUBLEPOINTLED_H
#define PDOUBLEPOINTLED_H

#include <qwidget.h>
#include <qpainter.h>
#include <qwidgetplugin.h>
#include <qtimer.h>
#include "qwt_global.h"
/*!
  \brief The LED Widget.

  The LED widget imitates look and behaviour of a Double Point LED.

  \image html led_rg0000.png
*/

class QWT_EXPORT PDoublePointLed: public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor OnColor READ getOnColor WRITE setOnColor);
    Q_PROPERTY(QColor OffColor READ getOffColor WRITE setOffColor);

public:
    PDoublePointLed(QWidget *parent=0,const char *name=0);

	enum State { On, Off };
	void setState(State state) { s = state; repaint(); }
	void toggleState() { if (s == On) s = Off; else if (s == Off) s = On; repaint(); };
    
    /*!
      \return current on color
    */
    QColor getOnColor () const
    {
        return OnColor;
    }
    /*!
      \return current off color
    */
    QColor getOffColor () const
    {
        return OffColor;
    }
    /*!
      \return size hint
    */
    QSize sizeHint() const
    {
        return minimumSizeHint();
    }
    /*!
      \return a minimum size hint
    */
    QSize minimumSizeHint() const
    {
        return QSize(30,30);
    }

	bool GetFlash() const
    {
        return isFlashing;
    }

	void startFlash();
    void stopFlash();

public slots:
    void setOnColor(QColor);
    void setOffColor(QColor);
	void on() { setState(On); };
    void off() { setState(Off); };

protected slots:
   void timerSlot();

protected:
    void paintEvent(QPaintEvent *);
    QColor OnColor,OffColor;
    void drawLed(QPainter *painter);
	void mouseReleaseEvent(QMouseEvent *);
	QTimer *timer;
	State s;
	bool isFlashing;

signals:
    /*!
      \brief This signal reports when the led is right clicked
    */
	void RightClicked(QString &class_name, QString &name);

};

#endif //PDOUBLEPOINTLED_H
