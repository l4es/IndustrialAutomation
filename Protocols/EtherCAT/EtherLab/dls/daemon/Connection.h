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

#ifndef DLSConnectionHpp
#define DLSConnectionHpp

/*****************************************************************************/

#include <pthread.h>
#include <sstream>

#include "config.h"
#include "lib/LibDLS/Dir.h"
#include "proto/dls.pb.h"

class ProcMother;

/*****************************************************************************/

/** Incoming network connection.
 */
class Connection
{
public:
    Connection(ProcMother *, int);
    ~Connection();

    int start_thread();
    int thread_finished();
    int return_code() const { return _ret; }

    void lock();
    void unlock();

private:
    ProcMother * const _parent_proc;
    const int _fd;
    pthread_t _thread;
    pthread_mutex_t _mutex;
    int _ret; /**< Return value. */
    bool _running;
    std::string _sendBuffer;
    std::string _receiveBuffer;
    unsigned int _messageSize;
    LibDLS::Directory _dir;
    LibDLS::Time _request_time;

    static void *_run_static(void *);
    void *_run();
    void _receive_data();
    void _send_data();
    void _send_msg(google::protobuf::Message &
#ifdef DLS_PROTO_DEBUG
            , bool debug = 1
#endif
            );
    void _send_hello();
    void _process(const DlsProto::Request &);
    void _process_dir_info(const DlsProto::DirInfoRequest &);
    void _process_job_request(const DlsProto::JobRequest &);
    void _process_channel_request(LibDLS::Job *,
            const DlsProto::ChannelRequest &);
    static int _static_data_callback(LibDLS::Data *, void *);
    void _data_callback(LibDLS::Data *);
    std::string pfx() const;
};

/*****************************************************************************/

#endif


