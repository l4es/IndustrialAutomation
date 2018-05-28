/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2009 - 2012  Florian Pose <fp@igh-essen.com>
 *
 * This file is part of the DLS widget library.
 *
 * The DLS widget library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * The DLS widget library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the DLS widget library. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

#ifndef DLS_SECTION_DIALOG_H
#define DLS_SECTION_DIALOG_H

#include <QDialog>

#include "ui_SectionDialog.h"

#include "ColorDelegate.h"
#include "DlsWidgets/Section.h"

namespace DLS {

class SectionModel;

/****************************************************************************/

/** Graph section configuration dialog.
 */
class SectionDialog:
    public QDialog,
    public Ui::SectionDialog
{
    Q_OBJECT

    public:
        SectionDialog(Section *, QWidget * = NULL);
        ~SectionDialog();

    private:
        Section * const section;
        const Section origSection;
        Section workSection;
        SectionModel *model;
        ColorDelegate colorDelegate;

        SectionDialog();

    private slots:
        void accept();
        void reject();
        void scaleValueChanged();
        void on_checkBoxPreview_toggled();
        void modelDataChanged();
        void on_pushButtonGuess_clicked();
        void manualScaleEdited();
        void tableContextMenu(const QPoint &);
        void removeLayers();
};

/****************************************************************************/

} // namespace

#endif
