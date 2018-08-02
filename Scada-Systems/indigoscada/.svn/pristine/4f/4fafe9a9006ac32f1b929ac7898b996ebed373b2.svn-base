/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include <qevent.h>
#include <qdrawutil.h>
#include <qpainter.h>
#include <qstyle.h>
#include "qwt_math.h"
#include "qwt_paint_buffer.h"
#include "qwt_wheel.h"


//! Constructor
QwtWheel::QwtWheel(QWidget *parent, const char *name): 
    QwtSliderBase(Qt::Horizontal, parent, name, 
        Qt::WRepaintNoErase|Qt::WResizeNoErase )
{
    d_viewAngle = 175.0;
    d_totalAngle = 360.0;
    d_tickCnt = 10;
    d_intBorder = 2;
    d_borderWidth = 2;
    d_colorCnt = 30;
    d_wheelWidth = 20;
    d_allocContext = 0;
    d_colors = new QColor[d_colorCnt];
    setUpdateTime(50);
}

//! Destructor
QwtWheel::~QwtWheel()  
{
    if ( d_allocContext )
        QColor::destroyAllocContext( d_allocContext );
    delete[] d_colors;
}

//! Set up the color array for the background pixmap.
void QwtWheel::setColorArray()
{
    if ( !d_colors ) 
        return;

    QColorGroup g = colorGroup();

    if ( !d_colors[0].isValid() ||
        d_colors[0] != g.light() ||
        d_colors[d_colorCnt - 1] != g.dark() )
    {
        if ( d_allocContext )
            QColor::destroyAllocContext( d_allocContext );

        d_allocContext = QColor::enterAllocContext();

        d_colors[0] = g.light();
        d_colors[d_colorCnt - 1] = g.dark();

        int dh, ds, dv, lh, ls, lv;
        d_colors[0].rgb(&lh, &ls, &lv);
        d_colors[d_colorCnt - 1].rgb(&dh, &ds, &dv);

        for ( int i = 1; i < d_colorCnt - 1; ++i )
        {
            const double factor = double(i) / double(d_colorCnt);

            d_colors[i].setRgb( lh + int( double(dh - lh) * factor ),
                      ls + int( double(ds - ls) * factor ),
                      lv + int( double(dv - lv) * factor ));
        }
        QColor::leaveAllocContext();
    }
}

/*!
  \brief Adjust the number of grooves in the wheel's surface.

  The number of grooves is limited to 6 <= cnt <= 50.
  Values outside this range will be clipped.
  The default value is 10.
  \param cnt Number of grooves per 360 degrees
*/
void QwtWheel::setTickCnt(int cnt)
{
    d_tickCnt = qwtLim( cnt, 6, 50 );
    update();
}

/*!
    \return mass
*/
double QwtWheel::mass() const
{
    return QwtSliderBase::mass();
}



/*!
  \brief Set the internal border width of the wheel.

  The internal border must not be smaller than 1
  and is limited in dependence on the wheel's size.
  Values outside the allowed range will be clipped.

  The internal border defaults to 2.
  \param w border width
*/
void QwtWheel::setInternalBorder( int w )
{
    const int d = qwtMin( width(), height() ) / 3;
    w = qwtMin( w, d );
    d_intBorder = qwtMax( w, 1 );
    layoutWheel();
}

//! Draw the Wheel's background gradient
void QwtWheel::drawWheelBackground( QPainter *p, const QRect &r )
{
    p->save();

    //
    // initialize pens
    //
    const QColorGroup g = colorGroup();

    QPen lightPen;
    lightPen.setColor(g.light());
    lightPen.setWidth(d_intBorder);

    QPen darkPen;
    darkPen.setColor(g.dark());
    darkPen.setWidth(d_intBorder);

    setColorArray();

    //
    // initialize auxiliary variables
    //

    const int nFields = d_colorCnt * 13 / 10;
    const int hiPos = nFields - d_colorCnt + 1;

    if ( orientation() == Qt::Horizontal )
    {
        const int rx = r.x();
        int ry = r.y() + d_intBorder;
        const int rh = r.height() - 2* d_intBorder;
        const int rw = r.width();
        //
        //  draw shaded background
        //
        int x1 = rx;
        for (int i = 1; i < nFields; i++ )
        {
            const int x2 = rx + (rw * i) / nFields;
            p->fillRect(x1, ry, x2-x1 + 1 ,rh, d_colors[qwtAbs(i-hiPos)]);
            x1 = x2 + 1;
        }
        p->fillRect(x1, ry, rw - (x1 - rx), rh, d_colors[d_colorCnt - 1]);

        //
        // draw internal border
        //
        p->setPen(lightPen);
        ry = r.y() + d_intBorder / 2;
        p->drawLine(r.x(), ry, r.x() + r.width() , ry);

        p->setPen(darkPen);
        ry = r.y() + r.height() - (d_intBorder - d_intBorder / 2);
        p->drawLine(r.x(), ry , r.x() + r.width(), ry);
    }
    else // Qt::Vertical
    {
        int rx = r.x() + d_intBorder;
        const int ry = r.y();
        const int rh = r.height();
        const int rw = r.width() - 2 * d_intBorder;

        //
        // draw shaded background
        //
        int y1 = ry;
        for ( int i = 1; i < nFields; i++ )
        {
            const int y2 = ry + (rh * i) / nFields;
            p->fillRect(rx, y1, rw, y2-y1 + 1, d_colors[qwtAbs(i-hiPos)]);
            y1 = y2 + 1;
        }
        p->fillRect(rx, y1, rw, rh - (y1 - ry), d_colors[d_colorCnt - 1]);

        //
        //  draw internal borders
        //
        p->setPen(lightPen);
        rx = r.x() + d_intBorder / 2;
        p->drawLine(rx, r.y(), rx, r.y() + r.height());

        p->setPen(darkPen);
        rx = r.x() + r.width() - (d_intBorder - d_intBorder / 2);
        p->drawLine(rx, r.y(), rx , r.y() + r.height());
    }

    p->restore();
}


