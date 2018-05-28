/*****************************************************************************
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
 *****************************************************************************/

#ifndef COMMON_YAML_H
#define COMMON_YAML_H

#include <yaml.h>

#include <string>

class Yaml
{
    public:
        Yaml();
        Yaml(const Yaml &);
        ~Yaml();

        void load(const std::string &);
        void dump() const;

        Yaml operator[](const std::string &) const;
        Yaml operator[](const char *) const;
        Yaml operator[](char *) const;
        Yaml operator[](size_t) const;

		bool isEmpty() const { return node == NULL; }

        operator bool() const;
        operator int() const;
        operator unsigned int() const;
        operator std::string() const;

        class Exception
        {
            public:
                Exception(const std::string &m):
					msg(m) {}
                std::string msg;
        };

        class NotFoundException {};

    protected:
        Yaml(yaml_document_t *, yaml_node_t *);

    private:
        yaml_document_t *document;
        bool master;
        yaml_node_t *node;

        static void dump_recursive(
                yaml_document_t *,
                yaml_node_t *,
                unsigned int
                );
};

#endif // YAML_H
