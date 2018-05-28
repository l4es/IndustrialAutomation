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

#include <sstream>
using namespace std;

#include "LibDLS/globals.h"
#include "LibDLS/ChannelPreset.h"

#include "XmlTag.h"
using namespace LibDLS;

/*****************************************************************************/

/**
   Konstruktor
*/

ChannelPreset::ChannelPreset()
{
    clear();
}

/*****************************************************************************/

/**
   Destruktor
*/

ChannelPreset::~ChannelPreset()
{
}

/*****************************************************************************/

/**
   Prüft zwei Kanalvorgaben auf Ungleichheit

   Wird benötigt, um bei Änderung der Auftragsvorgaben zu
   bestimmen, ob sich eine Kanalvorgabe geändert hat.

   \param other The "other" channel preset to compare with.
   \return true, if something has changed
   \see DLSJob::sync_loggers()
*/

bool ChannelPreset::operator!=(const ChannelPreset &other) const
{
    return name != other.name ||
        sample_frequency != other.sample_frequency ||
        block_size != other.block_size ||
        meta_mask != other.meta_mask ||
        meta_reduction != other.meta_reduction ||
        format_index != other.format_index ||
        mdct_block_size != other.mdct_block_size ||
        accuracy != other.accuracy;
}

/*****************************************************************************/

/**
   Importiert eine Kanalvorgabe aus einem XML-Tag

   Importiert aus einem <channel>-Tag aus der
   Auftragsvorgabendatei.

   \param tag Konstanter Zeiger auf ein XML-Tag,
   aus dem gelesen werden soll
   \throw EChannelPreset Fehler während des Importierens
*/

void ChannelPreset::read_from_tag(const XmlTag *tag)
{
    string format_string;
    stringstream err;

    clear();

    try
    {
        name = tag->att("name")->to_str();
        sample_frequency = tag->att("frequency")->to_dbl();
        block_size = tag->att("block_size")->to_int();
        meta_mask = tag->att("meta_mask")->to_int();
        meta_reduction = tag->att("meta_reduction")->to_int();
        format_string = tag->att("format")->to_str();

        for (int i = 0; i < FORMAT_COUNT; i++)
        {
            if (format_string == format_strings[i])
            {
                format_index = i;
                break;
            }
        }

        if (format_index == FORMAT_INVALID)
        {
            clear();
            err << "Unknown channel format \"" << format_string << "\"!";
            throw EChannelPreset(err.str());
        }

        if (format_index == FORMAT_MDCT)
        {
            mdct_block_size = tag->att("mdct_block_size")->to_int();
            accuracy = tag->att("mdct_accuracy")->to_dbl();
        }

        if (format_index == FORMAT_QUANT)
        {
            accuracy = tag->att("accuracy")->to_dbl();
        }

        if (tag->has_att("type"))
        {
            type = str_to_channel_type(tag->att("type")->to_str());
        }
        else
        {
            type = TUNKNOWN;
        }
    }
    catch (EXmlTag &e)
    {
        clear();
        throw EChannelPreset(e.msg);
    }
}

/*****************************************************************************/

/**
   Exportiert eine Kanalvorgabe in ein XML-Tag

   Erstellt ein von read_from_tag() lesbares XML-Tag
   für die Auftragsvorgabendatei.

   \param tag Zeiger auf ein XML-Tag, in das
   geschrieben werden soll
   \throw EChannelPreset Ungültiges Format
*/

void ChannelPreset::write_to_tag(XmlTag *tag) const
{
    if (format_index < 0 || format_index >= FORMAT_COUNT)
    {
        throw EChannelPreset("Invalid channel format!");
    }

    tag->clear();
    tag->title("channel");
    tag->push_att("name", name);
    tag->push_att("frequency", sample_frequency);
    tag->push_att("block_size", block_size);
    tag->push_att("meta_mask", meta_mask);
    tag->push_att("meta_reduction", meta_reduction);
    tag->push_att("format", format_strings[format_index]);

    if (format_index == FORMAT_MDCT)
    {
        tag->push_att("mdct_block_size", mdct_block_size);
        tag->push_att("mdct_accuracy", accuracy);
    }

    if (format_index == FORMAT_QUANT)
    {
        tag->push_att("accuracy", accuracy);
    }

    if (type != TUNKNOWN)
    {
        tag->push_att("type", channel_type_to_str(type));
    }
}

/*****************************************************************************/

/**
   Löscht die aktuellen Kanalvorgaben
*/

void ChannelPreset::clear()
{
    name = "";
    sample_frequency = 0.0;
    block_size = 0;
    meta_mask = 0;
    meta_reduction = 0;
    format_index = FORMAT_INVALID;
    mdct_block_size = 0;
    accuracy = 0.0;
    type = TUNKNOWN;
}

/*****************************************************************************/
