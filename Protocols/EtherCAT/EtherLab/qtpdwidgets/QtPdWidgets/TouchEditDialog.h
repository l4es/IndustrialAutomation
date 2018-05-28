/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2011 - 2012  Andreas Stewering-Bone <ab@igh-essen.com>
 *                            Florian Pose <fp@igh-essen.com>
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

#ifndef TOUCHEDITDIALOG_H
#define TOUCHEDITDIALOG_H

#include <QtGui>
#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>

#include "QtPdWidgets/CursorEditWidget.h"

namespace Pd {

/****************************************************************************/

/** Dialog for editing numeric values on a touchscreen.
 */
class TouchEditDialog:
    public QDialog
{
    Q_OBJECT

    public:
        TouchEditDialog(QWidget *parent = 0);
        ~TouchEditDialog();

        double getValue() const { return editWidget.getValue(); }
        void setValue(double);
        void setDecimals(int);
        void setSuffix(const QString &);
        void setLowerLimit(double);
        void setUpperLimit(double);
        int getEditDigit() const { return editWidget.getEditDigit(); };
        void setEditDigit(int);

    private slots:
        void buttonOk_clicked();
        void buttonCancel_clicked();

    private:
        CursorEditWidget editWidget;
        QVBoxLayout vboxLayout;
        QGridLayout gLayout;
        QPushButton buttonLeft;
        QPushButton buttonRight;
        QPushButton buttonUp;
        QPushButton buttonDown;
        QPushButton buttonOk;
        QPushButton buttonZero;
        QPushButton buttonCancel;
};

/****************************************************************************/

} // namespace Pd

#endif
