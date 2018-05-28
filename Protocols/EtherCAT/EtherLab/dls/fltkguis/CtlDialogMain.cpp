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
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <iostream>
#include <sstream>
#include <fstream>
using namespace std;

#include <FL/Fl.H>
#include <FL/fl_ask.H>

/*****************************************************************************/

#include "lib/XmlParser.h"

#include "CtlGlobals.h"
#include "CtlDialogJobEdit.h"
#include "CtlDialogJob.h"
#include "CtlDialogMain.h"
#include "CtlDialogMsg.h"

/*****************************************************************************/

#define WIDTH 700
#define HEIGHT 300

#define WATCHDOG_TIMEOUT 1.0

/*****************************************************************************/

/**
   Konstruktor

   \param dls_dir DLS-Datenverzeichnis
*/

CtlDialogMain::CtlDialogMain(const string &dls_dir)
{
    int x = Fl::w() / 2 - WIDTH / 2;
    int y = Fl::h() / 2 - HEIGHT / 2;

    _dls_dir = dls_dir;

    _wnd = new Fl_Double_Window(x, y, WIDTH, HEIGHT, "DLS Manager");
    _wnd->callback(_callback, this);

    _grid_jobs = new Fl_Grid(10, 10, WIDTH - 20, HEIGHT - 55);
    _grid_jobs->add_column("job", "Auftrag", 200);
    _grid_jobs->add_column("source", "Quelle");
    _grid_jobs->add_column("state", "Status");
    _grid_jobs->add_column("trigger", "Trigger");
    _grid_jobs->add_column("proc", "Prozess");
    _grid_jobs->add_column("logging", "Erfassung");
    _grid_jobs->callback(_callback, this);

    _button_close = new Fl_Button(WIDTH - 90, HEIGHT - 35, 80, 25,
                                  "Schliessen");
    _button_close->callback(_callback, this);

    _button_add = new Fl_Button(10, HEIGHT - 35, 130, 25, "Neuer Auftrag");
    _button_add->callback(_callback, this);

    _button_state = new Fl_Button(345, HEIGHT - 35, 100, 25);
    _button_state->callback(_callback, this);
    _button_state->hide();

    _wnd->end();

    _wnd->resizable(_grid_jobs);
}

/*****************************************************************************/

/**
   Destruktor
*/

CtlDialogMain::~CtlDialogMain()
{
    Fl::remove_timeout(_static_watchdog_timeout, this);

    delete _wnd;
}

/*****************************************************************************/

/**
   Dialog anzeigen
*/

void CtlDialogMain::show()
{
    // Fenster zeigen
    _wnd->show();

    // Überprüfen, ob das angegebene Verzeichnis
    // schon ein DLS-Datenverzeichnis ist
    _check_dls_dir();

    _load_jobs();
    _load_watchdogs();

    // Timeout für die Aktualisierung der Watchdogs hinzufügen
    Fl::add_timeout(WATCHDOG_TIMEOUT, _static_watchdog_timeout, this);

    // Solange in der Event-Loop bleiben, wie das Fenster sichtbar ist
    while (_wnd->shown()) Fl::wait();
}

/*****************************************************************************/

/**
   Statische Callback-Funktion

   \param sender Zeiger aud das Widget, das den Callback ausgelöst hat
   \param data User-Data, hier Zeiger auf den Dialog
*/

void CtlDialogMain::_callback(Fl_Widget *sender, void *data)
{
    CtlDialogMain *dialog = (CtlDialogMain *) data;

    if (sender == dialog->_grid_jobs) dialog->_grid_jobs_callback();
    if (sender == dialog->_button_close) dialog->_button_close_clicked();
    if (sender == dialog->_wnd) dialog->_button_close_clicked();
    if (sender == dialog->_button_add) dialog->_button_add_clicked();
    if (sender == dialog->_button_state) dialog->_button_state_clicked();
}

/*****************************************************************************/

/**
   Callback für das Erfassungsauftrags-Grid
*/

