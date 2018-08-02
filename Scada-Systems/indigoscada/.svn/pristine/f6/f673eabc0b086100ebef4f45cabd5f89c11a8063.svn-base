/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_THERMO_H
#define QWT_THERMO_H

#include <qwidget.h>
#include <qcolor.h>
#include <qfont.h>
#include <qrect.h>
#include "qwt_global.h"
#include "qwt_dimap.h"
#include "qwt_scldraw.h"
#include "qwt_sclif.h"


/*!
  \brief The Thermometer Widget

  QwtThermo is a widget which displays a value in an interval. It supports:
  - a horizontal or vertical layout;
  - a range;
  - a scale;
  - an alarm level.

  By default, the scale and range run over the same interval of values.
  QwtScaleIf::setScale() changes the interval of the scale and allows
  easy conversion between physical units.
  The example shows how to make the scale indicate in degrees Fahrenheit and
  to set the value in degrees Kelvin:
\code
#include <qapplication.h>
#include <qwt_thermo.h>

double Kelvin2Fahrenheit(double kelvin)
{
    // see http://en.wikipedia.org/wiki/Kelvin
    return 1.8*kelvin - 459.67;
}

int main(int argc, char **argv)
{
    const double minKelvin = 0.0;
    const double maxKelvin = 500.0;

    QApplication a(argc, argv);
    QwtThermo t;
    t.setRange(minKelvin, maxKelvin);
    t.setScale(Kelvin2Fahrenheit(minKelvin), Kelvin2Fahrenheit(maxKelvin));
    // set the value in Kelvin but the scale displays in Fahrenheit
    // 273.15 Kelvin = 0 Celsius = 32 Fahrenheit
    t.setValue(273.15);
    a.setMainWidget(&t);
    t.show();
    return a.exec();
}
\endcode

  \todo Improve the support for a logarithmic range and/or scale. 
*/
class QWT_EXPORT QwtThermo: public QWidget, public QwtScaleIf
{
    Q_OBJECT

    Q_ENUMS( ScalePos )

    Q_PROPERTY( QBrush alarmBrush READ alarmBrush WRITE setAlarmBrush )
    Q_PROPERTY( QColor alarmColor READ alarmColor WRITE setAlarmColor )
    Q_PROPERTY( bool alarmEnabled READ alarmEnabled WRITE setAlarmEnabled )
    Q_PROPERTY( double alarmLevel READ alarmLevel WRITE setAlarmLevel )
    Q_PROPERTY( ScalePos scalePosition READ scalePosition
        WRITE setScalePosition )
    Q_PROPERTY( int borderWidth READ borderWidth WRITE setBorderWidth )
    Q_PROPERTY( QBrush fillBrush READ fillBrush WRITE setFillBrush )
    Q_PROPERTY( QColor fillColor READ fillColor WRITE setFillColor )
    Q_PROPERTY( double maxValue READ maxValue WRITE setMaxValue )
    Q_PROPERTY( double minValue READ minValue WRITE setMinValue )
    Q_PROPERTY( int pipeWidth READ pipeWidth WRITE setPipeWidth )
    Q_PROPERTY( double value READ value WRITE setValue )

public:
    /*
      Scale position. QwtSlider tries to enforce valid combinations of its
      orientation and scale position:
      - Qt::Horizonal combines with None, Top and Bottom
      - Qt::Vertical combines with None, Left and Right
      
      \sa QwtThermo::setOrientation, QwtThermo::setScalePosition
    */
    enum ScalePos {None, Left, Right, Top, Bottom};

    QwtThermo(QWidget *parent = 0, const char *name = 0);
    virtual ~QwtThermo();

    void setOrientation(Qt::Orientation o, ScalePos s);

    void setScalePosition(ScalePos s);
    ScalePos scalePosition() const;

    void setBorderWidth(int w);
    int borderWidth() const;

    void setFillBrush(const QBrush &b);
    const QBrush &fillBrush() const;

    void setFillColor(const QColor &c);
    const QColor &fillColor() const;
 
    void setAlarmBrush(const QBrush &b);
    const QBrush &alarmBrush() const;

    void setAlarmColor(const QColor &c);
    const QColor &alarmColor() const;

    void setAlarmLevel(double v);
    double alarmLevel() const;

    void setAlarmEnabled(bool tf);
    bool alarmEnabled() const;

    void setPipeWidth(int w);
    int pipeWidth() const;

    //! Set the maximum value.
    void setMaxValue(double v) { setRange(d_minValue, v); }
    //! Return the maximum value.
    double maxValue() const { return d_maxValue; }

    //! Set the minimum value.
    void setMinValue(double v) { setRange(v, d_maxValue); }
    //! Return the minimum value.
    double minValue() const { return d_minValue; }

    //! Return the value.
    double value() const { return d_value; }

    void setRange(double vmin, double vmax, bool lg = FALSE);
    void setMargin(int m);

    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;
    virtual QSizePolicy sizePolicy() const;

public slots:
    void setValue(double val);

signals:
    /*!
      \brief This signal reports when the thermo is right clicked
    */
	void RightClicked(QString &class_name, QString &name);

    
protected:
    void draw(QPainter *p, const QRect& update_rect);
    void drawThermo(QPainter *p);
    void layoutThermo( bool update = TRUE );
    virtual void scaleChange();
    virtual void fontChange(const QFont &oldFont);

    virtual void paintEvent(QPaintEvent *e);
    virtual void resizeEvent(QResizeEvent *e);
	void mouseReleaseEvent(QMouseEvent *);

private:
    void init();
    
    QwtDiMap d_map;
    QRect d_thermoRect;
    QBrush d_fillBrush;
    QBrush d_alarmBrush;
    
    Qt::Orientation d_orient;
    ScalePos d_scalePos;
    int d_borderWidth;
    int d_scaleDist;
    int d_thermoWidth;

    double d_minValue;
    double d_maxValue;
    double d_value;
    double d_alarmLevel;
    bool d_alarmEnabled;
};

#endif

// Local Variables:
// mode: C++
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
