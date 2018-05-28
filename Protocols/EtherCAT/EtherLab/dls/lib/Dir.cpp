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

#include <sstream>
#include <algorithm>

#include <dirent.h>
#include <sys/types.h>

#ifdef _WIN32
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netdb.h>
#endif

#include <unistd.h>
#include <errno.h>

#include <google/protobuf/io/coded_stream.h>

#include <uriparser/Uri.h>

#include "config.h"
#include "LibDLS/Dir.h"
#include "proto/dls.pb.h"

#ifdef DEBUG_STREAM
#include <iostream>
#endif

using namespace std;
using namespace LibDLS;

#ifdef _WIN32
#define DLS_INVALID_SOCKET INVALID_SOCKET
#define DLS_CLOSE_SOCKET closesocket
#else
#define DLS_INVALID_SOCKET -1
#define DLS_CLOSE_SOCKET ::close
#endif

/*****************************************************************************/

void MyLogHandler(google::protobuf::LogLevel level,
        const char* filename, int line, const std::string& message)
{
    stringstream err;
    err << "protobuf error: " << filename << ":" << line
        << ": " << message;
    log(err.str());
}

/*****************************************************************************/

Directory::Directory(const std::string &uri_text):
    _access(Unknown),
    _sock(DLS_INVALID_SOCKET),
    _protocol_version(0),
    _proto_messages_warning_given(false)
{
    set_uri(uri_text);

#ifdef _WIN32
    WSADATA wsaData;
    int err = WSAStartup(0x0202, &wsaData);
    if (err != 0) {
        stringstream msg;
        msg << "WSAStartup() failed with error " << err << ".";
        _error_msg = msg.str();
        log(msg.str());
        throw DirectoryException(msg.str());
    } else {
        stringstream msg;
        msg << "WSAStartup() initialized with version "
            << hex << wsaData.wVersion << ".";
        log(msg.str());
    }
#endif

    google::protobuf::SetLogHandler(MyLogHandler);
}

/*****************************************************************************/

Directory::~Directory()
{
    _disconnect();

    list<Job *>::iterator job_i;
    for (job_i = _jobs.begin(); job_i != _jobs.end(); job_i++) {
        delete *job_i;
    }

#ifdef _WIN32
    WSACleanup();
#endif
}

/*****************************************************************************/

bool compare_job_id(Job *first, Job *second)
{
    return first->id() < second->id();
}

/*****************************************************************************/

std::string uriTextRange(const UriTextRangeA &range)
{
    if (range.first) {
        return string(range.first, range.afterLast - range.first);
    }

    return string();
}

/*****************************************************************************/

std::string uriPathSegment(const UriPathSegmentA *p)
{
    stringstream str;
    bool first = true;

    while (p) {
        if (first) {
            first = false;
        }
        else {
            str << "/";
        }
        str << uriTextRange(p->text);
        p = p->next;
    }
    return str.str();
}

/*****************************************************************************/

#if 0
std::string debugHost(const UriHostDataA &h) {
    stringstream str;
    if (h.ip4) {
        str << "ip4";
    }
    if (h.ip6) {
        str << "ip6";
    }
    str << uriTextRange(h.ipFuture);
    return str.str();
}
#endif

/*****************************************************************************/

