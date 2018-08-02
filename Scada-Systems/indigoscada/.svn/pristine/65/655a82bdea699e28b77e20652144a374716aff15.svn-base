/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include <qpainter.h>
#include <qevent.h>
#include <qstyle.h>
#include <qpixmap.h>
#include <qdrawutil.h>
#include "qwt_math.h"
#include "qwt_paint_buffer.h"
#include "qwt_thermo.h"

//! Constructor
QwtThermo::QwtThermo(QWidget *parent, const char *name): 
    QWidget(parent, name, Qt::WRepaintNoErase|Qt::WResizeNoErase)
{
    init();
}

//! Destructor
QwtThermo::~QwtThermo()
{
}

//! Initialize the data members.
void QwtThermo::init()
{
    // initialize data members
    d_orient = Qt::Vertical;
    d_scalePos = Left;
    d_scaleDist = 3;
    d_thermoWidth = 10;
    d_borderWidth = 2;
    d_maxValue = 1.0;
    d_minValue = 0.0;
    d_value = 0.0;
    d_alarmLevel = 0.0;
    d_alarmEnabled = 0;

    // initialize brushes;
    d_fillBrush = QBrush(Qt::black);
    d_alarmBrush = QBrush(Qt::white);

    // initialize scales
    d_map.setDblRange(d_minValue, d_maxValue);
    scaleDraw()->setScale(d_minValue, d_maxValue,
        scaleMaxMajor(), scaleMaxMinor());
}


//! Set the current value.
void QwtThermo::setValue(double v)
{
    if (d_value != v)
    {
        d_value = v;
        update();
    }
}

//! Qt paint event.
void QwtThermo::paintEvent(QPaintEvent *e)
{
    // Use double-buffering
    const QRect &ur = e->rect();
    if ( ur.isValid() )
    {
        QwtPaintBuffer paintBuffer(this, ur);
        draw(paintBuffer.painter(), ur);
    }
}

//! Draw the whole QwtThermo.
void QwtThermo::draw(QPainter *p, const QRect& ur)
{
    if ( !d_thermoRect.contains(ur) )
    {
        if (d_scalePos != None)
            scaleDraw()->draw(p);

        qDrawShadePanel(p,
            d_thermoRect.x() - d_borderWidth,
            d_thermoRect.y() - d_borderWidth,
            d_thermoRect.width() + 2*d_borderWidth,
            d_thermoRect.height() + 2*d_borderWidth,
            colorGroup(), TRUE, d_borderWidth,0);
    }
    drawThermo(p);
}

//! Qt resize event handler
void QwtThermo::resizeEvent(QResizeEvent *)
{
    layoutThermo( FALSE );
}

