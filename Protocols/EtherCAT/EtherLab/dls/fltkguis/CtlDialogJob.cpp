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

#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

#include <FL/Fl.H>

/*****************************************************************************/

#include "CtlGlobals.h"
#include "CtlDialogChannel.h"
#include "CtlDialogChannels.h"
#include "CtlDialogJobEdit.h"
#include "CtlDialogJob.h"
#include "CtlDialogMsg.h"

/*****************************************************************************/

#define WIDTH 600
#define HEIGHT 470

/*****************************************************************************/

/**
   Konstruktor

   \param dls_dir DLS-Datenverzeichnis
*/

CtlDialogJob::CtlDialogJob(const string &dls_dir)
{
    int x = Fl::w() / 2 - WIDTH / 2;
    int y = Fl::h() / 2 - HEIGHT / 2;

    _dls_dir = dls_dir;

    _wnd = new Fl_Double_Window(x, y, WIDTH, HEIGHT, "Auftrag bearbeiten");
    _wnd->callback(_callback, this);
    _wnd->set_modal();

    _output_desc = new Fl_Output(10, 25, 200, 25, "Beschreibung");
    _output_desc->align(FL_ALIGN_TOP_LEFT);
    _output_desc->callback(_callback, this);

    _output_source = new Fl_Output(220, 25, 110, 25, "Quelle");
    _output_source->align(FL_ALIGN_TOP_LEFT);
    _output_source->callback(_callback, this);

    _output_trigger = new Fl_Output(340, 25, 160, 25, "Trigger");
    _output_trigger->align(FL_ALIGN_TOP_LEFT);
    _output_trigger->callback(_callback, this);

    _button_change = new Fl_Button(WIDTH - 90, 25, 80, 25, "Ändern...");
    _button_change->callback(_callback, this);

    _grid_channels = new Fl_Grid(10, 60, WIDTH - 20, HEIGHT - 105);
    _grid_channels->add_column("name", "Kanal", 300);
    _grid_channels->add_column("type", "Typ");
    _grid_channels->add_column("freq", "Abtastrate");
    _grid_channels->add_column("block", "Blockgröße");
    _grid_channels->add_column("format", "Format", 200);
    _grid_channels->callback(_callback, this);
    _grid_channels->take_focus();
    _grid_channels->select_mode(flgMultiSelect);

    _button_add = new Fl_Button(10, HEIGHT - 35, 150, 25,
                                "Kanäle hinzufügen...");
    _button_add->callback(_callback, this);

    _button_edit = new Fl_Button(170, HEIGHT - 35, 150, 25,
                                 "Kanäle editieren...");
    _button_edit->callback(_callback, this);
    _button_edit->deactivate();

    _button_rem = new Fl_Button(330, HEIGHT - 35, 150, 25,
                                "Kanäle entfernen");
    _button_rem->callback(_callback, this);
    _button_rem->deactivate();

    _button_close = new Fl_Button(WIDTH - 90, HEIGHT - 35, 80, 25,
                                  "Schliessen");
    _button_close->callback(_callback, this);

    _wnd->end();

    _wnd->resizable(_grid_channels);
}

/*****************************************************************************/

/**
   Destruktor
*/

CtlDialogJob::~CtlDialogJob()
{
    delete _wnd;
}

/*****************************************************************************/

/**
   Zeigt den Dialog an

   \param job Zeiger auf den Auftrag, der bearbeitet werden soll
*/

void CtlDialogJob::show(CtlJobPreset *job)
{
    _job = job;

    _changed = false;
    _updated = false;

    _output_desc->value(_job->description().c_str());
    _output_source->value(_job->source().c_str());
    _output_trigger->value(_job->trigger().c_str());

    _grid_channels->record_count(_job->channels()->size());

    _wnd->show();

    while (_wnd->shown()) Fl::wait();
}

/*****************************************************************************/

/**
   Statische Callback-Funktion

   \param sender Widget, dass den Callback ausgelöst hat
   \param data Zeiger auf den Dialog
*/

