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

#include <math.h>

#include <iostream>
#include <sstream>
using namespace std;

#include <FL/Fl.H>

/*****************************************************************************/

#include "lib/LibDLS/ChannelPreset.h"

#include "lib/mdct.h"

#include "CtlGlobals.h"
#include "CtlJobPreset.h"
#include "CtlDialogChannel.h"
#include "CtlDialogMsg.h"

/*****************************************************************************/

#define WIDTH 270
#define HEIGHT 240

/*****************************************************************************/

/**
   Konstruktor

   \param dls_dir DLS-Datenverzeichnis
*/

CtlDialogChannel::CtlDialogChannel(const string &dls_dir)
{
    stringstream str;
    string format;
    int i, x, y;
    unsigned int j;

    x = Fl::w() / 2 - WIDTH / 2;
    y = Fl::h() / 2 - HEIGHT / 2;

    _dls_dir = dls_dir;

    _wnd = new Fl_Double_Window(x, y, WIDTH, HEIGHT, "Kanäle bearbeiten");
    _wnd->callback(_callback, this);
    _wnd->set_modal();

    _input_freq = new Fl_Float_Input(10, 25, 120, 25, "Abtastrate (Hz)");
    _input_freq->align(FL_ALIGN_TOP_LEFT);
    _input_freq->callback(_callback, this);

    _input_block = new Fl_Input(140, 25, 120, 25, "Blockgröße");
    _input_block->align(FL_ALIGN_TOP_LEFT);
    _input_block->callback(_callback, this);

    _input_mask = new Fl_Input(10, 70, 120, 25, "Meta-Maske");
    _input_mask->align(FL_ALIGN_TOP_LEFT);
    _input_mask->callback(_callback, this);
    _input_mask->readonly(META_MASK_FIXED);

    _input_red = new Fl_Input(140, 70, 120, 25, "Untersetzung");
    _input_red->align(FL_ALIGN_TOP_LEFT);
    _input_red->callback(_callback, this);
    _input_red->readonly(META_REDUCTION_FIXED);

    _choice_format = new Fl_Choice(10, 115, 250, 25, "Format");
    _choice_format->align(FL_ALIGN_TOP_LEFT);
    _choice_format->callback(_callback, this);
    _choice_format_selected = false;

    // Alle Kompressionsformate einfügen
    for (i = 0; i < LibDLS::FORMAT_COUNT; i++)
    {
        format = LibDLS::format_strings[i];

        // Slashes escapen, FLTK macht aus Strings mit
        // Slashes entsprechende Untermenüs...
        for (j = 1; j <= format.length(); j++)
        {
            if (format[j] == '/')
            {
                format.insert(j, "\\");
                j++;
            }
        }

        _choice_format->add(format.c_str());
    }

    _choice_mdct = new Fl_Choice(10, 160, 120, 25, "MDCT-Blockgröße");
    _choice_mdct->align(FL_ALIGN_TOP_LEFT);
    _choice_mdct->callback(_callback, this);
    _choice_mdct_selected = false;

    // Alle gültigen MDCT-Blockgrößen einfügen
    for (i = LibDLS::MDCT_MIN_EXP2; i <= LibDLS::MDCT_MAX_EXP2; i++)
    {
        str.str("");
        str.clear();
        str << (1 << i);
        _choice_mdct->add(str.str().c_str());
    }

    _input_accuracy = new Fl_Input(140, 160, 120, 25, "Genauigkeit");
    _input_accuracy->align(FL_ALIGN_TOP_LEFT);
    _input_accuracy->callback(_callback, this);

    _button_ok = new Fl_Return_Button(WIDTH - 90, HEIGHT - 35, 80, 25, "OK");
    _button_ok->callback(_callback, this);

    _button_cancel = new Fl_Button(WIDTH - 180, HEIGHT - 35, 80, 25,
                                   "Abbrechen");
    _button_cancel->callback(_callback, this);

    _wnd->end();
}

/*****************************************************************************/

/**
   Destruktor
*/

CtlDialogChannel::~CtlDialogChannel()
{
    delete _wnd;
}

/*****************************************************************************/

/**
   Dialog zeigen

   \param job Zeiger auf den Auftrag, dessen Kanäle geändert werden sollen
   \param channels Zeiger auf eine Liste von konstanten
   Zeigern auf die zu ändernden Kanäle
*/

