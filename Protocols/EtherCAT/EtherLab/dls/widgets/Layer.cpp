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

#include "DlsWidgets/Section.h"
#include "DlsWidgets/Layer.h"
#include "DlsWidgets/Graph.h"
#include "DlsWidgets/Model.h"
#include "Channel.h"

using DLS::Section;
using DLS::Layer;
using QtDls::Channel;

/****************************************************************************/

/** Constructor.
 */
Layer::Layer(
        Section *section
        ):
    section(section),
    channel(NULL),
    color(section->nextColor()),
    scale(1.0),
    offset(0.0),
    precision(-1),
    minimum(0.0),
    maximum(0.0),
    extremaValid(false)
{
}

/****************************************************************************/

/** Copy constructor.
 */
Layer::Layer(
        const Layer &o,
        Section *section
        ):
    section(section),
    channel(o.channel),
    urlString(o.urlString),
    name(o.name),
    unit(o.unit),
    color(o.color),
    scale(o.scale),
    offset(o.offset),
    precision(o.precision),
    minimum(o.minimum),
    maximum(o.maximum),
    extremaValid(o.extremaValid)
{
    dataMutex.lock();
    copyDataList(genericData, o.genericData);
    copyDataList(minimumData, o.minimumData);
    copyDataList(maximumData, o.maximumData);
    dataMutex.unlock();
}

/****************************************************************************/

/** Destructor.
 */
Layer::~Layer()
{
    dataMutex.lock();
    clearDataList(genericData);
    clearDataList(minimumData);
    clearDataList(maximumData);
    dataMutex.unlock();
}

/****************************************************************************/

void Layer::load(const QDomElement &e, QtDls::Model *model, const QDir &dir)
{
    if (e.hasAttribute("url")) {
        urlString = e.attribute("url");
    }
    else {
        qWarning() << tr("Layer element missing url attribute!");
    }

#if 0
    qDebug() << __func__ << this << urlString;
#endif

    connectChannel(model, dir);

    QDomNodeList children = e.childNodes();

    for (int i = 0; i < children.size(); i++) {
        QDomNode node = children.item(i);
        if (!node.isElement()) {
            continue;
        }

        QDomElement child = node.toElement();

        if (child.tagName() == "Name") {
            QString text = child.text();
            setName(text);
        }
        else if (child.tagName() == "Unit") {
            QString text = child.text();
            setUnit(text);
        }
        else if (child.tagName() == "Color") {
            QString text = child.text();
            QColor c;
            c.setNamedColor(text);
            if (!c.isValid()) {
                throw Exception(QString("Invalid color %1!").arg(text));
            }
            setColor(c);
        }
        else if (child.tagName() == "Scale") {
            QString text = child.text();
            bool ok;
            double num = text.toDouble(&ok);
            if (!ok) {
                QString msg("Invalid value in Scale");
                throw Exception(msg);
            }
            setScale(num);
        }
        else if (child.tagName() == "Offset") {
            QString text = child.text();
            bool ok;
            double num = text.toDouble(&ok);
            if (!ok) {
                QString msg("Invalid value in Offset");
                throw Exception(msg);
            }
            setOffset(num);
        }
        else if (child.tagName() == "Precision") {
            QString text = child.text();
            bool ok;
            int num = text.toInt(&ok);
            if (!ok) {
                QString msg("Invalid value in Precision");
                throw Exception(msg);
            }
            setPrecision(num);
        }
    }
}

/****************************************************************************/

/** Saves settings to an XML element.
 */
void Layer::save(QDomElement &e, QDomDocument &doc) const
{
    QDomElement layerElem = doc.createElement("Layer");
    layerElem.setAttribute("url", urlString);
    e.appendChild(layerElem);

    QDomElement elem = doc.createElement("Name");
    QDomText text = doc.createTextNode(name);
    elem.appendChild(text);
    layerElem.appendChild(elem);

    elem = doc.createElement("Unit");
    text = doc.createTextNode(unit);
    elem.appendChild(text);
    layerElem.appendChild(elem);

    elem = doc.createElement("Color");
    text = doc.createTextNode(color.name());
    elem.appendChild(text);
    layerElem.appendChild(elem);

    elem = doc.createElement("Scale");
    QString num;
    num.setNum(scale);
    text = doc.createTextNode(num);
    elem.appendChild(text);
    layerElem.appendChild(elem);

    elem = doc.createElement("Offset");
    num.setNum(offset);
    text = doc.createTextNode(num);
    elem.appendChild(text);
    layerElem.appendChild(elem);

    elem = doc.createElement("Precision");
    num.setNum(precision);
    text = doc.createTextNode(num);
    elem.appendChild(text);
    layerElem.appendChild(elem);
}

