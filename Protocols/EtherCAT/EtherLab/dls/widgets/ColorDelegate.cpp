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

#include <QDebug>
#include <QColorDialog>

#include "ColorDelegate.h"

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
