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

#include <fstream>
#include <sstream>
using namespace std;

/*****************************************************************************/

#include <pdcom/Variable.h>

/*****************************************************************************/

#include "lib/Base64.h"
#include "lib/XmlParser.h"
#include "lib/XmlTag.h"

#include "globals.h"
#include "Job.h"
#include "SaverGenT.h"
#include "Logger.h"

using namespace LibDLS;

/*****************************************************************************/

//#define DEBUG

/*****************************************************************************/

/**
   Kontruktor

   \param job Zeiger auf das besitzende Auftrags-Objekt
   \param channel_preset Kanalvorgaben
   \param dls_dir DLS-Datenverzeichnis
*/

Logger::Logger(
        Job *job,
        const ChannelPreset *channel_preset,
        const string &dls_dir,
        PdCom::Variable *pv
        ):
    _parent_job(job),
    _dls_dir(dls_dir),
    _var(NULL),
    _var_type(TUNKNOWN),
    _channel_preset(*channel_preset),
    _gen_saver(NULL),
    _data_size(0),
    _channel_dir_acquired(false),
    _chunk_created(false),
    _finished(true),
    _discard_data(false)
{
#ifdef DEBUG
    cerr << "Created logger " << this
        << " for " << channel_preset->name << endl;
#endif

    _subscribe(pv);
}

/*****************************************************************************/

/**
   Destruktor
*/

Logger::~Logger()
{
    if (_gen_saver) {
        delete _gen_saver;
    }

#ifdef DEBUG
    cerr << "Deleted logger " << this
        << " for " << _channel_preset.name << endl;
#endif
}

/*****************************************************************************/

/**
   Speichert wartende Daten

   Speichert alle Daten, die noch nicht im Dateisystem sind.

   \throw ELogger Fehler beim Speichern - Datenverlust!
*/

void Logger::finish()
{
    stringstream err;
    bool error = false;
#ifdef DEBUG
    Time start = Time::now();
#endif

    _unsubscribe();

    try {
        // Alle Daten speichern
        if (_gen_saver) {
            _gen_saver->flush();
        }
    }
    catch (ESaver &e) {
        error = true;
        err << "saver::flush(): " << e.msg;
    }

    // Chunk beenden
    _chunk_created = false;

    if (error) {
        throw ELogger(err.str());
    }

#ifdef DEBUG
    cout << "Logger::finish() for channel " << _real_channel.index
        << " took " << (Time::now() - start).to_dbl_time() << " s." << endl;
#endif

    _finished = true;
}

/*****************************************************************************/

/**
   Verwirft alle Daten und erstellt einen neuen Chunk

   Diese Methode löscht alle Daten im Speicher. Sie sollte
   nur mit Bedacht aufgerufen werden (z. B. in einem Zweig
   nach einem fork(), wobei der andere Zweig die Daten speichert).
*/

void Logger::discard()
{
    // Vorgeben, dass noch keine Daten geschrieben wurden
    _data_size = 0;

    // Neuen Saver erstellen (löscht vorher den alten Saver)
    _create_gen_saver();

    // Vorgeben, dass noch kein Chunk existiert
    _chunk_created = false;
}

/*****************************************************************************/

/**
   Erzeugt ein neues Chunk-Verzeichnis

   \param time_of_first Zeit des ersten Einzelwertes zur
   Generierung des Verzeichnisnamens
   \throw ELogger Fehler beim Erstellen des Verzeichnisses
*/

