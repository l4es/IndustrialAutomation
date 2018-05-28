/*****************************************************************************
 *
 * Dial / analog gauge.
 *
 * Copyright (C) 2012 - 2017  Richard Hacker <ha@igh.de>
 *                            Florian Pose <fp@igh.de>
 *
 * This file is part of the QtPdWidgets library.
 *
 * The QtPdWidgets library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * The QtPdWidgets library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the QtPdWidgets Library. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

#include "QtPdWidgets/Dial.h"

#include <QPainter>
#include <QEvent>
#include <QWheelEvent>
#include <QtCore/qmath.h>

//#include <QDebug>

using Pd::Dial;

#define DEFAULT_TITLE ""
#define DEFAULT_UNIT ""
#define DEFAULT_SPAN 280
#define DEFAULT_SCALEMIN 0.0
#define DEFAULT_MAJORSTEP 10.0
#define DEFAULT_MAJORSTOPS 8
#define DEFAULT_MINORSTOPS 5
#define DEFAULT_PIECOLOR QColor()

/****************************************************************************/

struct QPainterSave {
    QPainterSave(QPainter& painter): painter(painter) { painter.save(); }
    ~QPainterSave() { painter.restore(); }

    QPainter& painter;
};

/****************************************************************************/

Dial::Dial(QWidget* parent):
    QFrame(parent),
    currentValue(this),
    setpointValue(this),
    title(DEFAULT_TITLE),
    unit(DEFAULT_UNIT),
    span(DEFAULT_SPAN),
    scaleMin(DEFAULT_SCALEMIN),
    majorStep(DEFAULT_MAJORSTEP),
    majorStops(DEFAULT_MAJORSTOPS),
    minorStops(DEFAULT_MINORSTOPS),
    pieColor(DEFAULT_PIECOLOR),
    minorStopOffset(78),
    minorStopLength(7),
    majorStopOffset(75),
    majorStopLength(10),
    tickLabelOffset(70),
    needleLength(70),
    setpointOffset(80),
    setpointLength(20),
    needleRenderer(this),
    needleCenterX(0),
    redraw(false),
    setpointRenderer(this),
    dragging(false)
{
    refreshPixmap();

    setMinimumSize(100, 100);

    connect(getTimer(), SIGNAL(timeout()), this, SLOT(redrawEvent()));
}

/****************************************************************************/

void Dial::setTitle(const QString &t)
{
    if (t == title) {
        return;
    }

    title = t;
    refreshPixmap();
}

/****************************************************************************/

void Dial::resetTitle()
{
    setTitle(DEFAULT_TITLE);
}

/****************************************************************************/

void Dial::setUnit(const QString &u)
{
    if (u == unit) {
        return;
    }

    unit = u;
    refreshPixmap();
}

/****************************************************************************/

void Dial::resetUnit()
{
    setUnit(DEFAULT_UNIT);
}

/****************************************************************************/

void Dial::setSpan(double s)
{
    if (s == span or s <= 0.0 or s > 360.0) {
        return;
    }

    span = s;
    refreshPixmap();
}

/****************************************************************************/

void Dial::resetSpan()
{
    setSpan(DEFAULT_SPAN);
}

/****************************************************************************/

void Dial::setScaleMin(double s)
{
    if (s == scaleMin) {
        return;
    }

    scaleMin = s;
    refreshPixmap();
}

/****************************************************************************/

void Dial::resetScaleMin()
{
    setScaleMin(DEFAULT_SCALEMIN);
}

/****************************************************************************/

void Dial::setMajorStep(double s)
{
    if (s == majorStep or s <= 0.0) {
        return;
    }

    majorStep = s;
    refreshPixmap();
}

/****************************************************************************/

void Dial::resetMajorStep()
{
    setMajorStep(DEFAULT_MAJORSTEP);
}

/****************************************************************************/

void Dial::setMajorStops(unsigned int s)
{
    if (s == majorStops || s < 1) {
        return;
    }

    majorStops = s;
    refreshPixmap();
}

/****************************************************************************/

void Dial::resetMajorStops()
{
    setMajorStops(DEFAULT_MAJORSTOPS);
}

