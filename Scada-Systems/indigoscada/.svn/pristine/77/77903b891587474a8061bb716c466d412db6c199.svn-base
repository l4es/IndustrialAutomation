/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#include <math.h>
#include <qstyle.h>
#include <qevent.h>
#include <qdrawutil.h>
#include <qpainter.h>
#include "qwt_paint_buffer.h"
#include "qwt_slider.h"

/*!
  \brief Constructor
  \param parent parent widget
  \param name The Widget's name. Default = 0.
  \param orient Orientation of the slider. Can be Qt::Horizontal
         or Qt::Vertical. Defaults to Horizontal.
  \param scalePos Position of the scale.  Can be QwtSlider::None,
         QwtSlider::Left, QwtSlider::Right, QwtSlider::Top,
         or QwtSlider::Bottom. Defaults to QwtSlider::None.
  \param bgStyle Background style. QwtSlider::BgTrough draws the
         slider button in a trough, QwtSlider::BgSlot draws
         a slot underneath the button. An or-combination of both
         may also be used. The default is QwtSlider::BgTrough.

  QwtSlider enforces valid combinations of its orientation and scale position.
  If the combination is invalid, the scale position will be set to None. Valid
  combinations are:
  - Qt::Horizonal with None, Top, or Bottom;
  - Qt::Vertical with None, Left, or Right.
*/
QwtSlider::QwtSlider(QWidget *parent, const char *name,
        Qt::Orientation orient, ScalePos scalePos, BGSTYLE bgStyle): 
    QwtSliderBase(orient, parent, name, Qt::WRepaintNoErase|Qt::WResizeNoErase)
{
    d_borderWidth = 2;
    d_scaleDist = 4;
    d_scalePos = scalePos;
    d_xMargin = 0;
    d_yMargin = 0;
    d_bgStyle = bgStyle;

    if (bgStyle == BgSlot)
    {
        d_thumbLength = 16;
        d_thumbWidth = 30;
    }
    else
    {
        d_thumbLength = 31;
        d_thumbWidth = 16;
    }

    d_sliderRect.setRect(0,0,8,8);

    QwtScaleDraw::Orientation so;
    if ( orientation() == Qt::Vertical )
    {
        // enforce a valid combination of scale position and orientation
        if ((d_scalePos == Bottom) || (d_scalePos == Top))
            d_scalePos = None;
        // adopt the policy of layoutSlider (None lays out like Left)
        if (d_scalePos == Right)
           so = QwtScaleDraw::Right;
        else
           so = QwtScaleDraw::Left;
    }
    else
    {
        // enforce a valid combination of scale position and orientation
        if ((d_scalePos == Left) || (d_scalePos == Right))
            d_scalePos = None;
        // adopt the policy of layoutSlider (None lays out like Bottom)
        if (d_scalePos == Top)
           so = QwtScaleDraw::Top;
        else
           so = QwtScaleDraw::Bottom;
    }

    scaleDraw()->setGeometry(0,0,100,so);
}

/*!
  \brief Set the orientation.
  \param o Orientation. Allowed values are Qt::Horizontal and Qt::Vertical.
  
  If the new orientation and the old scale position are an invalid combination,
  the scale position will be set to None.
  \sa QwtSliderBase::orientation()
*/
void QwtSlider::setOrientation(Qt::Orientation o) 
{
    if (o == Qt::Horizontal)
    {
        if ((d_scalePos == Left) || (d_scalePos == Right))
            d_scalePos = None;
    }
    else // if (o == Qt::Vertical)
    {
        if ((d_scalePos == Bottom) || (d_scalePos == Top))
            d_scalePos = None;
    }
    QwtSliderBase::setOrientation(o);
    layoutSlider();
}

/*!
  \brief Change the scale position (and slider orientation).

  \param s Position of the scale.

  A valid combination of scale position and orientation is enforced:
  - if the new scale position is Left or Right, the scale orientation will
    become Qt::Vertical;
  - if the new scale position is Bottom or Top the scale orientation will
    become Qt::Horizontal;
  - if the new scale position is None, the scale orientation will not change.
*/
void QwtSlider::setScalePosition(ScalePos s)
{
    d_scalePos = s;
    if ((s == Bottom) || (s == Top))
        setOrientation(Qt::Horizontal);
    else if ((s == Left) || (s == Right))
        setOrientation(Qt::Vertical);
    else
        layoutSlider();
}

