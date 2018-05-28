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

#ifndef LibDLSDataH
#define LibDLSDataH

/*****************************************************************************/

#include <vector>

#include "globals.h"
#include "Time.h"

namespace DlsProto {
    class Data;
}

namespace LibDLS {

/*************************************************************************/

/** Block of data values.
 */
class Data
{
    public:
        Data();
        Data(const Data &);
        Data(const DlsProto::Data &);
        ~Data();

        template <class T>
            void import(Time, Time, MetaType, unsigned int,
                    unsigned int, unsigned int &, T*, unsigned int);

        void push_back(const Data &);

        Time start_time() const { return _start_time; }
        Time end_time() const {
            return _start_time + _time_per_value * _data.size();
        }
        Time time_per_value() const { return _time_per_value; }
        MetaType meta_type() const { return _meta_type; }
        unsigned int meta_level() const { return _meta_level; }

        size_t size() const { return _data.size(); }
        double value(unsigned int index) const { return _data[index]; }
        Time time(unsigned int index) const {
            return _start_time + _time_per_value * index;
        }

        int calc_min_max(double *, double *) const;

    protected:
        Time _start_time;
        Time _time_per_value;
        MetaType _meta_type;
        unsigned int _meta_level;
        std::vector<double> _data;
};

/*****************************************************************************/

/** Imports data block properties.
 */
template <class T>
void LibDLS::Data::import(
        Time time,
        Time tpv,
        MetaType meta_type,
        unsigned int meta_level,
        unsigned int decimation,
        unsigned int &decimationCounter,
        T *data,
        unsigned int size
        )
{
    unsigned int i;

    _start_time = time + tpv * decimationCounter;
    _time_per_value = tpv * decimation;
    _meta_type = meta_type;
    _meta_level = meta_level;
    _data.clear();

    for (i = 0; i < size; i++) {
        if (!decimationCounter) {
            _data.push_back((double) data[i]);
            decimationCounter = decimation - 1;
        } else {
            decimationCounter--;
        }
    }
}

/*****************************************************************************/

} // namespace

/*****************************************************************************/

#endif
