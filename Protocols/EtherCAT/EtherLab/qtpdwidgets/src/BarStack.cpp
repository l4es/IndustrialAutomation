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

#include <QGradient>
#include <QtGui>

#include "BarStack.h"
#include "BarSection.h"

/****************************************************************************/

/** Constructor.
 */
Bar::Stack::Stack(
        Bar *bar /**< Parent bar graph. */
        ):
    bar(bar)
{
#if 0
    Section *section = new Section(this, Qt::blue);
    sections.append(section);
    section = new Section(this, Qt::red);
    sections.append(section);
#endif
}

/****************************************************************************/

/** Destructor.
 */
Bar::Stack::~Stack()
{
    for (SectionList::iterator section = sections.begin();
            section != sections.end(); section++) {
        delete *section;
    }
}

/****************************************************************************/

/** Paint method.
 */
void Bar::Stack::paint(QPainter &painter)
{
    painter.setClipRect(rect);

    switch (bar->style) {
        case ColorBar:
        case MultiColorBar:
            paintColorBar(painter);
            break;
        case Arrow:
            paintArrow(painter);
            break;
    }

    // draw drag indicators
    if (sections.count() == 1 && sections.front()->hasData() &&
            sections.front()->getFilterConstant() > 0.0) {
        QPolygon dip;
        int pos;

        pos = bar->calcPosition(sections.front()->getDragMax());
        if (pos >= 0.0) {
            if (bar->orientation == Bar::Vertical) {
                dip = bar->verticalDragIndicatorPolygon;
                dip.translate(rect.left(), (int) (rect.bottom() - pos));
            } else {
                dip = bar->horizontalDragIndicatorPolygon;
                dip.translate((int) (rect.left() + pos), rect.top());
            }

            painter.setPen(Qt::black);
            painter.setBrush(Qt::black);
            painter.drawPolygon(dip);
        }

        pos = bar->calcPosition(sections.front()->getDragMin());
        if (pos >= 0.0) {
            if (bar->orientation == Bar::Vertical) {
                dip = bar->verticalDragIndicatorPolygon;
                dip.translate(rect.left(), (int) (rect.bottom() - pos));
            } else {
                dip = bar->horizontalDragIndicatorPolygon;
                dip.translate((int) (rect.left() + pos), rect.top());
            }

            painter.setPen(Qt::black);
            painter.setBrush(Qt::black);
            painter.drawPolygon(dip);
        }
    }
}

/****************************************************************************/

void Bar::Stack::addSection(PdCom::Variable *pv, double sampleTime,
        double gain, double offset, double tau, QColor color)
{
    Section *section = new Section(this, color);
    section->setVariable(pv, sampleTime, gain, offset, tau);
    sections.append(section);
}

/****************************************************************************/

void Bar::Stack::redrawEvent()
{
    for (SectionList::const_iterator section = sections.begin();
            section != sections.end(); section++) {
        (*section)->redrawEvent();
    }
}

/****************************************************************************/

void Bar::Stack::update()
{
    bar->update(rect);
}

/****************************************************************************/

void Bar::Stack::clearData()
{
    for (SectionList::const_iterator section = sections.begin();
            section != sections.end(); section++) {
        (*section)->clearData();
    }
    bar->update(rect);
}

/****************************************************************************/

/** Paint method for ColorBar style.
 */
void Bar::Stack::paintColorBar(QPainter &painter)
{
    double value, lastVal;
    int lastPos;

    switch (bar->origin) {
        case OriginMinimum:
            value = bar->valueScale.getMin();
            break;
        case OriginMaximum:
            value = bar->valueScale.getMax();
            break;
        case OriginZero:
        default:
            value = 0.0;
    }

    lastPos = bar->calcPosition(value);
    lastVal = 0.0;

    for (SectionList::const_iterator section = sections.begin();
            section != sections.end(); section++) {
        if (!(*section)->hasData()) {
            /* abort painting following sections. */
            break;
        }

        QRect barRect;

        value = (*section)->getValue() + lastVal;
        int pos = bar->calcPosition(value);
        int len = pos - lastPos;

        Pd::Bar::Style s = bar->getStyle();
        int gradientNum = bar->gradientStops.size();

        painter.setPen(Qt::NoPen);
        if (gradientNum && s == Pd::Bar::ColorBar) {
            painter.setBrush(bar->gradient);
        }
        else if (gradientNum && s == Pd::Bar::MultiColorBar) {
            painter.setBrush(findMultiColor((*section)->getValue()));
        }
        else {
            painter.setBrush((*section)->getColor());
        }

        if (bar->orientation == Bar::Vertical) {
            barRect = QRect(rect.left(),
                    rect.bottom() - lastPos - len + 1,
                    rect.width(), len);
        } else {
            barRect = QRect(rect.left() + lastPos, rect.top(),
                    len, rect.height());
        }

        painter.drawRect(barRect);

        lastPos = pos;
        lastVal = value;
    }
}


/****************************************************************************/

/** Paint method for Arrow style.
 */
void Bar::Stack::paintArrow(QPainter &painter)
{
    for (SectionList::const_iterator section = sections.begin();
            section != sections.end(); section++) {
        if (!(*section)->hasData()) {
            continue;
        }

        double value = (*section)->getValue();
        int pos = bar->calcPosition(value);

        //painter.setPen((*section)->getColor());
        //painter.setBrush(Qt::white);
        painter.setPen(Qt::NoPen);
        painter.setBrush((*section)->getColor());

        QPolygon poly;
        if (bar->orientation == Bar::Vertical) {
            poly = bar->verticalArrow;
            poly.translate(rect.left(), (int) (rect.bottom() - pos));
        }
        else {
            poly = bar->horizontalArrow;
            poly.translate(rect.left() + pos, rect.top());
        }

        painter.drawPolygon(poly);
    }
}

/****************************************************************************/

const QColor Bar::Stack::findMultiColor(double value) const
{
    QGradientStops gs = bar->gradient.stops();
    QPair<double, double> gradientLimits = bar->getGradientLimits();

    double minStop = gradientLimits.first;
    double maxStop = gradientLimits.second;

    if (value <= minStop || minStop - maxStop == 0) {
        return gs.first().second;
    } else if (value >= maxStop) {
        return gs.last().second;
    }

    QGradientStops::iterator it;

    double f = (value - minStop) / (maxStop - minStop);

    QGradientStop minColor, maxColor;

    for (it = gs.begin(); it < gs.end(); it++) {
        if (f >= (*it).first && f <= (*(it + 1)).first) {
            minColor = *it;
            maxColor = *(it + 1);
            break;
        }
    }

    double cf = (f - minColor.first) / (maxColor.first - minColor.first);

    int r = cf * (maxColor.second.red() - minColor.second.red())
        + minColor.second.red();
    int g = cf * (maxColor.second.green() - minColor.second.green())
        + minColor.second.green();
    int b = cf * (maxColor.second.blue() - minColor.second.blue())
        + minColor.second.blue();
    int a = cf * (maxColor.second.alpha() - minColor.second.alpha())
        + minColor.second.alpha();

    return QColor(r, g, b, a);
}

/****************************************************************************/
