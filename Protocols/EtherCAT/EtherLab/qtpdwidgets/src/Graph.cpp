/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2009 - 2013  Florian Pose <fp@igh-essen.com>
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
#include <QMenu>

#include "QtPdWidgets/Graph.h"

#define DEBUG 0

#if DEBUG
#include <QDebug>
#endif

using Pd::Graph;
using Pd::ValueRing;

/****************************************************************************/

#define DEFAULT_MODE                  Roll
#define DEFAULT_TIMERANGE             10.0
#define DEFAULT_SCALEMIN              0.0
#define DEFAULT_SCALEMAX              100.0
#define DEFAULT_TRIGGER_LEVEL_MODE    AutoLevel
#define DEFAULT_MANUAL_TRIGGER_LEVEL  0.0
#define DEFAULT_TRIGGER_POSITION      0.1
#define DEFAULT_TRIGGER_TIMEOUT       0.0
#define DEFAULT_GRID_COLOR            (QColor(160, 160, 160))
#define DEFAULT_AUTO_SCALE_WIDTH      (false)

/****************************************************************************/

/** Constructor.
 */
Graph::Graph(
        QWidget *parent /**< parent widget */
        ): QFrame(parent),
    mode(DEFAULT_MODE),
    effectiveMode(DEFAULT_MODE),
    timeRange(DEFAULT_TIMERANGE),
    timeScale(this),
    valueScale(this, Scale::Vertical),
    triggerLevelMode(DEFAULT_TRIGGER_LEVEL_MODE),
    manualTriggerLevel(DEFAULT_MANUAL_TRIGGER_LEVEL),
    triggerPosition(DEFAULT_TRIGGER_POSITION),
    triggerTimeout(DEFAULT_TRIGGER_TIMEOUT),
    gridColor(DEFAULT_GRID_COLOR),
    autoScaleWidth(DEFAULT_AUTO_SCALE_WIDTH),
    state(Run),
    stopPixmap(":/QtPdWidgets/images/media-playback-pause.png"),
    runAction(this),
    stopAction(this),
    scaleWidth(0),
    trigger(this),
    redraw(false)
{
#if DEBUG
    qDebug() << "Constructing" << this;
#endif
    valueScale.setMin(DEFAULT_SCALEMIN);
    valueScale.setMax(DEFAULT_SCALEMAX);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(60, 50);

    timeScale.setLength(1); // FIXME
    updateTimeScale();

    runAction.setIcon(
			QIcon(":/QtPdWidgets/images/media-playback-start.png"));
    stopAction.setIcon(
			QIcon(":/QtPdWidgets/images/media-playback-pause.png"));

    connect(getTimer(), SIGNAL(timeout()), this, SLOT(redrawEvent()));
    connect(&runAction, SIGNAL(triggered()), this, SLOT(run()));
    connect(&stopAction, SIGNAL(triggered()), this, SLOT(stop()));

    retranslate();
}

/****************************************************************************/

/** Destructor.
 */
Graph::~Graph()
{
#if DEBUG
    qDebug() << "Destructing..." << this;
#endif
    clearVariables();
    clearTriggerVariable();
#if DEBUG
    qDebug() << "Destructed" << this;
#endif
}

/****************************************************************************/

