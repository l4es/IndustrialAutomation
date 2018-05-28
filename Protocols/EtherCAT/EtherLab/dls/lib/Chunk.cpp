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

#include <dirent.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <typeinfo>
using namespace std;

/*****************************************************************************/

#include "XmlParser.h"
#include "IndexT.h"
#include "File.h"
#include "RingBufferT.h"
#include "CompressionT.h"

#include "proto/dls.pb.h"

#include "LibDLS/Channel.h"
#include "LibDLS/Chunk.h"
using namespace LibDLS;

/*****************************************************************************/

//#define DEBUG_DATA

/*****************************************************************************/

/**
  Constructor.
 */

Chunk::Chunk():
    _sample_frequency(0.0),
    _meta_reduction(0),
    _format_index(0),
    _mdct_block_size(0),
    _type(TUNKNOWN),
    _incomplete(true)
{
}

/*****************************************************************************/

Chunk::Chunk(const DlsProto::ChunkInfo &info, ChannelType type):
    _sample_frequency(0.0),
    _meta_reduction(0),
    _format_index(0),
    _mdct_block_size(0),
    _start(info.start()),
    _end(info.end()),
    _type(type),
    _incomplete(info.incomplete())
{
}

/*****************************************************************************/

/**
   Destruktor
*/

Chunk::~Chunk()
{
}

/*****************************************************************************/

/**
   Importiert die Informationen aus "chunk.xml"
*/

void Chunk::import(const string &path, ChannelType type)
{
    stringstream err;
    string chunk_file_name, format_str;
    fstream file;
    XmlParser xml;
    int i;

    _dir = path;
    _type = type;
    _incomplete = true;

    chunk_file_name = _dir + "/chunk.xml";

    file.open(chunk_file_name.c_str(), ios::in);

    if (!file.is_open()) {
        err << "Failed to open chunk file \"" << chunk_file_name << "\"!";
        throw ChunkException(err.str());
    }

    try {
        xml.parse(&file, "dlschunk", dxttBegin);
        xml.parse(&file, "chunk", dxttSingle);

        _sample_frequency = xml.tag()->att("sample_frequency")->to_dbl();
        _meta_reduction = xml.tag()->att("meta_reduction")->to_int();
        format_str = xml.tag()->att("format")->to_str();

        _format_index = FORMAT_INVALID;
        for (i = 0; i < FORMAT_COUNT; i++) {
            if (format_str == format_strings[i]) {
                _format_index = i;
                break;
            }
        }

        if (_format_index == FORMAT_INVALID) {
            throw ChunkException("Unknown compression format!");
        }

        if (_format_index == FORMAT_MDCT) {
            _mdct_block_size = xml.tag()->att("mdct_block_size")->to_int();
        }
    }
    catch (EXmlParser &e) {
        file.close();
        err << "Parsing error: " << e.msg;
        throw ChunkException(err.str());
    }
    catch (EXmlParserEOF &e) {
        file.close();
        err << "Parsing error: " << e.msg;
        throw ChunkException(err.str());
    }
    catch (EXmlTag &e) {
        file.close();
        err << "Parsing (tag) error: " << e.msg;
        throw ChunkException(err.str());
    }

    file.close();
}

/*****************************************************************************/

/**
   Fetches data.
*/