//! Return the scale position.
QwtSlider::ScalePos QwtSlider::scalePosition() const
{
    return d_scalePos;
}

/*!
  \brief Change the slider's border width
  \param bd border width
*/
void QwtSlider::setBorderWidth(int bd)
{
    if ( bd < 0 )
        bd = 0;

    if ( bd != d_borderWidth )
    {
        d_borderWidth = bd;
        layoutSlider();
    }
}

/*!
  \brief Set the slider's thumb length
  \param thumbLength new length
*/
void QwtSlider::setThumbLength(int thumbLength)
{
    if ( thumbLength < 8 )
        thumbLength = 8;

    if ( thumbLength != d_thumbLength )
    {
        d_thumbLength = thumbLength;
        layoutSlider();
    }
}

/*!
  \brief Change the width of the thumb
  \param w new width
*/
void QwtSlider::setThumbWidth(int w)
{
    if ( w < 4 )
        w = 4;

    if ( d_thumbWidth != w )
    {
        d_thumbWidth = w;
        layoutSlider();
    }
}


//! Notify changed scale
void QwtSlider::scaleChange()
{
    if (!hasUserScale())
    {
        scaleDraw()->setScale(minValue(), maxValue(), 
            scaleMaxMajor(), scaleMaxMinor(),
            scaleDraw()->scaleDiv().logScale());
    }

    layoutSlider();
}


//! Notify change in font
void QwtSlider::fontChange(const QFont &f)
{
    QwtSliderBase::fontChange( f );
    layoutSlider();
}

//! Draw the slider into the specified rectangle.
void QwtSlider::drawSlider(QPainter *p, const QRect &r)
{
    QRect cr(r);

    if (d_bgStyle & BgTrough)
    {
        qDrawShadePanel(p, r.x(), r.y(),
            r.width(), r.height(),
            colorGroup(), TRUE, d_borderWidth,0);

        cr.setRect(r.x() + d_borderWidth,
            r.y() + d_borderWidth,
            r.width() - 2 * d_borderWidth,
            r.height() - 2 * d_borderWidth);

        p->fillRect(cr.x(), cr.y(), cr.width(), cr.height(), 
            colorGroup().brush(QColorGroup::Mid));
    }

    if ( d_bgStyle & BgSlot)
    {
        int ws = 4;
        int ds = d_thumbLength / 2 - 4;
        if ( ds < 1 )
            ds = 1;

        QRect rSlot;
        if (orientation() == Qt::Horizontal)
        {
            if ( cr.height() & 1 )
                ws++;
            rSlot = QRect(cr.x() + ds, 
                    cr.y() + (cr.height() - ws) / 2,
                    cr.width() - 2 * ds, ws);
        }
        else
        {
            if ( cr.width() & 1 )
                ws++;
            rSlot = QRect(cr.x() + (cr.width() - ws) / 2, 
                    cr.y() + ds,
                    ws, cr.height() - 2 * ds);
        }
        p->fillRect(rSlot.x(), rSlot.y(), rSlot.width(), rSlot.height(),
            colorGroup().brush(QColorGroup::Dark));
        qDrawShadePanel(p, rSlot.x(), rSlot.y(),
            rSlot.width(), rSlot.height(), colorGroup(), TRUE, 1 ,0);

    }

    if ( isValid() )
        drawThumb(p, cr, xyPosition(value()));
}

