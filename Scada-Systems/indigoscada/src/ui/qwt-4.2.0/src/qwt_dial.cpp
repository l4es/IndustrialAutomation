/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include <math.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qevent.h>
#include "qwt_math.h"
#include "qwt_paint_buffer.h"
#include "qwt_painter.h"
#include "qwt_dial_needle.h"
#include "qwt_dial.h"

double QwtDial::d_previousDir = -1.0;

/*!
  Constructor

  \param parent Parent dial widget
*/
QwtDialScaleDraw::QwtDialScaleDraw(QwtDial *parent):
    d_parent(parent),
    d_penWidth(1),
    d_visibleLabels(TRUE)
{
}

/*!
  En/disable labels

  If labels are disabled, QwtDialScaleDraw::label returns always
  QString::null. Otherwise it calls QwtDial::scaleLabel of the
  parent dial.

  \sa QwtDialScaleDraw::visibleLabels
*/
void QwtDialScaleDraw::showLabels(bool enable)
{
    d_visibleLabels = enable;
}

/*!
  Return wether to display labels.
  \sa QwtDialScaleDraw::showLabels
*/
bool QwtDialScaleDraw::visibleLabels() const
{
    return d_visibleLabels;
}

/*!
  Set the pen width used for painting the scale

  \param penWidth Pen width
  \sa penWidth(), QwtDial::drawScale()
*/
    
void QwtDialScaleDraw::setPenWidth(uint penWidth)
{
    d_penWidth = penWidth;
}

/*!
  \return Pen width used for painting the scale
  \sa setPenWidth, QwtDial::drawScale()
*/
uint QwtDialScaleDraw::penWidth() const
{
    return d_penWidth;
}

/*! 
  Call QwtDial::scaleLabel of the parent dial widget.

  \param value Value to display
    
  \sa QwtDial::scaleLabel
*/  
QString QwtDialScaleDraw::label(double value) const
{
    if ( !d_visibleLabels )
        return QString::null;

    if ( d_parent == NULL )
        return QwtScaleDraw::label(value);

    return d_parent->scaleLabel(value);
}

/*!
  \brief Constructor
  \param parent Parent widget
  \param name Widget name

  Create a dial widget with no scale and no needle. 
  The default origin is 90.0 with no valid value. It accepts
  mouse and keyboard inputs and has no step size. The default mode
  is QwtDial::RotateNeedle.
*/  

QwtDial::QwtDial(QWidget* parent, const char* name):
        QwtSliderBase(Qt::Horizontal, parent, name, 
            Qt::WRepaintNoErase|Qt::WResizeNoErase),
    d_visibleBackground(FALSE),
    d_frameShadow(Sunken),
    d_lineWidth(0),
    d_mode(RotateNeedle),
    d_origin(90.0),
    d_minScaleArc(0.0),
    d_maxScaleArc(0.0),
    d_scaleDraw(0),
    d_maxMajIntv(36),
    d_maxMinIntv(10),
    d_scaleStep(0.0),
    d_needle(0)
{
    setBackgroundMode(NoBackground);
    setFocusPolicy(QWidget::TabFocus);

    QPalette p = palette();
    for ( int i = 0; i < QPalette::NColorGroups; i++ )
    {
        const QPalette::ColorGroup cg = (QPalette::ColorGroup)i;

        // Base: background color of the circle inside the frame.
        // Foreground: background color of the circle inside the scale

        p.setColor(cg, QColorGroup::Foreground, 
            p.color(cg, QColorGroup::Base));
    }
    setPalette(p);

    d_scaleDraw = new QwtDialScaleDraw(this);
    d_scaleDraw->setGeometry(0, 0, 0, QwtScaleDraw::Round);

    setScaleArc(0.0, 360.0); // scale as a full circle
    setRange(0.0, 360.0, 1.0, 10); // degrees as deafult
}

//!  Destructor
QwtDial::~QwtDial() 
{
    delete d_scaleDraw;
    delete d_needle;
}

