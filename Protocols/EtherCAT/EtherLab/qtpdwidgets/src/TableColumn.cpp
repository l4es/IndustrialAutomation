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

#include "QtPdWidgets/TableColumn.h"

#include <QDebug>
#include <QVariant>
#include <QBrush>

#define DEFAULT_DECIMALS        15
#define DEFAULT_HIGHLIGHT_COLOR QColor(152, 183, 255)
#define DEFAULT_DISABLED_COLOR  QColor(220, 220, 220)

using Pd::TableColumn;

/****************************************************************************/

/** Constructor.
 */
TableColumn::TableColumn(const QString &header):
    header(header),
    variable(NULL),
    dataPresent(false),
    editData(NULL),
    enabled(true),
    highlightRow(-1),
    decimals(DEFAULT_DECIMALS),
    highlightColor(DEFAULT_HIGHLIGHT_COLOR),
    disabledColor(DEFAULT_DISABLED_COLOR)
{
}

/****************************************************************************/

/** Destructor.
 */
TableColumn::~TableColumn()
{
    if (editData) {
        delete [] editData;
    }
}

/****************************************************************************/

/** Sets the column header.
 */
void TableColumn::setHeader(const QString &h)
{
    header = h;

    emit headerChanged();
}

/****************************************************************************/

/** Subscribes to a ProcessVariable.
 */
void TableColumn::setVariable(
        PdCom::Variable *pv,
        double sampleTime,
        double gain,
        double offset
        )
{
    clearVariable();

    if (pv) {
        scale.gain = gain;
        scale.offset = offset;

        try {
            pv->subscribe(this, sampleTime);
        } catch (PdCom::Exception &e) {
            qCritical() << QString("Failed to subscribe to variable"
                    " \"%1\" with sample time %2: %3")
                .arg(QString(pv->path.c_str()))
                .arg(sampleTime)
                .arg(e.what());
            return;
        }

        variable = pv;
        emit dimensionChanged();
        emit valueChanged();

        if (!sampleTime)
            pv->poll(this); // poll once to get initial value
    }
}

/****************************************************************************/

/** Unsubscribe from a Variable.
 */
void TableColumn::clearVariable()
{
    if (variable) {
        variable->unsubscribe(this);
        variable = NULL;
        dataPresent = false;
        if (editData) {
            delete [] editData;
            editData = NULL;
        }
        emit dimensionChanged();
        emit valueChanged();
    }
}

/****************************************************************************/

void TableColumn::clearData()
{
    dataPresent = false;
    emit valueChanged();
}

/****************************************************************************/

/** Sets the number of #decimals.
 */
void TableColumn::setDecimals(quint32 value)
{
    if (value != decimals) {
        decimals = value;
        emit valueChanged();
    }
}

/****************************************************************************/

/** Get number of rows.
 */
unsigned int TableColumn::getRows() const
{
    if (variable) {
        return variable->dimension.getElementCount();
    } else {
        return 0U;
    }
}

/****************************************************************************/

/** Get display text.
 */
QVariant TableColumn::data(unsigned int row, int role) const
{
    switch (role) {

        case Qt::DisplayRole:
        case Qt::EditRole:
            if (variable && dataPresent) {
                unsigned int nelem = variable->dimension.getElementCount();
                if (row < nelem) {
                    double val;

                    if (editData) {
                        val = editData[row];
                    } else {
                        double v[nelem];
                        variable->getValue(v, nelem, &scale);
                        val = v[row];
                    }

                    return QLocale().toString(val, 'f', decimals);

                } else {
                    return "";
                }
            } else {
                return "";
            }
            break;

        case Qt::BackgroundRole:
            if (variable) {
                unsigned int nelem = variable->dimension.getElementCount();

                if (!variable->isWrite() || !enabled ||
                        !enabledRows.value(row, true)) {
                    return QBrush(disabledColor);
                } else if (editData) {
                    return QBrush(Qt::yellow);
                } else if ((int) row == highlightRow) {
                    return QBrush(highlightColor);
                } else if (row >= nelem) {
                    return QBrush(Qt::darkGray);
                }
            }
            return QBrush();

        default:
            return QVariant();
    }
}

/****************************************************************************/

/** Get header data.
 */
QVariant TableColumn::headerData(int role) const
{
    switch (role) {
        case Qt::DisplayRole:
            return header;

        default:
            return QVariant();
    }
}

/****************************************************************************/

/** Implements the Model interface.
 */
Qt::ItemFlags TableColumn::flags(unsigned int row) const
{
    Qt::ItemFlags f = 0;

    if (!variable || !dataPresent ||
            row >= variable->dimension.getElementCount()) {
        return f;
    }

    f |= Qt::ItemIsEnabled;

    if (variable->isWrite() && enabled && enabledRows.value(row, true)) {
        f |= Qt::ItemIsEditable;
    }

    return f;
}

/****************************************************************************/

/** Set an edit value.
 */
bool TableColumn::setData(
        unsigned int row,
        const QString &valueString,
        int role
        )
{
    Q_UNUSED(role);

    bool ok;
    double value = QLocale().toDouble(valueString, &ok);

    if (!variable || !dataPresent || !ok) {
        qCritical() << "Failed to edit variable";
        return false;
    }

    size_t nelem = variable->dimension.getElementCount();

    if (row >= nelem) {
        qCritical() << "row" << row << "does not exist";
        return false;
    }

    double data[nelem];
    variable->getValue(data, nelem, &scale);

    if (!editData) {
        editData = new double[nelem];
        variable->getValue(editData, nelem, &scale);
    }

    editData[row] = value;

    for (size_t i = 0; i < nelem; i++) {
        if (data[i] != editData[i]) {
            return true;
        }
    }

    delete [] editData;
    editData = NULL;

    return true;
}

/****************************************************************************/

/** Set enabled for a column
 */
void TableColumn::setEnabled(bool value, int row)
{
    if (row < 0) {
        enabled = value;
    } else {
        enabledRows.insert(row, value);
    }

    emit valueChanged(); // FIXME, gibt es auch ein redraw
}

/****************************************************************************/

/** Write edited data to the process.
 */
void TableColumn::commit()
{
    if (!editData || !variable) {
        return;
    }

    size_t nelem = variable->dimension.getElementCount();

    variable->setValue(editData, nelem, &scale);

    delete [] editData;
    editData = NULL;
    emit valueChanged();
}

/****************************************************************************/

/** Reverts all edited values.
 */
void TableColumn::revert()
{
    if (editData) {
        delete [] editData;
        editData = NULL;
        emit valueChanged();
    }
}

/****************************************************************************/

void TableColumn::setHighlightRow(int value)
{
    highlightRow = value;
}

/****************************************************************************/

void TableColumn::setHighlightColor(QColor hc)
{
    highlightColor = hc;
    emit valueChanged();
}

/****************************************************************************/

void TableColumn::setDisabledColor(QColor dc)
{
    disabledColor = dc;
    emit valueChanged();
}

/****************************************************************************/

/** This virtual method is called by the ProcessVariable, if its value
 * changes.
 */
void TableColumn::notify(
        PdCom::Variable *
        )
{
    dataPresent = true;
    emit valueChanged();
}

/****************************************************************************/

/** Notification for variable deletion.
 *
 * This virtual function is called by the Variable, when it is about to be
 * destroyed.
 */
void TableColumn::notifyDelete(
        PdCom::Variable *
        )
{
    variable = NULL;
    dataPresent = false;
    if (editData) {
        delete [] editData;
        editData = NULL;
    }
    emit dimensionChanged();
}

/****************************************************************************/