/*!
  Recalculate the QwtThermo geometry and layout based on
  the QwtThermo::rect() and the fonts.
  \param update_geometry notify the layout system and call update
         to redraw the scale
*/
void QwtThermo::layoutThermo( bool update_geometry )
{
    QRect r = rect();
    int mbd = 0;
    if ( d_scalePos != None )
    {
        int d1, d2;
        scaleDraw()->minBorderDist(fontMetrics(), d1, d2);
        mbd = QMAX(d1, d2);
    }

    if ( d_orient == Qt::Horizontal )
    {
        switch ( d_scalePos )
        {
            case Top:
                d_thermoRect.setRect(
                    r.x() + mbd + d_borderWidth,
                    r.y() + r.height()
                    - d_thermoWidth - 2*d_borderWidth,
                    r.width() - 2*(d_borderWidth + mbd),
                    d_thermoWidth);
                scaleDraw()->setGeometry(
                    d_thermoRect.x(),
                    d_thermoRect.y() - d_borderWidth - d_scaleDist,
                    d_thermoRect.width(),
                    QwtScaleDraw::Top);
                break;

            case Bottom:
            case None: // like Bottom but without scale
            default:   // inconsistent orientation and scale position
                       // Mapping between values and pixels requires
                       // initialization of the scale geometry
                d_thermoRect.setRect(
                    r.x() + mbd + d_borderWidth,
                    r.y() + d_borderWidth,
                    r.width() - 2*(d_borderWidth + mbd),
                    d_thermoWidth);
                scaleDraw()->setGeometry(
                    d_thermoRect.x(),
                    d_thermoRect.y() + d_thermoRect.height()
                    + d_borderWidth + d_scaleDist,
                    d_thermoRect.width(),
                    QwtScaleDraw::Bottom);
                break;
        }
        d_map.setIntRange(d_thermoRect.x(),
            d_thermoRect.x() + d_thermoRect.width() - 1);
    }
    else // Qt::Vertical
    {
        switch ( d_scalePos )
        {
            case Right:
                d_thermoRect.setRect(
                    r.x() + d_borderWidth,
                    r.y() + mbd + d_borderWidth,
                    d_thermoWidth,
                    r.height() - 2*(d_borderWidth + mbd));
                scaleDraw()->setGeometry(
                    d_thermoRect.x() + d_thermoRect.width()
                    + d_borderWidth + d_scaleDist,
                    d_thermoRect.y(),
                    d_thermoRect.height(),
                    QwtScaleDraw::Right);
                break;

            case Left:
            case None: // like Left but without scale
            default:   // inconsistent orientation and scale position
                       // Mapping between values and pixels requires
                       // initialization of the scale geometry
                d_thermoRect.setRect(
                    r.x() + r.width() - 2*d_borderWidth - d_thermoWidth,
                    r.y() + mbd + d_borderWidth,
                    d_thermoWidth,
                    r.height() - 2*(d_borderWidth + mbd));
                scaleDraw()->setGeometry(
                    d_thermoRect.x() - d_scaleDist - d_borderWidth,
                    d_thermoRect.y(),
                    d_thermoRect.height(),
                    QwtScaleDraw::Left);
                break;
        }
        d_map.setIntRange(d_thermoRect.y() + d_thermoRect.height() - 1,
            d_thermoRect.y());
    }
    if ( update_geometry )
    {
        updateGeometry();
        update();
    }
}

/*!
  \brief Set the thermometer orientation and the scale position.

  The scale position None disables the scale.
  \param o orientation. Possible values are Qt::Horizontal and Qt::Vertical.
         The default value is Qt::Vertical.
  \param s Position of the scale.
         The default value is None.

  A valid combination of scale position and orientation is enforced:
  - a horizontal thermometer can have the scale positions Top, Bottom or None;
  - a vertical thermometer can have the scale positions Left, Right or None;
  - an invalid scale position will default to None.

  \sa QwtThermo::setScalePosition()
*/
void QwtThermo::setOrientation(Qt::Orientation o, ScalePos s)
{
    switch(o)
    {
        case Qt::Horizontal:
            d_orient = Qt::Horizontal;
            if ((s == None) || (s == Bottom) || (s == Top))
                d_scalePos = s;
            else
                d_scalePos = None;
            break;

        case Qt::Vertical:
            d_orient = Qt::Vertical;
            if ((s == None) || (s == Left) || (s == Right))
                d_scalePos = s;
            else
                d_scalePos = None;
            break;
    }
    layoutThermo();
}

/*!
  \brief Change the scale position (and thermometer orientation).

  \param s Position of the scale.
  
  A valid combination of scale position and orientation is enforced:
  - if the new scale position is Left or Right, the scale orientation will
    become Qt::Vertical;
  - if the new scale position is Bottom or Top, the scale orientation will
    become Qt::Horizontal;
  - if the new scale position is None, the scale orientation will not change.

  \sa QwtThermo::setOrientation()
*/
void QwtThermo::setScalePosition(ScalePos s)
{
    if ((s == Bottom) || (s == Top))
        setOrientation(Qt::Horizontal, s);
    else if ((s == Left) || (s == Right))
        setOrientation(Qt::Vertical, s);
    else
        setOrientation(d_orient, None);
}

