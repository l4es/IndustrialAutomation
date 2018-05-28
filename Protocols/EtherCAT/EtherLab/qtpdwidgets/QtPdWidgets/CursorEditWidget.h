/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2011  Florian Pose <fp@igh-essen.com>
 * Copyright (C) 2011  Andreas Stewering-Bone <ab@igh-essen.com>
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

#ifndef CURSOREDITWIDGET_H
#define CURSOREDITWIDGET_H

#include <QWidget>

/****************************************************************************/

/** Numerical editing widget with cursor movement.
 */
class CursorEditWidget:
    public QWidget
{
    Q_OBJECT

    public:
        CursorEditWidget(QWidget *parent = 0);
        virtual ~CursorEditWidget();

        QSize sizeHint() const;
        void setValue(double);
        void setDecimals(int);
        void setSuffix(const QString &);
        void setLowerLimit(double);
        void setUpperLimit(double);
        void setEditDigit(int);
        int getEditDigit() const;
        double getValue() const;

    public slots:
        void digitLeft();
        void digitRight();
        void digitUp();
        void digitDown();
        void setZero();

    protected:
        void paintEvent(QPaintEvent *);

    private:
        double value;
        int decimals;
        QString suffix;
        double lowerLimit;
        double upperLimit;
        int digPos;
        QString valueStr;

        void updateValueStr();
};

/****************************************************************************/

/**
 * \return The value.
 */
inline double CursorEditWidget::getValue() const
{
    return valueStr.toDouble();
}

/****************************************************************************/

/**
 * \return The digPos.
 */
inline int CursorEditWidget::getEditDigit() const
{
    return digPos;
}

/****************************************************************************/

#endif
