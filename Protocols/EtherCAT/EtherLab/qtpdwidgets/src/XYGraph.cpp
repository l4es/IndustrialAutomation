/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2009 - 2012  Florian Pose <fp@igh-essen.com>
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

#include "QtPdWidgets/XYGraph.h"

using Pd::XYGraph;

/****************************************************************************/

#define DEFAULT_TIMERANGE 1.0
#define DEFAULT_SCALEXMIN -10.0
#define DEFAULT_SCALEXMAX 10.0
#define DEFAULT_SCALEYMIN -10.0
#define DEFAULT_SCALEYMAX 10.0

/****************************************************************************/

/** Constructor.
 */
XYGraph::XYGraph(
        QWidget *parent /**< parent widget */
        ): QFrame(parent),
    axes(NumAxes, Axis(this)),
    xScale(this, Scale::Horizontal),
    yScale(this, Scale::Vertical)
{
    timeRange = DEFAULT_TIMERANGE;
    xScale.setMin(DEFAULT_SCALEXMIN);
    xScale.setMax(DEFAULT_SCALEXMAX);
    yScale.setMin(DEFAULT_SCALEYMIN);
    yScale.setMax(DEFAULT_SCALEYMAX);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(50, 50);

    retranslate();
}

/****************************************************************************/

/** Destructor.
 */
XYGraph::~XYGraph()
{
    clearVariables();
}

/****************************************************************************/

/** Subscribes the horizontal/vertical channel to a ProcessVariable.
 *
 * The first call connects the horizontal channel to the given
 * ProcessVariable. The second call will connect the vertical channel to the
 * given ProcessVariable. Any forther call will be ignored.
 *
 * \todo Add channel paremeter.
 */
void XYGraph::addVariable(
        PdCom::Variable *pv,
        double sampleTime,
        double gain,
        double offset
        )
{
    if (pv) {
        unsigned int index;
        Axis *axis;

        for (index = X; index < NumAxes; index++) {
            axis = &axes[index];
            if (!axis->variable) {
                axis->setVariable(pv, sampleTime, gain, offset);
                return;
            }
        }
    }
}

/****************************************************************************/

/** Unsubscribes from any subscribed ProcessVariables.
 */
void XYGraph::clearVariables()
{
    int index;

    for (index = X; index < NumAxes; index++) {
        axes[index].clearVariable();
    }
}

/****************************************************************************/

/** Clears any displayed data.
 */
void XYGraph::clearData()
{
    if (points.count()) {
        points.clear();
        update();
    }
}

/****************************************************************************/

/** Sets the #timeRange.
 */
void XYGraph::setTimeRange(double range)
{
    if (range != timeRange) {
        timeRange = range;
        removeDeprecated();
        update();
    }
}

/****************************************************************************/

/** Resets the #timeRange.
 */
void XYGraph::resetTimeRange()
{
    setTimeRange(DEFAULT_TIMERANGE);
}

/****************************************************************************/

/** Sets the minimum value of the horizontal scale.
 */
void XYGraph::setScaleXMin(double scale)
{
    if (scale != xScale.getMin()) {
        xScale.setMin(scale);
        updateRects();
    }
}

/****************************************************************************/

/** Resets the minimum value of the horizontal scale.
 */
void XYGraph::resetScaleXMin()
{
    setScaleXMin(DEFAULT_SCALEXMIN);
}

/****************************************************************************/

/** Sets the maximum value of the horizontal scale.
 */
void XYGraph::setScaleXMax(double scale)
{
    if (scale != xScale.getMax()) {
        xScale.setMax(scale);
        updateRects();
    }
}

/****************************************************************************/

/** Resets the maximum value of the horizontal scale.
 */
void XYGraph::resetScaleXMax()
{
    setScaleXMax(DEFAULT_SCALEXMAX);
}

/****************************************************************************/

/** Sets the minimum value of the vertical scale.
 */
void XYGraph::setScaleYMin(double scale)
{
    if (scale != yScale.getMin()) {
        yScale.setMin(scale);
        updateRects();
    }
}

