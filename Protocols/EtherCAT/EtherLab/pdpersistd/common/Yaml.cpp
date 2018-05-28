/****************************************************************************
 *
 *  $Id$
 *
 *  Copyright 2010 Richard Hacker (lerichi at gmx dot net)
 *            2012 Florian Pose <fp@igh-essen.com>
 *
 *  This file is part of the process data persistence daemon.
 *
 *  The process data persistence daemon is free software: you can redistribute
 *  it and/or modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  The process data persistence daemon is distributed in the hope that it
 *  will be useful, but WITHOUT ANY WARRANTY; without even the implied
 *  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with the process data persistence daemon. If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

#include <iostream>
#include <cstring>
#include <cstdio>
#include <cerrno>
#include <sstream>
#include <algorithm>
#include <cctype>

using namespace std;

#include "Yaml.h"

#define DEBUG 0

/****************************************************************************/

const char *node_types[] = {
    "unknown",
    "scalar",
    "sequence",
    "mapping"
};

/****************************************************************************/

Yaml::Yaml():
	document(NULL),
    master(false),
	node(NULL)
{
}

/****************************************************************************/

Yaml::Yaml(const Yaml &other):
    document(other.document),
    master(false),
	node(other.node)
{
}

/****************************************************************************/

Yaml::Yaml(yaml_document_t *d, yaml_node_t *n):
	document(d),
    master(false),
	node(n)
{
}

/****************************************************************************/

Yaml::~Yaml()
{
    if (master) {
        yaml_document_delete(document);
        delete document;
    }
}

/****************************************************************************/

void Yaml::load(const std::string &path)
{
    yaml_parser_t parser;
    FILE *fh;

    /* Initialize parser */
    if (!yaml_parser_initialize(&parser)) {
        stringstream err;
        err << "Failed to initialize parser.";
        throw Exception(err.str());
    }

    fh = ::fopen(path.c_str(), "r");
    if (!fh) {
		stringstream err;
        err << "Failed to open file " << path << ": "
			<< strerror(errno);
		yaml_parser_delete(&parser);
        throw Exception(err.str());
    }

    yaml_parser_set_input_file(&parser, fh);

    document = new yaml_document_t;

    if (!yaml_parser_load(&parser, document)) {
		stringstream err;
		err << "YAML parser error at line "
            << parser.problem_mark.line
			<< ": " << parser.problem;
        yaml_document_delete(document);
        delete document;
        document = NULL;
		::fclose(fh);
		yaml_parser_delete(&parser);
        throw Exception(err.str());
    }

    ::fclose(fh);
    yaml_parser_delete(&parser);

    master = true;
    node = yaml_document_get_root_node(document);
}

/****************************************************************************/

void Yaml::dump() const
{
    if (!document or !node) {
        return;
    }

    dump_recursive((yaml_document_t *) document, (yaml_node_t *) node, 0);
}

/****************************************************************************/

Yaml Yaml::operator[](const char *key) const
{
    return this->operator[](std::string(key));
}

/****************************************************************************/

Yaml Yaml::operator[](const std::string& key) const
{
#if DEBUG
    cerr << (void *) node << " index key=" << key << endl;
#endif
    if (!node) {
        throw Exception("Index operator called for empty node!");
    }

    if (node->type != YAML_MAPPING_NODE) {
        stringstream err;
        err << "Index operator with string key \"" << key << "\" "
            << "called for " << node_types[node->type] << " node "
            << (void *) node << "!";
        throw Exception(err.str());
    }

    for (yaml_node_pair_t *pair = node->data.mapping.pairs.start;
            pair != node->data.mapping.pairs.top; ++pair) {
        yaml_node_t *n = yaml_document_get_node(document, pair->key);
        if (n->type != YAML_SCALAR_NODE) {
            throw Exception("Non-scalar key found!");
        }
        if (key.size() == n->data.scalar.length
                and !strncmp((char *) n->data.scalar.value,
                    key.c_str(), n->data.scalar.length)) {
            return Yaml(document,
                    yaml_document_get_node(document, pair->value));
        }
    }

    throw NotFoundException();
}

/****************************************************************************/