void Chunk::fetch_data(
        Time start,
        Time end,
        unsigned int min_values,
        DataCallback cb,
        void *cb_data, /**< arbitrary callback param */
        unsigned int decimation
        ) const
{
    if (!decimation) {
        stringstream err;
        err << "Decimation may not be zero!";
        throw ChunkException(err.str());
    }

    // The requested time range does not intersect the chunk's range.
    if (start > _end || end < _start) {
        return;
    }

    unsigned int level = _calc_optimal_level(start, end, min_values);
    unsigned int decimationCounter = 0;
    Data *data = NULL;
    Time limit = (min_values > 0) ? 2 * (end - start).to_int64() / min_values : 0;
    Time end_to_use = (end < _end) ? end : _end;
    Time time_per_value, last;

    while(1) {
        time_per_value = _time_per_value(level);

#ifdef DEBUG_DATA
        cerr << "l=" << level << flush;
#endif

        if (!level) {
            _fetch_level_data_wrapper(start, end, MetaGen, level,
                    time_per_value, &data, cb, cb_data,
                    decimation, decimationCounter, last);
        } else {
            _fetch_level_data_wrapper(start, end, MetaMin, level,
                    time_per_value, &data, cb, cb_data,
                    decimation, decimationCounter, last);
            _fetch_level_data_wrapper(start, end, MetaMax, level,
                    time_per_value, &data, cb, cb_data,
                    decimation, decimationCounter, last);
        }

        Time diff_to_end = end_to_use - last;

#ifdef DEBUG_DATA
        {
            Time z;
            cerr << endl << "diff=" << z.diff_str_to(diff_to_end)
                << " limit=" << z.diff_str_to(limit) << endl;
        }
#endif

        // fetch deeper level, if last data are not close enough
        // to chunk end
        if (level > 0 &&
                limit > (int64_t) 0 &&
                diff_to_end > (int64_t) 0 &&
                diff_to_end > limit) {
            level--;
            start = last;
            continue;
        }

        break;
    }
}

/*****************************************************************************/

/**
   Loads data from a specified meta level.
*/

void Chunk::_fetch_level_data_wrapper(Time start,
                                              Time end,
                                              MetaType meta_type,
                                              unsigned int level,
                                              Time time_per_value,
                                              Data **data,
                                              DataCallback cb,
                                              void *cb_data, /**< arbitrary
                                                               callback
                                                               parameter */
                                              unsigned int decimation,
                                              unsigned int &decimationCounter,
                                              Time &last
                                              ) const
{
    switch (_type) {
        case TCHAR:
            _fetch_level_data<char>(start, end, meta_type, level,
                    time_per_value, data, cb, cb_data, decimation,
                    decimationCounter, last);
            break;
        case TUCHAR:
            _fetch_level_data<unsigned char>(start, end, meta_type, level,
                    time_per_value, data, cb, cb_data, decimation,
                    decimationCounter, last);
            break;
        case TSHORT:
            _fetch_level_data<short>(start, end, meta_type, level,
                    time_per_value, data, cb, cb_data, decimation,
                    decimationCounter, last);
            break;
        case TUSHORT:
            _fetch_level_data<unsigned short>(start, end, meta_type, level,
                    time_per_value, data, cb, cb_data, decimation,
                    decimationCounter, last);
            break;
        case TINT:
            _fetch_level_data<int>(start, end, meta_type, level,
                    time_per_value, data, cb, cb_data, decimation,
                    decimationCounter, last);
            break;
        case TUINT:
            _fetch_level_data<unsigned int>(start, end, meta_type, level,
                    time_per_value, data, cb, cb_data, decimation,
                    decimationCounter, last);
            break;
        case TLINT:
            _fetch_level_data<long>(start, end, meta_type, level,
                    time_per_value, data, cb, cb_data, decimation,
                    decimationCounter, last);
            break;
        case TULINT:
            _fetch_level_data<unsigned long>(start, end, meta_type, level,
                    time_per_value, data, cb, cb_data, decimation,
                    decimationCounter, last);
            break;
        case TFLT:
            _fetch_level_data<float>(start, end, meta_type, level,
                    time_per_value, data, cb, cb_data, decimation,
                    decimationCounter, last);
            break;
        case TDBL:
            _fetch_level_data<double>(start, end, meta_type, level,
                    time_per_value, data, cb, cb_data, decimation,
                    decimationCounter, last);
            break;

        default: {
            stringstream err;
            err << "Unknown channel type " << _type << ".";
            throw ChunkException(err.str());
        }
    }
}

