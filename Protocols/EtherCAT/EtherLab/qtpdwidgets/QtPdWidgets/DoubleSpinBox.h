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

#ifndef PD_DOUBLESPINBOX_H
#define PD_DOUBLESPINBOX_H

#include <QDoubleSpinBox>

#include "Export.h"
#include "ScalarSubscriber.h"

namespace Pd {

/****************************************************************************/

/** Spinbox containing a double value.
 */
class QDESIGNER_WIDGET_EXPORT DoubleSpinBox:
    public QDoubleSpinBox, public ScalarSubscriber
{
    Q_OBJECT

    public:
        DoubleSpinBox(QWidget *parent = 0);
        virtual ~DoubleSpinBox();

        void clearData(); // pure-virtual from ScalarSubscriber

    private slots:
        void on_textChanged(const QString &);
        void on_editingFinished();

    private:
        bool editing; /**< True, if the control is in editing state (the
                        background is yellow, then). */
        double internalValue; /**< Internal process value, that stores the
                                last process value, even if the user is
                                editing. */
        void keyPressEvent(QKeyEvent *);
        void stepBy(int); // virtual from QAbstractSpinBox

        void setEditing(bool);
        void setInternalValue(double);
        void updateFromInternal();

        void notify(PdCom::Variable *); // pure-virtual from PdCom::Subscriber
};

/****************************************************************************/

} // namespace

#endif
