/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2012  Florian Pose <fp@igh-essen.com>
 *
 * This file is part of the DLS widget library.
 *
 * The DLS widget library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * The DLS widget library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the DLS widget library. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

#include <QtGui>
#include <QDomElement>

#include "DlsWidgets/Graph.h"
#include "DlsWidgets/Section.h"
#include "DlsWidgets/Layer.h"
#include "DlsWidgets/Model.h"
#include "Channel.h"

using DLS::Section;
using DLS::Layer;
using QtDls::Model;
using QtDls::Channel;

/****************************************************************************/

/* Default colors, taken from Ethan Schoonover's Solarized colorscheme
 * see http://ethanschoonover.com/solarized
 */
const QColor Section::colorList[] = {
    QColor( 38, 139, 210), // blue
    QColor(220,  50,  47), // red
    QColor(133, 153,   0), // green
    QColor(181, 137,   0), // yellow
    QColor(211,  54, 130), // magenta
    QColor(108, 113, 196), // violet (close to blue)
    QColor( 42, 161, 152), // cyan
    QColor(203,  75,  22), // orange (close to red)
};

#define NUM_COLORS (sizeof(colorList) / sizeof(QColor))

/****************************************************************************/

/** Constructor.
 */
Section::Section(
        Graph *graph
        ):
    graph(graph),
    scale(graph),
    autoScale(true),
    showScale(true),
    scaleMin(0.0),
    scaleMax(100.0),
    height(100),
    relativePrintHeight(-1.0),
    minimum(0.0),
    maximum(0.0),
    extremaValid(false),
    busy(false)
{
    updateLegend();
}

/****************************************************************************/

/** Copy constructor.
 */
Section::Section(
        const Section &o
        ):
    graph(o.graph),
    scale(o.graph),
    autoScale(o.autoScale),
    showScale(o.showScale),
    scaleMin(o.scaleMin),
    scaleMax(o.scaleMax),
    height(o.height),
    relativePrintHeight(o.relativePrintHeight),
    minimum(o.minimum),
    maximum(o.maximum),
    extremaValid(o.extremaValid)
{
    for (QList<Layer *>::const_iterator l = o.layers.begin();
            l != o.layers.end(); l++) {
        Layer *newLayer = new Layer(**l, this);
        layers.append(newLayer);
    }

    updateLegend();
    updateScale();

    graph->updateRange();
    graph->update();
}

/****************************************************************************/

/** Destructor.
 */
Section::~Section()
{
    clearLayers();
}

/****************************************************************************/

/** Assignment constructor.
 */
Section &Section::operator=(
        const Section &o
        )
{
    if (graph != o.graph) {
        throw Exception("Assigning section of a different graph!");
    }

    // graph is const
    autoScale = o.autoScale;
    showScale = o.showScale;
    scaleMin = o.scaleMin;
    scaleMax = o.scaleMax;
    height = o.height;
    relativePrintHeight = o.relativePrintHeight;
    minimum = o.minimum;
    maximum = o.maximum;
    extremaValid = o.extremaValid;

    clearLayers();

    for (QList<Layer *>::const_iterator l = o.layers.begin();
            l != o.layers.end(); l++) {
        Layer *newLayer = new Layer(**l, this);
        rwLockLayers.lockForWrite();
        layers.append(newLayer);
        rwLockLayers.unlock();
    }

    updateLegend();
    updateScale();

    graph->updateRange();
    graph->update();

    return *this;
}

/****************************************************************************/

void Section::load(const QDomElement &e, Model *model, const QDir &dir)
{
    QDomNodeList children = e.childNodes();

    for (int i = 0; i < children.size(); i++) {
        QDomNode node = children.item(i);
        if (!node.isElement()) {
            continue;
        }

        QDomElement child = node.toElement();

        if (child.tagName() == "AutoScale") {
            QString text = child.text();
            setAutoScale(text == "yes");
        }
        else if (child.tagName() == "ShowScale") {
            QString text = child.text();
            setShowScale(text == "yes");
        }
        else if (child.tagName() == "ScaleMinimum") {
            QString text = child.text();
            bool ok;
            double num = text.toDouble(&ok);
            if (!ok) {
                QString msg = tr("Invalid value in ScaleMinimum");
                throw Exception(msg);
            }
           setScaleMinimum(num);
        }
        else if (child.tagName() == "ScaleMaximum") {
            QString text = child.text();
            bool ok;
            double num = text.toDouble(&ok);
            if (!ok) {
                QString msg = tr("Invalid value in ScaleMaximum");
                throw Exception(msg);
            }
            setScaleMaximum(num);
        }
        else if (child.tagName() == "Height") {
            QString text = child.text();
            bool ok;
            unsigned int num = text.toInt(&ok);
            if (!ok) {
                QString msg = tr("Invalid value in Height");
                throw Exception(msg);
            }
            setHeight(num);
        }
        else if (child.tagName() == "RelativePrintHeight") {
            QString text = child.text();
            bool ok;
            double num = text.toDouble(&ok);
            if (!ok) {
                QString msg = tr("Invalid value in RelativePrintHeight");
                throw Exception(msg);
            }
            setRelativePrintHeight(num);
        }
        else if (child.tagName() == "Layers") {
            loadLayers(child, model, dir);
        }
    }

    updateLegend();
    updateScale();
}