//! Draw the thumb at a position
void QwtSlider::drawThumb(QPainter *p, const QRect &sliderRect, int pos)
{
    pos++; // shade line points one pixel below
    if (orientation() == Qt::Horizontal)
    {
        qDrawShadePanel(p, pos - d_thumbLength / 2, 
            sliderRect.y(), d_thumbLength, sliderRect.height(),
            colorGroup(), FALSE, d_borderWidth, 
            &colorGroup().brush(QColorGroup::Button));

        qDrawShadeLine(p, pos, sliderRect.y(), 
            pos, sliderRect.y() + sliderRect.height() - 2, 
            colorGroup(), TRUE, 1);
    }
    else // Vertical
    {
        qDrawShadePanel(p,sliderRect.x(), pos - d_thumbLength / 2, 
            sliderRect.width(), d_thumbLength,
            colorGroup(),FALSE, d_borderWidth, 
            &colorGroup().brush(QColorGroup::Button));

        qDrawShadeLine(p, sliderRect.x(), pos,
            sliderRect.x() + sliderRect.width() - 2, pos, 
            colorGroup(), TRUE, 1);
    }
}

//! Find the x/y position for a given value v
int QwtSlider::xyPosition(double v) const
{
    int pos;
    if ( minValue() == scaleDraw()->d1() && maxValue() == scaleDraw()->d2() )
    {
        // We prefer to use the transformation of scaleDraw
        // So ticks and marker are always in sync and we don't have
        // to take care of rounding problems.

        pos = scaleDraw()->transform(v);
    }
    else
    {
        // range and scaleDraw differ ? Sounds strange but
        // might happen with logarithmic scales

        const double f = (v - minValue()) / (maxValue() - minValue());
        double dPos;
        if ( orientation() == Qt::Horizontal )
            dPos = scaleDraw()->i1() + f * (scaleDraw()->i2() - scaleDraw()->i1());
        else
            dPos = scaleDraw()->i1() - f * (scaleDraw()->i1() - scaleDraw()->i2());
        pos = qRound(dPos);
    }
    return pos;
}

//! Determine the value corresponding to a specified mouse location.
double QwtSlider::getValue(const QPoint &p)
{
    double rv;

    if ( minValue() == scaleDraw()->d1() && maxValue() == scaleDraw()->d2() )
    {
        rv = scaleDraw()->invTransform(
            orientation() == Qt::Horizontal ? p.x() : p.y());
    }
    else
    {
        double pos;
        double range;
        if ( orientation() == Qt::Horizontal )
        {
            pos = p.x() - scaleDraw()->i1();
            range = scaleDraw()->i2() - scaleDraw()->i1();
        }
        else
        {
            pos = scaleDraw()->i1() - p.y();
            range = scaleDraw()->i1() - scaleDraw()->i2();
        }
            
        rv = minValue() + pos / range * (maxValue() - minValue()); 
    }

    return(rv);
}


/*!
  \brief Determine scrolling mode and direction
  \param p point
  \param scrollMode Scrolling mode
  \param direction Direction
*/
void QwtSlider::getScrollMode(const QPoint &p, 
    int &scrollMode, int &direction )
{
    if (!d_sliderRect.contains(p))
    {
        scrollMode = ScrNone;
        direction = 0;
        return;
    }

    const int pos = ( orientation() == Qt::Horizontal ) ? p.x() : p.y();
    const int markerPos = xyPosition(value());

    if ((pos > markerPos - d_thumbLength / 2)
        && (pos < markerPos + d_thumbLength / 2))
    {
        scrollMode = ScrMouse;
        direction = 0;
        return;
    }

    scrollMode = ScrPage;
    direction = (pos > markerPos) ? 1 : -1;
    if ( scaleDraw()->i1() > scaleDraw()->i2() )
        direction = -direction;
}

//! Qt paint event
void QwtSlider::paintEvent(QPaintEvent *e)
{
    const QRect &ur = e->rect();
    if ( ur.isValid() )
    {
        QwtPaintBuffer paintBuffer(this, ur);
        draw(paintBuffer.painter(), ur);
    }
}

//! Draw the QwtSlider
void QwtSlider::draw(QPainter *painter, const QRect&)
{
    if (d_scalePos != None)
        scaleDraw()->draw(painter);

    drawSlider(painter, d_sliderRect);
    if ( hasFocus() )
    {
        const QRect rect = d_sliderRect;

#if QT_VERSION < 300
        style().drawFocusRect(painter, rect, colorGroup());
#else
        style().drawPrimitive(QStyle::PE_FocusRect, painter,
            rect, colorGroup());
#endif
    }
}

