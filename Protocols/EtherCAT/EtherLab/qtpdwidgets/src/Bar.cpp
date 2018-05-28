/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2009  Florian Pose <fp@igh-essen.com>
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

#include <QtGui>

#include "QtPdWidgets/Bar.h"
using Pd::Bar;

#include "BarStack.h"

/****************************************************************************/

#define DEFAULT_ORIENTATION     Vertical
#define DEFAULT_STYLE           ColorBar
#define DEFAULT_SHOWSCALE       true
#define DEFAULT_SCALEMIN        0.0
#define DEFAULT_SCALEMAX        100.0
#define DEFAULT_ORIGIN          OriginZero
#define DEFAULT_BORDERWIDTH     2
#define DEFAULT_BACKGROUNDCOLOR palette().window().color().dark(110)
#define DEFAULT_AUTOBARWIDTH    false

/** Drag indicator arrow shape in #Vertical mode.
 */
QPolygon Bar::verticalDragIndicatorPolygon;

/** Drag indicator arrow shape in #Horizontal mode.
 */
QPolygon Bar::horizontalDragIndicatorPolygon;

/** Arrow shape in #Vertical mode.
 */
QPolygon Bar::verticalArrow;

/** Arrow shape in #Horizontal mode.
 */
QPolygon Bar::horizontalArrow;

/****************************************************************************/

/** Constructor.
 */
Bar::Bar(
        QWidget *parent /**< parent widget */
        ): QWidget(parent),
    orientation(DEFAULT_ORIENTATION),
    style(DEFAULT_STYLE),
    showScale(DEFAULT_SHOWSCALE),
    valueScale(this, Scale::Vertical),
    origin(DEFAULT_ORIGIN),
    borderWidth(DEFAULT_BORDERWIDTH),
    backgroundColor(DEFAULT_BACKGROUNDCOLOR),
    autoBarWidth(DEFAULT_AUTOBARWIDTH),
    minStop(0),
    maxStop(0),
    darkPolygon(6),
    brightPolygon(6)
{
    valueScale.setMin(DEFAULT_SCALEMIN);
    valueScale.setMax(DEFAULT_SCALEMAX);

    maxBarWidth = 0;

    initDragIndicatorPolygons();

    updateLayout();

    connect(getTimer(), SIGNAL(timeout()), this, SLOT(redrawEvent()));

    retranslate();

#if 0
    QGradientStops s;
    s.append(qMakePair(35.0, QColor(255, 0, 0, 100))); // red
    s.append(qMakePair(50.0, QColor(255, 255, 0, 100))); // yellow
    s.append(qMakePair(65.0, QColor(0, 255, 0, 100))); // green
    s.append(qMakePair(250.0, QColor(0, 255, 0, 100))); // green
    s.append(qMakePair(330.0, QColor(255, 255, 0, 100))); // yellow
    s.append(qMakePair(390.0, QColor(255, 0, 0, 100))); // red
    setGradientStops(s);
#endif

#if 0
    Stack *stack = new Stack(this);
    stacks.append(stack);
    stack = new Stack(this);
    stacks.append(stack);
#endif
}

/****************************************************************************/

/** Destructor.
 */
Bar::~Bar()
{
    clearVariables();
}

/****************************************************************************/

/** Sets the #orientation.
 */
void Bar::setOrientation(Orientation orientation)
{
    Scale::Orientation scaleOrientation = Scale::Vertical;

    if (orientation == Bar::orientation)
        return;

    Bar::orientation = orientation;
    switch (orientation) { // map Bar::Orientation to Scale::Orientation
        case Vertical: scaleOrientation = Scale::Vertical; break;
        case Horizontal: scaleOrientation = Scale::Horizontal; break;
    }
    valueScale.setOrientation(scaleOrientation);

    updateLayout();
}

/****************************************************************************/

/** Resets the #orientation.
 */
void Bar::resetOrientation()
{
    setOrientation(DEFAULT_ORIENTATION);
}

/****************************************************************************/

/** Sets the #style.
 */
void Bar::setStyle(Style s)
{
    if (s == style) {
        return;
    }

    style = s;
    updateBackground();
}

/****************************************************************************/

/** Resets the #style.
 */
void Bar::resetStyle()
{
    setStyle(DEFAULT_STYLE);
}

/****************************************************************************/

/** Sets, if the value scale shall be shown.
 */
void Bar::setShowScale(bool showScale)
{
    if (showScale != Bar::showScale) {
        Bar::showScale = showScale;
        updateLayout();
    }
}