/*!
  Show/Hide the area outside of the frame
  \param show Show if TRUE, hide if FALSE

  \sa QwtDial::hasVisibleBackground, QWidget::setMask
  \warning When QwtDial is a toplevel widget the window
           border might disappear too.
*/
void QwtDial::showBackground(bool show)
{
    if ( d_visibleBackground != show )
    {
        d_visibleBackground = show;

        if ( d_visibleBackground )
            clearMask();
        else
            setMask(QRegion(boundingRect(), QRegion::Ellipse));

        update();
    }
}

/*!
  TRUE when the area outside of the frame is visible

  \sa QwtDial::showBackground, QWidget::setMask
*/
bool QwtDial::hasVisibleBackground() const 
{ 
    return d_visibleBackground; 
}

/*!
  Sets the frame shadow value from the frame style.
  \param shadow Frame shadow
  \sa QwtDial::setLineWidth, QFrame::setFrameShadow
*/
void QwtDial::setFrameShadow(Shadow shadow)
{
    if ( shadow != d_frameShadow )
    {
        d_frameShadow = shadow;
        if ( lineWidth() > 0 )
            update();
    }
}

/*!
  \return Frame shadow
  /sa QwtDial::setFrameShadow, QwtDial::lineWidth, 
      QFrame::frameShadow
*/
QwtDial::Shadow QwtDial::frameShadow() const 
{ 
    return d_frameShadow; 
}

/*!
  Sets the line width

  \param lineWidth Line width
  \sa QwtDial::setFrameShadow
*/
void QwtDial::setLineWidth(int lineWidth)
{
    if ( lineWidth < 0 )
        lineWidth = 0;

    if ( d_lineWidth != lineWidth )
    {
        d_lineWidth = lineWidth;
        update();
    }
}

/*!
  \return Line width of the frame
  \sa QwtDial::setLineWidth, QwtDial::frameShadow, 
      QFrame::lineWidth
*/
int QwtDial::lineWidth() const 
{ 
    return d_lineWidth; 
}

/*!
  \return bounding rect of the circle inside the frame
  \sa QwtDial::setLineWidth, QwtDial::scaleContentsRect, QwtDial::boundingRect
*/
QRect QwtDial::contentsRect() const
{
    const int lw = lineWidth();

    QRect r = boundingRect();
    if ( lw > 0 )
    {
        r.setRect(r.x() + lw, r.y() + lw, 
            r.width() - 2 * lw, r.height() - 2 * lw);
    }
    return r;
}

/*!
  \return bounding rect of the dial including the frame
  \sa QwtDial::setLineWidth, QwtDial::scaleContentsRect, QwtDial::contentsRect
*/
QRect QwtDial::boundingRect() const
{
    const int radius = QMIN(width(), height()) / 2;

    QRect r(0, 0, 2 * radius, 2 * radius);
    r.moveCenter(rect().center());
    return r;
}

/*!
  \return rect inside the scale
  \sa setLineWidth, QwtDial::boundingRect, QwtDial::contentsRect
*/
QRect QwtDial::scaleContentsRect() const
{
    const QPen scalePen(colorGroup().text(), 0, Qt::NoPen);

    int scaleDist = 0;
    if ( d_scaleDraw )
    {
        scaleDist = QMAX(
            d_scaleDraw->maxWidth(scalePen, fontMetrics()),
            d_scaleDraw->maxHeight(scalePen, fontMetrics()));
        scaleDist++; // margin
    }

    const QRect rect = contentsRect();
    return QRect(rect.x() + scaleDist, rect.y() + scaleDist,
        rect.width() - 2 * scaleDist, rect.height() - 2 * scaleDist);
}

/*!
  \brief Change the mode of the meter.
  \param mode New mode 
    
  The value of the meter is indicated by the difference
  between north of the scale and the direction of the needle.
  In case of QwtDial::RotateNeedle north is pointing
  to the origin() and the needle is rotating, in case of
  QwtDial::RotateScale, the needle points to origin()
  and the scale is rotating.
    
  The default mode is QwtDial::RotateNeedle.

  \sa QwtDial::mode, QwtDial::setValue, QwtDial::setOrigin
*/  
void QwtDial::setMode(Mode mode)
{   
    if ( mode != d_mode )
    {
        d_mode = mode;
        update(); 
    }
}       