/****************************************************************************/

/** Saves settings and layers to an XML element.
 */
void Section::save(QDomElement &e, QDomDocument &doc)
{
    QDomElement secElem = doc.createElement("Section");
    e.appendChild(secElem);

    QDomElement elem = doc.createElement("AutoScale");
    QDomText text = doc.createTextNode(autoScale ? "yes" : "no");
    elem.appendChild(text);
    secElem.appendChild(elem);

    elem = doc.createElement("ShowScale");
    text = doc.createTextNode(showScale ? "yes" : "no");
    elem.appendChild(text);
    secElem.appendChild(elem);

    elem = doc.createElement("ScaleMinimum");
    QString num;
    num.setNum(scaleMin);
    text = doc.createTextNode(num);
    elem.appendChild(text);
    secElem.appendChild(elem);

    elem = doc.createElement("ScaleMaximum");
    num.setNum(scaleMax);
    text = doc.createTextNode(num);
    elem.appendChild(text);
    secElem.appendChild(elem);

    elem = doc.createElement("Height");
    num.setNum(height);
    text = doc.createTextNode(num);
    elem.appendChild(text);
    secElem.appendChild(elem);

    elem = doc.createElement("RelativePrintHeight");
    num.setNum(relativePrintHeight);
    text = doc.createTextNode(num);
    elem.appendChild(text);
    secElem.appendChild(elem);

    QDomElement layersElem = doc.createElement("Layers");
    secElem.appendChild(layersElem);

    rwLockLayers.lockForRead();

    for (QList<Layer *>::const_iterator l = layers.begin();
            l != layers.end(); l++) {
        (*l)->save(layersElem, doc);
    }

    rwLockLayers.unlock();
}

/****************************************************************************/

void Section::connectChannels(Model *model, const QDir &dir)
{
    rwLockLayers.lockForRead();

    for (QList<Layer *>::const_iterator l = layers.begin();
            l != layers.end(); l++) {
        (*l)->connectChannel(model, dir);
    }

    rwLockLayers.unlock();

    updateLegend();
}

/****************************************************************************/

/** Returns, if a directory is used by some layer.
 */
bool Section::dirInUse(const LibDLS::Directory *d)
{
    bool inUse;

    rwLockLayers.lockForRead();

    for (QList<Layer *>::const_iterator l = layers.begin();
            l != layers.end(); l++) {
        inUse = (*l)->dirInUse(d);
        if (inUse) {
            break;
        }
    }

    rwLockLayers.unlock();

    return inUse;
}

/****************************************************************************/

void Section::setAutoScale(bool a)
{
    if (a != autoScale) {
        autoScale = a;
        if (autoScale) {
            updateExtrema();
        }
        updateScale();
        graph->update();
    }
}

/****************************************************************************/

void Section::setShowScale(bool s)
{
    if (s != showScale) {
        showScale = s;
        graph->update(); // FIXME notify graph on scale change?
    }
}

/****************************************************************************/

void Section::setScaleMinimum(double min)
{
    if (min != scaleMin) {
        scaleMin = min;
        if (!autoScale) {
            updateScale();
            graph->update();
        }
    }
}

/****************************************************************************/

void Section::setScaleMaximum(double max)
{
    if (max != scaleMax) {
        scaleMax = max;
        if (!autoScale) {
            updateScale();
            graph->update();
        }
    }
}

/****************************************************************************/

void Section::setHeight(int h)
{
    if (h < 0) {
        h = 0;
    }

    if (h != height) {
        height = h;
        updateScale();
        graph->update();
    }
}

/****************************************************************************/

void Section::setRelativePrintHeight(double h)
{
    relativePrintHeight = h;
}

/****************************************************************************/

void Section::resize(int width)
{
    legend.setPageSize(QSize(width, height));
}

/****************************************************************************/

