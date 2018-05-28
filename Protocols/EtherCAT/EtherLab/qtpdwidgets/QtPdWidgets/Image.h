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

#ifndef PD_IMAGE_H
#define PD_IMAGE_H

#include <QHash>
#include <QLabel>

#include "Export.h"
#include "ScalarSubscriber.h"

namespace Pd {

/****************************************************************************/

/** Image display widget.
 *
 * This widget can display images from a hash, depending on the attached
 * process variable's value.
 *
 * \todo Default image property.
 */
class QDESIGNER_WIDGET_EXPORT Image:
    public QFrame, public ScalarSubscriber
{
    Q_OBJECT
    Q_PROPERTY(QPixmap defaultPixmap READ getDefaultPixmap
            WRITE setDefaultPixmap RESET resetDefaultPixmap)
    Q_PROPERTY(qreal angle READ getAngle WRITE setAngle RESET resetAngle)

    public:
        Image(QWidget *parent = 0);
        virtual ~Image();

        void clearData(); // pure-virtual from ScalarSubscriber

        int getValue() const;
        void setValue(int);

        /** Pixmap hash type.
         *
         * The used pixmap hash shall contain a pixmap for every value to
         * display.
         */
        typedef QHash<int, QPixmap> PixmapHash;
        void setPixmapHash(const PixmapHash *);

        const QPixmap &getDefaultPixmap() const { return defaultPixmap; }
        void setDefaultPixmap(const QPixmap &);
        void resetDefaultPixmap();

        qreal getAngle() const { return angle; }
        void setAngle(qreal);
        void resetAngle();

        void clearTransformations();
        void translate(qreal, qreal);
        enum Axis {X, Y};
        void translate(
                Axis axis, /**< Translation axis. */
                PdCom::Variable *pv, /**< Process variable. */
                double sampleTime = 0.0, /**< Sample time. */
                double scale = 1.0, /**< Scale factor. */
                double offset = 0.0, /**< Offset (applied after scaling). */
                double tau = 0.0 /**< PT1 filter time constant. A value less
                                    or equal to 0.0 means, that no filter is
                                    applied. */
                );
        void rotate(qreal);
        void rotate(
                PdCom::Variable *pv, /**< Process variable. */
                double sampleTime = 0.0, /**< Sample time. */
                double scale = 1.0, /**< Scale factor. */
                double offset = 0.0, /**< Offset (applied after scaling). */
                double tau = 0.0 /**< PT1 filter time constant. A value less
                                    or equal to 0.0 means, that no filter is
                                    applied. */
                );

    protected:
        bool event(QEvent *);
        void paintEvent(QPaintEvent *);

    private:
        int value; /**< The current value from the process. */
        bool dataPresent; /**< There is a data value to display. */
        const PixmapHash *pixmapHash; /**< The PixmapHash to use. */
        QPixmap defaultPixmap; /**< Default pixmap. */
        qreal angle; /**< Designer angle. */
        QPixmap displayPixmap; /**< Current pixmap. */
        class Transformation;
        class FixedTranslation;
        class VariableTranslation;
        class FixedRotation;
        class VariableRotation;
        QList<Transformation *> transformationList; /**< List of
                                                      transformations. */

        void notify(PdCom::Variable *); // pure-virtual from PdCom::Subscriber

        void updatePixmap();
        void retranslate();
};

/****************************************************************************/

/**
 * \return The current process #value.
 */
inline int Image::getValue() const
{
    return value;
}

/****************************************************************************/

} // namespace

#endif