void Directory::set_uri(const string &uri_text)
{
    _uri_text = uri_text;

#if 0
    cerr << __func__ << "(" << uri_text << ")" << endl;
#endif

    UriUriA uri;
    UriParserStateA state;
    state.uri = &uri;
    if (uriParseUriA(&state, _uri_text.c_str()) != URI_SUCCESS) {
        stringstream err;
        err << "Failed to parse URI \"" << _uri_text << "\"!";
        _error_msg = err.str();
        log(err.str());
        throw DirectoryException(err.str());
    }

#if 0
    cerr << "scheme " << uriTextRange(uri.scheme) << endl;
    cerr << "userinfo " << uriTextRange(uri.userInfo) << endl;
    cerr << "hostText " << uriTextRange(uri.hostText) << endl;
    cerr << "hostData " << debugHost(uri.hostData) << endl;
    cerr << "portText " << uriTextRange(uri.portText) << endl;
    cerr << "pathHead " << uriPathSegment(uri.pathHead) << endl;
    cerr << "pathTail " << uriPathSegment(uri.pathTail) << endl;
    cerr << "abs " << (uri.absolutePath ? "yes" : "no") << endl;
#endif

    string scheme = uriTextRange(uri.scheme);
    std::transform(scheme.begin(), scheme.end(), scheme.begin(), ::tolower);

    _host = uriTextRange(uri.hostText);
    _port = uriTextRange(uri.portText);
    if (_port == "") {
        _port = "53584";
    }
    _path = uriPathSegment(uri.pathHead);
    if (uri.absolutePath) {
        _path = "/" + _path;
    }

#if 0
    cerr << "host=" << _host << " path=" << _path << endl;
#endif

    uriFreeUriMembersA(&uri);

    if (scheme == "" || scheme == "file") {
        _access = Local;
    }
    else if (scheme == "dls") {
        _access = Network;
    }
    else {
        _access = Unknown;
        stringstream err;
        err << "Unsupported URI scheme \"" << scheme << "\"!";
        _error_msg = err.str();
        log(err.str());
        throw DirectoryException(err.str());
    }
}

/*****************************************************************************/

void Directory::import()
{
    _jobs.clear();

    if (_access == Local) {
        _importLocal();
    }
    else if (_access == Network) {
        _importNetwork();
    }

    // Nach Job-ID sortieren
    _jobs.sort(compare_job_id);

    _notify_observers();
}

/*****************************************************************************/

Job *Directory::job(unsigned int index)
{
    list<Job *>::iterator job_i;

    for (job_i = _jobs.begin(); job_i != _jobs.end(); job_i++, index--) {
        if (!index) {
            return *job_i;
        }
    }

    return NULL;
}

/*****************************************************************************/

Job *Directory::find_job(unsigned int job_id)
{
    list<Job *>::iterator job_i;

    for (job_i = _jobs.begin(); job_i != _jobs.end(); job_i++) {
        if ((*job_i)->id() == job_id) {
            return *job_i;
        }
    }

    return NULL;
}

/*****************************************************************************/

void Directory::set_dir_info(DlsProto::DirInfo *dir_info) const
{
    //dir_info->set_path(_path);

    for (list<LibDLS::Job *>::const_iterator job_i = _jobs.begin();
            job_i != _jobs.end(); job_i++) {
        (*job_i)->set_job_info(dir_info->add_job());
    }
}

/*****************************************************************************/

void Directory::attach_observer(Observer *o)
{
    _observers.insert(o);
}

/*****************************************************************************/

void Directory::remove_observer(Observer *o)
{
    _observers.erase(o);
}

/*****************************************************************************/

void Directory::_importLocal()
{
    stringstream str;
    DIR *dir;
    struct dirent *dir_ent;
    Job *job;
    string dir_name;
    unsigned int job_id;

    str.exceptions(ios::failbit | ios::badbit);

    if (!(dir = opendir(_path.c_str()))) {
        stringstream err;
        err << "Failed to open DLS directory \"" << _path << "\"!";
        _error_msg = err.str();
        log(err.str());
        throw DirectoryException(err.str());
    }

    while ((dir_ent = readdir(dir))) {
        dir_name = dir_ent->d_name;
        if (dir_name.find("job")) continue;

        str.str("");
        str.clear();
        str << dir_name.substr(3);

        try {
            str >> job_id;
        }
        catch (...) {
            continue;
        }

        job = new Job(this);

        try {
            job->import(_path, job_id);
        }
        catch (JobException &e) {
            stringstream err;
            err << "WARNING: Failed to import job "
                 << job_id << ": " << e.msg;
            log(err.str());
            delete job;
            continue;
        }

        _jobs.push_back(job);
    }

    // Verzeichnis schliessen
    closedir(dir);
}

/*****************************************************************************/