//! Qt resize event
void QwtSlider::resizeEvent(QResizeEvent *)
{
    layoutSlider( FALSE );
}

/*!
  Recalculate the slider's geometry and layout based on
  the current rect and fonts.
  \param update_geometry  notify the layout system and call update
         to redraw the scale
*/
void QwtSlider::layoutSlider( bool update_geometry )
{
    int sliderWidth = d_thumbWidth;
    int sld1 = d_thumbLength / 2 - 1;
    int sld2 = d_thumbLength / 2 + d_thumbLength % 2;
    if ( d_bgStyle & BgTrough )
    {
        sliderWidth += 2 * d_borderWidth;
        sld1 += d_borderWidth;
        sld2 += d_borderWidth;
    }

    int scd = 0;
    if ( d_scalePos != None )
    {
        int d1, d2;
        scaleDraw()->minBorderDist(fontMetrics(), d1, d2);
        scd = QMAX(d1, d2);
    }

    int slo = scd - sld1;
    if ( slo < 0 )
        slo = 0;

    const QRect r = rect();
    if (orientation() == Qt::Horizontal)
    {
        switch (d_scalePos)
        {
        case Top:
            d_sliderRect.setRect(
        r.x() + d_xMargin + slo,
                r.y() + r.height() -
                d_yMargin - sliderWidth,
                r.width() - 2 * d_xMargin - 2 * slo,
                sliderWidth);
            scaleDraw()->setGeometry(
        d_sliderRect.x() + sld1,
                d_sliderRect.y() - d_scaleDist,
                d_sliderRect.width() - sld1 - sld2,
                QwtScaleDraw::Top);
            break;

        case Bottom:
            d_sliderRect.setRect(
        r.x() + d_xMargin + slo,
                r.y() + d_yMargin,
                r.width() - 2 * d_xMargin - 2 * slo,
                sliderWidth);
            scaleDraw()->setGeometry(
                d_sliderRect.x() + sld1,
                d_sliderRect.y() + d_sliderRect.height() + d_scaleDist,
                d_sliderRect.width() - sld1 - sld2,
                QwtScaleDraw::Bottom);
            break;

        case None: // like Bottom, but no scale. See QwtSlider().
        default:   // inconsistent orientation and scale position
            // QwtScaleDraw is derived from QwtDiMap.
            // The map serves to transform between coordinates and doubles.
            d_sliderRect.setRect(
                r.x() + d_xMargin + slo,
                r.y() + d_yMargin,
                r.width() - 2 * d_xMargin - 2 * slo,
                sliderWidth);
            scaleDraw()->setIntRange(
                d_sliderRect.x() + sld1,
                d_sliderRect.x() + d_sliderRect.width() - sld2 - 1);
            break;
        }
    }
    else // if (orientation() == Qt::Vertical
    {
        switch (d_scalePos)
        {
        case Right:
            d_sliderRect.setRect(
        r.x() + d_xMargin,
                r.y() + d_yMargin + slo,
                sliderWidth,
                r.height() - 2 * d_yMargin - 2 * slo);
            scaleDraw()->setGeometry(
        d_sliderRect.x() + d_sliderRect.width() + d_scaleDist,
                d_sliderRect.y() + sld1,
                d_sliderRect.height() - sld1 - sld2,
                QwtScaleDraw::Right);
            break;

        case Left:
            d_sliderRect.setRect(
        r.x() + r.width() - sliderWidth - d_xMargin,
                r.y() + d_yMargin + slo,
                sliderWidth,
                r.height() - 2 * d_yMargin - 2 * slo);
            scaleDraw()->setGeometry(
        d_sliderRect.x() - d_scaleDist,
                d_sliderRect.y() + sld1,
                d_sliderRect.height() - sld1 - sld2,
                QwtScaleDraw::Left);
            break;

        case None: // like Left, but no scale. See QwtSlider().
        default:   // inconsistent orientation and scale position
            // QwtScaleDraw is derived from QwtDiMap.
            // The map serves to transform between coordinates and doubles.
            d_sliderRect.setRect(
                r.x() + r.width() - sliderWidth - d_xMargin,
                r.y() + d_yMargin + slo,
                sliderWidth,
                r.height() - 2 * d_yMargin - 2 * slo);
            scaleDraw()->setIntRange(
                d_sliderRect.y() + d_sliderRect.height() - sld2 - 1,
                d_sliderRect.y() + sld1);
            break;
        }
    }

    if ( update_geometry )
    {
        updateGeometry();
        update();
    }
}