void CtlDialogChannel::show(CtlJobPreset *job,
        const list<const LibDLS::ChannelPreset *> *channels)
{
    stringstream str;
    list<const LibDLS::ChannelPreset *>::const_iterator channel_i;
    double freq;
    unsigned int block, mask, red, mdct_block_size;
    int format_index;
    bool freq_equal = true, block_equal = true;
    bool mask_equal = true, red_equal = true;
    bool format_equal = true, mdct_equal = true, accuracy_equal = true;
    double accuracy, exp2;

    _job = job;
    _channels = channels;

    _input_freq->value("");
    _input_block->value("");
    _input_red->value("");
    _input_mask->value("");
    _input_accuracy->value("");

    _choice_mdct->deactivate();
    _input_accuracy->deactivate();

    ((Fl_Menu_ *) _choice_format)->value((Fl_Menu_Item *) 0);
    _choice_format->redraw();
    ((Fl_Menu_ *) _choice_mdct)->value((Fl_Menu_Item *) 0);
    _choice_mdct->redraw();

    if (_channels->size() == 0) return;

    channel_i = _channels->begin();

    freq = (*channel_i)->sample_frequency;
    block = (*channel_i)->block_size;
    mask = (*channel_i)->meta_mask;
    red = (*channel_i)->meta_reduction;
    format_index = (*channel_i)->format_index;
    mdct_block_size = (*channel_i)->mdct_block_size;
    accuracy = (*channel_i)->accuracy;

    channel_i++;

    while (channel_i != _channels->end())
    {
        if ((*channel_i)->sample_frequency != freq) freq_equal = false;
        if ((*channel_i)->block_size != block) block_equal = false;
        if ((*channel_i)->meta_mask != mask) mask_equal = false;
        if ((*channel_i)->meta_reduction != red) red_equal = false;
        if ((*channel_i)->format_index != format_index) format_equal = false;
        if ((*channel_i)->mdct_block_size != mdct_block_size)
            mdct_equal = false;
        if ((*channel_i)->accuracy != accuracy) accuracy_equal = false;
        channel_i++;
    }

    if (freq_equal)
    {
        str.str("");
        str.clear();
        str << freq;
        _input_freq->value(str.str().c_str());
    }

    if (block_equal)
    {
        str.str("");
        str.clear();
        str << block;
        _input_block->value(str.str().c_str());
    }

    if (mask_equal)
    {
        str.str("");
        str.clear();
        str << mask;
        _input_mask->value(str.str().c_str());
    }

    if (red_equal)
    {
        str.str("");
        str.clear();
        str << red;
        _input_red->value(str.str().c_str());
    }

    // Wenn bei allen Kanälen das gleiche, gültige Format gewählt wurde
    if (format_equal && format_index >= 0
            && format_index < LibDLS::FORMAT_COUNT)
    {
        if (format_index == LibDLS::FORMAT_MDCT)
        {
            _choice_mdct->activate();

            if (mdct_equal) // MDCT gewählt und MDCT-Parameter gleich
            {
                exp2 = log10((double) mdct_block_size) / log10((double) 2);

                // MDCT-Blockgröße gültig?
                if (exp2 == (int) exp2 && exp2 >= LibDLS::MDCT_MIN_EXP2
                    && exp2 <= LibDLS::MDCT_MAX_EXP2)
                {
                    _choice_format->value(format_index);
                    _choice_format_selected = true;

                    _choice_mdct->value((int) exp2 - LibDLS::MDCT_MIN_EXP2);
                    _choice_mdct_selected = true;
                }
            }
        }

        if (format_index == LibDLS::FORMAT_MDCT
            || format_index == LibDLS::FORMAT_QUANT)
        {
            _input_accuracy->activate();

            if (accuracy_equal)
            {
                str.str("");
                str.clear();
                str << accuracy;
                _input_accuracy->value(str.str().c_str());
            }
        }

        if (format_index != LibDLS::FORMAT_MDCT)
        {
            _choice_format->value(format_index);
            _choice_format_selected = true;
        }
    } // Format gueltig

    _updated = false;

    _wnd->show();

    while (_wnd->shown()) Fl::wait();
}

/*****************************************************************************/

/**
   Statische Callback-Funktion

   \param sender Zeiger auf das Widget, dass den Callback ausgelöst hat
   \param data Zeiger auf den Dialog
*/

void CtlDialogChannel::_callback(Fl_Widget *sender, void *data)
{
    CtlDialogChannel *dialog = (CtlDialogChannel *) data;

    if (sender == dialog->_button_ok) dialog->_button_ok_clicked();
    if (sender == dialog->_button_cancel) dialog->_button_cancel_clicked();
    if (sender == dialog->_wnd) dialog->_button_cancel_clicked();
    if (sender == dialog->_choice_format) dialog->_choice_format_changed();
    if (sender == dialog->_choice_mdct) dialog->_choice_mdct_changed();
}

/*****************************************************************************/

/**
   Callback: "OK"-Button wurde geklickt
*/

void CtlDialogChannel::_button_ok_clicked()
{
    if (!_save_channels()) return;

    _wnd->hide();
}

/*****************************************************************************/

/**
   Callback: Cancel-Button wurde geklickt
*/

void CtlDialogChannel::_button_cancel_clicked()
{
    _updated = false;
    _wnd->hide();
}

