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

#include <fcntl.h>
#include <dirent.h>

#include <iostream>
#include <sstream>
#include <fstream>
using namespace std;

/*****************************************************************************/

#include "LibDLS/globals.h"
#include "LibDLS/Chunk.h"
#include "LibDLS/Job.h"
#include "LibDLS/Channel.h"
#include "LibDLS/Dir.h"

#include "proto/dls.pb.h"

#include "XmlParser.h"
using namespace LibDLS;

#define DEBUG_TIMING 0

#if DEBUG_TIMING
#include <iomanip>
#endif

/*****************************************************************************/

/**
   Constructor.
*/

Channel::Channel(Job *job):
    _job(job),
    _dir_index(0)
{
}

/*****************************************************************************/

Channel::Channel(Job *job, const DlsProto::ChannelInfo &info):
    _job(job),
    _dir_index(info.id()),
    _name(info.name()),
    _unit(info.unit()),
    _type((ChannelType) info.type())
{
}

/*****************************************************************************/

/**
   Destructor.
*/

Channel::~Channel()
{
    _chunks.clear();
}

/*****************************************************************************/

/**
   Imports channel information.
*/

void Channel::import(
        const string &channel_path, /**< channel directory path */
        unsigned int dir_index /**< index of the channel directory */
        )
{
    string file_name;
    fstream file;
    XmlParser xml;

    _path = channel_path;
    _dir_index = dir_index;

    file_name = _path + "/channel.xml";
    file.open(file_name.c_str(), ios::in);

    if (!file.is_open()) {
        stringstream err;
        err << "Failed to open channel file \"" << file_name << "\".";
        throw ChannelException(err.str());
    }

    try
    {
        xml.parse(&file, "dlschannel", dxttBegin);
        xml.parse(&file, "channel", dxttSingle);

        _name = xml.tag()->att("name")->to_str();
        _unit = xml.tag()->att("unit")->to_str();

        if ((_type = str_to_channel_type(xml.tag()->att("type")->to_str()))
            == TUNKNOWN) {
            stringstream err;
            file.close();
            err << "Unknown channel type \""
                << xml.tag()->att("type")->to_str() << "\".";
            throw ChannelException(err.str());
        }

        xml.parse(&file, "dlschannel", dxttEnd);
    }
    catch (EXmlParser &e) {
        stringstream err;
        file.close();
        err << "Parse error in " << file_name << ": " << e.msg;
        throw ChannelException(err.str());
    }
    catch (EXmlParserEOF &e) {
        stringstream err;
        file.close();
        err << "Parse error in " << file_name << ": " << e.msg;
        throw ChannelException(err.str());
    }
    catch (EXmlTag &e) {
        stringstream err;
        file.close();
        err << "XML tag parse error in " << file_name << ": " << e.msg;
        throw ChannelException(err.str());
    }

    file.close();
}

/*****************************************************************************/

std::pair<std::set<Chunk *>, std::set<int64_t> > Channel::fetch_chunks()
{
    if (_job->dir()->access() == Directory::Local) {
        return _fetch_chunks_local();
    }
    else {
        return _fetch_chunks_network();
    }
}

/*****************************************************************************/

/**
   Loads data values of the specified time range and resolution.

   If \a min_values is non-zero, the smallest resolution is chosen, that
   contains at least \a min_values for the requested range.

   Otherwise, all values (the maximum resolution) is loaded.

   The data are passed via the callback function.
*/

void Channel::fetch_data(
        Time start, /**< start of requested time range */
        Time end, /**< end of requested time range */
        unsigned int min_values, /**< minimal number */
        DataCallback cb, /**< callback */
        void *cb_data, /**< arbitrary callback parameter */
        unsigned int decimation /**< Decimation. */
        ) const
{
    if (_job->dir()->access() == Directory::Local) {
        _fetch_data_local(start, end, min_values, cb, cb_data, decimation);
    }
    else {
        _fetch_data_network(start, end, min_values, cb, cb_data, decimation);
    }
}

/*****************************************************************************/

/**
   Returns true, if this channel has exactly the same chunk times
   as the other channel.
   \return true, if channels have the same chunks.
*/

bool Channel::has_same_chunks_as(const Channel &other) const
{
    return _chunks == other._chunks;
}

/*****************************************************************************/

void Channel::set_channel_info(DlsProto::ChannelInfo *channel_info) const
{
    channel_info->set_id(_dir_index);
    channel_info->set_name(_name);
    channel_info->set_unit(_unit);
    channel_info->set_type((DlsProto::ChannelType) _type);
}

/*****************************************************************************/

void Channel::set_chunk_info(DlsProto::ChannelInfo *channel_info) const
{
    for (ChunkMap::const_iterator chunk_i = _chunks.begin();
            chunk_i != _chunks.end(); chunk_i++) {
        chunk_i->second.set_chunk_info(channel_info->add_chunk());
    }
}

/*****************************************************************************/

/**
   Lädt die Liste der Chunks und importiert deren Eigenschaften

   \param dls_dir DLS-Datenverzeichnis
   \param job_id Auftrags-ID
*/

