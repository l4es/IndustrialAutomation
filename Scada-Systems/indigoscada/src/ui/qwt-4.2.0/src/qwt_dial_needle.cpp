/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include <math.h>
#include <qapplication.h>
#include <qpainter.h>
#include "qwt_math.h"
#include "qwt_painter.h"
#include "qwt_dial_needle.h"

//! Constructor
QwtDialNeedle::QwtDialNeedle():
    d_palette(QApplication::palette())
{
}

//! Destructor
QwtDialNeedle::~QwtDialNeedle() 
{
}

/*!
    Sets the palette for the needle.

    \param palette New Palette
*/
void QwtDialNeedle::setPalette(const QPalette &palette) 
{ 
    d_palette = palette; 
}

/*!
  \return the palette of the needle.
*/
const QPalette &QwtDialNeedle::palette() const 
{ 
    return d_palette; 
}

//!  Draw the knob 
void QwtDialNeedle::drawKnob(QPainter *painter,
    const QPoint &pos, int width, const QBrush &brush, bool sunken)
{
    painter->save();

    QRect rect(0, 0, width, width);
    rect.moveCenter(pos);

    painter->setPen(Qt::NoPen);
    painter->setBrush(brush);
    painter->drawEllipse(rect);

    painter->setBrush(Qt::NoBrush);

    const int colorOffset = 20;

    int startAngle = 45;
    if ( sunken )
        startAngle += 180;

    QPen pen;
    pen.setWidth(1);

    pen.setColor(brush.color().dark(100 - colorOffset));
    painter->setPen(pen);
    painter->drawArc(rect, startAngle * 16, 180 * 16);

    pen.setColor(brush.color().dark(100 + colorOffset));
    painter->setPen(pen);
    painter->drawArc(rect, (startAngle + 180) * 16, 180 * 16);

    painter->restore();
}

/*! 
    \param cg Identifier of the color group
    \return The color group of the palette.
*/
const QColorGroup &QwtDialNeedle::colorGroup(QPalette::ColorGroup cg) const
{
    switch(cg)
    {
        case QPalette::Disabled:
            return d_palette.disabled();
        case QPalette::Inactive:
            return d_palette.inactive();
        default:
            return d_palette.active();
    }
}

/*!
  Constructor
*/
QwtDialSimpleNeedle::QwtDialSimpleNeedle(Style style, bool hasKnob, 
        const QColor &mid, const QColor &base):
    d_style(style),
    d_hasKnob(hasKnob),
    d_width(-1)
{
    QPalette palette;
    for ( int i = 0; i < QPalette::NColorGroups; i++ )
    {
        palette.setColor((QPalette::ColorGroup)i,
            QColorGroup::Mid, mid);
        palette.setColor((QPalette::ColorGroup)i,
            QColorGroup::Base, base);
    }

    setPalette(palette);
}

//! Set the width of the needle
void QwtDialSimpleNeedle::setWidth(int width)
{
    d_width = width;
}

/*!
  \return the width of the needle
*/
int QwtDialSimpleNeedle::width() const
{
    return d_width;
}

/*!
 Draw the needle

 \param painter Painter
 \param center Center of the dial, start position for the needle
 \param length Length of the needle
 \param direction Direction of the needle, in degrees counter clockwise
 \param cg Color group, used for painting
*/
void QwtDialSimpleNeedle::draw(QPainter *painter, const QPoint &center,
    int length, double direction, QPalette::ColorGroup cg) const
{
    if ( d_style == Arrow )
    {
        drawArrowNeedle(painter, colorGroup(cg),
            center, length, d_width, direction, d_hasKnob);
    }
    else
    {
        drawRayNeedle(painter, colorGroup(cg), center, length, d_width,
            direction, d_hasKnob); 
    }
}

