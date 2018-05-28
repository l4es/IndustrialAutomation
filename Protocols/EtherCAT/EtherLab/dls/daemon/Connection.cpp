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

#include <errno.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/select.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <google/protobuf/io/coded_stream.h>

#include "config.h"
#include "Connection.h"
#include "proto/dls.pb.h"
#include "ProcMother.h"

#include <iostream>
#include <typeinfo>
using namespace std;

#define PFX (pfx() + "." + __func__ + "() ").c_str()

/*****************************************************************************/

Connection::Connection(ProcMother *parent_proc, int fd):
    _parent_proc(parent_proc),
    _fd(fd),
    _ret(0),
    _running(true),
    _messageSize(0U)
{
    pthread_mutex_init(&_mutex, NULL);
}

/*****************************************************************************/

Connection::~Connection()
{
    cerr << PFX << "Closing connection." << endl;
    close(_fd);
}

/*****************************************************************************/

int Connection::start_thread()
{
    int ret;

    ret = pthread_create(&_thread, NULL, _run_static, this);
    if (ret) {
        return ret;
    }

    return 0;
}

/*****************************************************************************/

int Connection::thread_finished()
{
    void *res;

    int ret = pthread_tryjoin_np(_thread, &res);

    if (ret == EBUSY) {
        return 0;
    }
    if (ret) {
        return ret;
    }
    else {
        _ret = (uint64_t) res;
        return 1;
    }
}

/*****************************************************************************/

void Connection::lock()
{
    pthread_mutex_lock(&_mutex);
}

/*****************************************************************************/

void Connection::unlock()
{
    pthread_mutex_unlock(&_mutex);
}

/*****************************************************************************/

void *Connection::_run_static(void *arg)
{
    Connection *conn = (Connection *) arg;
    return conn->_run();
}

/*****************************************************************************/

void *Connection::_run()
{
    fd_set rfds;
    fd_set wfds;
    int ret;

    pthread_mutex_lock(&_mutex);
    _send_hello();
    pthread_mutex_unlock(&_mutex);

    while (_running) {
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);
        FD_SET(_fd, &rfds);

        if (!_sendBuffer.empty()) {
            FD_SET(_fd, &wfds);
        }

        ret = select(_fd + 1, &rfds, &wfds, NULL, NULL);
        if (ret == -1) {
            if (errno != EINTR) {
                char ebuf[1024], *str = strerror_r(errno, ebuf, sizeof(ebuf));
                cerr << PFX << "select() failed: " << str << endl;
                _running = false;
            }
        }

        if (ret > 0) { // file descriptors ready
            if (FD_ISSET(_fd, &rfds)) {
                pthread_mutex_lock(&_mutex);
                _receive_data();
                pthread_mutex_unlock(&_mutex);
            }
            if (FD_ISSET(_fd, &wfds)) {
                pthread_mutex_lock(&_mutex);
                _send_data();
                pthread_mutex_unlock(&_mutex);
            }
        }
    }

    return (void *) 0;
}

/*****************************************************************************/

void Connection::_receive_data()
{
    char data[1024];
    int ret = ::recv(_fd, data, sizeof(data), 0);

    if (ret == 0) {
        cerr << PFX << "Connection closed by peer." << endl;
        _running = false;
        return;
    }

    if (ret < 0) {
        cerr << PFX << "recv() failed: " << strerror(errno) << endl;
        _running = false;
        return;
    }

    _receiveBuffer += string(data, ret);

    if (!_messageSize) {
        google::protobuf::io::CodedInputStream
            ci((const google::protobuf::uint8 *) _receiveBuffer.c_str(),
                    _receiveBuffer.size());
        bool success = ci.ReadVarint32(&_messageSize);
        if (!success) {
            cerr << PFX << "Varint32 incomplete (size = "
                << _receiveBuffer.size() << "). Wait for next receive!"
                << endl;
            return;
        }

        int varIntSize =
            google::protobuf::io::CodedOutputStream::VarintSize32(
                    _messageSize);
        _receiveBuffer.erase(0, varIntSize);
    }

    if ((unsigned int) _receiveBuffer.size() < _messageSize) {
        cerr << PFX << "Data missing. Wait for next receive!" << endl;
        return;
    }

    DlsProto::Request req;
    bool success = req.ParseFromArray(_receiveBuffer.c_str(), _messageSize);
    if (!success) {
        cerr << PFX << "ParseFromArray() failed!" << endl;
        _running = true;
        return;
    }

#ifdef DLS_PROTO_DEBUG
    cerr << PFX << "Received request with " << _messageSize
        << " bytes: " << req.DebugString() << endl;
#endif

    _receiveBuffer.erase(0, _messageSize);
    _messageSize = 0;
    _request_time.set_now();

    _process(req);
}

