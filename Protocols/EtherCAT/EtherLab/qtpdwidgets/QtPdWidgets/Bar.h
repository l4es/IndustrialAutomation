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

#ifndef PD_BAR_H
#define PD_BAR_H

#include <QWidget>
#include <QPixmap>

#include "Export.h"

#include "ScalarSubscriber.h"
#include "Widget.h"
#include "Scale.h"

namespace Pd {

/****************************************************************************/

/** Bar graph widget.
 */
class QDESIGNER_WIDGET_EXPORT Bar:
    public QWidget, public Widget
{
    Q_OBJECT
    Q_ENUMS(Orientation Style Origin)
    Q_PROPERTY(Orientation orientation
            READ getOrientation WRITE setOrientation RESET resetOrientation)
    Q_PROPERTY(Style style
            READ getStyle WRITE setStyle RESET resetStyle)
    /** \deprecated */
    Q_PROPERTY(bool showScale
            READ getShowScale WRITE setShowScale RESET resetShowScale)
    /** \see setScaleMin() */
    Q_PROPERTY(double scaleMin
            READ getScaleMin WRITE setScaleMin RESET resetScaleMin)
    /** \see setScaleMax() */
    Q_PROPERTY(double scaleMax
            READ getScaleMax WRITE setScaleMax RESET resetScaleMax)
    Q_PROPERTY(Origin origin
            READ getOrigin WRITE setOrigin RESET resetOrigin)
    Q_PROPERTY(int borderWidth
            READ getBorderWidth WRITE setBorderWidth RESET resetBorderWidth)
    Q_PROPERTY(QColor backgroundColor
            READ getBackgroundColor WRITE setBackgroundColor
            RESET resetBackgroundColor)
    Q_PROPERTY(bool autoBarWidth
            READ getAutoBarWidth WRITE setAutoBarWidth
            RESET resetAutoBarWidth)

    public:
        Bar(QWidget *parent = 0);
        virtual ~Bar();

        /** Orientation of the bar widget. */
        enum Orientation  {
            Vertical, /**< Vertical bar with scale on the left. */
            Horizontal /**< Horizontal bar with scale on top. */
        };
        Orientation getOrientation() const;
        void setOrientation(Orientation);
        void resetOrientation();

        /** Style of the bar widget. */
        enum Style  {
            ColorBar, /**< Display colored bars over a plain background. */
            Arrow, /**< Display arrows over a colored background. */
            MultiColorBar /**< Display solid colored bar acc. to value. */
        };
        Style getStyle() const { return style; };
        void setStyle(Style);
        void resetStyle();

        bool getShowScale() const;
        void setShowScale(bool);
        void resetShowScale();

        double getScaleMin() const;
        void setScaleMin(double);
        void resetScaleMin();

        double getScaleMax() const;
        void setScaleMax(double);
        void resetScaleMax();

        /** Bar origin mode.
         *
         * This determines, from where bars are drawn.
         */
        enum Origin {
            OriginZero, /**< Draw bars originating from zero. */
            OriginMinimum, /**< Draw bars originating from the scale minimum.
                             If #orientation is set to #Vertical, this is the
                             bottom of the bar area, while with #orientation
                             set to #Horizontal, this is the left side of the
                             bar area. */
            OriginMaximum, /**< Draw bars originating from the scale maximum.
                             If #orientation is set to #Vertical, this is the
                             top of the bar area, while with #orientation set
                             to #Horizontal, this is the right side of the bar
                             area. */
        };
        Origin getOrigin() const;
        void setOrigin(Origin);
        void resetOrigin();

        int getBorderWidth() const;
        void setBorderWidth(int);
        void resetBorderWidth();

        QColor getBackgroundColor() const;
        void setBackgroundColor(QColor);
        void resetBackgroundColor();

        bool getAutoBarWidth() const;
        void setAutoBarWidth(bool);
        void resetAutoBarWidth();

        virtual QSize sizeHint() const;

        /** Connect to a process variable using a new bar stack.
         *
         * A new stack (horizontal division for veritcal bar orientation) is
         * created. A stack can display multiple sections on top of each other
         * (speaking for vertical bar orientation). To add a new section to an
         * existing stack, use addStackedVariable().
         *
         * \see ScalarSubscriber::setVariable().
         */
        void addVariable(
                PdCom::Variable *pv, /**< Process variable. */
                double sampleTime = 0.0, /**< Sample time. */
                double scale = 1.0, /**< Scale factor. */
                double offset = 0.0, /**< Offset (applied after scaling). */
                double tau = 0.0, /**< PT1 filter time constant. A value less
                                    or equal to 0.0 means, that no filter is
                                    applied. */
                QColor color = Qt::blue /**< Bar color. */
                );

        /** Connect to a process variable using the last stack.
         *
         * The variable is placed as upper element of the last stack. If
         * no stacks exist, the forst one is created.
         *
         * \see addVariable().
         * \see ScalarSubscriber::setVariable().
         */
        void addStackedVariable(
                PdCom::Variable *pv, /**< Process variable. */
                double sampleTime = 0.0, /**< Sample time. */
                double scale = 1.0, /**< Scale factor. */
                double offset = 0.0, /**< Offset (applied after scaling. */
                double tau = 0.0, /**< PT1 filter time constant. A value less
                                    or equal to 0.0 means, that no filter is
                                    applied. */
                QColor color = Qt::blue /**< Bar color. */
                );

        /** Clear all stacks and sections.
         */
        void clearVariables();

        /** Clear all stacks and sections and add a single variable.
         *
         * \deprecated
         * \see addVariable()
         */
        void setVariable(
                PdCom::Variable *pv, /**< Process variable. */
                double sampleTime = 0.0, /**< Sample time. */
                double scale = 1.0, /**< Scale factor. */
                double offset = 0.0, /**< Offset (applied after scaling). */
                double tau = 0.0, /**< PT1 filter time constant. A value less
                                    or equal to 0.0 means, that no filter is
                                    applied. */
                QColor color = Qt::blue /**< Bar color. */
                );
        void clearData();

        void setGradientStops(const QGradientStops &);
        const QPair<double, double> getGradientLimits() const;

    protected:
        bool event(QEvent *);
        void resizeEvent(QResizeEvent *);
        void paintEvent(QPaintEvent *event);
        void notifyMaxBarWidthChange();
        QList<Bar *> findSiblings();
        void maxBarWidthChanged();

    private:
        Orientation orientation; /**< Orientation of the bar */
        Style style; /**< Widget appearance. */
        bool showScale; /**< Whether or not the scale is shown. */
        Scale valueScale; /**< Value scale. */
        Origin origin; /**< Bar drawing origin. */
        int borderWidth; /**< Width of the border around the bar drawing area.
                          */
        QColor backgroundColor; /**< Background color of the bar area. */
        bool autoBarWidth;
        QGradientStops gradientStops; /**< Color gradient. */
        QLinearGradient gradient;

        double minStop; /**< Value of first gradient stop */
        double maxStop; /**< Value of last gradient stop */

        class Stack;
        typedef QList<Stack *> StackList;
        StackList stacks; /**< Bar stacks. */

        QPixmap backgroundPixmap; /**< Pixmap that stores the background and
                                    scale. */
        QRect scaleRect;
        QRect borderRect;
        QRect barRect; /**< Layout rectangle containing the bar drawing
                         area. */
        QLine zeroLine; /**< Zero line. */
        QPolygon darkPolygon; /**< Polygon to draw the border shadow. */
        QPolygon brightPolygon; /**< Polygon to draw the border light. */

        int maxBarWidth;

        QString debugStr;

        static QPolygon verticalDragIndicatorPolygon;
        static QPolygon horizontalDragIndicatorPolygon;
        static QPolygon verticalArrow;
        static QPolygon horizontalArrow;
        static void initDragIndicatorPolygons();

        void updateLayout();
        void updateBackground();
        unsigned int calcPosition(double, bool = true) const;

        void retranslate();

        void updateGradient();

    private slots:
        void redrawEvent();
};

/****************************************************************************/

/**
 * \return The #orientation.
 */
inline Bar::Orientation Bar::getOrientation() const
{
    return orientation;
}

/****************************************************************************/

/**
 * \return \a true, if the scale is shown.
 */
inline bool Bar::getShowScale() const
{
    return showScale;
}

/****************************************************************************/

/**
 * \return The value scale's minimum value.
 */
inline double Bar::getScaleMin() const
{
    return valueScale.getMin();
}

/****************************************************************************/

/**
 * \return The value scale's maximum value.
 */
inline double Bar::getScaleMax() const
{
    return valueScale.getMax();
}

/****************************************************************************/

/**
 * \return The bar drawing #origin.
 */
inline Bar::Origin Bar::getOrigin() const
{
    return origin;
}

/****************************************************************************/

/**
 * \return The #borderWidth around the bar rect.
 */
inline int Bar::getBorderWidth() const
{
    return borderWidth;
}

/****************************************************************************/

/**
 * \return The #backgroundColor of the widget.
 */
inline QColor Bar::getBackgroundColor() const
{
    return backgroundColor;
}

/****************************************************************************/

/**
 * \return The #autoBarWidth of the widget.
 */
inline bool Bar::getAutoBarWidth() const
{
    return autoBarWidth;
}

/****************************************************************************/

} // namespace Pd

#endif
