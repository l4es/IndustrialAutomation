/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_SLDBASE_H
#define QWT_SLDBASE_H

#include <qframe.h>
#include <qpixmap.h>
#include <qdatetime.h>
#include "qwt_global.h"
#include "qwt_drange.h"

/*!
  \brief A Base class for sliders

  QwtSliderBase is a base class for
  slider widgets. QwtSliderBase handles the mouse events
  and updates the slider's value accordingly. Derived classes
  only have to implement the getValue() and 
  getScrollMode() members, and should react to a
  valueChange(), which normally requires repainting. 
*/

class QWT_EXPORT QwtSliderBase : public QWidget, public QwtDblRange
{
    Q_OBJECT 
    Q_PROPERTY( bool readOnly READ isReadOnly WRITE setReadOnly )
    Q_PROPERTY( bool valid READ isValid WRITE setValid )
    Q_PROPERTY( double mass READ mass WRITE setMass )
    Q_PROPERTY( Orientation orientation READ orientation WRITE setOrientation )

public:
    enum ScrollMode { ScrNone, ScrMouse, 
        ScrTimer, ScrDirect, ScrPage };
    
    QwtSliderBase(Qt::Orientation orientation, QWidget *parent = 0, 
        const char *name = 0, Qt::WFlags flags = 0 );
    virtual ~QwtSliderBase();

    void setUpdateTime(int t);
    void stopMoving();
    void setTracking(bool enable);
    
    virtual void setMass(double val);
    virtual double mass() const;

    virtual void setOrientation(Orientation o);
    Orientation orientation() const;

    bool isReadOnly() const;

    /* 
        Wrappers for QwtDblRange::isValid/QwtDblRange::setValid made
        to be available as Q_PROPERTY in the designer.
    */

    /*! 
      \sa QwtDblRange::isValid
    */
    bool isValid() const { return QwtDblRange::isValid(); }

    /*! 
      \sa QwtDblRange::isValid
    */
    void setValid(bool valid) { QwtDblRange::setValid(valid); }

public slots:
    virtual void setValue(double val);
    virtual void fitValue(double val);
    virtual void incValue(int steps);

    virtual void setReadOnly(bool); 

signals:

    /*!
      \brief Notify a change of value.

      In the default setting 
      (tracking enabled), this signal will be emitted every 
      time the value changes ( see setTracking() ). 
      \param value new value
    */
    void valueChanged(double value);

    /*!
      This signal is emitted when the user presses the 
      movable part of the slider (start ScrMouse Mode).
    */
    void sliderPressed();

    /*!
      This signal is emitted when the user releases the 
      movable part of the slider.
    */

    void sliderReleased();
    /*!
      This signal is emitted when the user moves the
      slider with the mouse.
      \param value new value
    */
    void sliderMoved(double value);
    
protected:
    void setPosition(const QPoint &p);
    virtual void valueChange();

    virtual void timerEvent(QTimerEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void wheelEvent(QWheelEvent *e);

  /*!
    \brief Determine the value corresponding to a specified poind

    This is an abstract virtual function which is called when
    the user presses or releases a mouse button or moves the
    mouse. It has to be implemented by the derived class.
    \param p point 
  */
    virtual double getValue(const QPoint & p) = 0;
  /*!
    \brief Determine what to do when the user presses a mouse button.

    This function is abstract and has to be implemented by derived classes.
    It is called on a mousePress event. The derived class can determine
    what should happen next in dependence of the position where the mouse
    was pressed by returning scrolling mode and direction. QwtSliderBase
    knows the following modes:<dl>
    <dt>QwtSliderBase::ScrNone
    <dd>Scrolling switched off. Don't change the value.
    <dt>QwtSliderBase::ScrMouse
    <dd>Change the value while the user keeps the
        button pressed and moves the mouse.
    <dt>QwtSliderBase::ScrTimer
    <dd>Automatic scrolling. Increment the value
        in the specified direction as long as
    the user keeps the button pressed.
    <dt>QwtSliderBase::ScrPage
    <dd>Automatic scrolling. Same as ScrTimer, but
        increment by page size.</dl>

    \param p point where the mouse was pressed
    \retval scrollMode The scrolling mode
    \retval direction  direction: 1, 0, or -1.
  */
    virtual void getScrollMode( const QPoint &p,
                  int &scrollMode, int &direction) = 0;

    int d_scrollMode;
    double d_mouseOffset;
    int d_direction;
    int d_tracking;

private:
    void buttonReleased();

    int d_tmrID;
    int d_updTime;
    int d_timerTick;
    QTime d_time;
    double d_speed;
    double d_mass;
    Qt::Orientation d_orientation;
    bool d_readOnly;
};

#endif
