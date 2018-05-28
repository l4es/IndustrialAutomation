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

#ifndef ViewDialogMainHpp
#define ViewDialogMainHpp

/*****************************************************************************/

#include <vector>
using namespace std;

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Tile.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Check_Button.H>

#include "lib/LibDLS/Dir.h"
#include "lib/LibDLS/JobPreset.h"

#include "fl_grid.h"
#include "ViewViewData.h"
#include "ViewViewMsg.h"

/*****************************************************************************/

/**
   Hauptdialog des DLS-Viewers
*/

class ViewDialogMain
{
public:
    ViewDialogMain(const string &);
    ~ViewDialogMain();

    void show();

private:
    string _dls_dir_path; /**< DLS-Datenverzeichnis */
    Fl_Double_Window *_wnd; /**< Dialogfenster */
    Fl_Tile *_tile_ver; /**< Vertikaler Trenner zwischen Anzeigen
                           und Kanalliste */
    Fl_Tile *_tile_hor; /**< Horizontaler Trenner zwischen Kanälen
                           und Messages */
    Fl_Choice *_choice_job; /**< Auswahlfeld zum Wählen des Auftrags */
    Fl_Button *_button_full; /**< Button zum Anzeigen der gesamten
                                Zeitspanne */
    Fl_Button *_button_reload; /**< Button zum erneuten Laden der Daten */
    Fl_Button *_button_export; /**< Button zum Exportieren der Daten */
    Fl_Check_Button *_checkbutton_messages; /**< Checkbutton zum Anzeigen der
                                    Nachrichten. */
    Fl_Button *_button_close; /**< "Schliessen"-Button */
    Fl_Grid *_grid_channels; /**< Grid zum Anzeigen der Kanalliste*/

    ViewViewData *_view_data; /**< Anzeige für die Kanaldaten */
    ViewViewMsg *_view_msg; /**< Anzeige für die Messages */

    LibDLS::Directory _dls_dir; /**< LibDLS directory object */
    LibDLS::Job *_current_job; /**< current job */

    static void _callback(Fl_Widget *, void *);
    void _button_close_clicked();
    void _button_reload_clicked();
    void _button_full_clicked();
    void _button_export_clicked();
    void _checkbutton_messages_clicked();
    void _choice_job_changed();
    void _grid_channels_changed();

    static void _data_range_callback(LibDLS::Time, LibDLS::Time, void *);
};

/*****************************************************************************/

#endif
