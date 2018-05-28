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

#ifndef CtlDialogChannelH
#define CtlDialogChannelH

/*****************************************************************************/

#include <list>
using namespace std;

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Choice.H>

/*****************************************************************************/

#include "CtlJobPreset.h"

/*****************************************************************************/

/**
   Dialog zum Ändern der Eigenschaften von zu erfassenden Kanälen

   Über diese Eingabemaske kann der Benutzer die Abtastrate,
   Meta-Untersetzung, Kompression usw. eines Kanals bestimmen.
*/

class CtlDialogChannel
{
public:
    CtlDialogChannel(const string &);
    ~CtlDialogChannel();

    void show(CtlJobPreset *, const list<const LibDLS::ChannelPreset *> *);
    bool updated() const;

private:
    Fl_Double_Window *_wnd;       /**< Dialogfenster */
    Fl_Return_Button *_button_ok; /**< "OK"-Button */
    Fl_Button *_button_cancel;    /**< "Abbrechen"-Button */
    Fl_Input *_input_freq;        /**< Eingabefeld für die Abtastfrequenz */
    Fl_Input *_input_block;       /**< Eingabefeld für die Blockgröße */
    Fl_Input *_input_mask;        /**< Eingabefeld für die Meta-Maske */
    Fl_Input *_input_red;         /**< Eingabefeld für die Meta-Untersetzung */
    Fl_Choice *_choice_format;    /**< Auswahlfeld für die Kompression */
    Fl_Choice *_choice_mdct;      /**< Auswahlfeld für die MDCT-Blockgröße */
    Fl_Input *_input_accuracy;    /**< Eingabefeld für die MDCT-Genauigkeit */

    string _dls_dir; /**< DLS-Datenverzeichnis */
    CtlJobPreset *_job; /**< Zeiger auf das Auftrags-Objekt */
    const list<const LibDLS::ChannelPreset *> *_channels; /**< Liste der zu
                                                        ändernden Kanäle */
    bool _updated; /**< Es wurden Kanäle geändert */
    bool _choice_format_selected; /**< Es wurde ein Format ausgewählt */
    bool _choice_mdct_selected; /**< Es wurde eine MDCT-Blockgröße gewählt */

    static void _callback(Fl_Widget *, void *);
    void _button_ok_clicked();
    void _button_cancel_clicked();
    void _choice_format_changed();
    void _choice_mdct_changed();

    void _load_channel();
    bool _save_channels();
};

/*****************************************************************************/

/**
   Gibt zurück, ob Kanäle geändert wurden

   \return true, wenn Kanäle geändert wurden
*/

inline bool CtlDialogChannel::updated() const
{
    return _updated;
}

/*****************************************************************************/

#endif