//! Return the scale position.
QwtThermo::ScalePos QwtThermo::scalePosition() const
{
    return d_scalePos;
}

//! Notify a font change.
void QwtThermo::fontChange(const QFont &f)
{
    QWidget::fontChange( f );
    layoutThermo();
}

//! Notify a scale change.
void QwtThermo::scaleChange()
{
    update();
    layoutThermo();
}

//! Redraw the liquid in thermometer pipe.
void QwtThermo::drawThermo(QPainter *p)
{
    int alarm  = 0, taval = 0;

    QRect fRect;
    QRect aRect;
    QRect bRect;

    int inverted = ( d_maxValue < d_minValue );

    //
    //  Determine if value exceeds alarm threshold.
    //  Note: The alarm value is allowed to lie
    //        outside the interval (minValue, maxValue).
    //
    if (d_alarmEnabled)
    {
        if (inverted)
        {
            alarm = ((d_alarmLevel >= d_maxValue)
                 && (d_alarmLevel <= d_minValue)
                 && (d_value >= d_alarmLevel));
        
        }
        else
        {
            alarm = (( d_alarmLevel >= d_minValue)
                 && (d_alarmLevel <= d_maxValue)
                 && (d_value >= d_alarmLevel));
        }
    }

    //
    //  transform values
    //
    int tval = d_map.limTransform(d_value);

    if (alarm)
       taval = d_map.limTransform(d_alarmLevel);

    //
    //  calculate recangles
    //
    if ( d_orient == Qt::Horizontal )
    {
        if (inverted)
        {
            bRect.setRect(d_thermoRect.x(), d_thermoRect.y(),
                  tval - d_thermoRect.x(),
                  d_thermoRect.height());
        
            if (alarm)
            {
                aRect.setRect(tval, d_thermoRect.y(),
                      taval - tval + 1,
                      d_thermoRect.height());
                fRect.setRect(taval + 1, d_thermoRect.y(),
                      d_thermoRect.x() + d_thermoRect.width() - (taval + 1),
                      d_thermoRect.height());
            }
            else
            {
                fRect.setRect(tval, d_thermoRect.y(),
                      d_thermoRect.x() + d_thermoRect.width() - tval,
                      d_thermoRect.height());
            }
        }
        else
        {
            bRect.setRect(tval + 1, d_thermoRect.y(),
                  d_thermoRect.width() - (tval + 1 - d_thermoRect.x()),
                  d_thermoRect.height());
        
            if (alarm)
            {
                aRect.setRect(taval, d_thermoRect.y(),
                      tval - taval + 1,
                      d_thermoRect.height());
                fRect.setRect(d_thermoRect.x(), d_thermoRect.y(),
                      taval - d_thermoRect.x(),
                      d_thermoRect.height());
            }
            else
            {
                fRect.setRect(d_thermoRect.x(), d_thermoRect.y(),
                      tval - d_thermoRect.x() + 1,
                      d_thermoRect.height());
            }
        
        }
    }
    else // Qt::Vertical
    {
        if (tval < d_thermoRect.y())
            tval = d_thermoRect.y();
        else 
        {
            if (tval > d_thermoRect.y() + d_thermoRect.height())
                tval = d_thermoRect.y() + d_thermoRect.height();
        }

        if (inverted)
        {
            bRect.setRect(d_thermoRect.x(), tval + 1,
            d_thermoRect.width(),
            d_thermoRect.height() - (tval + 1 - d_thermoRect.y()));

            if (alarm)
            {
                aRect.setRect(d_thermoRect.x(), taval,
                    d_thermoRect.width(),
                    tval - taval + 1);
                fRect.setRect(d_thermoRect.x(), d_thermoRect.y(),
                    d_thermoRect.width(),
                taval - d_thermoRect.y());
            }
            else
            {
                fRect.setRect(d_thermoRect.x(), d_thermoRect.y(),
                    d_thermoRect.width(),
                    tval - d_thermoRect.y() + 1);
            }
        }
        else
        {
            bRect.setRect(d_thermoRect.x(), d_thermoRect.y(),
            d_thermoRect.width(),
            tval - d_thermoRect.y());
            if (alarm)
            {
                aRect.setRect(d_thermoRect.x(),tval,
                    d_thermoRect.width(),
                    taval - tval + 1);
                fRect.setRect(d_thermoRect.x(),taval + 1,
                    d_thermoRect.width(),
                    d_thermoRect.y() + d_thermoRect.height() - (taval + 1));
            }
            else
            {
                fRect.setRect(d_thermoRect.x(),tval,
                    d_thermoRect.width(),
                d_thermoRect.y() + d_thermoRect.height() - tval);
            }
        }
    }

    //
    // paint thermometer
    //
    p->fillRect(bRect, colorGroup().color(QColorGroup::Background));

    if (alarm)
       p->fillRect(aRect, d_alarmBrush);

    p->fillRect(fRect, d_fillBrush);
}

