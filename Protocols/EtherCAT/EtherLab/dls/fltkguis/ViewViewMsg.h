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

#ifndef ViewViewMsgHpp
#define ViewViewMsgHpp

/*****************************************************************************/

#include <list>
using namespace std;

#include <FL/Fl_Widget.H>

/*****************************************************************************/

#include "lib/LibDLS/Time.h"
#include "lib/LibDLS/Job.h"

#include "fl_track_bar.h"

/*****************************************************************************/

/**
   Widget zum Anzeigen der MSR-Nachrichten
*/

class ViewViewMsg : public Fl_Widget
{
public:
    ViewViewMsg(int, int, int, int, const char * = "");
    ~ViewViewMsg();

    void load_msg(const LibDLS::Job *, LibDLS::Time, LibDLS::Time);
    void clear();

private:
    Fl_Track_Bar *_track_bar; /**< Vertikale Scroll-Leiste */

    // Daten
    struct Message {
        LibDLS::Job::Message message;
        int level;
    };
    list<Message> _messages; /**< Liste der geladenen Nachrichten */
    LibDLS::Time _range_start; /**< Startzeit der anzuzeigenden Zeitspanne */
    LibDLS::Time _range_end; /**< Endzeit der anzuzeigenden Zeitspanne */
    int _level_count; /**< Aktuelle Anzahl der anzuzeigenden Ebenen */

    // Widget-Zustand
    bool _focused; /**< Das Widget hat gerade den Fokus */

    virtual void draw();
    virtual int handle(int);

    void _calc_msg_levels();
};

/*****************************************************************************/

#endif
