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

#include <iostream>
#include <sstream>
using namespace std;

#include <FL/Fl.H>
#include <FL/fl_ask.H>

/*****************************************************************************/

#include "ViewGlobals.h"
#include "ViewChannel.h"
#include "ViewDialogExport.h"
#include "ViewDialogMain.h"

/*****************************************************************************/

#define WIDTH 850
#define HEIGHT 550

/*****************************************************************************/

/**
   Konstruktor

   \param dls_dir DLS-Datenverzeichnis
*/

ViewDialogMain::ViewDialogMain(const string &dls_dir)
{
    int x = Fl::w() / 2 - WIDTH / 2;
    int y = Fl::h() / 2 - HEIGHT / 2;

    _dls_dir_path = dls_dir;
    _current_job = NULL;

    _wnd = new Fl_Double_Window(x, y, WIDTH, HEIGHT, "DLSView");
    _wnd->callback(_callback, this);
    _wnd->set_modal();

    _choice_job = new Fl_Choice(10, 25, 240, 25, "Auftrag");
    _choice_job->align(FL_ALIGN_TOP_LEFT);
    _choice_job->callback(_callback, this);

    _checkbutton_messages = new Fl_Check_Button(260, 25, 100, 25, "Nachrichten");
    _checkbutton_messages->set();
    _checkbutton_messages->callback(_callback, this);

    _button_full = new Fl_Button(370, 25, 100, 25, "Gesamt");
    _button_full->callback(_callback, this);

    _button_reload = new Fl_Button(480, 25, 100, 25, "Aktualisieren");
    _button_reload->callback(_callback, this);

    _button_export = new Fl_Button(590, 25, 100, 25, "Exportieren...");
    _button_export->callback(_callback, this);

    _button_close = new Fl_Button(WIDTH - 90, 25, 80, 25, "Schließen");
    _button_close->callback(_callback, this);

    _tile_ver = new Fl_Tile(10, 60, WIDTH - 20, HEIGHT - 70);

    _tile_hor = new Fl_Tile(10, 60, WIDTH - 220, HEIGHT - 70);

    _view_data = new ViewViewData(10, 60, WIDTH - 220, HEIGHT - 120);
    _view_msg = new ViewViewMsg(10, HEIGHT - 60, WIDTH - 220, 50);

    _tile_hor->end();
    //_tile_hor->resizable(_view_data); // Setzt Datenansicht fest

    _grid_channels = new Fl_Grid(WIDTH - 210, 60, 200, HEIGHT - 70);
    _grid_channels->add_column("channel", "Kanal");
    _grid_channels->select_mode(flgNoSelect);
    _grid_channels->check_boxes(true);
    _grid_channels->callback(_callback, this);

    _tile_ver->end();
    //_tile_ver->resizable(_tile_hor);

    _view_data->range_callback(_data_range_callback, this);

    _wnd->resizable(_view_data);
    _view_data->take_focus();
}

/*****************************************************************************/

/**
   Destruktor
*/

ViewDialogMain::~ViewDialogMain()
{
    delete _wnd;
}

/*****************************************************************************/

/**
   Anzeigen des Dialoges
*/