/*****************************************************************************/

/**
   Loads data.
*/

template <class T>
void Chunk::_fetch_level_data(Time start,
        Time end,
        MetaType meta_type,
        unsigned int level,
        Time time_per_value,
        Data **data,
        DataCallback cb,
        void *cb_data, /**< arbitrary callback
                         parameter */
        unsigned int decimation,
        unsigned int &decimationCounter,
        Time &last
        ) const
{
    stringstream level_dir_name;
    string global_index_file_name;
    stringstream data_file_name;
    IndexT<GlobalIndexRecord> global_index;
    GlobalIndexRecord global_index_record;
    IndexT<IndexRecord> index;
    IndexRecord index_record, next_index_record;
    File data_file;
    unsigned int i, index_row, blocks_read = 0;
    CompressionT<T> *comp;
    bool next_record_already_read = false;

    if (_format_index == FORMAT_ZLIB) {
        comp = new CompressionT_ZLib<T>();
    }
    else if (_format_index == FORMAT_MDCT) {
        if (typeid(T) == typeid(float)) {
            comp = (CompressionT<T> *)
                new CompressionT_MDCT<float>(_mdct_block_size, 0);
        }
        else if (typeid(T) == typeid(double)) {
            comp = (CompressionT<T> *)
                new CompressionT_MDCT<double>(_mdct_block_size, 0);
        }
        else {
            stringstream err;
            err << "ERROR: MDCT only for floating point types!";
            log(err.str());
            return;
        }
    }
    else if (_format_index == FORMAT_QUANT) {
        if (typeid(T) == typeid(float)) {
            comp = (CompressionT<T> *)
                new CompressionT_Quant<float>(0.0);
        }
        else if (typeid(T) == typeid(double)) {
            comp = (CompressionT<T> *)
                new CompressionT_Quant<double>(0.0);
        }
        else {
            stringstream err;
            err << "ERROR: Quant only for floating point types!";
            log(err.str());
            return;
        }
    }
    else {
        stringstream err;
        err << "ERROR: Unknown compression type index: "
             << _format_index;
        log(err.str());
        return;
    }

    level_dir_name << _dir << "/level" << level;
    global_index_file_name = level_dir_name.str() + "/data_"
        + meta_type_str(meta_type) + ".idx";

    try {
        global_index.open_read(global_index_file_name);
    } catch (EIndexT &e) {
        // global index not found.
        delete comp;
        return;
    }

    // loop through all indexed data files -- FIXME use binary search
    for (i = 0; i < global_index.record_count(); i++) {
        try {
            global_index_record = global_index[i];
        } catch (EIndexT &e) {
            stringstream err;
            err << "ERROR: Failed read record " << i
                 << " from global index \"";
            err << global_index_file_name << "\". Reason: " << e.msg;
            log(err.str());
            delete comp;
            return;
        }

        if (Time(global_index_record.end_time) < start
            && global_index_record.end_time != 0) {
            // this data file covers a time range before the requested range
            continue;
        }

        if (Time(global_index_record.start_time) > end) {
            // from here, all data files cover time ranges after
            // the requested range -> abort search
            break;
        }

        data_file_name.str("");
        data_file_name.clear();
        data_file_name << level_dir_name.str()
                       << "/data" << global_index_record.start_time
                       << "_" << meta_type_str(meta_type);

        string indexPath = data_file_name.str() + ".idx";
        try {
            index.open_read(indexPath);
            data_file.open_read(data_file_name.str().c_str());
        } catch (EIndexT &e) {
            stringstream err;
            err << "ERROR: Failed to open index \"";
            err << indexPath << "\": " << e.msg;
            log(err.str());
            delete comp;
            return;
        } catch (EFile &e) {
            stringstream err;
            err << "ERROR: Failed to open data file \"";
            err << indexPath << "\": " << e.msg;
            log(err.str());
            delete comp;
            return;
        }

        bool next_record_already_read = false;

        // loop through all index records -- FIXME use binary search!
        for (index_row = 0; index_row < index.record_count(); index_row++) {
            if (next_record_already_read) {
                index_record = next_index_record;
            }
            else {
                try {
                    index_record = index[index_row];
                } catch (EIndexT &e) {
                    stringstream err;
                    err << "ERROR: Could not read from index \"" << indexPath
                        << "\": " << e.msg;
                    log(err.str());
                    delete comp;
                    return;
                }
            }

            // the block covers a time range before the requested range.
            // try the next one.
            if (Time(index_record.end_time) < start) {
                continue;
            }

            // the following blocks cover time ranges after the requested
            // range. abort loading.
            if (Time(index_record.start_time) >= end) {
                break;
            }

            if (!_read_tag(index, index_row, index_record, next_index_record,
                        next_record_already_read, data_file, comp, meta_type,
                        level, time_per_value, data, cb, cb_data, decimation,
                        decimationCounter, last)) {
                delete comp;
                return;
            }

            blocks_read++;
        }
    }

    // blocks read, files still open

    if (blocks_read && _format_index == FORMAT_MDCT) {
        // read one more block! -- FIXME index_row valid?
        if (!_read_tag(index, index_row, index_record, next_index_record,
                    next_record_already_read, data_file, comp, meta_type,
                    level, time_per_value, data, cb, cb_data, decimation,
                    decimationCounter, last)) {
            delete comp;
            return;
        }
    }

    delete comp;
}

