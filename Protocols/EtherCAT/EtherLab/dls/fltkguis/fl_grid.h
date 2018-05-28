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

#ifndef FlGridHpp
#define FlGridHpp

/*****************************************************************************/

#include <string>
#include <list>
using namespace std;

/*****************************************************************************/

#include <FL/Fl_Widget.H>

/*****************************************************************************/

enum Fl_Grid_Event
{
    flgContent,
    flgChecked,
    flgSelect,
    flgDeSelect,
    flgDoubleClick,
    flgCheck
};

enum Fl_Grid_Select_Mode
{
    flgNoSelect,
    flgSingleSelect,
    flgMultiSelect
};

/*****************************************************************************/

/**
   Anzuzeigende Spalte in einem Fl_Grid
*/

class Fl_Grid_Column
{
public:
    Fl_Grid_Column(const string &, const string &, int);
    ~Fl_Grid_Column();

    const string &name() const;
    const string &title() const;
    unsigned int width() const;

private:
    string _name;        /**< Identifizierender Spaltenname */
    string _title;       /**< Angezeigter Spaltentitel */
    unsigned int _width; /**< Anteilige Breite der Spalte */

    Fl_Grid_Column();
};

/*****************************************************************************/

/**
   Tabellenobjekt für FLTK

   Zeigt eine Tabelle mit Daten in Spalten und Zeilen an.
   Unterstützt Auswahl von Zeilen, eine automatische
   vertikale Scrolling-Leiste und farbliche Kennzeichnung
   von Zellenwerten.
*/

class Fl_Grid : public Fl_Widget
{
public:
    Fl_Grid(int, int, int, int, const char * = "");
    ~Fl_Grid();

    // Spaltenverwaltung
    void add_column(const string &, const string & = "", int = 100);

    // Inhalt
    void record_count(unsigned int);
    unsigned int record_count() const;
    void clear();

    // Callback
    void callback(void (*)(Fl_Widget *, void *), void *);

    // Aussehen
    void row_height(unsigned int);

    // Event handling
    Fl_Grid_Event current_event() const;
    unsigned int current_record() const;
    const string &current_col() const;
    bool current_selected() const;
    void current_content(const string &);
    void current_content_color(Fl_Color);
    void current_checked(bool);

    // Auswahl
    unsigned int select_count() const;
    unsigned int selected_index() const;
    const list<unsigned int> *selected_list() const;
    void select_mode(Fl_Grid_Select_Mode);
    void select(unsigned int);
    void select_add(unsigned int);
    void deselect(unsigned int);
    void deselect_all();
    bool selected(unsigned int) const;

    // Scrolling
    unsigned int top_index() const;
    void scroll(unsigned int);

    // Checkboxes
    void check_boxes(bool);

protected:
    bool _focused; /**< true, wenn das Grid gerade den Fensterfokus besitzt */
    list<Fl_Grid_Column> _cols; /**< Liste der anzuzeigenden Spalten */
    unsigned int _record_count; /**< Anzahl der aktuellen Zeilen */
    void (*_cb)(Fl_Widget *, void *); /**< Zeiger auf die Callback-Funktion */
    void *_cb_data; /**< Bei einem Callback übergebene Daten */
    string _content; /**< Zu zeichnender Zelleninhalt */
    Fl_Color _content_color; /**< Farbe, in der der Zelleninhalt
                                gezeichnet werden soll */
    bool _checked;
    Fl_Grid_Event _event; /**< Art des Callback-Events */
    unsigned int _event_record; /**< Record-Index des aktuellen Events */
    string _event_col; /**< Spalten-Identifier des aktuellen Events */
    bool _event_sel; /**< true, wenn die Zeile des aktuellen Events
                        selekiert ist */
    Fl_Grid_Select_Mode _select_mode; /**< Auswahlmodus: Keine, nur eine,
                                         oder mehrere Zeilen */
    unsigned int _row_height; /**< Höhe aller Zeilen in Pixel */
    unsigned int _scroll_index; /**< Index der zu oberst angezeigten Zeile */
    list<unsigned int> _selected; /**< Liste der Indizes der
                                     angewählten Records */
    int _push_x, _push_y; /**< Position des letzten Mausklicks auf dem Grid */
    bool _scroll_tracking; /**< true, wenn der Benutzer gerade
                              den Scroll-Balken zieht */
    int _scroll_grip; /**< Vertikale Position des Mauscursors
                         auf dem Scroll-Balken */
    bool _range_select_possible; /**< true, wenn gerade eine
                                    Bereichsauswahl möglich ist */
    unsigned int _range_select_partner; /**< Anfangs- oder Endindex
                                           der aktuell möglichen
                                           Bereichsauswahl */
    bool _check_boxes; /**< true, wenn vor jeder Zeile
                          eine Checkbox erscheinen soll */

    void _range_select(unsigned int);

    virtual void draw();
    virtual int handle(int);
};

/*****************************************************************************/

/**
   Gibt die anteilige Breite einer Spalte zurück

   \return Anteilige Breite
*/

inline unsigned int Fl_Grid_Column::width() const
{
    return _width;
}

/*****************************************************************************/

/**
   Gibt den Titel einer Spalte zurück

   \return Spaltentitel
*/

inline const string &Fl_Grid_Column::title() const
{
    return _title;
}

/*****************************************************************************/

/**
   Gibt identifiziernden Namen einer Spalte zurück

   \return Name
*/

inline const string &Fl_Grid_Column::name() const
{
    return _name;
}

/*****************************************************************************/

/**
   Gibt den Ereignistyp zurück

   Gibt während eines Callbacks den Typ des Ereignisses zurück,
   dass den Callback ausgelöst hat. Dies kann folgendes sein:

   - flgContent:     Der Callback erwartet einen Aufruf
   von current_content(), um den Zelleninhalt
   zu bekommen
   - flgSelect:      Der Callback teilt mit, dass ein Record
   selektiert wurde
   - flgDeSelect:    Der Callback teilt mit, dass ein Record
   nicht mehr selektiert ist
   - flgDoubleClick: Der Callback teilt mit, dass ein
   Doppelklick auf einen Record gemacht
   wurde

   \return Ereignistyp
*/

inline Fl_Grid_Event Fl_Grid::current_event() const
{
    return _event;
}

/*****************************************************************************/

/**
   Gibt den Index des Records eines Callbacks an

   \return Record-Index
*/

inline unsigned int Fl_Grid::current_record() const
{
    return _event_record;
}

/*****************************************************************************/

/**
   Gibt die Spalte in einem Callback zurück

   Gibt während eines Callbacks mit dem Event-Typ
   flgContent den identifizierenden Namen der
   betreffenden Spalte zurück

   \return Spalten-Identifier
*/

inline const string &Fl_Grid::current_col() const
{
    return _event_col;
}

/*****************************************************************************/

/**
   Gibt während eines Callbacks mit dem Event-Typ
   flgContent an, ob der entsprechende Record
   gerade selektiert ist

   \return Record-Index
*/

inline bool Fl_Grid::current_selected() const
{
    return _event_sel;
}

/*****************************************************************************/

/**
   Gibt einen konstanten Zeiger auf die Liste der
   Indizes der selektierten Records zurück

   \return Record-Index
*/

inline const list<unsigned int> *Fl_Grid::selected_list() const
{
    return &_selected;
}

/*****************************************************************************/

#endif