void Logger::create_chunk(Time time_of_first)
{
    stringstream dir_name, err;
    fstream file;
    XmlTag tag;
    string arch_str, file_name;

    _chunk_created = false;

    if (_channel_preset.format_index < 0
        || _channel_preset.format_index >= FORMAT_COUNT) {
        throw ELogger("Invalid channel format!");
    }

    switch (arch) {
        case LittleEndian: arch_str = "LittleEndian"; break;
        case BigEndian: arch_str = "BigEndian"; break;
        default: throw ELogger("Unknown architecture!");
    }

    // acquire channel directory
    if (!_channel_dir_acquired) {
        _acquire_channel_dir();
    }

    // create chunk directory
    dir_name << _channel_dir_name << "/chunk" << time_of_first;
    _chunk_dir_name = dir_name.str();
    if (mkdir(_chunk_dir_name.c_str(), 0755)) {
        err << "Failed to create chunk directory \"" << _chunk_dir_name
            << "\": " << strerror(errno);
        throw ELogger(err.str());
    }

    // create chunk.xml
    file_name = dir_name.str() + "/chunk.xml";
    file.open(file_name.c_str(), ios::out);
    if (!file) {
        err << "Failed to create \"" << file_name << "\": "
            << strerror(errno);
        throw ELogger(err.str());
    }

    tag.clear();
    tag.title("dlschunk");
    tag.type(dxttBegin);
    file << tag.tag() << endl;

    tag.clear();
    tag.title("chunk");
    tag.push_att("sample_frequency", _channel_preset.sample_frequency);
    tag.push_att("block_size", _channel_preset.block_size);
    tag.push_att("meta_mask", _channel_preset.meta_mask);
    tag.push_att("meta_reduction", _channel_preset.meta_reduction);
    tag.push_att("format", format_strings[_channel_preset.format_index]);

    if (_channel_preset.format_index == FORMAT_MDCT) {
        tag.push_att("mdct_block_size", _channel_preset.mdct_block_size);
        tag.push_att("mdct_accuracy", _channel_preset.accuracy);
    }
    else if (_channel_preset.format_index == FORMAT_QUANT) {
        tag.push_att("accuracy", _channel_preset.accuracy);
    }

    tag.push_att("architecture", arch_str);

    file << " " << tag.tag() << endl;

    tag.clear();
    tag.title("dlschunk");
    tag.type(dxttEnd);
    file << tag.tag() << endl;

    file.close();
    _chunk_created = true;
}

/*****************************************************************************/

/**
 * Searches for a matching channel directory to store data.
 * If no matching directory is found, a new one is created.
 * \throw ELogger Failed to create directory.
 */

void Logger::_acquire_channel_dir()
{
    stringstream job_dir_name, err, index_stream;
    DIR *dir;
    struct dirent *dir_ent;
    string entry_name, channel_dir_name, file_name;
    ofstream file;
    unsigned int index, highest_index = 0;
    XmlTag tag;

    job_dir_name << _dls_dir << "/job" << _parent_job->id();

    if (!(dir = opendir(job_dir_name.str().c_str()))) {
        err << "Failed to open job directory \""
            << job_dir_name.str() << "\": " << strerror(errno);
        throw ELogger(err.str());
    }

    while ((dir_ent = readdir(dir))) {
        entry_name = dir_ent->d_name;

        if (entry_name.size() < 8) // size of "channelX"
            continue;

        if (entry_name.substr(0, 7) != "channel")
            continue;

        // only continue, if remaining characters are all numbers
        if (entry_name.find_first_not_of("0123456789", 7) != string::npos)
            continue;

        index_stream.clear();
        index_stream.str("");
        index_stream << entry_name.substr(7);
        index_stream >> index;
        if (index > highest_index) highest_index = index;

        channel_dir_name = job_dir_name.str() + "/" + entry_name;

        try {
            if (_channel_dir_matches(channel_dir_name)) {
                _channel_dir_name = channel_dir_name;
                break;
            }
        }
        catch (ELogger &e) {
            continue;
        }
    }

    closedir(dir);

    if (_channel_dir_name != "") { // found a matching directory
#ifdef DEBUG
        cerr << "Using existing " << _channel_dir_name << endl;
#endif
        return;
    }

    index_stream.clear();
    index_stream.str("");
    index_stream << (highest_index + 1);
    channel_dir_name = job_dir_name.str() + "/channel" + index_stream.str();

#ifdef DEBUG
    cerr << "Creating " << channel_dir_name << endl;
#endif

    if (mkdir(channel_dir_name.c_str(), 0755)) {
        err << "Failed to create channel directory \""
            << channel_dir_name << "\": " << strerror(errno);
        throw ELogger(err.str());
    }

    // create channel.xml
    file_name = channel_dir_name + "/channel.xml";
    file.open(file_name.c_str(), ios::out);
    if (!file) {
        err << "Failed to create \"" << file_name
            << "\": " << strerror(errno);
        throw ELogger(err.str());
    }

    tag.clear();
    tag.title("dlschannel");
    tag.type(dxttBegin);
    file << tag.tag() << endl;

    tag.clear();
    tag.title("channel");
    tag.push_att("name", _channel_preset.name);
    tag.push_att("unit", ""); // keep for backward compability
    tag.push_att("type", channel_type_to_str(_var_type));
    file << " " << tag.tag() << endl;

    tag.clear();
    tag.title("dlschannel");
    tag.type(dxttEnd);
    file << tag.tag() << endl;

    file.close();

    _channel_dir_name = channel_dir_name;
}