/*! 
  \return mode of the dial.
    
  The value of the dial is indicated by the difference
  between the origin and the direction of the needle.
  In case of QwtDial::RotateNeedle the scale arc is fixed
  to the origin() and the needle is rotating, in case of
  QwtDial::RotateScale, the needle points to origin()
  and the scale is rotating.
 
  The default mode is QwtDial::RotateNeedle.

  \sa QwtDial::setMode, QwtDial::origin, QwtDial::setScaleArc, QwtDial::value
*/
QwtDial::Mode QwtDial::mode() const
{
    return d_mode;
}

/*! 
    Sets whether it is possible to step the value from the highest value to 
    the lowest value and vice versa to on.

    \param wrapping en/disables wrapping

    \sa QwtDial::wrapping, QwtDblRange::periodic
    \note The meaning of wrapping is like the wrapping property of QSpinBox,
          but not like it is used in QDial. 
*/
void QwtDial::setWrapping(bool wrapping)
{
    setPeriodic(wrapping);
} 

/*! 
    wrapping() holds whether it is possible to step the value from the 
    highest value to the lowest value and vice versa. 

    \sa QwtDial::setWrapping, QwtDblRange::setPeriodic
    \note The meaning of wrapping is like the wrapping property of QSpinBox,
          but not like it is used in QDial. 
*/ 
bool QwtDial::wrapping() const
{
    return periodic();
}

//! Resize the dial widget
void QwtDial::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);

    if ( !hasVisibleBackground() )
        setMask(QRegion(boundingRect(), QRegion::Ellipse));
}

//! Repaint the dial 
void QwtDial::paintEvent(QPaintEvent *e)
{
    const QRect &ur = e->rect();
    if ( ur.isValid() )
    {
        QwtPaintBuffer paintBuffer(this, ur);

        QPainter *painter = paintBuffer.painter();

        drawContents(painter);
        drawFrame(painter);

        if ( hasFocus() )
            drawFocusIndicator(painter);
    }
}

/*!
  Draw a dotted round circle, if !isReadOnly()

  \param painter Painter
*/
void QwtDial::drawFocusIndicator(QPainter *painter) const
{
    if ( !isReadOnly() )
    {
        QRect focusRect = contentsRect();

        const int margin = 2;
        focusRect.setRect( 
            focusRect.x() + margin,
            focusRect.y() + margin,
            focusRect.width() - 2 * margin,
            focusRect.height() - 2 * margin);

        QColor color = colorGroup().color(QColorGroup::Base);
        if (color.isValid())
        {
            int h, s, v;
            color.hsv(&h, &s, &v);
            color = (v > 128) ? Qt::gray.dark(120) : Qt::gray.light(120);
        }
        else
            color = Qt::darkGray;

        painter->save();
        painter->setBrush(Qt::NoBrush);
        painter->setPen(QPen(color, 0, Qt::DotLine));
        painter->drawEllipse(focusRect);
        painter->restore();
    }
}

/*!
  Draw the frame around the dial

  \param painter Painter
  \sa QwtDial::lineWidth, QwtDial::frameShadow
*/
void QwtDial::drawFrame(QPainter *painter)
{
    const int lw = lineWidth();
    const int off = (lw + 1) % 2;

    QRect r = boundingRect();
    r.setRect(r.x() + lw / 2 - off, r.y() + lw / 2 - off,
        r.width() - lw + off + 1, r.height() - lw + off + 1);

    if ( lw > 0 )
    {
        switch(d_frameShadow)
        {
            case QwtDial::Raised:
                QwtPainter::drawRoundFrame(painter, r, 
                    lw, colorGroup(), FALSE);
                break;
            case QwtDial::Sunken:
                QwtPainter::drawRoundFrame(painter, r, 
                    lw, colorGroup(), TRUE);
                break;
            default: // Plain
            {
                painter->save();
                painter->setPen(QPen(Qt::black, lw));
                painter->setBrush(Qt::NoBrush);
                painter->drawEllipse(r);
                painter->restore();
            }
        }
    }
}

