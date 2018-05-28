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

#ifndef PD_XYGRAPH_H
#define PD_XYGRAPH_H

#include <QFrame>

#include <pdcom.h>

#include "Export.h"
#include "Scale.h"

namespace Pd {

/****************************************************************************/

/** XY Graph.
 */
class QDESIGNER_WIDGET_EXPORT XYGraph:
    public QFrame
{
    Q_OBJECT
    /** The time range to display. Older points will be discarded. */
    Q_PROPERTY(double timeRange
            READ getTimeRange WRITE setTimeRange RESET resetTimeRange)
    /** The minimum value of the horizontal scale. */
    Q_PROPERTY(double scaleXMin
            READ getScaleXMin WRITE setScaleXMin RESET resetScaleXMin)
    /** The maximum value of the horizontal scale. */
    Q_PROPERTY(double scaleXMax
            READ getScaleXMax WRITE setScaleXMax RESET resetScaleXMax)
    /** The minimum value of the vertical scale. */
    Q_PROPERTY(double scaleYMin
            READ getScaleYMin WRITE setScaleYMin RESET resetScaleYMin)
    /** The maximum value of the vertical scale. */
    Q_PROPERTY(double scaleYMax
            READ getScaleYMax WRITE setScaleYMax RESET resetScaleYMax)

    public:
        XYGraph(QWidget *parent = 0);
        virtual ~XYGraph();

        void addVariable(PdCom::Variable *,
                double = 0.0, double = 1.0, double = 0.0);
        void clearVariables();
        void clearData();

        double getTimeRange() const;
        void setTimeRange(double);
        void resetTimeRange();
        double getScaleXMin() const;
        void setScaleXMin(double);
        void resetScaleXMin();
        double getScaleXMax() const;
        void setScaleXMax(double);
        void resetScaleXMax();
        double getScaleYMin() const;
        void setScaleYMin(double);
        void resetScaleYMin();
        double getScaleYMax() const;
        void setScaleYMax(double);
        void resetScaleYMax();

        virtual QSize sizeHint() const;

    protected:
        bool event(QEvent *);
        void resizeEvent(QResizeEvent *);
        void paintEvent(QPaintEvent *);

    private:
        /** Time/value pair type.
         */
        struct TimeValuePair {
            PdCom::Time time; /**< Time. */
            double value; /**< Value. */
        };

        /** Axis specification.
         */
        enum AxisOrientation {
            X, /**< The Horizontal axis. */
            Y, /**< The vertical axis. */
            NumAxes /**< Number of axes. */
        };

        /** Axis class.
         */
        class Axis: public PdCom::Subscriber {
            public:
                XYGraph * const graph;
                PdCom::Variable *variable;
                PdCom::Variable::Scale scale;
                QList<TimeValuePair> values; /**< List of values. */

                Axis(XYGraph *graph);

                void setVariable(PdCom::Variable *,
                        double = 0.0, double = 1.0, double = 0.0);
                void clearVariable();

                // pure-virtual from PdCom::Subscriber
                void notify(PdCom::Variable *);
                void notifyDelete(PdCom::Variable *);

                void removeDeprecated();

            private:
                Axis();
        };

        std::vector<Axis> axes; /**< Axes. */

        double timeRange; /**< See the #timeRange property. */
        Scale xScale; /**< X Scale. */
        Scale yScale; /**< Y Scale. */
        QRect contRect; /**< Contents area. This is the rectangle inside the
                          frame borders. */
        QRect rect[NumAxes]; /**< Areas for scales and data. */
        QRect graphRect; /**< Graph area. */

        /** XY point type. */
        struct Point {
            double value[NumAxes]; /**< Value for each axis. */
            PdCom::Time time; /**< Time of the point. */
        };
        QList<Point> points; /**< Extracted points. */

        void updateRects();
        void extractPoints();
        void removeDeprecated();

        void retranslate();
};

/****************************************************************************/

/**
 * \return The #timeRange.
 */
inline double XYGraph::getTimeRange() const
{
    return timeRange;
}

/****************************************************************************/

/**
 * \return The minimum value of the horizontal scale.
 */
inline double XYGraph::getScaleXMin() const
{
    return xScale.getMin();
}

/****************************************************************************/

/**
 * \return The maximum value of the horizontal scale.
 */
inline double XYGraph::getScaleXMax() const
{
    return xScale.getMax();
}

/****************************************************************************/

/**
 * \return The minimum value of the vertical scale.
 */
inline double XYGraph::getScaleYMin() const
{
    return yScale.getMin();
}

/****************************************************************************/

/**
 * \return The maximum value of the vertical scale.
 */
inline double XYGraph::getScaleYMax() const
{
    return yScale.getMax();
}

/****************************************************************************/

} // namespace

#endif