void Directory::_importNetwork()
{
    DlsProto::Request req;
    DlsProto::Response res;

    DlsProto::DirInfoRequest *dir_req = req.mutable_dir_info();
    dir_req->set_path(_path);

    _send_message(req);
    _receive_message(res);

    if (res.has_error()) {
        _error_msg = res.error().message();
        log(_error_msg);
        throw DirectoryException(res.error().message());
    }

    const DlsProto::DirInfo &dir_info = res.dir_info();

    _jobs.clear();

    //_path = dir_info.path();

    google::protobuf::RepeatedPtrField<DlsProto::JobInfo>::const_iterator
        job_i;
    for (job_i = dir_info.job().begin();
            job_i != dir_info.job().end(); job_i++) {
        _jobs.push_back(new Job(this, *job_i));
    }
}

/*****************************************************************************/

void Directory::_connect()
{
    if (connected()) {
        return;
    }

    {
        stringstream msg;
        msg << "Connecting to " << _host << " on port " << _port << ".";
        log(msg.str());
    }

    /* Obtain address(es) matching host/port */

    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC; /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Stream socket */

    struct addrinfo *result;
    int ret = getaddrinfo(_host.c_str(), _port.c_str(), &hints, &result);
    if (ret) {
        stringstream err;
        err << "Failed to get address info: " << gai_strerror(ret);
        _error_msg = err.str();
        log(_error_msg);
        throw DirectoryException(err.str());
    }

    struct addrinfo *rp;
#ifdef _WIN32
    SOCKET sock;
#else
    int sock;
#endif
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sock == DLS_INVALID_SOCKET) {
            continue;
        }

#ifdef _WIN32
        unsigned long arg = 0;
        int ret = ioctlsocket(sock, FIONBIO, &arg);
        if (ret) {
            _error_msg = "Failed to set socket to blocking mode!";
            log(_error_msg);
            DLS_CLOSE_SOCKET(sock);
            throw DirectoryException(_error_msg);
        }
#endif

        if (connect(sock, rp->ai_addr, rp->ai_addrlen) != -1) {
            break;
        }

        DLS_CLOSE_SOCKET(sock);
    }

    freeaddrinfo(result);

    if (!rp) {
        _error_msg = "Connection failed!";
        log(_error_msg);
        throw DirectoryException(_error_msg);
    }

    _sock = sock;

    {
        stringstream msg;
        msg << "Connected.";
        log(msg.str());
    }

    /* read hello message */
    _receive_hello();
}

/*****************************************************************************/

void Directory::_disconnect()
{
    if (!connected()) {
        return;
    }

    {
        stringstream msg;
        msg << "Disconnecting.";
        log(msg.str());
    }

    DLS_CLOSE_SOCKET(_sock);
    _sock = DLS_INVALID_SOCKET;
    _protocol_version = 0;
    _proto_messages_warning_given = false;
    _receive_buffer.clear();
}

/*****************************************************************************/

void Directory::_send_data(const char *buffer, size_t size)
{
#ifdef DEBUG_STREAM
    cerr << __func__ << "(" << size << ")" << endl;
#endif

    ssize_t ret;
    while (size > 0) {
        ret = send(_sock, buffer, size, 0);

#ifdef DEBUG_STREAM
        cerr << __func__ << "(): send() returned " << ret << endl;
#endif

        if (ret < 0) {
#ifdef _WIN32
            int e = WSAGetLastError();
#else
            int e = errno;
#endif
            stringstream err;
            err << "send() failed: " << strerror(e);
            log(err.str());
            _disconnect();
            throw DirectoryException(err.str());
        }

        size -= ret;
        buffer += ret;
    }
}

/*****************************************************************************/

