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
using namespace std;

#include <FL/Fl.H>

/*****************************************************************************/

#include "CtlGlobals.h"
#include "CtlDialogJobEdit.h"
#include "CtlDialogMsg.h"

/*****************************************************************************/

#define WIDTH 220
#define HEIGHT 300

/*****************************************************************************/

/**
   Konstruktor
*/

CtlDialogJobEdit::CtlDialogJobEdit(const string &dls_dir)
{
    int x = Fl::w() / 2 - WIDTH / 2;
    int y = Fl::h() / 2 - HEIGHT / 2;

    _dls_dir = dls_dir;
    _updated = false;

    _wnd = new Fl_Double_Window(x, y, WIDTH, HEIGHT, "Auftrag ändern");
    _wnd->callback(_callback, this);
    _wnd->set_modal();

    _button_ok = new Fl_Return_Button(WIDTH - 90, HEIGHT - 35, 80, 25, "OK");
    _button_ok->callback(_callback, this);

    _button_cancel = new Fl_Button(WIDTH - 180, HEIGHT - 35, 80, 25,
                                   "Abbrechen");
    _button_cancel->callback(_callback, this);

    _input_desc = new Fl_Input(10, 30, 200, 25, "Beschreibung");
    _input_desc->align(FL_ALIGN_TOP_LEFT);
    _input_desc->take_focus();

    _input_source = new Fl_Input(10, 80, 200, 25, "Quelle");
    _input_source->align(FL_ALIGN_TOP_LEFT);

    _input_trigger = new Fl_Input(10, 130, 200, 25, "Trigger");
    _input_trigger->align(FL_ALIGN_TOP_LEFT);

    _input_quota_time = new Fl_Input(10, 180, 90, 25, "Zeit-Quota");
    _input_quota_time->align(FL_ALIGN_TOP_LEFT);

    _choice_time_ext = new Fl_Choice(110, 180, 100, 25);
    _choice_time_ext->add("Sekunden");
    _choice_time_ext->add("Minuten");
    _choice_time_ext->add("Stunden");
    _choice_time_ext->add("Tage");
    _choice_time_ext->value(2);

    _input_quota_size = new Fl_Input(10, 230, 90, 25, "Daten-Quota");
    _input_quota_size->align(FL_ALIGN_TOP_LEFT);

    _choice_size_ext = new Fl_Choice(110, 230, 100, 25);
    _choice_size_ext->add("Byte");
    _choice_size_ext->add("Megabyte");
    _choice_size_ext->add("Gigabyte");
    _choice_size_ext->value(1);

    _wnd->end();
}

/*****************************************************************************/

/**
   Destruktor
*/

CtlDialogJobEdit::~CtlDialogJobEdit()
{
    delete _wnd;
}

/*****************************************************************************/

/**
   Dialog anzeigen

   \param job zeiger auf den zu editiernden Messauftrag
*/

void CtlDialogJobEdit::show(CtlJobPreset *job)
{
    _job = job;
    _updated = false;

    // Wenn ein bestehender Auftrag editiert werden soll,
    // müssen zuerst dessen Daten geladen werden
    if (_job)
    {
        _input_desc->value(_job->description().c_str());
        _input_source->value(_job->source().c_str());
        _input_trigger->value(_job->trigger().c_str());
        _display_time_quota();
        _display_size_quota();
    }

    // Wenn der Auftrag schon existiert, soll kein Editieren
    // der Datenquelle mehr möglich sein!
    _input_source->readonly(_job != 0);
    if (_job != 0) {
        _input_source->tooltip("Can not be edited for an existing job!");
    } else {
        _input_source->tooltip("");
    }

    // Fenster anzeigen
    _wnd->show();

    // Solange in der Event-Loop bleiben, wie das fenster sichtbar ist
    while (_wnd->shown()) Fl::wait();
}

/*****************************************************************************/

/**
   Statische Callback-Funktion

   \param sender Zeiger auf das Widget, dass den Callback ausgelöst hat
   \param data Zeiger auf den Dialog
*/

void CtlDialogJobEdit::_callback(Fl_Widget *sender, void *data)
{
    CtlDialogJobEdit *dialog = (CtlDialogJobEdit *) data;

    if (sender == dialog->_button_ok) dialog->_button_ok_clicked();
    if (sender == dialog->_button_cancel) dialog->_button_cancel_clicked();
    if (sender == dialog->_wnd) dialog->_button_cancel_clicked();
}

/*****************************************************************************/

/**
   Callback: Der "OK"-Button wurde geklickt
*/