/*****************************************************************************/

/** Read one data tag.
 */
template <class T>
bool Chunk::_read_tag(
        IndexT<IndexRecord> &index,
        unsigned int index_row,
        IndexRecord &index_record,
        IndexRecord &next_index_record,
        bool &next_record_already_read,
        File &data_file,
        CompressionT<T> *comp,
        MetaType meta_type,
        unsigned int level,
        Time time_per_value,
        Data **data,
        DataCallback cb,
        void *cb_data, /**< arbitrary callback parameter */
        unsigned int decimation,
        unsigned int &decimationCounter,
        Time &last
        ) const
{
    size_t to_read, read_bytes;
    XmlParser xml;

    // determine data size to read
    if (index_row < index.record_count() - 1) {
        // there is a following index tag, so we can take the amount of
        // data to read from the index!
        try {
            next_index_record = index[index_row + 1];
        } catch (EIndexT &e) {
            stringstream err;
            err << "ERROR: Could not read from index \"" << index.path()
                << "\": " << e.msg;
            log(err.str());
            return false;
        }
        next_record_already_read = true;
        to_read = next_index_record.position - index_record.position;
    }
    else {
        // last index record, get size from data file
        try {
            size_t data_file_size = data_file.calc_size();
            to_read = data_file_size - index_record.position;
        } catch (EFile &e) {
            stringstream err;
            err << "ERROR: Could not seek in data file!";
            log(err.str());
            return false;
        }
    }

    // go to desired position in the data file
    try {
        data_file.seek(index_record.position);
    } catch (EFile &e) {
        stringstream err;
        err << "ERROR: Could not seek in data file!";
        log(err.str());
        return false;
    }

    string buffer;

    try {
        read_bytes = data_file.read(buffer, to_read);
    } catch (EFile &e) {
        stringstream err;
        err << "ERROR: Could not read from data file!";
        log(err.str());
        return false;
    }

    if (read_bytes != to_read) {
        stringstream err;
        err << "ERROR: EOF in \"" << data_file.path()
            << "\" after searching position "
            << index_record.position << "! Read " << read_bytes
            << " of " << to_read << ".";
        log(err.str());
        return false;
    }

    try {
        istringstream str(buffer);
        xml.parse(&str);
    } catch (EXmlParserEOF &e) {
        stringstream err;
        err << "EOF while parsing XML tag: " << e.msg;
        log(err.str());
        return false;
    } catch (EXmlParser &e) {
        stringstream err;
        err << "parsing error: " << e.msg;
        log(err.str());
        return false;
    }

    if (xml.tag()->title() == "d") {
        try {
            _process_data_tag(xml.tag(), index_record.start_time,
                    meta_type, level, time_per_value,
                    comp, data, cb, cb_data,
                    decimation, decimationCounter,
                    last);
        } catch (EXmlTag &e) {
            stringstream err;
            err << "ERROR: Could not read block: " << e.msg;
            log(err.str());
            return false;
        }
    }

    return true;
}