/****************************************************************************/

void Layer::connectChannel(QtDls::Model *model, const QDir &dir)
{
#if 0
    qDebug() << __func__ << this << urlString;
#endif

    if (channel) {
        return;
    }

    QUrl url;

    url = QUrl(urlString);

    if (url.isValid()) {
        // allow relative paths
        if (url.scheme().isEmpty() || url.scheme() == "file") {
            QString path = url.path();
            if (QDir::isRelativePath(path)) {
                url.setPath(QDir::cleanPath(dir.absoluteFilePath(path)));
            }
        }
    }
    else {
        qWarning() << tr("Invalid URL %1!").arg(url.toString());
        return;
    }

    if (!url.isEmpty()) {
        try {
            channel = model->getChannel(url);
        }
        catch (QtDls::Model::Exception &e) {
            qWarning() << tr("Failed to get channel %1: %2")
                .arg(url.toString())
                .arg(e.msg);
        }
    }
}

/****************************************************************************/

/** Returns, if the directory is used by this layer.
 */
bool Layer::dirInUse(const LibDLS::Directory *d) const
{
    return channel && channel->job()->dir() == d;
}

/****************************************************************************/

void Layer::setChannel(QtDls::Channel *ch)
{
    channel = ch;
    urlString = ch->url().toString();
}

/****************************************************************************/

void Layer::setName(const QString &n)
{
    if (n != name) {
        name = n;
        section->updateLegend();
    }
}

/****************************************************************************/

void Layer::setUnit(const QString &u)
{
    if (u != unit) {
        unit = u;
        section->updateLegend();
    }
}

/****************************************************************************/

void Layer::setColor(QColor c)
{
    if (!c.isValid()) {
        c = section->nextColor();
    }

    if (c != color) {
        color = c;
        section->updateLegend();
    }
}

/****************************************************************************/

void Layer::setScale(double s)
{
    if (s != scale) {
        scale = s;
        dataMutex.lock();
        updateExtrema();
        dataMutex.unlock();
        section->update();
    }
}

/****************************************************************************/

void Layer::setOffset(double o)
{
    if (o != offset) {
        offset = o;
        dataMutex.lock();
        updateExtrema();
        dataMutex.unlock();
        section->update();
    }
}

/****************************************************************************/

void Layer::setPrecision(int p)
{
    if (p < -1) {
        p = -1;
    }

    if (p != precision) {
        precision = p;
    }
}

/****************************************************************************/

void Layer::loadData(const LibDLS::Time &start, const LibDLS::Time &end,
        int width, GraphWorker *worker, std::set<LibDLS::Job *> &jobSet)
{
#if 0
    qDebug() << __func__ << start.to_str().c_str()
        << end.to_str().c_str() << width;
#endif

    if (!channel) {
        return;
    }

    worker->clearData();
    channel->fetchData(start, end, width,
            GraphWorker::dataCallback, worker, 1);

    dataMutex.lock();
    // FIXME implement transferDataList
    copyDataList(genericData, worker->genData());
    copyDataList(minimumData, worker->minData());
    copyDataList(maximumData, worker->maxData());
    updateExtrema();
    dataMutex.unlock();

    jobSet.insert(channel->job());
}

/****************************************************************************/

QString Layer::title() const
{
    QString ret;

    if (!name.isEmpty()) {
        ret = name;
    }
    else if (channel) {
        ret = channel->name();
    }
    else {
        ret = urlString;
    }

    if (!unit.isEmpty()) {
        ret += " [" + unit + "]";
    }

    return ret;
}

/****************************************************************************/

QString Layer::formatValue(double value) const
{
    QString ret;

    ret = QLocale().toString(value, 'f', precision);

    if (!unit.isEmpty()) {
        if (unit != "°") {
            ret += QChar(0x202f); // narrow no-break space U+202f
        }

        ret += unit;
    }

    return ret;
}

/****************************************************************************/

void Layer::clearDataList(QList<LibDLS::Data *> &list)
{
    for (QList<LibDLS::Data *>::iterator d = list.begin();
            d != list.end(); d++) {
        delete *d;
    }

    list.clear();
}

