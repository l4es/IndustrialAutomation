/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2012-2013  Florian Pose <fp@igh-essen.com>
 *                    2013  Dr. Wilhelm Hagemeister <hm@igh-essen.com>
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

#ifndef PD_TABLEMODEL_H
#define PD_TABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QColor>

#include "QtPdWidgets/TableColumn.h"
#include "QtPdWidgets/ScalarVariable.h"

namespace Pd {

/****************************************************************************/

/** Table model.
 *
 * \see TableColumn.
 */
class Q_DECL_EXPORT TableModel:
    public QAbstractTableModel
{
    Q_OBJECT

    public:
        TableModel();
        ~TableModel();

        void addColumn(TableColumn *);
        void clearColumns();

        bool isEditing() const;
        unsigned int getRowCapacity() const;
        bool hasVisibleRowsVariable() const;
        virtual int rowCount(const QModelIndex &) const;
        virtual int columnCount(const QModelIndex &) const;
        virtual QVariant data(const QModelIndex &, int) const;
        virtual QVariant headerData(int, Qt::Orientation, int) const;
        virtual Qt::ItemFlags flags(const QModelIndex &) const;
        virtual bool setData(const QModelIndex &, const QVariant &, int);

        /** Subscribe to a process variable for highlighting a row.
          The variable value determines the highlighted row.
         */
        void setHighlightRowVariable(
                PdCom::Variable *pv, /**< Process variable. */
                double sampleTime = 0.0 /**< Sample time. */
                );

        void clearHighlightRowVariable();

        /** Subscribe to a process variable for setting the visible rows
            of the Table, this is most often a parameter which also gets
            updated if the Table row count gets changed by editing.
         */
        void setVisibleRowsVariable(
                PdCom::Variable *pv /**< Process variable. */
                );

        void clearVisibleRowsVariable();

        void setHighlightColor(QColor, int = -1);
        void setDisabledColor(QColor, int = -1);

        /** Exception type.
         */
        struct Exception {
            /** Constructor.
             */
            Exception(const QString &msg): msg(msg) {}
            QString msg; /**< Exception message. */
        };

    signals:
        void editingChanged(bool);

    public slots:
        void commit();
        void revert(); // virtual from AbstractItemModel
        void addRow();
        void remRow(); // not to be mixed up with the removeRow()
                       // function from AbstractItemModel

    private:
        unsigned int rows;
        unsigned int visibleRows;
        unsigned int rowCapacity;
        typedef QVector<TableColumn *> ColumnVector; /**< Column vector type.
                                                      */
        ColumnVector columnVector; /**< Vector of table columns. */

        void updateRows();
        Pd::IntVariable valueHighlightRow;
        Pd::IntVariable visibleRowCount;

    private slots:
        void dimensionChanged();
        void columnHeaderChanged();
        void valueChanged();
        void highlightRowChanged();
        void visibleRowCountChanged();
};

/****************************************************************************/

} // namespace

#endif