void CtlDialogJob::_callback(Fl_Widget *sender, void *data)
{
    CtlDialogJob *dialog = (CtlDialogJob *) data;

    if (sender == dialog->_grid_channels) dialog->_grid_channels_callback();
    if (sender == dialog->_button_close) dialog->_button_close_clicked();
    if (sender == dialog->_wnd) dialog->_button_close_clicked();
    if (sender == dialog->_button_add) dialog->_button_add_clicked();
    if (sender == dialog->_button_rem) dialog->_button_rem_clicked();
    if (sender == dialog->_button_edit) dialog->_button_edit_clicked();
    if (sender == dialog->_button_change) dialog->_button_change_clicked();
}

/*****************************************************************************/

/**
   Callback-Methode des Kanal-Grids
*/

void CtlDialogJob::_grid_channels_callback()
{
    unsigned int i;
    stringstream str;
    const LibDLS::ChannelPreset *channel;

    switch (_grid_channels->current_event())
    {
        case flgContent:
            i = _grid_channels->current_record();
            channel = &(*_job->channels())[i];

            if (_grid_channels->current_col() == "name")
            {
                _grid_channels->current_content(channel->name);
            }
            else if (_grid_channels->current_col() == "type")
            {
                _grid_channels->current_content(
                    LibDLS::channel_type_to_str(channel->type));
            }
            else if (_grid_channels->current_col() == "freq")
            {
                str << channel->sample_frequency << " Hz";
                _grid_channels->current_content(str.str());
            }
            else if (_grid_channels->current_col() == "block")
            {
                str << channel->block_size;
                _grid_channels->current_content(str.str());
            }
            else if (_grid_channels->current_col() == "format")
            {
                if (channel->format_index >= 0
                    && channel->format_index < LibDLS::FORMAT_COUNT)
                {
                    _grid_channels->current_content(
                        LibDLS::format_strings[channel->format_index]);
                }
                else
                {
                    _grid_channels->current_content("UNGÜLTIG!");
                }
            }
            break;

        case flgSelect:
        case flgDeSelect:
            _grid_selection_changed();
            break;

        case flgDoubleClick:
            _edit_channels();
            break;

        default:
            break;
    }
}

/*****************************************************************************/

/**
   Callback: Der "Schliessen"-Button wurde geklickt
*/

void CtlDialogJob::_button_close_clicked()
{
    _wnd->hide();
}

/*****************************************************************************/

/**
   Callback: Der "Hinzufügen"-Button wurde geklickt
*/

void CtlDialogJob::_button_add_clicked()
{
    list<LibDLS::RealChannel *>::const_iterator ch_i;
    CtlDialogChannels *dialog;

    dialog = new CtlDialogChannels(_job->source(), _job->port());
    dialog->show();

    _insert_channels(dialog->channels());

    delete dialog;
}

/*****************************************************************************/

/**
   Callback: Der "Entfernen"-Button wurde geklickt
*/

void CtlDialogJob::_button_rem_clicked()
{
    _remove_channels(_grid_channels->selected_list());
}

/*****************************************************************************/

/**
   Callback: Der "Ändern"-Button wurde geklickt
*/

void CtlDialogJob::_button_change_clicked()
{
    CtlDialogJobEdit *dialog = new CtlDialogJobEdit(_dls_dir);

    dialog->show(_job);

    if (dialog->updated())
    {
        _output_desc->value(_job->description().c_str());
        _output_source->value(_job->source().c_str());
        _output_trigger->value(_job->trigger().c_str());
    }

    delete dialog;
}

/*****************************************************************************/

/**
   Callback: Der "Editieren"-Button wurde geklickt
*/

void CtlDialogJob::_button_edit_clicked()
{
    _edit_channels();
}

/*****************************************************************************/

/**
   Editieren eines oder mehrerer Kanäle

   Öffnet für die gewählten Kanäle den Änderungsdialog
*/

