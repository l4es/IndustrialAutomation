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
using namespace std;

#include <FL/Fl.H>
#include <FL/fl_draw.H>

#include "lib/LibDLS/globals.h"

#include "fl_grid.h"

/*****************************************************************************/

#define MIN_SCROLL_HEIGHT 10
#define LEFT_MARGIN 4
#define FRAME_WIDTH 3

/*****************************************************************************/

/**
   Konstruktor

   \param name Identifiziernder Name der neuen Spalte
   \param title Anzuzeigender Titel
   \param width Relative Breite
*/

Fl_Grid_Column::Fl_Grid_Column(const string &name,
                               const string &title, int width)
{
    _name = name;
    _title = title;
    _width = width;
}

/*****************************************************************************/

/**
   Destruktor
*/

Fl_Grid_Column::~Fl_Grid_Column()
{
}

/*****************************************************************************/

/**
   Konstruktor

   \param xp Horizontale Positon der oberen, linken Ecke in Pixel
   \param yp Vertikale Position der oberen, linken Ecke in Pixel
   \param wp Breite des Widgets in Pixel
   \param hp Höhe des Widgets in Pixel
   \param label Name des Widgets in FLTK (Wird nicht verwendet)
*/

Fl_Grid::Fl_Grid(int xp, int yp, int wp, int hp, const char *label)
    : Fl_Widget(xp, yp, wp, hp, label)
{
    _cb = 0;
    _record_count = 0;
    _row_height = 21;
    _scroll_index = 0;
    _focused = false;
    _select_mode = flgSingleSelect;
    _scroll_tracking = false;
    _range_select_possible = false;
    _check_boxes = false;
}

/*****************************************************************************/

/**
   Destruktor
*/

Fl_Grid::~Fl_Grid()
{
    // Kein deselect_all(), callbacks könnten in die Hose gehen!
}

/*****************************************************************************/

/**
   Fügt eine Spalte hinzu

   \param name Identifiziernder Name der neuen Spalte
   \param title Anzuzeigender Titel
   \param width Relative Breite
*/

void Fl_Grid::add_column(const string &name,
                         const string &title, int width)
{
    Fl_Grid_Column col(name, title, width);
    _cols.push_back(col);
    redraw();
}

/*****************************************************************************/

/**
   Setzt die Callback-Funktion

   Die Callback-Funktion wird bei bestimmten Ereignissen
   aufgerufen. Dazu gehören:

   - Das Zeichnen einer Zelle
   - Das Selektieren/Deselektieren eines Records
   - Das Doppelklicken auf einen Record

   \param cb Zeiger auf die aufzurufende Callback-Funktion
   \param data Beim Aufruf zu übergebende Daten
*/

void Fl_Grid::callback(void (*cb)(Fl_Widget *, void *), void *data)
{
    _cb = cb;
    _cb_data = data;
}

/*****************************************************************************/

/**
   Setzt die globale Zeilenhöhe

   Alle Zeilen haben die selbe Zeilenhöhe in Pixel.
   Die Zeilenhöhe darf nicht 0 sein, da dann etliche
   Berechnungen, die durch die Zeilenhöhe teilen, scheitern würden!

   \param height Zeilenhöhe
*/

void Fl_Grid::row_height(unsigned int height)
{
    if (height != _row_height && height > 0)
    {
        _row_height = height;
        redraw();
    }
}

/*****************************************************************************/

/**
   Setzt den Inhalt der aktuellen Zelle

   Nur aufzurufen während eines Callbacks vom Typ flgContent

   \param content Anzuzeigender Inhalt
*/

void Fl_Grid::current_content(const string &content)
{
    _content = content;
}

/*****************************************************************************/

/**
   Setzt die Farbe des Inhaltes der aktuellen Zelle

   Nur aufzurufen während eines Callbacks vom Typ flgContent

   Kann mit den FLTK-Farbkonstanten (FL_RED, FL_BLACK, usw.),
   oder mit dem Konstruktor Fl_Color(R, G, B) aufgerufen
   werden.

   \param col Textfarbe
*/

void Fl_Grid::current_content_color(Fl_Color col)
{
    _content_color = col;
}

/*****************************************************************************/


void Fl_Grid::current_checked(bool checked)
{
    _checked = checked;
}

/*****************************************************************************/

