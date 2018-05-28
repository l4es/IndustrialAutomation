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

#ifndef LibDLSJobH
#define LibDLSJobH

/****************************************************************************/

#include <string>
#include <list>
#include <set>

#include "Exception.h"
#include "Time.h"
#include "JobPreset.h"
#include "Channel.h"

/****************************************************************************/

namespace DlsProto {
    class Response;
    class JobInfo;
}

namespace LibDLS {

class Directory;
class BaseMessageList;

/****************************************************************************/

/** Job exception.
 */
class JobException:
    public Exception
{
    public:
        JobException(const std::string &pmsg):
            Exception(pmsg) {};
};

/****************************************************************************/

/** Measuring job.
 */
class Job
{
    public:
        Job(Directory *);
        Job(Directory *, const DlsProto::JobInfo &);
        ~Job();

        void import(const std::string &, unsigned int);
        void fetch_channels();

        std::list<Channel> &channels() { return _channels; }
        Channel *channel(unsigned int);
        Channel *find_channel(unsigned int);
        std::set<Channel *> find_channels_by_name(const std::string &);

        const std::string &path() const { return _path; }
        unsigned int id() const { return _preset.id(); }
        const JobPreset &preset() const { return _preset; }

        bool operator<(const Job &) const;

        struct Message
        {
            Time time;
            enum Type {
                Unknown = -1,
                Info,
                Warning,
                Error,
                Critical,
                Broadcast,
                TypeCount
            };
            Type type;
            std::string text; /**< Message text coded in UTF-8. */

            bool operator<(const Message &other) const {
                return time < other.time;
            }

            const std::string &type_str() const;
        };

        std::list<Message> load_msg(Time, Time,
                std::string = std::string()) const;
        std::list<Message> load_msg_filtered(Time, Time,
                const std::string &, std::string = std::string()) const;

        void set_job_info(DlsProto::JobInfo *, bool = true) const;
        Directory *dir() const { return _dir; }

    private:
        Directory * const _dir; /**< Parent directory. */
        std::string _path; /**< Job path. */
        JobPreset _preset; /**< Job preset. */
        std::list<Channel> _channels; /**< List of recorded channels. */
        BaseMessageList *_messages; /**< List of messages. */

        void _fetch_channels_local();
        void _fetch_channels_network();

        void _load_msg_local(std::list<Message> &, Time, Time,
                const std::string &, std::string = std::string()) const;
        void _load_msg_network(std::list<Message> &, Time, Time,
                const std::string &, std::string = std::string()) const;

        Job(); // private
};

/*****************************************************************************/

} // namespace

/*****************************************************************************/

#endif
