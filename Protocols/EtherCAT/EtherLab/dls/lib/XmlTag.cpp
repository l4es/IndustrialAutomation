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

#include "XmlTag.h"

using namespace LibDLS;

/*****************************************************************************/

/**
   Konstruktor
*/

XmlTag::XmlTag()
{
    clear();
}

/*****************************************************************************/

/**
   Destruktor
*/

XmlTag::~XmlTag()
{
}

/*****************************************************************************/

/**
   Leert das XMl-Tag
*/

void XmlTag::clear()
{
    _title.clear();
    _atts.clear();
    _type = dxttSingle;
}

/*****************************************************************************/

/**
   Setzt den Titel
*/

void XmlTag::title(const string &title)
{
    _title = title;
}

/*****************************************************************************/

/**
   Setzt den Typ (Start-Tag/Einzeltag/End-Tag)

   \param type Neuer Typ
*/

void XmlTag::type(XmlTagType type)
{
    _type = type;
}

/*****************************************************************************/

/**
   Liefert einen konstanten Zeiger auf ein bestimmtes Attribut

   \param name Name des gewünschten Attrubutes
   \return Konstanter Zeiger auf das Attribut
   \throw EXmlTag Ein Attribut mit diesem Namen existiert nicht
*/

const XmlAtt *XmlTag::att(const string &name) const
{
    list<XmlAtt>::const_iterator iter = _atts.begin();

    // Liste der Attribute durchsuchen
    while (iter != _atts.end())
    {
        if (iter->name() == name) return &(*iter);
        iter++;
    }

    throw EXmlTag("Attribute \"" + name + "\" does not exist!", tag());
}

/*****************************************************************************/

/**
   Prüft, ob das Tag ein Attribute mit einem bestimmten Namen hat

   \param name Name des gesuchten Attributes
   \return true, wenn das tag ein solches Attribut besitzt
*/

bool XmlTag::has_att(const string &name) const
{
    list<XmlAtt>::const_iterator iter = _atts.begin();

    // Liste der Attribute durchsuchen
    while (iter != _atts.end())
    {
        if (iter->name() == name) return true;
        iter++;
    }

    return false;
}

/*****************************************************************************/

/**
   Fügt dem Tag ein Attribut mit Namen und Wert (String) hinzu

   \param name name des Attributes
   \param value Wert des Attributes
*/

void XmlTag::push_att(const string &name, const string &value)
{
    _atts.push_back(XmlAtt(name, value));
}

/*****************************************************************************/

/**
   Fügt dem Tag ein Attribut mit Namen und Wert hinzu (int)

   Dies ist die (int)-Version von
   push_att(const string &, const string &).

   \param name name des Attributes
   \param value Wert des Attributes
*/

void XmlTag::push_att(const string &name, int value)
{
    stringstream str;
    str << value;
    _atts.push_back(XmlAtt(name, str.str()));
}

/*****************************************************************************/

/**
   Fügt dem Tag ein Attribut mit Namen und Wert hinzu (unsigned int)

   Dies ist die (unsigned int)-Version von
   push_att(const string &, const string &).

   \param name name des Attributes
   \param value Wert des Attributes
*/

void XmlTag::push_att(const string &name, unsigned int value)
{
    stringstream str;
    str << value;
    _atts.push_back(XmlAtt(name, str.str()));
}

/*****************************************************************************/

/**
   Fügt dem Tag ein Attribut mit Namen und Wert hinzu (double)

   Dies ist die (double)-Version von
   push_att(const string &, const string &).

   \param name name des Attributes
   \param value Wert des Attributes
*/

void XmlTag::push_att(const string &name, double value)
{
    stringstream str;
    str << fixed << value;
    _atts.push_back(XmlAtt(name, str.str()));
}

/*****************************************************************************/

/**
   Fügt dem Tag ein Attribut mit Namen und Wert hinzu (uint64_t)

   Dies ist die (uint64_t)-Version von
   push_att(const string &, const string &).

   \param name name des Attributes
   \param value Wert des Attributes
*/

void XmlTag::push_att(const string &name, uint64_t value)
{
    stringstream str;
    str << value;
    _atts.push_back(XmlAtt(name, str.str()));
}

/*****************************************************************************/

/**
   Zählt die Attribute des Tags

   \return Anzahl der Attribute
*/

int XmlTag::att_count() const
{
    return _atts.size();
}

/*****************************************************************************/

/**
   Gibt das gesamte Tag als String zurück

   Erst diese Methode baut das eigentliche Tag zusammen.

   \return XML-Tag als String
*/

string XmlTag::tag() const
{
    string str;
    list<XmlAtt>::const_iterator iter;

    str = "<";

    if (_type == dxttEnd) str += "/";

    str += _title;

    iter = _atts.begin();
    while (iter != _atts.end())
    {
        str += " " + iter->name() + "=\"" + iter->to_str() + "\"";
        iter++;
    }

    if (_type == dxttSingle) str += "/";

    str += ">";

    return str;
}

/*****************************************************************************/
//
//  XmlAtt-Methoden
//
/*****************************************************************************/

/**
   Konstruktor
*/

XmlAtt::XmlAtt(const string &name, const string &value)
{
    _name = name;
    _value = value;
}

/*****************************************************************************/

/**
   Gibt den Wert des Attributes als String zurück

   \return Attributwert
*/

const string & XmlAtt::to_str() const
{
    return _value;
}

/*****************************************************************************/

/**
   Gibt den Wert des Attributes als (int) zurück

   \return Attributwert
*/

int XmlAtt::to_int() const
{
    int i;
    stringstream str;
    str.exceptions(stringstream::failbit | stringstream::badbit);
    str << _value;

    try
    {
        str >> i;
    }
    catch (...)
    {
        throw EXmlTag("\"" + _value + "\" is no integer!", "");
    }

    return i;
}

/*****************************************************************************/

/**
   Gibt den Wert des Attributes als (double) zurück

   \return Attributwert
*/

double XmlAtt::to_dbl() const
{
    double d;
    stringstream str;
    str.exceptions(stringstream::failbit | stringstream::badbit);
    str << _value;

    try
    {
        str >> d;
    }
    catch (...)
    {
        throw EXmlTag("\"" + _value + "\" is no double!", "");
    }

    return d;
}

/*****************************************************************************/

/**
   Gibt den Wert des Attributes als (uint64_t) zurück

   \return Attributwert
*/

uint64_t XmlAtt::to_uint64() const
{
    uint64_t value;
    stringstream str;
    str.exceptions(stringstream::failbit | stringstream::badbit);
    str << _value;

    try
    {
        str >> value;
    }
    catch (...)
    {
        throw EXmlTag("\"" + _value + "\" is no uint64_t!", "");
    }

    return value;
}

/*****************************************************************************/
