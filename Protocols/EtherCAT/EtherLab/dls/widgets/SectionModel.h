/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2009 - 2015  Florian Pose <fp@igh-essen.com>
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

#ifndef DLS_SECTION_MODEL_H
#define DLS_SECTION_MODEL_H

#include <QList>
#include <QString>
#include <QAbstractTableModel>

/*****************************************************************************/

namespace DLS {

class Section;

class SectionModel:
    public QAbstractTableModel
{
    Q_OBJECT

    public:
        SectionModel(Section *);
        ~SectionModel();

        int rowCount(const QModelIndex &) const;
        int columnCount(const QModelIndex &) const;
        QVariant data(const QModelIndex &, int) const;
        QVariant headerData(int, Qt::Orientation, int) const;
        Qt::ItemFlags flags(const QModelIndex &) const;
        bool setData(const QModelIndex &, const QVariant &, int);
        bool removeRows(int, int, const QModelIndex &);

    private:
        Section * const section;

        SectionModel();
};

} // namespace

#endif

/****************************************************************************/
