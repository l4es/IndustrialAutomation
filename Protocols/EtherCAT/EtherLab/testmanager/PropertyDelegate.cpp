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

#include "PropertyDelegate.h"

#include "PropertyNode.h"
#include "Property.h"
#include "PropertyModel.h"

#include <QComboBox>
#include <QColorDialog>
#include <QDebug>

/****************************************************************************/

PropertyDelegate::PropertyDelegate():
    QStyledItemDelegate()
{
}

/****************************************************************************/

QWidget *PropertyDelegate::createEditor(
        QWidget *parent,
        const QStyleOptionViewItem &option,
        const QModelIndex &index
        ) const
{
    const Property *property = getProperty(index);
    if (!property) {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }

    const QMetaProperty &metaProperty = property->getMetaProperty();

    if (metaProperty.isEnumType() and not metaProperty.isFlagType()) {
        QComboBox *comboBox = new QComboBox(parent);
        const QMetaEnum &metaEnum = metaProperty.enumerator();
        for (int i = 0; i < metaEnum.keyCount(); i++) {
            comboBox->addItem(metaEnum.key(i));
        }
        connect(comboBox, SIGNAL(currentIndexChanged(int)),
                this, SLOT(comboBoxChanged()));
        return comboBox;
    }

    if (metaProperty.type() == QVariant::Color) {
        QColorDialog *colorDialog = new QColorDialog(parent);
        colorDialog->setFocusPolicy(Qt::StrongFocus);
        colorDialog->setOption(QColorDialog::ShowAlphaChannel);
        return colorDialog;
    }

    return QStyledItemDelegate::createEditor(parent, option, index);
}

/****************************************************************************/

void PropertyDelegate::setEditorData(
        QWidget *editor,
        const QModelIndex &index
        ) const
{
    const Property *property = getProperty(index);
    const PropertyModel *propertyModel =
        dynamic_cast<const PropertyModel *>(index.model());
    if (!property or !propertyModel) {
        QStyledItemDelegate::setEditorData(editor, index);
        return;
    }

    const QMetaProperty &metaProperty = property->getMetaProperty();
    if (metaProperty.isEnumType() and not metaProperty.isFlagType()) {
        const QMetaEnum &metaEnum = metaProperty.enumerator();
        QComboBox *comboBox = dynamic_cast<QComboBox *>(editor);
        if (comboBox) {
            int val = property->getValue(propertyModel).toInt();
            QString key(metaEnum.valueToKey(val));
            comboBox->setCurrentText(key);
        }
        return;
    }

    if (metaProperty.type() == QVariant::Color) {
        QColorDialog *colorDialog = dynamic_cast<QColorDialog *>(editor);
        if (colorDialog) {
            colorDialog->setCurrentColor(
                    property->getValue(propertyModel).value<QColor>());
        }
        return;
    }

    QStyledItemDelegate::setEditorData(editor, index);
}

/****************************************************************************/

void PropertyDelegate::setModelData(
        QWidget *editor,
        QAbstractItemModel *model,
        const QModelIndex &index
        ) const
{
    QColorDialog *colorDialog = dynamic_cast<QColorDialog *>(editor);
    if (colorDialog) {
        QColor color = colorDialog->currentColor();
        model->setData(index, color.name(QColor::HexArgb), Qt::EditRole);
        return;
    }

    QStyledItemDelegate::setModelData(editor, model, index);
}

/****************************************************************************/

const Property *PropertyDelegate::getProperty(
        const QModelIndex &index
        ) const
{
    if (index.isValid()) {
        PropertyNode *propertyNode = (PropertyNode *) index.internalPointer();
        return dynamic_cast<Property *>(propertyNode);
    }
    else {
        return NULL;
    }
}

/****************************************************************************/

void PropertyDelegate::comboBoxChanged()
{
    QComboBox *comboBox = dynamic_cast<QComboBox *>(sender());
    if (comboBox) {
        emit commitData(comboBox);
    }
}

/****************************************************************************/