/****************************************************************************/

/** Resets #showScale.
 */
void Bar::resetShowScale()
{
    setShowScale(DEFAULT_SHOWSCALE);
}

/****************************************************************************/

/** Sets the value scale minimum.
 */
void Bar::setScaleMin(double val)
{
    valueScale.setMin(val);
    updateLayout();
}

/****************************************************************************/

/** Resets the value scale minimum.
 */
void Bar::resetScaleMin()
{
    setScaleMin(DEFAULT_SCALEMIN);
}

/****************************************************************************/

/** Sets the value scale maximum.
 */
void Bar::setScaleMax(double val)
{
    valueScale.setMax(val);
    updateLayout();
}

/****************************************************************************/

/** Resets the value scale maximum.
 */
void Bar::resetScaleMax()
{
    setScaleMax(DEFAULT_SCALEMAX);
}

/****************************************************************************/

/** Sets the bar #origin.
 *
 * \see Origin.
 */
void Bar::setOrigin(Origin o)
{
    if (o != origin) {
        origin = o;
        update();
    }
}

/****************************************************************************/

/** Resets the bar origin mode.
 */
void Bar::resetOrigin()
{
    setOrigin(DEFAULT_ORIGIN);
}

/****************************************************************************/

/** Sets the width of the border around the bar rect.
 */
void Bar::setBorderWidth(int width)
{
    if (width < 0)
        width = 0;

    if (width != borderWidth) {
        borderWidth = width;
        updateLayout();
    }
}

/****************************************************************************/

/** Resets the #borderWidth.
 */
void Bar::resetBorderWidth()
{
    setBorderWidth(DEFAULT_BORDERWIDTH);
}

/****************************************************************************/

/** Sets the background color of the widget.
 */
void Bar::setBackgroundColor(QColor c)
{
    if (c != backgroundColor) {
        backgroundColor = c;
        updateBackground();
    }
}

/****************************************************************************/

/** Resets the #backgroundColor.
 */
void Bar::resetBackgroundColor()
{
    setBackgroundColor(DEFAULT_BACKGROUNDCOLOR);
}

/****************************************************************************/

/** Sets the autoBarWidth flag of the widget.
 */
void Bar::setAutoBarWidth(bool a)
{
    if (a != autoBarWidth) {
        autoBarWidth = a;
    }
}

/****************************************************************************/

/** Resets the #autoBarWidth.
 */
void Bar::resetAutoBarWidth()
{
    setAutoBarWidth(DEFAULT_AUTOBARWIDTH);
}

/****************************************************************************/

/** Gives a hint aboute the optimal size.
 */
QSize Bar::sizeHint() const
{
    if (orientation == Vertical) {
        return QSize(60, 120);
    } else {
        return QSize(120, 40);
    }
}

/****************************************************************************/

void Bar::addVariable(
        PdCom::Variable *pv,
        double sampleTime,
        double gain,
        double offset,
        double tau,
        QColor color
        )
{
    if (!pv) {
        return;
    }

    Stack *s = new Stack(this);
    stacks.append(s);
    addStackedVariable(pv, sampleTime, gain, offset, tau, color);
}

/****************************************************************************/

void Bar::addStackedVariable(
        PdCom::Variable *pv,
        double sampleTime,
        double gain,
        double offset,
        double tau,
        QColor color
        )
{
    if (!pv) {
        return;
    }

    if (stacks.empty()) {
        addVariable(pv, sampleTime, gain, offset, tau, color);
    } else {
        Stack *stack = stacks.back();
        stack->addSection(pv, sampleTime, gain, offset, tau, color);
    }

    updateLayout();
}

/****************************************************************************/

void Bar::clearVariables()
{
    int i;

    for (i = 0; i < stacks.count(); i++) {
        delete stacks[i];
    }

    stacks.clear();
    updateLayout();
}

/****************************************************************************/

void Bar::setVariable(
        PdCom::Variable *pv,
        double sampleTime,
        double gain,
        double offset,
        double tau,
        QColor color
        )
{
    clearVariables();
    addVariable(pv, sampleTime, gain, offset, tau, color);
}

/****************************************************************************/

void Bar::clearData()
{
    for (StackList::iterator stack = stacks.begin();
            stack != stacks.end(); stack++) {
        (*stack)->clearData();
    }
}

/****************************************************************************/

void Bar::setGradientStops(const QGradientStops &g)
{
    if (g == gradientStops) {
        return;
    }

    gradientStops = g;
    updateBackground();
}

