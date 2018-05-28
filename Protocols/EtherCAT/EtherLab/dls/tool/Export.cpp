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

#ifdef __unix__
#include <sys/ioctl.h>
#endif

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <set>
using namespace std;

#include "lib/LibDLS/Dir.h"
#include "lib/LibDLS/Export.h"
using namespace LibDLS;

/*****************************************************************************/

extern unsigned int sig_int_term;

static string dls_dir_path;
static string dls_export_dir;
static string dls_export_format;
static unsigned int job_id = 0;
static list<unsigned int> channel_indices; // FIXME replace with set
static set<string> paths;
static Time start_time;
static Time end_time;
static unsigned int decimation = 1;
static bool export_ascii = false;
static bool export_matlab = false;
static bool quiet = false;
static bool export_messages = false;
static string message_lang;

static unsigned int term_width;

/*****************************************************************************/

typedef struct
{
    list<Export *> *exporters;
    Time start;
    double channel_percentage;
    double channel_factor;
}
ExportInfo;

/*****************************************************************************/

struct ExportChannel {
    Channel *channel;
    string filename;
};

/*****************************************************************************/

int export_data_callback(Data *, void *);
void draw_progress(double percentage);
void export_get_environment();
void export_get_options(int, char **);
void export_print_usage();
int terminal_width();

/*****************************************************************************/

