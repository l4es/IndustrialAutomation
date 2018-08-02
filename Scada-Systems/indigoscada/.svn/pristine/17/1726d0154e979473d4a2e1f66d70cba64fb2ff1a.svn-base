/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include "qwt_sclif.h"

//! Constructor
QwtScaleIf::QwtScaleIf()
{
    d_userScale = FALSE;
    d_maxMajor = 5;
    d_maxMinor = 3;

    d_scaleDraw = new QwtScaleDraw();
    d_scaleDraw->setScale(0.0,100.0,d_maxMajor, d_maxMinor);
}

//! Destructor
QwtScaleIf::~QwtScaleIf()
{
    delete d_scaleDraw;
}

/*!
  \brief Specify a user-defined scale.

  By default, the widget is supposed to control the range of its scale
  automatically, but sometimes it is desirable to have a user-defined
  scale which is not in sync with
  the widget's range, e.g. if a logarithmic scale is needed
  (sliders don't support that) or if the scale is required
  to have a fixed range (say 0...100%), independent of the
  widget's range. 
  \param vmin
  \param vmax boundary values
  \param logarithmic If != 0, Build a logarithmic scale
  \sa QwtScaleIf::autoScale()
*/
void QwtScaleIf::setScale(double vmin, double vmax, int logarithmic)
{
    setScale(vmin,vmax,0.0,logarithmic);
}


/*!
  \brief Specify a user-defined scale.

  By default, the widget is supposed to control the range of its scale
  automatically, but sometimes it is desirable to have a user-defined
  scale which is not in sync with
  the widget's range, e.g. if a logarithmic scale is needed
  (sliders don't support that) or if the scale is required
  to have a fixed range (say 0...100%), independent of the
  widget's range. 
  \param vmin
  \param vmax interval boundaries
  \param step major step size
  \param logarithmic If != 0, build a logarithmic scale
  \sa QwtScaleIf::autoScale()
*/
void QwtScaleIf::setScale(double vmin, double vmax, 
    double step, int logarithmic)
{
    QwtScaleDiv oldscl(d_scaleDraw->scaleDiv());
    
    d_scaleDraw->setScale(vmin, vmax, d_maxMajor, d_maxMinor, step, logarithmic);
    d_userScale = TRUE;
    if (oldscl != d_scaleDraw->scaleDiv())
       scaleChange();
}

/*!
  Assign a user-defined scale division
  \param s scale division
*/
void QwtScaleIf::setScale(const QwtScaleDiv &s)
{
    if (s != d_scaleDraw->scaleDiv())
    {
        d_scaleDraw->setScale(s);
        d_maxMajor = s.majCnt();
        d_maxMinor = s.minCnt();
        d_userScale = TRUE;
        scaleChange();
    }
}

/*!
  \brief Advise the widget to control the scale range internally.

  Autoscaling is on by default. 
  \sa QwtScaleIf::setScale()
*/
void QwtScaleIf::autoScale()
{
    if (!d_userScale) 
    {
        d_userScale = FALSE;
        scaleChange();
    }
}

/*!
  \brief Set the maximum number of major tick intervals.

  The scale's major ticks are calculated automatically such that
  the number of major intervals does not exceed ticks.
  The default value is 5.
  \param ticks maximal number of major ticks.
  \sa QwtScaleDraw
*/
void QwtScaleIf::setScaleMaxMajor(int ticks)
{
    if (ticks != d_maxMajor)
    {
        d_maxMajor = ticks;
        d_scaleDraw->setScale(d_scaleDraw->scaleDiv().lBound(), 
        d_scaleDraw->scaleDiv().hBound(),
        d_maxMajor, d_maxMinor, 0.0,d_scaleDraw->scaleDiv().logScale()); 
        scaleChange();
    }
}

/*!
  \brief Set the maximum number of minor tick intervals

  The scale's minor ticks are calculated automatically such that
  the number of minor intervals does not exceed ticks.
  The default value is 3.
  \param ticks
  \sa QwtScaleDraw
*/
void QwtScaleIf::setScaleMaxMinor(int ticks)
{
    if ( ticks != d_maxMinor)
    {
        d_maxMinor = ticks;
        d_scaleDraw->setScale(d_scaleDraw->scaleDiv().lBound(), 
            d_scaleDraw->scaleDiv().hBound(),
            d_maxMajor, d_maxMinor, 0.0, d_scaleDraw->scaleDiv().logScale()); 
        scaleChange();
    }
}

/*!
  \brief Set a scale draw
  sd has to be created with new and will be deleted in
  QwtScaleIf::~QwtScaleIf or the next call of QwtScaleIf::setScaleDraw.
*/
void QwtScaleIf::setScaleDraw(QwtScaleDraw *sd)
{
    if ( sd == NULL || sd == d_scaleDraw )
        return;

    delete d_scaleDraw;
    d_scaleDraw = sd;
} 

/*!
    \return scale draw
    \sa QwtScaleIf::setScaleDraw
*/
QwtScaleDraw *QwtScaleIf::scaleDraw() 
{
    return d_scaleDraw;
}

/*!
    \return scale draw
    \sa QwtScaleIf::setScaleDraw
*/
const QwtScaleDraw *QwtScaleIf::scaleDraw() const
{
    return d_scaleDraw;
}

