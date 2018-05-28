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

#ifndef PD_RADIOBUTTON_H
#define PD_RADIOBUTTON_H

#include <QRadioButton>

#include "Export.h"
#include "ScalarSubscriber.h"

namespace Pd {

/****************************************************************************/

/** RadioButton.
 */
class QDESIGNER_WIDGET_EXPORT RadioButton:
    public QRadioButton, public ScalarSubscriber
{
    Q_OBJECT
    Q_PROPERTY(int checkValue
            READ getCheckValue WRITE setCheckValue RESET resetCheckValue)

    public:
        RadioButton(QWidget *parent = 0);
        virtual ~RadioButton();

        void clearData(); // pure-virtual from ScalarSubscriber

        int getCheckValue() const;
        void setCheckValue(int);
        void resetCheckValue();

    protected:
        void nextCheckState();

    private:
        bool dataPresent; /**< A value shall be displayed. */
        int value; /**< the current variable value */
        int checkValue; /**< The value that is sent to the process, if the
                       button is checked. */

        void notify(PdCom::Variable *); // pure-virtual from PdCom::Subscriber
        void updateCheck();
};

/****************************************************************************/

/** \returns The #checkValue.
 */
inline int RadioButton::getCheckValue() const
{
    return checkValue;
}

/****************************************************************************/

} // namespace

#endif
