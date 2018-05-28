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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <sstream>
using namespace std;

#include "BaseMessageList.h"
#include "BaseMessage.h"

using namespace LibDLS;

/****************************************************************************/

/** Constructor.
 */
BaseMessageList::BaseMessageList()
{
}

/****************************************************************************/

/** Destructor.
 */
BaseMessageList::~BaseMessageList()
{
    clear();
}

/****************************************************************************/

/** Clear the messages.
 */
void BaseMessageList::clear()
{
    for (map<string, BaseMessage *>::iterator i = _messages.begin();
            i != _messages.end(); i++) {
        delete i->second;
    }

    _messages.clear();
}

/****************************************************************************/

/** Return message file path.
 */
std::string BaseMessageList::path(const string &job_path)
{
    return job_path + "/plainmessages.xml";
}

/****************************************************************************/

/** Check, if message file exists.
 */
bool BaseMessageList::exists(const string &job_path)
{
    struct stat buf;
    int ret = stat(path(job_path).c_str(), &buf);
    if (ret == 0) {
        return true;
    }
    else if (errno == ENOENT) {
        return false;
    }
    else {
        stringstream err;
        err << "stat() failed: " << strerror(errno);
        throw Exception(err.str());
    }
}

/****************************************************************************/

/** Import XML file.
 */
void BaseMessageList::import(const string &job_path)
{
    clear();

    xmlDocPtr doc = xmlParseFile(path(job_path).c_str());

    if (!doc) {
        stringstream err;

        err << "Failed to import messages";
        xmlErrorPtr e = xmlGetLastError();
        if (e) {
            err << ": " << e->message;
        }
        else {
            err << ".";
        }

        throw Exception(err.str());
    }

    xmlNode *rootNode = xmlDocGetRootElement(doc);

    for (xmlNode *curNode = rootNode->children;
            curNode; curNode = curNode->next) {
        if (curNode->type == XML_ELEMENT_NODE
                && string((const char *) curNode->name) == "Message") {
            BaseMessage *message;

            try {
                message = newMessage(curNode);
            } catch (BaseMessage::Exception &e) {
                clear();
                xmlFreeDoc(doc);
                throw Exception("Failed to import message: " + e.msg);
            }

            pair<map<string, BaseMessage *>::iterator, bool> ret;
            ret = _messages.insert(
                    pair<string, BaseMessage *>(message->path(), message));
            if (ret.second == false) {
                // already existing
                stringstream err;
                err << "Duplicate message path: " << message->path();
                delete message;
                clear();
                xmlFreeDoc(doc);
                throw Exception(err.str());
            }
        }
    }

    xmlFreeDoc(doc);
}

/****************************************************************************/

/** Count the messages.
 */
unsigned int BaseMessageList::count() const
{
    unsigned int c = 0;

    for (map<string, BaseMessage *>::const_iterator i = _messages.begin();
            i != _messages.end(); i++) {
        c++;
    }

    return c;
}

/****************************************************************************/

/** Find a message matching a path.
 */
const BaseMessage *BaseMessageList::findPath(const std::string &p) const
{
    map<string, BaseMessage *>::const_iterator it;
    it = _messages.find(p);

    if (it != _messages.end()) {
        return it->second;
    }
    else {
        return NULL;
    }
}

/****************************************************************************/

/** Construct a new message.
 */
BaseMessage *BaseMessageList::newMessage(xmlNode *node)
{
    return new BaseMessage(node);
}

/****************************************************************************/
