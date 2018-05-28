/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2016       Richard Hacker (lerichi at gmx dot net)
 *
 * This file is part of the PdCom library.
 *
 * The PdCom library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * The PdCom library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the PdCom library. If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/

#ifndef MSR_SUBSCRIPTIONLIST_H
#define MSR_SUBSCRIPTIONLIST_H

#include <map>
#include <list>

/** List that allows insertion and deletion while being iterated */
template <class T1, class T2>
struct SubscriptionList {
    typedef std::list<T1*> List;
    typedef typename List::value_type value_type;
    typedef typename List::reference reference;
    typedef typename List::iterator iterator;

    typedef T2* key_type;
    typedef std::map<key_type, iterator> Map;

    SubscriptionList() {
        it = list.end();
    }

    bool erase(key_type key) {
        typename Map::iterator mit = map.find(key);
        if (it == mit->second)
            ++it;

        list.erase(mit->second);
        map.erase(mit);

        return empty();
    }

    reference operator[] (key_type key) {
        // Insert the new Subscription to list start as well as
        // the iterator thereof into the map
        return *map.insert(
                std::make_pair(key,
                    list.insert(list.begin(), value_type())))
            .first->second;
    }

    size_t count(key_type key) {
        return map.count(key);
    }

    bool empty() {
        return list.empty();
    }

    value_type begin() {
        it = list.begin();
        return next();
    }

    value_type next() {
        return it != list.end() ? *it++ : 0;
    }

    List list;
    Map map;

    iterator it;
};

#endif //MSR_SUBSCRIPTIONLIST_H