void Directory::_send_message(const DlsProto::Request &req)
{
    _connect();

    int messageSize = req.ByteSize();
    string sendBuffer;

#ifdef DLS_PROTO_DEBUG
    cerr << "Sending message with " << messageSize << " bytes:" << endl;
    cerr << req.DebugString() << endl;
#endif

    google::protobuf::uint8 varIntStr[32];
    google::protobuf::uint8 *past =
        google::protobuf::io::CodedOutputStream::
        WriteVarint32ToArray(messageSize, varIntStr);

    int varIntStrSize = past - varIntStr;
    sendBuffer += string((const char *) varIntStr, varIntStrSize);

    string msgStr;
    req.SerializeToString(&msgStr);
    sendBuffer += msgStr;

    _send_data(sendBuffer.c_str(), sendBuffer.size());
}

/*****************************************************************************/

void Directory::_receive_data()
{
#ifdef DEBUG_STREAM
    cerr << __func__ << "()" << endl;
#endif

    ssize_t ret;
    char data[1024];
    ret = recv(_sock, data, sizeof(data), 0);

#ifdef DEBUG_STREAM
    cerr << "recv() returned " << ret << endl;
#endif

    if (ret > 0) {
        _receive_buffer += string(data, ret);
    }
    else if (ret == 0) {
        stringstream err;
        err << "Connection closed by peer.";
        log(err.str());
        _disconnect();
        throw DirectoryException(err.str());
    }
    else {
#ifdef _WIN32
        int e = WSAGetLastError();
#else
        int e = errno;
#endif
        stringstream err;
        err << "recv() failed: " << strerror(e);
        log(err.str());
        _disconnect();
        throw DirectoryException(err.str());
    }
}

/*****************************************************************************/

void Directory::_receive_message(
        google::protobuf::Message &msg,
        bool debug
        )
{
    if (_receive_buffer.empty()) {
        _receive_data();
    }

    unsigned int messageSize = 0;

    while (1) {
        google::protobuf::io::CodedInputStream
            ci((const google::protobuf::uint8 *) _receive_buffer.c_str(),
                    _receive_buffer.size());
        if (ci.ReadVarint32(&messageSize)) {
            break;
        }

        // try to fetch more data to complete varint
#ifdef DEBUG_STREAM
        cerr << "Varint32 incomplete (" << _receive_buffer.size()
            << " bytes). Fetching more data... " << endl;
#endif
        _receive_data();
    }

    int varIntSize =
        google::protobuf::io::CodedOutputStream::VarintSize32(
                messageSize);
    _receive_buffer.erase(0, varIntSize);

    while ((unsigned int) _receive_buffer.size() < messageSize) {
        _receive_data();
    }

    bool success = msg.ParseFromArray(_receive_buffer.c_str(), messageSize);
    if (!success) {
        stringstream err;
        err << "ParseFromArray(" << _receive_buffer.size()
            << " / " << messageSize << ") failed!";
        log(err.str());
        _disconnect();
        throw DirectoryException(err.str());
    }

    _receive_buffer.erase(0, messageSize);

#ifdef DLS_PROTO_DEBUG
    cerr << "Received message with " << messageSize << " bytes. "
        << _receive_buffer.size() << " remaining. " << endl;
    if (debug) {
        cerr << msg.DebugString() << endl;
    }
#endif
}

/*****************************************************************************/

void Directory::_receive_hello()
{
    DlsProto::Hello hello;

    _receive_message(hello);

    _protocol_version = hello.protocol_version();

    stringstream str;
    str << "Received hello from DLS " << hello.version()
        << " " << hello.revision() << " protocol version "
        << hello.protocol_version() << ".";
    log(str.str());
}

/*****************************************************************************/

void Directory::_notify_observers()
{
    std::set<Observer *>::iterator o;
    for (o = _observers.begin(); o != _observers.end(); o++) {
        (*o)->update();
    }
}

/*****************************************************************************/

bool Directory::serverSupportsMessages()
{
    if (!connected()) {
        return false;
    }

    if (_protocol_version < 2) {
        if (!_proto_messages_warning_given) {
            _proto_messages_warning_given = true;
            stringstream err;
            err << "WARNING: Server does not support loading messages. "
                << "Please update to protocol verion 2 or later.";
            log(err.str());
        }
        return false;
    }
    else {
        return true;
    }
}

/*****************************************************************************/