//! Set the border width of the pipe.
void QwtThermo::setBorderWidth(int w)
{
    if ((w >= 0) && (w < (qwtMin(d_thermoRect.width(), 
        d_thermoRect.height()) + d_borderWidth) / 2  - 1))
    {
        d_borderWidth = w;
        layoutThermo();
    }
}

//! Return the border width of the thermometer pipe.
int QwtThermo::borderWidth() const
{
    return d_borderWidth;
}

/*!
  \brief Set the range
  \param vmin value corresponding lower or left end of the thermometer
  \param vmax value corresponding to the upper or right end of the thermometer
  \param logarithmic logarithmic mapping, TRUE or FALSE 
*/
void QwtThermo::setRange(double vmin, double vmax, bool logarithmic)
{
    d_minValue = vmin;
    d_maxValue = vmax;

    d_map.setDblRange(d_minValue, d_maxValue, logarithmic);
    if (!hasUserScale())
    {
        QwtScaleDiv oldscl(scaleDraw()->scaleDiv());

        scaleDraw()->setScale(d_minValue, d_maxValue,
            scaleMaxMajor(), scaleMaxMinor(), 0.0, logarithmic);
        if (oldscl != scaleDraw()->scaleDiv())
            scaleChange();
    }
    layoutThermo();
}

/*!
  \brief Change the brush of the liquid.
  \param c New brush. The default brush is solid black.
*/
void QwtThermo::setFillBrush(const QBrush& b)
{
    d_fillBrush = b;
    update();
}

//! Return the liquid brush.
const QBrush& QwtThermo::fillBrush() const
{
    return d_fillBrush;
}

/*!
  \brief Change the color of the liquid.
  \param c New color. The default color is black.
*/
void QwtThermo::setFillColor(const QColor &c)
{
    d_fillBrush.setColor(c);
    update();
}

//! Return the liquid color.
const QColor &QwtThermo::fillColor() const
{
    return d_fillBrush.color();
}

/*!
  \brief Specify the liquid brush above the alarm threshold
  \param c New brush. The default is solid white.
*/
void QwtThermo::setAlarmBrush(const QBrush& b)
{
    d_alarmBrush = b;
    update();
}

//! Return the liquid brush above the alarm threshold.
const QBrush& QwtThermo::alarmBrush() const
{
    return d_alarmBrush;
}

/*!
  \brief Specify the liquid color above the alarm threshold
  \param c New color. The default is white.
*/
void QwtThermo::setAlarmColor(const QColor &c)
{
    d_alarmBrush.setColor(c);
    update();
}

//! Return the liquid color above the alarm threshold.
const QColor &QwtThermo::alarmColor() const
{
    return d_alarmBrush.color();
}

