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

#include <FL/Fl.H>

/*****************************************************************************/

#include "CtlDialogMsg.h"

/*****************************************************************************/

#define WIDTH 500
#define HEIGHT 150

/*****************************************************************************/

/**
   Konstruktor
*/

CtlDialogMsg::CtlDialogMsg()
{
    int x = Fl::w() / 2 - WIDTH / 2;
    int y = Fl::h() / 2 - HEIGHT / 2;

    _wnd = new Fl_Double_Window(x, y, WIDTH, HEIGHT, "Meldungen");
    _wnd->callback(_callback, this);
    _wnd->set_modal();

    _grid_msg = new Fl_Grid(10, 10, WIDTH - 20, HEIGHT - 55);
    _grid_msg->add_column("text", "Meldung");
    _grid_msg->select_mode(flgNoSelect);
    _grid_msg->callback(_callback, this);

    _button_ok = new Fl_Button(WIDTH - 90, HEIGHT - 35, 80, 25, "Schließen");
    _button_ok->callback(_callback, this);

    _wnd->end();

    _wnd->resizable(_grid_msg);
}

/*****************************************************************************/

/**
   Destruktor
*/

CtlDialogMsg::~CtlDialogMsg()
{
    delete _wnd;
}

/*****************************************************************************/

/**
   Fügt einen Fehler hinzu

   Der Text wird aus dem Stream gelesen.
*/

void CtlDialogMsg::error()
{
    COMMsg msg;

    msg.type = 1; // Error
    msg.text = _str.str();

    _messages.insert(_messages.begin(), msg);
    _grid_msg->record_count(_messages.size());

    _str.str("");
    _str.clear();

    _wnd->show();
}

/*****************************************************************************/

/**
   Fügt eine Warnung hinzu

   Der Text wird aus dem Stream gelesen.
*/

void CtlDialogMsg::warning()
{
    COMMsg msg;

    msg.type = 2; // Warning
    msg.text = _str.str();

    _messages.insert(_messages.begin(), msg);
    _grid_msg->record_count(_messages.size());

    _str.str("");
    _str.clear();

    _wnd->show();
}

/*****************************************************************************/

/**
   Statische Callback-Funktion

   \param sender Widget, das den Callback ausgelöst hat
   \param data Zeiger auf den Dialog
*/

void CtlDialogMsg::_callback(Fl_Widget *sender, void *data)
{
    CtlDialogMsg *dialog = (CtlDialogMsg *) data;

    if (sender == dialog->_button_ok) dialog->_button_ok_clicked();
    if (sender == dialog->_grid_msg) dialog->_grid_msg_callback();
    if (sender == dialog->_wnd) dialog->_button_ok_clicked();
}

/*****************************************************************************/

/**
   Callback: Der "OK"-Button wurde geklickt
*/

void CtlDialogMsg::_button_ok_clicked()
{
    // Fenster Schließen
    _wnd->hide();

    // Meldungen entfernen
    _grid_msg->record_count(0);
    _messages.clear();
}

/*****************************************************************************/

/**
   Callback-Funktion des Grids
*/

void CtlDialogMsg::_grid_msg_callback()
{
    COMMsg msg;

    // Anforderung für Zelleninhalt?
    if (_grid_msg->current_event() == flgContent)
    {
        msg = _messages[_grid_msg->current_record()];

        // Spalte "text" angefragt?
        if (_grid_msg->current_col() == "text")
        {
            if (msg.type == 1) // Error
            {
                _grid_msg->current_content_color(FL_RED);
            }
            else if (msg.type == 2) // Warning
            {
                _grid_msg->current_content_color(FL_DARK_YELLOW);
            }

            _grid_msg->current_content(msg.text);
        }
    }
}

/*****************************************************************************/