std::pair<std::set<Chunk *>, std::set<int64_t> >
Channel::_fetch_chunks_local()
{
    DIR *dir;
    struct dirent *dir_ent;
    string dir_ent_name;
    Chunk new_chunk, *chunk;
    bool first = true;
    int64_t dir_time;
    set<int64_t> dir_chunks;
    std::pair<std::set<Chunk *>, std::set<int64_t> > ret;

#if DEBUG_TIMING
    Time ts, te;
    ts.set_now();
#endif

    _range_start.set_null();
    _range_end.set_null();

    if (!(dir = opendir(path().c_str()))) {
        stringstream err;
        err << "Failed to open \"" << path() << "\".";
        throw ChannelException(err.str());
    }

    while ((dir_ent = readdir(dir))) {
        dir_ent_name = dir_ent->d_name;
        if (dir_ent_name.find("chunk") != 0) {
            continue;
        }

        // get chunk time from directory name for map indexing
        stringstream str;
        str << dir_ent_name.substr(5);
        str >> dir_time;

        // remember chunk time for later removal check
        dir_chunks.insert(dir_time);

        ChunkMap::iterator chunk_i = _chunks.find(dir_time);
        if (chunk_i == _chunks.end()) {
            // chunk not existing yet
            try {
                new_chunk.import(path() + "/" + dir_ent_name, _type);
            }
            catch (ChunkException &e) {
                stringstream err;
                err << "WARNING: Failed import chunk: " << e.msg;
                log(err.str());
                continue;
            }

            pair<int64_t, Chunk> val(dir_time, new_chunk);
            pair<ChunkMap::iterator, bool> ins_ret = _chunks.insert(val);
            chunk = &ins_ret.first->second;
            ret.first.insert(chunk);
        }
        else {
            // chunk existing
            chunk = &chunk_i->second;
        }

        if (chunk->incomplete()) {
            // chunk is still logging, fetch current end time
            try {
                chunk->fetch_range();
            }
            catch (ChunkException &e) {
                stringstream err;
                err << "WARNING: Failed to fetch chunk range: " << e.msg;
                log(err.str());
                continue;
            }
            ret.first.insert(chunk);
        }

        if (first) {
            _range_start = chunk->start();
            _range_end = chunk->end();
            first = false;
        }
        else {
            if (chunk->start() < _range_start) {
                _range_start = chunk->start();
            }
            if (chunk->end() > _range_end) {
                _range_end = chunk->end();
            }
        }
    }

    closedir(dir);


    // check for removed chunks and erase them from map
    ChunkMap::iterator ch_i = _chunks.begin();
    while (ch_i != _chunks.end()) {
        ChunkMap::iterator cur = ch_i++;
        if (dir_chunks.find(cur->first) == dir_chunks.end()) {
            ret.second.insert(cur->first);
            _chunks.erase(cur);
        }
    }

#if DEBUG_TIMING
    te.set_now();
    stringstream msg;
    msg << __func__ << "() " << ts.diff_str_to(te);
    log(msg.str());
#endif

    return ret;
}

/*****************************************************************************/

std::pair<std::set<Chunk *>, std::set<int64_t> >
Channel::_fetch_chunks_network()
{
    DlsProto::Request req;
    DlsProto::Response res;
    Chunk new_chunk;
    Chunk *chunk;
    std::pair<std::set<Chunk *>, std::set<int64_t> > ret;
#if DEBUG_TIMING
    Time ts;
    ts.set_now();
#endif

    DlsProto::JobRequest *job_req = req.mutable_job_request();
    job_req->set_id(_job->id());
    DlsProto::ChannelRequest *ch_req = job_req->mutable_channel_request();
    ch_req->set_id(_dir_index);
    ch_req->set_fetch_chunks(true);

    try {
        _job->dir()->_send_message(req);
    }
    catch (DirectoryException &e) {
        stringstream err;
        err << "Failed to request chunks: " << e.msg;
        log(err.str());
        return ret;
    }

    try {
        _job->dir()->_receive_message(res);
    }
    catch (DirectoryException &e) {
        stringstream err;
        err << "Failed to receive chunks: " << e.msg;
        log(err.str());
        return ret;
    }

    if (res.has_error()) {
        stringstream err;
        err << "Error response: " << res.error().message();
        log(err.str());
        return ret;
    }

    const DlsProto::DirInfo &dir_info = res.dir_info();
    const DlsProto::JobInfo &job_info = dir_info.job(0); // FIXME check
    const DlsProto::ChannelInfo &ch_info = job_info.channel(0); // FIXME check

    google::protobuf::RepeatedPtrField<DlsProto::ChunkInfo>::const_iterator
        ch_info_i;
    for (ch_info_i = ch_info.chunk().begin();
            ch_info_i != ch_info.chunk().end(); ch_info_i++) {
        uint64_t start = ch_info_i->start();
        ChunkMap::iterator chunk_i = _chunks.find(start);
        if (chunk_i == _chunks.end()) {
            new_chunk = Chunk(*ch_info_i, _type);
            pair<int64_t, Chunk> val(start, new_chunk);
            pair<ChunkMap::iterator, bool> ins_ret = _chunks.insert(val);
            chunk = &ins_ret.first->second;
        } else {
            // chunk existing
            chunk_i->second.update_from_chunk_info(*ch_info_i);
            chunk = &chunk_i->second;
        }
        ret.first.insert(chunk);
    }

    // update time range
    ChunkMap::const_iterator ch_i = _chunks.begin();
    if (ch_i != _chunks.end()) {
        _range_start = ch_i->second.start();
        _range_end = ch_i->second.end();
        ch_i++;

        for (; ch_i != _chunks.end(); ch_i++) {
            if (ch_i->second.start() < _range_start) {
                _range_start = ch_i->second.start();
            }
            if (ch_i->second.end() > _range_end) {
                _range_end = ch_i->second.end();
            }
        }
    }
    else {
        _range_start.set_null();
        _range_end.set_null();
    }

    for (google::protobuf::RepeatedField<uint64_t>::const_iterator rem_i =
            ch_info.removed_chunks().begin();
            rem_i != ch_info.removed_chunks().end(); rem_i++) {
        ret.second.insert(*rem_i);
        _chunks.erase(*rem_i);
    }

#if DEBUG_TIMING
    Time te;
    te.set_now();
    stringstream msg;
    msg << __func__ << "() " << setw(20) << ts.diff_str_to(te);
    if (res.has_response_time()) {
        Time r(res.response_time());
        Time z;
        msg << " server: " << setw(20) << z.diff_str_to(r);
    }
    log(msg.str());
    cerr << msg.str() << endl;
#endif
    return ret;
}

