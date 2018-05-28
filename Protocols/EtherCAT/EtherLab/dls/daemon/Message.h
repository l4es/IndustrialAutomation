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

#ifndef MessageH
#define MessageH

/*****************************************************************************/

#include <pdcom/Process.h>
#include <pdcom/Subscriber.h>

#include "lib/BaseMessage.h"

class MessageList;

/*****************************************************************************/

/** Message
 */
class Message:
    public LibDLS::BaseMessage,
    public PdCom::Subscriber
{
public:
    Message(MessageList *, xmlNode *);
    ~Message();

    void subscribe(PdCom::Process *);
    void unsubscribe();

private:
    MessageList * const _parent_list;
    PdCom::Variable *_var;
    double _value;
    bool _data_present;

    virtual void notify(PdCom::Variable *);
    virtual void notifyDelete(PdCom::Variable *);
};

/*****************************************************************************/

#endif