/*****************************************************************************/

/**
   Loads data from an XML tag.
*/

template <class T>
void Chunk::_process_data_tag(const XmlTag *tag,
        Time start_time,
        MetaType meta_type,
        unsigned int level,
        Time time_per_value,
        CompressionT<T> *comp,
        Data **data,
        DataCallback cb,
        void *cb_data, /**< arbitrary callback
                         parameter */
        unsigned int decimation,
        unsigned int &decimationCounter,
        Time &last
        ) const
{
    unsigned int block_size;
    const char *block_data;

    block_data = tag->att("d")->to_str().c_str();
    block_size = tag->att("s")->to_int();

    if (block_size) {
        try {
            comp->uncompress(block_data, strlen(block_data), block_size);
        } catch (ECompression &e) {
            stringstream err;
            err << "ERROR while uncompressing: " << e.msg;
            log(err.str());
            return;
        }

        if (!*data) *data = new Data();

        (*data)->import(start_time, time_per_value, meta_type, level,
                decimation, decimationCounter, comp->decompression_output(),
                comp->decompressed_length());

        if (comp->decompressed_length() > 0) {
            last = start_time +
                time_per_value * (comp->decompressed_length() - 1);
#ifdef DEBUG_DATA
            cerr << "." << flush;
#endif
        }

        // invoke data callback
        if (cb(*data, cb_data)) {
            // data structure adopted: forget its address.
            *data = NULL;
        }
        return;
    } else if (_format_index == FORMAT_MDCT) {
        try {
            comp->flush_uncompress(block_data, strlen(block_data));
        } catch (ECompression &e) {
            stringstream err;
            err << "ERROR while uncompressing: " << e.msg;
            log(err.str());
            return;
        }

        if (!*data) *data = new Data();

        (*data)->import(start_time, time_per_value, meta_type, level,
                decimation, decimationCounter, comp->decompression_output(),
                comp->decompressed_length());

        if (comp->decompressed_length() > 0) {
            last = start_time +
                time_per_value * (comp->decompressed_length() - 1);
#ifdef DEBUG_DATA
            cerr << "." << flush;
#endif
        }

        // invoke data callback
        if (cb(*data, cb_data)) {
            // data structure adopted: forget its address.
            *data = NULL;
        }
        return;
    }

    return;
}

/*****************************************************************************/

/**
   Holt den Zeitbereich des Chunks
*/