int export_main(int argc, char *argv[])
{
    Directory dls_dir;
    Job *job;
    list<unsigned int>::iterator index_i;
    list<ExportChannel> channels;
    list<Channel>::iterator job_channel_i;
    Channel *channel;
    Time channels_start, channels_end, now;
    list<Export *> exporters;
    list<Export *>::iterator exp_i;
    ExportInfo info;
    unsigned int current_channel, total_channels;
    stringstream info_file_name;
    ofstream info_file;
    int ret;

    now.set_now();

    export_get_environment();
    export_get_options(argc, argv);

    if (!quiet) {
        term_width = terminal_width();
    }

    if (export_ascii) {
        exporters.push_back(new ExportAscii());
    }
    if (export_matlab) {
        exporters.push_back(new ExportMat4());
    }

    if (exporters.empty()) {
        cerr << "ERROR: No exporters active! Enable at least one." << endl;
        export_print_usage();
        exit(1);
    }

    try {
        dls_dir.set_uri(dls_dir_path);
    } catch (DirectoryException &e) {
        cerr << "ERROR: Passing URI: " << e.msg << endl;
        exit(1);
    }

    try {
        dls_dir.import();
    } catch (DirectoryException &e) {
        cerr << "ERROR: Importing DLS directory: " << e.msg << endl;
        exit(1);
    }

    if (!(job = dls_dir.find_job(job_id))) {
        cerr << "ERROR: No such job - " << job_id << "." << endl;
        cerr << "Call \"dls list\" to list available jobs." << endl;
        exit(1);
    }

    try {
        job->fetch_channels();
    } catch (JobException &e) {
        cerr << "ERROR: Fetching channels: " << e.msg << endl;
        exit(1);
    }

    for (index_i = channel_indices.begin();
         index_i != channel_indices.end();
         index_i++) {
        if (!(channel = job->find_channel(*index_i))) {
            cerr << "ERROR: No such channel - " << *index_i << "." << endl;
            cerr << "Call \"dls list -j " << job_id
                 << "\" to list available channels." << endl;
            exit(1);
        }
        ExportChannel ec;
        ec.channel = channel;
        stringstream str;
        str << "channel" << *index_i;
        ec.filename = str.str();
        channels.push_back(ec);
    }

    for (set<string>::const_iterator path_i = paths.begin();
         path_i != paths.end(); path_i++) {

        string name;
        string filename;
        string::size_type idx = path_i->find(':');
        if (idx == string::npos) { // no colon found
            name = *path_i;
        }
        else {
            filename = path_i->substr(0, idx);
            name = path_i->substr(idx + 1);
        }

        set<Channel *> chs = job->find_channels_by_name(name);

        if (chs.empty()) {
            cerr << "ERROR: No such channel - " << *path_i << "." << endl;
            cerr << "Call \"dls list -j " << job_id
                 << "\" to list available channels." << endl;
            exit(1);
        }

        if (chs.size() > 1) {
            cerr << "ERROR: Channel " << *path_i << " is not unique." << endl;
            cerr << "Call \"dls list -j " << job_id
                 << "\" to list available channels." << endl;
            exit(1);
        }

        ExportChannel ec;
        set<Channel *>::iterator ch_i = chs.begin();
        ec.channel = *ch_i;
        if (filename.empty()) {
            stringstream str;
            str << "channel" << ec.channel->dir_index();
            ec.filename = str.str();
        }
        else {
            ec.filename = filename;
        }
        channels.push_back(ec);
    }

    if (channel_indices.empty() && paths.empty()) {
        for (job_channel_i = job->channels().begin();
             job_channel_i != job->channels().end();
             job_channel_i++) {
            ExportChannel ec;
            ec.channel = &(*job_channel_i);
            stringstream str;
            str << "channel" << job_channel_i->dir_index();
            ec.filename = str.str();
            channels.push_back(ec);
        }
    }

    if (channels.empty()) {
        cerr << "ERROR: No channels to export." << endl;
        exit(1);
    }

    cout << "Channels to export:" << endl;
    for (list<ExportChannel>::const_iterator channel_i = channels.begin();
         channel_i != channels.end();
         channel_i++) {
        channel = channel_i->channel;
        cout << "  (" << channel->dir_index() << ") "
            << channel->name() << endl;

        try {
            channel->fetch_chunks();
        } catch (ChannelException &e) {
            cerr << "ERROR: Fetching chunks: " << e.msg << endl;
            exit(1);
        }

        if (!channel->start().is_null()) {
            if (channels_start.is_null()) {
                channels_start = channel->start();
            } else if (channel->start() < channels_start) {
                channels_start = channel->start();
            }
        }

        if (!channel->end().is_null()) {
            if (channels_end.is_null()) {
                channels_end = channel->end();
            } else if (channel->end() > channels_end) {
                channels_end = channel->end();
            }
        }
    }

    if (start_time.is_null()) start_time = channels_start;
    if (end_time.is_null()) end_time = channels_end;

    cout << "Start time: " << start_time.to_real_time() << endl
         << "  End time: " << end_time.to_real_time() << endl
         << "  duration: " << start_time.diff_str_to(end_time) << endl;

    if (start_time >= end_time) {
        cerr << "ERROR: Invalid time range!" << endl;
        exit(1);
    }

    dls_export_dir += "/" + now.format_time(dls_export_format.c_str());

    cout << "Exporting to \"" << dls_export_dir << "\" ..." << endl;

    // create unique directory
#ifdef __unix__
    ret = mkdir(dls_export_dir.c_str(), 0755);
#else
    ret = mkdir(dls_export_dir.c_str());
#endif

    if (ret) {
        cerr << "ERROR: Failed to create export directory: ";
        cerr << strerror(errno) << endl;
        exit(1);
    }

    if (start_time < channels_start) {
        start_time = channels_start;
    }
    if (end_time > channels_end) {
        end_time = channels_end;
    }

    current_channel = 0;
    total_channels = channels.size();

    info.exporters = &exporters;
    info.start = start_time;
    info.channel_percentage = 0.0;
    info.channel_factor = 100.0 / total_channels
        / (end_time - start_time).to_dbl();

    // actual exporting
    for (list<ExportChannel>::const_iterator channel_i = channels.begin();
         channel_i != channels.end(); channel_i++) {
        channel = channel_i->channel;

        try {
            for (exp_i = exporters.begin();
                    exp_i != exporters.end(); exp_i++) {
                (*exp_i)->begin(*channel, dls_export_dir,
                        channel_i->filename);
            }
        } catch (ExportException &e) {
            cerr << "ERROR: Beginning export file: " << e.msg << endl;
            exit(1);
        }

        try {
            channel->fetch_data(start_time, end_time,
                                0, export_data_callback, &info, decimation);
        } catch (ChannelException &e) {
            cerr << "ERROR: Fetching data: " << e.msg << endl;
            exit(1);
        }

        try {
            for (exp_i = exporters.begin(); exp_i != exporters.end(); exp_i++)
                (*exp_i)->end();
        } catch (ExportException &e) {
            cerr << "ERROR: Finishing export file: " << e.msg << endl;
            exit(1);
        }

        current_channel++;

        if (!quiet) {
            // display progress
            info.channel_percentage = 100.0 * current_channel / total_channels;
            draw_progress(info.channel_percentage);
        }
    }

    cerr << endl;

    if (export_messages) {
        list<LibDLS::Job::Message> msgs;

        cerr << "Exporting messages... " << flush;

        try {
            msgs = job->load_msg(start_time, end_time, message_lang);
        }
        catch (LibDLS::Exception &e) {
            cerr << "failed!" << endl
                << "ERROR: Message export failed: " << e.msg << endl;
            exit(1);
        }

        cerr << "done." << endl;

        stringstream msg_file_name;
        msg_file_name << dls_export_dir << "/messages.txt";

        ofstream msg_file;
        msg_file.open(msg_file_name.str().c_str(), ios::trunc);
        if (!msg_file.is_open()) {
            cerr << "ERROR: Failed to write \""
                << msg_file_name.str() << "\"!" << endl;
            exit(1);
        }

        for (list<LibDLS::Job::Message>::const_iterator msg = msgs.begin();
                msg != msgs.end(); msg++) {
            msg_file << msg->time.to_iso_time() << " "
                << msg->type_str() << " "
                << msg->text << endl;
        }

        msg_file.close();
    }

    // create info file
    info_file_name << dls_export_dir << "/dls_export_info";
    info_file.open(info_file_name.str().c_str(), ios::trunc);

    if (!info_file.is_open()) {
        cerr << "ERROR: Failed to write \""
             << info_file_name.str() << "\"!" << endl;
        exit(1);
    }

    info_file << endl
              << "This is a DLS export directory." << endl << endl
              << "Exported on: "
              << now.to_rfc811_time() << endl << endl
              << "Exported range from: "
              << start_time.to_real_time() << endl
              << "                 to: "
              << end_time.to_real_time() << endl
              << "           duration: "
              << start_time.diff_str_to(end_time) << endl << endl;

    for (list<ExportChannel>::const_iterator channel_i = channels.begin();
         channel_i != channels.end(); channel_i++) {
        channel = channel_i->channel;
        info_file << channel_i->filename << ": " << channel->name() << endl;
    }

    info_file << endl;

    info_file.close();

    cout << "Export finished." << endl;
    return 0;
}

