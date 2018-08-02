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

#ifndef ELSWITCH_H
#define ELSWITCH_H

#include <qwidget.h>
#include <qpainter.h>
#include <qwidgetplugin.h>
#include "qwt_global.h"

/*!
  \brief The Electrical Switch Widget.

  The switch widget imitates look and behaviour of an electrical switch.
  
*/

class QWT_EXPORT Breaker: public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool switchValue READ getBreakerValue WRITE setBreakerValue);
    Q_PROPERTY(QString OnLabel READ getOnLabel WRITE setOnLabel);
    Q_PROPERTY(QString OffLabel READ getOffLabel WRITE setOffLabel);

public:
    Breaker(QWidget *parent=0,const char *name=0);
    /*!
      \return current switch value
    */
    bool getBreakerValue() const
    {
        return value;
    }

    /*!
      \return current switch on label
    */
    QString getOnLabel() const
    {
        return OnString;
    }

    /*!
      \return current switch off label
    */
    QString getOffLabel() const
    {
        return OffString;
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
        return QSize(60,60);
    }

	void undoToggle();

public slots:
    void setOnLabel(QString);
    void setOffLabel(QString);
    void setBreakerValue(bool);
	void setBreakerValueInvalid(bool);

signals:
    /*!
      \brief This signal reports when the switch is toggled
    */
    void valueChanged(bool);
	//void RightClicked(void);
	void RightClicked(QString &class_name, QString &name);

protected:
    void paintEvent(QPaintEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void keyReleaseEvent(QKeyEvent *);
    QString OnString,OffString;
    bool value;
    void drawSwitch();
    void toggleValue();
	void toggleValueManually();
};

#endif //ELSWITCH_H
