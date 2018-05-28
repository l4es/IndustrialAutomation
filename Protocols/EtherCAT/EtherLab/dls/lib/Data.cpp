/******************************************************************************
 *
 *  $Id$
 *
 *  This file is part of the Data Logging Service (DLS).
 *
 *  DLS is free software: you can redistribute it and/or modify it under the
 *  terms of the GNU General Public License as published by the Free Software
 *  Foundation, either version 3 of the License, or (at your option) any later
 *  version.
 *
 *  DLS is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with DLS. If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/

#include <iostream>
#include <sstream>
using namespace std;

#include "LibDLS/Data.h"
using namespace LibDLS;

#include "proto/dls.pb.h"

/*****************************************************************************/

/**
   Konstruktor
*/

Data::Data()
{
}

/*****************************************************************************/

/** Copy constructor.
*/
Data::Data(const Data &o)
{
    _start_time = o._start_time;
    _time_per_value = o._time_per_value;
    _meta_type = o._meta_type;
    _meta_level = o._meta_level;
    _data = o._data;
}

/*****************************************************************************/

/** Constructor from protocol message.
*/
Data::Data(const DlsProto::Data &d)
{
    _start_time = d.start_time();
    _time_per_value = d.time_per_value();
    _meta_type = (MetaType) d.meta_type();
    _meta_level = d.meta_level();

    for (int i = 0; i < d.value_size(); i++) {
        _data.push_back(d.value(i));
    }
}

/*****************************************************************************/

/**
   Destruktor
*/

Data::~Data()
{
}

/*****************************************************************************/

/**
   Appends a data block.
*/

void Data::push_back(const Data &other)
{
    unsigned int i;

    if (other._time_per_value != _time_per_value
        || other._start_time != end_time() + _time_per_value) {
        stringstream err;
        err << "WARNING: Data appending failed!";
        log(err.str());
        return;
    }

    for (i = 0; i < other._data.size(); i++)
        _data.push_back(other._data[i]);
}

/*****************************************************************************/

int Data::calc_min_max(double *min, double *max) const
{
    vector<double>::const_iterator data_i;
    double current_min, current_max;

    if (_data.empty()) {
        *min = 0.0;
        *max = 0.0;
        return 0;
    }

    data_i = _data.begin();

    current_min = *data_i;
    current_max = *data_i;
    data_i++;

    while (data_i != _data.end()) {
        if (*data_i < current_min) current_min = *data_i;
        if (*data_i > current_max) current_max = *data_i;
        data_i++;
    }

    *min = current_min;
    *max = current_max;
    return 1;
}

/*****************************************************************************/
