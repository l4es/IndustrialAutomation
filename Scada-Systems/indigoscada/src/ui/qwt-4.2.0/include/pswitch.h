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

#ifndef PSWITCH_H
#define PSWITCH_H

#include <qwidget.h>
#include <qpainter.h>
#include <qwidgetplugin.h>
#include "qwt_global.h"

/*!
  \brief The Switch Widget.

  The Switch widget imitates look and behaviour of a Binary Switch.

  \image html pswitch.png
*/

class QWT_EXPORT PSwitch: public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool switchValue READ getPSwitchValue WRITE setPSwitchValue);
    Q_PROPERTY(QString OnLabel READ getOnLabel WRITE setOnLabel);
    Q_PROPERTY(QString OffLabel READ getOffLabel WRITE setOffLabel);

public:
    PSwitch(QWidget *parent=0,const char *name=0);
    /*!
      \return current switch value
    */
    bool getPSwitchValue() const
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
    void setPSwitchValue(bool);
	void setPSwitchValueInvalid(bool);

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

#endif