//! Specify the alarm threshold.
void QwtThermo::setAlarmLevel(double v)
{
    d_alarmLevel = v;
    d_alarmEnabled = 1;
    update();
}

//! Return the alarm threshold.
double QwtThermo::alarmLevel() const
{
    return d_alarmLevel;
}

//! Change the width of the pipe.
void QwtThermo::setPipeWidth(int w)
{
    if (w > 0)
    {
        d_thermoWidth = w;
        layoutThermo();
    }
}

//! Return the width of the pipe.
int QwtThermo::pipeWidth() const
{
    return d_thermoWidth;
}


/*!
  \brief Specify the distance between the pipe's endpoints
         and the widget's border

  The margin is used to leave some space for the scale
  labels. If a large font is used, it is advisable to
  adjust the margins.
  \param m New Margin. The default values are 10 for
           horizontal orientation and 20 for vertical
           orientation.
  \warning The margin has no effect if the scale is disabled.
  \warning This function is a NOOP because margins are determined
           automatically.
*/
void QwtThermo::setMargin(int)
{
}


/*!
  \brief Enable or disable the alarm threshold
  \param tf TRUE (disabled) or FALSE (enabled)
*/
void QwtThermo::setAlarmEnabled(bool tf)
{
    d_alarmEnabled = tf;
    update();
}

//! Return if the alarm threshold is enabled or disabled.
bool QwtThermo::alarmEnabled() const
{
    return d_alarmEnabled;
}

/*!
  \return Fixed/MinimumExpanding for vertical, 
          MinimumExpanding/Fixed for horizontal thermos.
*/ 

QSizePolicy QwtThermo::sizePolicy() const
{
    QSizePolicy sp;
    if ( scaleDraw()->orientation() == QwtScaleDraw::Left ||
        scaleDraw()->orientation() == QwtScaleDraw::Right )
    {
        sp.setHorData( QSizePolicy::Fixed );
        sp.setVerData( QSizePolicy::MinimumExpanding );
    }
    else
    {
        sp.setHorData( QSizePolicy::MinimumExpanding );
        sp.setVerData( QSizePolicy::Fixed );
    }
    return sp;
}

/*!
  \return the minimum size hint
  \sa QwtThermo::minimumSizeHint
*/
QSize QwtThermo::sizeHint() const
{
    return minimumSizeHint();
}

/*!
  \brief Return a minimum size hint
  \warning The return value depends on the font and the scale.
  \sa QwtThermo::sizeHint
*/
QSize QwtThermo::minimumSizeHint() const
{
    int w = 0, h = 0;

    if ( d_scalePos != None )
    {
        int smw = scaleDraw()->minWidth( QPen(), fontMetrics() );
        int smh = scaleDraw()->minHeight( QPen(), fontMetrics() );

        if ( d_orient == Qt::Vertical )
        {
            w = d_thermoWidth + smw + 3 * d_borderWidth + d_scaleDist;
            h = smh + 2 * d_borderWidth;
        }
        else
        {
            w = smw + 2 * d_borderWidth;
            h = d_thermoWidth + smh + 3 * d_borderWidth + d_scaleDist;
        }

    }
    else // no scale
    {
        if ( d_orient == Qt::Vertical )
        {
            w = d_thermoWidth + 2 * d_borderWidth;
            h = 200 + 2 * d_borderWidth;
        }
        else
        {
            w = 200 + 2 * d_borderWidth;
            h = d_thermoWidth + 2 * d_borderWidth;
        }
    }
    return QSize( w, h );
}


/*!
  \brief Mouse Release Event management
*/
void QwtThermo::mouseReleaseEvent(QMouseEvent * e)
{
	if(e->button() == Qt::RightButton)
	{
		QString name = this->name();

		emit RightClicked(QString("QwtThermo"), name);
	}
}

// Local Variables:
// mode: C++
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