void CtlDialogMain::_grid_jobs_callback()
{
    unsigned int i;
    stringstream str;

    switch (_grid_jobs->current_event())
    {
        case flgContent:
            i = _grid_jobs->current_record();

            if (_grid_jobs->current_col() == "job")
            {
                str << _jobs[i].id_desc();
                _grid_jobs->current_content(str.str());
            }
            else if (_grid_jobs->current_col() == "source")
            {
                _grid_jobs->current_content(_jobs[i].source());
            }
            else if (_grid_jobs->current_col() == "state")
            {
                _grid_jobs->current_content(
                    _jobs[i].running() ? "gestartet" : "angehalten");
            }
            else if (_grid_jobs->current_col() == "trigger")
            {
                _grid_jobs->current_content(_jobs[i].trigger());
            }
            else if (_grid_jobs->current_col() == "proc")
            {
                if (_jobs[i].running())
                {
                    if (_jobs[i].process_watch_determined)
                    {
                        if (_jobs[i].process_bad_count < WATCH_ALERT)
                        {
                            if (!_grid_jobs->current_selected())
                            {
                                _grid_jobs->current_content_color(
                                    FL_DARK_GREEN);
                            }

                            _grid_jobs->current_content("läuft");
                        }
                        else
                        {
                            if (!_grid_jobs->current_selected())
                            {
                                _grid_jobs->current_content_color(FL_RED);
                            }

                            _grid_jobs->current_content("läuft nicht!");
                        }
                    }
                    else
                    {
                        if (!_grid_jobs->current_selected())
                        {
                            _grid_jobs->current_content_color(FL_DARK_YELLOW);
                        }

                        _grid_jobs->current_content("(unbekannt)");
                    }
                }
            }
            else if (_grid_jobs->current_col() == "logging")
            {
                if (_jobs[i].running())
                {
                    if (_jobs[i].logging_watch_determined)
                    {
                        if (_jobs[i].logging_bad_count < WATCH_ALERT)
                        {
                            if (!_grid_jobs->current_selected())
                            {
                                _grid_jobs->current_content_color(
                                    FL_DARK_GREEN);
                            }

                            _grid_jobs->current_content("läuft");
                        }
                        else
                        {
                            if (!_grid_jobs->current_selected())
                            {
                                _grid_jobs->current_content_color(FL_RED);
                            }

                            _grid_jobs->current_content("läuft nicht!");
                        }
                    }
                    else
                    {
                        if (!_grid_jobs->current_selected())
                        {
                            _grid_jobs->current_content_color(FL_DARK_YELLOW);
                        }

                        _grid_jobs->current_content("(unbekannt)");
                    }
                }
            }
            break;

        case flgSelect:
            _update_button_state();
            break;

        case flgDeSelect:
            _update_button_state();
            break;

        case flgDoubleClick:
            _edit_job(_grid_jobs->current_record());
            break;

        default:
            break;
    }
}

/*****************************************************************************/

/**
   Callback: Der "Beenden"-Button wurde geklickt
*/

void CtlDialogMain::_button_close_clicked()
{
    _wnd->hide();
}

/*****************************************************************************/

/**
   Callback: Der "Hinzufügen"-Button wurde geklickt
*/

void CtlDialogMain::_button_add_clicked()
{
    CtlDialogJobEdit *dialog = new CtlDialogJobEdit(_dls_dir);
    dialog->show(0); // 0 = Neuer Auftrag

    if (dialog->updated())
    {
        _load_jobs();
    }

    delete dialog;
}

/*****************************************************************************/

/**
   Callback: Der "Starten/Anhalten"-Button wurde geklickt
*/

void CtlDialogMain::_button_state_clicked()
{
    int index = _grid_jobs->selected_index();
    CtlJobPreset job_copy;

    if (index < 0 || index >= (int) _jobs.size())
    {
        msg_win->str() << "Ungültiger Auftrags-Index!";
        msg_win->error();
        return;
    }

    job_copy = _jobs[index];

    job_copy.toggle_running();

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
        msg_win->str() << "Konnte den dlsd nicht benachrichtigen!";
        msg_win->warning();
    }

    _jobs[index] = job_copy;
    _grid_jobs->redraw();
    _update_button_state();
}

/*****************************************************************************/

/**
   Alle Watchdog-Informationen laden
*/

