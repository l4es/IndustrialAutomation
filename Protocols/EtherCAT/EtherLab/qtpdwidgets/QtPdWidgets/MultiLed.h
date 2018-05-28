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

#ifndef PD_MULTILED_H
#define PD_MULTILED_H

#include <QHash>
#include <QFrame>

#include "Export.h"
#include "ScalarSubscriber.h"

namespace Pd {

/****************************************************************************/

/** Multi-Color LED widget.
 *
 * \todo Default color property.
 */
class QDESIGNER_WIDGET_EXPORT MultiLed:
    public QWidget, public ScalarSubscriber
{
    Q_OBJECT
    Q_PROPERTY(quint32 diameter
            READ getDiameter WRITE setDiameter RESET resetDiameter)

    public:
        MultiLed(QWidget *parent = 0);
        virtual ~MultiLed();

        void clearData(); // pure-virtual from ScalarSubscriber

        int getValue() const;
        void setValue(int);
        quint32 getDiameter() const;
        void setDiameter(quint32);
        void resetDiameter();

        /** Value type.
         *
         * A set of attributes, that can be provided for each process value.
         */
        struct Value {
            QColor color; /**< Text color. */
            enum BlinkMode {
                Steady, /**< No blinking. */
                Blink /**< Blink. */
            };
            BlinkMode blink;

            Value(
                    QColor c = Qt::green, /**< Color. */
                    BlinkMode b = Steady /**< Blink mode. */
                    ):
                color(c), blink(b) {}
        };

        /** Value hash type.
         *
         * This hash shall contain a value object for each possible value to
         * display.
         */
        class Q_DECL_EXPORT Hash: public QHash<int, Value> {
            public:
                void insert(
                        int position,
                        QColor c = Qt::green,
                        Value::BlinkMode b = Value::Steady);
        };
        void setHash(const Hash *);

        virtual QSize sizeHint() const;

    protected:
        bool dataPresent; /**< There is a value to display. */

        virtual void updateColor();
        void setCurrentValue(Value);

        static QColor disconnectColor;

        bool event(QEvent *);
        void paintEvent(QPaintEvent *);

    private:
        int value; /**< The current value from the process. */
        quint32 diameter; /**< The LED's diameter in pixel. */
        const Hash *hash; /**< Pointer to the ColorHash
                                        to use. */
        Value currentValue; /**< The currently displayed value attributes. */
        bool blinkState; /**< Blink timer. */
        QColor currentColor; /**< Current color. */

        void setCurrentColor(QColor);
        void notify(PdCom::Variable *); // pure-virtual from PdCom::Subscriber
        void retranslate();

        static QTimer blinkTimer; /**< Blink timer. */

    private slots:
        void blinkEvent();
};

/****************************************************************************/

/**
 * \return The current process #value.
 */
inline int MultiLed::getValue() const
{
    return value;
}

/****************************************************************************/

/**
 * \return The LED #diameter.
 */
inline quint32 MultiLed::getDiameter() const
{
    return diameter;
}

/****************************************************************************/

} // namespace

#endif
