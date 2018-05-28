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

#ifndef LibDLSChannelPreset
#define LibDLSChannelPreset

/*****************************************************************************/

#include <string>

/*****************************************************************************/

#include "Exception.h"
#include "globals.h"

/*****************************************************************************/

namespace LibDLS {

/*****************************************************************************/

class XmlTag;

/*****************************************************************************/

/** Channel preset exception.
 */
class EChannelPreset:
    public Exception
{
    public:
        EChannelPreset(const std::string &pmsg):
            Exception(pmsg) {};
};

/*****************************************************************************/

/** Channel preset.
 *
 * Enthält Kanalname, Abtastrate, Blockgröße, Meta-Vorgaben und das Format, in
 * dem die Daten gespeichert werden sollen.
 */
class ChannelPreset
{
    public:
        ChannelPreset();
        ~ChannelPreset();

        bool operator!=(const ChannelPreset &) const;

        void read_from_tag(const XmlTag *);
        void write_to_tag(XmlTag *) const;

        void clear();

        std::string name; /**< Kanalname */
        double sample_frequency; /**< Abtastrate, mit der aufgezeichnet werden
                                   soll */
        unsigned int block_size; /**< Blockgröße, mit der aufgezeichnet werden
                                   soll */
        unsigned int meta_mask; /**< Bitmaske mit den aufzuzeichnenden
                                  Meta-Typen */
        unsigned int meta_reduction; /**< Meta-Untersetzung */
        int format_index; /**< Index des Formates zum Speichern der Daten */
        unsigned int mdct_block_size; /**< Blockgröße für MDCT */
        double accuracy; /**< Genauigkeit von verlustbehafteten Kompressionen
                          */
        ChannelType type; /**< Datentyp des Kanals (nur für MDCT-Prüfung) */
};

/*****************************************************************************/

} // namespace

/*****************************************************************************/

#endif
