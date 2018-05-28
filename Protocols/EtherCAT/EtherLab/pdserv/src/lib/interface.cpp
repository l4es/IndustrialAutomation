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

#include "Main.h"
#include "Task.h"
#include "Signal.h"
#include "Event.h"
#include "Parameter.h"
#include "../DataType.h"
#include "pdserv.h"

typedef std::vector<PdServ::DataType*> CompoundVector;
CompoundVector compoundType;

static const PdServ::DataType& getDataType(int dt);

/////////////////////////////////////////////////////////////////////////////
struct pdserv* pdserv_create( const char *name, const char *version,
        int (*gettime)(struct timespec*))
{
    return reinterpret_cast<struct pdserv*>(
            new Main(name, version, gettime));
}

/////////////////////////////////////////////////////////////////////////////
void pdserv_config_file( struct pdserv* pdserv, const char *name)
{
    reinterpret_cast<Main*>(pdserv)->setConfigFile(name);
}

/////////////////////////////////////////////////////////////////////////////
struct pdtask* pdserv_create_task(struct pdserv* pdserv, double tsample,
        const char *name)
{
    return reinterpret_cast<struct pdtask*>(
            reinterpret_cast<Main*>(pdserv)->addTask(tsample, name));
}

/////////////////////////////////////////////////////////////////////////////
int pdserv_create_compound( const char *name, size_t size)
{
    int dt = pd_datatype_end + compoundType.size();
    compoundType.push_back(new PdServ::DataType(name, size));
    return dt;
}

/////////////////////////////////////////////////////////////////////////////
void pdserv_compound_add_field(int compound, const char *name,
        int data_type, size_t offset, size_t ndims, const size_t *dim)
{
    compoundType[compound - pd_datatype_end]
        ->addField(name, getDataType(data_type), offset, ndims, dim);
}

/////////////////////////////////////////////////////////////////////////////
void pdserv_exit(struct pdserv* pdserv)
{
    for (CompoundVector::const_iterator it = compoundType.begin();
            it != compoundType.end(); ++it)
        delete *it;
    delete reinterpret_cast<Main*>(pdserv);
}

/////////////////////////////////////////////////////////////////////////////
void pdserv_update_statistics(struct pdtask* task,
        double exec_time, double cycle_time, unsigned int overrun)
{
    reinterpret_cast<Task*>(task)->updateStatistics(
            exec_time, cycle_time, overrun);
}

/////////////////////////////////////////////////////////////////////////////
void pdserv_update(struct pdtask* task, const struct timespec *t)
{
    reinterpret_cast<Task*>(task)->rt_update(t);
}

/////////////////////////////////////////////////////////////////////////////
static const PdServ::DataType& getDataType(int dt)
{
    switch (dt) {
        case pd_boolean_T: return PdServ::DataType::boolean;
        case pd_uint8_T:   return PdServ::DataType::uint8;
        case pd_uint16_T:  return PdServ::DataType::uint16;
        case pd_uint32_T:  return PdServ::DataType::uint32;
        case pd_uint64_T:  return PdServ::DataType::uint64;
        case pd_sint8_T:   return PdServ::DataType::int8;
        case pd_sint16_T:  return PdServ::DataType::int16;
        case pd_sint32_T:  return PdServ::DataType::int32;
        case pd_sint64_T:  return PdServ::DataType::int64;
        case pd_double_T:  return PdServ::DataType::float64;
        case pd_single_T:  return PdServ::DataType::float32;
        default:
                           return *compoundType[dt - pd_datatype_end];
    }
}

/////////////////////////////////////////////////////////////////////////////
struct pdevent *pdserv_event (struct pdserv* pdserv, const char *path,
        int prio, size_t n)
{
    Main *main = reinterpret_cast<Main*>(pdserv);

    return reinterpret_cast<struct pdevent *>(
            main->addEvent(path, prio, n));
}

/////////////////////////////////////////////////////////////////////////////
void pdserv_event_set(struct pdevent *event,
        size_t element, char state, const timespec *t)
{
    reinterpret_cast<Event*>(event)->set(element, bool(state), t);
}

/////////////////////////////////////////////////////////////////////////////
struct pdvariable *pdserv_signal(
        struct pdtask* pdtask,
        unsigned int decimation,
        const char *path,
        int datatype,
        const void *addr,
        size_t n,
        const size_t *dim,
        read_signal_t read_cb,
        void* priv_data
        )
{
    Task *task = reinterpret_cast<Task*>(pdtask);

    Signal *s = task->addSignal(
            decimation, path, getDataType(datatype), addr, n, dim);
    if (read_cb)
        s->read_cb = read_cb;
    s->priv_data = priv_data;

    return reinterpret_cast<struct pdvariable *>(
            static_cast<PdServ::Variable*>(s));
}

/////////////////////////////////////////////////////////////////////////////
struct pdvariable *pdserv_parameter(
        struct pdserv* pdserv,
        const char *path,
        unsigned int mode,
        int datatype,
        void *addr,
        size_t n,
        const size_t *dim,
        write_parameter_t trigger = 0,
        void *priv_data = 0
        )
{
    Main *main = reinterpret_cast<Main*>(pdserv);

    Parameter *p = main->addParameter(
            path, mode, getDataType(datatype), addr, n, dim);
    if (trigger)
        p->write_cb = trigger;
    p->priv_data = priv_data;

    return reinterpret_cast<struct pdvariable *>
        (static_cast<PdServ::Variable*>(p));
}

/////////////////////////////////////////////////////////////////////////////
void pdserv_set_alias(struct pdvariable *var, const char *alias)
{
    reinterpret_cast<PdServ::Variable*>(var)->alias = alias;
}

/////////////////////////////////////////////////////////////////////////////
void pdserv_set_unit(struct pdvariable *var, const char *unit)
{
    reinterpret_cast<PdServ::Variable*>(var)->unit = unit;
}

/////////////////////////////////////////////////////////////////////////////
void pdserv_set_comment(struct pdvariable *var, const char *comment)
{
    reinterpret_cast<PdServ::Variable*>(var)->comment = comment;
}

/////////////////////////////////////////////////////////////////////////////
int pdserv_prepare(struct pdserv* pdserv)
{
    return reinterpret_cast<Main*>(pdserv)->setup();
}
