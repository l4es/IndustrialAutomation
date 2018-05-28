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

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

#include <FL/Fl.H>
#include <FL/Fl_File_Chooser.H>

/*****************************************************************************/

#include "ViewGlobals.h"
#include "ViewDialogExport.h"

using namespace LibDLS;

/*****************************************************************************/

#define WIDTH 500
#define HEIGHT 285

/*****************************************************************************/

typedef struct
{
    ViewDialogExport *dialog;
    double channel_percentage;
    double channel_factor;
}
ExportInfo;

/*****************************************************************************/

/**
   Konstruktor
*/

ViewDialogExport::ViewDialogExport(const string &dls_dir
                                   /**< DLS-Datenverzeichnis */
                                   )
{
    int x = Fl::w() / 2 - WIDTH / 2;
    int y = Fl::h() / 2 - HEIGHT / 2;

    _dls_dir = dls_dir;

    _wnd = new Fl_Double_Window(x, y, WIDTH, HEIGHT, "Ansicht Exportieren");
    _wnd->callback(_callback, this);
    _wnd->set_modal();

    _box_info = new Fl_Box(10, 10, WIDTH - 20, 40);
    _box_info->align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT | FL_ALIGN_TOP);

    _output_times = new Fl_Output(WIDTH - 410, 85, 400, 25, "Zeitstempel");

#if 0
    _output_time = new Fl_Output(WIDTH - 60, 85, 50, 25, "Verbleibende Zeit");
    _output_time->deactivate();
    _output_size = new Fl_Output(WIDTH - 60, 120, 50, 25, "Geschätzte Größe");
    _output_size->deactivate();
#endif

    _check_ascii = new Fl_Check_Button(10, HEIGHT - 165, 240, 25,
                                       "Matlab ASCII (.dat)");
    _check_mat4 = new Fl_Check_Button(10, HEIGHT - 140, 240, 25,
                                      "Matlab binary, level 4 (.mat)");
    _spinner_decimation = new Fl_Spinner(10 + (WIDTH - 20) / 2, HEIGHT - 110,
            (WIDTH - 20) / 2, 25, "Decimation");
    _spinner_decimation->type(FL_INT_INPUT);
    _spinner_decimation->range(1.0, ~0U);

    _progress = new Fl_Progress(10, HEIGHT - 70, WIDTH - 20, 25, "0%");
    _progress->maximum(100);
    _progress->value(0);
    _progress->deactivate();

    _button_export = new Fl_Button(WIDTH - 130, HEIGHT - 35, 120, 25,
                                   "Exportieren");
    _button_export->callback(_callback, this);

    _button_close = new Fl_Button(WIDTH - 240, HEIGHT - 35, 100, 25,
                                  "Abbrechen");
    _button_close->callback(_callback, this);

    _thread_running = false;
    _export_finished = false;
}

/*****************************************************************************/

/**
   Destruktor
*/

ViewDialogExport::~ViewDialogExport()
{
    _clear_exporters();
    delete _wnd;
}

/*****************************************************************************/

/**
 */

void ViewDialogExport::_clear_exporters()
{
    list<Export *>::iterator exp_i;

    for (exp_i = _exporters.begin(); exp_i != _exporters.end(); exp_i++) {
        delete *exp_i;
    }

    _exporters.clear();
}

/*****************************************************************************/

/**
   Anzeigen des Dialoges
*/

void ViewDialogExport::show(const list<Channel> *channels,
                            LibDLS::Time start, LibDLS::Time end)
{
    stringstream str;
    list<ViewChannel>::const_iterator channel_i;
    list<Chunk>::const_iterator chunk_i;

    _channels = channels;
    _start = start;
    _end = end;
    _channel_count = _channels->size();

    str << "Anzahl Kanäle: " << _channel_count << endl
        << "Beginn: " << _start.to_real_time() << endl
        << "Ende: " << _end.to_real_time() << endl
        << "Zeitspanne: " << _start.diff_str_to(_end);
    _box_info->copy_label(str.str().c_str());

    {
        stringstream str;
        str << _start.to_real_time() << " "
            << _end.to_real_time();
        _output_times->value(str.str().c_str());
    }

    _wnd->show();

    while (_wnd->shown()) Fl::wait();
}

/*****************************************************************************/

/**
   Statische Callback-Funktion

   \param sender Widget, dass den Callback ausgelöst hat
   \param data Zeiger auf den Dialog
*/

void ViewDialogExport::_callback(Fl_Widget *sender, void *data)
{
    ViewDialogExport *dialog = (ViewDialogExport *) data;

    if (sender == dialog->_button_close) dialog->_button_close_clicked();
    if (sender == dialog->_wnd) dialog->_button_close_clicked();
    if (sender == dialog->_button_export) dialog->_button_export_clicked();
}

/*****************************************************************************/

/**
   Callback: Der "Schliessen"-Button wurde geklickt
*/

void ViewDialogExport::_button_close_clicked()
{
    if (_thread_running) {
        _thread_running = false;
        Fl::unlock();
        pthread_join(_thread, NULL);
        Fl::lock();
    }

    _wnd->hide();
}

/*****************************************************************************/

/**
   Callback: Der "Export"-Button wurde geklickt
*/