/*****************************************************************************/

/**
   Callback: Es wurde ein Eintrag in der Format-Auswahlbox gewählt
*/

void CtlDialogChannel::_choice_format_changed()
{
    _choice_format_selected = true;

    if (_choice_format->value() == LibDLS::FORMAT_MDCT)
    {
        _choice_mdct->activate();
        _input_accuracy->activate();
    }
    else if (_choice_format->value() == LibDLS::FORMAT_QUANT)
    {
        ((Fl_Menu_ *) _choice_mdct)->value((Fl_Menu_Item *) 0);
        _choice_mdct_selected = false;
        _choice_mdct->deactivate();
        _input_accuracy->activate();
    }
    else
    {
        ((Fl_Menu_ *) _choice_mdct)->value((Fl_Menu_Item *) 0);
        _choice_mdct_selected = false;
        _choice_mdct->deactivate();
        _input_accuracy->value("");
        _input_accuracy->deactivate();
    }
}

/*****************************************************************************/

/**
   Callback: Es wurde ein Eintrag der MDCT-Blockgrößen-Auswahlbox gewählt
*/

void CtlDialogChannel::_choice_mdct_changed()
{
    _choice_mdct_selected = true;
}

/*****************************************************************************/

/**
   Speichert alle Kanäle

   \return true, wenn alle Kanäle gespeichert werden konnten
*/

