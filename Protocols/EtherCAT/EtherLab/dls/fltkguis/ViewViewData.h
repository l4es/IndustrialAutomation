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

#ifndef ViewViewDataHpp
#define ViewViewDataHpp

/*****************************************************************************/

#include <list>
using namespace std;

#include <FL/Fl_Widget.H>

/*****************************************************************************/

#include "lib/LibDLS/Time.h"
#include "lib/LibDLS/Job.h"

#include "fl_track_bar.h"
#include "ViewChannel.h"

/*****************************************************************************/

/**
   Bereich eines Chunks zur Anzeige der nicht erfassten Bereiche
*/

struct ViewViewDataChunkRange
{
    LibDLS::Time start;
    LibDLS::Time end;
};

/*****************************************************************************/

/**
 */

struct ScanInfo
{
    bool found;
    double min_value;
    double max_value;
    int snapped_x;
};

/*****************************************************************************/

/**
   Anzeige für Messdaten

   Diese Anzeige kann die Daten mehrerer Kanäle
   übereinander darstellen. Diese werden auf die
   volle Widget-Höhe verteilt. Würden, die einzelnen
   Kanäle zu schmal werden, wird eine Mindesthöhe
   für jeden Kanal verwendet und eine Track-Bar
   angezeigt. Das Widget unterstützt Interaktionen
   mit Maus und Keyboard
*/

class ViewViewData : public Fl_Widget
{
public:
    ViewViewData(int, int, int, int, const char * = "");
    ~ViewViewData();

    void add_channel(LibDLS::Channel *);
    void rem_channel(const LibDLS::Channel *);
    bool has_channel(const LibDLS::Channel *) const;
    void clear();
    void full_range();
    void update();
    void range_callback(void (*)(LibDLS::Time, LibDLS::Time, void *), void *);

    LibDLS::Time start() const;
    LibDLS::Time end() const;

private:
    Fl_Track_Bar *_track_bar;    /**< Vertikale Track-Bar */
    list<ViewChannel> _channels; /**< Liste der zu Zeigenden Kanäle */
    LibDLS::Time _range_start;   /**< Startzeit der anzuzeigenden Zeitspanne */
    LibDLS::Time _range_end;     /**< Endzeit der anzuzeigenden Zeitspanne */
    bool _full_range;            /**< Gibt an, ob beim Hinzufügen eines
                                    Kanales die Zeitspanne auf die volle
                                    Zeitspanne ausgeweitet werden soll. */

    // Callbacks
    void (*_range_cb)(LibDLS::Time, LibDLS::Time, void *); /**<
                                                             Funktionszeiger
                                                             auf Callback bei
                                                             Zeitbereichs-
                                                             änderungen */
    void *_range_cb_data;                        /**< Daten zur Übergabe bei
                                                    Range-CB */

    // Widget-Zustand
    bool _focused;       /**< Das Widget hält momentan den Fokus */
    bool _zooming;       /**< Der Benutzer zieht einen Zoom-Bereich auf */
    bool _moving;        /**< Der Benutzer zieht einen Verschiebungs-Pfeil */
    bool _scanning;      /**< Der Benutzer lässt die Scan-Linie anzeigen */
    int _scan_x;         /**< X-Position der Scan-Linie */
    int _start_x;        /**< X-Position des Cursors zu Beginn einer Aktion */
    int _start_y;        /**< Y-Position des Cursors zu Beginn einer Aktion */
    int _end_x;          /**< X-Position des Cursors zum Ende einer Aktion */
    int _end_y;          /**< Y-Position des Cursors zum Ende einer Aktion */
    bool _mouse_in;      /**< Der Mauscursor befindet sich auf dem Widget */
    bool _do_not_draw;   /**< Flag: Der Inhalt soll nicht neu
                            gezeichnet werden */

    // Temporäre Größen zum Zeichnen
    int _channel_area_width;  /**< Breite des Zeichenbereiches
                                 für die Kanäle */
    int _channel_area_height; /**< Höhe des zeichenbereiches für die Kanäle */
    int _channel_height;      /**< Höhe einer Kanalzeile, incl. Textbox */
    int _scroll_pos;          /**< Anzeige-Offset (abhängig von Track-Bar) */

    // Private Methoden
    void _load_data();
    void _calc_range();
    void _draw_gaps(const ViewChannel *, int, int, int, int, double) const;
    void _draw_time_scale(unsigned int, unsigned int,
                          unsigned int, unsigned int, double) const;
    void _draw_scroll_bar(unsigned int, unsigned int,
                          unsigned int, unsigned int) const;
    void _draw_channel(const ViewChannel *, int, int, int, int, double) const;
    void _draw_gen(const ViewChannel *, ScanInfo *, int, int, int, int,
                   double, double, double) const;
    void _draw_min_max(const ViewChannel *, ScanInfo *,
                       int, int, int, int, double, double, double) const;
    void _draw_interactions(double) const;
    void _draw_scan(const ViewChannel *, ScanInfo *, int, int, int, int,
                    double, double, double) const;

    // Prädikatsfunktion zum Sortieren
    static bool range_before(const ViewViewDataChunkRange &,
                             const ViewViewDataChunkRange &);

    // FLTK
    virtual void draw();
    virtual int handle(int);
};

/*****************************************************************************/

inline LibDLS::Time ViewViewData::start() const
{
    return _range_start;
}

/*****************************************************************************/

inline LibDLS::Time ViewViewData::end() const
{
    return _range_end;
}

/*****************************************************************************/

#endif