/*!
  \brief Draw the contents inside the frame
 
  QColorGroup::Background is the background color outside of the frame.
  QColorGroup::Base is the background color inside the frame.
  QColorGroup::Foreground is the background color inside the scale.

  \param painter Painter
  \sa QwtDial::boundingRect, QwtDial::contentsRect,
    QwtDial::scaleContentsRect, QWidget::setPalette
*/
void QwtDial::drawContents(QPainter *painter) const
{
    if ( backgroundMode() == NoBackground || 
        colorGroup().brush(QColorGroup::Base) != 
            colorGroup().brush(QColorGroup::Background) )
    {
        // Don´t use QPainter::drawEllipse. There are some pixels
        // different compared to the region in the mask, leaving
        // them in background color.

        painter->save();
        painter->setPen(Qt::NoPen);
        painter->setBrush(colorGroup().brush(QColorGroup::Base));

        // Even if we want to fill the contentsRect only, we fill the
        // complete boundingRect. The frame will be painted later
        // above, but we want to have the base color below it
        // because round objects doesn´t cover all pixels.

        QRect br = boundingRect();
#if QT_VERSION < 300
#ifdef _WS_WIN32_
        // Qt-230-NC draws ellipses not as nicely as Qt-2.3.x on X Windows
        br.setTop(br.top()-1);
        br.setLeft(br.left()-1);
        br.setBottom(br.bottom()+1);
        br.setRight(br.right()+1);
#endif
#endif
        painter->setClipRegion(QRegion(painter->xForm(br), QRegion::Ellipse));
        painter->drawRect(br);
        painter->restore();
    }


    const QRect insideScaleRect = scaleContentsRect();
    if ( colorGroup().brush(QColorGroup::Foreground) !=
        colorGroup().brush(QColorGroup::Base) )
    {
        painter->save();
        painter->setPen(Qt::NoPen);
        painter->setBrush(colorGroup().brush(QColorGroup::Foreground));

        painter->setClipRegion(
            QRegion(painter->xForm(insideScaleRect), QRegion::Ellipse));
        painter->drawRect(insideScaleRect);
        painter->restore();
    }

    const QPoint center = insideScaleRect.center();
    const int radius = insideScaleRect.width() / 2;

    painter->save();
    drawScaleContents(painter, center, radius);
    painter->restore();

    double direction = d_origin;

    if (isValid())
    {
        direction = d_origin + d_minScaleArc;
        if ( maxValue() > minValue() && d_maxScaleArc > d_minScaleArc )
        {
            const double ratio = 
                (value() - minValue()) / (maxValue() - minValue());
            direction += ratio * (d_maxScaleArc - d_minScaleArc);
        }

        if ( direction >= 360.0 )
            direction -= 360.0;
    }

    double origin = d_origin;
    if ( mode() == RotateScale )
    {
        origin -= direction - d_origin;
        direction = d_origin;
    }

    painter->save();
    drawScale(painter, center, radius, origin, d_minScaleArc, d_maxScaleArc);
    painter->restore();

    if ( isValid() )
    {
        QPalette::ColorGroup cg;
        if ( isEnabled() )
            cg = hasFocus() ? QPalette::Active : QPalette::Inactive;
        else
            cg = QPalette::Disabled;

        painter->save();
        drawNeedle(painter, center, radius, direction, cg);
        painter->restore();
    }
}

/*!
  Draw the needle

  Qwt is missing a set of good looking needles.
  Contributions are very welcome.

  \param painter Painter
  \param center Center of the dial
  \param radius Length for the needle
  \param direction Direction of the needle in degrees, counter clockwise
  \param cg ColorGroup
*/
void QwtDial::drawNeedle(QPainter *painter, const QPoint &center, 
    int radius, double direction, QPalette::ColorGroup cg) const
{
    if ( d_needle )
    {
        direction = 360.0 - direction; // counter clockwise
        d_needle->draw(painter, center, radius, direction, cg);
    }
}

