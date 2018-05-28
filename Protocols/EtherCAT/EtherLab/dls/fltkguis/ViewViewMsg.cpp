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

#include <sys/types.h>
#include <dirent.h>

#include <iostream>
#include <sstream>
using namespace std;

#include <FL/Fl.H>
#include <FL/fl_draw.H>

/*****************************************************************************/

#include "ViewGlobals.h"
#include "ViewViewMsg.h"

//#define DEBUG

#define FRAME_WIDTH 3
#define LEVEL_HEIGHT 15
#define TRACK_BAR_WIDTH 20

/*****************************************************************************/

const Fl_Color msg_colors[LibDLS::Job::Message::TypeCount] =
{
    FL_BLUE,           // Info
    FL_DARK_YELLOW,    // Warning
    FL_RED,            // Error
    FL_MAGENTA,        // Critical Error
    fl_darker(FL_GRAY) // Broadcast
};

/*****************************************************************************/

/**
   Konstruktor

   \param x X-Position des linken Randes
   \param y Y-Position des oberen Randes
   \param w Breite des Widgets
   \param h Höhe des Widgets
   \param label Label des Widgets
*/

ViewViewMsg::ViewViewMsg(int x, int y, int w, int h, const char *label)
    : Fl_Widget(x, y, w, h, label)
{
    _focused = false;
    _track_bar = new Fl_Track_Bar();
}

/*****************************************************************************/

/**
   Destruktor
*/

ViewViewMsg::~ViewViewMsg()
{
    _messages.clear();
    delete _track_bar;
}

/*****************************************************************************/

/**
   Löscht alle Nachrichten
*/

void ViewViewMsg::clear()
{
    _messages.clear();
    redraw();
}

/*****************************************************************************/

/**
   Lädt Nachrichten im angegebenen Zeitbereich

   \param start Anfangszeit des Bereiches
   \param end Endzeit des Bereiches
*/

void ViewViewMsg::load_msg(const LibDLS::Job *job,
        LibDLS::Time start, LibDLS::Time end)
{
    clear();

    _range_start = start;
    _range_end = end;

    list<LibDLS::Job::Message> msgs = job->load_msg(start, end);

    for (list<LibDLS::Job::Message>::const_iterator i = msgs.begin();
            i != msgs.end(); i++) {
        Message msg;
        msg.message = *i;
        msg.level = 0;
        _messages.push_back(msg);
    }

    redraw();
}

/*****************************************************************************/

/**
   FLTK-Zeichenfunktion
*/