void ViewDialogMain::show()
{
    list<LibDLS::Job *>::const_iterator job_i;

    try {
        _dls_dir.set_uri(_dls_dir_path);
        _dls_dir.import();
    }
    catch (LibDLS::DirectoryException &e) {
        cerr << "Failed to import DLS directory." << endl;
        return;
    }

    for (job_i = _dls_dir.jobs().begin();
         job_i != _dls_dir.jobs().end();
         job_i++) {
        _choice_job->add((*job_i)->preset().id_desc().c_str());
    }

    if (_dls_dir.jobs().empty()) {
        stringstream str;
        str << "Im DLS-Datenverzeichnis" << endl
            << "\"" << _dls_dir_path << "\"" << endl
            << "wurden keine Aufträge gefunden!";
        fl_alert(str.str().c_str());
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

void ViewDialogMain::_callback(Fl_Widget *sender, void *data)
{
    ViewDialogMain *dialog = (ViewDialogMain *) data;

    if (sender == dialog->_button_close) dialog->_button_close_clicked();
    if (sender == dialog->_wnd) dialog->_button_close_clicked();
    if (sender == dialog->_choice_job) dialog->_choice_job_changed();
    if (sender == dialog->_grid_channels) dialog->_grid_channels_changed();
    if (sender == dialog->_button_reload) dialog->_button_reload_clicked();
    if (sender == dialog->_button_full) dialog->_button_full_clicked();
    if (sender == dialog->_button_export) dialog->_button_export_clicked();
    if (sender == dialog->_checkbutton_messages)
        dialog->_checkbutton_messages_clicked();
}

/*****************************************************************************/

/**
   Callback: Der "Schliessen"-Button wurde geklickt
*/

void ViewDialogMain::_button_close_clicked()
{
    _wnd->hide();
}

/*****************************************************************************/

/**
   Callback: Der "Aktualisieren"-Button wurde geklickt
*/

void ViewDialogMain::_button_reload_clicked()
{
    _view_data->update();
}

/*****************************************************************************/

/**
   Callback: Der "Gesamt"-Button wurde geklickt
*/

void ViewDialogMain::_button_full_clicked()
{
    _view_data->full_range();
}

/*****************************************************************************/

/**
   Callback: Der "Export"-Button wurde geklickt
*/

void ViewDialogMain::_button_export_clicked()
{
    ViewDialogExport *dialog;
    list<LibDLS::Channel> viewed_channels;
    list<LibDLS::Channel>::const_iterator channel_i;

    if (!_current_job) return;

    for (channel_i = _current_job->channels().begin();
         channel_i != _current_job->channels().end();
         channel_i++) {
        if (_view_data->has_channel(&(*channel_i))) {
            viewed_channels.push_back(*channel_i);
        }
    }

    dialog = new ViewDialogExport(_dls_dir_path);
    dialog->show(&viewed_channels, _view_data->start(), _view_data->end());
    delete dialog;
}

/*****************************************************************************/

/**
   Callback: Es wurde ein Auftrag in der Auftragsauswahl gewählt
*/

void ViewDialogMain::_choice_job_changed()
{
    _view_data->clear();
    _view_msg->clear();

    if (!(_current_job = _dls_dir.job(_choice_job->value()))) {
        _grid_channels->record_count(0);
        cerr << "Invalid job!" << endl;
        return;
    }

    _current_job->fetch_channels();
    _grid_channels->record_count(_current_job->channels().size());
}

/*****************************************************************************/

/**
   Callback: Die Checkbox "Nachrichten" wurde angeklickt.
*/

void ViewDialogMain::_checkbutton_messages_clicked()
{
    if (_checkbutton_messages->value() && _current_job) {
        _view_msg->load_msg(
                _current_job, _view_data->start(), _view_data->end());
    } else {
        _view_msg->clear();
    }
}

/*****************************************************************************/

/**
   Callback der Kanal-Grids
*/

void ViewDialogMain::_grid_channels_changed()
{
    unsigned int i = _grid_channels->current_record();
    LibDLS::Channel *channel;

    switch (_grid_channels->current_event()) {
        case flgContent:
            if (_grid_channels->current_col() == "channel") {
                channel = _current_job->channel(i);
                _grid_channels->current_content(channel->name());
            }
            break;

        case flgChecked:
            channel = _current_job->channel(i);
            _grid_channels->current_checked(_view_data->has_channel(channel));
            break;

        case flgCheck:
            channel = _current_job->channel(i);
            if (_view_data->has_channel(channel)) {
                _view_data->rem_channel(channel);
            }
            else {
                _view_data->add_channel(channel);
            }
            break;

        default:
            break;
    }
}

/*****************************************************************************/

/**
   Statischer Callback der Datenanzeige: Die Zeitspanne hat sich geändert!

   \param start Neuer Anfang der Zeitspanne
   \param end Neues Ende der Zeitspanne
   \param data Zeiger auf den Dialog
*/

void ViewDialogMain::_data_range_callback(LibDLS::Time start,
                                          LibDLS::Time end,
                                          void *data)
{
    ViewDialogMain *dialog = (ViewDialogMain *) data;
    if (dialog->_checkbutton_messages->value())
        dialog->_view_msg->load_msg(dialog->_current_job, start, end);
}

/*****************************************************************************/
