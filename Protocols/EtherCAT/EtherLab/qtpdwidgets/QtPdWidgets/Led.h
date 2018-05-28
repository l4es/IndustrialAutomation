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

#ifndef PD_LED_H
#define PD_LED_H

#include "Export.h"
#include "MultiLed.h"

namespace Pd {

/****************************************************************************/

/** LED widget.
 */
class QDESIGNER_WIDGET_EXPORT Led:
    public MultiLed
{
    Q_OBJECT
    Q_ENUMS(OffColorMode)
    Q_PROPERTY(QColor onColor
            READ getOnColor WRITE setOnColor RESET resetOnColor)
    Q_PROPERTY(QColor offColor
            READ getOffColor WRITE setOffColor RESET resetOffColor)
    Q_PROPERTY(OffColorMode offColorMode
            READ getOffColorMode WRITE setOffColorMode
            RESET resetOffColorMode)
    Q_PROPERTY(bool invert
            READ getInvert WRITE setInvert RESET resetInvert)

    public:
        Led(QWidget *parent = 0);
        virtual ~Led();

        /** Off color mode.
         *
         * This influences the displayed color, when the LED is off.
         */
        enum OffColorMode {
            DarkOnColor, /**< Display a darker shade of #onColor. */
            ExplicitOffColor /**< Use #offColor. */
        };

        QColor getOnColor() const;
        void setOnColor(QColor);
        void resetOnColor();
        QColor getOffColor() const;
        void setOffColor(QColor);
        void resetOffColor();
        OffColorMode getOffColorMode() const;
        void setOffColorMode(OffColorMode);
        void resetOffColorMode();
        bool getInvert() const;
        void setInvert(bool);
        void resetInvert();

    protected:
        bool event(QEvent *);

    private:
        QColor onColor; /**< The color to display, when the LED is on. */
        QColor offColor; /**< The color to display, when the LED is off.
                           This is only used, if #offColorMode
                           is #ExplicitOffColor. */
        OffColorMode offColorMode; /**< Determines, which color to use,
                                     when the LED is off. */
        QColor currentOffColor; /**< Current Off-Color, depending
                                  on #onColor, #offColor and #offColorMode. */
        bool invert; /**< Inversion flag. This inverts the value returned
                       by ledOn(). */

        void updateColor(); // virtual from MultiLed
        void updateCurrentOffColor();

        void retranslate();

        /** MultiLed::setHash() made private. */
        void setHash(const Hash *);
};

/****************************************************************************/

/**
 * \returns The #onColor.
 */
inline QColor Led::getOnColor() const
{
    return onColor;
}

/****************************************************************************/

/**
 * \returns The #offColor.
 */
inline QColor Led::getOffColor() const
{
    return offColor;
}

/****************************************************************************/

/**
 * \returns The #offColorMode.
 */
inline Led::OffColorMode Led::getOffColorMode() const
{
    return offColorMode;
}

/****************************************************************************/

/**
 * \returns The #invert flag.
 */
inline bool Led::getInvert() const
{
    return invert;
}

/****************************************************************************/

} // namespace

#endif