Yaml Yaml::operator[](size_t index) const
{
#if DEBUG
    cerr << (void *) node << " index index=" << index << endl;
#endif
    if (!node) {
        throw Exception("Index operator called for empty node!");
    }

    if (node->type != YAML_SEQUENCE_NODE) {
        throw Exception("Numeric index operator called"
                " for non-sequence node!");
    }

    yaml_node_item_t *n = node->data.sequence.items.start;
    for (size_t i = 0; i < index; ++n, ++i) {
        if (n == node->data.sequence.items.top) {
            throw NotFoundException();
        }
    }

    if (n == node->data.sequence.items.top) {
        throw NotFoundException();
    }

    return Yaml(document, yaml_document_get_node(document, *n));
}

/****************************************************************************/

Yaml::operator bool() const
{
#if DEBUG
    cerr << (void *) node << " bool" << endl;
#endif
    string val(*this);

    std::transform(val.begin(), val.end(), val.begin(),
            (int (*)(int)) tolower);

    return val == "yes" or val == "true" or val == "1";
}

/****************************************************************************/

Yaml::operator std::string() const
{
#if DEBUG
    cerr << (void *) node << " string" << endl;
#endif
    if (!node) {
        throw Exception("String conversion called for empty node!");
    }

    if (node->type != YAML_SCALAR_NODE) {
        throw Exception("String-conversion called for non-scalar node!");
    }

    return std::string((char *) node->data.scalar.value,
			node->data.scalar.length);
}

/****************************************************************************/

Yaml::operator int() const
{
#if DEBUG
    cerr << (void *) node << " int" << endl;
#endif
    string val(*this);
    char *end;
    int ret;

    if (val.empty()) {
        stringstream err;
        err << "Failed to parse integer from empty string";
        throw Exception(err.str());
    }

    ret = strtol(val.c_str(), &end, 0);

    if (*end != '\0') {
        stringstream err;
        err << "Failed to parse integer from " << val;
        throw Exception(err.str());
    }

    return ret;
}

/****************************************************************************/

Yaml::operator unsigned int() const
{
#if DEBUG
    cerr << (void *) node << " unsigned int" << endl;
#endif
    string val(*this);
    char *end;
    unsigned int ret;

    if (val.empty()) {
        stringstream err;
        err << "Failed to parse unsigned integer from empty string";
        throw Exception(err.str());
    }

    ret = strtoul(val.c_str(), &end, 0);

    if (*end != '\0') {
        stringstream err;
        err << "Failed to parse unsigned integer from " << val;
        throw Exception(err.str());
    }

    return ret;
}

/****************************************************************************/

void Yaml::dump_recursive(
        yaml_document_t *doc,
        yaml_node_t *node,
        unsigned int level
        )
{
    size_t i = 0;

    if (!node) {
        return;
    }

    cerr << level << " " << (void *) node << "  ";

    switch (node->type) {
        case YAML_SCALAR_NODE:
            cerr << "scalar: "
                << string((char *) node->data.scalar.value,
                    node->data.scalar.length) << endl;
            break;

        case YAML_SEQUENCE_NODE:
            cerr << "seq" << endl;
            for (yaml_node_item_t *n = node->data.sequence.items.start;
                    n != node->data.sequence.items.top; n++, i++) {
                cerr << "  " << i << endl;
                yaml_node_t *c = yaml_document_get_node(doc, *n);
                dump_recursive(doc, c, level + 1);
            }
            break;
        case YAML_MAPPING_NODE:
            cerr << "map" << endl;
            for (yaml_node_pair_t *pair = node->data.mapping.pairs.start;
                    pair != node->data.mapping.pairs.top; ++pair) {
                cerr << "  key:" << endl;
                yaml_node_t *key =
                    yaml_document_get_node(doc, pair->key);
                dump_recursive(doc, key, level + 1);
                cerr << "  value:" << endl;
                yaml_node_t *val =
                    yaml_document_get_node(doc, pair->value);
                dump_recursive(doc, val, level + 1);
            }
            break;
        default:
            cerr << "unknown" << endl;
            break;
    }
}

/****************************************************************************/
