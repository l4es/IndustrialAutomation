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

#include <pdcom/Variable.h>

#include "Message.h"
#include "MessageList.h"
#include "globals.h"

/****************************************************************************/

/** Constructor.
 */
Message::Message(
        MessageList *list,
        xmlNode *node
        ):
    BaseMessage(node),
    _parent_list(list),
    _var(NULL),
    _value(0.0),
    _data_present(false)
{
}

/****************************************************************************/

/** Destructor.
 */
Message::~Message()
{
    unsubscribe();
}

/****************************************************************************/

/** Subscribe variable.
 */
void Message::subscribe(PdCom::Process *process)
{
    unsubscribe();

#if 0
    msg() << __func__ << "() " << path();
    log(::Info);
#endif

    _var = process->findVariable(path());

    if (!_var) {
        msg() << "Message variable " << path() << " not found!";
        log(::Error);
        return;
    }

    _data_present = false;

    try {
        _var->subscribe(this, 0.0);
    }
    catch (PdCom::Exception &e) {
        msg() << "Message variable subscription failed!";
        log(::Error);
        _var = NULL;
        return;
    }

    try {
        _var->poll(this);
    }
    catch (PdCom::Exception &e) {
        msg() << "Message poll failed!";
        log(::Error);
        unsubscribe();
    }
}

/****************************************************************************/

/** Unsubscribe from variable.
 */
void Message::unsubscribe()
{
    if (_var) {
#if 0
        msg() << __func__ << "(): " << path();
        log(::Info);
#endif
        _var->unsubscribe(this);
        _var = NULL;
    }
}

/****************************************************************************/

void Message::notify(PdCom::Variable *var)
{
    if (var != _var) {
        return;
    }

    double new_value;
    var->getValue(&new_value);

    if (_data_present && _value == 0.0 && new_value != 0.0) {
        double t = var->getMTime();
        LibDLS::Time time;
        time.from_dbl_time(t);
        string storeType;

        switch (type()) {
            case Information:
                storeType = "info";
                break;
            case Warning:
                storeType = "warn";
                break;
            case Error:
                storeType = "error";
                break;
            case Critical:
                storeType = "error";
                break;
            default:
                storeType = "info";
                break;
        }

        _parent_list->store_message(time, storeType, path());
    }

    _value = new_value;
    _data_present = true;
}

/****************************************************************************/

void Message::notifyDelete(PdCom::Variable *var)
{
    if (var == _var) {
        _var = NULL;
        _data_present = false;
    }
}

/****************************************************************************/