void Graph::setVariable(
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

void Graph::addVariable(
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

    Layer *layer = new Layer(this, color, state, timeRange);
    layer->setVariable(pv, sampleTime, gain, offset, tau);
    layer->resizeExtrema(timeScale.getLength());
    layers.append(layer);
}

/****************************************************************************/

void Graph::clearVariables()
{
    for (QList<Layer *>::iterator layer = layers.begin();
            layer != layers.end();
            layer++) {
        delete *layer;
    }

    layers.clear();
    update(graphRect);
}

/****************************************************************************/

void Graph::clearData()
{
    for (QList<Layer *>::iterator layer = layers.begin();
            layer != layers.end();
            layer++) {
        (*layer)->clearData();
    }

    update(graphRect);
}

/****************************************************************************/

void Graph::setTriggerVariable(
        PdCom::Variable *pv,
        double sampleTime,
        double gain,
        double offset,
        double tau
        )
{
    clearTriggerVariable();

    if (!pv) {
        return;
    }

    trigger.setVariable(pv, sampleTime, gain, offset, tau);
}

/****************************************************************************/

void Graph::clearTriggerVariable()
{
    trigger.clearVariable();
    trigger.reset();
}

/****************************************************************************/

/** Sets the widget's #mode.
 */
void Graph::setMode(Mode mode)
{
    if (mode == Graph::mode) {
        return;
    }

    Graph::mode = mode;
    updateTimeScale();
    clearData();
    trigger.reset();

    if (mode == Roll) {
        effectiveMode = mode;
    }
}

/****************************************************************************/

/** Resets the widget's #mode.
 */
void Graph::resetMode()
{
    setMode(DEFAULT_MODE);
}

/****************************************************************************/

/** Sets the time range.
 */
void Graph::setTimeRange(double range)
{
    if (range == timeRange) {
        return;
    }

    timeRange = range;
    trigger.setTimeRange(range);

    for (QList<Layer *>::iterator layer = layers.begin();
            layer != layers.end();
            layer++) {
        (*layer)->setTimeRange(range);
    }

    updateTimeScale();
}

/****************************************************************************/

/** Resets the time range.
 */
void Graph::resetTimeRange()
{
    setTimeRange(DEFAULT_TIMERANGE);
}

/****************************************************************************/

/** Sets the value scale's minimum value.
 */
void Graph::setScaleMin(double scale)
{
    if (scale != valueScale.getMin()) {
        valueScale.setMin(scale);
        updateBackground();
    }
}

/****************************************************************************/

/** Resets the value scale's minimum value.
 */
void Graph::resetScaleMin()
{
    setScaleMin(DEFAULT_SCALEMIN);
}

/****************************************************************************/

/** Sets the value scale's maximum value.
 */
void Graph::setScaleMax(double scale)
{
    if (scale != valueScale.getMax()) {
        valueScale.setMax(scale);
        updateBackground();
    }
}

/****************************************************************************/

/** Resets the value scale's maximum value.
 */
void Graph::resetScaleMax()
{
    setScaleMax(DEFAULT_SCALEMAX);
}

/****************************************************************************/

/** Sets the widget's #triggerLevelMode.
 */
void Graph::setTriggerLevelMode(TriggerLevelMode m)
{
    if (m == triggerLevelMode) {
        return;
    }

    triggerLevelMode = m;
    if (triggerLevelMode == ManualLevel) {
        trigger.setLevel(manualTriggerLevel);
    }
    trigger.reset();
}

/****************************************************************************/

/** Resets the widget's #triggerLevelMode.
 */
void Graph::resetTriggerLevelMode()
{
    setTriggerLevelMode(DEFAULT_TRIGGER_LEVEL_MODE);
}

/****************************************************************************/

/** Sets the #manualTriggerLevel.
 */
void Graph::setManualTriggerLevel(double l)
{
    if (l == manualTriggerLevel) {
        return;
    }

    manualTriggerLevel = l;
    if (triggerLevelMode == ManualLevel) {
        trigger.setLevel(manualTriggerLevel);
        trigger.reset();
    }
}

/****************************************************************************/

/** Resets the #manualTriggerLevel.
 */
void Graph::resetManualTriggerLevel()
{
    setManualTriggerLevel(DEFAULT_MANUAL_TRIGGER_LEVEL);
}

/****************************************************************************/

/** Sets the #triggerPosition.
 */
void Graph::setTriggerPosition(double triggerPosition)
{
    if (triggerPosition > 1.0) {
        triggerPosition = 1.0;
    }
    else if (triggerPosition < 0.0) {
        triggerPosition = 0.0;
    }

    if (triggerPosition == Graph::triggerPosition) {
        return;
    }

    Graph::triggerPosition = triggerPosition;
    updateTimeScale();
    clearData();
}

/****************************************************************************/

/** Resets the #triggerPosition.
 */
void Graph::resetTriggerPosition()
{
    setTriggerPosition(DEFAULT_TRIGGER_POSITION);
}

/****************************************************************************/

/** Sets the #triggerTimeout.
 */
void Graph::setTriggerTimeout(double triggerTimeout)
{
    if (triggerTimeout < 0.0) {
        triggerTimeout = 0.0;
    }

    if (triggerTimeout == Graph::triggerTimeout) {
        return;
    }

    Graph::triggerTimeout = triggerTimeout;
}

/****************************************************************************/

/** Resets the #triggerTimeout.
 */
void Graph::resetTriggerTimeout()
{
    setTriggerTimeout(DEFAULT_TRIGGER_TIMEOUT);
}

/****************************************************************************/

/** Sets the #suffix to display after the Y-axis values.
 */
void Graph::setSuffix(const QString &suffix)
{
    if (suffix != getSuffix()) {
        valueScale.setSuffix(suffix);
        updateBackground();
    }
}

/****************************************************************************/

/** Resets the #suffix to display after the Y-axis values.
 */
void Graph::resetSuffix()
{
    valueScale.resetSuffix();
    updateBackground();
}

/****************************************************************************/

/** Sets the #gridColor.
 */
void Graph::setGridColor(const QColor &col)
{
    if (gridColor == col) {
        return;
    }

    gridColor = col;
    updateBackground();
}

/****************************************************************************/

/** Resets the #gridColor.
 */
void Graph::resetGridColor()
{
    setGridColor(DEFAULT_GRID_COLOR);
}

/****************************************************************************/

/** Sets the #autoScaleWidth flag.
 */
void Graph::setAutoScaleWidth(bool a)
{
    if (autoScaleWidth == a) {
        return;
    }

    autoScaleWidth = a;
    updateBackground();
}

/****************************************************************************/

/** Resets the #gridColor.
 */
void Graph::resetAutoScaleWidth()
{
    setAutoScaleWidth(DEFAULT_AUTO_SCALE_WIDTH);
}

/****************************************************************************/

/** Gives a hint aboute the optimal size.
 */
QSize Graph::sizeHint() const
{
    return QSize(300, 100);
}

/****************************************************************************/

/** Marks the graph for redrawing.
 */
void Graph::setRedraw()
{
    redraw = true;
}

/****************************************************************************/

/** Set the graph state.
 */
void Graph::setState(State s)
{
    if (state == s) {
        return;
    }

    state = s;

#if DEBUG
    qDebug() << this << state;
#endif

    for (QList<Layer *>::iterator layer = layers.begin();
            layer != layers.end();
            layer++) {
        (*layer)->setState(state);
    }

    update();
}

/****************************************************************************/

/** Toggle the graph state.
 */
void Graph::toggleState()
{
    switch (state) {
        case Run:
            setState(Stop);
            break;

        default:
            setState(Run);
            break;
    }
}

/****************************************************************************/

/** Event handler.
 */
bool Graph::event(
        QEvent *event /**< Paint event flags. */
        )
{
    switch (event->type()) {
        case QEvent::MouseButtonDblClick:
            toggleState();
            return true;

        case QEvent::LanguageChange:
            retranslate();
            break;

        case QEvent::StyleChange:
            updateBackground();
            break;

        default:
            break;
    }

    return QFrame::event(event);
}

/****************************************************************************/

/** Handles the widget's resize event.
 */
void Graph::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    updateBackground();
}