void ViewDialogExport::_button_export_clicked()
{
    list<ViewChannel>::const_iterator channel_i;
    string env_export, env_export_fmt;
    char *env;
    stringstream info_file_name;
    ofstream info_file;
    LibDLS::Time now;
    Export *exporter;

    if (_export_finished) {
        _wnd->hide();
        return;
    }

    now.set_now();

    if ((env = getenv("DLS_EXPORT"))) env_export = env;
    else env_export = ".";

    if ((env = getenv("DLS_EXPORT_FMT"))) env_export_fmt = env;
    else env_export_fmt = "dls-export-%Y-%m-%d-%H-%M-%S";

    _export_dir += env_export + "/" + now.format_time(env_export_fmt.c_str());

    cout << "Exporting to \"" << _export_dir << "\"." << endl;

    // create unique directory
    if (mkdir(_export_dir.c_str(), 0755)) {
        cerr << "ERROR: Failed to create export directory: ";
        cerr << strerror(errno) << endl;
        return;
    }

    // create info file
    info_file_name << _export_dir << "/dls_export_info";
    info_file.open(info_file_name.str().c_str(), ios::trunc);

    if (!info_file.is_open()) {
        cerr << "Failed to write \"" << info_file_name.str() << "\"!" << endl;
        return;
    }

    info_file << endl
              << "This is a DLS export directory." << endl << endl
              << "Exported on: "
              << now.to_rfc811_time() << endl << endl
              << "Exported range from: "
              << _start.to_real_time() << endl
              << "                 to: "
              << _end.to_real_time() << endl
              << "           duration: "
              << _start.diff_str_to(_end) << endl << endl;

    info_file.close();

    _clear_exporters();

    if (_check_ascii->value()) {
        try {
            exporter = new ExportAscii();
        }
        catch (...) {
            cerr << "Failed to allocate Ascii-Exporter." << endl;
            return;
        }
        _exporters.push_back(exporter);
    }

    if (_check_mat4->value()) {
        try {
            exporter = new ExportMat4();
        }
        catch (...) {
            cerr << "Failed to allocate Mat4-Exporter." << endl;
            return;
        }
        _exporters.push_back(exporter);
    }

    _decimation = (unsigned int) _spinner_decimation->value();

    if (pthread_create(&_thread, 0, _static_thread_function, this)) {
        cerr << "Failed to create thread!" << endl;
        return;
    }
}

/*****************************************************************************/

void *ViewDialogExport::_static_thread_function(void *data)
{
    ViewDialogExport *dialog = (ViewDialogExport *) data;

    Fl::lock();
    dialog->_thread_running = true;
    dialog->_button_export->deactivate();
    dialog->_progress->activate();
    dialog->_check_ascii->deactivate();
    dialog->_check_mat4->deactivate();
#if 0
    dialog->_output_time->activate();
    dialog->_output_size->activate();
#endif
    Fl::unlock();
    Fl::awake();

    dialog->_thread_function();

    Fl::lock();
    dialog->_thread_running = false;
    dialog->_export_finished = true;
#if 0
    dialog->_output_time->deactivate();
    dialog->_output_size->deactivate();
#endif
    dialog->_button_close->deactivate();
    dialog->_button_export->label("Schliessen");
    dialog->_button_export->activate();
    Fl::unlock();
    Fl::awake();

    cout << "Export finished." << endl;

    return (void *) NULL;
}

/*****************************************************************************/

void ViewDialogExport::_thread_function()
{
    list<Channel>::const_iterator channel_i;
    list<Export *>::iterator exp_i;
    ExportInfo info;
    unsigned int current_channel, total_channels;

    current_channel = 0;
    total_channels = _channels->size();

    if (!total_channels || _end <= _start) {
        _clear_exporters();
        return;
    }

    info.dialog = this;
    info.channel_percentage = 0.0;
    info.channel_factor = 100.0 / total_channels / (_end - _start).to_dbl();

    for (channel_i = _channels->begin();
         channel_i != _channels->end();
         channel_i++) {

        for (exp_i = _exporters.begin(); exp_i != _exporters.end(); exp_i++)
            (*exp_i)->begin(*channel_i, _export_dir);

        channel_i->fetch_data(_start, _end, 0, _export_data_callback, &info,
                _decimation);

        for (exp_i = _exporters.begin(); exp_i != _exporters.end(); exp_i++)
            (*exp_i)->end();

        current_channel++;

        // display progress
        info.channel_percentage = 100.0 * current_channel / total_channels;
        _set_progress_value((int) (info.channel_percentage + 0.5));

        if (!_thread_running) break;
    }

    _clear_exporters();
}

/*****************************************************************************/

/**
 */

int ViewDialogExport::_export_data_callback(Data *data, void *cb_data)
{
    ExportInfo *info = (ExportInfo *) cb_data;
    list<Export *>::iterator exp_i;
    double diff_time;
    double percentage;

    for (exp_i = info->dialog->_exporters.begin();
         exp_i != info->dialog->_exporters.end();
         exp_i++)
        (*exp_i)->data(data);

    // display progress
    diff_time = (data->end_time() - info->dialog->_start).to_dbl();
    percentage = info->channel_percentage + diff_time * info->channel_factor;
    info->dialog->_set_progress_value((int) (percentage + 0.5));

    return 0; // not adopted
}

/*****************************************************************************/

void ViewDialogExport::_set_progress_value(int percentage)
{
    stringstream label;

    if (_progress->value() == percentage) return;

    label << percentage << "%";

    Fl::lock();
    _progress->copy_label(label.str().c_str());
    _progress->value(percentage);
    _progress->redraw();
    Fl::unlock();
    Fl::awake();
}

/*****************************************************************************/