/****************************************************************************/

void Dial::setMinorStops(unsigned int s)
{
    if (s == minorStops) {
        return;
    }

    minorStops = s;
    refreshPixmap();
}

/****************************************************************************/

void Dial::resetMinorStops()
{
    setMinorStops(DEFAULT_MINORSTOPS);
}

/****************************************************************************/

void Dial::setPieColor(const QColor &c)
{
    if (c == pieColor) {
        return;
    }

    pieColor = c;
    update();
}

/****************************************************************************/

void Dial::resetPieColor()
{
    setPieColor(DEFAULT_PIECOLOR);
}

/****************************************************************************/

void Dial::setNeedle(const QString &path)
{
    if (needlePath == path) {
        return;
    }

    needlePath = path;

    if (path.isEmpty()) {
        needleRenderer.load(QByteArray());
    }
    else {
        needleRenderer.load(path);
    }

    updateNeedleRect();
}

/****************************************************************************/

void Dial::setNeedleCenterX(int x)
{
    if (needleCenterX != x) {
        needleCenterX = x;
        updateNeedleRect();
    }
}

/****************************************************************************/

void Dial::setSetpoint(const QString &path)
{
    if (setpointPath == path) {
        return;
    }

    setpointPath = path;

    if (path.isEmpty()) {
        setpointRenderer.load(QByteArray());
    }
    else {
        setpointRenderer.load(path);
    }

    updateSetpointRect();
}

/****************************************************************************/

/** Gives a hint aboute the optimal size.
 */
QSize Dial::sizeHint() const
{
    return QSize(200, 200);
}

/****************************************************************************/

bool Dial::event(QEvent *event)
{
    switch (event->type()) {
        case QEvent::Resize:
            refreshPixmap();
            break;

        case QEvent::MouseButtonPress:
            mouseButtonPressEvent(static_cast<QMouseEvent*>(event));
            return true;

        case QEvent::MouseButtonRelease:
            mouseButtonReleaseEvent(static_cast<QMouseEvent*>(event));
            return true;

        case QEvent::MouseMove:
            mouseMoveEvent(static_cast<QMouseEvent*>(event));
            return true;

        case QEvent::MouseButtonDblClick:
            mouseButtonPressEvent(static_cast<QMouseEvent*>(event));
            return true;

        default:
            break;
    }

    return QFrame::event(event);
}

/****************************************************************************/

void Dial::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);

    QPainter painter(this);

    int side = qMin(contentsRect().width(), contentsRect().height());

    if (pieColor.isValid())
    {
        QPainterSave state(painter);
        painter.setRenderHint(QPainter::Antialiasing, true);

        painter.setViewport(
                (contentsRect().width() - side) / 2,
                (contentsRect().height() - side) / 2,
                side, side);
        painter.setWindow(-100, -100, 200, 200);

        painter.setPen(Qt::NoPen);
        painter.setBrush(pieColor);

        QRectF r(-80, -80, 160, 160);
        painter.drawPie(r,
                (90.0 + span / 2.0) * 16,
                -span * (currentValue.value - scaleMin) /
                majorStep / majorStops * 16);
    }

    painter.drawPixmap(0, 0, pixmap);

    {
        QPainterSave state(painter);
        painter.setRenderHint(QPainter::Antialiasing, true);

        painter.setViewport(
                (contentsRect().width() - side) / 2,
                (contentsRect().height() - side) / 2,
                side, side);
        painter.setWindow(-100, -100, 200, 200);

        painter.rotate(270.0 - 0.5 * span +
                span * (currentValue.value - scaleMin) /
                majorStep / majorStops);

        if (needleRect.isValid()) {
            needleRenderer.render(&painter, needleRect);
        }
        else {
            static const int pointer[][2] = {
                {needleLength, 0},
                {-16,   4}, {-25,  15},
                {-25, -15}, {-16, -4},
                {needleLength, 0}
            };

            QPen pen(palette().foreground(), 2.5,
                    Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
            painter.setPen(pen);
            painter.setBrush(palette().foreground());
            painter.drawPolygon(QPolygon(6, &pointer[0][0]));

            painter.setBrush(QBrush(Qt::white));
            painter.drawEllipse(-8, -8, 16, 16);
        }
    }
}