/****************************************************************************/

/** Paint function.
 */
void Graph::paintEvent(
        QPaintEvent *event /**< paint event flags */
        )
{
    QFrame::paintEvent(event);

    QPainter painter(this);
    QPen pen = painter.pen();
    QFont font = painter.font();

    // update style
    if (pen.color() != foregroundColor ||
            font != foregroundFont) {
        foregroundColor = pen.color();
        foregroundFont = font;
        timeScale.update();
        valueScale.update();
        updateBackground();
    }

    // draw background from pixmap
    painter.drawPixmap(event->rect().topLeft(), backgroundPixmap,
            event->rect());

    // draw layers
    if (valueScale.range() && event->rect().intersects(graphRect)) {
        painter.setClipRect(graphRect);
        double scale = graphRect.height() / valueScale.range();

        for (QList<Layer *>::const_iterator layer = layers.begin();
                layer != layers.end();
                layer++) {
            (*layer)->paint(painter, scale, valueScale, graphRect);
        }
    }

    // draw pause icon
    if (state == Stop) {
        QRect iconRect;
        iconRect.setSize(stopPixmap.size());
        iconRect.moveTop(graphRect.top() + 5);
        iconRect.moveRight(graphRect.right() - 5);

        if (event->rect().intersects(iconRect)) {
            painter.drawPixmap(iconRect.topLeft(), stopPixmap);
        }
    }

#if DEBUG
    if (effectiveMode == Trigger) {
        QRect rect(contentsRect());
        QPen pen2;
        pen2.setColor(Qt::red);
        painter.setClipping(false);
        painter.setPen(pen2);
        painter.drawText(rect, Qt::AlignLeft | Qt::AlignTop, "T");
    }
#endif
}