/*!
  Draw a needle looking like a ray
*/
void QwtDialSimpleNeedle::drawRayNeedle(
    QPainter *painter, const QColorGroup &cg,
    const QPoint &center, int length, int width, double direction, 
    bool hasKnob)
{
    if ( width <= 0 )
        width = 5;

    direction *= M_PI / 180.0;

    painter->save();

    const QPoint p1(center.x() + 1, center.y() + 2);
    const QPoint p2 = qwtPolar2Pos(p1, length, direction);

    if ( width == 1 )
    {
        painter->setPen(QPen(cg.mid(), 1));
        painter->drawLine(p1, p2);
    }
    else
    {
        QPointArray pa(4);
        pa.setPoint(0, qwtPolar2Pos(p1, width / 2, direction + M_PI_2));
        pa.setPoint(1, qwtPolar2Pos(p2, width / 2, direction + M_PI_2));
        pa.setPoint(2, qwtPolar2Pos(p2, width / 2, direction - M_PI_2));
        pa.setPoint(3, qwtPolar2Pos(p1, width / 2, direction - M_PI_2));

        painter->setPen(Qt::NoPen);
        painter->setBrush(cg.brush(QColorGroup::Mid));
        painter->drawPolygon(pa);
    }
    if ( hasKnob )
    {
        int knobWidth = QMAX(qRound(width * 0.7), 5);
        if ( knobWidth % 2 == 0 )
            knobWidth++;

        drawKnob(painter, center, knobWidth, 
            cg.brush(QColorGroup::Base), FALSE);
    }

    painter->restore();
}

/*!
  Draw a needle looking like an arrow
*/
void QwtDialSimpleNeedle::drawArrowNeedle(
    QPainter *painter, const QColorGroup &cg,
    const QPoint &center, int length, int width,
    double direction, bool hasKnob)
{
    direction *= M_PI / 180.0;

    painter->save();

    if ( width <= 0 )
    {
        width = (int)QMAX(length * 0.06, 9);
        if ( width % 2 == 0 )
            width++;
    }

    const int peak = 3;
    const QPoint p1(center.x() + 1, center.y() + 1);
    const QPoint p2 = qwtPolar2Pos(p1, length - peak, direction);
    const QPoint p3 = qwtPolar2Pos(p1, length, direction);

    QPointArray pa(5);
    pa.setPoint(0, qwtPolar2Pos(p1, width / 2, direction - M_PI_2));
    pa.setPoint(1, qwtPolar2Pos(p2, 1, direction - M_PI_2));
    pa.setPoint(2, p3);
    pa.setPoint(3, qwtPolar2Pos(p2, 1, direction + M_PI_2));
    pa.setPoint(4, qwtPolar2Pos(p1, width / 2, direction + M_PI_2));

    painter->setPen(Qt::NoPen);
    painter->setBrush(cg.brush(QColorGroup::Mid));
    painter->drawPolygon(pa);

    QPointArray shadowPa(3);

    const int colorOffset = 10;

    int i;
    for ( i = 0; i < 3; i++ )
        shadowPa.setPoint(i, pa[i]);

    painter->setPen(cg.mid().dark(100 + colorOffset));
    painter->drawPolyline(shadowPa);

    for ( i = 0; i < 3; i++ )
        shadowPa.setPoint(i, pa[i + 2]);

    painter->setPen(cg.mid().dark(100 - colorOffset));
    painter->drawPolyline(shadowPa);

    if ( hasKnob )
    {
        drawKnob(painter, center, qRound(width * 1.3), 
            cg.brush(QColorGroup::Base), FALSE);
    }

    painter->restore();
}

//! Constructor

QwtCompassMagnetNeedle::QwtCompassMagnetNeedle(Style style,
        const QColor &light, const QColor &dark):
    d_style(style)
{   
    QPalette palette;
    for ( int i = 0; i < QPalette::NColorGroups; i++ )
    {
        palette.setColor((QPalette::ColorGroup)i,
            QColorGroup::Light, light);
        palette.setColor((QPalette::ColorGroup)i,
            QColorGroup::Dark, dark);
        palette.setColor((QPalette::ColorGroup)i,
            QColorGroup::Base, Qt::darkGray);
    }

    setPalette(palette); 
}

