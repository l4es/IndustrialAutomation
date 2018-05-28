/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2009 - 2012  Florian Pose <fp@igh-essen.com>
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

#ifndef PD_VALUERING_H
#define PD_VALUERING_H

#include <QList>
#include <QPair>

#include <pdcom.h>

#define VALUERING_DEBUG 0

#if VALUERING_DEBUG
#include <QDebug>
#endif

namespace Pd {

/****************************************************************************/

/** Time/Value ring buffer.
 */
template <class T>
class ValueRing
{
    public:
        ValueRing();
        ~ValueRing();

        void setRange(const PdCom::Time &);
        const PdCom::Time &getRange() const { return range; }

        void append(const PdCom::Time &time, const T &value);
        void copyUntil(const ValueRing<T> &, const PdCom::Time &);
        void clear();

        unsigned int getLength() const;

        typedef QPair<PdCom::Time, T> TimeValuePair;
        TimeValuePair &operator[](unsigned int index);
        const TimeValuePair &operator[](unsigned int index) const;
        TimeValuePair &operator[](int index);
        const TimeValuePair &operator[](int index) const;

    private:
        QList<TimeValuePair> ring; /**< Time/Value ring. */
        unsigned int offset; /**< Ring offset. */
        unsigned int length; /**< Number of valid elements at #offset. */
        PdCom::Time range; /**< Time range covered by the ring. Used to remove
                             values from the end. */

        void removeDeprecated();
        void reshape();
};

/****************************************************************************/

/** Constructor.
 */
template <class T>
ValueRing<T>::ValueRing():
    offset(0),
    length(0)
{
}

/****************************************************************************/

/** Destructor.
 */
template <class T>
ValueRing<T>::~ValueRing()
{
}

/****************************************************************************/

/** Sets the #range.
 */
template <class T>
void ValueRing<T>::setRange(const PdCom::Time &r)
{
    range = r;
    removeDeprecated();
}

/****************************************************************************/

/** Appends a value to the ring.
 */
template <class T>
void ValueRing<T>::append(const PdCom::Time &time, const T &value)
{
    TimeValuePair newPair(time, value);

    if ((int) length < ring.size()) {
        unsigned int o = (offset + length) % ring.size();
        ring[o] = newPair;
    } else {
#if VALUERING_DEBUG
        qDebug() << ring.size() << "reached.";
#endif
        // ring is full
        if (offset) {
            reshape();
        }
        ring.append(newPair);
    }

    length++;
    removeDeprecated();
}

/****************************************************************************/

/** Copies data from another ring, up to a specific time.
 */
template <class T>
void ValueRing<T>::copyUntil(
        const ValueRing<T> &other,
        const PdCom::Time &time
        )
{
    clear();

    for (unsigned int i = 0; i < other.length; i++) {
        TimeValuePair p = other[i];
        if (p.first > time) {
            break;
        }

        ring.append(p);
        length++;
    }
}

/****************************************************************************/

/** Clears the ring.
 */
template <class T>
void ValueRing<T>::clear()
{
    ring.clear();
    offset = 0;
    length = 0;
}

/****************************************************************************/

/**
 * \returns The #length.
 */
template <class T>
inline unsigned int ValueRing<T>::getLength() const
{
    return length;
}

/****************************************************************************/

/** Index operator.
 */
template <class T>
inline typename ValueRing<T>::TimeValuePair &ValueRing<T>::operator[](
        unsigned int index
        )
{
    return ring[(offset + index) % ring.size()];
}

/****************************************************************************/

/** Constant index operator.
 */
template <class T>
inline const typename ValueRing<T>::TimeValuePair &ValueRing<T>::operator[](
        unsigned int index
        ) const
{
    return ring.at((offset + index) % ring.size());
}

/****************************************************************************/

/** Index operator with int argument.
 *
 * Allows to specify negative indices (from the end).
 */
template <class T>
inline typename ValueRing<T>::TimeValuePair &ValueRing<T>::operator[](
        int index
        )
{
    if (index >= 0) {
        return ring[(offset + index) % ring.size()];
    }
    else {
        return ring[(offset + length + index) % ring.size()];
    }
}

/****************************************************************************/

/** Constant index operator with int argument.
 *
 * Allows to specify negative indices (from the end).
 */
template <class T>
inline const typename ValueRing<T>::TimeValuePair &ValueRing<T>::operator[](
        int index
        ) const
{
    if (index >= 0) {
        return ring.at((offset + index) % ring.size());
    }
    else {
        return ring.at((offset + length + index) % ring.size());
    }
}

/****************************************************************************/

/** Remove values that exceed the time range.
 */
template <class T>
void ValueRing<T>::removeDeprecated()
{
    if (length) {
        PdCom::Time depTime = (*this)[length - 1].first - range;
        while (length) {
            if (ring[offset].first < depTime) {
                offset = (offset + 1) % ring.size();
                length--;
            } else {
                break;
            }
        }
    }
}

/****************************************************************************/

/** Reshape the ring to move the offset to zero.
 */
template <class T>
void ValueRing<T>::reshape()
{
    QList<TimeValuePair> newRing;
    unsigned int i;

#if VALUERING_DEBUG
    qDebug() << "reshaping" << length << "values";
#endif

    for (i = 0; i < length; i++) {
        newRing.append((*this)[i]);
    }
    ring = newRing;
    offset = 0;
}

/****************************************************************************/

} // namespace

#endif
