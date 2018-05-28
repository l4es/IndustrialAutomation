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

#ifndef ViewChannelHpp
#define ViewChannelHpp

/*****************************************************************************/

#include <string>
#include <list>
using namespace std;

#include "lib/LibDLS/Exception.h"
#include "lib/LibDLS/Time.h"
#include "lib/LibDLS/Channel.h"

#include "lib/RingBufferT.h"

#include "ViewGlobals.h"

/*****************************************************************************/

/**
   Union of a channel and loaded data.
*/

class ViewChannel
{
    friend int data_callback(LibDLS::Data *, void *);

public:
    ViewChannel();
    ~ViewChannel();

    void set_channel(LibDLS::Channel *);
    void fetch_data(LibDLS::Time, LibDLS::Time, unsigned int);
    LibDLS::Channel *channel();

    const LibDLS::Channel *channel() const;
    const list<LibDLS::Data> &gen_data() const;
    const list<LibDLS::Data> &min_data() const;
    const list<LibDLS::Data> &max_data() const;

    double min() const;
    double max() const;
    unsigned int min_level() const;
    unsigned int max_level() const;

    bool operator<(const ViewChannel &) const;

private:
    LibDLS::Channel *_channel;
    list<LibDLS::Data> _gen_data;
    list<LibDLS::Data> _min_data;
    list<LibDLS::Data> _max_data;

    double _min;
    double _max;
    unsigned int _min_level;
    unsigned int _max_level;

    void _calc_min_max();
    void _calc_min_max_data(const list<LibDLS::Data> &, bool *);
};

/*****************************************************************************/

/**
*/

inline LibDLS::Channel *ViewChannel::channel()
{
    return _channel;
}

/*****************************************************************************/

/**
*/

inline const LibDLS::Channel *ViewChannel::channel() const
{
    return _channel;
}

/*****************************************************************************/

/**
*/

inline const list<LibDLS::Data> &ViewChannel::gen_data() const
{
    return _gen_data;
}

/*****************************************************************************/

/**
*/

inline const list<LibDLS::Data> &ViewChannel::min_data() const
{
    return _min_data;
}

/*****************************************************************************/

/**
*/

inline const list<LibDLS::Data> &ViewChannel::max_data() const
{
    return _max_data;
}

/*****************************************************************************/

/**
   Gibt den kleinsten, geladenen Wert zurück

   \return Kleinster Wert
*/

inline double ViewChannel::min() const
{
    return _min;
}

/*****************************************************************************/

/**
   Gibt den größten, geladenen Wert zurück

   \return Größter Wert
*/

inline double ViewChannel::max() const
{
    return _max;
}

/*****************************************************************************/

/**
   Gibt die niedrigste Meta-Ebene zurück, aus der geladen wurde

   \return Meta-Ebene
*/

inline unsigned int ViewChannel::min_level() const
{
    return _min_level;
}

/*****************************************************************************/

/**
   Gibt die höchste Meta-Ebene zurück, aus der geladen wurde

   \return Meta-Ebene
*/

inline unsigned int ViewChannel::max_level() const
{
    return _max_level;
}

/*****************************************************************************/

#endif