/*!
    Draw the needle

    \param painter Painter
    \param center Center of the dial, start position for the needle
    \param length Length of the needle
    \param direction Direction of the needle, in degrees counter clockwise
    \param cg Color group, used for painting
*/
void QwtCompassMagnetNeedle::draw(QPainter *painter, const QPoint &center,
    int length, double direction, QPalette::ColorGroup cg) const
{
    if ( d_style == ThinStyle )
    {
        drawThinNeedle(painter, colorGroup(cg), 
            center, length, direction); 
    }
    else
    {
        drawTriangleNeedle(painter, colorGroup(cg),
            center, length, direction);
    }
}

/*!
  Draw a compass needle 
*/
void QwtCompassMagnetNeedle::drawTriangleNeedle(
    QPainter *painter, const QColorGroup &cg,
    const QPoint &center, int length, double direction) 
{
    QBrush brush;

    const int width = qRound(length / 3.0);
    const int colorOffset =  10;

    painter->save();
    painter->setPen(Qt::NoPen);

    const QPoint arrowCenter(center.x() + 1, center.y() + 1);

    QPointArray pa(3);
    pa.setPoint(0, arrowCenter);
    pa.setPoint(1, qwtDegree2Pos(arrowCenter, length, direction));

    pa.setPoint(2, qwtDegree2Pos(arrowCenter, width / 2, direction + 90.0));

    brush = cg.brush(QColorGroup::Dark);
    brush.setColor(brush.color().dark(100 + colorOffset));
    painter->setBrush(brush);
    painter->drawPolygon(pa);

    pa.setPoint(2, qwtDegree2Pos(arrowCenter, width / 2, direction - 90.0));

    brush = cg.brush(QColorGroup::Dark);
    brush.setColor(brush.color().dark(100 - colorOffset));
    painter->setBrush(brush);
    painter->drawPolygon(pa);

    // --

    pa.setPoint(1, qwtDegree2Pos(arrowCenter, length, direction + 180.0));

    pa.setPoint(2, qwtDegree2Pos(arrowCenter, width / 2, direction + 90.0));

    brush = cg.brush(QColorGroup::Light);
    brush.setColor(brush.color().dark(100 + colorOffset));
    painter->setBrush(brush);
    painter->drawPolygon(pa);

    pa.setPoint(2, qwtDegree2Pos(arrowCenter, width / 2, direction - 90.0));

    brush = cg.brush(QColorGroup::Light);
    brush.setColor(brush.color().dark(100 - colorOffset));
    painter->setBrush(brush);
    painter->drawPolygon(pa);

    painter->restore();
}

/*!
  Draw a compass needle 
*/
void QwtCompassMagnetNeedle::drawThinNeedle(
    QPainter *painter, const QColorGroup &cg,
    const QPoint &center, int length, double direction) 
{
    const int colorOffset = 10;
    const int width = QMAX(qRound(length / 6.0), 3);

    painter->save();

    const QPoint arrowCenter(center.x() + 1, center.y() + 1);

    drawPointer(painter, cg.brush(QColorGroup::Dark), colorOffset, 
        arrowCenter, length, width, direction);
    drawPointer(painter, cg.brush(QColorGroup::Light), -colorOffset, 
        arrowCenter, length, width, direction + 180.0);
    
    drawKnob(painter, arrowCenter, width, 
        cg.brush(QColorGroup::Base), TRUE);

    painter->restore();
}