/****************************************************************************/

void Dial::refreshPixmap()
{
    pixmap = QPixmap(contentsRect().size());

    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.initFrom(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QFont font(painter.font());

    int side = qMin(contentsRect().width(), contentsRect().height());

    painter.setViewport(
            (contentsRect().width() - side) / 2,
            (contentsRect().height() - side) / 2,
            side, side);

    // Full width = 200, from center to side is 100
    painter.setWindow(-100, -100, 200, 200);

    // Draw perimeter
#if 0
    {
        QPainterSave save(painter);

        QRadialGradient radialGradient( 0, 0, 96, 0, 0);
        radialGradient.setColorAt(0.00, Qt::white);
        radialGradient.setColorAt(0.90, Qt::white);
        radialGradient.setColorAt(0.97, Qt::gray);
        radialGradient.setColorAt(1.00, Qt::white);

        painter.setPen(Qt::NoPen);
        painter.setBrush(radialGradient);
        painter.drawEllipse(-96, -96, 192, 192);
    }
#endif

    QPen pen(painter.pen());
    pen.setCapStyle(Qt::RoundCap);

    int count = majorStops * minorStops;
    double startAngle = 270.0 - span / 2.0;

    {   // Draw minor stops
        QPainterSave save(painter);
        double rotation = span / count;

        painter.setPen(pen);
        painter.rotate(startAngle);
        for (int i = 0; i <= count; ++i) {
            painter.drawLine(minorStopOffset, 0,
                    minorStopOffset + minorStopLength, 0);
            painter.rotate(rotation);
        }
    }

    {   // Draw major stops
        QPainterSave save(painter);
        double rotation = span / majorStops;

        pen.setWidth(3);
        painter.setPen(pen);
        painter.rotate(startAngle);
        for (unsigned int i = 0; i <= majorStops; ++i) {
            painter.drawLine(majorStopOffset, 0,
                    majorStopOffset + majorStopLength, 0);
            painter.rotate(rotation);
        }
    }

#if 0
    {   // Draw needle endstop // hard to position if using custom needle
        QPainterSave save(painter);

        pen.setWidth(0);
        painter.setPen(pen);
        painter.setBrush(Qt::SolidPattern);
        painter.rotate(startAngle);
        painter.drawEllipse(55, -5, 3, 3);
    }
#endif

    {   // Label major ticks
        double rotation = span / majorStops;

        QTransform transform;
        transform.rotate(startAngle);

        font.setPointSize(10);
        painter.setFont(font);

        for (unsigned int i = 0; i <= majorStops; ++i) {
            QLocale locale;
            QString text(locale.toString(i * majorStep + scaleMin));

            double dx = 0.5 * painter.fontMetrics().width(text);
            double dy = 0.4 * painter.fontMetrics().ascent();

            QPointF edge = transform.map(QPoint(tickLabelOffset, 0));
            QPointF shift(transform.m11() * dx, transform.m12() * dy);
            QPointF center = edge - shift;
            QPointF bottomLeft = center + QPointF(-dx, dy);

            painter.drawText(bottomLeft, text);
            transform.rotate(rotation);
        }
    }

    font.setPointSize(8);
    painter.setFont(font);
    painter.drawText(QRectF(-100, 35, 200, 16),
            Qt::AlignCenter, unit);

    font.setPointSize(11);
    font.setBold(true);
    painter.setFont(font);
    painter.drawText(QRectF(-100, 55, 200, 30),
            Qt::AlignCenter, title);

    if (setpointValue.dataPresent) {

        // Draw setpoint

        double value = dragging ? setpoint : setpointValue.value;
        painter.rotate(270.0 - span / 2.0 +
                span * (value - scaleMin) / majorStep / majorStops);

        if (setpointRect.isValid()) {
            setpointRenderer.render(&painter, setpointRect);
        }
        else {
            QPainterSave state(painter);
            static const int pointer[][2] = {
                {setpointOffset, 0},
                {setpointOffset + 10, 5}, {setpointOffset + 20, 5},
                {setpointOffset + 20, -5}, {setpointOffset + 10, -5},
                {setpointOffset, 0},
            };

            QPen pen(Qt::green, 2.5,
                    Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

            painter.setPen(pen);
            painter.setBrush(Qt::green);
            painter.drawPolygon(QPolygon(6, &pointer[0][0]));
        }

        setpointPos = painter.combinedTransform().map(QPoint(90,0));
    }

    update();
}

/****************************************************************************/

void Dial::mouseButtonPressEvent(QMouseEvent *event)
{
    dragging = (event->pos() - setpointPos).manhattanLength() < 30;
    setpoint = setpointValue.value;
}

/****************************************************************************/

void Dial::mouseButtonReleaseEvent(QMouseEvent *)
{
    dragging = false;
    setpointValue.writeValue(setpoint);
}

/****************************************************************************/

void Dial::mouseMoveEvent(QMouseEvent *event)
{
    if (!dragging) {
        return;
    }

    QPoint ray =
        event->pos() - QPoint(contentsRect().width() / 2,
                contentsRect().height() / 2);
    if (ray.manhattanLength() < 5) {
        return;
    }

    if (!QRect(0, 0, contentsRect().width(), contentsRect().height())
            .contains(event->pos())) {
        if (setpoint != setpointValue.value) {
            setpoint = setpointValue.value;
            refreshPixmap();
        }
        return;
    }

    double angle = 180.0 / M_PI * qAtan2(ray.y(), ray.x()) + 180.0;
    double minAngle = 450.0 - span / 2.0;

    angle += (angle >= minAngle ? 0 : 360.0) - minAngle;

    // Make the setpoint snap over the deadband
    if (angle > span) {
        angle = angle > 0.5 * (span + 360.0) ? 0 : span;
    }

    setpoint = angle / span * majorStep * majorStops + scaleMin;
    refreshPixmap();
}

/****************************************************************************/

void Dial::updateNeedleRect()
{
    QSize size(needleRenderer.defaultSize());
    int right = size.width() - needleCenterX;

    if (size.width() > 0 && right != 0) {
        float width = (float) needleLength * size.width() / right;
        float height = width * size.height() / size.width();
        needleRect =
            QRectF(needleLength - width, -height / 2.0, width, height);
    }
    else {
        needleRect = QRectF();
    }
}

/****************************************************************************/

void Dial::updateSetpointRect()
{
    QSize size(setpointRenderer.defaultSize());

    if (size.width() > 0) {
        float height = setpointLength * size.height() / size.width();
        setpointRect =
            QRectF(setpointOffset, -height / 2.0, setpointLength, height);
    }
    else {
        setpointRect = QRectF();
    }
}

/****************************************************************************/

/** Redraw event slot, that is called by the redraw timer.
 */
void Dial::redrawEvent()
{
    if (redraw) {
        redraw = false;
        update();
    }
}

/****************************************************************************/

void Dial::CurrentValue::notify(PdCom::Variable *pv)
{
    double newValue;

    pv->getValue(&newValue, 1, &scale);
    dataPresent = true;

    if (getFilterConstant() > 0.0) {
        value = getFilterConstant() * (newValue - value) + value;
        dial->redraw = true;
    } else {
        if (value != newValue) {
            value = newValue;
            dial->redraw = true;
        }
    }
}

/****************************************************************************/

void Dial::CurrentValue::variableEvent()
{
    if (!hasVariable()) {
        dataPresent = false;
        value = 0.0;
    }
    dial->redraw = true;
}

/****************************************************************************/

void Dial::SetpointValue::notify(PdCom::Variable *pv)
{
    pv->getValue(&value, 1, &scale);
    dataPresent = true;
    dial->refreshPixmap();
}

/****************************************************************************/

void Dial::SetpointValue::variableEvent()
{
    if (!hasVariable()) {
        dataPresent = false;
        value = 0.0;
    }
    dial->refreshPixmap();
}

/****************************************************************************/
