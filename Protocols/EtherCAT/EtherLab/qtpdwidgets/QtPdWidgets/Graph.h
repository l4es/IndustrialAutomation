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

#ifndef PD_GRAPH_H
#define PD_GRAPH_H

#include <QFrame>
#include <QIcon>
#include <QAction>

#include "Export.h"
#include "ScalarSubscriber.h"
#include "Widget.h"
#include "ValueRing.h"
#include "TimeScale.h"
#include "Scale.h"

namespace Pd {

/****************************************************************************/

/** Graph widget.
 */
class QDESIGNER_WIDGET_EXPORT Graph:
    public QFrame, public Widget
{
    Q_OBJECT
    Q_ENUMS(Mode TriggerLevelMode)
    Q_PROPERTY(Mode mode
            READ getMode WRITE setMode RESET resetMode)
    /** The time range to display. */
    Q_PROPERTY(double timeRange
            READ getTimeRange WRITE setTimeRange RESET resetTimeRange)
    /** The minimum value on the value scale. */
    Q_PROPERTY(double scaleMin
            READ getScaleMin WRITE setScaleMin RESET resetScaleMin)
    /** The maximum value on the value scale. */
    Q_PROPERTY(double scaleMax
            READ getScaleMax WRITE setScaleMax RESET resetScaleMax)
    Q_PROPERTY(TriggerLevelMode triggerLevelMode
            READ getTriggerLevelMode WRITE setTriggerLevelMode
            RESET resetTriggerLevelMode)
    Q_PROPERTY(double manualTriggerLevel
            READ getManualTriggerLevel WRITE setManualTriggerLevel
            RESET resetManualTriggerLevel)
    Q_PROPERTY(double triggerPosition
            READ getTriggerPosition WRITE setTriggerPosition
            RESET resetTriggerPosition)
    Q_PROPERTY(double triggerTimeout
            READ getTriggerTimeout WRITE setTriggerTimeout
            RESET resetTriggerTimeout)
    Q_PROPERTY(QString suffix
            READ getSuffix WRITE setSuffix RESET resetSuffix)
    Q_PROPERTY(QColor gridColor
            READ getGridColor WRITE setGridColor RESET resetGridColor)
    Q_PROPERTY(bool autoScaleWidth
            READ getAutoScaleWidth WRITE setAutoScaleWidth
            RESET resetAutoScaleWidth)

    public:
        Graph(QWidget *parent = 0);
        virtual ~Graph();

        /** Subscribe to a process variable.
         */
        void setVariable(
                PdCom::Variable *pv, /**< Process variable. */
                double sampleTime = 0.0, /**< Sample time. */
                double scale = 1.0, /**< Scale factor. */
                double offset = 0.0, /**< Offset (applied after scaling). */
                double tau = 0.0, /**< PT1 filter time constant. A value less
                                    or equal to 0.0 means, that no filter is
                                    applied. */
                QColor color = Qt::blue /**< Graph color. */
                );
        void addVariable(
                PdCom::Variable *pv, /**< Process variable. */
                double sampleTime = 0.0, /**< Sample time. */
                double scale = 1.0, /**< Scale factor. */
                double offset = 0.0, /**< Offset (applied after scaling). */
                double tau = 0.0, /**< PT1 filter time constant. A value less
                                    or equal to 0.0 means, that no filter is
                                    applied. */
                QColor color = Qt::blue /**< Graph color. */
                );
        void clearVariables();
        void clearData();

        /** Subscribe to a process variable as trigger variable.
         */
        void setTriggerVariable(
                PdCom::Variable *pv, /**< Process variable. */
                double sampleTime = 0.0, /**< Sample time. */
                double scale = 1.0, /**< Scale factor. */
                double offset = 0.0, /**< Offset (applied after scaling). */
                double tau = 0.0 /**< PT1 filter time constant. A value less
                                   or equal to 0.0 means, that no filter is
                                   applied. */
                );
        void clearTriggerVariable();

        /** Graph mode.
         *
         * The general behaviour of the graph.
         */
        enum Mode {
            Roll, /**< Roll mode. The right border of the widget always
                    represents the current time, so the displayed data values
                    'roll' over the widget from right to left. */
            Trigger /**< Trigger mode. The data are recorded in a circular
                      buffer and analyzed continuously. If the trigger
                      condition is fulfilled, the recording is continued,
                      until there are enough data in the buffer to display a
                      complete snapshot. */
        };
        Mode getMode() const;
        void setMode(Mode);
        void resetMode();
        double getTimeRange() const;
        void setTimeRange(double);
        void resetTimeRange();
        double getScaleMin() const;
        void setScaleMin(double);
        void resetScaleMin();
        double getScaleMax() const;
        void setScaleMax(double);
        void resetScaleMax();
        /** Trigger level mode.
         *
         * Determines, how the trigger level is generated.
         */
        enum TriggerLevelMode {
            AutoLevel, /**< Automatic trigger level calculation. */
            ManualLevel /**< Use the #manualTriggerLevel property. */
        };
        TriggerLevelMode getTriggerLevelMode() const;
        void setTriggerLevelMode(TriggerLevelMode);
        void resetTriggerLevelMode();
        double getManualTriggerLevel() const;
        void setManualTriggerLevel(double);
        void resetManualTriggerLevel();
        double getTriggerPosition() const;
        void setTriggerPosition(double);
        void resetTriggerPosition();
        double getTriggerTimeout() const { return triggerTimeout; }
        void setTriggerTimeout(double);
        void resetTriggerTimeout();
        const QString &getSuffix() const;
        void setSuffix(const QString &);
        void resetSuffix();
        QColor getGridColor() const { return gridColor; }
        void setGridColor(const QColor &);
        void resetGridColor();
        bool getAutoScaleWidth() const { return autoScaleWidth; }
        void setAutoScaleWidth(bool);
        void resetAutoScaleWidth();

        Mode getEffectiveMode() const { return effectiveMode; }

        virtual QSize sizeHint() const;

        void setRedraw();

        /** Graph state.
         */
        enum State {
            Run, /**< Incoming data shall be displayed immediately. */
            Stop, /**< Incoming data shall be processed in background, but the
                    display shall not be changed. */
        };
        bool getState() const { return state; }
        void setState(State);
        void toggleState();

    protected:
        bool event(QEvent *);
        void resizeEvent(QResizeEvent *);
        void paintEvent(QPaintEvent *);
        void contextMenuEvent(QContextMenuEvent *);

    private:
        Mode mode; /**< Current #Mode. */
        Mode effectiveMode; /**< If #mode is Trigger, effective mode can be
                              Roll anyway (if #triggerTimeout is set). */
        double timeRange; /**< Time range. */
        TimeScale timeScale; /**< Time scale. */
        Scale valueScale; /**< Value scale. */
        TriggerLevelMode triggerLevelMode; /**< The current #TriggerLevelMode.
                                            */
        double manualTriggerLevel; /**< Manual trigger level. This is used, if
                                    * #triggerLevelMode is #ManualLevel. */
        double triggerPosition; /**< Horizontal trigger position. In #Trigger
                                  mode, the data that caused the trigger event
                                  will be displayed on the horizontal position
                                  determined by this value. It can range from
                                  0 (\a left) to 1 (\a right). */
        double triggerTimeout; /**< If no trigger condition has been detected
                                 within this time, the graph will display data
                                 like in Roll mode. */
        QColor gridColor; /**< Color of the scale grids. */
        bool autoScaleWidth; /**< Align time scale among sibling graphs. */
        State state; /**< Current graph state. */
        QPixmap stopPixmap; /**< Icon diplayed if state is Stop. */
        QAction runAction; /**< Action for setting state to Run. */
        QAction stopAction; /**< Action for setting state to Stop. */
        int scaleWidth; /**< Width of the value scale. */

        class Layer:
            public ScalarSubscriber
        {
            public:
                Layer(Graph *, QColor, State, double);

                void clearData();
                const ValueRing<double> &getValues() const;
                void setState(State);
                void setTimeRange(double);
                void paint(QPainter &, double, const Scale &, const QRect &);
                void resizeExtrema(unsigned int);
                void prepareSample(PdCom::Time);
                bool hasSampled() const {
                    return (double) timeToSample == 0.0;
                }

            private:
                Graph * const graph; /**< Parent graph. */
                QColor color; /**< Graph color. */
                State state; /**< Layer state. */
                double value; /**< Current value. */
                bool dataPresent; /**< \a value contains a valid value. */
                ValueRing<double> values; /**< Ring buffer with time/value
                                            pairs. */
                ValueRing<double> savedValues; /**< Buffer for saved values.
                                                */
                PdCom::Time timeToSample;
                typedef QPair<double, double> Extrema; /**< Extrema type with
                                                         minimum and maximum.
                                                        */
                QVector<Extrema> extrema; /**< Vector containing the extrema
                                            to display. */
                int offset; /**< Current offset in the #extrema buffer. */
                PdCom::Time offsetTime; /**< Absolute time of the #offset. */
                unsigned int validExtrema; /**< Number of valid extrema. */
                double lastAppendedValue; /**< The last value appended by
                                            appendToExtrema(). */

                void notify(PdCom::Variable *); /* pure-virtual from
                                                   PdCom::Subscriber */
                void fillExtrema();
                bool appendToExtrema(const PdCom::Time &, double);
        };

        QList<Layer *> layers; /**< List of data layers. */

        class TriggerDetector:
            public ScalarSubscriber
        {
            public:
                TriggerDetector(Graph *);

                void setTimeRange(double);
                void reset();
                void setLevel(double);
                double getLevel() const { return level; };
                void updateLevel();

                /** Internal trigger state.
                 */
                enum State {
                    Armed, /**< Trigger armed. The received data are are
                             analyzed for the trigger condition. */
                    Fired, /**< Trigger fired. The trigger condition was
                             detected. The trigger remains in this state,
                             until the extrema buffer is filled and the data
                             can be displayed. */
                };
                State getState() const { return state; }

            private:
                Graph * const graph; /**< Parent graph. */
                ValueRing<double> values; /**< Ring buffer with time/value. */
                bool dataPresent; /**< \a value contains a valid value. */
                State state; /**< The current #State. */
                double level; /**< Effective trigger level. If the data cross
                                the trigger level from negative to positive,
                                the trigger condition is fulfilled. */
                PdCom::Time lastEvent; /**< Time of last trigger event. */

                void notify(PdCom::Variable *); /* pure-virtual from
                                                   PdCom::Subscriber */
                void variableEvent(); /* virtual from ScalarSubscriber */
        };

        TriggerDetector trigger;

        QPixmap backgroundPixmap; /**< Pixmap that stores the background. */
        QColor foregroundColor; /**< Foreground color. */
        QFont foregroundFont; /**< Foreground font. */
        QRect graphRect; /**< Area, where the data are displayed. */

        bool redraw; /**< \a true, if the widget shall be redrawn on next
                       redrawEvent(). */

        void triggerConditionDetected(PdCom::Time);
        void triggerIdle();
        void notifySampled();

        void updateBackground();
        void updateTimeScale();
        void retranslate();
        void notifyScaleWidthChange();
        QList<Graph *> findSiblings() const;

    private slots:
        void redrawEvent();
        void run();
        void stop();
};

/****************************************************************************/

/**
 * \return The #mode.
 */
inline Graph::Mode Graph::getMode() const
{
    return mode;
}

/****************************************************************************/

/**
 * \return The #timeRange.
 */
inline double Graph::getTimeRange() const
{
    return timeRange;
}

/****************************************************************************/

/**
 * \return The value scale minimum.
 */
inline double Graph::getScaleMin() const
{
    return valueScale.getMin();
}

/****************************************************************************/

/**
 * \return The value scale maximum.
 */
inline double Graph::getScaleMax() const
{
    return valueScale.getMax();
}

/****************************************************************************/

/**
 * \return The #triggerLevelMode.
 */
inline Graph::TriggerLevelMode Graph::getTriggerLevelMode() const
{
    return triggerLevelMode;
}

/****************************************************************************/

/**
 * \return The #manualTriggerLevel.
 */
inline double Graph::getManualTriggerLevel() const
{
    return manualTriggerLevel;
}

/****************************************************************************/

/**
 * \return The #triggerPosition.
 */
inline double Graph::getTriggerPosition() const
{
    return triggerPosition;
}

/****************************************************************************/

/**
 * \return The #suffix.
 */
inline const QString &Graph::getSuffix() const
{
    return valueScale.getSuffix();
}

/****************************************************************************/

} // namespace

#endif