/*****************************************************************************/

/**
 * Checks the given channel directory can be used for the current job.
 * \return non-zero, if the directory matches
 * \throw ELogger Failed to check.
 */

int Logger::_channel_dir_matches(const string &dir_name) const
{
    stringstream err;
    string channel_file_name;
    fstream channel_file;
    struct stat stat_buf;
    XmlParser xml;
    XmlTag channel_tag;

    if (stat(dir_name.c_str(), &stat_buf) == -1) {
        err << "Failed to stat() \"" << dir_name << "\": "
            << strerror(errno);
        throw ELogger(err.str());
    }

    if (!S_ISDIR(stat_buf.st_mode)) {
        err << "\"" << dir_name << "\" is not a directory!";
        throw ELogger(err.str());
    }

    channel_file_name = dir_name + "/channel.xml";
    if (lstat(channel_file_name.c_str(), &stat_buf) == -1) {
        err << "Failed to stat() \"" << channel_file_name << "\": "
            << strerror(errno);
        throw ELogger(err.str());
    }

    channel_file.open(channel_file_name.c_str(), ios::in);
    if (!channel_file) {
        err << "Failed to open() \"" << channel_file_name << "\": "
            << strerror(errno);
        throw ELogger(err.str());
    }

    try {
        xml.parse(&channel_file, "dlschannel", dxttBegin);
        channel_tag = *xml.parse(&channel_file, "channel", dxttSingle);
        xml.parse(&channel_file, "dlschannel", dxttEnd);
    }
    catch (EXmlParser &e) {
        err << "Parsing \"" << channel_file_name << "\": " << e.msg
            << " tag: " << e.tag;
        throw ELogger(err.str());
    }

    if (channel_tag.att("name")->to_str() != _channel_preset.name) {
        return 0;
    }

#if 0
    if (channel_tag.att("unit")->to_str() != _real_channel.unit)
        return 0;
#endif

    if (channel_tag.att("type")->to_str()
            != channel_type_to_str(_var_type)) {
        return 0;
    }

    return 1;
}

/*****************************************************************************/

/** Creates the generic saver object.
 */
void Logger::_create_gen_saver()
{
    if (_gen_saver) {
        delete _gen_saver;
        _gen_saver = NULL;
    }

    try {
        switch (_var_type) {
            case TCHAR:
                _gen_saver = new SaverGenT<char>(this);
                break;
            case TUCHAR:
                _gen_saver = new SaverGenT<unsigned char>(this);
                break;
            case TSHORT:
                _gen_saver = new SaverGenT<short int>(this);
                break;
            case TUSHORT:
                _gen_saver = new SaverGenT<unsigned short int>(this);
                break;
            case TINT:
                _gen_saver = new SaverGenT<int>(this);
                break;
            case TUINT:
                _gen_saver = new SaverGenT<unsigned int>(this);
                break;
            case TLINT:
                _gen_saver = new SaverGenT<long>(this);
                break;
            case TULINT:
                _gen_saver = new SaverGenT<unsigned long>(this);
                break;
            case TFLT:
                _gen_saver = new SaverGenT<float>(this);
                break;
            case TDBL:
                _gen_saver = new SaverGenT<double>(this);
                break;

            default:
                throw ELogger("Unknown data type!");
        }
    }
    catch (ESaver &e) {
        throw ELogger("Constructing new saver: " + e.msg);
    }
    catch (...) {
        throw ELogger("Out of memory while constructing saver!");
    }

    if (_channel_preset.meta_mask & MetaMean) {
        _gen_saver->add_meta_saver(MetaMean);
    }
    if (_channel_preset.meta_mask & MetaMin) {
        _gen_saver->add_meta_saver(MetaMin);
    }
    if (_channel_preset.meta_mask & MetaMax) {
        _gen_saver->add_meta_saver(MetaMax);
    }
}