void ViewViewMsg::draw()
{
    list<Message>::iterator msg_i;
    int i, text_width, text_height;
    double scale_x;
    int xp, scroll_pos;

    // Schriftart und -größe setzen
    fl_font(FL_HELVETICA, 10);

    // Hintergrund zeichnen
    draw_box(FL_DOWN_BOX, FL_WHITE);

    if (_range_end <= _range_start) return;

    // Skalierung berechnen
    scale_x = (w() - 2 * FRAME_WIDTH) / (_range_end - _range_start).to_dbl();

    // Ebenen berechnen, auf denen die einzelnen
    // Nachrichten gezeichnet werden sollen
    _calc_msg_levels();

    // Scroll-Bar zeichnen
    _track_bar->content_height(_level_count * LEVEL_HEIGHT);
    _track_bar->view_height(h() - 2 * FRAME_WIDTH);
    _track_bar->draw(x() + w() - FRAME_WIDTH - TRACK_BAR_WIDTH,
                     y() + FRAME_WIDTH,
                     TRACK_BAR_WIDTH,
                     h() - 2 * FRAME_WIDTH);

    scroll_pos = _track_bar->position();

    // Clipping einrichten
    if (_track_bar->visible()) {
        fl_push_clip(x() + FRAME_WIDTH, y() + FRAME_WIDTH,
                     w() - 2 * FRAME_WIDTH - TRACK_BAR_WIDTH - 1,
                     h() - 2 * FRAME_WIDTH);
    }
    else {
        fl_push_clip(x() + FRAME_WIDTH, y() + FRAME_WIDTH,
                     w() - 2 * FRAME_WIDTH, h() - 2 * FRAME_WIDTH);
    }

    // Level von unten nach oben zeichnen
    for (i = _level_count - 1; i >= 0; i--) {
        msg_i = _messages.begin();
        while (msg_i != _messages.end()) {
            if (msg_i->level == i) {
                xp = (int)
                    ((msg_i->message.time - _range_start).to_dbl() * scale_x);

                // Text ausmessen
                text_width = 0;
                fl_measure(msg_i->message.text.c_str(),
                        text_width, text_height, 0);

#ifdef DEBUG
                cout << text_width << " " << text_height << " "
                     << msg_i->message.text << endl;
#endif

                // Hintergrund hinter dem Text weiss zeichnen
#ifdef DEBUG
                fl_color(FL_RED);
#else
                fl_color(FL_WHITE);
#endif
                fl_rectf(x() + FRAME_WIDTH + xp,
                         y() + FRAME_WIDTH + i * LEVEL_HEIGHT - scroll_pos,
                         text_width + 5,
                         LEVEL_HEIGHT);

                // Zeitlinie einzeichnen
                fl_color(FL_BLACK);
                fl_line(x() + FRAME_WIDTH + xp,
                        y() + FRAME_WIDTH - scroll_pos,
                        x() + FRAME_WIDTH + xp,
                        y() + FRAME_WIDTH + i * LEVEL_HEIGHT
                        + LEVEL_HEIGHT / 2 - scroll_pos);
                fl_line(x() + FRAME_WIDTH + xp,
                        y() + FRAME_WIDTH + i * LEVEL_HEIGHT
                        + LEVEL_HEIGHT / 2 - scroll_pos,
                        x() + FRAME_WIDTH + xp + 2,
                        y() + FRAME_WIDTH + i * LEVEL_HEIGHT
                        + LEVEL_HEIGHT / 2 - scroll_pos);

                // Text zeichnen
                if (msg_i->message.type >= 0 &&
                        msg_i->message.type
                        < LibDLS::Job::Message::TypeCount) {
                    fl_color(msg_colors[msg_i->message.type]);
                }
                else {
                    fl_color(FL_BLACK);
                }
                fl_draw(msg_i->message.text.c_str(),
                        x() + FRAME_WIDTH + xp + 4,
                        y() + FRAME_WIDTH + i * LEVEL_HEIGHT
                        + (LEVEL_HEIGHT - text_height) / 2
                        + text_height - fl_descent() - scroll_pos);
            }

            msg_i++;
        }
    }

    // Clipping wieder entfernen
    fl_pop_clip();
}

/*****************************************************************************/

/**
   Berechnet für jede Nachricht die Anzeigeebene
*/

void ViewViewMsg::_calc_msg_levels()
{
    double scale_x;
    int level, xp;
    list<Message>::iterator msg_i;
    list<int> levels;
    list<int>::iterator level_i;
    bool found;

    _level_count = 0;

    if (_range_end <= _range_start) return;

    // Skalierung berechnen
    scale_x = (w() - 2 * FRAME_WIDTH) / (_range_end - _range_start).to_dbl();

    msg_i = _messages.begin();
    while (msg_i != _messages.end()) {
        xp = (int) ((msg_i->message.time - _range_start).to_dbl() * scale_x);

        // Zeile finden, in der die Nachricht gezeichnet werden kann
        level = 0;
        found = false;
        level_i = levels.begin();
        while (level_i != levels.end()) {
            if (*level_i < xp) { // Nachricht würde in diese Zeile passen
                found = true;
                msg_i->level = level;

                // Endposition in Zeile vermerken
                *level_i = (int)
                    (xp + fl_width(msg_i->message.text.c_str())) + 5;
                break;
            }

            level_i++;
            level++;
        }

        if (!found) {
            msg_i->level = level;

            // Alle Zeilen voll. Neue erstellen.
            levels.push_back((int)
                    (xp + fl_width(msg_i->message.text.c_str())) + 5);
            _level_count++;
        }

        msg_i++;
    }
}

/*****************************************************************************/

/**
   FLTK-Ereignisfunktion

   \param event Ereignistyp
   \return 1, wenn Ereignis verarbeitet wurde
*/

int ViewViewMsg::handle(int event)
{
    int xp, yp;

    xp = Fl::event_x() - x();
    yp = Fl::event_y() - y();

    if (_track_bar->handle(event, xp - w() + TRACK_BAR_WIDTH,
                           yp - FRAME_WIDTH)) {
        redraw();
        return 1;
    }

    switch (event) {
        case FL_PUSH:
            take_focus();
            return 1;

        case FL_FOCUS:
            _focused = true;
            redraw();
            return 1;

        case FL_UNFOCUS:
            _focused = false;
            redraw();
            return 1;

        default:
            return 0;
    }

    return 0;
}

/*****************************************************************************/