/****************************************************************************/

/** Resets the minimum value of the vertical scale.
 */
void XYGraph::resetScaleYMin()
{
    setScaleYMin(DEFAULT_SCALEYMIN);
}

/****************************************************************************/

/** Sets the maximum value of the vertical scale.
 */
void XYGraph::setScaleYMax(double scale)
{
    if (scale != yScale.getMax()) {
        yScale.setMax(scale);
        updateRects();
    }
}

/****************************************************************************/

/** Resets the maximum value of the vertical scale.
 */
void XYGraph::resetScaleYMax()
{
    setScaleYMax(DEFAULT_SCALEYMAX);
}

/****************************************************************************/

/** Gives a hint aboute the optimal size.
 */
QSize XYGraph::sizeHint() const
{
    return QSize(150, 150);
}

/****************************************************************************/

/** Event handler.
 */
bool XYGraph::event(
        QEvent *event /**< Paint event flags. */
        )
{
    if (event->type() == QEvent::LanguageChange) {
        retranslate();
    }

    return QFrame::event(event);
}

/****************************************************************************/

/** Handles the widget's resize event.
 */
void XYGraph::resizeEvent(
        QResizeEvent *event /**< Resize event. */
        )
{
    contRect = contentsRect();
    updateRects();
    QFrame::resizeEvent(event);
}

/****************************************************************************/

/** Paint function.
 */
void XYGraph::paintEvent(
        QPaintEvent *event /**< paint event flags */
        )
{
    QPainter painter;
    QPen pen;
    QListIterator<Point> pointIter(points);
    const Point *point;
    int curX, curY, lastX, lastY;
    double xScaleFactor, yScaleFactor;

    QFrame::paintEvent(event); // draw frame
    painter.begin(this);
    pen = painter.pen();

    QColor gridColor = palette().window().color().dark(110);
    xScale.draw(painter, rect[X], gridColor);
    yScale.draw(painter, rect[Y], gridColor);

    if (points.count() <= 1 || !xScale.range() || !yScale.range())
        return;

    xScaleFactor = graphRect.width() / xScale.range();
    yScaleFactor = graphRect.height() / yScale.range();

    painter.setPen(Qt::blue);
    point = &pointIter.next();
    lastX = (int) ((point->value[X] - xScale.getMin()) * xScaleFactor);
    lastY = (int) ((point->value[Y] - yScale.getMin()) * yScaleFactor);

    while (pointIter.hasNext()) {
        point = &pointIter.next();
        curX = (int) ((point->value[X] - xScale.getMin()) * xScaleFactor);
        curY = (int) ((point->value[Y] - yScale.getMin()) * yScaleFactor);

        painter.drawLine(graphRect.left() + lastX,
                graphRect.bottom() - lastY,
                graphRect.left() + curX,
                graphRect.bottom() - curY);
        lastX = curX;
        lastY = curY;
    }
}

/****************************************************************************/

/** Calculates the widget layout.
 */
void XYGraph::updateRects()
{
    rect[Y] = contRect;
    rect[Y].setTop(contRect.top() + xScale.getOuterLength() + 1);

    if (rect[Y].height() != yScale.getLength()) {
        yScale.setLength(rect[Y].height());
    }

    rect[X] = contRect;
    rect[X].setLeft(contRect.left() + yScale.getOuterLength() + 1);

    if (rect[X].width() != xScale.getLength()) {
        xScale.setLength(rect[X].width());
    }

    graphRect = contRect;
    graphRect.setLeft(rect[X].left());
    graphRect.setTop(rect[Y].top());

    update();
}

/****************************************************************************/

/** Extracts graph points from the horizontal and vertical value lists.
 */
