/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2011 - 2012  Andreas Stewering-Bone <ab@igh-essen.com>
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

#ifndef PD_TOUCHEDIT_H
#define PD_TOUCHEDIT_H

#include <QWidget>

#include "Export.h"
#include "Digital.h"

namespace Pd {

class TouchEditDialog;

/****************************************************************************/

/** Digital display and touch edit.
 */
class QDESIGNER_WIDGET_EXPORT TouchEdit:
    public Digital
{
    Q_OBJECT

    Q_PROPERTY(double lowerLimit
            READ getLowerLimit WRITE setLowerLimit
            RESET resetLowerLimit)
    Q_PROPERTY(double upperLimit
            READ getUpperLimit WRITE setUpperLimit
            RESET resetUpperLimit)
    Q_PROPERTY(bool editing READ getEditing);

    public:
        TouchEdit(QWidget *parent = 0);
        virtual ~TouchEdit();

        double getLowerLimit() const;
        void setLowerLimit(double);
        void resetLowerLimit();
        double getUpperLimit() const;
        void setUpperLimit(double);
        void resetUpperLimit();
        bool getEditing() const { return editing; }

    protected:
        bool event(QEvent *);

    private:
        double lowerLimit;
        double upperLimit;
        void openDialog();

        virtual void variableEvent(); // from Pd::ScalarSubscriber
        void changeEvent(QEvent *); // from QWidget

        void retranslate();

        int editDigit;
        bool editing;
        TouchEditDialog *editDialog;
};

/****************************************************************************/

/**
 * \return The lowerLimit.
 */
inline double TouchEdit::getLowerLimit() const
{
    return lowerLimit;
}

/****************************************************************************/

/**
 * \return The upperLimit.
 */
inline double TouchEdit::getUpperLimit() const
{
    return upperLimit;
}

/****************************************************************************/

} // namespace

#endif