/****************************************************************************/

void Layer::copyDataList(QList<LibDLS::Data *> &list,
        const QList<LibDLS::Data *> &other)
{
    clearDataList(list);

    for (QList<LibDLS::Data *>::const_iterator d = other.begin();
            d != other.end(); d++) {
        LibDLS::Data *data = new LibDLS::Data(**d);
        list.push_back(data);
    }
}

/****************************************************************************/

void Layer::updateExtrema()
{
    bool first = true;

    updateExtremaList(genericData, &first);
    updateExtremaList(minimumData, &first);
    updateExtremaList(maximumData, &first);

    extremaValid = !first;
}

/****************************************************************************/

void Layer::updateExtremaList(const QList<LibDLS::Data *> &list, bool *first)
{
    for (QList<LibDLS::Data *>::const_iterator d = list.begin();
            d != list.end(); d++) {
        double current_min, current_max;

        if (!(*d)->calc_min_max(&current_min, &current_max)) {
            continue;
        }

        if (scale >= 0.0) {
            current_min = current_min * scale + offset;
            current_max = current_max * scale + offset;
        }
        else {
            current_min = current_max * scale + offset;
            current_max = current_min * scale + offset;
        }

        if (*first) {
            minimum = current_min;
            maximum = current_max;
            *first = false;
        }
        else {
            if (current_min < minimum) {
                minimum = current_min;
            }
            if (current_max > maximum) {
                maximum = current_max;
            }
        }
    }
}

/****************************************************************************/

