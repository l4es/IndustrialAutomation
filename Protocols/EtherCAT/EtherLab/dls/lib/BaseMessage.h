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

#ifndef LibDLSBaseMessageH
#define LibDLSBaseMessageH

/*****************************************************************************/

#include <map>

#include <libxml/parser.h>

#include "LibDLS/Exception.h"

namespace LibDLS {

class MessageList;

/*****************************************************************************/

/** Message base class.
 */
class BaseMessage
{
public:
    BaseMessage(xmlNode *);
    virtual ~BaseMessage();

    /** Message type.
     */
    enum Type {
        Information, /**< Non-critical information. */
        Warning, /**< Warning, that does not influence
                   the process flow. */
        Error, /**< Error, that influences the process flow. */
        Critical /**< Critical error, that makes the process
                   unable to run. */
    };

    Type type() const { return _type; }
    const std::string &path() const { return _path; }
    std::string text(const std::string &) const;

    /** Exception.
     */
    class Exception:
        public LibDLS::Exception
    {
        public:
            Exception(string pmsg):
                LibDLS::Exception(pmsg) {};
    };

private:
    Type _type;
    std::string _path;
    typedef std::map<std::string, std::string> TranslationMap;
    TranslationMap _text; /**< Translated texts in UTF-8 encoding. */

    static Type _typeFromString(const std::string &);
    static std::string _simplified(const std::string &);
    static void loadTranslations(xmlNode *, TranslationMap &);
};

} // namespace LibDLS

/*****************************************************************************/

#endif


