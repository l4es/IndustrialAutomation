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

#ifndef PTOGGLE_H
#define PTOGGLE_H

#include <qwidget.h>
#include <qpainter.h>
#include <qwidgetplugin.h>
#include "qwt_global.h"

/*!
  \brief The Toggle Widget.

  The Toggle widget imitates look and behaviour of a Toggle Switch.

  \image html ptoggle.png
*/

class QWT_EXPORT PToggle: public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool toggleValue READ getPToggleValue WRITE setPToggleValue);

public:
    PToggle(QWidget *parent=0,const char *name=0);
    /*!
      \return current toggle switch value
    */
    bool getPToggleValue() const
    {
        return value;
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

public slots:
    void setPToggleValue(bool);

signals:
    /*!
      \brief This signal reports when the switch is toggled
    */
    void valueChanged(bool);

protected:
    void paintEvent(QPaintEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void keyReleaseEvent(QKeyEvent *);
    bool value;
    void drawSwitch();
    void toggleValue();
};

#endif