void CtlDialogMain::_load_watchdogs()
{
    vector<CtlJobPreset>::iterator job_i;
    struct stat file_stat;
    stringstream dir_name;

    job_i = _jobs.begin();
    while (job_i != _jobs.end())
    {
        // Dateinamen konstruieren
        dir_name.str("");
        dir_name.clear();
        dir_name << _dls_dir << "/job" << job_i->id();

        if (stat((dir_name.str() + "/watchdog").c_str(), &file_stat) == 0)
        {
            // Wenn die neue Dateizeit jünger ist, als die zuletzt gelesene...
            if (file_stat.st_mtime > job_i->process_watchdog)
            {
                job_i->process_watchdog = file_stat.st_mtime;

                if (!job_i->process_watch_determined)
                {
                    job_i->process_watch_determined = true;
                    _grid_jobs->redraw();
                }

                if (job_i->process_bad_count > 0)
                {
                    job_i->process_bad_count = 0;
                    _grid_jobs->redraw();
                }
            }
            else // Watchdog nicht verändert
            {
                job_i->process_bad_count++;
            }
        }
        else // Konnte Watchdog nicht lesen
        {
            job_i->process_bad_count++;
        }

        if (job_i->process_bad_count == WATCH_ALERT)
        {
            job_i->process_watch_determined = true;
            _grid_jobs->redraw();
        }

        if (stat((dir_name.str() + "/logging").c_str(), &file_stat) == 0)
        {
            // Wenn die neue Dateizeit jünger ist, als die zuletzt gelesene...
            if (file_stat.st_mtime > job_i->logging_watchdog)
            {
                job_i->logging_watchdog = file_stat.st_mtime;

                if (!job_i->logging_watch_determined)
                {
                    job_i->logging_watch_determined = true;
                    _grid_jobs->redraw();
                }

                if (job_i->logging_bad_count > 0)
                {
                    job_i->logging_bad_count = 0;
                    _grid_jobs->redraw();
                }
            }
            else // Watchdog nicht verändert
            {
                job_i->logging_bad_count++;
            }
        }
        else // Konnte Watchdog nicht lesen
        {
            job_i->logging_bad_count++;
        }

        if (job_i->logging_bad_count == WATCH_ALERT)
        {
            job_i->logging_watch_determined = true;
            _grid_jobs->redraw();
        }


        job_i++;
    }
}

/*****************************************************************************/

/**
   Alle Erfassungsaufträge laden

   Läuft durch das Verzeichnis, merkt sich alle Einträge, die
   "jobXXX" heissen, sortiert diese Liste und überprüft dann
   jeweils, ob es sich um ein gültiges Auftragsverzeichnis
   handelt. Wenn ja, wird der entsprechende Auftrag importiert
   und in die Liste eingefügt.
*/

void CtlDialogMain::_load_jobs()
{
    unsigned int job_id;
    DIR *dir;
    struct dirent *dir_ent;
    string dirname;
    stringstream str;
    fstream file;
    CtlJobPreset job;
    struct stat file_stat;
    stringstream watch_file_name;
    list<unsigned int> job_ids;
    list<unsigned int>::iterator job_id_i;

    str.exceptions(ios::failbit | ios::badbit);

    // Liste der Aufträge leeren
    _grid_jobs->record_count(0);
    _jobs.clear();

    // Das Hauptverzeichnis öffnen
    if ((dir = opendir(_dls_dir.c_str())) == NULL)
    {
        msg_win->str() << "Konnte das Datenverzeichnis \"" << _dls_dir
                       << "\" nicht öffnen!";
        msg_win->error();
        return;
    }

    // Alle Dateien und Unterverzeichnisse durchlaufen
    while ((dir_ent = readdir(dir)) != NULL)
    {
        // Verzeichnisnamen kopieren
        dirname = dir_ent->d_name;

        // Wenn das Verzeichnis nicht mit "job" beginnt,
        // das nächste verarbeiten
        if (dirname.substr(0, 3) != "job") continue;

        str.str("");
        str.clear();
        str << dirname.substr(3);

        try
        {
            // ID aus dem Verzeichnisnamen lesen
            str >> job_id;
        }
        catch (...)
        {
            // Der Rest des Verzeichnisnamens ist keine Nummer!
            continue;
        }

        job_ids.push_back(job_id);
    }

    // Auftrags-IDs aufsteigend sortieren
    job_ids.sort();

    // Alle "gemerkten", potentiellen Auftrags-IDs durchlaufen
    for (job_id_i = job_ids.begin(); job_id_i != job_ids.end(); job_id_i++)
    {
        // Gibt es in dem Verzeichnis eine Datei job.xml?
        str.str("");
        str.clear();
        str << _dls_dir << "/job" << *job_id_i << "/job.xml";
        file.open(str.str().c_str(), ios::in);
        if (!file.is_open()) continue;

        try
        {
            job.import(_dls_dir, *job_id_i);
        }
        catch (LibDLS::EJobPreset &e)
        {
            msg_win->str() << "Importieren des Auftrags " << *job_id_i
                           << ": " << e.msg;
            msg_win->error();
            continue;
        }

        job.process_bad_count = 0;
        job.process_watch_determined = false;
        job.process_watchdog = 0;
        job.logging_bad_count = 0;
        job.logging_watch_determined = false;
        job.logging_watchdog = 0;

        // Auftrag in die Liste einfügen
        _jobs.push_back(job);

        // Dateinamen konstruieren
        watch_file_name.str("");
        watch_file_name.clear();
        watch_file_name << _dls_dir << "/job" << *job_id_i << "/watchdog";

        if (stat(watch_file_name.str().c_str(), &file_stat) == 0)
        {
            _jobs.back().process_watchdog = file_stat.st_mtime;
        }

        watch_file_name.str("");
        watch_file_name.clear();
        watch_file_name << _dls_dir << "/job" << *job_id_i << "/logging";

        if (stat(watch_file_name.str().c_str(), &file_stat) == 0)
        {
            _jobs.back().logging_watchdog = file_stat.st_mtime;
        }
    }

    _grid_jobs->record_count(_jobs.size());
}

