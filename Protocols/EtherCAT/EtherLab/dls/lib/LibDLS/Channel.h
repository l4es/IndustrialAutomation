/*****************************************************************************
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
 ****************************************************************************/

#ifndef LibDLSChannelH
#define LibDLSChannelH

/****************************************************************************/

#include <string>
#include <map>
#include <utility> // pair
#include <set>

#include "Exception.h"
#include "Time.h"

#include "Chunk.h"

/****************************************************************************/

namespace DlsProto {
    class ChannelInfo;
}

namespace LibDLS {

/****************************************************************************/

class Job;

/****************************************************************************/

/**
   Channel exception.
*/

class ChannelException:
    public Exception
{
    public:
        ChannelException(const std::string &pmsg):
            Exception(pmsg) {};
};

/****************************************************************************/


/**
   Darstellung eines Kanals in der Anzeige
*/

class Channel
{
public:
    Channel(Job *);
    Channel(Job *, const DlsProto::ChannelInfo &);
    ~Channel();

    Job *getJob() const { return _job; }

    void import(const std::string &, unsigned int);
    std::pair<std::set<Chunk *>, std::set<int64_t> > fetch_chunks();
    void fetch_data(Time, Time, unsigned int,
                    DataCallback, void *, unsigned int = 1) const;

    std::string path() const { return _path; }
    unsigned int dir_index() const { return _dir_index; }

    const std::string &name() const { return _name; }
    const std::string &unit() const { return _unit; }
    ChannelType type() const { return _type; }

    typedef std::map<int64_t, Chunk> ChunkMap;
    const ChunkMap &chunks() const { return _chunks; }
    bool has_same_chunks_as(const Channel &) const;

    Time start() const { return _range_start; }
    Time end() const { return _range_end; }

    bool operator<(const Channel &other) const {
        return _dir_index < other._dir_index;
    }

    void set_channel_info(DlsProto::ChannelInfo *) const;
    void set_chunk_info(DlsProto::ChannelInfo *) const;

private:
    Job * const _job; /**< Parent job. */
    std::string _path; /**< channel directory path */
    unsigned int _dir_index; /**< index of the channel directory */

    std::string _name; /**< channel name */
    std::string _unit; /**< channel unit */
    ChannelType _type; /**< channel type */

    ChunkMap _chunks; /**< list of chunks */
    Time _range_start; /**< start of channel data range */
    Time _range_end; /**< end of channel data range */

    std::pair<std::set<Chunk *>, std::set<int64_t> > _fetch_chunks_local();
    std::pair<std::set<Chunk *>, std::set<int64_t> > _fetch_chunks_network();
    void _fetch_data_local(Time, Time, unsigned int,
                    DataCallback, void *, unsigned int) const;
    void _fetch_data_network(Time, Time, unsigned int,
                    DataCallback, void *, unsigned int) const;

    Channel();
};

} // namespace

/****************************************************************************/

#endif
