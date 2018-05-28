/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2012  Florian Pose <fp@igh-essen.com>
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

#include <cmath>

#include <QMenu>
#include <QDebug>

#include "SectionDialog.h"
#include "SectionModel.h"

using DLS::SectionDialog;

/****************************************************************************/

/** Constructor.
 */
SectionDialog::SectionDialog(
        Section *section,
        QWidget *parent
        ):
    QDialog(parent),
    section(section),
    origSection(*section),
    workSection(*section),
    model(new SectionModel(&workSection)),
    colorDelegate(this)
{
    setupUi(this);

    radioButtonAuto->setChecked(section->getAutoScale());
    radioButtonManual->setChecked(!section->getAutoScale());

    lineEditMinimum->setText(QLocale().toString(section->getScaleMinimum()));
    lineEditMaximum->setText(QLocale().toString(section->getScaleMaximum()));

    checkBoxShowScale->setChecked(section->getShowScale());
    doubleSpinBoxRelHeight->setValue(
            section->getRelativePrintHeight() * 100.0);

    connect(model,
            SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
            this, SLOT(modelDataChanged()));

    tableViewLayers->setItemDelegateForColumn(3, &colorDelegate);
    tableViewLayers->setModel(model);
    tableViewLayers->verticalHeader()->hide();
    QHeaderView *header = tableViewLayers->horizontalHeader();
#if QT_VERSION >= 0x050000
    header->setSectionResizeMode(0, QHeaderView::Stretch);
    header->setSectionResizeMode(1, QHeaderView::Stretch);
    header->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(6, QHeaderView::ResizeToContents);
#else
    header->setResizeMode(0, QHeaderView::Stretch);
    header->setResizeMode(1, QHeaderView::Stretch);
    header->setResizeMode(2, QHeaderView::ResizeToContents);
    header->setResizeMode(3, QHeaderView::ResizeToContents);
    header->setResizeMode(4, QHeaderView::ResizeToContents);
    header->setResizeMode(5, QHeaderView::ResizeToContents);
    header->setResizeMode(6, QHeaderView::ResizeToContents);
#endif
    tableViewLayers->resizeColumnsToContents();

    connect(radioButtonAuto, SIGNAL(toggled(bool)),
            this, SLOT(scaleValueChanged()));
    connect(radioButtonManual, SIGNAL(toggled(bool)),
            this, SLOT(scaleValueChanged()));
    connect(lineEditMinimum, SIGNAL(textChanged(const QString &)),
            this, SLOT(scaleValueChanged()));
    connect(lineEditMaximum, SIGNAL(textChanged(const QString &)),
            this, SLOT(scaleValueChanged()));
    connect(lineEditMinimum, SIGNAL(textEdited(const QString &)),
            this, SLOT(manualScaleEdited()));
    connect(lineEditMaximum, SIGNAL(textEdited(const QString &)),
            this, SLOT(manualScaleEdited()));
    connect(checkBoxShowScale, SIGNAL(toggled(bool)),
            this, SLOT(scaleValueChanged()));
    connect(tableViewLayers,
            SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(tableContextMenu(const QPoint &)));
}

/****************************************************************************/

/** Destructor.
 */
SectionDialog::~SectionDialog()
{
    delete model;
}

/****************************************************************************/

void SectionDialog::accept()
{
    bool ok;
    double min, max;

    min = QLocale().toDouble(lineEditMinimum->text(), &ok);
    if (!ok) {
        return;
    }

    max = QLocale().toDouble(lineEditMaximum->text(), &ok);
    if (!ok) {
        return;
    }

    workSection.setScaleMinimum(min);
    workSection.setScaleMaximum(max);
    workSection.setAutoScale(radioButtonAuto->isChecked());
    workSection.setShowScale(checkBoxShowScale->isChecked());

    double relHeight = doubleSpinBoxRelHeight->value() / 100.0;
    if (relHeight < 0.0) {
        relHeight = -1.0;
    }
    workSection.setRelativePrintHeight(relHeight);

    *section = workSection;

    done(Accepted);
}

/****************************************************************************/

void SectionDialog::reject()
{
    *section = origSection;
    done(Rejected);
}

/****************************************************************************/

void SectionDialog::scaleValueChanged()
{
    bool ok;
    double min, max;

    min = QLocale().toDouble(lineEditMinimum->text(), &ok);
    if (ok) {
        workSection.setScaleMinimum(min);
    }

    max = QLocale().toDouble(lineEditMaximum->text(), &ok);
    if (ok) {
        workSection.setScaleMaximum(max);
    }

    workSection.setAutoScale(radioButtonAuto->isChecked());
    workSection.setShowScale(checkBoxShowScale->isChecked());

    if (checkBoxPreview->isChecked()) {
        *section = workSection;
    }
}

/****************************************************************************/

void SectionDialog::on_checkBoxPreview_toggled()
{
    if (checkBoxPreview->isChecked()) {
        *section = workSection;
    }
    else {
        *section = origSection;
    }
}

/****************************************************************************/

void SectionDialog::modelDataChanged()
{
    if (checkBoxPreview->isChecked()) {
        *section = workSection;
    }
}

/****************************************************************************/

void SectionDialog::on_pushButtonGuess_clicked()
{
    double min, max, norm;

    if (!workSection.getExtrema(min, max) || max <= min) {
        return;
    }

    double absMin, absMax, diff = max - min;
    if (min < 0) {
        absMin = -min;
    }
    else {
        absMin = min;
    }
    if (max < 0) {
        absMax = -max;
    }
    else {
        absMax = max;
    }

    double decade = floor(log10(diff));

    norm = absMin / pow(10.0, decade); // 1 <= norm < 10
    if (min < 0) {
        norm *= -1.0;
    }
    double myMin = floor(norm) * pow(10.0, decade);

    norm = absMax / pow(10.0, decade); // 1 <= norm < 10
    if (max < 0) {
        norm *= -1.0;
    }
    double myMax = ceil(norm) * pow(10.0, decade);

    lineEditMinimum->setText(QLocale().toString(myMin));
    lineEditMaximum->setText(QLocale().toString(myMax));
    radioButtonManual->setChecked(true);
}

/****************************************************************************/

void SectionDialog::manualScaleEdited()
{
    radioButtonManual->setChecked(true);
}

/****************************************************************************/

void SectionDialog::tableContextMenu(const QPoint &pos)
{
    QModelIndexList indexes =
        tableViewLayers->selectionModel()->selectedRows();

    QMenu menu(this);

    QAction removeAction(this);
    removeAction.setText(tr("Remove %n layer(s)", "", indexes.size()));
    removeAction.setIcon(QIcon(":/DlsWidgets/images/list-remove.svg"));
    removeAction.setEnabled(indexes.size() > 0);
    connect(&removeAction, SIGNAL(triggered()), this, SLOT(removeLayers()));
    menu.addAction(&removeAction);

    menu.exec(tableViewLayers->mapToGlobal(pos));
}

/****************************************************************************/

void SectionDialog::removeLayers()
{
    QModelIndexList indexes =
        tableViewLayers->selectionModel()->selectedRows();
    QList<int> rowList;

    foreach (QModelIndex index, indexes) {
        rowList << index.row();
    }

    qSort(rowList);

    for (int i = 0; i < rowList.size(); i++) {
        model->removeRow(rowList[i] - i);
    }
}

/****************************************************************************/
