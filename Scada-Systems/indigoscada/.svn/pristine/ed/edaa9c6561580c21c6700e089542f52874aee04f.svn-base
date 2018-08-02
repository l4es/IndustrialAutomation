/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include <qevent.h>
#include "qwt_sldbase.h"
#include "qwt_math.h"

#ifndef WHEEL_DELTA
#define WHEEL_DELTA 120
#endif

//! Constructor
QwtSliderBase::QwtSliderBase(Qt::Orientation orient,
        QWidget *parent, const char *name, Qt::WFlags f): 
    QWidget(parent, name, f),
    d_scrollMode(ScrNone),
    d_orientation(orient),
    d_readOnly(FALSE)
{
    d_tmrID = 0;
    d_updTime = 150;
    d_mass = 0.0;
    d_tracking = TRUE;
    d_mouseOffset = 0.0;
    setRange(0.0,100.0, 1.0);
    setValue(0.0);

    setFocusPolicy(QWidget::TabFocus);
}

//! Destructor
QwtSliderBase::~QwtSliderBase()
{
    if(d_tmrID) 
        killTimer(d_tmrID);
}

/*!
  En/Disable read only mode

  In read only mode the slider can´t be controlled by mouse
  or keyboard.

  \param readOnly Enables in case of TRUE
  \sa isReadOnly
*/
void QwtSliderBase::setReadOnly(bool readOnly)
{
    d_readOnly = readOnly;
    update();
}

/*!
  In read only mode the slider can´t be controlled by mouse
  or keyboard.

  \return TRUE if read only
  \sa setReadOnly
*/
bool QwtSliderBase::isReadOnly() const
{
    return d_readOnly;
}

/*!
  \brief Set the orientation.
  \param o Orientation. Allowed values are
           Qt::Horizontal and Qt::Vertical.
*/
void QwtSliderBase::setOrientation(Orientation o)
{
    d_orientation = o;
}

/*! 
  \return Orientation
  \sa QwtSliderBase::setOrientation
*/
Qt::Orientation QwtSliderBase::orientation() const
{
    return d_orientation;
}

//! Stop updating if automatic scrolling is active

void QwtSliderBase::stopMoving() 
{
    if(d_tmrID)
    {
        killTimer(d_tmrID);
        d_tmrID = 0;
    }
}

/*!
  \brief Specify the update interval for automatic scrolling
  \param t update interval in milliseconds
  \sa QwtSliderBase::getScrollMode()
*/
void QwtSliderBase::setUpdateTime(int t) 
{
    if (t < 50) 
        t = 50;
    d_updTime = t;
}


//! Mouse press event handler
void QwtSliderBase::mousePressEvent(QMouseEvent *e) 
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

    const QPoint &p = e->pos();

    d_timerTick = 0;

    getScrollMode(p, d_scrollMode, d_direction);
    stopMoving();
    
    switch(d_scrollMode)
    {
        case ScrPage:
        case ScrTimer:
            d_mouseOffset = 0;
            d_tmrID = startTimer(qwtMax(250, 2 * d_updTime));
            break;
        
        case ScrMouse:
            d_time.start();
            d_speed = 0;
            d_mouseOffset = getValue(p) - value();
            emit sliderPressed();
            break;
        
        default:
            d_mouseOffset = 0;
            d_direction = 0;
            break;
    }
}


//! Emits a valueChanged() signal if necessary
void QwtSliderBase::buttonReleased()
{
    if ((!d_tracking) || (value() != prevValue()))
        emit valueChanged(value());
}


//! Mouse Release Event handler
void QwtSliderBase::mouseReleaseEvent(QMouseEvent *e)
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

    const double inc = step();
    
    switch(d_scrollMode) 
    {
        case ScrMouse:
        {
            setPosition(e->pos());
            d_direction = 0;
            d_mouseOffset = 0;
            if (d_mass > 0.0) 
            {
                const int ms = d_time.elapsed();
                if ((fabs(d_speed) >  0.0) && (ms < 50))
                    d_tmrID = startTimer(d_updTime);
            }
            else
            {
                d_scrollMode = ScrNone;
                buttonReleased();
            }
            emit sliderReleased();
            
            break;
        }

        case ScrDirect:
        {
            setPosition(e->pos());
            d_direction = 0;
            d_mouseOffset = 0;
            d_scrollMode = ScrNone;
            buttonReleased();
            break;
        }

        case ScrPage:
        {
            stopMoving();
            if (!d_timerTick)
                QwtDblRange::incPages(d_direction);
            d_timerTick = 0;
            buttonReleased();
            d_scrollMode = ScrNone;
            break;
        }

        case ScrTimer:
        {
            stopMoving();
            if (!d_timerTick)
                QwtDblRange::fitValue(value() + double(d_direction) * inc);
            d_timerTick = 0;
            buttonReleased();
            d_scrollMode = ScrNone;
            break;
        }

        default:
        {
            d_scrollMode = ScrNone;
            buttonReleased();
        }
    }
}


/*!
  Move the slider to a specified point, adjust the value
  and emit signals if necessary.
*/
void QwtSliderBase::setPosition(const QPoint &p) 
{
    QwtDblRange::fitValue(getValue(p) - d_mouseOffset);
}


/*!
  \brief Enables or disables tracking.

  If tracking is enabled, the slider emits a
  valueChanged() signal whenever its value
  changes (the default behaviour). If tracking
  is disabled, the value changed() signal will only
  be emitted if:<ul>
  <li>the user releases the mouse
      button and the value has changed or
  <li>at the end of automatic scrolling.</ul>
  Tracking is enabled by default.
  \param enable \c TRUE (enable) or \c FALSE (disable) tracking.
*/
void QwtSliderBase::setTracking(bool enable)
{
    d_tracking = enable;
}