/****************************************************************************/

/** Shows the context menu.
 */
void Graph::contextMenuEvent(QContextMenuEvent *event)
{
    runAction.setEnabled(state != Run);
    stopAction.setEnabled(state != Stop);

    QMenu menu(this);
    menu.addAction(&runAction);
    menu.addAction(&stopAction);
    menu.exec(event->globalPos());
}

/****************************************************************************/

/** Called by the trigger detector, if the trigger condition was detected.
 */
void Graph::triggerConditionDetected(PdCom::Time triggerTime)
{
    effectiveMode = Trigger;

    // calculate time to sample triggered values
    PdCom::Time timeToSample =
        triggerTime + PdCom::Time((1.0 - triggerPosition) * timeRange);

    for (QList<Layer *>::const_iterator layer = layers.begin();
            layer != layers.end();
            layer++) {
        (*layer)->prepareSample(timeToSample);
    }
}

/****************************************************************************/

/** Called by the trigger detector, if no trigger condition is detected for a
 * while.
 */
void Graph::triggerIdle()
{
    effectiveMode = Roll;
}

/****************************************************************************/

/** Called by the layers if the data were sampled.
 */
void Graph::notifySampled()
{
    bool allSampled = true;

    for (QList<Layer *>::const_iterator layer = layers.begin();
            layer != layers.end();
            layer++) {
        if (!(*layer)->hasSampled()){
            allSampled = false;
            break;
        }
    }

    if (allSampled) {
        redraw = true;
        trigger.reset();
    }
}

/****************************************************************************/

/** Calculated the widget's layout.
 */
void Graph::updateBackground()
{
    QRect rect, valueRect, timeRect;
    QPainter painter;

    rect = contentsRect();

#if DEBUG
    qDebug() << this << "updateBackground" << rect;
#endif

    valueRect = rect;
    valueRect.setTop(rect.top() + timeScale.getOuterLength() + 1);
    valueScale.setLength(valueRect.height());

    if (scaleWidth != valueScale.getOuterLength()) {
        scaleWidth = valueScale.getOuterLength();
        notifyScaleWidthChange();
    }

    int effScaleWidth = scaleWidth;
    if (autoScaleWidth) {
        QList<Graph *> siblings = findSiblings();
        while (siblings.count()) {
            Graph *g = siblings.takeFirst();
            if (g->scaleWidth > effScaleWidth) {
                effScaleWidth = g->scaleWidth;
            }
        }
    }

    timeRect = rect;
    timeRect.setLeft(rect.left() + effScaleWidth + 1);
    if (timeRect.width() != timeScale.getLength()) {
        int length = timeRect.width();
        if (length < 0) {
            length = 0;
        }
#if DEBUG
        qDebug() << this << "timescale length" << length;
#endif
        timeScale.setLength(length);

        for (QList<Layer *>::const_iterator layer = layers.begin();
                layer != layers.end();
                layer++) {
            (*layer)->resizeExtrema(length);
        }
    }

    graphRect = rect;
    graphRect.setLeft(timeRect.left());
    graphRect.setTop(valueRect.top());

    // draw new background into pixmap

    backgroundPixmap = QPixmap(size());
    backgroundPixmap.fill(Qt::transparent);

    painter.begin(&backgroundPixmap);
    painter.setPen(foregroundColor);
    painter.setFont(foregroundFont);
    valueScale.draw(painter, valueRect, gridColor, effScaleWidth);

    painter.setPen(foregroundColor);
    painter.setFont(foregroundFont);
    timeScale.draw(painter, timeRect, gridColor);

    update();
}

