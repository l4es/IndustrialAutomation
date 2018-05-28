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

#ifndef PD_TABLECOLUMN_H
#define PD_TABLECOLUMN_H

#include <QColor>
#include <QHash>
#include <QObject>
#include <QString>

#include <pdcom.h>

namespace Pd {

/****************************************************************************/

/** Table Column.
 */
class Q_DECL_EXPORT TableColumn:
    public QObject,
    public PdCom::Subscriber
{
    Q_OBJECT

    public:
        TableColumn(const QString & = QString());
        ~TableColumn();

        void setHeader(const QString &);
        const QString &getHeader() const { return header; }

        void setVariable(PdCom::Variable *,
                double = 0.0, double = 1.0, double = 0.0);
        void clearVariable();
        void clearData();

        quint32 getDecimals() const;
        void setDecimals(quint32);

        unsigned int getRows() const;

        QVariant data(unsigned int, int) const;
        QVariant headerData(int) const;
        Qt::ItemFlags flags(unsigned int) const;
        bool setData(unsigned int, const QString &, int);
        void setEnabled(bool, int = -1);

        bool isEditing() const { return editData != NULL; }
        bool isEnabled() const { return enabled; }
        void commit();
        void revert();

        void setHighlightRow(int);

        void setHighlightColor(QColor);
        void setDisabledColor(QColor);

        /** Exception type.
         */
        struct Exception {
            /** Constructor.
             */
            Exception(const QString &msg): msg(msg) {}
            QString msg; /**< Exception message. */
        };

    signals:
        void dimensionChanged();
        void headerChanged();
        void valueChanged();

    protected:
        PdCom::Variable::Scale scale; /**< Scale vector. */

    private:
        QString header; /**< Table column header. */
        PdCom::Variable *variable; /**< Subscribed variable. */
        bool dataPresent; /**< Valid data have been received. */
        double *editData; /**< Temporary editing data. */
        bool enabled;
        QHash<unsigned int, bool> enabledRows;
        int highlightRow; /**< Index of the row to highlight, or -1. */
        quint32 decimals; /**< Number of decimal digits. */

        QColor highlightColor;
        QColor disabledColor;

        void notify(PdCom::Variable *); // pure-virtual from PdCom::Subscriber

        // pure-virtual from PdCom::Subscriber
        void notifyDelete(PdCom::Variable *);
};

/****************************************************************************/

} // namespace

#endif