void Layer::draw(QPainter &painter, const QRect &rect, double xScale,
        double yScale, double min, MeasureData *measure)
{
    drawGaps(painter, rect, xScale);

    dataMutex.lock();

    if (genericData.size()) {
        double prev_value = 0.0;
        int prev_xp = 0, prev_yp = 0;
        bool first_in_chunk = true;

        QPen pen;
        pen.setColor(color);

        painter.save();
        painter.setPen(pen);
        painter.setClipRect(rect, Qt::IntersectClip);

        for (QList<LibDLS::Data *>::const_iterator d = genericData.begin();
                d != genericData.end(); d++) {

            for (unsigned int i = 0; i < (*d)->size(); i++) {
                double value = (*d)->value(i) * scale + offset;
                LibDLS::Time dt = (*d)->time(i)
                    - section->getGraph()->getStart();
                double xv = dt.to_dbl_time() * xScale;
                double yv = (value -  min) * yScale;
                int xp, yp;

                if (xv >= 0.0) {
                    xp = (int) (xv + 0.5);
                }
                else {
                    xp = (int) (xv - 0.5);
                }

                if (yv >= 0.0) {
                    yp = (int) (yv + 0.5);
                }
                else {
                    yp = (int) (yv - 0.5);
                }

                if (xp >= 0) {
                    if (first_in_chunk) {
                        QPointF p(rect.left() + xv, rect.bottom() - yv);
                        painter.drawPoint(p);
                    }
                    else {
                        QPoint prev(rect.left() + prev_xp,
                                rect.bottom() - prev_yp);
                        QPoint inter(rect.left() + xp,
                                rect.bottom() - prev_yp);
                        QPoint cur(rect.left() + xp,
                                rect.bottom() - yp);

                        painter.drawLine(prev, inter);
                        painter.drawLine(inter, cur);
                    }

                    if (measure) {
                        if (xp == measure->x) {
                            if (measure->found) {
                                if (value < measure->minimum) {
                                    measure->minimum = value;
                                    measure->minY = yp;
                                }
                                if (value > measure->maximum) {
                                    measure->maximum = value;
                                    measure->maxY = yp;
                                }
                            }
                            else {
                                measure->minimum = value;
                                measure->maximum = value;
                                measure->minY = yp;
                                measure->maxY = yp;
                                measure->found = true;
                            }
                        }
                        else if (xp > measure->x && prev_xp
                                < measure->x && !first_in_chunk) {
                            measure->minimum = prev_value;
                            measure->maximum = prev_value;
                            measure->minY = prev_yp;
                            measure->maxY = prev_yp;
                            measure->found = true;
                        }
                    }

                    if (xp >= rect.width()) {
                        break;
                    }
                }

                if (xp >= rect.width()) {
                    break;
                }

                prev_xp = xp;
                prev_yp = yp;
                prev_value = value;
                first_in_chunk = false;
            }
        }

        painter.restore();
    }

    if (minimumData.size() && maximumData.size()) {
        double yv, value;
        int xp, yp, i;
        unsigned int j;

        struct extrema {
            int min;
            int max;
            bool minValid;
            bool maxValid;
        };
        struct extrema *extrema;

        try {
            extrema = new struct extrema[rect.width()];
        }
        catch (...) {
            QString msg;
            QTextStream str(&msg);
            str << "ERROR: Failed to allocate drawing memory!";
            LibDLS::log(msg.toLocal8Bit().constData());
            return;
        }

        for (i = 0; i < rect.width(); i++) {
            extrema[i].minValid = false;
            extrema[i].maxValid = false;
        }

        QPen pen;
        pen.setColor(color);
        painter.setPen(pen);

        const QList<LibDLS::Data *> *minData, *maxData;
        if (scale >= 0.0) {
            minData = &minimumData;
            maxData = &maximumData;
        }
        else {
            minData = &maximumData;
            maxData = &minimumData;
        }

        for (QList<LibDLS::Data *>::const_iterator d = minData->begin();
                d != minData->end(); d++) {

            for (j = 0; j < (*d)->size(); j++) {
                value = (*d)->value(j) * scale + offset;
                LibDLS::Time dt = (*d)->time(j)
                    - section->getGraph()->getStart();
                double xv = dt.to_dbl_time() * xScale;
                yv = (value - min) * yScale;

                if (xv >= 0.0) {
                    xp = (int) (xv + 0.5);
                }
                else {
                    xp = (int) (xv - 0.5);
                }
                if (yv >= 0.0) {
                    yp = (int) (yv + 0.5);
                }
                else {
                    yp = (int) (yv - 0.5);
                }

                if (xp >= 0 && xp < rect.width()) {
                    if (!extrema[xp].minValid ||
                            (extrema[xp].minValid && yp < extrema[xp].min)) {
                        extrema[xp].min = yp;
                        extrema[xp].minValid = true;

                        if (measure && xp == measure->x) {
                            if (measure->found) {
                                if (value < measure->minimum) {
                                    measure->minimum = value;
                                    measure->minY = yp;
                                }
                                if (value > measure->maximum) {
                                    measure->maximum = value;
                                    measure->maxY = yp;
                                }
                            }
                            else {
                                measure->minimum = value;
                                measure->maximum = value;
                                measure->minY = yp;
                                measure->maxY = yp;
                                measure->found = true;
                            }
                        }
                    }
                }

                else if (xp >= rect.width()) {
                    break;
                }
            }
        }

        for (QList<LibDLS::Data *>::const_iterator d = maxData->begin();
                d != maxData->end(); d++) {

            for (j = 0; j < (*d)->size(); j++) {
                value = (*d)->value(j) * scale + offset;
                LibDLS::Time dt = (*d)->time(j)
                    - section->getGraph()->getStart();
                double xv = dt.to_dbl_time() * xScale;
                yv = (value - min) * yScale;

                if (xv >= 0.0) {
                    xp = (int) (xv + 0.5);
                }
                else {
                    xp = (int) (xv - 0.5);
                }
                if (yv >= 0.0) {
                    yp = (int) (yv + 0.5);
                }
                else {
                    yp = (int) (yv - 0.5);
                }

                if (xp >= 0 && xp < rect.width()) {
                    if (!extrema[xp].maxValid ||
                            (extrema[xp].maxValid && yp > extrema[xp].max)) {
                        extrema[xp].max = yp;
                        extrema[xp].maxValid = true;

                        if (measure && xp == measure->x) {
                            if (measure->found) {
                                if (value < measure->minimum) {
                                    measure->minimum = value;
                                    measure->minY = yp;
                                }
                                if (value > measure->maximum) {
                                    measure->maximum = value;
                                    measure->maxY = yp;
                                }
                            }
                            else {
                                measure->minimum = value;
                                measure->maximum = value;
                                measure->minY = yp;
                                measure->maxY = yp;
                                measure->found = true;
                            }
                        }
                    }
                }

                else if (xp >= rect.width()) {
                    break;
                }
            }
        }

        QRect col;
        col.setWidth(1);
        for (i = 0; i < rect.width(); i++) {
            col.moveLeft(rect.left() + i);
            if (extrema[i].minValid && extrema[i].maxValid) {
                if (extrema[i].min >= rect.height() || extrema[i].max < 0) {
                    continue;
                }
                if (extrema[i].min < 0) {
                    extrema[i].min = 0;
                }
                if (extrema[i].max >= rect.height()) {
                    extrema[i].max = rect.height() - 1;
                }
                if (extrema[i].min != extrema[i].max) {
                    col.setTop(rect.bottom() - extrema[i].max);
                    col.setHeight(extrema[i].max - extrema[i].min + 1);
                }
                else {
                    col.setTop(rect.bottom() - extrema[i].max);
                    col.setHeight(1);
                }
                painter.fillRect(col, color);
            }
            else {
                if (extrema[i].minValid && extrema[i].min >= 0 &&
                        extrema[i].min < rect.height()) {
                    col.setTop(rect.bottom() - extrema[i].min);
                    col.setHeight(1);
                    painter.fillRect(col, color);
                }
                if (extrema[i].maxValid && extrema[i].max >= 0 &&
                        extrema[i].max < rect.height()) {
                    col.setTop(rect.bottom() - extrema[i].max);
                    col.setHeight(1);
                    painter.fillRect(col, color);
                }
            }
        }

        delete [] extrema;
    }

    dataMutex.unlock();
}

