/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include <qpainter.h>
#include <qpalette.h>
#include <qstyle.h>
#include <qevent.h>
#include "qwt_knob.h"
#include "qwt_math.h"
#include "qwt_paint_buffer.h"

/*!
  \brief Constructor
  \param parent Parent widget
  \param name Name
*/
QwtKnob::QwtKnob(QWidget* parent, const char *name): 
    QwtSliderBase(Qt::Horizontal, parent, name, 
                Qt::WRepaintNoErase|Qt::WResizeNoErase)
{
    d_angle = 0.0;
    d_oldAngle = 0.0;
    d_nTurns = 0.0;
    d_borderWidth = 2;
    d_borderDist = 4;
    d_totalAngle = 270.0;
    d_scaleDist = 4;
    d_hasScale = 0;
    d_symbol = Line;
    d_maxScaleTicks = 11;
    d_knobWidth = 50;
    d_dotWidth = 8;

    scaleDraw()->setGeometry(
        0, 0, d_knobWidth + 2 * d_scaleDist, QwtScaleDraw::Round );
    setUpdateTime(50);
    setTotalAngle( 270.0 );
    recalcAngle();
    setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
}

//! Destructor
QwtKnob::~QwtKnob()
{
}

/*!
  \brief Set the symbol of the knob
  \sa QwtKnob::symbol()
*/
void QwtKnob::setSymbol(QwtKnob::Symbol s)
{
    if ( d_symbol != s )
    {
        d_symbol = s;
        update();
    }
}

/*! 
    \return symbol of the knob
    \sa QwtKnob::setSymbol()
*/
QwtKnob::Symbol QwtKnob::symbol() const
{
    return d_symbol;
}

/*!
  \brief Set the total angle by which the knob can be turned
  \param angle Angle in degrees.

  The default angle is 270 degrees. It is possible to specify
  an angle of more than 360 degrees so that the knob can be
  turned several times around its axis.
*/
void QwtKnob::setTotalAngle (double angle)
{
    if (angle < 10.0)
       d_totalAngle = 10.0;
    else
       d_totalAngle = angle;
    scaleDraw()->setAngleRange( -0.5 * d_totalAngle, 0.5 * d_totalAngle);
    layoutKnob();
}


/*!
  \brief Draw the knob
  \param p painter
  \param r borders of the knob
*/
void QwtKnob::drawKnob(QPainter *p, const QRect &r)
{
    const int bw2 = d_borderWidth / 2;

    QRect aRect(r.x() + bw2, r.y() + bw2,
          r.width() - 2 * bw2, r.height() - 2 * bw2);

    //
    // draw button face
    //
    p->setBrush(colorGroup().brush(QColorGroup::Button));
    p->drawEllipse(aRect);

    //
    // draw button shades
    //
    QPen pn;
    pn.setWidth(d_borderWidth);

    pn.setColor(colorGroup().light());
    p->setPen(pn);
    p->drawArc(aRect, 45*16,180*16);

    pn.setColor(colorGroup().dark());
    p->setPen(pn);
    p->drawArc(aRect, 225*16,180*16);

    //
    // draw marker
    //
    if ( isValid() )
        drawMarker(p, d_angle, colorGroup().buttonText());
}

/*!
  \brief Notify change of value

  Sets the slider's value to the nearest multiple
  of the step size.
*/
void QwtKnob::valueChange()
{
    recalcAngle();
    update();
    QwtSliderBase::valueChange();
}

/*!
  \brief Determine the value corresponding to a specified position

  Called by QwtSliderBase
  \param p point
*/
double QwtKnob::getValue(const QPoint &p)
{
    const double dx = double((rect().x() + rect().width() / 2) - p.x() );
    const double dy = double((rect().y() + rect().height() / 2) - p.y() );

    const double arc = atan2(-dx,dy) * 180.0 / M_PI;

    double newValue =  0.5 * (minValue() + maxValue())
       + (arc + d_nTurns * 360.0) * (maxValue() - minValue())
      / d_totalAngle;

    const double oneTurn = fabs(maxValue() - minValue()) * 360.0 / d_totalAngle;
    const double eqValue = value() + d_mouseOffset;

    if (fabs(newValue - eqValue) > 0.5 * oneTurn)
    {
        if (newValue < eqValue)
           newValue += oneTurn;
        else
           newValue -= oneTurn;
    }

    return newValue;    
}

/*!
  \brief Set the scrolling mode and direction

  Called by QwtSliderBase
  \param p Point in question
*/
void QwtKnob::getScrollMode(const QPoint &p, int &scrollMode, int &direction)
{
    const int r = d_kRect.width() / 2;

    const int dx = d_kRect.x() + r - p.x();
    const int dy = d_kRect.y() + r - p.y();

    if ( (dx * dx) + (dy * dy) <= (r * r)) // point is inside the knob
    {
        scrollMode = ScrMouse;
        direction = 0;
    }
    else                                // point lies outside
    {
        scrollMode = ScrTimer;
        double arc = atan2(double(-dx),double(dy)) * 180.0 / M_PI;
        if ( arc < d_angle)
           direction = -1;
        else if (arc > d_angle)
           direction = 1;
        else
           direction = 0;
    }
}


/*!
  \brief Notify a change of the range

  Called by QwtSliderBase
*/
void QwtKnob::rangeChange()
{
    if (!hasUserScale())
    {
        scaleDraw()->setScale(minValue(), maxValue(),
            scaleMaxMajor(), scaleMaxMinor());
    }
    layoutKnob();
    recalcAngle();
}

