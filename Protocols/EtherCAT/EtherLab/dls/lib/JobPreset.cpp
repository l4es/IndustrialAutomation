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

#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <sstream>
#include <fstream>
using namespace std;

/*****************************************************************************/

#include "LibDLS/globals.h"
#include "LibDLS/JobPreset.h"

#include "proto/dls.pb.h"

#include "XmlParser.h"

using namespace LibDLS;

/*****************************************************************************/

/**
   Konstruktor
*/

JobPreset::JobPreset():
    _id(0),
    _running(false),
    _quota_time(0),
    _quota_size(0),
    _port(MSR_PORT)
{
}

/*****************************************************************************/

/**
   Destruktor
*/

JobPreset::~JobPreset()
{
}

/*****************************************************************************/

/**
   Importiert Auftragsvorgaben aus einer XML-Datei

   Der Dateiname wird aus dem Datenverzeichnis und der
   Auftrags-ID generiert. Die Datei wird geöffnet und ge'parse't.

   \param dls_dir Das DLS-Datenverzeichnis
   \param id Auftrags-ID
   \throw EJobPreset Fehler während des Importierens
*/

void JobPreset::import(const string &dls_dir, unsigned int id)
{
    ChannelPreset channel;
    string value;
    stringstream file_name;
    fstream file;
    XmlParser parser;
    stringstream err;
    const XmlTag *tag;

    _id = id;

    _channels.clear();

    // Dateinamen konstruieren
    file_name << dls_dir << "/job" << id << "/job.xml";

    // Datei öffnen
    file.open(file_name.str().c_str(), ios::in);

    if (!file.is_open())
    {
        err << "Could not open file \"" << file_name.str() << "\"";
        throw EJobPreset(err.str());
    }

    try
    {
        parser.parse(&file, "dlsjob", dxttBegin);

        _description = parser.parse(&file, "description",
                                    dxttSingle)->att("text")->to_str();

        value = parser.parse(&file, "state",
                             dxttSingle)->att("name")->to_str();

        if (value == "running") _running = true;
        else if (value == "paused") _running = false;
        else
        {
            file.close();
            throw EJobPreset("Unknown state \"" + value + "\"!");
        }

        tag = parser.parse(&file, "source",
                               dxttSingle);
        _source = tag->att("address")->to_str();
        if (tag->has_att("port")) {
            _port = tag->att("port")->to_int();
        } else {
            _port = MSR_PORT;
        }

        parser.parse(&file, "quota", dxttSingle);

        if (parser.tag()->has_att("time"))
        {
            _quota_time = parser.tag()->att("time")->to_uint64();
        }
        else
        {
            _quota_time = 0;
        }

        if (parser.tag()->has_att("size"))
        {
            _quota_size = parser.tag()->att("size")->to_uint64();
        }
        else
        {
            _quota_size = 0;
        }

        _trigger = parser.parse(&file, "trigger",
                                dxttSingle)->att("parameter")->to_str();

        parser.parse(&file, "channels", dxttBegin);

        while (1)
        {
            parser.parse(&file);

            if (parser.tag()->title() == "channels"
                && parser.tag()->type() == dxttEnd)
            {
                break;
            }

            if (parser.tag()->title() == "channel"
                && parser.tag()->type() == dxttSingle)
            {
                try
                {
                    channel.read_from_tag(parser.tag());
                }
                catch (EChannelPreset &e)
                {
                    file.close();
                    err << "Error reading channel: " << e.msg;
                    throw EJobPreset(err.str());
                }

                _channels.push_back(channel);
            }
            else
            {
                file.close();
                err << "Expected channel/ or /channels!";
                throw EJobPreset(err.str());
            }
        }

        parser.parse(&file, "dlsjob", dxttEnd);
    }
    catch (EXmlParser &e)
    {
        file.close();
        err << "Parsing: " << e.msg;
        throw EJobPreset(err.str());
    }
    catch (EXmlParserEOF &e)
    {
        file.close();
        err << "Parsing: " << e.msg;
        throw EJobPreset(err.str());
    }
    catch (EXmlTag &e)
    {
        file.close();
        err << "Tag: " << e.msg;
        throw EJobPreset(err.str());
    }

    file.close();
}

/*****************************************************************************/

void JobPreset::import_from(const DlsProto::JobPresetInfo &info)
{
    _id = info.id();
    _description = info.description();
    _running = info.running();
    _quota_time = info.quota_time();
    _quota_size = info.quota_size();
    _source = info.source();
    _port = info.port();
    _trigger = info.trigger();

    _channels.clear();
    // FIXME channels
}

/*****************************************************************************/

/**
   Prüfen, ob ein bestimmter Kanal in den Vorgaben existiert

   \param name Kanalname
   \return true, wenn der Kanal in den Vorgaben existiert
*/

bool JobPreset::channel_exists(const string &name) const
{
    vector<ChannelPreset>::const_iterator channel = _channels.begin();
    while (channel != _channels.end())
    {
        if (channel->name == name) return true;
        channel++;
    }

    return false;
}

/*****************************************************************************/

/**
   Erstellt einen String aus ID und Beschreibung

   Format "(<ID>) <Beschreibung>"

   \returns String
   \see _id
   \see _description
*/

string JobPreset::id_desc() const
{
    stringstream ret;

    ret << "(" << _id << ") " << _description;

    return ret.str();
}

/*****************************************************************************/

void JobPreset::set_job_preset_info(DlsProto::JobPresetInfo *info) const
{
    info->set_id(_id);
    info->set_description(_description);
    info->set_running(_running);
    info->set_quota_time(_quota_time);
    info->set_quota_size(_quota_size);
    info->set_source(_source);
    info->set_port(_port);
    info->set_trigger(_trigger);

    // FIXME channels
}

/*****************************************************************************/