//! Mouse Move Event handler
void QwtSliderBase::mouseMoveEvent(QMouseEvent *e)
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

    if (d_scrollMode == ScrMouse )
    {
        setPosition(e->pos());
        if (d_mass > 0.0) 
        {
            double ms = double(d_time.elapsed());
            if (ms < 1.0) 
                ms = 1.0;
            d_speed = (exactValue() - exactPrevValue()) / ms;
            d_time.start();
        }
        if (value() != prevValue())
            emit sliderMoved(value());
    }
}

//! Qt wheel event
void QwtSliderBase::wheelEvent(QWheelEvent *e)
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

    int mode = ScrNone, direction = 0;

    // Give derived classes a chance to say ScrNone
    getScrollMode(e->pos(), mode, direction);
    if ( mode != ScrNone )
    {
        const int inc = e->delta() / WHEEL_DELTA;
        QwtDblRange::incPages(inc);
        if (value() != prevValue())
            emit sliderMoved(value());
    }
}

/*!
  Handles key events

  - Key_Down, KeyLeft\n
    Decrement by 1
  - Key_Up, Key_Right\n
    Increment by 1

  \sa isReadOnly()
*/

void QwtSliderBase::keyPressEvent(QKeyEvent *e)
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

    int increment = 0;
    switch ( e->key() ) 
    {
        case Qt::Key_Down:
            if ( orientation() == Qt::Vertical )
                increment = -1;
            break;
        case Qt::Key_Up:
            if ( orientation() == Qt::Vertical )
                increment = 1;
            break;
        case Qt::Key_Left:
            if ( orientation() == Qt::Horizontal )
                increment = -1;
            break;
        case Qt::Key_Right:
            if ( orientation() == Qt::Horizontal )
                increment = 1;
            break;
        default:;
#if QT_VERSION >= 300
            e->ignore();
#endif
    }

    if ( increment != 0 )
    {
        QwtDblRange::incValue(increment);
        if (value() != prevValue())
            emit sliderMoved(value());
    }
}

//! Qt timer event
void QwtSliderBase::timerEvent(QTimerEvent *)
{
    const double inc = step();

    switch (d_scrollMode)
    {
        case ScrMouse:
        {
            if (d_mass > 0.0)
            {
                d_speed *= exp( - double(d_updTime) * 0.001 / d_mass );
                const double newval = 
                    exactValue() + d_speed * double(d_updTime);
                QwtDblRange::fitValue(newval);
                // stop if d_speed < one step per second
                if (fabs(d_speed) < 0.001 * fabs(step()))
                {
                    d_speed = 0;
                    stopMoving();
                    buttonReleased();
                }

            }
            else
               stopMoving();
            break;
        }

        case ScrPage:
        {
            QwtDblRange::incPages(d_direction);
            if (!d_timerTick) 
            {
                killTimer(d_tmrID);
                d_tmrID = startTimer(d_updTime);
            }
            break;
        }
        case ScrTimer:
        {
            QwtDblRange::fitValue(value() +  double(d_direction) * inc);
            if (!d_timerTick) 
            {
                killTimer(d_tmrID);
                d_tmrID = startTimer(d_updTime);
            }
            break;
        }
        default:
        {
            stopMoving();
            break;
        }
    }

    d_timerTick = 1;
}


/*!
  Notify change of value

  This function can be reimplemented by derived classes
  in order to keep track of changes, i.e. repaint the widget.
  The default implementation emits a valueChanged() signal
  if tracking is enabled.
*/
void QwtSliderBase::valueChange() 
{
    if (d_tracking)
       emit valueChanged(value());  
}

/*!
  \brief Set the slider's mass for flywheel effect.

  If the slider's mass is greater then 0, it will continue
  to move after the mouse button has been released. Its speed
  decreases with time at a rate depending on the slider's mass.
  A large mass means that it will continue to move for a
  long time.

  Derived widgets may overload this function to make it public.

  \param val new mass in kg

  \bug If the mass is smaller than 1g, it is set to zero.
       The maximal mass is limited to 100kg.
  \sa QwtSliderBase::mass
*/
void QwtSliderBase::setMass(double val)
{
    if (val < 0.001)
       d_mass = 0.0;
    else if (val > 100.0)
       d_mass = 100.0;
    else
       d_mass = val;
}

/*!
    \return mass
    \sa QwtSliderBase::setMass
*/
double QwtSliderBase::mass() const
{   
    return d_mass; 
}


/*!
  \brief Move the slider to a specified value

  This function can be used to move the slider to a value
  which is not an integer multiple of the step size.
  \param val new value
  \sa QwtSliderBase::fitValue
*/
void QwtSliderBase::setValue(double val)
{
    if (d_scrollMode == ScrMouse) 
        stopMoving();
    QwtDblRange::setValue(val);
}


/*!
  \brief Set the slider's value to the nearest integer multiple
         of the step size.
     \sa QwtSliderBase::setValue()
*/
void QwtSliderBase::fitValue(double val)
{
    if (d_scrollMode == ScrMouse) 
        stopMoving();
    QwtDblRange::fitValue(val);
}


/*!
  \brief Increment the value by a specified number of steps
  \param steps number of steps
*/
void QwtSliderBase::incValue(int steps)
{
    if (d_scrollMode == ScrMouse) 
        stopMoving();
    QwtDblRange::incValue(steps);
}