/*!
  Draw the scale

  \param painter Painter
  \param center Center of the dial
  \param radius Radius of the scale
  \param origin Origin of the scale
  \param minArc Minimum of the arc 
  \param maxArc Minimum of the arc 
  
  \sa QwtScaleDraw::setAngleRange
*/
void QwtDial::drawScale(QPainter *painter, const QPoint &center,
    int radius, double origin, double minArc, double maxArc) const
{
    if ( d_scaleDraw == NULL )
        return;

    origin -= 270.0; // hardcoded origin of QwtScaleDraw

    double angle = maxArc - minArc;
    if ( angle > 360.0 )
        angle = fmod(angle, 360.0);

    minArc += origin;
    if ( minArc < -360.0 )
        minArc = fmod(minArc, 360.0);
    
    maxArc = minArc + angle;
    if ( maxArc > 360.0 )
    {
        // QwtScaleDraw::setAngleRange accepts only values
        // in the range [-360.0..360.0]
        minArc -= 360.0;
        maxArc -= 360.0;
    }
    
    painter->setFont(font());
    painter->setPen(QPen(colorGroup().text(), d_scaleDraw->penWidth()));

    d_scaleDraw->setAngleRange(minArc, maxArc);
    d_scaleDraw->setGeometry(
        center.x() - radius + 1, 
        center.y() - radius + 1,
        2 * radius, QwtScaleDraw::Round);

    d_scaleDraw->draw(painter);
}

void QwtDial::drawScaleContents(QPainter *, 
    const QPoint &, int) const
{
    // empty default implementation
}

/*!
  Set a needle for the dial

  Qwt is missing a set of good looking needles. 
  Contributions are very welcome.

  \param needle Needle
  \warning The needle will be deleted, when a different needle is
    set or in ~QwtDial
*/
void QwtDial::setNeedle(QwtDialNeedle *needle)
{
    if ( needle != d_needle )
    {
        if ( d_needle )
            delete d_needle;

        d_needle = needle;
        update();
    }
}

/*! 
  \return needle
  \sa QwtDial::setNeedle
*/
const QwtDialNeedle *QwtDial::needle() const 
{ 
    return d_needle; 
}

/*! 
  \return needle
  \sa QwtDial::setNeedle
*/
QwtDialNeedle *QwtDial::needle() 
{ 
    return d_needle; 
}

//! QwtDblRange update hook
void QwtDial::rangeChange()
{
    updateScale();
}

/*! 
  Update the scale with the current attributes
  \sa QwtDial::setScale
*/
void QwtDial::updateScale()
{
    if ( d_scaleDraw )
    {
        d_scaleDraw->setScale(minValue(), maxValue(),
            d_maxMajIntv, d_maxMinIntv, d_scaleStep);
    }
}

/*!
  Set an individual scale draw

  \warning The previous scale draw is deleted
*/
void QwtDial::setScaleDraw(QwtDialScaleDraw *scaleDraw)
{
    if ( scaleDraw != d_scaleDraw )
    {
        if ( d_scaleDraw )
            delete d_scaleDraw;
    
        d_scaleDraw = scaleDraw;
        updateScale();
        update();
    }
}

/*!
  Change the intervals of the scale
  \sa QwtScaleDraw::setScale
*/
void QwtDial::setScale(int maxMajIntv, int maxMinIntv, double step)
{
    d_maxMajIntv = maxMajIntv;
    d_maxMinIntv = maxMinIntv;
    d_scaleStep = step;

    updateScale();
}

/*!
  A wrapper method for accessing the scale draw. 

  - options == 0\n
    No visible scale: setScaleDraw(NULL)
  - options & ScaleBackbone\n
    En/disable the backbone of the scale.
  - options & ScaleTicks\n
    Don´t change anything. Otherwise set all tick lengths to 0. 
  - options & ScaleLabel\n
    En/disable scale labels
    
  \sa QwtScaleDraw:.setOptions, QwtScaleDraw::setTickLength, 
      QwtDialScaleDraw::showLabels
*/
void QwtDial::setScaleOptions(int options)
{
    if ( options == 0 )
        setScaleDraw(NULL);

    if ( d_scaleDraw == NULL )
        return;

    int flags = d_scaleDraw->options();
    if ( options & ScaleBackbone )
        flags |= QwtScaleDraw::Backbone;
    else
        flags &= ~QwtScaleDraw::Backbone;
    d_scaleDraw->setOptions(flags);

    if ( !(options & ScaleTicks) )
        d_scaleDraw->setTickLength(0, 0, 0);

    d_scaleDraw->showLabels(options & ScaleLabel);
}