/****************************************************************************/

/** Calculates the range of the #timeScale.
 */
void Graph::updateTimeScale()
{
    if (mode == Trigger) {
        timeScale.setMin(-triggerPosition * timeRange);
        timeScale.setMax((1 - triggerPosition) * timeRange);
    }
    else {
        timeScale.setMin(-timeRange);
        timeScale.setMax(0.0);
    }

    updateBackground();
}

/****************************************************************************/

/** Retranslate the widget.
 */
void Graph::retranslate()
{
    setWindowTitle(Pd::Graph::tr("Graph"));
    runAction.setText(Pd::Graph::tr("Run"));
    stopAction.setText(Pd::Graph::tr("Stop"));
}

/****************************************************************************/

void Graph::notifyScaleWidthChange()
{
    if (!autoScaleWidth) {
        return;
    }

    QList<Graph *> siblings = findSiblings();
    while (siblings.count()) {
        Graph *g = siblings.takeFirst();
        if (g->autoScaleWidth) {
            g->updateBackground();
        }
    }
}

/****************************************************************************/

QList<Graph *> Graph::findSiblings() const
{
    QObject *p = parent();
    QList<Graph *> siblings;

    if (p) {
        siblings = p->findChildren<Pd::Graph *>();
        siblings.removeAll((Pd::Graph *) this);
    }

    return siblings;
}

/****************************************************************************/

/** Redraw event slot, that is called by the redraw timer.
 */
void Graph::redrawEvent()
{
    if (mode == Trigger && state == Run) {
        trigger.updateLevel();
    }

    if (redraw) {
        redraw = false;
        update(graphRect);
    }
}

/****************************************************************************/

/** Set the graph to Run state.
 */
void Graph::run()
{
    setState(Run);
}

/****************************************************************************/

/** Set the graph to Stop state.
 */
void Graph::stop()
{
    setState(Stop);
}

/****************************************************************************/

/** Constructor.
 */
Graph::Layer::Layer(
        Graph *parent,
        QColor c,
        State state,
        double timeRange
        ):
    graph(parent),
    color(c),
    state(state),
    value(0.0),
    dataPresent(false),
    offset(0),
    validExtrema(0U),
    lastAppendedValue(0.0)
{
    setTimeRange(timeRange);
}

/****************************************************************************/

void Graph::Layer::clearData()
{
    savedValues.clear();
    validExtrema = 0U;
    dataPresent = false;
}

/****************************************************************************/

/**
 * \return The values currently displayed.
 */
const ValueRing<double> &Graph::Layer::getValues() const
{
    if (graph->getMode() == Roll && state == Run) {
        return values;
    } else {
        return savedValues;
    }
}

/****************************************************************************/

void Graph::Layer::setState(State s)
{
    if (s == state) {
        return;
    }

    state = s;

    if (graph->getMode() == Roll) {
        if (state == Run) {
            fillExtrema();
        }
        else {
            savedValues = values; // copy values
        }
    }
}

/****************************************************************************/

void Graph::Layer::setTimeRange(double range)
{
#if DEBUG
    qDebug() << this << "time range" << range;
#endif
    values.setRange(range + 0.5); /* hold some more values than necessary for
                                     synchronisation reasons. */
    fillExtrema();
}

/****************************************************************************/