/****************************************************************************/

const QPair<double, double> Bar::getGradientLimits() const
{
    return qMakePair(minStop, maxStop);
}

/****************************************************************************/

/** Event handler.
 */
bool Bar::event(
        QEvent *event /**< Paint event flags. */
        )
{
    switch (event->type()) {
        case QEvent::LanguageChange:
            retranslate();
            break;

        case QEvent::StyleChange:
            updateLayout();
            break;

        default:
            break;
    }

    return QWidget::event(event);
}

/****************************************************************************/

/** Processes a resize event.
 */
void Bar::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    updateLayout();
}

/****************************************************************************/

/** Paint function.
 */
void Bar::paintEvent(
        QPaintEvent *event /**< paint event flags */
        )
{
    QPainter painter(this);

    // draw background from pixmap
    painter.drawPixmap(
            event->rect().topLeft(), backgroundPixmap, event->rect());

    painter.save();

    for (StackList::const_iterator stack = stacks.begin();
            stack != stacks.end(); stack++) {
        if (event->rect().intersects((*stack)->getRect()))
            (*stack)->paint(painter);
    }

    painter.restore();

    // draw origin line
    painter.setPen(Qt::black);
    painter.drawLine(zeroLine);

#if DEBUG
    QFont font = painter.font();
    font.setPointSize(7);
    painter.setFont(font);
    painter.setPen(Qt::blue);
    painter.drawText(contentsRect(), 0, debugStr);
#endif
}

/****************************************************************************/

/** Drag indicator polygon initializer.
 */
void Bar::initDragIndicatorPolygons()
{
    if (verticalDragIndicatorPolygon.size()) {
        return;
    }

    verticalDragIndicatorPolygon = QPolygon(4);
    verticalDragIndicatorPolygon.setPoint(0, 0, 0);
    verticalDragIndicatorPolygon.setPoint(1, 6, -2);
    verticalDragIndicatorPolygon.setPoint(2, 5, 0);
    verticalDragIndicatorPolygon.setPoint(3, 6, 2);

    horizontalDragIndicatorPolygon = QPolygon(4);
    horizontalDragIndicatorPolygon.setPoint(0, 0, 0);
    horizontalDragIndicatorPolygon.setPoint(1, 2, 6);
    horizontalDragIndicatorPolygon.setPoint(2, 0, 5);
    horizontalDragIndicatorPolygon.setPoint(3, -2, 6);

    verticalArrow = QPolygon(7);
    verticalArrow.setPoint(0, 0, 0);
    verticalArrow.setPoint(1, 10, 10);
    verticalArrow.setPoint(2, 10,  5);
    verticalArrow.setPoint(3, 20, 5);
    verticalArrow.setPoint(4, 20, -5);
    verticalArrow.setPoint(5, 10, -5);
    verticalArrow.setPoint(6, 10, -10);

    horizontalArrow = QPolygon(7);
    horizontalArrow.setPoint(0, 0, 0);
    horizontalArrow.setPoint(1, -10, 10);
    horizontalArrow.setPoint(2, -5,  10);
    horizontalArrow.setPoint(3, -5, 20);
    horizontalArrow.setPoint(4, 5, 20);
    horizontalArrow.setPoint(5, 5, 10);
    horizontalArrow.setPoint(6, 10, 10);
}

/****************************************************************************/

/** Calculates the widget's layout and draws the background into a pixmap.
 */
