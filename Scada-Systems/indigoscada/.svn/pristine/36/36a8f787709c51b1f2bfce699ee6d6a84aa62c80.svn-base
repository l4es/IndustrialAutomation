/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_KNOB_H
#define QWT_KNOB_H

#include "qwt_global.h"
#include "qwt_sldbase.h"
#include "qwt_sclif.h"

/*!
  \brief The Knob Widget

  The QwtKnob widget imitates look and behaviour of a volume knob on a radio.
  It contains a scale around the knob which is set up automatically or can
  be configured manually (see QwtScaleIf).
  Automatic scrolling is enabled when the user presses a mouse
  button on the scale. For a description of signals, slots and other
  members, see QwtSliderBase.

  \image html knob.gif
  \sa   QwtSliderBase and QwtScaleIf for the descriptions
    of the inherited members.
*/

class QWT_EXPORT QwtKnob : public QwtSliderBase, public QwtScaleIf
{
    Q_OBJECT 
    Q_ENUMS (Symbol)
    Q_PROPERTY( int knobWidth READ knobWidth WRITE setKnobWidth )
    Q_PROPERTY( int borderWidth READ borderWidth WRITE setBorderWidth )
    Q_PROPERTY( double totalAngle READ totalAngle WRITE setTotalAngle )
    Q_PROPERTY( Symbol symbol READ symbol WRITE setSymbol )

public:
    /*!
        Symbol
        \sa QwtKnob::QwtKnob()
    */

    enum Symbol { Line, Dot };

    QwtKnob(QWidget* parent = 0, const char *name = 0);
    virtual ~QwtKnob();

    void setKnobWidth(int w);
    void setTotalAngle (double angle);
    void setBorderWidth(int bw);

    //! Return the width of the knob
    int knobWidth() const {return d_knobWidth;};
    //! Return the total angle 
    double totalAngle() const {return d_totalAngle;};
    //! Return the border width
    int borderWidth() const {return d_borderWidth;};

    void setSymbol(Symbol);
    Symbol symbol() const;

    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;
    
protected:
    virtual void paintEvent(QPaintEvent *e);
    virtual void resizeEvent(QResizeEvent *e);

    void draw(QPainter *p, const QRect& ur);
    void drawKnob(QPainter *p, const QRect &r);
    void drawMarker(QPainter *p, double arc, const QColor &c);

private:
    void layoutKnob( bool update = TRUE );
    double getValue(const QPoint &p);
    void getScrollMode( const QPoint &p, int &scrollMode, int &direction );
    void recalcAngle();
    
    virtual void valueChange();
    virtual void rangeChange();
    virtual void scaleChange();
    virtual void fontChange(const QFont &oldFont);

    int d_borderWidth;
    int d_borderDist;
    int d_hasScale;
    int d_scaleDist;
    int d_maxScaleTicks;
    int d_knobWidth;
    int d_dotWidth;

    Symbol d_symbol;
    double d_angle;
    double d_oldAngle;
    double d_totalAngle;
    double d_nTurns;

    QRect d_kRect;
};

#endif

// Local Variables:
// mode: C++
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