/*****************************************************************************/

void Channel::_fetch_data_local(
        Time start, /**< start of requested time range */
        Time end, /**< end of requested time range */
        unsigned int min_values, /**< minimal number */
        DataCallback cb, /**< callback */
        void *cb_data, /**< arbitrary callback parameter */
        unsigned int decimation /**< Decimation. */
        ) const
{
#if DEBUG_TIMING
    Time ts, te;
    ts.set_now();
#endif

    ChunkMap::const_iterator chunk_i;

    if (start < end) {
        try {
            for (chunk_i = _chunks.begin(); chunk_i != _chunks.end();
                    chunk_i++) {
                chunk_i->second.fetch_data(start, end,
                        min_values, cb, cb_data, decimation);
            }
        } catch (ChunkException &e) {
            stringstream err;
            err << "Failed to fetch data from chunk: " << e.msg;
            throw ChannelException(err.str());
        }
    }

#if DEBUG_TIMING
    te.set_now();
    stringstream msg;
    msg << "fetch_data " << ts.diff_str_to(te);
    log(msg.str());
#endif
}

/*****************************************************************************/

void Channel::_fetch_data_network(
        Time start, /**< start of requested time range */
        Time end, /**< end of requested time range */
        unsigned int min_values, /**< minimal number */
        DataCallback cb, /**< callback */
        void *cb_data, /**< arbitrary callback parameter */
        unsigned int decimation /**< Decimation. */
        ) const
{
    DlsProto::Request req;
    DlsProto::Response res;
#if DEBUG_TIMING
    Time ts;
    ts.set_now();
#endif

    DlsProto::JobRequest *job_req = req.mutable_job_request();
    job_req->set_id(_job->id());
    DlsProto::ChannelRequest *ch_req = job_req->mutable_channel_request();
    ch_req->set_id(_dir_index);
    DlsProto::DataRequest *data_req = ch_req->mutable_data_request();
    data_req->set_start(start.to_uint64());
    data_req->set_end(end.to_uint64());
    data_req->set_min_values(min_values);
    data_req->set_decimation(decimation);

    try {
        _job->dir()->_send_message(req);
    }
    catch (DirectoryException &e) {
        stringstream err;
        err << "Failed to request data: " << e.msg;
        log(err.str());
        return;
    }

    while(1) {
        try {
            _job->dir()->_receive_message(res, 0);
        }
        catch (DirectoryException &e) {
            stringstream err;
            err << "Failed to receive data: " << e.msg;
            log(err.str());
            break;
        }

        if (res.has_error()) {
            stringstream err;
            err << "Error response: " << res.error().message();
            log(err.str());
            break;
        }

        if (res.has_end_of_response() && res.end_of_response()) {
            break;
        }

        if (!res.has_data()) {
            stringstream err;
            err << "Error: Expected data!";
            log(err.str());
            break;
        }

        const DlsProto::Data &data_res = res.data();
        Data *d = new Data(data_res);
        int adopted = cb(d, cb_data);
        if (!adopted) {
            delete d;
        }
    }

#if DEBUG_TIMING
    Time te;
    te.set_now();
    stringstream msg;
    msg << __func__ << "() " << setw(22) << ts.diff_str_to(te);
    if (res.has_response_time()) {
        Time r(res.response_time());
        Time z;
        msg << " server: " << setw(20) << z.diff_str_to(r);
    }
    log(msg.str());
    cerr << msg.str() << endl;
#endif
}

/*****************************************************************************/
