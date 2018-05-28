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

#ifndef PD_TIME_H
#define PD_TIME_H

#include <QLabel>

#include "Export.h"
#include "ScalarSubscriber.h"

namespace Pd {

/****************************************************************************/

/** Relative time display.
 */
class QDESIGNER_WIDGET_EXPORT Time:
    public QLabel, public ScalarSubscriber
{
    Q_OBJECT

    public:
        Time(QWidget *parent = 0);
        virtual ~Time();

        void clearData(); // pure-virtual from ScalarSubscriber

        double getValue() const;
        void setValue(double);

        virtual QSize sizeHint() const;

    protected:
        bool event(QEvent *);

    private:
        bool dataPresent; /**< \a true, if data was received. */
        double value; /**< Current time value in seconds. */

        void notify(PdCom::Variable *); // pure-virtual from PdCom::Subscriber

        void outputValue();

        void retranslate();
};

/****************************************************************************/

/**
 * \return The current #value.
 */
inline double Time::getValue() const
{
    return value;
}

/****************************************************************************/

} // namespace

#endif
