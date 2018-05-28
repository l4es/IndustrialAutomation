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
#include <FL/fl_draw.H>

/*****************************************************************************/

#include "fl_track_bar.h"

#define MIN_BUTTON_HEIGHT 10

/*****************************************************************************/

/**
   Konstruktor
*/

Fl_Track_Bar::Fl_Track_Bar()
{
    _position = 0;
    _tracking = false;

    _last_width = 0;
    _last_height = 0;
}

/*****************************************************************************/

/**
   Destruktor
*/

Fl_Track_Bar::~Fl_Track_Bar()
{
}

/*****************************************************************************/

/**
   Setzt die (virtuelle) Höhe des zu scrollenden Inhalts

   \param height Neue Höhe
*/

void Fl_Track_Bar::content_height(int height)
{
    _content_height = height;
}

/*****************************************************************************/

/**
   Setzt die Höhe des Anzeigebereiches

   \param height Neue Höhe
*/

void Fl_Track_Bar::view_height(int height)
{
    _view_height = height;
}

/*****************************************************************************/

/**
   Zeichnet die track-Bar in das angegebene Rechteck

   \param left Linker Rand
   \param top Oberer Rand
   \param width Breite
   \param height Höhe
*/

void Fl_Track_Bar::draw(int left, int top, int width, int height)
{
    _last_width = width;
    _last_height = height;

    if (!_content_height || !_view_height || _content_height <= _view_height)
    {
        _visible = false;
        _position = 0;
        return;
    }

    _visible = true;

    // Höhe des Scroll-Buttons bestimmen
    _button_height = (int) (_view_height * height / (double) _content_height);
    if (_button_height < MIN_BUTTON_HEIGHT) _button_height = MIN_BUTTON_HEIGHT;

    if (_button_height >= height)
    {
        _visible = false;
        _position = 0;
        return;
    }

    // Position des Scroll-Buttons bestimmen
    _button_position = (int) (_position * (height - _button_height)
                              / (double) (_content_height - _view_height));

    if (_button_position + _button_height > height)
    {
        _button_position = height - _button_height;
        _position = (int) (_button_position * (_content_height - _view_height)
                           / (double) (height - _button_height));
    }

    // Scroll-Button zeichnen
    fl_color(150, 150, 150);
    fl_rectf(left, top + _button_position, width, _button_height);
}

/*****************************************************************************/

/**
   Überprüft die anliegenden Events und wertet diese bei Bedarf aus

   \param event FLTK-Ereignis
   \param xp X-Position des Cursors auf der Track-Bar
   \param yp Y-Position des Cursors auf der Track-Bar
   \return true, wenn das Ereignis für die Track-Bar war
   und ausgewertet wurde
*/

bool Fl_Track_Bar::handle(int event, int xp, int yp)
{
    int new_button_position;

    switch (event)
    {
        case FL_PUSH:

            _pushed_on_button = false;

            // Auf die Scrollbar geklickt
            if (_visible
                && xp >= 0
                && xp <= _last_width
                && yp >= _button_position
                && yp <= _button_position + _button_height)
            {
                if (Fl::event_button() == 1)
                {
                    _pushed_on_button = true;
                    _grip = yp - _button_position;
                }

                return true;
            }

            break;

        case FL_DRAG:

            if (!_tracking && _pushed_on_button) _tracking = true;

            if (_tracking)
            {
                new_button_position = yp - _grip;

                if (new_button_position < 0)
                {
                    new_button_position = 0;
                }
                else if (new_button_position > _last_height - _button_height)
                {
                    new_button_position = _last_height - _button_height;
                }

                _position = (int) (new_button_position
                                   * (_content_height - _view_height)
                                   / (double) (_last_height - _button_height));
                return true;
            }

            break;

        case FL_RELEASE:

            if (_tracking)
            {
                _tracking = false;
                return true;
            }

            break;

        default:
            return false;
    }

    return false;
}

/*****************************************************************************/



