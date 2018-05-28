/*****************************************************************************
 *
 *  $Id$
 *
 *  Copyright 2010 Richard Hacker (lerichi at gmx dot net)
 *
 *  This file is part of the pdserv library.
 *
 *  The pdserv library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or (at
 *  your option) any later version.
 *
 *  The pdserv library is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 *  License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with the pdserv library. If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/

#include <cstring>
#include <cstdio>
#include <cerrno>

#include "Config.h"

#include <yaml.h>
using namespace PdServ;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
static char error[100];

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Config::Config(): node(0)
{
}

/////////////////////////////////////////////////////////////////////////////
Config::Config(const Config& other):
    document(other.document), node(other.node)
{
}

/////////////////////////////////////////////////////////////////////////////
Config::Config(yaml_document_t *d, yaml_node_t *n):
    document(d), node(n)
{
}

/////////////////////////////////////////////////////////////////////////////
Config::~Config()
{
    clear();
}

/////////////////////////////////////////////////////////////////////////////
void Config::clear()
{
    if (!file.empty()) {
        yaml_document_delete(document);
        delete document;

        file.clear();
    }
}

/////////////////////////////////////////////////////////////////////////////
const char * Config::reload()
{
    std::string file(this->file);
    return load(file.c_str());
}

/////////////////////////////////////////////////////////////////////////////
const char * Config::load(const char *filename)
{
    yaml_parser_t parser;
    FILE *fh;

    /* Initialize parser */
    if(!yaml_parser_initialize(&parser)) {
        ::snprintf(error, sizeof(error), "Could not initialize YAML parser");
        return error;
    }

    fh = ::fopen(filename, "r");
    if (!fh) {
        ::snprintf(error, sizeof(error), "Could not open config file %s: %s",
                filename, strerror(errno));
        return error;
    }

    /* Set input file */
    yaml_parser_set_input_file(&parser, fh);

    clear();
    document = new yaml_document_t;
    this->file = filename;

    /* START new code */
    if (!yaml_parser_load(&parser, document)) {
        snprintf(error, sizeof(error), "YAML parser failure at line %zu: %s",
            parser.problem_mark.line, parser.problem);
        clear();
        return error;
    }

    // Now finished with the file
    ::fclose(fh);

    node = yaml_document_get_root_node(document);

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
Config& Config::operator=(const Config& other)
{
    document = other.document;
    node     = other.node;

    return *this;
}

/////////////////////////////////////////////////////////////////////////////
Config Config::operator[](const char *key) const
{
    return this->operator[](std::string(key));
}

/////////////////////////////////////////////////////////////////////////////
Config::operator bool() const
{
    return node != 0;
}

/////////////////////////////////////////////////////////////////////////////
Config Config::operator[](const std::string& key) const
{
    if (!node or node->type != YAML_MAPPING_NODE)
        return Config();

    for (yaml_node_pair_t *pair = node->data.mapping.pairs.start;
            pair != node->data.mapping.pairs.top; ++pair) {
        yaml_node_t *n = yaml_document_get_node(document, pair->key);
        if (n->type == YAML_SCALAR_NODE
                and key.size() == n->data.scalar.length
                and !strncmp((char*)n->data.scalar.value,
                    key.c_str(), n->data.scalar.length)) {
            return Config(document,
                    yaml_document_get_node(document, pair->value));
        }
    }

    return Config();
}

/////////////////////////////////////////////////////////////////////////////
Config Config::operator[](size_t index) const
{
    if (!node)
        return Config();

    switch (node->type) {
        case YAML_SEQUENCE_NODE:
            {
                yaml_node_item_t *n = node->data.sequence.items.start;
                do {
                    if (!index--)
                        return Config(document,
                                yaml_document_get_node(document, *n));
                } while (++n != node->data.sequence.items.top);
            }
            break;

        case YAML_MAPPING_NODE:
            {
                yaml_node_pair_t *pair = node->data.mapping.pairs.start;
                do {
                    if (!index--)
                        return Config(document,
                                yaml_document_get_node(document,
                                    pair->key));
                } while (++pair != node->data.mapping.pairs.top);
            }
            break;

        case YAML_SCALAR_NODE:
            if (index == 0)
                return *this;
            break;

        default:
            break;
    }

    return Config();
}

/////////////////////////////////////////////////////////////////////////////
bool Config::isMapping() const
{
    return node and node->type == YAML_MAPPING_NODE;
}

/////////////////////////////////////////////////////////////////////////////
std::string Config::toString(const std::string& defaultString) const
{
    if (!node or node->type != YAML_SCALAR_NODE)
        return defaultString;

    return std::string((char*)node->data.scalar.value, node->data.scalar.length);
}

/////////////////////////////////////////////////////////////////////////////
namespace PdServ {
    template <typename T>
        bool Config::get(T &value) const
        {
            if (!*this)
                return false;

            value = *this;
            return true;
        }

    template bool Config::get(int& value) const;
}

/////////////////////////////////////////////////////////////////////////////
int Config::toInt(int dflt) const
{
    std::string sval(toString());
    if (sval.empty())
        return dflt;

    return strtol(sval.c_str(), 0, 0);
}

/////////////////////////////////////////////////////////////////////////////
unsigned int Config::toUInt(unsigned int dflt) const
{
    std::string sval(toString());
    if (sval.empty())
        return dflt;

    return strtoul(sval.c_str(), 0, 0);
}