/*!
  \brief Qt Resize Event
*/
void QwtKnob::resizeEvent(QResizeEvent *)
{
    layoutKnob( FALSE );
}

//! Recalculate the slider's geometry and layout based on
//  the current rect and fonts.
//  \param update_geometry  notify the layout system and call update
//         to redraw the scale
void QwtKnob::layoutKnob( bool update_geometry )
{
    const QRect &r = rect();

    const int width = qwtMin(qwtMin(r.height(), r.width()), d_knobWidth);
    const int width_2 = width / 2;

    d_kRect.setRect(r.x() + r.width() / 2 - width_2,
            r.y() + r.height() / 2 - width_2,
            width, width);

    scaleDraw()->setGeometry(d_kRect.x() - d_scaleDist,
            d_kRect.y() - d_scaleDist,
            width + 2 * d_scaleDist, QwtScaleDraw::Round );

    if ( update_geometry )
    {
        updateGeometry();
        update();
    }
}

/*!
  \brief Repaint the knob
*/
void QwtKnob::paintEvent(QPaintEvent *e)
{
    const QRect &ur = e->rect();
    if ( ur.isValid() ) 
    {
        QwtPaintBuffer paintBuffer(this, ur);
        draw(paintBuffer.painter(), ur);
    }
}

/*!
  \brief Repaint the knob
*/
void QwtKnob::draw(QPainter *painter, const QRect& ur)
{
    if ( !d_kRect.contains( ur ) ) // event from valueChange()
        scaleDraw()->draw( painter );
    drawKnob( painter, d_kRect );

    if ( hasFocus() )
    {
        QRect r = rect();

#if QT_VERSION < 300
        style().drawFocusRect(painter, r, colorGroup());
#else
        style().drawPrimitive(QStyle::PE_FocusRect, painter,
            r, colorGroup());
#endif
    }

}

/*!
  \brief Draw the marker at the knob's front
  \param p Painter
  \param arc Angle of the marker
  \param c Marker color
*/
void QwtKnob::drawMarker(QPainter *p, double arc, const QColor &c)
{
    const double rarc = arc * M_PI / 180.0;
    const double ca = cos(rarc);
    const double sa = - sin(rarc);

    int radius = d_kRect.width() / 2 - d_borderWidth;
    if (radius < 3) 
        radius = 3; 

    const int ym = d_kRect.y() + radius + d_borderWidth;
    const int xm = d_kRect.x() + radius + d_borderWidth;

    switch (d_symbol)
    {
        case Dot:
        {
            p->setBrush(c);
            p->setPen(Qt::NoPen);

            const double rb = double(qwtMax(radius - 4 - d_dotWidth / 2, 0));
            p->drawEllipse(xm - int(floor (sa * rb + 0.5)) - d_dotWidth / 2,
                   ym - int(floor (ca * rb + 0.5)) - d_dotWidth / 2,
                   d_dotWidth, d_dotWidth);
            break;
        }
        case Line:
        {
            p->setPen(QPen(c, 2));

            const double rb = qwtMax(double((radius - 4) / 3.0), 0.0);
            const double re = qwtMax(double(radius - 4), 0.0);
            
            p->drawLine (xm - int (floor (sa * rb + 0.5)),
                 ym - int (floor (ca * rb + 0.5)),
                 xm - int (floor (sa * re + 0.5)),
                 ym - int (floor (ca * re + 0.5)));
            
            break;
        }
    }
}

/*!
  \brief Change the knob's width.

  The specified width must be >= 5, or it will be clipped.
  \param w New width
*/
void QwtKnob::setKnobWidth(int w)
{
    d_knobWidth = qwtMax(w,5);
    layoutKnob();
}

/*!
  \brief Set the knob's border width
  \param bw new border width
*/
void QwtKnob::setBorderWidth(int bw)
{
    d_borderWidth = qwtMax(bw, 0);
    layoutKnob();
}

/*!
  \brief Recalculate the marker angle corresponding to the
    current value
*/
void QwtKnob::recalcAngle()
{
    d_oldAngle = d_angle;

    //
    // calculate the angle corresponding to the value
    //
    if (maxValue() == minValue())
    {
        d_angle = 0;
        d_nTurns = 0;
    }
    else
    {
        d_angle = (value() - 0.5 * (minValue() + maxValue()))
            / (maxValue() - minValue()) * d_totalAngle;
        d_nTurns = floor((d_angle + 180.0) / 360.0);
        d_angle = d_angle - d_nTurns * 360.0;
    }
}


/*!
    Recalculates the layout
    \sa QwtKnob::layoutKnob()
*/
void QwtKnob::scaleChange()
{
    layoutKnob();
}

/*!
    Recalculates the layout
    \sa QwtKnob::layoutKnob()
*/
void QwtKnob::fontChange(const QFont &f)
{
    QwtSliderBase::fontChange( f );
    layoutKnob();
}

/*!
  \return QwtKnob::minimumSizeHint()
*/
QSize QwtKnob::sizeHint() const
{
    return minimumSizeHint();
}

/*!
  \brief Return a minimum size hint
  \warning The return value of QwtKnob::minimumSizeHint() depends on the 
           font and the scale.
*/
QSize QwtKnob::minimumSizeHint() const
{
    // Add the scale radial thickness to the knobWidth
    const int sh = scaleDraw()->minHeight( QPen(), fontMetrics() );
    const int d = 2 * sh + 2 * d_scaleDist + d_knobWidth;

    return QSize( d, d );
}

// Local Variables:
// mode: C++
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