bool CtlDialogChannel::_save_channels()
{
    LibDLS::ChannelPreset channel;
    stringstream str;
    list<const LibDLS::ChannelPreset *>::const_iterator channel_i;
    double freq;
    unsigned int block, mask, red, mdct_block_size = 0;
    double accuracy;
    bool write_freq, write_block, write_mask, write_red, write_acc;
    bool channel_changed, channels_changed = false;
    list<LibDLS::ChannelPreset> channel_backups;
    list<LibDLS::ChannelPreset>::iterator backup_i;

    str.exceptions(ios::failbit | ios::badbit);

    try
    {
        if ((write_freq = (string(_input_freq->value()) != "")))
        {
            str.str("");
            str.clear();
            str << _input_freq->value();
            str >> freq;
        }

        if ((write_block = (string(_input_block->value()) != "")))
        {
            str.str("");
            str.clear();
            str << _input_block->value();
            str >> block;
        }

        if ((write_mask = (string(_input_mask->value()) != "")))
        {
            str.str("");
            str.clear();
            str << _input_mask->value();
            str >> mask;
        }

        if ((write_red = (string(_input_red->value()) != "")))
        {
            str.str("");
            str.clear();
            str << _input_red->value();
            str >> red;
        }

        if ((write_acc = (string(_input_accuracy->value()) != "")))
        {
            str.str("");
            str.clear();
            str << _input_accuracy->value();
            str >> accuracy;
        }

        if (_choice_format_selected)
        {
            if (_choice_format->value() == LibDLS::FORMAT_MDCT)
            {
                // MDCT nur für Fließkommatypen
                channel_i = _channels->begin();
                while (channel_i != _channels->end())
                {
                    if ((*channel_i)->type == LibDLS::TUNKNOWN)
                    {
                        msg_win->str() << "Kanal \"" << (*channel_i)->name
                                       << "\" hat keine Typinformation!";
                        msg_win->error();
                        return false;
                    }

                    if ((*channel_i)->type != LibDLS::TFLT
                        && (*channel_i)->type != LibDLS::TDBL)
                    {
                        msg_win->str() << "Kanal \"" << (*channel_i)->name
                                       << "\" hat keinen Gleitkommatyp!";
                        msg_win->error();
                        return false;
                    }

                    // Alle Kanäle müssen eine Genauigkeit haben
                    if (!write_acc && (*channel_i)->accuracy <= 0.0)
                    {
                        msg_win->str() << "Kanal \"" << (*channel_i)->name
                                       << "\" benötigt noch eine Genauigkeit!";
                        msg_win->error();
                        return false;
                    }

                    channel_i++;
                }

                if (!_choice_mdct_selected)
                {
                    msg_win->str() << "Sie haben keine MDCT-Blockgröße"
                                   << " angegeben!";
                    msg_win->error();
                    return false;
                }

                mdct_block_size =
                    1 << (_choice_mdct->value() + LibDLS::MDCT_MIN_EXP2);

                // Blockgröße kein Vielfaches von MDCT-Dimension?
                if (write_block)
                {
                    if (block % mdct_block_size)
                    {
                        msg_win->str() << "Die Blockgröße muss ein Vielfaches"
                                       << " der MDCT-Blockgröße sein!";
                        msg_win->error();
                        return false;
                    }
                }
                else
                {
                    // Keine Blockgröße angegeben.
                    // Alle Kanäle mit ihren bisherigen
                    // Blockgrößen überprüfen
                    channel_i = _channels->begin();
                    while (channel_i != _channels->end())
                    {
                        if ((*channel_i)->block_size % mdct_block_size)
                        {
                            msg_win->str() << "Die bisherigen Blockgröße"
                                           << " des Kanals \"";
                            msg_win->str() << (*channel_i)->name;
                            msg_win->str() << "\" ist kein Vielfaches"
                                           << " der MDCT-Blockgröße!";
                            msg_win->error();
                            return false;
                        }

                        channel_i++;
                    }
                }
            } // MDCT

            else if (_choice_format->value() == LibDLS::FORMAT_QUANT)
            {
                channel_i = _channels->begin();
                while (channel_i != _channels->end())
                {
                    if ((*channel_i)->type == LibDLS::TUNKNOWN)
                    {
                        msg_win->str() << "Kanal \"" << (*channel_i)->name
                                       << "\" hat keine Typinformation!";
                        msg_win->error();
                        return false;
                    }

                    // Quantisierung nur für Fließkommatypen
                    if ((*channel_i)->type != LibDLS::TFLT
                        && (*channel_i)->type != LibDLS::TDBL)
                    {
                        msg_win->str() << "Kanal \"" << (*channel_i)->name
                                       << "\" hat keinen Gleitkommatyp!";
                        msg_win->error();
                        return false;
                    }

                    // Alle Kanäle müssen eine Genauigkeit haben
                    if (!write_acc && (*channel_i)->accuracy <= 0.0)
                    {
                        msg_win->str() << "Kanal \"" << (*channel_i)->name
                                       << "\" benötigt noch eine Genauigkeit!";
                        msg_win->error();
                        return false;
                    }

                    channel_i++;
                }
            } // Quant

        } // Format ausgewaehlt
    }
    catch (...)
    {
        msg_win->str() << "Ungültige Eingabe!";
        msg_win->error();
        return false;
    }

    channel_i = _channels->begin();
    while (channel_i != _channels->end())
    {
        // Daten von Original-Kanal kopieren
        channel = **channel_i;

        channel_changed = false;

        // Soll sich für diesen Kanal etwas ändern?
        if ((write_freq && channel.sample_frequency != freq) ||
            (write_block && channel.block_size != block) ||
            (write_mask && channel.meta_mask != mask) ||
            (write_red && channel.meta_reduction != red) ||
            (write_acc && channel.accuracy != accuracy))
        {
            channel_changed = true;
        }

        if (_choice_format_selected)
        {
            if (channel.format_index != _choice_format->value()
                || (_choice_format->value() == LibDLS::FORMAT_MDCT
                    && (mdct_block_size != channel.mdct_block_size
                        || accuracy != channel.accuracy))
                || (_choice_format->value() == LibDLS::FORMAT_QUANT
                    && accuracy != channel.accuracy))
            {
                channel_changed = true;
            }
        }

        if (channel_changed)
        {
            // Alte Vorgaben sichern, falls etwas schief geht
            channel_backups.push_back(channel);

            // Neue Daten übernehmen
            if (write_freq) channel.sample_frequency = freq;
            if (write_block) channel.block_size = block;
            if (write_mask) channel.meta_mask = mask;
            if (write_red) channel.meta_reduction = red;
            if (write_acc) channel.accuracy = accuracy;
            if (_choice_format_selected)
            {
                channel.format_index = _choice_format->value();

                if (channel.format_index == LibDLS::FORMAT_MDCT)
                {
                    channel.mdct_block_size = mdct_block_size;
                }
            }

            try
            {
                _job->change_channel(&channel);
            }
            catch (LibDLS::EJobPreset &e)
            {
                msg_win->str() << e.msg;
                msg_win->error();
                return false;
            }

            channels_changed = true;
        }

        channel_i++;
    }

    if (channels_changed)
    {
        try
        {
            _job->write(_dls_dir);
        }
        catch (LibDLS::EJobPreset &e)
        {
            msg_win->str() << e.msg;
            msg_win->error();

            try
            {
                backup_i = channel_backups.begin();
                while (backup_i != channel_backups.end())
                {
                    _job->change_channel(&(*backup_i));
                    backup_i++;
                }
            }
            catch (LibDLS::EChannelPreset &e)
            {
                msg_win->str() << "FATAL: " << e.msg << "!";
                msg_win->str() << " Bitte starten Sie die Anwendung neu!";
                msg_win->error();
            }

            return false;
        }

        try
        {
            _job->spool(_dls_dir);
        }
        catch (LibDLS::EJobPreset &e)
        {
            msg_win->str() << "Konnte dlsd nicht benachrichtigen: " << e.msg;
            msg_win->warning();
        }

        _updated = true;
    }
    else
    {
        _updated = false;
    }

    return true;
}

/*****************************************************************************/