/*****************************************************************************/

int export_data_callback(Data *data, void *cb_data)
{
    ExportInfo *info = (ExportInfo *) cb_data;

    list<Export *>::iterator exp_i;
    double diff_time;
    double percentage;

    for (exp_i = info->exporters->begin();
         exp_i != info->exporters->end();
         exp_i++)
        (*exp_i)->data(data);

    if (!quiet) {
        // display progress
        diff_time = (data->end_time() - info->start).to_dbl();
        percentage = info->channel_percentage +
            diff_time * info->channel_factor;
        draw_progress(percentage + 0.5);
    }

    return 0; // not adopted
}

/*****************************************************************************/

void draw_progress(double percentage)
{
    static unsigned int number = 0;
    static unsigned int blocks = 0;
    unsigned int new_number, new_blocks, i;

    if (percentage > 100.0) {
        percentage = 100.0;
    }

    new_number = (int) (percentage + 0.5);
    new_blocks = (int) (percentage * (term_width - 9) / 100.0);

    if (new_number != number || new_blocks != blocks) {
        number = new_number;
        blocks = new_blocks;

        cout << "\r " << setw(3) << number << "% [";
        for (i = 0; i < blocks; i++) cout << "=";
        for (i = blocks; i < (term_width - 9); i++) cout << " ";
        cout << "]";
        cout.flush();
    }
}

/*****************************************************************************/

list<unsigned int> parse_intlist(const char *intlist_str)
{
    unsigned int index, last_index, i;
    const char *current;
    char *remainder;
    list<unsigned int> numbers;
    bool range;

    if (!strlen(intlist_str)) return numbers;

    current = intlist_str;

    range = false;
    last_index = 0;

    while (1) {
        while (current[0] == ' ') current++;

        index = strtoul(current, &remainder, 10);

        if (remainder == current) {
            stringstream err;
            err << "Invalid numeric character '" << remainder[0]
                << "' in \"" << intlist_str << "\".";
            throw err.str();
        }

        if (range) {
            if (last_index < index) {
                for (i = last_index; i <= index; i++)
                    numbers.push_back(i);
            }
            else {
                for (i = last_index; i >= index; i--)
                    numbers.push_back(i);
            }
        }
        else {
            numbers.push_back(index);
        }

        if (!strlen(remainder)) break;

        while (remainder[0] == ' ') remainder++;

        if (remainder[0] == '-') {
            last_index = index;
            range = true;
        }
        else {
            if (remainder[0] != ',') {
                stringstream err;
                err << "Invalid separator '" << remainder[0]
                    << "' in \"" << intlist_str << "\".";
                throw err.str();
            }
            range = false;
        }

        current = remainder + 1;
    }

    numbers.sort();
    numbers.unique();

    return numbers;
}

