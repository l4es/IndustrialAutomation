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

#ifndef LibDLSJobPresetH
#define LibDLSJobPresetH

/*****************************************************************************/

#include <stdint.h>

#include <string>
#include <vector>

/*****************************************************************************/

#include "Exception.h"
#include "ChannelPreset.h"

/*****************************************************************************/

namespace DlsProto {
	class JobPresetInfo;
}

namespace LibDLS {

/*****************************************************************************/

/** Job preset exception.
 */
class EJobPreset:
    public Exception
{
    public:
        EJobPreset(const std::string &pmsg):
            Exception(pmsg) {};
};

/*****************************************************************************/

/**
   Auftragsvorgaben mit Liste der Kanalvorgaben

   Enthält Beschreibung, Zustand, Datenquelle, Trigger, usw.
*/

class JobPreset
{
    public:
        JobPreset();
        ~JobPreset();

        void import(const std::string &, unsigned int);
        void import_from(const DlsProto::JobPresetInfo &);

        unsigned int id() const { return _id; }
        const std::string &description() const { return _description; }
        std::string id_desc() const;
        const std::string &owner() const { return _owner; }
        bool running() const { return _running; }
        uint64_t quota_time() const { return _quota_time; }
        uint64_t quota_size() const { return _quota_size; }
        const std::string &source() const { return _source; }
        uint16_t port() const { return _port; }
        const std::string &trigger() const { return _trigger; }
        const std::vector<ChannelPreset> *channels() const {
            return &_channels;
        }
        bool channel_exists(const std::string &) const;
        void set_job_preset_info(DlsProto::JobPresetInfo *) const;

    protected:
        unsigned int _id; /**< Job ID. */
        std::string _description; /**< Beschreibender Name des Auftrages */
        std::string _owner; /**< Besitzer des Auftrages \todo Nicht genutzt */
        bool _running; /**< Soll erfasst werden? */
        uint64_t _quota_time; /**< Auftrags-Quota nach Zeit */
        uint64_t _quota_size; /**< Auftrags-Quota nach Datengröße */
        std::string _source; /**< IP-Adresse oder Hostname der Datenquelle */
        uint16_t _port; /**< Port der Datenquelle. */
        std::string _trigger; /**< Name des Trigger-Parameters, andernfalls
                                leer */
        std::vector<ChannelPreset> _channels; /**< Liste der Kanalvorgaben */
};

/*****************************************************************************/

} // namespace

/*****************************************************************************/

#endif