/*!
  Draw a compass needle 
*/
void QwtCompassMagnetNeedle::drawPointer(
    QPainter *painter, const QBrush &brush,
    int colorOffset, const QPoint &center, int length, 
    int width, double direction)
{
    painter->save();

    const int peak = QMAX(qRound(length / 10.0), 5);

    const int knobWidth = width + 8;
    QRect knobRect(0, 0, knobWidth, knobWidth);
    knobRect.moveCenter(center);

    QPointArray pa(5);

    pa.setPoint(0, qwtDegree2Pos(center, width / 2, direction + 90.0));
    pa.setPoint(1, center);
    pa.setPoint(2, qwtDegree2Pos(pa.point(1), length - peak, direction));
    pa.setPoint(3, qwtDegree2Pos(center, length, direction));
    pa.setPoint(4, qwtDegree2Pos(pa.point(0), length - peak, direction));

    painter->setPen(Qt::NoPen);

    QBrush darkBrush = brush;
    darkBrush.setColor(darkBrush.color().dark(100 + colorOffset));
    painter->setBrush(darkBrush);
    painter->drawPolygon(pa);
    painter->drawPie(knobRect, qRound(direction * 16), 90 * 16);

    pa.setPoint(0, qwtDegree2Pos(center, width / 2, direction - 90.0));
    pa.setPoint(4, qwtDegree2Pos(pa.point(0), length - peak, direction));

    QBrush lightBrush = brush;
    lightBrush.setColor(lightBrush.color().dark(100 - colorOffset));
    painter->setBrush(lightBrush);
    painter->drawPolygon(pa);
    painter->drawPie(knobRect, qRound(direction * 16), -90 * 16);

    painter->restore();
}

//! Constructor

QwtCompassWindArrow::QwtCompassWindArrow(Style style, 
        const QColor &light, const QColor &dark):
    d_style(style)
{
    QPalette palette;
    for ( int i = 0; i < QPalette::NColorGroups; i++ )
    {
        palette.setColor((QPalette::ColorGroup)i,
            QColorGroup::Light, light);
        palette.setColor((QPalette::ColorGroup)i,
            QColorGroup::Dark, dark);
    }

    setPalette(palette);
}

/*!
 Draw the needle

 \param painter Painter
 \param center Center of the dial, start position for the needle
 \param length Length of the needle
 \param direction Direction of the needle, in degrees counter clockwise
 \param cg Color group, used for painting
*/
void QwtCompassWindArrow::draw(QPainter *painter, const QPoint &center,
    int length, double direction, QPalette::ColorGroup cg) const
{
    if ( d_style == Style1 )
    {
        drawStyle1Needle(painter, colorGroup(cg),
            center, length, direction);
    }
    else
    {
        drawStyle2Needle(painter, colorGroup(cg),
            center, length, direction); 
    }
}

/*!
  Draw a compass needle 
*/
void QwtCompassWindArrow::drawStyle1Needle(
    QPainter *painter, const QColorGroup &cg,
    const QPoint &center, int length, double direction) 
{
    const double AR1[] = {0, 0.4, 0.3, 1, 0.8, 1, 0.3, 0.4};
    const double AW1[] = {0, -45, -20, -15, 0, 15, 20, 45};

    const QPoint arrowCenter(center.x() + 1, center.y() + 1);

    QPointArray pa(8);
    pa.setPoint(0, arrowCenter);
    for (int i=1; i<8; i++) 
    {
        const QPoint p = qwtDegree2Pos(center, 
            AR1[i] * length, direction + AW1[i]);
        pa.setPoint(i, p);
    }

    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(cg.brush(QColorGroup::Light));
    painter->drawPolygon(pa);
    painter->restore();
}

/*!
  Draw a compass needle 
*/
void QwtCompassWindArrow::drawStyle2Needle(
    QPainter *painter, const QColorGroup &cg,
    const QPoint &center, int length, double direction) 
{
    painter->save();
    painter->setPen(Qt::NoPen);

    const double angle = 12.0;
    const double ratio = 0.7;

    const QPoint arrowCenter(center.x() + 1, center.y() + 1);

    QPointArray pa(3);

    pa.setPoint(0, center);
    pa.setPoint(2, qwtDegree2Pos(arrowCenter, ratio * length, direction));

    pa.setPoint(1, qwtDegree2Pos(arrowCenter, length, direction + angle));
    painter->setBrush(cg.brush(QColorGroup::Dark));
    painter->drawPolygon(pa);

    pa.setPoint(1, qwtDegree2Pos(arrowCenter, length, direction - angle));
    painter->setBrush(cg.brush(QColorGroup::Light));
    painter->drawPolygon(pa);

    painter->restore();
}