void XYGraph::extractPoints()
{
    const TimeValuePair *x, *y;
    Point p;
    int oldPointCount;

    if (axes[X].values.count() && axes[Y].values.count()) {
        oldPointCount = points.count();
        x = &axes[X].values.first();
        y = &axes[Y].values.first();

        while (1) {
            if (x->time == y->time) {
                p.value[X] = x->value;
                p.value[Y] = y->value;
                p.time = x->time;
                points.append(p);
                axes[X].values.removeFirst();
                axes[Y].values.removeFirst();
                if (!axes[X].values.count() || !axes[Y].values.count())
                    break;
                x = &axes[X].values.first();
                y = &axes[Y].values.first();
            }
            else if (x->time < y->time) {
                if (axes[X].values.count() == 1) {
                    // only one x value before y
                    break;
                }

                if (axes[X].values.first().time > y->time) {
                    // x is before y, next x ist after y
                    p.value[X] = x->value;
                    p.value[Y] = y->value;
                    p.time = y->time;
                    points.append(p);
                }

                axes[X].values.removeFirst();
                x = &axes[X].values.first();
            }
            else { // y->time < x->time
                if (axes[Y].values.count() == 1) {
                    // only one y value before x
                    break;
                }

                if (axes[Y].values.first().time > x->time) {
                    // y is before x, next y ist after x
                    p.value[X] = x->value;
                    p.value[Y] = y->value;
                    p.time = x->time;
                    points.append(p);
                }

                axes[Y].values.removeFirst();
                y = &axes[Y].values.first();
            }
        }

        if (points.count() != oldPointCount)
            update();
    }

    removeDeprecated();
}

/****************************************************************************/

/** Removed values and points, that are older than specified in #timeRange.
 */
void XYGraph::removeDeprecated()
{
    PdCom::Time depTime;
    unsigned int index;
    int oldPointCount;

    for (index = X; index < NumAxes; index++) {
        axes[index].removeDeprecated();
    }

    if (points.count()) {
        oldPointCount = points.count();
        depTime = points.last().time - PdCom::Time(timeRange);
        while (points.count() && points.first().time < depTime)
            points.removeFirst();
        if (points.count() != oldPointCount)
            update();
    }
}

/****************************************************************************/

/** Axis constructor.
 */
XYGraph::Axis::Axis(XYGraph *graph):
    graph(graph),
    variable(NULL)
{
}

/****************************************************************************/

/** Attach variable to axis.
 */
void XYGraph::Axis::setVariable(
        PdCom::Variable *pv,
        double sampleTime,
        double gain,
        double offset
        )
{
    clearVariable();

    scale.gain = gain;
    scale.offset = offset;

    pv->subscribe(this, sampleTime);
    variable = pv;
}

/****************************************************************************/

/** Detach variable from axis.
 */
void XYGraph::Axis::clearVariable()
{
    if (variable) {
        variable->unsubscribe(this);
        variable = NULL;
    }

    values.clear();
}

/****************************************************************************/

/** This virtual method is called by the ProcessVariable, if its value
 * changes.
 */
void XYGraph::Axis::notify(PdCom::Variable *pv)
{
    TimeValuePair timeVal;

    timeVal.time = pv->getMTime();
    pv->getValue(&timeVal.value, 1, &scale);

    values.append(timeVal);
    graph->extractPoints();
}

/****************************************************************************/

/** Notification for subscription cancellation forced by the ProcessVariable.
 *
 * This virtual function is called by the ProcessVariable, for any
 * subscription that is still active, when the ProcessVariable is about to be
 * destroyed.
 */
void XYGraph::Axis::notifyDelete(PdCom::Variable *)
{
    variable = NULL;
    values.clear();
}

/****************************************************************************/

/** Removed values, that are older than specified in #timeRange.
 */
void XYGraph::Axis::removeDeprecated()
{
    PdCom::Time depTime;

    if (values.count()) {
        depTime = values.last().time - PdCom::Time(graph->timeRange);
        while (values.count() && values.first().time < depTime)
            values.removeFirst();
    }
}

/****************************************************************************/

/** Retranslate the widget.
 */
void XYGraph::retranslate()
{
    setWindowTitle(Pd::XYGraph::tr("XY Graph"));
}

/****************************************************************************/
