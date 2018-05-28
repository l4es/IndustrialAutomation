/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2011 - 2012  Andreas Stewering-Bone <ab@igh-essen.com>
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

#ifndef NOPD_TOUCHEDIT_H
#define NOPD_TOUCHEDIT_H

#include <QFrame>

#include "Export.h"

namespace Pd {

class TouchEditDialog;

/****************************************************************************/

/** Touch edit for non-process data.
 */
class QDESIGNER_WIDGET_EXPORT NoPdTouchEdit:
    public QFrame
{
    Q_OBJECT

    Q_PROPERTY(double value
            READ getValue WRITE setValue RESET resetValue)
    Q_PROPERTY(Qt::Alignment alignment
            READ getAlignment WRITE setAlignment RESET resetAlignment)
    Q_PROPERTY(quint32 decimals
            READ getDecimals WRITE setDecimals RESET resetDecimals)
    Q_PROPERTY(QString suffix
            READ getSuffix WRITE setSuffix RESET resetSuffix)
    Q_PROPERTY(double lowerLimit
            READ getLowerLimit WRITE setLowerLimit
            RESET resetLowerLimit)
    Q_PROPERTY(double upperLimit
            READ getUpperLimit WRITE setUpperLimit
            RESET resetUpperLimit)

    public:
        NoPdTouchEdit(QWidget *parent = 0);
        virtual ~NoPdTouchEdit();

        double getValue() const { return value; }
        void setValue(double);
        void resetValue();

        Qt::Alignment getAlignment() const { return alignment; }
        void setAlignment(Qt::Alignment);
        void resetAlignment();

        quint32 getDecimals() const { return decimals; }
        void setDecimals(quint32);
        void resetDecimals();

        const QString &getSuffix() const { return suffix; }
        void setSuffix(const QString &);
        void resetSuffix();

        double getLowerLimit() const { return lowerLimit; }
        void setLowerLimit(double);
        void resetLowerLimit();

        double getUpperLimit() const { return upperLimit; }
        void setUpperLimit(double);
        void resetUpperLimit();

        QSize sizeHint() const;

    signals:
        void valueChanged();

    protected:
        bool event(QEvent *);
        void paintEvent(QPaintEvent *);
        void drawText(QPaintEvent *, QPainter &);

    private:
        double value; /**< Current value. */
        Qt::Alignment alignment; /**< Text alignment. */
        quint32 decimals; /**< Number of decimal digits. */
        QString suffix; /**< Suffix, that is appended to the displayed
                          string. The suffix is appended without a separator
                          (like in other Qt classes), so if you want to
                          specify a unit, you'll have to set suffix to
                          " kN", for example. */
        double lowerLimit;
        double upperLimit;
        int editDigit;
        TouchEditDialog *editDialog;

        void openDialog();
        void changeEvent(QEvent *); // from QWidget
        void retranslate();
};

/****************************************************************************/

} // namespace

#endif
