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

#ifndef CtlDialogMsgH
#define CtlDialogMsgH

/*****************************************************************************/

#include <vector>
#include <sstream>
using namespace std;

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>

/*****************************************************************************/

#include "fl_grid.h"

/*****************************************************************************/

/**
   Nachricht mit Text und Typ für CtlDialogMsg
*/

struct COMMsg
{
    int type;
    string text;
};

/*****************************************************************************/

/**
   Allgemeiner Dialog zum Anzeigen von Fehlern und Warnungen
*/

class CtlDialogMsg
{
public:
    CtlDialogMsg();
    ~CtlDialogMsg();

    stringstream &str();
    void error();
    void warning();

private:
    Fl_Double_Window *_wnd; /**< Dialogfenster */
    Fl_Grid *_grid_msg;     /**< Grid zum Anzeigen der Meldungen */
    Fl_Button *_button_ok;  /**< "OK"-Button */

    vector<COMMsg> _messages; /**< Vektor mit den aktuell
                                 angezeigten Meldungen */
    stringstream _str;        /**< Stream zum einfachen Hinzufügen
                                 von Meldungen */

    static void _callback(Fl_Widget *, void *);
    void _button_ok_clicked();
    void _grid_msg_callback();
};

/*****************************************************************************/

inline stringstream &CtlDialogMsg::str()
{
    return _str;
}

/*****************************************************************************/

#endif