void spreadGroup(QList<Layer::MeasureData> &list,
        unsigned int group, int labelHeight)
{
    int sumY = 0;
    unsigned int count = 0;

    for (QList<Layer::MeasureData>::const_iterator measure = list.begin();
            measure != list.end(); measure++) {
        if (measure->group == group) {
            sumY += measure->meanY;
            count++;
        }
    }

    if (!count) {
        return;
    }

    int off = sumY / count - (labelHeight * (count - 1) / 2);
    unsigned int index = 0;

    for (QList<Layer::MeasureData>::iterator measure = list.begin();
            measure != list.end(); measure++) {
        if (measure->group == group) {
            measure->movedY = off + labelHeight * index++;
        }
    }
}

/****************************************************************************/

void Section::draw(QPainter &painter, const QRect &rect, int measureX,
        int scaleWidth, bool drawBusy)
{
    QRect legendRect(rect);
    legendRect.setHeight(legend.size().height());
    QRect scaleRect(rect);
    scaleRect.setTop(legendRect.bottom() + 1);
    QRect dataRect(rect);
    dataRect.setLeft(rect.left() + scaleWidth);
    dataRect.setTop(legendRect.bottom() + 1);

    if (legendRect.isValid()) {
        painter.fillRect(legendRect, graph->palette().window());

        painter.save();
        painter.translate(rect.topLeft());
        legend.drawContents(&painter);
        painter.restore();

        if (busy && drawBusy) {
            QRect busyRect(legendRect);
            busyRect.setWidth(legendRect.height());
            busyRect.moveRight(legendRect.right());
            graph->busySvg.render(&painter, busyRect);
        }
    }

    dataRect.adjust(0, Margin, 0, -Margin);

    if (!dataRect.isValid() || graph->getStart() >= graph->getEnd()) {
        return;
    }

    if (showScale) {
        scale.draw(painter, scaleRect, scaleWidth);
    }

    double xScale = (dataRect.width() - 1) /
        (graph->getEnd() - graph->getStart()).to_dbl_time();

    double yScale;
    if (scale.getMin() < scale.getMax()) {
        yScale = (dataRect.height() - 1) / scale.getRange();
    }
    else {
        yScale = 0.0;
    }

    Layer::MeasureData measureData;
    measureData.x = measureX;
    QList<Layer::MeasureData> measureList;
    QFont font;
    font.setPointSize(8);
    QFontMetrics fm(font);
    unsigned int group = 1;
    int totalLabelHeight = 0;

    rwLockLayers.lockForRead();

    // draw data and capture measuring intersections
    for (QList<Layer *>::const_iterator l = layers.begin();
            l != layers.end(); l++) {
        Layer::MeasureData *measure;

        if (measureX > -1) {
            measureData.found = false;
            measure = &measureData;
        }
        else {
            measure = NULL;
        }

        (*l)->draw(painter, dataRect, xScale, yScale, scale.getMin(),
                measure);

        if (measure && measureData.found &&
                totalLabelHeight + fm.height() <= dataRect.height()) {
            measureData.layer = *l;
            measureData.meanY = (measureData.maxY + measureData.minY) / 2;
            measureData.movedY = measureData.meanY;
            measureData.group = group++;
            measureList.append(measureData);
            totalLabelHeight += fm.height();
        }
    }

    rwLockLayers.unlock();

    // sort labels by minimum value
    qStableSort(measureList.begin(), measureList.end());

    // eliminate overlaps
    bool foundOverlaps;
    do {
        foundOverlaps = false;
        int lastY = -1, lastGroup = 0;

        for (QList<Layer::MeasureData>::iterator measure =
                measureList.begin();
                measure != measureList.end(); measure++) {
            if (lastY >= 0) {
                if (measure->movedY - lastY < fm.height()) {
                    foundOverlaps = true;
                    measure->group = lastGroup; // merge groups
                    spreadGroup(measureList, measure->group, fm.height());
                }
            }
            lastY = measure->movedY;
            lastGroup = measure->group;
        }
    }
    while (foundOverlaps);

    // push out-of-range labels into the drawing rect from bottom side
    if (!measureList.empty()) {
        QList<Layer::MeasureData>::iterator measure = measureList.begin();
        int bottom = measure->movedY - fm.height() / 2;
        if (bottom < 0) {
            measure->movedY -= bottom;
            int lastY = measure->movedY;
            measure++;
            for (; measure != measureList.end(); measure++) {
                if (measure->movedY - lastY >= fm.height()) {
                    break;
                }
                measure->movedY = lastY + fm.height();
                lastY = measure->movedY;
            }
        }
    }

    // push out-of-range labels into the drawing rect from top side
    if (!measureList.empty()) {
        QList<Layer::MeasureData>::iterator measure = measureList.end();
        measure--;
        int over = measure->movedY + fm.height() / 2 - dataRect.height();
        if (over > 0) {
            measure->movedY -= over;
            int lastY = measure->movedY;

            while (measure != measureList.begin()) {
                measure--;

                if (lastY - measure->movedY >= fm.height()) {
                    break;
                }
                measure->movedY = lastY - fm.height();
                lastY = measure->movedY;
            }
        }
    }

    // draw measuring labels
    for (QList<Layer::MeasureData>::const_iterator measure =
            measureList.begin();
            measure != measureList.end(); measure++) {
        QString label;
        QPen pen;

        pen.setColor(measure->layer->getColor());
        painter.setPen(pen);
        painter.setFont(font);

        if (measure->minimum != measure->maximum) {
            label = measure->layer->formatValue(measure->minimum) +
                QString().fromUtf8(" – ") + // unicode en-dash U+2013
                measure->layer->formatValue(measure->maximum);
        }
        else {
            label = measure->layer->formatValue(measure->minimum);
        }

        QRect textRect(dataRect);
        textRect.setLeft(dataRect.left() + measure->x + 10);
        textRect.moveTop(
                dataRect.bottom() + 1 - measure->movedY - fm.height() / 2);
        textRect.setHeight(fm.height());
        int flags = Qt::AlignLeft | Qt::AlignVCenter;
        QRect rect = fm.boundingRect(textRect, flags, label);

        // try drawing left from measure line
        if (rect.width() > textRect.width() &&
                measure->x - 10 > textRect.width()) {
            textRect.setLeft(dataRect.left());
            textRect.setWidth(measure->x - 10);
            rect.moveRight(textRect.right());
            flags = Qt::AlignRight | Qt::AlignVCenter;
        }

        painter.save();
        painter.setClipRect(dataRect, Qt::IntersectClip);
        QRect backRect = rect.adjusted(-2, 0, 2, 0);
        painter.fillRect(backRect, Qt::white);
        painter.drawText(textRect, flags, label);
        QPen linePen;
        painter.setPen(linePen);
        painter.drawLine(backRect.bottomLeft(), backRect.bottomRight());
        painter.drawLine(backRect.bottomRight(), backRect.topRight());
        painter.restore();
    }
}