/*!
  \brief Set the total angle which the wheel can be turned.

  One full turn of the wheel corresponds to an angle of
  360 degrees. A total angle of n*360 degrees means
  that the wheel has to be turned n times around its axis
  to get from the minimum value to the maximum value.

  The default setting of the total angle is 360 degrees.
  \param angle total angle in degrees
*/
void QwtWheel::setTotalAngle(double angle)
{
    d_totalAngle = qwtLim( angle, 10.0, 3600.0 );
    update();
}

/*!
  \brief Set the wheel's orientation.
  \param o Orientation. Allowed values are
           Qt::Horizontal and Qt::Vertical.
   Defaults to Qt::Horizontal.
  \sa QwtSliderBase::orientation()
*/
void QwtWheel::setOrientation(Qt::Orientation o)
{
    if ( orientation() != o )
    {
        QwtSliderBase::setOrientation(o);
        layoutWheel();
    }
}

/*!
  \brief Specify the visible portion of the wheel.

  You may use this function for fine-tuning the appearance of
  the wheel. The default value is 175 degrees. The value is
  limited from 10 to 175 degrees.
  \param angle Visible angle in degrees
*/
void QwtWheel::setViewAngle(double angle)
{
    d_viewAngle = qwtLim( angle, 10.0, 175.0 );
    update();
}

/*!
  \brief Redraw the wheel
  \param p painter
  \param r contents rectangle
*/
void QwtWheel::drawWheel( QPainter *p, const QRect &r )
{
    const double sign = (minValue() < maxValue()) ? 1.0 : -1.0;
    double cnvFactor = qwtAbs(d_totalAngle / (maxValue() - minValue()));
    const double halfIntv = 0.5 * d_viewAngle / cnvFactor;
    const double loValue = value() - halfIntv;
    const double hiValue = value() + halfIntv;
    const double tickWidth = 360.0 / double(d_tickCnt) / cnvFactor;
    const double sinArc = sin(d_viewAngle * M_PI / 360.0);
    cnvFactor *= M_PI / 180.0;

    //
    // draw background gradient
    //
    drawWheelBackground( p, r );

    //
    // draw grooves
    //
    if ( orientation() == Qt::Horizontal )
    {
        const double halfSize = double(r.width()) * 0.5;

        int l1 = r.y() + d_intBorder;
        int l2 = r.y() + r.height() - d_intBorder - 1;

        // draw one point over the border if border > 1
        if ( d_intBorder > 1 )
        {
            l1 --;
            l2 ++;
        }

        const int maxpos = r.x() + r.width() - 2;
        const int minpos = r.x() + 2;

        //
        // draw tick marks
        //
        for ( double tickValue = ceil(loValue / tickWidth) * tickWidth;
            tickValue < hiValue; tickValue += tickWidth )
        {
            //
            //  calculate position
            //
            const int tickPos = r.x() + r.width()
                - int( halfSize
                    * (sinArc + sign *  sin((tickValue - value()) * cnvFactor))
                    / sinArc);
            //
            // draw vertical line
            //
            if ( (tickPos <= maxpos) && (tickPos > minpos) )
            {
                p->setPen(colorGroup().dark());
                p->drawLine(tickPos -1 , l1, tickPos - 1,  l2 );  
                p->setPen(colorGroup().light());
                p->drawLine(tickPos, l1, tickPos, l2);  
            }
        }
    }
    else if ( orientation() == Qt::Vertical )
    {
        const double halfSize = double(r.height()) * 0.5;

        int l1 = r.x() + d_intBorder;
        int l2 = r.x() + r.width() - d_intBorder - 1;

        if ( d_intBorder > 1 )
        {
            l1--;
            l2++;
        }

        const int maxpos = r.y() + r.height() - 2;
        const int minpos = r.y() + 2;

        //
        // draw tick marks
        //
        for ( double tickValue = ceil(loValue / tickWidth) * tickWidth;
            tickValue < hiValue; tickValue += tickWidth )
        {

            //
            // calculate position
            //
            const int tickPos = r.y() + int( halfSize *
                (sinArc + sign * sin((tickValue - value()) * cnvFactor))
                / sinArc);

            //
            //  draw horizontal line
            //
            if ( (tickPos <= maxpos) && (tickPos > minpos) )
            {
                p->setPen(colorGroup().dark());
                p->drawLine(l1, tickPos - 1 ,l2, tickPos - 1);  
                p->setPen(colorGroup().light());
                p->drawLine(l1, tickPos, l2, tickPos);  
            }
        }
    }
}