/** Paint the graph layer.
 */
void Graph::Layer::paint(
        QPainter &painter,
        double scaleFactor,
        const Scale &valueScale,
        const QRect &graphRect
        )
{
    int count = extrema.count();
    const Extrema *ext;
    int x, yMin, yMax;

    if (count <= 0) {
        return;
    }

    painter.setPen(color);

    for (x = count - validExtrema; x < count; x++) {
        ext = &extrema[(x + offset + 1) % count];

        if (ext->second < valueScale.getMin()
                || ext->first > valueScale.getMax()) {
            continue;
        }

        if (ext->first >= valueScale.getMin()) {
            yMin = (int) ((ext->first - valueScale.getMin()) * scaleFactor);
        } else {
            yMin = 0;
        }

        if (ext->second <= valueScale.getMax()) {
            yMax = (int) ((ext->second - valueScale.getMin()) * scaleFactor);
        } else {
            yMax = graphRect.height();
        }

        if (yMin < yMax) {
            painter.drawLine(graphRect.left() + x,
                    graphRect.bottom() - yMax,
                    graphRect.left() + x,
                    graphRect.bottom() - yMin);
        }
        else {
            /* Workaround for
             * https://bugreports.qt-project.org/browse/QTBUG-25153
             * which causes one-pixel lines not to be drawn.
             */
            painter.drawPoint(graphRect.left() + x,
                    graphRect.bottom() - yMin);
        }
    }
}

/****************************************************************************/

/** Resizes the #extrema ring buffer.
 */
void Graph::Layer::resizeExtrema(unsigned int length)
{
#if DEBUG
    qDebug() << this << "resize extrema" << length;
#endif
    extrema.resize(length);
    fillExtrema();
}

/****************************************************************************/

/** Copy current values into trigger buffer.
 */
void Graph::Layer::prepareSample(PdCom::Time t)
{
    timeToSample = t;
}

/****************************************************************************/

/** This virtual method is called by the ProcessVariable, if its value
 * changes.
 */
void Graph::Layer::notify(PdCom::Variable *pv)
{
    double newValue;
    PdCom::Time time;

    pv->getValue(&newValue, 1, &scale);
    time = pv->getMTime();

    if (dataPresent) {
        if (getFilterConstant() > 0.0) {
            value += getFilterConstant() * (newValue - value);
        } else {
            value = newValue;
        }
    } else {
        dataPresent = true;
        value = newValue;
    }

    values.append(time, value);

    if (graph->getEffectiveMode() == Roll && state == Run) {
        if (extrema.count() && !validExtrema) {
            extrema[offset].first = value;
            extrema[offset].second = value;
            offsetTime = time;
            validExtrema = 1U;
        }
        if (appendToExtrema(time, value)) {
            graph->setRedraw();
        }
    } else if (graph->getEffectiveMode() == Trigger &&
            (double) timeToSample != 0.0 && time >= timeToSample) {
        savedValues.copyUntil(values, timeToSample);
        timeToSample = 0.0; // mark as sampled
        fillExtrema();
        graph->notifySampled();
    }
}

/****************************************************************************/

/** Fills the #extrema with the values from the ring.
 */
void Graph::Layer::fillExtrema()
{
    unsigned int count = extrema.count();
    const ValueRing<double> *v;

    offset = 0;
    validExtrema = 0U;

    if (graph->getEffectiveMode() == Roll && graph->getState() == Run) {
        v = &values;
    } else {
        v = &savedValues;
    }

    if (!count || !v->getLength()) {
        return;
    }

    PdCom::Time dropTime = (double) (*v)[-1].first - graph->getTimeRange();

    unsigned int i = 0; // drop data before drop time
    while (i < v->getLength() && (*v)[i].first < dropTime) {
        i++;
    }

    if (i >= v->getLength()) {
        return;
    }

    offsetTime = (double) (*v)[i].first;
    extrema[0].first = (*v)[i].second;
    extrema[0].second = (*v)[i].second;
    validExtrema = 1U;

    for (; i < v->getLength(); i++) {
        appendToExtrema((*v)[i].first, (*v)[i].second);
    }
}

