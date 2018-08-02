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

#ifndef PTANK_H
#define PTANK_H

#include <qwidget.h>
#include <qpainter.h>
#include <qwidgetplugin.h>
#include "qwt_global.h"

/*!
  \brief The Tank Widget plugin.

  \image html ptank.png
*/
class QWT_EXPORT PTank : public QWidget
{
    Q_OBJECT
    Q_ENUMS( BorderStyle )
    Q_PROPERTY(double Value READ getValue WRITE setValue )
    Q_PROPERTY(double minvalue READ getMinValue WRITE setMinValue )
    Q_PROPERTY(double maxValue READ getMaxValue WRITE setMaxValue )
    Q_PROPERTY(int Precision READ getPrecision WRITE setPrecision )
    Q_PROPERTY(int numTicks READ getNumTicks WRITE setNumTicks )
    Q_PROPERTY(double Threshold READ getThreshold WRITE setThreshold)
    Q_PROPERTY(BorderStyle Style READ getBorderStyle WRITE setBorderStyle)
    Q_PROPERTY(QColor tankColor READ getColor WRITE setColor);

public:

    /*!
       \brief BorderStyle can be Flat,Frame,Raised,Sunken.

    */
    enum BorderStyle {Flat,Frame,Raised,Sunken};
    PTank(QWidget *parent=0,const char *name=0);
    /*!
      \return current value
    */
    double getValue() const
    {
        return dValue;
    }

    /*!
      \return current min value
    */
    double getMinValue() const
    {
        return dMin;
    }
    /*!
      \return current max value
    */
    double getMaxValue() const
    {
        return dMax;
    }
    /*!
      \return current precision
    */
    int    getPrecision() const
    {
        return precision;
    }
    /*!
      \return current number of ticks
    */
    int    getNumTicks() const
    {
        return numTicks;
    }
    /*!
      \return current tank indicator color
    */
    QColor getColor () const
    {
        return indicatorColor;
    }
    /*!
      \return current threshold value
    */
    double getThreshold() const
    {
        return dThreshold;
    }
    /*!
      \return current border style
      \sa BorderStyle
    */
    BorderStyle getBorderStyle() const
    {
        return borderStyle;
    }
    /*!
      \return size hint
    */
    QSize sizeHint();
    /*!
      \return minimum size hint
    */
    QSize minimumSizeHint( );

public slots:
    int setValue(double newValue);
    int setMinValue(double newMinValue);
    int setMaxValue(double newMaxValue);
    int setPrecision(int newPrecision);
    int setNumTicks(int newNumTicks);
    int setThreshold(double newThreshold);
    int setBorderStyle(BorderStyle newBorderStyle);
    void setColor(QColor newColor);

signals:
    void OutOfRange(double value);
    void ThresholdEvent(double value);

    /*!
      \brief This signal reports when the led is right clicked
    */
	void RightClicked(QString &class_name, QString &name);

protected:
    void paintEvent(QPaintEvent *);
    void drawTank(QPainter *painter);
    double dValue,dMin,dMax;
    double dThreshold ;
    int precision;
    int numTicks;
    BorderStyle borderStyle;
    QColor indicatorColor;
	void mouseReleaseEvent(QMouseEvent *);
};

#endif
