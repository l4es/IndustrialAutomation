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

#include "QtPdWidgets/TableModel.h"

#include <QDebug>

using Pd::TableModel;

/****************************************************************************/

/** Constructor.
 */
TableModel::TableModel():
    rows(0U),
    visibleRows(UINT_MAX),
    rowCapacity(0U)
{
    connect(&valueHighlightRow, SIGNAL(valueChanged()),
            this, SLOT(highlightRowChanged()));
    connect(&visibleRowCount, SIGNAL(valueChanged()),
            this, SLOT(visibleRowCountChanged()));
}

/****************************************************************************/

/** Destructor.
 */
TableModel::~TableModel()
{
    valueHighlightRow.clearVariable();
    clearColumns();
}

/****************************************************************************/

/** Adds a column.
 */
void TableModel::addColumn(TableColumn *col)
{
    beginInsertColumns(QModelIndex(),
            columnVector.count(), columnVector.count());
    columnVector.append(col);
    endInsertColumns();

    QObject::connect(col, SIGNAL(dimensionChanged()),
            this, SLOT(dimensionChanged()));
    QObject::connect(col, SIGNAL(headerChanged()),
            this, SLOT(columnHeaderChanged()));
    QObject::connect(col, SIGNAL(valueChanged()),
            this, SLOT(valueChanged()));

    updateRows();
}

/****************************************************************************/

/** Clears the Columns.
 */
void TableModel::clearColumns()
{
    ColumnVector::const_iterator it;

    beginRemoveColumns(QModelIndex(), 0, columnVector.count() - 1);
    columnVector.clear();
    endRemoveColumns();

    for (it = columnVector.begin(); it != columnVector.end(); it++) {
        QObject::disconnect(*it, SIGNAL(dimensionChanged()),
                this, SLOT(dimensionChanged()));
        QObject::disconnect(*it, SIGNAL(headerChanged()),
                this, SLOT(columnHeaderChanged()));
        QObject::disconnect(*it, SIGNAL(valueChanged()),
                this, SLOT(valueChanged()));
    }

    updateRows();
}

/****************************************************************************/

bool TableModel::isEditing() const
{
    ColumnVector::const_iterator it;
    bool editing = false;

    for (it = columnVector.begin(); it != columnVector.end(); it++) {
        if ((*it)->isEditing()) {
            editing = true;
            break;
        }
    }

    return editing;
}

/****************************************************************************/

unsigned int TableModel::getRowCapacity() const
{
  return rowCapacity;
}

/****************************************************************************/

bool TableModel::hasVisibleRowsVariable() const
{
  return visibleRowCount.hasVariable();
}

/****************************************************************************/

/** Implements the model interface.
 *
 * \returns Number of rows.
 */
int TableModel::rowCount(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return rows;
    } else {
        return 0;
    }
}

/****************************************************************************/

/** Implements the model interface.
 *
 * \returns Number of columns.
 */
int TableModel::columnCount(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return columnVector.count();
    } else {
        return 0;
    }
}

/****************************************************************************/

/** Implements the Model interface.
 */
QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    return columnVector[index.column()]->data(index.row(), role);
}

/****************************************************************************/

/** Implements the Model interface.
 */
QVariant TableModel::headerData(
        int section,
        Qt::Orientation o,
        int role
        ) const
{
    if (o != Qt::Horizontal) {
        return QVariant();
    }

    return columnVector[section]->headerData(role);
}

/****************************************************************************/

/** Implements the Model interface.
 */
Qt::ItemFlags TableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags f = 0;

    if (index.isValid()) {
        f = columnVector[index.column()]->flags(index.row());
    }

    return f;
}

/****************************************************************************/

bool TableModel::setData(const QModelIndex &index,
        const QVariant &value, int role)
{
    if (!index.isValid()) {
        return false;
    }

    bool ret = columnVector[index.column()]->setData(index.row(),
            value.toString(), role);

    emit editingChanged(isEditing());

    return ret;
}

/****************************************************************************/

void TableModel::setHighlightRowVariable(
        PdCom::Variable *pv, /**< Process variable. */
        double sampleTime /**< Sample time. */
        )
{
    clearHighlightRowVariable();

    if (!pv) {
        return;
    }

    valueHighlightRow.setVariable(pv, sampleTime);
}

/****************************************************************************/

void TableModel::clearHighlightRowVariable()
{
    ColumnVector::const_iterator it;

    valueHighlightRow.clearVariable();

    for (it = columnVector.begin(); it != columnVector.end(); it++) {
        (*it)->setHighlightRow(-1);
    }
}

/****************************************************************************/

void TableModel::setVisibleRowsVariable(PdCom::Variable *pv)
{
    clearVisibleRowsVariable();

    if (!pv) {
        return;
    }

    visibleRowCount.setVariable(pv);
}

