/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2012  Florian Pose <fp@igh-essen.com>
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

#ifndef PD_BAR_STACK_H
#define PD_BAR_STACK_H

#include <QtGui>
#include <QList>

#include <QtPdWidgets/Bar.h>
using Pd::Bar;

/****************************************************************************/

class Bar::Stack
{
    public:
        Stack(Bar *);
        virtual ~Stack();

        void addSection(PdCom::Variable *,
                double = 0.0, double = 1.0, double = 0.0, double = 0.0,
                QColor = Qt::black);
        void clearData();

        QRect &getRect() { return rect; }
        const QRect &getRect() const { return rect; }

        void paint(QPainter &);

        void redrawEvent();
        void update();

    private:
        Bar * const bar;
        QRect rect;

        class Section;
        typedef QList<Section *> SectionList;
        SectionList sections;

        void paintColorBar(QPainter &);
        void paintArrow(QPainter &);
        const QColor findMultiColor(double) const;

        Stack();
};

#endif

/****************************************************************************/
