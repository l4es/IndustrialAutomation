/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_SCALE_IF_H
#define QWT_SCALE_IF_H

#include "qwt_global.h"
#include "qwt_scldraw.h"

/*!
  \brief An interface class for widgets containing a scale 

  This interface class is used to provide classes a QwtScaleDraw and
  an interface to access that scale.

  \internal
  The primary purpose of this class is to define a common interface for classes
  which need a QwtScaleDraw class. It provides access to a QwtScaleDraw and a
  couple of public member functions which allow direct but restricted access 
  to this scale by the user.

  Widgets derived from this class have to implement the member function
  scaleChange(), which is called to notify changes of the scale parameters
  and usually requires repainting or resizing respectively.

  In general, a class derived from QwtScaleIf is  expected to manage the
  division and the position of its scale internally when no user-defined
  scale is set.  It should take the values returned by scaleMaxMinor()
  and scaleMaxMajor() into account. Those can be changed by the user.
  An implementation can check if a user-defined scale is set by calling the
  hasUserScale() member.
*/

class QWT_EXPORT QwtScaleIf
{
public:
    QwtScaleIf();
    virtual ~QwtScaleIf();
    
    void setScale (double vmin, double vmax, int logarithmic = 0);
    void setScale (double vmin, double vmax, double step, int logarithmic = 0);
    void setScale(const QwtScaleDiv &s);
    void setScaleMaxMajor( int ticks);
    void setScaleMaxMinor( int ticks);
    void autoScale();
    /*! \return max. number of minor tick intervals */
    int scaleMaxMinor() const {return d_maxMinor;}
    /*! \return max. number of major tick intervals */
    int scaleMaxMajor() const {return d_maxMajor;}

    void setScaleDraw(QwtScaleDraw *);
    const QwtScaleDraw *scaleDraw() const;

protected:
    //! Check for user-defined scale
    bool hasUserScale() {return d_userScale;}

    //! Notify changed scale
    virtual void scaleChange() = 0;
    QwtScaleDraw *scaleDraw();

private:
    QwtScaleDraw *d_scaleDraw;
    int d_maxMajor;
    int d_maxMinor;

    bool d_userScale;
};
#endif