/*****************************************************************************/

void Connection::_send_data()
{
    if (_sendBuffer.empty()) {
        return;
    }

    ssize_t ret = ::send(_fd, _sendBuffer.c_str(), _sendBuffer.size(), 0);

    if (ret < 0) {
        cerr << PFX << "send() failed: " << strerror(errno) << endl;
        _running = false;
        return;
    }

    _sendBuffer.erase(0, ret);
}

/*****************************************************************************/

void Connection::_send_msg(
        google::protobuf::Message &msg
#ifdef DLS_PROTO_DEBUG
        , bool debug
#endif
        )
{
    try {
        DlsProto::Response &res = dynamic_cast<DlsProto::Response &>(msg);
        if (_request_time != (int64_t) 0) {
            LibDLS::Time t;
            t.set_now();
            res.set_response_time((t - _request_time).to_int64());
        }
    }
    catch (bad_cast &e) {
    }

    int messageSize = msg.ByteSize();

#ifdef DLS_PROTO_DEBUG
    if (debug) {
        cerr << PFX << "Sending message with "
            << messageSize << " bytes: " << endl
            << msg.DebugString() << endl;
    }
#endif

    google::protobuf::uint8 varIntStr[32];
    google::protobuf::uint8 *past =
        google::protobuf::io::CodedOutputStream::
        WriteVarint32ToArray(messageSize, varIntStr);
    int varIntStrSize = past - varIntStr;
    _sendBuffer += string((const char *) varIntStr, varIntStrSize);

    string str;
    msg.SerializeToString(&str);
    _sendBuffer += str;
}

/*****************************************************************************/

void Connection::_send_hello()
{
    DlsProto::Hello msg;
    msg.set_version(PACKAGE_VERSION);
    msg.set_revision(REVISION);
    msg.set_protocol_version(2); // support message_request (see dls.proto)
    _send_msg(msg);
}

/*****************************************************************************/

void Connection::_process(const DlsProto::Request &req)
{
    if (req.has_dir_info()) {
        _process_dir_info(req.dir_info());
    }

    if (req.has_job_request()) {
        _process_job_request(req.job_request());
    }
}

/*****************************************************************************/

void Connection::_process_dir_info(const DlsProto::DirInfoRequest &req)
{
    string path;
    if (req.path() != "") {
        path = req.path();
    }
    else {
        path = _parent_proc->dls_dir();
    }

    try {
        _dir.set_uri(path);
        _dir.import();
    }
    catch (LibDLS::DirectoryException &e) {
        DlsProto::Response res;
        DlsProto::Error *err = res.mutable_error();
        err->set_message(e.msg);
        _send_msg(res);
        return;
    }

    DlsProto::Response res;
    _dir.set_dir_info(res.mutable_dir_info());
    _send_msg(res);
}

/*****************************************************************************/

void Connection::_process_job_request(const DlsProto::JobRequest &req)
{
    LibDLS::Job *job = _dir.find_job(req.id());

    if (!job) {
        DlsProto::Response res;
        stringstream str;
        str << "Job " << req.id() << " not found!";
        DlsProto::Error *err = res.mutable_error();
        err->set_message(str.str());
        _send_msg(res);
        return;
    }

    if (req.fetch_channels()) {
        job->fetch_channels();
        DlsProto::Response res;
        DlsProto::DirInfo *dir_info = res.mutable_dir_info();
        DlsProto::JobInfo *job_info = dir_info->add_job();
        job->set_job_info(job_info, false);
        _send_msg(res);
    }

    if (req.has_channel_request()) {
        _process_channel_request(job, req.channel_request());
    }

    if (req.has_message_request()) {
        const DlsProto::MessageRequest &msg_req = req.message_request();
        list<LibDLS::Job::Message> msgs;
        msgs = job->load_msg_filtered(msg_req.start(), msg_req.end(),
                msg_req.filter(), msg_req.language());

        DlsProto::Response res;
        DlsProto::DirInfo *dir_info = res.mutable_dir_info();
        DlsProto::JobInfo *job_info = dir_info->add_job();
        for (list<LibDLS::Job::Message>::const_iterator msg_i = msgs.begin();
                msg_i != msgs.end(); msg_i++) {
            DlsProto::Message *msg = job_info->add_message();
            msg->set_time(msg_i->time.to_uint64());
            switch (msg_i->type) {
                case LibDLS::Job::Message::Unknown:
                default:
                    msg->set_type(DlsProto::MsgUnknown);
                    break;
                case LibDLS::Job::Message::Info:
                    msg->set_type(DlsProto::MsgInfo);
                    break;
                case LibDLS::Job::Message::Warning:
                    msg->set_type(DlsProto::MsgWarning);
                    break;
                case LibDLS::Job::Message::Error:
                    msg->set_type(DlsProto::MsgError);
                    break;
                case LibDLS::Job::Message::Critical:
                    msg->set_type(DlsProto::MsgCritical);
                    break;
                case LibDLS::Job::Message::Broadcast:
                    msg->set_type(DlsProto::MsgBroadcast);
                    break;
            }
            msg->set_text(msg_i->text);
        }
        _send_msg(res);
    }
}