/**
   Setzt die Anzahl der anzuzeigenden Records

   Wenn die neue Anzahl der Records sich von
   der Aktuellen unterscheidet, wird veranlasst,
   dass

   - alle aktuell ausgewählten Records deselektiert werden
   (die entsprechenden Callbacks werden ausgeführt)
   - die Scroll-Position ganz nach oben gesetzt wird
   - das Grid neu gezeichnet wird

   \param count Anzahl
*/

void Fl_Grid::record_count(unsigned int count)
{
    if (count != _record_count)
    {
        deselect_all();

        _record_count = count;
        _scroll_index = 0;

        redraw();
    }
}

/*****************************************************************************/

/**
   Setzt die Anzahl der anzuzeigenden Records auf 0

   \see record_count()
*/

void Fl_Grid::clear()
{
    record_count(0);
}

/*****************************************************************************/

/**
   Gibt die Anzahl der aktuell ausgewählten Records zurück

   \return Anzahl der ausgewählten Records
*/

unsigned int Fl_Grid::select_count() const
{
    return _selected.size();
}

/*****************************************************************************/

/**
   Gibt den Index des ersten, ausgewählten Records zurück

   Sollte nur im Single-Select Modus benutzt werden.

   \return Index des ausgewählten Records
   \throws char* Es ist kein Record ausgewählt
*/

unsigned int Fl_Grid::selected_index() const
{
    if (_selected.size())
    {
        return *_selected.begin();
    }
    else throw "ERROR";
}

/*****************************************************************************/

/**
   Gibt den Index des Records zurück, der momentan
   an oberster Position angezeigt wird.

   \return Record-Index
*/

unsigned int Fl_Grid::top_index() const
{
    return _scroll_index;
}

/*****************************************************************************/

/**
   Scrollt zu dem angegebenen Record-Index

   Es wird versucht, den angegebenen Record
   in der obersten Zeile erscheinen zu lassen.

   \param index Record-Index
*/

void Fl_Grid::scroll(unsigned int index)
{
    int row_count = h() / _row_height - 1;

    if (index >= _record_count - row_count)
    {
        index = _record_count - row_count;
    }

    _scroll_index = index;

    redraw();
}

/*****************************************************************************/

/**
   Setzt den Auswahl-Modus

   Folgende Werte sind möglich:

   - flgNoSelect:     Selektieren von Records ist nicht möglich
   - flgSingleSelect: Es kann immer nur ein Record selektiert
   werden
   - flgMultiSelect:  Es können mehrere Records selektiert werden

   \param mode Auswahlmodus
*/

void Fl_Grid::select_mode(Fl_Grid_Select_Mode mode)
{
    deselect_all();
    _select_mode = mode;
}

/*****************************************************************************/

/**
   Deselektiert alle momentan selektierten Records

   Die entsprechenden Callbacks werden aufgerufen!
*/

void Fl_Grid::deselect_all()
{
    list<unsigned int>::iterator sel_i;
    unsigned int index;

    _range_select_possible = false;

    while (_selected.begin() != _selected.end())
    {
        index = *_selected.begin();

        _selected.erase(_selected.begin());

        if (_cb)
        {
            _event = flgDeSelect;
            _event_record = index;
            _cb(this, _cb_data);
        }
    }
}

/*****************************************************************************/

/**
   Selektiert einen bestimmten Record

   Vorher werden alle Anderen deselektiert. Um mehrere
   Records zu selektieren, bitte select_add benutzen!

   \param index Record-Index
*/

void Fl_Grid::select(unsigned int index)
{
    deselect_all();
    select_add(index);
}

/*****************************************************************************/

/**
   Selektiert einen weiteren Record

   Fügt den angegebenen record der Liste der momentan
   ausgewählten records hinzu. Dies geht nur, wenn

   - Selektieren überhaut möglich ist
   - der angegebene Record-Index sinnvoll ist
   - der angegebene Record noch nicht selektiert ist

   Wenn der Auswahlmodus flgSingleSelect aktiv ist
   und schon ein Record selektiert ist, wird dieser
   erst deselektiert.

   \param index Record-Index
*/