void CtlDialogJob::_edit_channels()
{
    CtlDialogChannel *dialog;
    list<const LibDLS::ChannelPreset *> channels_to_edit;
    list<unsigned int>::const_iterator sel_i;

    sel_i = _grid_channels->selected_list()->begin();
    while (sel_i != _grid_channels->selected_list()->end())
    {
        // Adressen aller zu Ändernden Kanäle in eine Liste einfügen
        channels_to_edit.push_back(&(*_job->channels())[*sel_i]);
        sel_i++;
    }

    if (channels_to_edit.size() == 0) return;

    dialog = new CtlDialogChannel(_dls_dir);
    dialog->show(_job, &channels_to_edit);

    if (dialog->updated())
    {
        _changed = true;
        _grid_channels->redraw();
    }

    delete dialog;
}

/*****************************************************************************/

/**
   Einfügen von Kanälen

   Öffnet den Dialog zum Hinzufügen von Kanälen
*/

void CtlDialogJob::_insert_channels(const list<LibDLS::RealChannel> *channels)
{
    list<LibDLS::RealChannel>::const_iterator ch_i;
    LibDLS::ChannelPreset new_channel;
    CtlJobPreset job_copy;

    if (channels->size() == 0) return;

    job_copy = *_job;

    ch_i = channels->begin();
    while (ch_i != channels->end())
    {
        if (job_copy.channel_exists(ch_i->name))
        {
            msg_win->str() << "Kanal \"" << ch_i->name
                           << "\" ist bereits in der Liste!";
            msg_win->warning();
        }
        else
        {
            new_channel.name = ch_i->name;
            new_channel.sample_frequency = ch_i->frequency;
            new_channel.block_size =
                (ch_i->frequency >= 10 ? ch_i->frequency : 10);
            new_channel.meta_mask = 6; // min and max
            new_channel.meta_reduction = 30;
            new_channel.format_index = LibDLS::FORMAT_ZLIB;
            new_channel.type = ch_i->type;

            job_copy.add_channel(&new_channel);
        }

        ch_i++;
    }

    try
    {
        job_copy.write(_dls_dir);
    }
    catch (LibDLS::EJobPreset &e)
    {
        msg_win->str() << "Schreiben der Vorgabendatei: " << e.msg;
        msg_win->error();
        return;
    }

    try
    {
        job_copy.spool(_dls_dir);
    }
    catch (LibDLS::EJobPreset &e)
    {
        msg_win->str() << "Konnte den dlsd nicht benachrichtigen: " << e.msg;
        msg_win->warning();
    }

    *_job = job_copy;
    _changed = true;
    _grid_channels->record_count(_job->channels()->size());
}

/*****************************************************************************/

/**
   Löscht die ausgewählten Kanäle

   \param sel_list Liste mit den Indizes der selektierten Kanäle
*/

void CtlDialogJob::_remove_channels(const list<unsigned int> *sel_list)
{
    CtlJobPreset job_copy;
    list<unsigned int>::const_iterator sel_i;

    job_copy = *_job;

    sel_i = sel_list->begin();
    while (sel_i != sel_list->end())
    {
        if (*sel_i >= _job->channels()->size())
        {
            msg_win->str() << "Ungültiger Kanal-Index!";
            msg_win->error();
            return;
        }

        job_copy.remove_channel((*_job->channels())[*sel_i].name);

        sel_i++;
    }

    try
    {
        job_copy.write(_dls_dir);
    }
    catch (LibDLS::EJobPreset &e)
    {
        msg_win->str() << "Schreiben der Vorgabendatei: " << e.msg;
        msg_win->error();
        return;
    }

    try
    {
        job_copy.spool(_dls_dir);
    }
    catch (LibDLS::EJobPreset &e)
    {
        msg_win->str() << "Konnte den dlsd nicht benachrichtigen: " << e.msg;
        msg_win->warning();
    }

    *_job = job_copy;
    _changed = true;
    _grid_channels->record_count(_job->channels()->size());
}

/*****************************************************************************/

/**
   Passt bei Änderung der Kanalauswahl die Buttons an
*/

void CtlDialogJob::_grid_selection_changed()
{
    bool selected = _grid_channels->select_count() > 0;

    if (selected)
    {
        _button_rem->activate();
        _button_edit->activate();
    }
    else
    {
        _button_rem->deactivate();
        _button_edit->deactivate();
    }
}

/*****************************************************************************/