/****************************************************************************/

void Layer::drawGaps(QPainter &painter, const QRect &rect,
        double xScale) const
{
    double xp, prev_xp;
    std::vector<Channel::TimeRange> ranges, relevant_chunk_ranges;
    std::vector<Channel::TimeRange> overlap;
    QColor gapColor(0xff, 0xec, 0x6b, 127);
    QColor overlapColor(255, 0, 0, 127);

    if (channel) {
        ranges = channel->chunkRanges();
    }

    // check if chunks overlap and identify relevant ranges
    for (std::vector<Channel::TimeRange>::iterator range = ranges.begin();
         range != ranges.end();
         range++) {
        if (range->end < section->getGraph()->getStart()) {
            continue;
        }
        if (range->start > section->getGraph()->getEnd()) {
            break;
        }

        bool not_overlapping = true;

        if (!relevant_chunk_ranges.empty()) {
            Channel::TimeRange &last = relevant_chunk_ranges.back();
            if (range->start <= last.end) {

                Channel::TimeRange lap;
                lap.start = range->start;
                if (range->end < last.end) {
                    lap.end = range->end;
                } else {
                    lap.end = last.end;
                    last.end = range->end; // extend last range
                }
                overlap.push_back(lap);
                not_overlapping = false;
            }
        }

        if (not_overlapping) {
            relevant_chunk_ranges.push_back(*range);
        }
    }

    if (!overlap.empty()) {
        QString msg;
        QTextStream str(&msg);
        str << "WARNING: Chunks overlapping in channel"
            << channel->name();
        LibDLS::log(msg.toLocal8Bit().constData());
    }

    prev_xp = -1;

    // draw gaps
    for (std::vector<Channel::TimeRange>::iterator range =
            relevant_chunk_ranges.begin();
         range != relevant_chunk_ranges.end(); range++) {
        xp = (range->start -
                section->getGraph()->getStart()).to_dbl_time() * xScale;

        if (xp > prev_xp + 1) {
            QRect drawRect(rect.left() + (int) (prev_xp + 1.5),
                     rect.top(),
                     (int) (xp - prev_xp - 1),
                     rect.height());
            painter.fillRect(drawRect, gapColor);
        }

        prev_xp = (range->end -
                section->getGraph()->getStart()).to_dbl_time() * xScale;
    }

    // draw last gap
    if (rect.width() > prev_xp + 1) {
        QRect drawRect(rect.left() + (int) (prev_xp + 1.5),
                rect.top(),
                (int) (rect.width() - prev_xp - 1),
                rect.height());
        painter.fillRect(drawRect, gapColor);
    }

    // draw overlaps
    for (std::vector<Channel::TimeRange>::iterator range =
            overlap.begin();
         range != overlap.end(); range++) {
        int xs = (range->start -
                section->getGraph()->getStart()).to_dbl_time() * xScale;
        int xe = (range->end -
                section->getGraph()->getStart()).to_dbl_time() * xScale;
        int w = xe - xs;
        if (w < 1) {
            w = 1;
        }

        QRect drawRect(rect.left() + xs, rect.top(),
                w, rect.height());
        painter.fillRect(drawRect, overlapColor);
    }
}

/****************************************************************************/
