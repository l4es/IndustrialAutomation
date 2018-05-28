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
using namespace std;

#include <libxml/parser.h>

#include "MessageList.h"
#include "Message.h"
#include "Job.h"

/****************************************************************************/

/** Constructor.
 */
MessageList::MessageList(
        Job *job
        ):
    _parent_job(job)
{
}

/****************************************************************************/

/** Destructor.
 */
MessageList::~MessageList()
{
}

/****************************************************************************/

/** Construct a new message.
 */
LibDLS::BaseMessage *MessageList::newMessage(xmlNode *node)
{
    Message *message = new Message(this, node);
    LibDLS::BaseMessage *bmsg = static_cast<LibDLS::BaseMessage *>(message);
    return bmsg;
}

/****************************************************************************/

/** Subscribe variables.
 */
void MessageList::subscribe(PdCom::Process *process)
{
    for (map<string, LibDLS::BaseMessage *>::iterator i = _messages.begin();
            i != _messages.end(); i++) {
        Message *message = static_cast<Message *>(i->second);
        message->subscribe(process);
    }
}

/****************************************************************************/

/** Unsubscribe variables.
 */
void MessageList::unsubscribe()
{
    for (map<string, LibDLS::BaseMessage *>::iterator i = _messages.begin();
            i != _messages.end(); i++) {
        Message *message = static_cast<Message *>(i->second);
        message->unsubscribe();
    }
}

/****************************************************************************/

/** Store a message.
 */
void MessageList::store_message(LibDLS::Time time, const std::string &type,
        const std::string &msg)
{
    _parent_job->message(time, type, msg);
}

/****************************************************************************/
