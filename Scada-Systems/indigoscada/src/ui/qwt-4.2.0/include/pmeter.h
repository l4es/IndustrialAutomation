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

#ifndef PMETER_H
#define PMETER_H

#include <qwidget.h>
#include <qpainter.h>
#include "qwt_global.h"

/*!
  \brief The Meter Widget.

  \image html PMeter.png
*/
class QWT_EXPORT PMeter: public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int intValue READ getIntValue WRITE setValue);
    Q_PROPERTY(double Value READ getValue WRITE setValue);
    Q_PROPERTY(int intMinValue READ getIntMinValue WRITE setMinValue);
    Q_PROPERTY(double minValue READ getMinValue WRITE setMinValue);
    Q_PROPERTY(int intMaxValue READ getIntMaxValue WRITE setMaxValue);
    Q_PROPERTY(double maxValue READ getMaxValue WRITE setMaxValue);
    Q_PROPERTY(int Precision READ getPrecision WRITE setPrecision);

public:
    PMeter(QWidget *parent=0,const char *name=0);
    /*!
      \return current value in integer format
    */
    int getIntValue( ) const
    {
        return (int)(value < 0 ? value - 0.5 : value + 0.5);
    }
    /*!
      \return current value in double format
    */
    double getValue( ) const
    {
        return value;
    }
    /*!
      \return current minimum value in integer format
    */

    int getIntMinValue( ) const
    {
        return (int)(minValue < 0 ? minValue - 0.5 : minValue + 0.5);
    }
    /*!
      \return current minimum value in double format
    */
    double getMinValue( ) const
    {
        return minValue;
    }
    /*!
      \return current maximum value in integer format
    */
    int getIntMaxValue( ) const
    {
        return (int)(maxValue < 0 ? maxValue - 0.5 : maxValue + 0.5);
    }
    /*!
      \return current maximum value in double format
    */
    double getMaxValue( ) const
    {
        return maxValue;
    }
    /*!
      \return current precision value
    */
    int getPrecision( ) const
    {
        return precision;
    }
    /*!
      \return size hint
    */
    QSize sizeHint() const
    {
        return minimumSizeHint();
    }
    /*!
      \return minimum size hint
    */
    QSize minimumsizeHint() const
    {
        return QSize(120,120);
    }


public slots:
    int setValue(int);
    int setMinValue(int);
    int setMaxValue(int) ;
    int setValue(double);
    int setMinValue(double);
    int setMaxValue(double) ;
    int setPrecision(int);

protected:
    void paintEvent(QPaintEvent *);
    double value;
    double minValue;
    double maxValue;
    int precision;
    void drawMeter(QPainter *painter);
};
#endif