/****************************************************************************/

double Section::relativeHeight(int totalHeight) const
{
    if (relativePrintHeight >= 0) {
        return relativePrintHeight;
    }
    else if (totalHeight > 0) {
        return (double) height / totalHeight;
    }
    else {
        return 0.0;
    }
}

/****************************************************************************/

Layer *Section::appendLayer(QtDls::Channel *ch)
{
    Layer *l = new Layer(this);

    l->setChannel(ch);

    rwLockLayers.lockForWrite();
    layers.append(l);
    rwLockLayers.unlock();

    updateLegend();
    return l;
}

/****************************************************************************/

void Section::getRange(bool &valid, LibDLS::Time &start, LibDLS::Time &end)
{
    LibDLS::Time s, e;

    rwLockLayers.lockForRead();

    for (QList<Layer *>::const_iterator l = layers.begin();
            l != layers.end(); l++) {
        Channel *ch = (*l)->getChannel();
        if (!ch || !ch->getRange(s, e)) {
            continue;
        }

        if (valid) {
            if (s < start) {
                start = s;
            }
            if (e > end) {
                end = e;
            }
        }
        else {
            start = s;
            end = e;
            valid = true;
        }
    }

    rwLockLayers.unlock();
}

/****************************************************************************/

void Section::loadData(const LibDLS::Time &start, const LibDLS::Time &end,
        int width, GraphWorker *worker, std::set<LibDLS::Job *> &jobSet)
{
    rwLockLayers.lockForRead();

    for (QList<Layer *>::const_iterator l = layers.begin();
            l != layers.end(); l++) {
        (*l)->loadData(start, end, width, worker, jobSet);
    }

    rwLockLayers.unlock();
}

/****************************************************************************/

QColor Section::nextColor()
{
    unsigned int used[NUM_COLORS];

    for (unsigned int i = 0; i < NUM_COLORS; i++) {
        used[i] = 0;
    }

    // count usage and find maximum
    unsigned int max = 0U;

    rwLockLayers.lockForRead();

    for (QList<Layer *>::const_iterator l = layers.begin();
            l != layers.end(); l++) {
        for (unsigned int i = 0; i < NUM_COLORS;
                i++) {
            if (colorList[i] == (*l)->getColor()) {
                used[i]++;
                if (used[i] > max) {
                    max = used[i];
                }
                break;
            }
        }
    }

    rwLockLayers.unlock();

    // find minimum
    unsigned int min = max;
    for (unsigned int i = 0; i < NUM_COLORS; i++) {
        if (used[i] < min) {
            min = used[i];
        }
    }

    // return first in list that is least used
    for (unsigned int i = 0; i < NUM_COLORS; i++) {
        if (used[i] == min) {
            return colorList[i];
        }
    }

    return Qt::blue;
}