void Bar::updateLayout()
{
    QRect rect;
    unsigned int zeroPosition;

    rect = contentsRect();
    borderRect = rect;
    scaleRect = rect;

    valueScale.update();

    if (orientation == Vertical) {
        int barWidth;

        scaleRect.setTop(rect.top() + borderWidth);
        scaleRect.setBottom(rect.bottom() - borderWidth);
        valueScale.setLength(scaleRect.height());

        if (showScale) {
            barWidth = rect.width() - valueScale.getOuterLength();
        } else {
            barWidth = rect.width();
        }

        if (maxBarWidth != barWidth) {
            maxBarWidth = barWidth;
            notifyMaxBarWidthChange();
        }

        if (autoBarWidth) { // fetch minimum bar width of all siblings
            QList<Bar *> siblings = findSiblings();
            while (siblings.count()) {
                Bar *b = siblings.takeFirst();
                if (b->maxBarWidth < barWidth)
                    barWidth = b->maxBarWidth;
            }
        }
        scaleRect.setRight(rect.left() + rect.width() - barWidth);
        scaleRect.setLeft(scaleRect.right() +
                1 - valueScale.getOuterLength());
        borderRect.setLeft(scaleRect.right() + 1);
    } else { // Horizontal
        scaleRect.setLeft(rect.left() + borderWidth);
        scaleRect.setRight(rect.right() - borderWidth);
        valueScale.setLength(scaleRect.width());
        if (showScale) {
            scaleRect.setBottom(rect.top() + valueScale.getOuterLength() + 1);
            borderRect.setTop(scaleRect.bottom() + 1);
        }
    }

    barRect.setLeft(borderRect.left() + borderWidth);
    barRect.setTop(borderRect.top() + borderWidth);
    barRect.setRight(borderRect.right() - borderWidth);
    barRect.setBottom(borderRect.bottom() - borderWidth);

    zeroPosition = calcPosition(0.0);

    if (orientation == Vertical) {
        zeroLine = QLine(barRect.left(),
                barRect.bottom() - zeroPosition + 1,
                barRect.right(),
                barRect.bottom() - zeroPosition + 1);
    } else { // Horizontal
        zeroLine = QLine(barRect.left() + zeroPosition, barRect.top(),
                barRect.left() + zeroPosition, barRect.bottom());
    }

    if (stacks.count()) {
        int l, stackLength;

        if (orientation == Vertical) {
            l = barRect.left();
            stackLength = barRect.width() / stacks.count();
        } else {
            l = barRect.top();
            stackLength = barRect.height() / stacks.count();
        }

        for (StackList::const_iterator stack = stacks.begin();
                stack != stacks.end(); stack++) {
            QRect &r = (*stack)->getRect();
            if (orientation == Vertical) {
                r.setLeft(l);
                r.setWidth(stackLength);
                r.setTop(barRect.top());
                r.setHeight(barRect.height());
            } else {
                r.setLeft(barRect.left());
                r.setWidth(barRect.width());
                r.setTop(l);
                r.setBottom(l + stackLength - 1);
            }
            l += stackLength;
        }
    }

    updateBackground();
}

/****************************************************************************/

/** Calculates the widget's layout and draws the background into a pixmap.
 */
void Bar::updateBackground()
{
    QPainter painter;

    darkPolygon.setPoint(0, borderRect.topLeft());
    darkPolygon.setPoint(1, borderRect.topRight() + QPoint(1, 0));
    darkPolygon.setPoint(2, borderRect.topRight()
            + QPoint(-borderWidth, borderWidth) + QPoint(1, 0));
    darkPolygon.setPoint(3, borderRect.topLeft()
            + QPoint(borderWidth, borderWidth));
    darkPolygon.setPoint(4, borderRect.bottomLeft()
            + QPoint(borderWidth, -borderWidth) + QPoint(0, 1));
    darkPolygon.setPoint(5, borderRect.bottomLeft() + QPoint(0, 1));

    brightPolygon.setPoint(0, borderRect.topRight() + QPoint(1, 0));
    brightPolygon.setPoint(1, borderRect.bottomRight() + QPoint(1, 1));
    brightPolygon.setPoint(2, borderRect.bottomLeft() + QPoint(0, 1));
    brightPolygon.setPoint(3, borderRect.bottomLeft()
            + QPoint(borderWidth, -borderWidth) + QPoint(0, 1));
    brightPolygon.setPoint(4, borderRect.bottomRight()
            + QPoint(-borderWidth, -borderWidth) + QPoint(1, 1));
    brightPolygon.setPoint(5, borderRect.topRight()
            + QPoint(-borderWidth, borderWidth) + QPoint(1, 0));

    // draw new background into pixmap

    backgroundPixmap = QPixmap(size());
    backgroundPixmap.fill(Qt::transparent);
    painter.begin(&backgroundPixmap);
    painter.initFrom(this);

    updateGradient();

    QBrush brush;
    switch (style) {
        case ColorBar:
        case MultiColorBar:
            brush = backgroundColor;
            break;
        case Arrow:
            if (gradientStops.empty()) {
                brush = backgroundColor;
            }
            else {
                brush = gradient;
            }
            break;
    }

    painter.fillRect(barRect, brush);

    // draw scale
    if (showScale) {
        valueScale.draw(painter, scaleRect,
                palette().window().color().dark(110));
    }

    // draw border
    painter.setPen(Qt::NoPen);
    painter.setBrush(palette().window().color().light());
    painter.drawPolygon(brightPolygon);
    painter.setBrush(palette().window().color().dark());
    painter.drawPolygon(darkPolygon);

    update();
}

