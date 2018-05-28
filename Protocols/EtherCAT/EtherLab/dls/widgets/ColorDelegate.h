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

#ifndef DLS_COLOR_DELEGATE_H
#define DLS_COLOR_DELEGATE_H

#include <QStyledItemDelegate>

/****************************************************************************/

class ColorDelegate:
    public QStyledItemDelegate
{
    Q_OBJECT

    public:
        ColorDelegate(QObject *);

        QWidget *createEditor(QWidget *,
                const QStyleOptionViewItem &,
                const QModelIndex &) const;

        void setEditorData(QWidget *, const QModelIndex &) const;
        void setModelData(QWidget *, QAbstractItemModel *,
                const QModelIndex &) const;

        void updateEditorGeometry(QWidget *,
                const QStyleOptionViewItem &, const QModelIndex &) const;

    private:
        ColorDelegate();

    private slots:
        void editingFinished();
};

/****************************************************************************/

#endif