//! Notify change of value
void QwtSlider::valueChange()
{
    QwtSliderBase::valueChange();
    update();
}


//! Notify change of range
void QwtSlider::rangeChange()
{
    if (!hasUserScale())
    {
        scaleDraw()->setScale(minValue(), maxValue(), 
            scaleMaxMajor(), scaleMaxMinor(),
            scaleDraw()->scaleDiv().logScale());
    }

    QwtSliderBase::rangeChange();
    layoutSlider();
}

/*!
  \brief Set distances between the widget's border and internals.
  \param xMargin Horizontal margin
  \param yMargin Vertical margin
*/
void QwtSlider::setMargins(int xMargin, int yMargin)
{
    if ( xMargin < 0 )
        xMargin = 0;
    if ( yMargin < 0 )
        yMargin = 0;

    if ( xMargin != d_xMargin || yMargin != d_yMargin )
    {
        d_xMargin = xMargin;
        d_yMargin = yMargin;
        layoutSlider();
    }
}

/*!
  \return MinimumExpanding/Fixed for a horizontal orientation;
          Fixed/MinimumExpanding for a vertical orientation.
*/
QSizePolicy QwtSlider::sizePolicy() const
{
    QSizePolicy sp;
    if ( orientation() == Qt::Horizontal )
    {
        sp.setHorData( QSizePolicy::MinimumExpanding );
        sp.setVerData( QSizePolicy::Fixed );
    }
    else
    {
        sp.setHorData( QSizePolicy::Fixed );
        sp.setVerData( QSizePolicy::MinimumExpanding );
    }
    return sp;
}

/*!
  \return QwtSlider::minimumSizeHint()
*/
QSize QwtSlider::sizeHint() const
{
    return minimumSizeHint();
}

/*!
  \brief Return a minimum size hint
  \warning The return value of QwtSlider::minimumSizeHint() depends on 
           the font and the scale.
*/
QSize QwtSlider::minimumSizeHint() const
{
    int w = 0, h = 0;

    int sliderWidth = d_thumbWidth;
    if (d_bgStyle & BgTrough)
        sliderWidth += 2 * d_borderWidth;

    if (d_scalePos != None)
    {
        int msWidth = scaleDraw()->minWidth( QPen(), fontMetrics() );
        int msHeight = scaleDraw()->minHeight( QPen(), fontMetrics() );
        
        int d1, d2;
        scaleDraw()->minBorderDist(fontMetrics(), d1, d2);
        int msMbd = QMAX(d1, d2);

        int mbd = d_thumbLength / 2;
        if (d_bgStyle & BgTrough)
            mbd += d_borderWidth;

        if ( mbd < msMbd )
            mbd = msMbd;

        if (orientation() == Qt::Vertical)
        {
            w = 2 * d_xMargin + sliderWidth + msWidth + d_scaleDist;
            h = msHeight - 2 * msMbd + 2 * mbd + 2 * d_yMargin;
        }
        else
        {
            w = msWidth - 2 * msMbd + 2 * mbd + 2 * d_xMargin;
            h = 2 * d_yMargin + sliderWidth  + msHeight + d_scaleDist;
        }
    }
    else  // no scale
    {
        if (orientation() == Qt::Vertical)
        {
            w = 2 * d_xMargin + sliderWidth;
            h = 200 + 2 * d_yMargin;
        }
        else
        {
            w = 200 + 2 * d_xMargin;
            h = 2 * d_yMargin + sliderWidth;
        }
    }
    return QSize(w,h);
}

// Local Variables:
// mode: C++
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
