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

#ifndef FlTrackBarHpp
#define FlTrackBarHpp

/*****************************************************************************/

/**
   Track-Bar-Klasse für FLTK

   Diese Track-Bar ist kein eigenes Widget, sondern eine
   Hilfsklasse, die die Berechnung, das Zeichnen und die
   Interaktion mit dem Benutzer übernimmt.
*/

class Fl_Track_Bar
{
public:
    Fl_Track_Bar();
    ~Fl_Track_Bar();

    void content_height(int);
    void view_height(int);
    void draw(int, int, int, int);
    bool handle(int, int, int);

    bool visible() const;
    int position() const;

private:
    int _last_width; /**< Breite beim letzten Zeichnen */
    int _last_height; /**< Höhe beim letzten Zeichnen */

    int _content_height; /**< Virtuelle Höhe des zu scrollenden Inhaltes */
    int _view_height; /**< Höhe des Anzeigebereiches für den Inhalt */

    bool _visible; /**< Die Trackbar ist momentan sichtbar */
    bool _pushed_on_button; /**< Der Klick vor einem Drag-Event
                               war auf den Track-Button */
    bool _tracking; /**< Der Benutzer zieht gerade den Track-Button */

    int _position; /**< Offset des scrollenden Inhaltes in Pixel */
    int _button_position; /**< Position des track-Buttons */
    int _button_height; /**< Höhe des Track-Buttons */
    int _grip; /**< Position des Cursors auf dem Track-Button beim Ziehen */
};

/*****************************************************************************/

/**
   Gibt zurück, ob der Track-Bar momentan sichtbar ist

   \return true, wenn sichtbar
*/

inline bool Fl_Track_Bar::visible() const
{
    return _visible;
}

/*****************************************************************************/

/**
   Gibt das Offset der scrollenden Daten für das Zeichnen zurück

   \return Offset in Pixeln
*/

inline int Fl_Track_Bar::position() const
{
    return _position;
}

/*****************************************************************************/

#endif
