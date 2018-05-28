/*****************************************************************************
 *
 * Testmanager - Graphical Automation and Visualisation Tool
 *
 * Copyright (C) 2018  Florian Pose <fp@igh.de>
 *
 * This file is part of Testmanager.
 *
 * Testmanager is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Testmanager is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with Testmanager. If not, see <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

#include "ui_StyleDialog.h"

#include <QDialog>

/****************************************************************************/

class StyleDialog:
    public QDialog,
    public Ui::StyleDialog
{
    Q_OBJECT

    public:
        StyleDialog(QWidget *, QWidget * = 0);
        ~StyleDialog();

    private:
        QWidget * const editWidget;
        QAction addResourceAction;
        QAction addColorAction;
        QAction addFontAction;

        void insertCssProperty(const QString &, const QString &);

        static bool isStyleSheetValid(const QString &);

    private slots:
        void validateStyleSheet();
        void on_buttonBox_accepted();
        void slotAddResource(const QString &);
        void slotAddColor(const QString &);
        void slotAddFont();
};

/****************************************************************************/