//! See: QwtScaleDraw::setTickLength, QwtDialScaleDraw::setPenWidth
void QwtDial::setScaleTicks(int minLen, int medLen, 
    int majLen, int penWidth)
{
    if ( d_scaleDraw )
    {
        d_scaleDraw->setTickLength(minLen, medLen, majLen);
        d_scaleDraw->setPenWidth(penWidth);
    }
}

/*!
   \return the label for a value
*/
QString QwtDial::scaleLabel(double value) const
{
#if 1
    if ( value == -0 )
        value = 0;
#endif

    QString text;
    text.sprintf("%g", value);

    return text;
}

/*!
  \brief Change the origin 
 
  The origin is the angle where scale and needle is relative to.

  \param origin New origin
  \sa QwtDial::origin()
*/
void QwtDial::setOrigin(double origin)
{
    d_origin = origin;
    update();
}

/*!
  The origin is the angle where scale and needle is relative to.

  \return Origin of the dial
  \sa QwtDial::setOrigin()
*/
double QwtDial::origin() const
{
    return d_origin;
}

/*!
  Change the arc of the scale

  \param minArc Lower limit
  \param maxArc Upper limit
*/
void QwtDial::setScaleArc(double minArc, double maxArc)
{
    if ( minArc != 360.0 && minArc != -360.0 )
        minArc = fmod(minArc, 360.0);
    if ( maxArc != 360.0 && maxArc != -360.0 )
        maxArc = fmod(maxArc, 360.0);

    d_minScaleArc = QMIN(minArc, maxArc);
    d_maxScaleArc = QMAX(minArc, maxArc);
    if ( d_maxScaleArc - d_minScaleArc > 360.0 )
        d_maxScaleArc = d_minScaleArc + 360.0;
    
    update();
}

//! QwtDblRange update hook
void QwtDial::valueChange()
{
    update();
    QwtSliderBase::valueChange();
}

/*!
  \return QwtDial::sizeHint()
*/
QSize QwtDial::sizeHint() const
{
    int sh = 0;
    if ( d_scaleDraw )
        sh = d_scaleDraw->minHeight( QPen(), fontMetrics() );

    const int d = 6 * sh + 2 * lineWidth();
    
    return QSize( d, d );
}

/*! 
  \brief Return a minimum size hint
  \warning The return value of QwtDial::minimumSizeHint() depends on the
           font and the scale.
*/  
QSize QwtDial::minimumSizeHint() const
{   
    int sh = 0;
    if ( d_scaleDraw )
        sh = d_scaleDraw->minHeight( QPen(), fontMetrics() );

    const int d = 3 * sh + 2 * lineWidth();
    
    return QSize( d, d );
}

static double line2Radians(const QPoint &p1, const QPoint &p2)
{
    const QPoint p = p2 - p1;

    double angle;
    if ( p.x() == 0 )
        angle = ( p.y() <= 0 ) ? M_PI_2 : 3 * M_PI_2;
    else
    {
        angle = atan(double(-p.y()) / double(p.x()));
        if ( p.x() < 0 )
            angle += M_PI;
        if ( angle < 0.0 )
            angle += 2 * M_PI;
    }
    return 360.0 - angle * 180.0 / M_PI;
}