void CtlDialogJobEdit::_button_ok_clicked()
{
    // Wenn ein existierender Auftrag editiert wird
    if (_job)
    {
        // Diesen Speichern
        if (!_save_job())
        {
            // Fehler beim Speichern. Dialog offen lassen!
            return;
        }
    }

    // Wenn ein neuer Auftrag editiert wird
    else
    {
        // Diesen erstellen
        if (!_create_job())
        {
            // Fehler beim Erstellen. Dialog offen lassen!
            return;
        }
    }

    // Alles OK! Fenster schliessen
    _wnd->hide();
}

/*****************************************************************************/

/**
   Callback: Der "Abbrechen"-Button wurde geklickt
*/

void CtlDialogJobEdit::_button_cancel_clicked()
{
    // Dialog schließen
    _wnd->hide();
}

/*****************************************************************************/

/**
   Geänderte Auftragsdaten speichern

   \return true, wenn Daten gespeichert wurden
*/

bool CtlDialogJobEdit::_save_job()
{
    CtlJobPreset job_copy;
    string quota_str;
    uint64_t quota_time, quota_size;

    if (!_calc_time_quota(&quota_time)) return false;
    if (!_calc_size_quota(&quota_size)) return false;

    if (_job->description() != _input_desc->value()
        || _job->source() != _input_source->value()
        || _job->trigger() != _input_trigger->value()
        || _job->quota_time() != quota_time
        || _job->quota_size() != quota_size)
    {
        // Alte Daten sichern
        job_copy = *_job;

        // Neue Daten setzen
        _job->description(_input_desc->value());
        _job->source(_input_source->value());
        _job->trigger(_input_trigger->value());
        _job->quota_time(quota_time);
        _job->quota_size(quota_size);

        try
        {
            // Versuchen, die Daten zu speichern
            _job->write(_dls_dir);
        }
        catch (LibDLS::EJobPreset &e)
        {
            // Fehler! Alte Daten zurückladen
            *_job = job_copy;
            msg_win->str() << e.msg;
            msg_win->error();
            return false;
        }

        // Die Auftragsdaten wurden jetzt verändert
        _updated = true;

        try
        {
            // Versuchen, einen Spooling-Eintrag zu erzeugen
            _job->spool(_dls_dir);
        }
        catch (LibDLS::EJobPreset &e)
        {
            // Fehler! Aber nur Warnung ausgeben.
            msg_win->str() << "Konnte den dlsd nicht benachrichtigen: "
                           << e.msg;
            msg_win->warning();
        }
    }

    return true;
}

/*****************************************************************************/

/**
   Einen neuen Erfassungsauftrag erstellen

   \return true, wenn neue Auftrag erstellt wurde
*/

bool CtlDialogJobEdit::_create_job()
{
    int new_id;
    CtlJobPreset job;
    uint64_t quota_time, quota_size;

    if (!_calc_time_quota(&quota_time)) return false;
    if (!_calc_size_quota(&quota_size)) return false;

    // Neue Job-ID aus der ID-Sequence-Datei lesen
    if (!_get_new_id(&new_id))
    {
        msg_win->str() << "Es konnte keine neue Auftrags-ID ermittelt werden!";
        msg_win->error();
        return false;
    }

    // Auftragsdaten setzen
    job.id(new_id);
    job.description(_input_desc->value());
    job.source(_input_source->value());
    job.trigger(_input_trigger->value());
    job.quota_time(quota_time);
    job.quota_size(quota_size);
    job.running(false);

    try
    {
        // Versuchen, die neuen Daten zu speichern
        job.write(_dls_dir);
    }
    catch (LibDLS::EJobPreset &e)
    {
        // Fehler beim Speichern!
        msg_win->str() << e.msg;
        msg_win->error();
        return false;
    }

    // Die Auftragsdaten wurden verändert
    _updated = true;

    try
    {
        // Versuchen, eine Spooling-Eintrag zu erzeugen
        job.spool(_dls_dir);
    }
    catch (LibDLS::EJobPreset &e)
    {
        // Fehler! Aber nur eine Warnung ausgeben.
        msg_win->str() << "Konnte den dlsd nicht benachrichtigen: " << e.msg;
        msg_win->warning();
    }

    return true;
}

/*****************************************************************************/

/**
   Erhöht die ID in der Sequenzdatei und liefert den alten Wert

   \param id Zeiger auf Speicher für die neue ID
   \return true, wenn die neue ID geholt werden konnte
*/

