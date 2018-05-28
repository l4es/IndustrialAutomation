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

#ifndef CtlDialogJobEditH
#define CtlDialogJobEditH

/*****************************************************************************/

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Choice.H>

/*****************************************************************************/

#include "CtlJobPreset.h"

/*****************************************************************************/

/**
   Dialog zum Editieren der Grunddaten eines Messauftrages
*/

class CtlDialogJobEdit
{
public:
    CtlDialogJobEdit(const string &);
    ~CtlDialogJobEdit();

    void show(CtlJobPreset *);
    bool updated();

private:
    Fl_Double_Window *_wnd; /**< Dialogfenster */
    Fl_Return_Button *_button_ok; /**< Bestätigungs-Button */
    Fl_Button *_button_cancel; /**< Button zum Abbrechen */
    Fl_Input *_input_desc; /**< Eingabefeld für die Beschreibung */
    Fl_Input *_input_source; /**< Eingabefeld für die Datenquelle */
    Fl_Input *_input_trigger; /**< Eingabefeld für den Trigger-Parameter */
    Fl_Input *_input_quota_time; /**< Eingabefeld für Zeit-Quota */
    Fl_Choice *_choice_time_ext; /**< Auswahlfenster für die Erweiterung
                                    der Zeit-Quota */
    Fl_Input *_input_quota_size; /**< Eingabefeld für Daten-Quota */
    Fl_Choice *_choice_size_ext; /**< Auswahlfenster für die Erweiterung
                                    der Daten-Quota */

    string _dls_dir;    /**< DLS-Datenverzeichnis */
    CtlJobPreset *_job; /**< Zeiger auf den Messauftrag */
    bool _updated;      /**< Wurde etwas geändert? */

    static void _callback(Fl_Widget *, void *);
    void _button_ok_clicked();
    void _button_cancel_clicked();
    bool _save_job();
    bool _create_job();
    bool _get_new_id(int *);
    void _display_time_quota();
    void _display_size_quota();
    bool _calc_time_quota(uint64_t *);
    bool _calc_size_quota(uint64_t *);
};

/*****************************************************************************/

/**
   Gibt zurück, ob sich etwas geändert hat

   \return true, wenn Daten geändert wurden
*/

inline bool CtlDialogJobEdit::updated()
{
    return _updated;
}

/*****************************************************************************/

#endif
