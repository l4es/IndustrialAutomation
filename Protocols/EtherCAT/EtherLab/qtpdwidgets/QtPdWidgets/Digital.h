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

#ifndef PD_DIGITAL_H
#define PD_DIGITAL_H

#include <QFrame>

#include "ScalarSubscriber.h"
#include "Widget.h"

namespace Pd {

/****************************************************************************/

/** Base functionality for digital displays.
 */
class Q_DECL_EXPORT Digital:
    public QFrame, public ScalarSubscriber, Widget
{
    Q_OBJECT

    Q_ENUMS(TimeDisplay)

    Q_PROPERTY(Qt::Alignment alignment
            READ getAlignment WRITE setAlignment RESET resetAlignment)
    Q_PROPERTY(quint32 decimals
            READ getDecimals WRITE setDecimals RESET resetDecimals)
    Q_PROPERTY(QString suffix
            READ getSuffix WRITE setSuffix RESET resetSuffix)
    Q_PROPERTY(TimeDisplay timeDisplay
            READ getTimeDisplay WRITE setTimeDisplay RESET resetTimeDisplay)

    public:
        Digital(QWidget *parent = 0);
        virtual ~Digital();

        void clearData(); // pure-virtual from ScalarSubscriber

        double getValue() const;

        Qt::Alignment getAlignment() const;
        void setAlignment(Qt::Alignment);
        void resetAlignment();

        quint32 getDecimals() const;
        void setDecimals(quint32);
        void resetDecimals();

        const QString &getSuffix() const;
        void setSuffix(const QString &);
        void resetSuffix();

        enum TimeDisplay {
            None,
            Seconds,
            Minutes,
            Hours
        };
        TimeDisplay getTimeDisplay() const { return timeDisplay; }
        void setTimeDisplay(TimeDisplay);
        void resetTimeDisplay();

        const QString &getValueString() const;

        QSize sizeHint() const;

    protected:
        bool dataPresent; /**< True, if data were received. */
        double value; /**< Current value. */
        bool redraw; /**< Value shall be redrawn on next redraw event. */

        bool event(QEvent *);
        void paintEvent(QPaintEvent *);

        void drawText(QPaintEvent *, QPainter &);

    private:
        Qt::Alignment alignment; /**< Text alignment. */
        quint32 decimals; /**< Number of decimal digits. */
        QString suffix; /**< Suffix, that is appended to the displayed
                          string. The suffix is appended without a separator
                          (like in other Qt classes), so if you want to
                          specify a unit, you'll have to set suffix to
                          " kN", for example. */
        TimeDisplay timeDisplay; /**< Time display. */
        QString displayText; /**< Displayed text. */

        void notify(PdCom::Variable *); // pure-virtual from PdCom::Subscriber

        void outputValue();

        void retranslate();

    private slots:
        void redrawEvent();
};

/****************************************************************************/

/**
 * \return The current #value.
 */
inline double Digital::getValue() const
{
    return value;
}

/****************************************************************************/

/**
 * \return The text #alignment.
 */
inline Qt::Alignment Digital::getAlignment() const
{
    return alignment;
}

/****************************************************************************/

/**
 * \return The number of displayed #decimals.
 */
inline quint32 Digital::getDecimals() const
{
    return decimals;
}

/****************************************************************************/

/**
 * \return The #suffix.
 */
inline const QString &Digital::getSuffix() const
{
    return suffix;
}

/****************************************************************************/

} // namespace

#endif