bool CtlDialogJobEdit::_get_new_id(int *id)
{
    fstream id_in_file, id_out_file;
    string id_file_name = _dls_dir + "/id_sequence";

    id_in_file.open(id_file_name.c_str(), ios::in);

    if (!id_in_file.is_open())
    {
        return false;
    }

    id_in_file.exceptions(ios::badbit | ios::failbit);

    try
    {
        // Dateiinhalt in einen Integer konvertieren
        id_in_file >> *id;
    }
    catch (...)
    {
        // Fehler beim Konvertieren
        id_in_file.close();
        return false;
    }

    id_in_file.close();

    id_out_file.open(id_file_name.c_str(), ios::out | ios::trunc);

    if (!id_out_file.is_open())
    {
        return false;
    }

    try
    {
        // Neuen ID-Wert in die Datei schreiben
        id_out_file << (*id + 1) << endl;
    }
    catch (...)
    {
        id_out_file.close();
        return false;
    }

    id_out_file.close();

    return true;
}

/*****************************************************************************/

/**
   Zeigt die aktuelle Zeit-Quota an
*/

void CtlDialogJobEdit::_display_time_quota()
{
    uint64_t time_quota = _job->quota_time();
    stringstream str;

    if (time_quota > 0)
    {
        if (time_quota % 86400 == 0)
        {
            time_quota /= 86400;
            _choice_time_ext->value(3);
        }
        else if (time_quota % 3600 == 0)
        {
            time_quota /= 3600;
            _choice_time_ext->value(2);
        }
        else if (time_quota % 60 == 0)
        {
            time_quota /= 60;
            _choice_time_ext->value(1);
        }
        else
        {
            _choice_time_ext->value(0);
        }

        str << time_quota;
    }
    else
    {
        _choice_time_ext->value(2);
    }

    _input_quota_time->value(str.str().c_str());
}

/*****************************************************************************/

/**
   Zeigt die aktuelle Daten-Quota an
*/

void CtlDialogJobEdit::_display_size_quota()
{
    uint64_t size_quota = _job->quota_size();
    stringstream str;

    if (size_quota > 0)
    {
        if (size_quota % 1073741824 == 0) // GB
        {
            size_quota /= 1073741824;
            _choice_size_ext->value(2);
        }
        else if (size_quota % 1048576 == 0) // MB
        {
            size_quota /= 1048576;
            _choice_size_ext->value(1);
        }
        else
        {
            _choice_size_ext->value(0);
        }

        str << size_quota;
    }
    else
    {
        _choice_size_ext->value(1);
    }

    _input_quota_size->value(str.str().c_str());
}

/*****************************************************************************/

/**
   Berechnet die Zeit-Quota

   \param time_quota Zeiger auf Variable für berechnete Quota
   \return true, wenn Quota berechnet werden konnte
*/

bool CtlDialogJobEdit::_calc_time_quota(uint64_t *time_quota)
{
    stringstream str;
    string quota_str = _input_quota_time->value();
    int ext = _choice_time_ext->value();

    str.exceptions(ios::badbit | ios::failbit);

    if (quota_str.length() > 0)
    {
        str << quota_str;

        try
        {
            str >> *time_quota;
        }
        catch (...)
        {
            msg_win->str() << "Die Zeit-Quota muss eine Ganzzahl sein!";
            msg_win->error();
            return false;
        }

        if (ext == 1)
        {
            *time_quota *= 60;
        }
        else if (ext == 2)
        {
            *time_quota *= 3600;
        }
        else if (ext == 3)
        {
            *time_quota *= 86400;
        }
    }
    else
    {
        *time_quota = 0;
    }

    return true;
}

/*****************************************************************************/

/**
   Berechnet die angegebene Daten-Quota

   \param size_quota Zeiger auf Variable für berechnete Quota
   \return true, wenn Quota berechnet wurde
*/

bool CtlDialogJobEdit::_calc_size_quota(uint64_t *size_quota)
{
    stringstream str;
    string quota_str = _input_quota_size->value();
    int ext = _choice_size_ext->value();

    str.exceptions(ios::badbit | ios::failbit);

    if (quota_str.length() > 0)
    {
        str << quota_str;

        try
        {
            str >> *size_quota;
        }
        catch (...)
        {
            msg_win->str() << "Die Daten-Quota muss eine Ganzzahl sein!";
            msg_win->error();
            return false;
        }

        if (ext == 1)
        {
            *size_quota *= 1048576;
        }
        else if (ext == 2)
        {
            *size_quota *= 1073741824;
        }
    }
    else
    {
        *size_quota = 0;
    }

    return true;
}

/*****************************************************************************/
