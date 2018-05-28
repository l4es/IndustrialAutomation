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

#ifndef PD_DIAL_H
#define PD_DIAL_H

#include "ScalarSubscriber.h"
#include "Widget.h"

#include <QFrame>
#include <QPixmap>
#include <QTimer>
#include <QElapsedTimer>
#include <QSvgRenderer>

class QEvent;
class QWidget;
class QMouseEvent;

namespace Pd {

/****************************************************************************/

class Dial:
    public QFrame, Widget
{
    Q_OBJECT

    Q_PROPERTY(QString title READ getTitle WRITE setTitle RESET resetTitle)
    Q_PROPERTY(QString unit READ getUnit WRITE setUnit RESET resetUnit)
    Q_PROPERTY(double span READ getSpan WRITE setSpan RESET resetSpan)
    Q_PROPERTY(double scaleMin READ getScaleMin WRITE setScaleMin
            RESET resetScaleMin)
    Q_PROPERTY(double majorStep READ getMajorStep WRITE setMajorStep
            RESET resetMajorStep)
    Q_PROPERTY(unsigned int majorStops READ getMajorStops WRITE setMajorStops
            RESET resetMajorStops)
    Q_PROPERTY(unsigned int minorStops READ getMinorStops WRITE setMinorStops
            RESET resetMinorStops)
    Q_PROPERTY(QColor pieColor READ getPieColor WRITE setPieColor
            RESET resetPieColor)

    public:
        Dial(QWidget * = 0);

        const QString &getTitle() const { return title; }
        void setTitle(const QString &);
        void resetTitle();

        const QString &getUnit() const { return unit; }
        void setUnit(const QString &);
        void resetUnit();

        double getSpan() const { return span; }
        void setSpan(double);
        void resetSpan();

        double getScaleMin() const { return scaleMin; }
        void setScaleMin(double);
        void resetScaleMin();

        double getMajorStep() const { return majorStep; }
        void setMajorStep(double);
        void resetMajorStep();

        unsigned int getMajorStops() const { return majorStops; }
        void setMajorStops(unsigned int);
        void resetMajorStops();

        unsigned int getMinorStops() const { return minorStops; }
        void setMinorStops(unsigned int);
        void resetMinorStops();

        const QColor &getPieColor() const { return pieColor; }
        void setPieColor(const QColor &);
        void resetPieColor();

        class CurrentValue:
            public Pd::ScalarSubscriber
        {
            public:
                CurrentValue(Dial *dial):
                    dial(dial),
                    dataPresent(false),
                    value(0.0)
                    {};
                Dial * const dial;
                bool dataPresent;
                double value;

            protected:
                void notify(PdCom::Variable *);
                void variableEvent();
        } currentValue;

        class SetpointValue:
            public Pd::ScalarSubscriber
        {
            public:
                SetpointValue(Dial *dial):
                    dial(dial),
                    dataPresent(false),
                    value(0.0)
                    {};
                Dial * const dial;
                bool dataPresent;
                double value;

            protected:
                void notify(PdCom::Variable *);
                void variableEvent();
        } setpointValue;

        void setNeedle(const QString &);
        void setNeedleCenterX(int);

        void setSetpoint(const QString &);

    signals:
        void setpointChanged(double);

    protected:
        QSize sizeHint() const;
        bool event(QEvent *);
        void paintEvent(QPaintEvent *);

    private:
        QString title;
        QString unit;
        double span;
        double scaleMin;
        double majorStep;
        unsigned int majorStops;
        unsigned int minorStops;
        QColor pieColor;

        int minorStopOffset;
        int minorStopLength;
        int majorStopOffset;
        int majorStopLength;
        int tickLabelOffset;
        int needleLength;
        int setpointOffset;
        int setpointLength;

        QString needlePath;
        QSvgRenderer needleRenderer;
        QRectF needleRect;
        int needleCenterX;

        bool redraw; /**< Value shall be redrawn on next redraw event. */

        QString setpointPath;
        QSvgRenderer setpointRenderer;
        QRectF setpointRect;

        double setpoint;

        QPixmap pixmap;

        bool dragging;
        QPoint setpointPos;

        void refreshPixmap();
        void updateNeedleRect();
        void updateSetpointRect();

        void mouseButtonPressEvent(QMouseEvent*);
        void mouseButtonReleaseEvent(QMouseEvent*);
        void mouseMoveEvent(QMouseEvent*);

    private slots:
        void redrawEvent();
};

/****************************************************************************/

} // namespace

#endif
