/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_COMPASS_H
#define QWT_COMPASS_H 1

#include <qstring.h>
#include <qmap.h>
#include "qwt_dial.h"

#if defined(QWT_TEMPLATEDLL)
// MOC_SKIP_BEGIN
template class QWT_EXPORT QMap<double, QString>;
// MOC_SKIP_END
#endif

class QwtCompassRose;

/*!
  \brief A Compass Widget

  QwtCompass is a widget to display and enter directions. It consists
  of a scale, an optional needle and rose. 

  \image html compass.gif 

  \note The examples/dials example shows how to use QwtCompass.
*/

class QWT_EXPORT QwtCompass: public QwtDial 
{
    Q_OBJECT

public:
    QwtCompass( QWidget* parent=0, const char* name = 0);
    virtual ~QwtCompass();

    void setRose(QwtCompassRose *rose);
    const QwtCompassRose *rose() const;
    QwtCompassRose *rose();

    const QMap<double, QString> &labelMap() const;
    QMap<double, QString> &labelMap();
    void setLabelMap(const QMap<double, QString> &map);

protected:
    virtual QString scaleLabel(double value) const;

    virtual void drawRose(QPainter *, const QPoint &center,
        int radius, double north, QPalette::ColorGroup) const;

    virtual void drawScaleContents(QPainter *, 
        const QPoint &center, int radius) const; 

    virtual void keyPressEvent(QKeyEvent *);

private:
    QwtCompassRose *d_rose;
    QMap<double, QString> d_labelMap;
};

#endif