/****************************************************************************/

void TableModel::setHighlightColor(QColor hc, int idx)
{
    if (idx <= -1) {
        ColumnVector::const_iterator it;

        for (it = columnVector.begin(); it != columnVector.end(); it++) {
            (*it)->setHighlightColor(hc);
        }
    } else if (idx < columnVector.size()) {
        columnVector[idx]->setHighlightColor(hc);
    }
}

/****************************************************************************/

void TableModel::setDisabledColor(QColor dc, int idx)
{
    if (idx <= -1) {
        ColumnVector::const_iterator it;

        for (it = columnVector.begin(); it != columnVector.end(); it++) {
            (*it)->setDisabledColor(dc);
        }
    } else if (idx < columnVector.size()) {
        columnVector[idx]->setDisabledColor(dc);
    }
}

/****************************************************************************/

void TableModel::clearVisibleRowsVariable()
{
    visibleRowCount.clearVariable();
    visibleRows = UINT_MAX;
    updateRows();
}

/****************************************************************************/

/** Commits all edited data.
 */
void TableModel::commit()
{
    ColumnVector::const_iterator it;

    for (it = columnVector.begin(); it != columnVector.end(); it++) {
        (*it)->commit();
    }

    emit editingChanged(false);
}

/****************************************************************************/

/** Reverts all edited data.
 */
void TableModel::revert()
{
    ColumnVector::const_iterator it;

    for (it = columnVector.begin(); it != columnVector.end(); it++) {
        (*it)->revert();
    }

    emit editingChanged(false);
}

/****************************************************************************/

/** updates the visibleRowCount variable.

 */
void TableModel::addRow()
{
    if (rowCapacity > 0) {
        visibleRowCount.writeValue(visibleRows + 1);
    }
}

/****************************************************************************/

/** updates the visibleRowCount variable.

 */
void TableModel::remRow()
{
    if (visibleRows > 1) {
        visibleRowCount.writeValue(visibleRows - 1);
    }
}

/****************************************************************************/

/** Calculates the number of table rows.
 */
void TableModel::updateRows()
{
    ColumnVector::const_iterator it;
    unsigned int maxRows = 0;

    for (it = columnVector.begin(); it != columnVector.end(); it++) {
        unsigned int r = (*it)->getRows();
        if (r > maxRows) {
            maxRows = r;
        }
    }

    if (maxRows > visibleRows) {
        rowCapacity = maxRows - visibleRows;
        maxRows = visibleRows;
    }
    else {
        rowCapacity = 0;
    }

    if (maxRows > rows) {
        beginInsertRows(QModelIndex(), rows, maxRows - 1);
        rows = maxRows;
        endInsertRows();
    } else if (maxRows < rows) {
        beginRemoveRows(QModelIndex(), maxRows, rows - 1);
        rows = maxRows;
        endRemoveRows();
    }
}

/****************************************************************************/

/** Reacts on process variable dimension changes.
 */
void TableModel::dimensionChanged()
{
    updateRows();
}

/****************************************************************************/

/** Reacts on header data changes.
 */
void TableModel::columnHeaderChanged()
{
    TableColumn *col = dynamic_cast<TableColumn *>(sender());
    int j = columnVector.indexOf(col);

    if (j > -1) {
        headerDataChanged(Qt::Horizontal, j, j);
    }
}

/****************************************************************************/

/** Reacts on process variable changes.
 */
void TableModel::valueChanged()
{
    TableColumn *col = dynamic_cast<TableColumn *>(sender());

    int j = columnVector.indexOf(col);
    if (j > -1) {
        QModelIndex topLeft = index(0, j);
        QModelIndex bottomRight = index(qMin(col->getRows(), rows) - 1, j);
        emit dataChanged(topLeft, bottomRight);
        /* qDebug() << "Table changes: " << topLeft.row()
         * << topLeft.column() << bottomRight.row()
         * << bottomRight.column(); */
    }
}

/****************************************************************************/

void TableModel::highlightRowChanged()
{
    ColumnVector::const_iterator it;

    unsigned int row = -1;

    if (valueHighlightRow.hasData()) {
        row = valueHighlightRow.getValue();
    }

    for (it = columnVector.begin(); it != columnVector.end(); it++) {
        (*it)->setHighlightRow(row);
    }

    if ((columnVector.count() > 0) && (row < rows)) {
        QModelIndex topLeft = index(row, 0);
        QModelIndex bottomRight = index(row, columnVector.count() - 1);
        emit dataChanged(topLeft, bottomRight);
    }
}

/****************************************************************************/

void TableModel::visibleRowCountChanged()
{
    if (visibleRowCount.hasData()) {
        visibleRows = visibleRowCount.getValue();
        updateRows();
    }
}

/****************************************************************************/