/*****************************************************************************/

Time parse_time(const char *time_str)
{
    struct tm t;
    unsigned int usec;
    const char *current;
    char *remainder;
    stringstream err;

    if (!strlen(time_str)) return Time();

    t.tm_year = 0;
    t.tm_mon = 0;
    t.tm_mday = 1;
    t.tm_hour = 0;
    t.tm_min = 0;
    t.tm_sec = 0;
    t.tm_isdst = -1;
    usec = 0;

    current = time_str;

    t.tm_year = strtoul(current, &remainder, 10) - 1900;
    if (remainder == current) goto num_error;
    if (!strlen(remainder)) goto mktime;
    if (remainder[0] != '-') goto sep_error;
    current = remainder + 1;

    t.tm_mon = strtoul(current, &remainder, 10) - 1;
    if (remainder == current) goto num_error;
    if (!strlen(remainder)) goto mktime;
    if (remainder[0] != '-') goto sep_error;
    current = remainder + 1;

    t.tm_mday = strtoul(current, &remainder, 10);
    if (remainder == current) goto num_error;
    if (!strlen(remainder)) goto mktime;
    if (remainder[0] != '-' && remainder[0] != ' ') goto sep_error;
    current = remainder + 1;

    t.tm_hour = strtoul(current, &remainder, 10);
    if (remainder == current) goto num_error;
    if (!strlen(remainder)) goto mktime;
    if (remainder[0] != '-' && remainder[0] != ':') goto sep_error;
    current = remainder + 1;

    t.tm_min = strtoul(current, &remainder, 10);
    if (remainder == current) goto num_error;
    if (!strlen(remainder)) goto mktime;
    if (remainder[0] != '-' && remainder[0] != ':') goto sep_error;
    current = remainder + 1;

    t.tm_sec = strtoul(current, &remainder, 10);
    if (remainder == current) goto num_error;
    if (!strlen(remainder)) goto mktime;
    if (remainder[0] != '-' && remainder[0] != '.' && remainder[0] != ',')
        goto sep_error;
    current = remainder + 1;

    usec = strtoul(current, &remainder, 10);
    if (remainder == current || strlen(remainder)) goto num_error;

 mktime:
    return Time(&t, usec);

 num_error:
    err << "Invalid numeric character '" << remainder[0]
        << "' in \"" << time_str << "\"!";
    throw err.str();

 sep_error:
    err << "Invalid separator '" << remainder[0]
        << "' in \"" << time_str << "\"!";
    throw err.str();
}

/*****************************************************************************/

void export_get_environment()
{
    char *env;

    if ((env = getenv("DLS_DIR")))
        dls_dir_path = env;

    if ((env = getenv("DLS_EXPORT")))
        dls_export_dir = env;

    if ((env = getenv("DLS_EXPORT_FMT")))
        dls_export_format = env;
}

/*****************************************************************************/