/*****************************************************************************/

/**
   Erfassungsauftrag editieren

   Öffnet einen neuen Dialog, um den Auftrag zu bearbeiten

   \param index Index des zu bearbeitenden Auftrags in der Liste
*/

void CtlDialogMain::_edit_job(unsigned int index)
{
    CtlDialogJob *dialog = new CtlDialogJob(_dls_dir);

    dialog->show(&_jobs[index]);

    if (dialog->updated())
    {
        _load_jobs();
    }

    delete dialog;
}

/*****************************************************************************/

/**
   Aktualisieren des "Starten/Anhalten"-Buttons
*/

void CtlDialogMain::_update_button_state()
{
    int i;

    if (_grid_jobs->select_count())
    {
        i = _grid_jobs->selected_index();

        if (_jobs[i].running())
        {
            _button_state->label("Anhalten");
        }
        else
        {
            _button_state->label("Starten");
        }

        _button_state->show();
    }
    else
    {
        _button_state->hide();
    }
}

/*****************************************************************************/

/**
   Überprüft, ob das Verzeichnis ein DLS-Datenverzeichnis ist
*/

void CtlDialogMain::_check_dls_dir()
{
    struct stat stat_buf;
    stringstream str;
    bool build_dls_dir = false;
    int fd;
    pid_t pid;
    int status;
    fstream pid_file;
    bool start_dlsd;

    // Prüfen, ob das Verzeichnis überhaupt existiert
    if (stat(_dls_dir.c_str(), &stat_buf) == -1)
    {
        str << "Das Verzeichnis \"" << _dls_dir << "\"" << endl;
        str << "existiert noch nicht. Soll es als" << endl;
        str << "DLS-Datenverzeichnis angelegt werden?";

        if (fl_choice(str.str().c_str(), "Nein", "Ja", NULL) == 0) return;

        build_dls_dir = true;

        if (mkdir(_dls_dir.c_str(), 0755) == -1)
        {
            msg_win->str() << "Konnte das Verzeichnis \"" << _dls_dir << "\"";
            msg_win->str() << " nicht anlegen: " << strerror(errno);
            msg_win->error();
            return;
        }
    }
    else
    {
        // Prüfen, ob das angegebene DLS-Datenverzeichnis überhaupt
        // ein Verzeichnis ist
        if (!S_ISDIR(stat_buf.st_mode))
        {
            msg_win->str() << "\"" << _dls_dir << "\" ist kein Verzeichnis!";
            msg_win->error();
            return;
        }
    }

    // Existiert das Spooling-Verzeichnis?
    if (stat((_dls_dir + "/spool").c_str(), &stat_buf) == -1)
    {
        if (!build_dls_dir)
        {
            str.clear();
            str.str("");
            str << "Das Verzeichnis \"" << _dls_dir << "\"" << endl;
            str << "ist noch kein DLS-Datenverzeichnis." << endl;
            str << "Soll es als solches initialisiert werden?";

            if (fl_choice(str.str().c_str(), "Nein", "Ja", NULL) == 0) return;

            build_dls_dir = true;
        }

        // Spooling-Verzeichnis anlegen
        if (mkdir((_dls_dir + "/spool").c_str(), 0755) == -1)
        {
            msg_win->str() << "Konnte das Verzeichnis \""
                           << (_dls_dir + "/spool") << "\"";
            msg_win->str() << " nicht anlegen: " << strerror(errno);
            msg_win->error();
            return;
        }
    }

    // Existiert die Datei mit der ID-Sequenz?
    if (stat((_dls_dir + "/id_sequence").c_str(), &stat_buf) == -1)
    {
        if (!build_dls_dir)
        {
            str.clear();
            str.str("");
            str << "Das Verzeichnis \"" << _dls_dir << "\"" << endl;
            str << "ist noch kein DLS-Datenverzeichnis." << endl;
            str << "Soll es als solches initialisiert werden?";

            if (fl_choice(str.str().c_str(), "Nein", "Ja", NULL) == 0) return;

            build_dls_dir = true;
        }

        // Datei anlegen
        if ((fd = open((_dls_dir + "/id_sequence").c_str(),
                       O_WRONLY | O_CREAT, 0644)) == -1)
        {
            msg_win->str() << "Konnte die Datei \""
                           << (_dls_dir + "/id_sequence") << "\"";
            msg_win->str() << " nicht anlegen: " << strerror(errno);
            msg_win->error();
            return;
        }

        if (write(fd, "100\n", 4) != 4)
        {
            close(fd);
            msg_win->str() << "Konnte die Datei \""
                           << (_dls_dir + "/id_sequence") << "\"";
            msg_win->str() << " nicht beschreiben! Bitte manuell löschen!";
            msg_win->error();
            return;
        }

        close(fd);
    }

    start_dlsd = false;

    // Existiert die PID-Datei des dlsd?
    if (stat((_dls_dir + "/" + DLS_PID_FILE).c_str(), &stat_buf) == -1)
    {
        start_dlsd = true;
    }
    else
    {
        pid_file.exceptions(ios::badbit | ios::failbit);

        pid_file.open((_dls_dir + "/" + DLS_PID_FILE).c_str(), ios::in);

        if (!pid_file.is_open())
        {
            msg_win->str() << "Konnte die Datei \""
                           << (_dls_dir + "/" + DLS_PID_FILE)
                           << "\" nicht öffnen!";
            msg_win->error();
            return;
        }

        try
        {
            pid_file >> pid;
        }
        catch (...)
        {
            pid_file.close();

            msg_win->str() << "Datei \"" << (_dls_dir + "/" + DLS_PID_FILE)
                           << "\" ist korrupt!";
            msg_win->error();
            return;
        }

        pid_file.close();

        if (kill(pid, 0) == -1)
        {
            if (errno == ESRCH) // Prozess existiert nicht
            {
                start_dlsd = true;
            }
            else
            {
                msg_win->str() << "Konnte Prozess " << pid
                               << " nicht signalisieren!";
                msg_win->error();
                return;
            }
        }
    }

    if (start_dlsd)
    {
        str.clear();
        str.str("");
        str << "Für das Verzeichnis \"" << _dls_dir << "\"" << endl;
        str << "läuft noch kein DLS-Daemon. Jetzt starten?";

        if (fl_choice(str.str().c_str(), "Nein", "Ja", NULL) == 1)
        {
            if ((pid = fork()) == -1)
            {
                msg_win->str() << "fork() fehlgeschlagen!";
                msg_win->error();
                return;
            }

            if (pid == 0) // Kindprozess
            {
                const char *params[4] = {"dlsd", "-d", _dls_dir.c_str(), 0};

                if (execvp("dlsd", (char * const *) params) == -1)
                {
                    cerr << "ERROR: Could not exec dlsd: "
                         << strerror(errno) << endl;
                    exit(-1);
                }
            }
            else // Mutterprozess
            {
                waitpid(pid, &status, 0);

                if ((signed char) WEXITSTATUS(status) == -1)
                {
                    msg_win->str() << "Konnte dlsd nicht ausführen!"
                                   << " Siehe Konsole für Fehlermeldungen.";
                    msg_win->error();
                    return;
                }
            }
        }
    }
}

/*****************************************************************************/

/**
   Statische Callback-Funktion für den Watchdog-Timeout

   \param data Zeiger auf den Dialog
*/

void CtlDialogMain::_static_watchdog_timeout(void *data)
{
    CtlDialogMain *dialog = (CtlDialogMain *) data;

    dialog->_load_watchdogs();

    Fl::add_timeout(WATCHDOG_TIMEOUT, _static_watchdog_timeout, dialog);
}

/*****************************************************************************/
