/*****************************************************************************
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
 ****************************************************************************/

#include <sstream>
#include <iostream>
using namespace std;

#include "BaseMessage.h"
#include "BaseMessageList.h"
using namespace LibDLS;

/****************************************************************************/

/** Constructor.
 */
BaseMessage::BaseMessage(
        xmlNode *node
        ):
    _type(Information)
{
    char *data;
    string str;

    data = (char *) xmlGetProp(node, (const xmlChar *) "type");
    if (!data) {
        throw Exception("Missing type attribute!");
    }
    str = data;
    xmlFree(data);

    _type = _typeFromString(str);

    data = (char *) xmlGetProp(node, (const xmlChar *) "variable");
    if (!data) {
        throw Exception("Missing variable attribute!");
    }
    _path = data;
    xmlFree(data);

    for (xmlNode *curNode = node->children;
            curNode; curNode = curNode->next) {
        if (curNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        if (string((const char *) curNode->name) == "Text") {
            loadTranslations(curNode, _text);
        }
    }

#if 0
    string a = "  a  b\tc  ";
    cerr << a << "1--" << _simplified(a) << "##" << endl;
    a = "a  b\tc";
    cerr << a << "2--" << _simplified(a) << "##" << endl;
#endif
}

/****************************************************************************/

/** Destructor.
 */
BaseMessage::~BaseMessage()
{
}

/****************************************************************************/

std::string BaseMessage::text(const std::string &lang) const
{
    TranslationMap::const_iterator it;

    if (lang == "") { // not specified. Try "en", otherwise first.
        it = _text.find("en");
        if (it != _text.end()) {
            return it->second;
        }
        if (!_text.empty()) {
            return _text.begin()->second;
        }
    }
    else { // lang specified
        it = _text.find(lang);
        if (it != _text.end()) {
            return it->second;
        }
    }

    return string();
}

/****************************************************************************/

/** Converts a message type string to the appropriate #Type.
 */
BaseMessage::Type BaseMessage::_typeFromString(const std::string &str)
{
    if (str == "Information") {
        return Information;
    }
    if (str == "Warning") {
        return Warning;
    }
    if (str == "Error") {
        return Error;
    }
    if (str == "Critical") {
        return Critical;
    }

    stringstream err;
    err << "Invalid message type " << str;
    throw Exception(err.str());
}

/****************************************************************************/

/** Simplify XML content.
 *
 * Replaces all sequences of '\t', '\n', '\v', '\f', '\r', and ' ' with a
 * single space.
 */
std::string BaseMessage::_simplified(
        const std::string &input
        )
{
    unsigned int i = 0;
    string output;
    bool space_inserted = false;

    // skip whitespace at beginning
    while (i < input.size() && isspace(input[i])) {
        i++;
    }

    for (; i < input.size(); i++) {
        if (isspace(input[i])) {
            if (!space_inserted) {
                output += ' ';
                space_inserted = true;
            }
        }
        else {
            output += input[i];
            space_inserted = false;
        }
    }

    // remove whitespace at end
    if (output.size() > 0 && isspace(output[output.size() - 1])) {
        output = output.substr(0, output.size() - 1);
    }

#if 0
    cerr << "--" << output << "--" << endl;
#endif

    return output;
}

/****************************************************************************/

/** Processes a TextNode XML element.
 */
void BaseMessage::loadTranslations(
        xmlNode *node, /**< XML node. */
        TranslationMap &map /**< Translation map. */
        )
{
    for (xmlNode *curNode = node->children;
            curNode; curNode = curNode->next) {
        if (curNode->type != XML_ELEMENT_NODE
                || string((const char *) curNode->name) != "Translation") {
            continue;
        }

        char *lang = (char *) xmlGetProp(curNode, (const xmlChar *) "lang");
        if (!lang) {
            throw Exception("Translation missing lang attribute!");
        }
        char *content = (char *) xmlNodeGetContent(curNode);
        if (content) {
            map[lang] = _simplified(content);
            xmlFree(content);
        }
        xmlFree(lang);
    }
}

/****************************************************************************/