void export_get_options(int argc, char *argv[])
{
    int c;

    while (1) {
        if ((c = getopt(argc, argv, "d:o:f:amj:c:p:s:e:n:qhgl:")) == -1) {
            break;
        }

        switch (c) {
            case 'd':
                dls_dir_path = optarg;
                break;

            case 'o':
                dls_export_dir = optarg;
                break;

            case 'f':
                dls_export_format = optarg;
                break;

            case 'a':
                export_ascii = true;
                break;

            case 'm':
                export_matlab = true;
                break;

            case 'j':
                job_id = strtoul(optarg, NULL, 10);
                break;

            case 'c':
                try {
                    channel_indices = parse_intlist(optarg);
                }
                catch (string &e) {
                    cerr << "ERROR: Parsing channels argument failed: "
                         << e << endl;
                    exit(1);
                }
                break;

            case 'p':
                paths.insert(optarg);
                break;

            case 's':
                try {
                    start_time = parse_time(optarg);
                }
                catch (string &e) {
                    cerr << "ERROR: Parsing start-time argument failed: "
                         << e << endl;
                    exit(1);
                }
                break;

            case 'e':
                try {
                    end_time = parse_time(optarg);
                }
                catch (string &e) {
                    cerr << "ERROR: Parsing end-time argument failed: "
                         << e << endl;
                    exit(1);
                }
                break;

            case 'n':
                decimation = strtoul(optarg, NULL, 10);
                break;

            case 'q':
                quiet = true;
                break;

            case 'g':
                export_messages = true;
                break;

            case 'l':
                message_lang = optarg;
                break;

            case 'h':
                export_print_usage();
                exit(0);

            default:
                export_print_usage();
                exit(1);
        }
    }

    if (optind < argc) {
        cerr << "Extra parameter given!" << endl;
        export_print_usage();
        exit(1);
    }

    if (dls_dir_path == "") {
        cerr << "No DLS data directory specified!" << endl;
        export_print_usage();
        exit(1);
    }

    if (!job_id) {
        cerr << "You must specify a job ID!" << endl;
        cerr << "Call \"dls list\" to list available jobs." << endl;
        export_print_usage();
        exit(1);
    }

    if (dls_export_dir == "") {
        dls_export_dir = ".";
    }

    if (dls_export_format == "") {
        dls_export_format = "dls-export-%Y-%m-%d-%H-%M-%S";
    }
}

/*****************************************************************************/

void export_print_usage()
{
    cout << "Usage: dls export [OPTIONS]" << endl
         << "Options:" << endl
         << "   -d DIR         DLS data directory."
         << " Default: $DLS_DIR" << endl
         << "   -o DIR         Output directory."
         << " Default: $DLS_EXPORT_DIR or \".\"" << endl
         << "   -f NAMEFMT     Naming format for export directory." << endl
         << "                  See strftime(3)." << endl
         << "                  Default: $DLS_EXPORT_FMT"
         << " or \"dls-export-%Y-%m-%d-%H-%M-%S\"" << endl
         << "   -a             Enable ASCII exporter" << endl
         << "   -m             Enable MATLAB4 exporter" << endl
         << "   -j ID          Job to export (MANDATORY)" << endl
         << "   -c CHANNELS    Indices of channels to export"
         << " (see below)." << endl
         << "                  Default: All channels" << endl
         << "   -p CHANNEL     Path of one channel to export (see" << endl
         << "                  below). This option may appear" << endl
         << "                  multiple times. Default: All channels." << endl
         << "   -s TIMESTAMP   Start time (see below)."
         << " Default: Start of recording"
         << endl
         << "   -e TIMESTAMP   End time (see below)."
         << " Default: End of recording"
         << endl
         << "   -n DECIMATION  Export every n'th value." << endl
         << "   -g             Export messages." << endl
         << "   -l LANGUAGE    2-character language code for messages." << endl
         << "   -q             Be quiet (no progress bar)" << endl
         << "   -h             Print this help" << endl
         << "CHANNELS is a comma-separated list of channel indices." << endl
         << "   Use the minus sign to specify ranges." << endl
         << "   Examples: \"2,4,9\", \"1-20\", \"2,4,13-15,42\"." << endl
         << "CHANNEL is a signal name, optionally prefixed with" << endl
         << "   'FILE:', where FILE is the name of the exported" << endl
         << "   channel data file. If FILE is empty, or there is no" << endl
         << "   colon found, files are named according to the channel" << endl
         << "   indices." << endl
         << "TIMESTAMP is a broken-down time with microsecond resolution:"
         << endl
         << "   YYYY[-MM[-DD[-HH[-MM[-SS[-UUUUUU]]]]]] or" << endl
         << "   YYYY[-MM[-SS[ HH[:MM[:SS[.UUUUUU]]]]]]." << endl
         << "   Examples: \"2006-08\", \"2005-08-15 13:14:58.896366\""
         << endl;
}

/*****************************************************************************/

int terminal_width()
{
#ifdef TIOCGWINSZ
    struct winsize w;

    w.ws_col = 0;
    ioctl(1, TIOCGWINSZ, &w);

    if (w.ws_col)
        return w.ws_col;
    else
        return 50;
#else
    return 50;
#endif
}

/*****************************************************************************/