/****************************************************************************/

/** Appends a new data value to the #extrema ring buffer.
 *
 * \return \a true, if the widget has to be redrawn.
 */
bool Graph::Layer::appendToExtrema(
        const PdCom::Time &time,
        double value
        )
{
    unsigned int feed, count = extrema.count();
    bool redrawNeeded = false;

    if (count) {
        PdCom::Time dt = time - offsetTime;
        if ((double) dt >= 0.0 && (double) dt < graph->getTimeRange()) {
            // calculate feed and new offset time
            feed = (unsigned int)
                ((double) dt / graph->getTimeRange() * count);
            offsetTime += graph->getTimeRange() / count * feed;
        } else {
            if ((double) dt < 0.0) {
                qWarning() << "Invalid time step:"
                    << offsetTime.str().c_str() << "to" << time.str().c_str();
            }
            feed = count; // feed over all extrema
            offsetTime = time;
        }

        if (feed) {
            if (validExtrema + feed >= count) {
                validExtrema = count;
            } else {
                validExtrema += feed;
            }

            while (feed) {
                // feed ring buffer
                offset = (offset + 1) % count;
                extrema[offset].first = lastAppendedValue;
                extrema[offset].second = lastAppendedValue;
                feed--;
            }

            redrawNeeded = true;
        }

        if (value < extrema[offset].first) {
            extrema[offset].first = value;
            redrawNeeded = true;
        } else if (value > extrema[offset].second) {
            extrema[offset].second = value;
            redrawNeeded = true;
        }
    }

    lastAppendedValue = value;

    return redrawNeeded;
}

/****************************************************************************/

Graph::TriggerDetector::TriggerDetector(
        Graph *graph
        ):
    graph(graph),
    dataPresent(false),
    state(Armed),
    level(0.0)
{
}

/****************************************************************************/

void Graph::TriggerDetector::setTimeRange(double range)
{
    values.setRange(range);
}

/****************************************************************************/

/** Resets the trigger detection.
 */
void Graph::TriggerDetector::reset()
{
    state = Armed;
}

/****************************************************************************/

/** Sets the trigger level.
 */
void Graph::TriggerDetector::setLevel(double l)
{
    level = l;
}

/****************************************************************************/

/** Updates the automatic trigger level.
 */
void Graph::TriggerDetector::updateLevel()
{
    if (state != Armed || graph->getTriggerLevelMode() != AutoLevel) {
        return;
    }

    if (values.getLength() <= 0) {
        level = 0.0;
        return;
    }

    unsigned int i;
    double min, max, sum, value, mean;

    min = max = sum = values[0].second;

    for (i = 1; i < values.getLength(); i++) {
        value = values[i].second;
        if (value < min) {
            min = value;
        }
        if (value > max) {
            max = value;
        }
        sum += value;
    }

    mean = sum / values.getLength();

    if (max - mean > mean - min) {
        level = (mean + max) / 2.0;
    }
    else {
        level = (min + mean) / 2.0;
    }
}

/****************************************************************************/

void Graph::TriggerDetector::notify(PdCom::Variable *pv)
{
    double newValue;
    PdCom::Time time = pv->getMTime();

    pv->getValue(&newValue, 1, &scale);

    values.append(time, newValue);

    if (values.getLength() >= 2 && graph->getMode() == Trigger &&
            graph->getState() == Run && state == Armed) {
        if (values[-2].second < level && values[-1].second >= level) {
            state = Fired;
            lastEvent = time;
            graph->triggerConditionDetected(lastEvent);
        }
        else if (graph->getTriggerTimeout() > 0.0) {
            PdCom::Time dt = time - lastEvent;
            if ((double) dt >= graph->getTriggerTimeout()) {
                graph->triggerIdle();
            }
        }
    }
}

/****************************************************************************/

void Graph::TriggerDetector::variableEvent()
{
    if (!hasVariable()) {
        values.clear();
    }
}

/****************************************************************************/