void Fl_Grid::select_add(unsigned int index)
{
    if (_select_mode == flgNoSelect || index >= _record_count) return;

    if (selected(index)) return;

    if (_select_mode == flgSingleSelect) deselect_all();

    _selected.push_back(index);

    _range_select_partner = index;
    _range_select_possible = true;

    if (_cb)
    {
        _event = flgSelect;
        _event_record = index;
        _cb(this, _cb_data);
    }

    redraw();
}

/*****************************************************************************/

/**
   Prüft, ob ein angegebener Record ausgewählt ist

   \param index Record-Index
   \return true, wenn der angegebene Record ausgewählt ist
*/

bool Fl_Grid::selected(unsigned int index) const
{
    list<unsigned int>::const_iterator sel_i;

    if (_select_mode == flgNoSelect || index >= _record_count)
    {
        return false;
    }

    sel_i = _selected.begin();
    while (sel_i != _selected.end())
    {
        if (*sel_i == index) return true;
        sel_i++;
    }

    return false;
}

/*****************************************************************************/

/**
   Deselektiert den angegebenen Record

   \param index Record-Index
*/

void Fl_Grid::deselect(unsigned int index)
{
    list<unsigned int>::iterator sel_i;

    _range_select_possible = false;

    sel_i = _selected.begin();
    while (sel_i != _selected.end())
    {
        if (*sel_i == index)
        {
            _selected.erase(sel_i);

            if (_cb)
            {
                _event = flgDeSelect;
                _event_record = index;
                _cb(this, _cb_data);
            }

            return;
        }
        sel_i++;
    }
}

/*****************************************************************************/

/**
   Selektiert einen Bereich von Records

   Der Bereich wird vom letzten, selektierten
   Record bis zum aktuellen gewählt. Gab es vorher noch
   keine Selektierung, oder wurde vorher deselektiert,
   geht dies nicht.

   \param index Record-Index des angeklickten Records
*/

void Fl_Grid::_range_select(unsigned int index)
{
    unsigned int start, end;

    if (!_range_select_possible) return;

    if (index < _range_select_partner)
    {
        start = index;
        end = _range_select_partner;
    }
    else
    {
        start = _range_select_partner;
        end = index;
    }

    for (unsigned int i = start; i <= end; i++)
    {
        select_add(i);
    }

    _range_select_partner = index;
}

/*****************************************************************************/

/**
   Bestimmt, ob links neben jeder Zeile eine Checkbox sein soll

   \param check true, wenn Checkboxen angezeigt werden sollen
*/

void Fl_Grid::check_boxes(bool check)
{
    if (_check_boxes != check)
    {
        _check_boxes = check;
        redraw();
    }
}

/*****************************************************************************/

/**
   Zeichnet das Grid

   Diese Funktion wird von FLTK aufgerufen, sobald das Widget
   neu gezeichnet werden soll. Sie darf nicht "von Hand" aufgerufen
   werden, da die Zeichenfunktionen nur in einem bestimmten Kontext
   funktionieren. Um ein Neuzeichnen zu erzwingen, also bitte redraw()
   benutzen!
*/

