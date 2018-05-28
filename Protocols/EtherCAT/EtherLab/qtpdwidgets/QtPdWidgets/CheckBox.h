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

#ifndef PD_CHECKBOX_H
#define PD_CHECKBOX_H

#include <QCheckBox>

#include "Export.h"
#include "ScalarSubscriber.h"

namespace Pd {

/****************************************************************************/

/** CheckBox.
 */
class QDESIGNER_WIDGET_EXPORT CheckBox:
    public QCheckBox, public ScalarSubscriber
{
    Q_OBJECT
    Q_PROPERTY(int onValue
            READ getOnValue WRITE setOnValue RESET resetOnValue)
    Q_PROPERTY(int offValue
            READ getOffValue WRITE setOffValue RESET resetOffValue)

    public:
        CheckBox(QWidget *parent = 0);
        virtual ~CheckBox();

        void clearData(); // pure-virtual from ScalarSubscriber

        int getOnValue() const;
        void setOnValue(int);
        void resetOnValue();
        int getOffValue() const;
        void setOffValue(int);
        void resetOffValue();

    protected:
        void checkStateSet();
        void nextCheckState();

    private:
        int value; /**< the current variable value */
        bool dataPresent; /**< A value is to be displayed. */
        int onValue; /**< The value that is sent to the process, if the
                       checkbox is checked. */
        int offValue; /**< The value that is sent to the process, if the
                       checkbox is unchecked. */

        void setYellow(bool);

        void notify(PdCom::Variable *); // pure-virtual from PdCom::Subscriber
        void updateCheck();
};

/****************************************************************************/

/** \returns The #onValue.
 */
inline int CheckBox::getOnValue() const
{
    return onValue;
}

/****************************************************************************/

/** \returns The #offValue.
 */
inline int CheckBox::getOffValue() const
{
    return offValue;
}

/****************************************************************************/

} // namespace

#endif