/*****************************************************************************/

void Connection::_process_channel_request(
        LibDLS::Job *job,
        const DlsProto::ChannelRequest &req
        )
{
    LibDLS::Channel *channel = job->find_channel(req.id());

    if (!channel) {
        stringstream str;
        str << "Channel " << req.id()
            << " not found in job " << job->id() << "!";
        DlsProto::Response res;
        DlsProto::Error *err = res.mutable_error();
        err->set_message(str.str());
        _send_msg(res);
        return;
    }

    if (req.fetch_chunks()) {
        pair<set<LibDLS::Chunk *>, set<int64_t> > updated_removed;

        try {
            updated_removed = channel->fetch_chunks();
        }
        catch (LibDLS::ChannelException &e) {
            stringstream str;
            str << "fetch_chunks(): " << e.msg;
            DlsProto::Response res;
            DlsProto::Error *err = res.mutable_error();
            err->set_message(str.str());
            _send_msg(res);
            return;
        }

        DlsProto::Response res;
        DlsProto::DirInfo *dir_info = res.mutable_dir_info();
        DlsProto::JobInfo *job_info = dir_info->add_job();
        DlsProto::ChannelInfo *channel_info = job_info->add_channel();
        channel_info->set_id(channel->dir_index());

        // add updated chunks
        for (set<LibDLS::Chunk *>::const_iterator upd_i =
                updated_removed.first.begin();
                upd_i != updated_removed.first.end(); upd_i++) {
            (*upd_i)->set_chunk_info(channel_info->add_chunk());
        }

        // add removed chunks
        for (set<int64_t>::const_iterator rem_i =
                updated_removed.second.begin();
                rem_i != updated_removed.second.end(); rem_i++) {
            channel_info->add_removed_chunks(*rem_i);
        }

        _send_msg(res);
    }

    if (req.has_data_request()) {
        const DlsProto::DataRequest &data_req = req.data_request();
        unsigned int min_values = 0;
        if (data_req.has_min_values()) {
            min_values = data_req.min_values();
        }
        unsigned int decimation = 0;
        if (data_req.has_decimation()) {
            decimation = data_req.decimation();
        }
        channel->fetch_data(LibDLS::Time(data_req.start()),
                LibDLS::Time(data_req.end()), min_values,
                _static_data_callback, this, decimation);

        DlsProto::Response res;
        res.set_end_of_response(true);
        _send_msg(res);
    }
}

/*****************************************************************************/

int Connection::_static_data_callback(LibDLS::Data *data, void *cb_data)
{
    Connection *c = (Connection *) cb_data;
    c->_data_callback(data);
    return 0;
}

/*****************************************************************************/

void Connection::_data_callback(LibDLS::Data *data)
{
    DlsProto::Response res;
    DlsProto::Data *data_res = res.mutable_data();
    data_res->set_start_time(data->start_time().to_int64());
    data_res->set_time_per_value(data->time_per_value().to_int64());
    data_res->set_meta_type((DlsProto::MetaType) data->meta_type());
    data_res->set_meta_level(data->meta_level());

    for (size_t i = 0; i < data->size(); i++) {
        data_res->add_value(data->value(i));
    }

    _send_msg(res
#ifdef DLS_PROTO_DEBUG
            , 0
#endif
            );
}

/****************************************************************************/

std::string Connection::pfx() const
{
    stringstream str;
    str << "Connection(" << _fd << ")";
    return str.str();
}

/*****************************************************************************/