void Fl_Grid::draw()
{
    int drawing_width, drawing_height;
    list<Fl_Grid_Column>::iterator col_i;
    unsigned int width_sum;
    float width_factor;
    unsigned int rec_index;
    bool rec_selected;
    unsigned int row_count;
    int scroll_height, scroll_pos, left, top;

    drawing_width = w() - 2 * FRAME_WIDTH;
    drawing_height = h() - 2 * FRAME_WIDTH;

    // Hintergrund zeichnen
    draw_box(FL_DOWN_BOX, FL_WHITE);

    // Fokuslinien zeichnen
    if (_focused) draw_focus();

    // Spaltenbreiten Addieren
    col_i = _cols.begin();
    width_sum = 0;
    while (col_i != _cols.end())
    {
        width_sum += col_i->width();
        col_i++;
    }

    if (width_sum == 0) return;

    // Clipping erzwingen
    fl_push_clip(x() + FRAME_WIDTH, y() + FRAME_WIDTH,
                 drawing_width, drawing_height);

    row_count = drawing_height / _row_height - 1;

    if (_record_count <= row_count)
    {
        width_factor = (_check_boxes
                        ? drawing_width - _row_height
                        : drawing_width)
            / (float) width_sum;
        _scroll_index = 0;
    }
    else // Es ist eine Scrollbar nötig
    {
        width_factor = ((_check_boxes
                         ? drawing_width - _row_height
                         : drawing_width) - 20)
            / (float) width_sum;

        if (_scroll_index > _record_count - row_count)
        {
            _scroll_index = _record_count - row_count;
        }

        // Scrollbar zeichnen
        fl_color(150, 150, 150);
        fl_rectf(x() + FRAME_WIDTH + drawing_width - 19,
                 y() + FRAME_WIDTH + 1, 18, 18);
        fl_rectf(x() + FRAME_WIDTH + drawing_width - 19,
                 y() + FRAME_WIDTH + drawing_height - 19, 18, 18);
        fl_color(200, 200, 200);
        fl_polygon(x() + FRAME_WIDTH + drawing_width - 17,
                   y() + FRAME_WIDTH + 14,
                   x() + FRAME_WIDTH + drawing_width - 5,
                   y() + FRAME_WIDTH + 14,
                   x() + FRAME_WIDTH + drawing_width - 11,
                   y() + FRAME_WIDTH + 5);
        fl_polygon(x() + FRAME_WIDTH + drawing_width - 17,
                   y() + FRAME_WIDTH + drawing_height - 14,
                   x() + FRAME_WIDTH + drawing_width - 5,
                   y() + FRAME_WIDTH + drawing_height - 14,
                   x() + FRAME_WIDTH + drawing_width - 11,
                   y() + FRAME_WIDTH + drawing_height - 5);

        scroll_height = (int) (row_count
                               / (double) _record_count // _record_count >0, da
                               * (drawing_height - 38)); // _record_count
                                                         //    > _row_count
        if (scroll_height < MIN_SCROLL_HEIGHT)
            scroll_height = MIN_SCROLL_HEIGHT;

        scroll_pos = (int) (_scroll_index
                            / (double) (_record_count
                                        - row_count) // Hier auf jeden Fall >0
                            * ((drawing_height - 38) - scroll_height));

        fl_rectf(x() + FRAME_WIDTH + drawing_width - 19,
                 y() + FRAME_WIDTH + 19 + scroll_pos, 18, scroll_height);
    }

    // Header zeichnen
    col_i = _cols.begin();
    width_sum = 0;
    while (col_i != _cols.end())
    {
        left = (int) (width_sum * width_factor)
            + (_check_boxes ? _row_height : 0) + 1;

        // Header-Hintergrund
        fl_color(200, 200, 200);
        fl_rectf(x() + FRAME_WIDTH + left,
                 y() + FRAME_WIDTH + 1,
                 (int) (col_i->width() * width_factor) - 2,
                 _row_height - 2);

        // Header-Text
        fl_color(0, 0, 0);
        fl_font(FL_HELVETICA | FL_BOLD, 12);
        fl_push_clip(x() + FRAME_WIDTH + left + 1,
                     y() + FRAME_WIDTH + 2,
                     (int) (col_i->width() * width_factor) - 4,
                     _row_height - 4);
        fl_draw(col_i->title().c_str(),
                x() + FRAME_WIDTH + left + LEFT_MARGIN,
                y() + FRAME_WIDTH + (int) (0.5 * _row_height + fl_descent()));
        fl_pop_clip();

        width_sum += col_i->width();
        col_i++;
    }

    for (unsigned int i = 0; i < _record_count; i++)
    {
        if ((i + 2) * _row_height > (unsigned int) drawing_height) break;

        if (i + _scroll_index < 0 || i + _scroll_index >= _record_count)
            continue;

        rec_index = i + _scroll_index;
        rec_selected = selected(rec_index);
        top = (i + 1) * _row_height + 1;

        if (_check_boxes)
        {
            // Zellen-Hintergrund
            if (rec_selected)
            {
                fl_color(82, 133, 156);
            }
            else
            {
                fl_color(230, 230, 230);
            }

            fl_rectf(x() + FRAME_WIDTH + 1, y() + FRAME_WIDTH + top,
                     _row_height, _row_height - 2);

            fl_color(0, 0, 0);
            fl_rect(x() + FRAME_WIDTH + 3, y() + FRAME_WIDTH + top + 2,
                    _row_height - 6, _row_height - 6);
            fl_color(255, 255, 255);
            fl_rectf(x() + FRAME_WIDTH + 4, y() + FRAME_WIDTH + top + 3,
                     _row_height - 8, _row_height - 8);

            // Checked-Status erfragen
            _checked = false;
            if (_cb)
            {
                _event = flgChecked;
                _event_record = rec_index;
                _cb(this, _cb_data);
            }

            if (_checked)
            {
                fl_color(0, 0, 0);
                fl_line(x() + FRAME_WIDTH + 4,
                        y() + FRAME_WIDTH + top + 3,
                        x() + FRAME_WIDTH + _row_height - 5,
                        y() + FRAME_WIDTH + top + _row_height - 6);
                fl_line(x() + FRAME_WIDTH + 4,
                        y() + FRAME_WIDTH + top + _row_height - 6,
                        x() + FRAME_WIDTH + _row_height - 5,
                        y() + FRAME_WIDTH + top + 3);
            }
        }

        col_i = _cols.begin();
        width_sum = 0;
        while (col_i != _cols.end())
        {
            // Zellen-Hintergrund
            if (rec_selected)
            {
                fl_color(82, 133, 156);
            }
            else
            {
                fl_color(230, 230, 230);
            }

            left = (int) (width_sum * width_factor)
                + (_check_boxes ? _row_height : 0) + 1;

            fl_rectf(x() + FRAME_WIDTH + left,
                     y() + FRAME_WIDTH + (i + 1) * _row_height + 1,
                     (int) (col_i->width() * width_factor) - 2,
                     _row_height - 2);

            // Zellen-Text
            if (rec_selected)
            {
                _content_color = fl_rgb_color(255, 255, 255);
            }
            else
            {
                _content_color = fl_rgb_color(0, 0, 0);
            }

            // Zelleninhalt erfragen
            _content = "";
            if (_cb)
            {
                _event = flgContent;
                _event_record = rec_index;
                _event_col = col_i->name();
                _event_sel = rec_selected;
                _cb(this, _cb_data);
            }

            fl_font(FL_HELVETICA, 12);
            fl_color(_content_color);
            fl_push_clip(x() + FRAME_WIDTH + left + 1,
                         y() + FRAME_WIDTH + (i + 1) * _row_height + 2,
                         (int) (col_i->width() * width_factor) - 4,
                         _row_height - 4);
            fl_draw(_content.c_str(),
                    x() + FRAME_WIDTH + left + LEFT_MARGIN,
                    y() + FRAME_WIDTH + (int) ((i + 1.5)
                                               * _row_height + fl_descent()));
            fl_pop_clip();

            width_sum += col_i->width();
            col_i++;
        }
    }

    // Clipping entfernen
    fl_pop_clip();
}

