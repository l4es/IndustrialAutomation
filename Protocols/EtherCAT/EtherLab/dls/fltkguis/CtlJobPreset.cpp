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
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sstream>
#include <fstream>
using namespace std;


#include "lib/LibDLS/globals.h"
#include "lib/XmlTag.h"

#include "CtlJobPreset.h"

/*****************************************************************************/

/**
   Konstruktor
*/

CtlJobPreset::CtlJobPreset():
    LibDLS::JobPreset()
{
    process_watchdog = 0;
    logging_watchdog = 0;
};

/*****************************************************************************/

/**
   Auftragsvorgaben in XML-Datei speichern

   Schreibt alle Auftragsvorgaben (inclusive Kanalvorgaben)
   in eine XML-Datei im richtigen Verzeichnis. Danach wird
   der Mutterprozess über eine Spooling-Datei benachrichtigt,
   dass er die Vorgaben neu einlesen soll.

   \param dls_dir DLS-Datenverzeichnis
   \throw EJobPreset Ungültige Daten oder Schreiben nicht möglich
*/

void CtlJobPreset::write(const string &dls_dir)
{
    stringstream dir_name, err;
    string file_name;
    fstream file;
    LibDLS::XmlTag tag;
    vector<LibDLS::ChannelPreset>::iterator channel_i;
    int fd;
    char *tmp_file_name;

    // Verzeichnisnamen konstruieren
    dir_name << dls_dir << "/job" << _id;

    if (mkdir(dir_name.str().c_str(), 0755) != 0)
    {
        if (errno != EEXIST)
        {
            err << "Could not create \"" << dir_name.str()
                << "\" (errno " << errno << ")!";
            throw LibDLS::EJobPreset(err.str());
        }
    }

    // Dateinamen konstruieren
    file_name = dir_name.str() + "/job.xml";

    try {
        tmp_file_name = new char[strlen(file_name.c_str()) + 8];
    }
    catch (...) {
        err << "Failed to allocate memory for file name!";
        throw LibDLS::EJobPreset(err.str());
    }

    sprintf(tmp_file_name, "%s.XXXXXX", file_name.c_str());

    fd = mkstemp(tmp_file_name);
    if (fd == -1) {
        err << "Could not create \"" << tmp_file_name
            << "\": " << strerror(errno);
        delete [] tmp_file_name;
        throw LibDLS::EJobPreset(err.str());
    }

    if (chmod(tmp_file_name, 0644) == -1) {
        err << "Could not change rights of \"" << tmp_file_name
            << "\": " << strerror(errno);
        delete [] tmp_file_name;
        throw LibDLS::EJobPreset(err.str());
    }

    // Datei öffnen
    file.open(tmp_file_name, ios::out);
    close(fd);

    if (!file.is_open())
    {
        err << "Could not attach to file \"" << tmp_file_name << "\"";
        delete [] tmp_file_name;
        throw LibDLS::EJobPreset(err.str());
    }

    file.exceptions(fstream::failbit | fstream::badbit);

    try
    {
        tag.clear();
        tag.type(LibDLS::dxttBegin);
        tag.title("dlsjob");
        file << tag.tag() << endl;

        tag.clear();
        tag.title("description");
        tag.push_att("text", _description);
        file << " " << tag.tag() << endl;

        tag.clear();
        tag.title("state");
        tag.push_att("name", _running ? "running" : "paused");
        file << " " << tag.tag() << endl;

        tag.clear();
        tag.title("source");
        tag.push_att("address", _source);
        tag.push_att("port", _port);
        file << " " << tag.tag() << endl;

        tag.clear();
        tag.title("quota");
        tag.push_att("size", _quota_size);
        tag.push_att("time", _quota_time);
        file << " " << tag.tag() << endl;

        tag.clear();
        tag.title("trigger");
        tag.push_att("parameter", _trigger);
        file << " " << tag.tag() << endl;

        file << endl;

        tag.clear();
        tag.title("channels");
        tag.type(LibDLS::dxttBegin);
        file << " " << tag.tag() << endl;

        channel_i = _channels.begin();
        while (channel_i != _channels.end())
        {
            channel_i->write_to_tag(&tag);
            file << "  " << tag.tag() << endl;
            channel_i++;
        }

        tag.clear();
        tag.title("channels");
        tag.type(LibDLS::dxttEnd);
        file << " " << tag.tag() << endl;

        tag.clear();
        tag.title("dlsjob");
        tag.type(LibDLS::dxttEnd);
        file << tag.tag() << endl;
    }
    catch (LibDLS::EChannelPreset &e)
    {
        file.close();
        unlink(tmp_file_name);
        delete [] tmp_file_name;
        err << "Failed to write: " << e.msg;
        throw LibDLS::EJobPreset(err.str());
    }
    catch (ios_base::failure &e)
    {
        // file.close() throws exception
        unlink(tmp_file_name);
        delete [] tmp_file_name;
        err << "Failed to write: " << e.what();
        throw LibDLS::EJobPreset(err.str());
    }

    file.close();

    if (rename(tmp_file_name, file_name.c_str()) == -1) {
        delete [] tmp_file_name;
        err << "Failed to rename " << tmp_file_name << " to "
            << file_name << ": " << strerror(errno);
        throw LibDLS::EJobPreset(err.str());
    }

    delete [] tmp_file_name;
}