/****************************************************************************/

bool Section::getExtrema(double &min, double &max)
{
    updateExtrema();

    min = minimum;
    max = maximum;

    return extremaValid;
}

/****************************************************************************/

void Section::setBusy(bool b)
{
    busy = b;
    // TODO update legend only
}

/****************************************************************************/

void Section::update()
{
    if (autoScale) {
        updateExtrema();
        updateScale();
    }

    graph->update(); // FIXME update section only
}

/****************************************************************************/

QSet<Channel *> Section::channels()
{
    QSet<Channel *> channels;

    rwLockLayers.lockForRead();

    for (QList<Layer *>::const_iterator l = layers.begin();
            l != layers.end(); l++) {
        if ((*l)->getChannel()) {
            channels += (*l)->getChannel();
        }
    }

    rwLockLayers.unlock();

    return channels;
}

/****************************************************************************/

void Section::updateLegend()
{
    int pointSize = 8;

    QString html = QString("<html><head><meta http-equiv=\"Content-Type\" "
        "content=\"text/html; charset=utf-8\"></head>"
        "<body style=\"font-size: %1pt\">").arg(pointSize);

    QFont f(legend.defaultFont());
    f.setPointSize(pointSize);
    QFontMetrics fm(f);
    int ascent = fm.ascent();

    bool first = true;

    rwLockLayers.lockForRead();

    for (QList<Layer *>::const_iterator l = layers.begin();
            l != layers.end(); l++) {
        if (first) {
            first = false;
        } else {
            html += ", ";
        }

        QString col, img;
        if ((*l)->getChannel()) {
            col = (*l)->getColor().name();
        }
        else {
            col = "#555555";
            img = QString("<img width=\"%1\" height=\"%1\" "
                    "src=\":/DlsWidgets/images/dialog-error.svg\"/> ")
                .arg(ascent);
        }

        html += "<nobr style=\"color: " + col + ";\">";
        html += img + (*l)->title();
        html += "</nobr>";
    }

    rwLockLayers.unlock();

    html += "</body></html>";

    legend.setHtml(html);
}

/****************************************************************************/

void Section::updateScale()
{
    double min, max;

    if (autoScale) {
        // use extrema of loaded data
        min = minimum;
        max = maximum;
    }
    else {
        min = scaleMin;
        max = scaleMax;
    }

    scale.setMin(min);
    scale.setMax(max);
    scale.setHeight(height - legend.size().height());
    scale.update();
}

/****************************************************************************/

void Section::updateExtrema()
{
    minimum = 0.0;
    maximum = 0.0;
    extremaValid = false;

    rwLockLayers.lockForRead();

    for (QList<Layer *>::const_iterator l = layers.begin();
            l != layers.end(); l++) {
        if (!(*l)->getExtremaValid()) {
            continue;
        }

        double min = (*l)->getMinimum();
        double max = (*l)->getMaximum();

        if (extremaValid) {
            if (min < minimum) {
                minimum = min;
            }
            if (max > maximum) {
                maximum = max;
            }
        } else {
            minimum = min;
            maximum = max;
            extremaValid = true;
        }
    }

    rwLockLayers.unlock();
}

/****************************************************************************/

void Section::clearLayers()
{
    rwLockLayers.lockForWrite();

    for (QList<Layer *>::const_iterator l = layers.begin();
            l != layers.end(); l++) {
        delete *l;
    }

    layers.clear();

    rwLockLayers.unlock();
}

/****************************************************************************/

void Section::loadLayers(const QDomElement &elem, Model *model,
        const QDir &dir)
{
    QDomNodeList children = elem.childNodes();

    for (int i = 0; i < children.size(); i++) {
        QDomNode node = children.item(i);
        if (!node.isElement()) {
            continue;
        }

        QDomElement child = node.toElement();
        if (child.tagName() != "Layer") {
            continue;
        }

        Layer *layer = new Layer(this);

        try {
            layer->load(child, model, dir);
        } catch (Layer::Exception &e) {
            delete layer;
            qWarning() << tr("Failed to load layer: %1").arg(e.msg);
            continue;
        }

        rwLockLayers.lockForWrite();
        layers.append(layer);
        rwLockLayers.unlock();
    }
}

/****************************************************************************/