/*****************************************************************************/

/**
   Lässt das Grid FLTK-Ereignisse verarbeiten

   Diese Funktion wird nur von FLTK aufgerufen und sollte
   nicht manuell aufgerufen werden!

   \param e Ereigniscode
   \return 1, wenn das Ereignis verarbeitet wurde, sonst 0
*/

int Fl_Grid::handle(int e)
{
    int drawing_width, drawing_height;
    unsigned int record_index, row_count;
    int row_index;
    int xp, yp;
    int scroll_height, scroll_pos;

    drawing_width = w() - 2 * FRAME_WIDTH;
    drawing_height = h() - 2 * FRAME_WIDTH;

    xp = Fl::event_x() - x() - FRAME_WIDTH;
    yp = Fl::event_y() - y() - FRAME_WIDTH;

    row_count = drawing_height / _row_height - 1;

    switch (e)
    {
        case FL_PUSH:

            take_focus();

            _push_x = xp;
            _push_y = yp;

            if (_record_count > row_count && xp > drawing_width - 20)
            {
                if (yp > drawing_height - 20) // Untere Taste
                {
                    if (_scroll_index < _record_count - row_count)
                    {
                        _scroll_index++;
                        redraw();
                    }
                }
                else if (yp < 20) // Obere Taste
                {
                    if (_scroll_index > 0)
                    {
                        _scroll_index--;
                        redraw();
                    }
                }
            }

            else // Auf den Zeilenbereich geklickt
            {
                row_index = yp / _row_height - 1;
                record_index = row_index + _scroll_index;

                if (row_index >= 0
                    && row_index < (int) row_count
                    && record_index < _record_count) // Auf  Record geklickt
                {
                    if (_select_mode != flgNoSelect)
                    {
                        if (Fl::event_key(FL_Control_L)
                            || Fl::event_key(FL_Control_R))
                        {
                            if (selected(record_index)) deselect(record_index);
                            else select_add(record_index);
                        }
                        else if (Fl::event_key(FL_Shift_L)
                                 || Fl::event_key(FL_Shift_R))
                        {
                            _range_select(record_index);
                        }
                        else
                        {
                            select(record_index);
                        }
                    }

                    // Kein Selektieren möglich, aber Checkboxes, dann führt
                    // ein "Markieren" zum Checken
                    else if (_check_boxes && _cb)
                    {
                        _event = flgCheck;
                        _event_record = record_index;
                        _cb(this, _cb_data);
                        redraw();
                    }

                    if (Fl::event_clicks() == 1 && _cb)
                    {
                        Fl::event_clicks(0);

                        _event = flgDoubleClick;
                        _event_record = record_index;
                        _cb(this, _cb_data);
                    }
                }
                else
                {
                    deselect_all();
                }
            }

            return 1;

        case FL_DRAG:

            if (_record_count > row_count) // Scrollbar vorhanden
            {
                // Aktuelle Höhe und Position des Scroll-Balkens berechnen
                scroll_height = (int) (row_count
                                       / (double) _record_count
                                       * (drawing_height - 38));
                if (scroll_height < MIN_SCROLL_HEIGHT)
                    scroll_height = MIN_SCROLL_HEIGHT;
                scroll_pos = (int) (_scroll_index
                                    / (double) (_record_count - row_count)
                                    * ((drawing_height - 38) - scroll_height));

                if (_scroll_tracking)
                {
                    int new_scroll_pos = yp - 19 - _scroll_grip;

                    if (new_scroll_pos < 0)
                    {
                        new_scroll_pos = 0;
                    }
                    else if (new_scroll_pos >= drawing_height - 19)
                    {
                        new_scroll_pos = drawing_height - 19;
                    }

                    _scroll_index = (int) (new_scroll_pos
                                           / (double) (drawing_height
                                                       - 38 - scroll_height)
                                           * (_record_count - row_count));

                    redraw();
                }

                // Auf den Scrollbar-Bereich geklickt?
                else if (_push_x > drawing_width - 20)
                {
                    if (_push_y >= 19 + scroll_pos
                        && _push_y < 19 + scroll_pos + scroll_height)
                    {
                        _scroll_tracking = true;
                        _scroll_grip = _push_y - scroll_pos - 19;
                    }
                }
            }

            return 1;

        case FL_RELEASE:
            _scroll_tracking = false;
            return 1;

        case FL_MOUSEWHEEL:

            take_focus();

            if (Fl::event_dy() > 0) // Untere Taste
            {
                if (_scroll_index < _record_count - row_count)
                {
                    _scroll_index++;
                    redraw();
                }
            }
            else if (Fl::event_dy() < 0) // Obere Taste
            {
                if (_scroll_index > 0)
                {
                    _scroll_index--;
                    redraw();
                }
            }
            return 1;

        case FL_KEYDOWN:

            if (Fl::event_key() == FL_Down)
            {
                if (_scroll_index < _record_count - row_count)
                {
                    _scroll_index++;
                    redraw();
                }
                return 1;
            }
            else if (Fl::event_key() == FL_Up)
            {
                if (_scroll_index > 0)
                {
                    _scroll_index--;
                    redraw();
                }
                return 1;
            }
            else if (Fl::event_key() == FL_Page_Down)
            {
                if (_scroll_index
                    < _record_count - row_count) // Ist noch nicht am Ende
                {
                    if (row_count > _record_count - _scroll_index - row_count)
                    {
                        _scroll_index = _record_count - row_count;
                    }
                    else
                    {
                        _scroll_index += row_count;
                    }

                    redraw();
                }
                return 1;
            }
            else if (Fl::event_key() == FL_Page_Up)
            {
                if (_scroll_index > 0)
                {
                    if (_scroll_index > row_count)
                    {
                        _scroll_index -= row_count;
                    }
                    else
                    {
                        _scroll_index = 0;
                    }

                    redraw();
                }

                return 1;
            }

            return 0;

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
}

/*****************************************************************************/