/*****************************************************************************/

/**
   Spooling-Datei erzeugen, um den Prozess zu benachrichtigen

   Diese Methode benachrichtigt den Prozess bezüglich
   einer Änderung einer Auftragsvorgabe. Es kann z. B.
   eine neue Vorgabe erstellt worden sein, eine
   vorhandene Vorgabe kann sich geändert haben, oder es
   wurde eine Vorgabe gelöscht.

   Die Spooling-Datei enthält nur die ID des Auftrags. Der
   Prozess entscheidet damit selbstständig, was zu tun ist.

   \param dls_dir DLS-Datenverzeichnis
*/

void CtlJobPreset::spool(const string &dls_dir)
{
    fstream file;
    stringstream filename, err;
    struct timeval tv;

    gettimeofday(&tv, 0);

    // Eindeutigen Dateinamen erzeugen
    filename << dls_dir << "/spool/";
    filename << tv.tv_sec << "_" << tv.tv_usec;
    filename << "_" << (unsigned long) this;

    file.open(filename.str().c_str(), ios::out);

    if (!file.is_open())
    {
        err << "Could not write spooling file \"" << filename.str() << "\"!";
        throw LibDLS::EJobPreset(err.str());
    }

    file << _id << endl;
    file.close();
}

/*****************************************************************************/

/**
   Setzt die Auftrags-ID
*/

void CtlJobPreset::id(unsigned int id)
{
    _id = id;
}

/*****************************************************************************/

/**
   Setzt die Auftragsbeschreibung
*/

void CtlJobPreset::description(const string &desc)
{
    _description = desc;
}

/*****************************************************************************/

/**
   Setzt den Sollstatus
*/

void CtlJobPreset::running(bool run)
{
    _running = run;
}

/*****************************************************************************/

/**
   Wechselt den Sollstatus
*/

void CtlJobPreset::toggle_running()
{
    _running = !_running;
}

/*****************************************************************************/

/**
   Setzt die Datenquelle
*/

void CtlJobPreset::source(const string &src)
{
    _source = src;
}

/*****************************************************************************/

/**
   Setzt den Namen des Trigger-Parameters
*/

void CtlJobPreset::trigger(const string &trigger_name)
{
    _trigger = trigger_name;
}

/*****************************************************************************/

/**
   Setzt die Größe der Zeit-Quota

   \param seconds Anzahl der maximal zu erfassenden Sekunden
*/

void CtlJobPreset::quota_time(uint64_t seconds)
{
    _quota_time = seconds;
}

/*****************************************************************************/

/**
   Setzt die Größe der Daten-Quota

   \param bytes Anzahl der maximal zu erfassenden Bytes
*/

void CtlJobPreset::quota_size(uint64_t bytes)
{
    _quota_size = bytes;
}

/*****************************************************************************/

/**
   Fügt eine Kanalvorgabe hinzu

   \param channel Neue Kanalvorgabe
   \throw EJobPreset Eine Vorgabe für diesen Kanal
   existiert bereits!
*/

void CtlJobPreset::add_channel(const LibDLS::ChannelPreset *channel)
{
    stringstream err;

    if (channel_exists(channel->name))
    {
        err << "Channel \"" << channel->name << "\" already exists!";
        throw LibDLS::EJobPreset(err.str());
    }

    _channels.push_back(*channel);
}

/*****************************************************************************/

/**
   Ändert eine Kanalvorgabe

   Der zu ändernde Kanal wird anhand des Kanalnamens in der
   neuen Vorgabe bestimmt.

   \param new_channel Zeiger auf neue KanalvorgabeKanalname
   \throw EJobPreset Es existiert keine Vorgabe für
   den angegebenen Kanal
*/

void CtlJobPreset::change_channel(const LibDLS::ChannelPreset *new_channel)
{
    vector<LibDLS::ChannelPreset>::iterator channel_i;
    stringstream err;

    channel_i = _channels.begin();
    while (channel_i != _channels.end())
    {
        if (channel_i->name == new_channel->name)
        {
            *channel_i = *new_channel;
            return;
        }

        channel_i++;
    }

    err << "Preset for channel \"" << new_channel->name << "\" doesn't exist!";
    throw LibDLS::EJobPreset(err.str());
}

/*****************************************************************************/

/**
   Entfernt eine Kanalvorgabe

   \param channel_name Kanalname des Kanals, dessen Vorgabe
   entfernt werden soll
   \throw EJobPreset Es existiert keine Vorgabe für
   den angegebenen Kanal
*/

void CtlJobPreset::remove_channel(const string &channel_name)
{
    vector<LibDLS::ChannelPreset>::iterator channel_i;
    stringstream err;

    channel_i = _channels.begin();
    while (channel_i != _channels.end())
    {
        if (channel_i->name == channel_name)
        {
            _channels.erase(channel_i);
            return;
        }

        channel_i++;
    }

    err << "Preset for channel \"" << channel_name << "\" doesn't exist!";
    throw LibDLS::EJobPreset(err.str());
}

/*****************************************************************************/
