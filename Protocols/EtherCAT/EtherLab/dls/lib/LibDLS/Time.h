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

#ifndef LibDLSTimeH
#define LibDLSTimeH

/*****************************************************************************/

#include <sys/time.h>
#include <stdint.h>

#include <ostream>

#include "globals.h"

/*****************************************************************************/

namespace LibDLS {

/*****************************************************************************/

/**
   Datentyp zur Speicherung der Zeit in Mikrosekunden

   Dieser Datentyp verwaltet einen int64_t integer
   zur Speicherung der Mikrosekunden nach epoch.
*/

class DLS_EXPORT Time
{
    friend std::ostream &operator <<(std::ostream &, const Time &);

    public:
        Time();
        Time(int64_t);
        Time(uint64_t);
        Time(double);
        Time(struct timeval *);
        Time(struct tm *, unsigned int);

        void from_dbl_time(double);
        void set_null();
        void set_now();
        int set_date(int, int = 1, int = 1, int = 0, int = 0, int = 0);

        Time &operator =(int64_t);
        Time &operator =(uint64_t);
        Time &operator =(double);
        Time &operator =(struct timeval);

        bool operator ==(const Time &) const;
        bool operator !=(const Time &) const;
        bool operator <(const Time &) const;
        bool operator >(const Time &) const;
        bool operator <=(const Time &) const;
        bool operator >=(const Time &) const;
        bool is_null() const;

        Time operator +(const Time &) const;
        Time &operator +=(const Time &);
        Time operator -(const Time &) const;
        Time operator *(int64_t) const;

        double to_dbl() const;
        double to_dbl_time() const;
        int64_t to_int64() const;
        uint64_t to_uint64() const;
        time_t to_time_t() const;
        std::string to_str() const;
        struct timeval to_tv() const;
        std::string to_real_time() const;
        std::string format_time(const char *) const;
        std::string to_rfc811_time() const;
        std::string to_iso_time() const;
        std::string diff_str_to(const Time &) const;
        int year() const;
        int month() const;
        int day() const;
        int hour() const;
        int min() const;
        int sec() const;
        int day_of_week() const;
        bool is_leap_year() const;
        int month_days() const;

        static Time now();

    private:
        int64_t _time; /**< Mikrosekunden nach epoch */
};

/*****************************************************************************/

std::ostream &operator <<(std::ostream &, const Time &);

/*****************************************************************************/

} // namespace

/*****************************************************************************/

#endif
