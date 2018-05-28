/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2009  Florian Pose <fp@igh-essen.com>
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

#ifndef PD_SCALARVARIABLE_H
#define PD_SCALARVARIABLE_H

#include <QObject>

#include "ScalarSubscriber.h"

namespace Pd {

/*****************************************************************************/

/** Abstract Scalar Value.
 */
class Q_DECL_EXPORT AbstractScalarVariable:
    public QObject, public ScalarSubscriber
{
    Q_OBJECT

    public:
        struct Exception {
            /** Constructor.
             */
            Exception(const QString &msg):
                msg(msg) {}
            QString msg; /**< Exception message. */
        };

    signals:
        void valueChanged(); /**< Emitted, when the value changes, or the
                               variable is disconnected. */
};

/****************************************************************************/

/** Scalar Value Template.
 */
template <class T>
class ScalarVariable:
    public AbstractScalarVariable
{
    public:
        ScalarVariable();
        virtual ~ScalarVariable();

        void clearData(); // pure-virtual from ScalarSubscriber
        bool hasData() const;

        T getValue() const;
        PdCom::Time getMTime() const;
        void inc();

    private:
        T value; /**< Current value. */
        PdCom::Time mTime; /**< Modification Time of Current value. */
        bool dataPresent; /**< There is a process value to display. */

        void notify(PdCom::Variable *); // pure-virtual from PdCom::Subscriber
};

/****************************************************************************/

typedef ScalarVariable<bool> BoolVariable;
typedef ScalarVariable<int> IntVariable;
typedef ScalarVariable<double> DoubleVariable;

/****************************************************************************/

/** Constructor.
 */
template <class T>
ScalarVariable<T>::ScalarVariable():
    value((T) 0),
    dataPresent(false)
{
}

/****************************************************************************/

/** Destructor.
 */
template <class T>
ScalarVariable<T>::~ScalarVariable()
{
}

/****************************************************************************/

template <class T>
void ScalarVariable<T>::clearData()
{
    value = ((T) 0);
    dataPresent = false;
    emit valueChanged();
}

/****************************************************************************/

/**
 * \return \a true, if data are present.
 */
template <class T>
inline bool ScalarVariable<T>::hasData() const
{
    return dataPresent;
}

/****************************************************************************/

/**
 * \return The current #value.
 */
template <class T>
inline T ScalarVariable<T>::getValue() const
{
    return value;
}

/****************************************************************************/

/**
 * \return The current Modification Time.
 */
template <class T>
inline PdCom::Time ScalarVariable<T>::getMTime() const
{
    return mTime;
}

/****************************************************************************/

/** Increments the current #value and writes it to the process.
 *
 * This does \a not update #value directly.
 */
template <class T>
void ScalarVariable<T>::inc()
{
    writeValue(value + 1);
}

/****************************************************************************/

/** This virtual method is called by the ProcessVariable, if its value
 * changes.
 */
template <class T>
void ScalarVariable<T>::notify(
        PdCom::Variable *pv
        )
{
    T newValue;

    pv->getValue(&newValue, 1, &scale);
    mTime = pv->getMTime();

    if (newValue != value || !dataPresent) {
        value = newValue;
        dataPresent = true;
        emit valueChanged();
    }
}

/****************************************************************************/

} // namespace

#endif