/****************************************************************************/

void Bar::notifyMaxBarWidthChange()
{
    if (autoBarWidth) {
        QList<Bar *> siblings = findSiblings();
        while (siblings.count()) {
            siblings.takeFirst()->maxBarWidthChanged();
        }
    }
}

/****************************************************************************/

QList<Bar *> Bar::findSiblings()
{
    QObject *p = parent();
    QList<Bar *> siblings;

    if (p) {
        siblings = p->findChildren<Pd::Bar *>();
        siblings.removeAll(this);
    }

    return siblings;
}

/****************************************************************************/

void Bar::maxBarWidthChanged()
{
    if (autoBarWidth) {
        updateLayout();
    }
}

/****************************************************************************/

/** Calculates a 1-D position from a real-world value.
 */
unsigned int Bar::calcPosition(double value, bool limit) const
{
    unsigned int max;

    if (!valueScale.range()) {
        return 0;
    }

    if (orientation == Vertical) {
        max = barRect.height();
    } else {
        max = barRect.width();
    }

    if (limit) {
        if (value >= valueScale.getMax()) {
            return max;
        }

        if (value <= valueScale.getMin()) {
            return 0;
        }
    }

    return (unsigned int)
        (max / valueScale.range() * (value - valueScale.getMin()) + 0.5);
}

/****************************************************************************/

/** Retranslate the widget.
 */
void Bar::retranslate()
{
    setWindowTitle(Pd::Bar::tr("Bar Graph"));
}

/****************************************************************************/

/** Redraw event slot, that is called by the redraw timer.
 */
void Bar::redrawEvent()
{
    for (StackList::iterator stack = stacks.begin();
            stack != stacks.end(); stack++) {
        (*stack)->redrawEvent();
    }
}

/****************************************************************************/

void Bar::updateGradient()
{
    QPoint start, end;

#if DEBUG
    debugStr = "";
#endif

    if (gradientStops.empty()) {
        return;
    }

    QGradientStops::const_iterator it = gradientStops.begin();

    minStop = it->first;
    maxStop = it->first;
    it++;

    for (; it != gradientStops.end(); it++) {
        if (it->first < minStop) {
            minStop = it->first;
        }
        if (it->first > maxStop) {
            maxStop = it->first;
        }
    }

#if DEBUG
    debugStr += QString("min=%1\nmax=%2\n").arg(minStop).arg(maxStop);
#endif

    if (maxStop - minStop <= 0.0) {
        return;
    }

    int startPos = calcPosition(minStop, false);
    int endPos = calcPosition(maxStop, false);

#if DEBUG
    debugStr += QString("sP=%1\neP=%2\n").arg(startPos).arg(endPos);
#endif

    switch (orientation) { // map Bar::Orientation to Scale::Orientation
        case Vertical:
            start = QPoint(0, barRect.bottom() + 1 - startPos);
            end = QPoint(0, barRect.bottom() + 1 - endPos);
            break;
        case Horizontal:
            start = QPoint(barRect.left() + startPos, 0);
            end = QPoint(barRect.left() + endPos, 0);
            break;
    }

#if DEBUG
    debugStr += QString("w=%1\nh=%2\n")
        .arg(barRect.width()).arg(barRect.height());
    debugStr += QString("s=%1,%2\ne=%3,%4\n")
        .arg(start.x()).arg(start.y()).arg(end.x()).arg(end.y());
#endif

    double m, b;
    QGradientStops s;

    m = 1.0 / (maxStop - minStop);
    b = -(m * minStop);

#if DEBUG
    debugStr += QString("m=%1\nb=%2\n").arg(m).arg(b);
#endif

    for (QGradientStops::const_iterator it = gradientStops.begin();
            it != gradientStops.end(); it++) {
        double f = it->first * m + b;

        /* Limit gradient stops to 0..1. Numerical issues can cause the stop
         * to be slightly smaller than zeros, which leads to display problems
         * on windows. */
        if (f < 0.0) {
            f = 0.0;
        }
        if (f > 1.0) {
            f = 1.0;
        }
#if DEBUG
        debugStr += QString("s=%1\n").arg(it->first);
        debugStr += QString("f=%1\n").arg(f);
#endif
        s.append(qMakePair(f, it->second));
    }

    gradient.setStart(start);
    gradient.setFinalStop(end);
    gradient.setStops(s);
}

/****************************************************************************/