//! Determine the value corresponding to a specified point
double QwtWheel::getValue( const QPoint &p )
{
    // The reference position is arbitrary, but the
    // sign of the offset is important
    int w, dx;
    if ( orientation() == Qt::Vertical )
    {
        w = d_sliderRect.height();
        dx = d_sliderRect.y() - p.y();
    }
    else
    {
        w = d_sliderRect.width();
        dx = p.x() - d_sliderRect.x();
    }

    // w pixels is an arc of viewAngle degrees,
    // so we convert change in pixels to change in angle
    const double ang = dx * d_viewAngle / w;

    // value range maps to totalAngle degrees,
    // so convert the change in angle to a change in value
    const double val = ang * ( maxValue() - minValue() ) / d_totalAngle;

    // Note, range clamping and rasterizing to step is automatically
    // handled by QwtSliderBase, so we simply return the change in value
    return val;
}

//! Qt Resize Event
void QwtWheel::resizeEvent(QResizeEvent *)
{
    layoutWheel( FALSE );
}

//! Recalculate the slider's geometry and layout based on
//  the current rect and fonts.
//  \param update_geometry  notify the layout system and call update
//         to redraw the scale
void QwtWheel::layoutWheel( bool update_geometry )
{
    const QRect r = this->rect();
    d_sliderRect.setRect(r.x() + d_borderWidth, r.y() + d_borderWidth,
        r.width() - 2*d_borderWidth, r.height() - 2*d_borderWidth);

    if ( update_geometry )
    {
        updateGeometry();
        update();
    }
}

//! Qt Paint Event
void QwtWheel::paintEvent(QPaintEvent *e)
{
    // Use double-buffering
    const QRect &ur = e->rect();
    if ( ur.isValid() )
    {
        QwtPaintBuffer paintBuffer(this, ur);
        draw(paintBuffer.painter(), ur);
    }
}

//! Redraw panel and wheel
void QwtWheel::draw(QPainter *painter, const QRect&)
{
    QColorGroup cg = colorGroup();

    // Rather than maintain a separate color group
    // for the outer frame, use our parent's cg
    QWidget *parent = parentWidget();
    if ( parent )
        cg = parent->colorGroup();

    qDrawShadePanel( painter, rect().x(), rect().y(),
        rect().width(), rect().height(),
        cg, TRUE, d_borderWidth );

    drawWheel( painter, d_sliderRect );

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

//! Notify value change 
void QwtWheel::valueChange()
{
    QwtSliderBase::valueChange();
    update();
}


/*!
  \brief Determine the scrolling mode and direction corresponding
         to a specified point
  \param p point
  \param scrollMode scrolling mode
  \param direction direction
*/
void QwtWheel::getScrollMode( const QPoint &p, int &scrollMode, int &direction)
{
    if ( d_sliderRect.contains(p) )
        scrollMode = ScrMouse;
    else
        scrollMode = ScrNone;

    direction = 0;
}

/*!
  \brief Set the mass of the wheel

  Assigning a mass turns the wheel into a flywheel.
  \param val the wheel's mass
*/
void QwtWheel::setMass(double val)
{
    QwtSliderBase::setMass(val);
}

/*!
  \brief Set the width of the wheel

  Corresponds to the wheel height for horizontal orientation,
  and the wheel width for vertical orientation.
  \param w the wheel's width
*/
void QwtWheel::setWheelWidth(int w)
{
    d_wheelWidth = w;
    layoutWheel();
}

/*!
  \return Preferred/Fixed for horizontal,
          Fixed/Preferred for vertical scales.
*/

QSizePolicy QwtWheel::sizePolicy() const
{
    QSizePolicy sp;
    if ( orientation() == Qt::Horizontal )
    {
        sp.setHorData( QSizePolicy::Preferred );
        sp.setVerData( QSizePolicy::Fixed );
    }
    else
    {
        sp.setHorData( QSizePolicy::Fixed );
        sp.setVerData( QSizePolicy::Preferred );
    }
    return sp;
}

/*!
  \return a size hint
*/
QSize QwtWheel::sizeHint() const
{
    return minimumSizeHint();
}

/*!
  \brief Return a minimum size hint
  \warning The return value is based on the wheel width.
*/
QSize QwtWheel::minimumSizeHint() const
{
    QSize sz( 3*d_wheelWidth + 2*d_borderWidth,
    d_wheelWidth + 2*d_borderWidth );
    if ( orientation() != Qt::Horizontal )
        sz.transpose();
    return sz;
}

/*!
  \brief Call update() when the palette changes
*/
void QwtWheel::paletteChange( const QPalette& )
{
    update();
}