void Chunk::fetch_range()
{
    string global_index_file_name;
    stringstream err, index_file_name;
    IndexT<GlobalIndexRecord> global_index;
    GlobalIndexRecord first_global_index_record;
    GlobalIndexRecord last_global_index_record;
    IndexT<IndexRecord> index;
    IndexRecord index_record;

    _start = (uint64_t) 0;
    _end = (uint64_t) 0;
    _incomplete = true;

    global_index_file_name = _dir + "/level0/data_gen.idx";

    try
    {
        global_index.open_read(global_index_file_name);
    }
    catch (EIndexT &e)
    {
        err << "Opening global index: " << e.msg << endl;
        throw ChunkException(err.str());
    }

    if (global_index.record_count() == 0)
    {
        err << "Global index file \"" << global_index_file_name
            << "\" has no records!";
        throw ChunkException(err.str());
    }

    // Ersten und letzten Record lesen, um die Zeitspanne zu bestimmen
    try
    {
        first_global_index_record = global_index[0];
    }
    catch (EIndexT &e)
    {
        err << "Could not read first record of global index file \""
            << global_index_file_name << "\": " << e.msg;
        throw ChunkException(err.str());
    }

    unsigned int rec_idx = global_index.record_count() - 1;
    try
    {
        last_global_index_record = global_index[rec_idx];
    }
    catch (EIndexT &e)
    {
        err << "Could not read last record (" << rec_idx
            << ") of global index file \"" << global_index_file_name
            << "\": " << e.msg;
        throw ChunkException(err.str());
    }

    // In die letzte Datendatei wird noch erfasst
    // -> Die aktuelle, letzte Zeit aus dem Datendatei-Index holen
    if (last_global_index_record.end_time == 0)
    {
        index_file_name << _dir << "/level0/data"
                        << last_global_index_record.start_time << "_gen.idx";

        try
        {
            // Index öffnen
            index.open_read(index_file_name.str());
        }
        catch (EIndexT &e)
        {
            err << "Could not open index file \""
                << index_file_name.str() << "\": " << e.msg;
            throw ChunkException(err.str());
        }

        if (index.record_count() == 0)
        {
            err << "Index file \"" << index_file_name.str()
                << "\" has no records!";
            throw ChunkException(err.str());
        }

        unsigned int rec_idx = index.record_count() - 1;
        try
        {
            // Letzten Record lesen
            index_record = index[rec_idx];
        }
        catch (EIndexT &e)
        {
            stringstream err;
            err << "Could not read last (" << rec_idx
                << ") record from index file \""
                << index_file_name.str() << "\" (logging in progress): "
                << e.msg;
            throw ChunkException(err.str());
        }

        last_global_index_record.end_time = index_record.end_time;

        index.close();
    }
    else {
        // last global index record has time != 0
        _incomplete = false;
    }

    global_index.close();

    _start = first_global_index_record.start_time;
    _end = last_global_index_record.end_time;
}

/*****************************************************************************/

/**
   Berechnet die optimale Meta-Ebene für die angegebene Auflösung

   \param start Anfang des zu ladenden zeitbereichs
   \param end Ende des zu ladenden Zeitbereichs
   \param values_wanted Maximale Anzahl der zu ladenden Werte
*/

unsigned int Chunk::_calc_optimal_level(Time start,
                                        Time end,
                                        unsigned int min_values) const
{
    double level;

    if (!min_values) return 0;

    level = floor(log10(_sample_frequency
                        * (end - start).to_dbl_time()
                        / min_values)
                  / log10((double) _meta_reduction));

    if (level < 0) level = 0;

    return (unsigned int) level;
}

/*****************************************************************************/

/**
   Calculates the time per value (delta t) for the specified meta level.
*/

Time Chunk::_time_per_value(unsigned int level) const
{
    return (pow((double) _meta_reduction, (double) level)
            * 1000000.0 / _sample_frequency);
}

/*****************************************************************************/

/**
   Kleiner-Operator.

   \return true, wenn der linke Chunk eher beginnt, als der Rechte.
*/

bool Chunk::operator<(const Chunk &other) const
{
    return _start < other._start;
}

/*****************************************************************************/

/**
   == operator.
   \return true, if the chunks have exactly the same times.
*/

bool Chunk::operator==(const Chunk &other) const
{
    return _start == other._start && _end == other._end;
}

/*****************************************************************************/

void Chunk::set_chunk_info(DlsProto::ChunkInfo *chunk_info) const
{
    chunk_info->set_start(_start.to_uint64());
    chunk_info->set_end(_end.to_uint64());
    chunk_info->set_incomplete(_incomplete);
}

/*****************************************************************************/

void Chunk::update_from_chunk_info(const DlsProto::ChunkInfo &info)
{
    _start = info.start();
    _end = info.end();
    _incomplete = info.incomplete();
}

/*****************************************************************************/
