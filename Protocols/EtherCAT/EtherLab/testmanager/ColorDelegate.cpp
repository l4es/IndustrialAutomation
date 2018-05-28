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

#include "ColorDelegate.h"

#include <QDebug>
#include <QColorDialog>

/****************************************************************************/

ColorDelegate::ColorDelegate(QObject *parent):
    QStyledItemDelegate(parent)
{
}

/****************************************************************************/

QWidget *ColorDelegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem &,
        const QModelIndex &) const
{
    QColorDialog *colorDialog = new QColorDialog(parent);
    colorDialog->setOption(QColorDialog::DontUseNativeDialog);
    colorDialog->setFocusPolicy(Qt::StrongFocus);
    connect(colorDialog, SIGNAL(colorSelected(const QColor &)),
                this, SLOT(editingFinished()));
    return colorDialog;
}

/****************************************************************************/

void ColorDelegate::setEditorData(QWidget *editor,
        const QModelIndex &index) const
{
    QColor color;
    color = QColor::fromRgb(index.model()->data(index, Qt::EditRole).toInt());

    QColorDialog *colorDialog = static_cast<QColorDialog *>(editor);
    colorDialog->setCurrentColor(color);
}

/****************************************************************************/

void ColorDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
        const QModelIndex &index) const
{
    QColorDialog *colorDialog = static_cast<QColorDialog *>(editor);
    QColor color = colorDialog->currentColor();

    model->setData(index, color.rgb(), Qt::EditRole);
}

/****************************************************************************/

void ColorDelegate::updateEditorGeometry(QWidget *,
        const QStyleOptionViewItem &, const QModelIndex &) const
{
}

/****************************************************************************/

void ColorDelegate::editingFinished()
{
    QColorDialog *colorDialog = dynamic_cast<QColorDialog *>(sender());
    emit commitData(colorDialog);
    emit closeEditor(colorDialog);
}

/****************************************************************************/
