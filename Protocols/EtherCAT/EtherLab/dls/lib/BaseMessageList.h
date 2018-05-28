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

#ifndef LibDLSBaseMessageListH
#define LibDLSBaseMessageListH

/*****************************************************************************/

#include <libxml/parser.h>

#include <string>
#include <map>

#include "lib/LibDLS/Exception.h"

/*****************************************************************************/

namespace LibDLS {

class BaseMessage;

/*****************************************************************************/

/** Message list.
 */
class BaseMessageList
{
public:
    BaseMessageList();
    virtual ~BaseMessageList();

    void clear();

    static std::string path(const std::string &);
    static bool exists(const std::string &);
    void import(const std::string &);
    unsigned int count() const;
    const BaseMessage *findPath(const std::string &) const;

    /** Exception.
     */
    class Exception:
        public LibDLS::Exception
    {
        public:
            Exception(string pmsg):
                LibDLS::Exception(pmsg) {};
    };

protected:
    std::map<std::string, BaseMessage *> _messages; /**< Messages. */

    virtual BaseMessage *newMessage(xmlNode *);
};

} // namespace LibDLS

/*****************************************************************************/

#endif