/*!
  Find the value for a given position

  \param pos
  \return Value
*/
double QwtDial::getValue(const QPoint &pos)
{
    if ( d_maxScaleArc == d_minScaleArc || maxValue() == minValue() )
        return minValue();

    double dir = line2Radians(rect().center(), pos) - d_origin;
    if ( dir < 0.0 )
        dir += 360.0;

    if ( mode() == RotateScale )
        dir = 360.0 - dir;

    // The position might be in the area that is outside the scale arc.
    // We need the range of the scale if it was a complete circle.

    const double completeCircle = 360.0 / (d_maxScaleArc - d_minScaleArc) 
        * (maxValue() - minValue());

    double posValue = minValue() + completeCircle * dir / 360.0;

    if ( d_scrollMode == ScrMouse )
    {
        if ( d_previousDir >= 0.0 ) // valid direction
        {
            // We have to find out whether the mouse is moving
            // clock or counter clockwise

            bool clockWise = FALSE;

            const double angle = dir - d_previousDir;
            if ( (angle >= 0.0 && angle <= 180.0) || angle < -180.0 )
                clockWise = TRUE;

            if ( clockWise )
            {
                if ( dir < d_previousDir && d_mouseOffset > 0.0 )
                {
                    // We passed 360 -> 0
                    d_mouseOffset -= completeCircle;    
                }

                if ( wrapping() )
                {
                    if ( posValue - d_mouseOffset > maxValue() )
                    {
                        // We passed maxValue and the value will be set
                        // to minValue. We have to adjust the d_mouseOffset.

                        d_mouseOffset = posValue - minValue();
                    }
                }
                else
                {
                    if ( posValue - d_mouseOffset > maxValue() ||
                        value() == maxValue() )
                    {
                        // We fix the value at maxValue by adjusting
                        // the mouse offset.

                        d_mouseOffset = posValue - maxValue();
                    }
                }
            }
            else
            {
                if ( dir > d_previousDir && d_mouseOffset < 0.0 )
                {
                    // We passed 0 -> 360 
                    d_mouseOffset += completeCircle;    
                }

                if ( wrapping() )
                {
                    if ( posValue - d_mouseOffset < minValue() )
                    {
                        // We passed minValue and the value will be set
                        // to maxValue. We have to adjust the d_mouseOffset.

                        d_mouseOffset = posValue - maxValue();
                    }
                }
                else
                {
                    if ( posValue - d_mouseOffset < minValue() ||
                        value() == minValue() )
                    {
                        // We fix the value at minValue by adjusting
                        // the mouse offset.

                        d_mouseOffset = posValue - minValue();
                    }
                }
            }
        }
        d_previousDir = dir;
    }

    return posValue;
}

/*!
  \sa QwtSliderBase::getScrollMode
*/
void QwtDial::getScrollMode(const QPoint &p, int &scrollMode, int &direction)
{
    direction = 0;
    scrollMode = ScrNone;

    const QRegion region(contentsRect(), QRegion::Ellipse);
    if ( region.contains(p) && p != rect().center() )
    {
        scrollMode = ScrMouse;
        d_previousDir = -1.0;
    }
}

/*! 
  Handles key events

  - Key_Down, KeyLeft\n
    Decrement by 1
  - Key_Prior\n
    Decrement by pageSize()
  - Key_Home\n
    Set the value to minValue()

  - Key_Up, KeyRight\n
    Increment by 1
  - Key_Next\n
    Increment by pageSize()
  - Key_End\n
    Set the value to maxValue()

  \sa isReadOnly()
*/
void QwtDial::keyPressEvent(QKeyEvent *e)
{
    if ( isReadOnly() )
    {
#if QT_VERSION >= 300
        e->ignore();
#endif
        return;
    }

    if ( !isValid() )
        return;

    double previous = prevValue();
    switch ( e->key() )
    {
        case Qt::Key_Down:
        case Qt::Key_Left:
            QwtDblRange::incValue(-1);
            break;
        case Qt::Key_Prior:
            QwtDblRange::incValue(-pageSize());
            break;
        case Qt::Key_Home:
            setValue(minValue());
            break;

        case Qt::Key_Up:
        case Qt::Key_Right:
            QwtDblRange::incValue(1);
            break;
        case Qt::Key_Next:
            QwtDblRange::incValue(pageSize());
            break;
        case Qt::Key_End:
            setValue(maxValue());
            break;
        default:;
#if QT_VERSION >= 300
            e->ignore();
#endif
    }

    if (value() != previous)
        emit sliderMoved(value());
}