/*****************************************************************************/

/** Subscribes to the process variable.
 */
void Logger::_subscribe(PdCom::Variable *pv)
{
    switch(pv->type) {
        case PdCom::Data::bool_T:
        case PdCom::Data::uint8_T:
            _var_type = TUCHAR;
            break;
        case PdCom::Data::sint8_T:
            _var_type = TCHAR;
            break;
        case PdCom::Data::uint16_T:
            _var_type = TUSHORT;
            break;
        case PdCom::Data::sint16_T:
            _var_type = TSHORT;
            break;
        case PdCom::Data::uint32_T:
            _var_type = TUINT;
            break;
        case PdCom::Data::sint32_T:
            _var_type = TINT;
            break;
        case PdCom::Data::single_T:
            _var_type = TFLT;
            break;
        case PdCom::Data::double_T:
            _var_type = TDBL;
            break;
        case PdCom::Data::uint64_T:
        case PdCom::Data::sint64_T:
        default:
            stringstream err;
            err << "Channel \"" << _channel_preset.name
                << "\" has invalid type " << pv->type << "!";
            throw ELogger(err.str());
    }

    if (_channel_preset.sample_frequency <= 0.0) {
        stringstream err;
        err << "Channel \"" << _channel_preset.name << "\": "
            << "Invalid sample frequency "
            << _channel_preset.sample_frequency << "!";
        throw ELogger(err.str());
    }

    if (_channel_preset.format_index == FORMAT_MDCT
            && _var_type != TFLT
            && _var_type != TDBL) {
        stringstream err;
        err << "MDCT compression only for floating point channels!";
        throw ELogger(err.str());
    }

    _create_gen_saver();

    double period = 1.0 / _channel_preset.sample_frequency;

    if (period <= 0.0) {
        stringstream err;
        err << "Invalid period " << period;
        throw ELogger(err.str());
    }

    try {
        pv->subscribe(this, period);
    }
    catch (PdCom::Exception &e) {
        stringstream err;
        err << "Subscription failed: " << e.what();
        throw ELogger(err.str());
    }

    _var = pv;
    _discard_data = false;
}

/*****************************************************************************/

/** Unsubscribes from the process variable.
 */
void Logger::_unsubscribe()
{
    if (_var) {
        _var->unsubscribe(this);
        _var = NULL;
    }
}

/*****************************************************************************/

void Logger::notify(PdCom::Variable *pv)
{
    if (_discard_data) {
        return;
    }

    Time t;
    t.from_dbl_time(pv->getMTime());

#if 0
    double val;
    pv->getValue(&val, 1);
    cout << t.to_dbl_time() << "     " << val << endl;
#endif

    try {
        _gen_saver->process_one(pv->getDataPtr(), t);
    }
    catch (ESaver &e) {
        /* PdCom does not like, if exceptions are thrown in notify context.
         * Therefore notify the logger process by calling a method. */
        _parent_job->notify_error(E_DLS_ERROR_RESTART);
        _discard_data = true;
        msg() << e.msg;
        log(Error);
        return;
    }
    catch (ETimeTolerance &e) {
        _parent_job->notify_error(E_DLS_ERROR_RESTART);
        _discard_data = true;
        msg() << e.msg;
        log(Error);
        return;
    }

    if (!_finished) {
        // now something to finish
        _finished = false;
    }

    _parent_job->notify_data();
}

/***************************************************************************/

void Logger::notifyDelete(PdCom::Variable *pv)
{
#ifdef DEBUG
    cout << __func__ << endl;
#endif

    if (_var && _var == pv) {
        _var = NULL;
    }
}

/*****************************************************************************/
