/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2015-2016  Richard Hacker (lerichi at gmx dot net)
 *
 * This file is part of the PdCom library.
 *
 * The PdCom library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * The PdCom library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the PdCom library. If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/

//#include "headers.hpp"
#include <boost/python.hpp>
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>

#include <list>
#include <map>

#include "Debug.h"

#include "pdcom.h"

namespace bp = boost::python;

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
struct Variable;
struct Subscriber;
struct Process: PdCom::Process
{/*{{{*/
    Process(PyObject* self): self(self)
    {
        log_debug("ctor: Process");
    }

    ~Process() {
        log_debug("dtor: Process");
    }

    PyObject* const self;

    std::map<const PdCom::Variable*, PyObject*> variableMap;
    bp::object get(const PdCom::Variable* v);
    void forget(const PdCom::Variable* v) {
        variableMap[v] = 0;
    }

    bool find(const std::string& path) {
        return PdCom::Process::find(path);
    }

    // Reimplemented from PdCom::Process
    int read(       char *buf, size_t n) {
        std::string data = bp::call_method<std::string>(self, "read", n);
        std::copy(data.begin(), data.end(), buf);
        return data.size();
    }
    void write(const char *buf, size_t n) {
        bp::call_method<int>(self, "write", std::string(buf, n));
    }
    void flush() {
        bp::call_method<int>(self, "flush");
    }
    void connected() {
        bp::call_method<void>(self, "connected");
    }
    void listReply(
            std::list<const PdCom::Variable*>& variables,
            std::list<std::string>& path);
    void findReply(const PdCom::Variable* v) {
        bp::call_method<void>(self, "findReply", get(v));
    }
    void loginReply(const char* serverData, bool finished, bool success) {
        bp::call_method<void>(self, "loginReply",
                std::string(serverData), finished, success);
    }
    void pingReply() {
        bp::call_method<void>(self, "pingReply");
    }
    void processMessage(LogLevel_t level, const std::string& path,
            int index, bool state, uint64_t time) {
        if (PyObject_HasAttrString(self, "processMessage"))
            bp::call_method<void>(self, "processMessage",
                    int(level), path, index, state, time);
    }
    void protocolLog(LogLevel_t level, const std::string& message) {
        if (PyObject_HasAttrString(self, "protocolLog"))
            bp::call_method<void>(self, "protocolLog", level, message);
    }
    bool alive() {
        return PyObject_HasAttrString(self, "alive")
            ? bp::call_method<bool>(self, "alive")
            : true;
    }

    void subscribe(bp::object obj, const std::string& path, double interval, int id);
};/*}}}*/

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
struct Subscription
{/*{{{*/
    Subscription(
            PdCom::Subscriber* subscriber,
            const PdCom::Variable::Subscription* s);

    ~Subscription()
    {
        log_debug("dtor: Subscription\n");
        subscription->cancel();
    }

    PdCom::Subscriber* const subscriber;
    const PdCom::Variable::Subscription* subscription;

    void cancel() {
        subscription->cancel();
    }

    bp::object getValue() const {
        return (this->*m_getValue)();
    };

    typedef bp::object (Subscription::*GetValue_t)() const;
    GetValue_t m_getValue;

    template <class T>
        bp::object getValueFunc() const
        {/*{{{*/
            T val[subscription->variable->nelem];
            subscription->getValue(val, 0, subscription->variable->nelem);

            if (subscription->variable->nelem > 1) {
                bp::list result;
                for (const T* p = val;
                        p < val + subscription->variable->nelem; ++p)
                    result.append(*p);
                return result;
            }
            else
                return bp::object(*val);
        }/*}}}*/

    // Proxy methods
    int id()            const { return subscription->id; }
    int decimation()    const { return subscription->decimation; }
    uint64_t time()     const { return **subscription->time_ns; }
    bool poll()         const { return subscription->poll(); }
    bp::object variable() const {
        return static_cast<Process*>(subscription->process)
            ->get(subscription->variable);
    }
};/*}}}*/

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
struct Variable
{/*{{{*/
    Variable(Process* process, const PdCom::Variable* var);
    ~Variable() { process->forget(variable); }

    Process* const process;
    const PdCom::Variable* const variable;

    const bp::object processObject;     // For reference counting

    typedef bool (Variable::*SetValue_t)(bp::object value) const;
    SetValue_t m_setValue;
    bool setValue(bp::object val) const
    {/*{{{*/
        if (PyString_Check(val.ptr())) {

            if (variable->type != PdCom::Variable::int8_T
                    and variable->type != PdCom::Variable::uint8_T)
                return false;

            int8_t str[variable->nelem];
            size_t end = std::min(variable->nelem-1, size_t(bp::len(val)));
            bp::str s(val);

            for (size_t i = 0; i < end; ++i)
                str[i] = bp::extract<char>(s[i]);
            str[end] = '\0';

            return variable->setValue(str, 0, end);
        }

        return (this->*m_setValue)(val);
    };/*}}}*/

    template <class T>
        bool setValueFunc(bp::object obj) const
        {/*{{{*/
            T val[variable->nelem];
            size_t len;

            if (variable->nelem == 1) {
                len = 1;
                *val = bp::extract<T>(obj);
            }
            else {
                len = std::min(variable->nelem, size_t(bp::len(obj)));
                for (size_t i = 0; i < len; ++i)
                    val[i] = bp::extract<T>(obj[i]);
            }

            return variable->setValue(val, 0, len);
        }/*}}}*/

    // Proxy methods
    PdCom::Variable::Type type() const { return variable->type; }
    std::string ctype()         const { return variable->ctype; }
    double      sampleTime()    const { return variable->sampleTime; }
    bool        writeable()     const { return variable->writeable; }
    std::string alias()         const { return variable->alias; }
    std::string vpath()         const { return variable->path(); }
    std::string vname()         const { return variable->name(); }
    size_t      nelem()         const { return variable->nelem; }
    size_t      task()          const { return variable->task; }
    bp::object  dim()           const { return bp::object(variable->dim); }
    bool        isScalar()      const { return variable->isScalar(); }
    bool        isVector()      const { return variable->isVector(); }
};/*}}}*/

///////////////////////////////////////////////////////////////////////////
Variable::Variable(Process* process, const PdCom::Variable* var):
    process(process), variable(var),
    processObject(bp::handle<>(bp::borrowed(process->self)))
{/*{{{*/
    static const SetValue_t func[] = {
        &Variable::setValueFunc<    bool>,
        &Variable::setValueFunc< uint8_t>,
        &Variable::setValueFunc<  int8_t>,
        &Variable::setValueFunc<uint16_t>,
        &Variable::setValueFunc< int16_t>,
        &Variable::setValueFunc<uint32_t>,
        &Variable::setValueFunc< int32_t>,
        &Variable::setValueFunc<uint64_t>,
        &Variable::setValueFunc< int64_t>,
        &Variable::setValueFunc<  double>,
        &Variable::setValueFunc<   float>,
    };

    m_setValue = func[variable->type];
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
bp::object Process::get(const PdCom::Variable* var)
{/*{{{*/
    // This function creates a python proxy object for the variable
    // and stores it for future use
    if (!var)
        return bp::object();

    PyObject*& obj = variableMap[var];
    if (obj)
        return bp::object(bp::handle<>(bp::borrowed(obj)));

    bp::object variable(
            boost::shared_ptr<Variable>(new Variable(this, var)));
    obj = variable.ptr();
    return variable;
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
void Process::listReply(
        std::list<const PdCom::Variable*>& variables,
        std::list<std::string>& dir)
{/*{{{*/
    bp::list v;
    while (!variables.empty()) {
        v.append(get(variables.front()));
        variables.pop_front();
    }

    bp::list p;
    while (!dir.empty()) {
        p.append(bp::str(dir.front()));
        dir.pop_front();
    }

    bp::call_method<void>(self, "listReply", v, p);
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
// Careful: cannot derive Subscriber directly from PdCom::Process::Subscriber
// (is-a), but need a 2-level system with a has-a structure. The problem is
// that if Subscriber is a PdCom::Process::Subscriber, PdCom::Process
// would die before Subscriber dies, because all handles on Process
// would be released while Subscriber is still active.
//
// With a 2-stage Subscriber having a PdCom::Process::Subscriber,
// PdCom::Process::Subscriber can be deleted before Subscriber is deleted,
// thus still being able to hold on to PdCom::Process!
struct Subscriber
{/*{{{*/
    struct PySubscriber: PdCom::Subscriber {
        PySubscriber(PyObject* self): self(self)
        {
        }

        ~PySubscriber() {
            log_debug("dtor: Subscriber\n");
        }

        // Reimplemented from PdCom::Process::Subscriber
        void newValue(const PdCom::Variable::Subscription* subscription) {
            bp::call_method<void>(self, "newValue",
                    getSubscriptionObject(subscription));
        }

        // Reimplemented from PdCom::Process::Subscriber
        void newGroupValue(uint64_t time_ns) {
            bp::call_method<void>(self, "newGroupValue", time_ns);
        }

        // Reimplemented from PdCom::Process::Subscriber
        void invalid(const std::string& path, int id) {
            bp::call_method<void>(self, "invalid", path, id);
        }

        // Reimplemented from PdCom::Process::Subscriber
        void active(const std::string& path, 
                const PdCom::Variable::Subscription* subscription) {
            bp::call_method<void>(self, "active", path,
                    getSubscriptionObject(subscription));
        }

        bp::object getSubscriptionObject(
                const PdCom::Variable::Subscription* s) {
            bp::object subscription;

            PyObject*& obj = map[s];

            if (obj) {
                subscription = bp::object(bp::handle<>(bp::borrowed(obj)));
            }
            else {
                subscription = bp::object(
                        boost::shared_ptr<Subscription>(new Subscription(this, s)));
                obj = subscription.ptr();
            }

            return subscription;
        }

        std::map<const PdCom::Variable::Subscription*, PyObject*> map;

        PyObject* const self;
    };

    Subscriber(PyObject* self): subscriber(new PySubscriber(self))
    {
    }

    ~Subscriber() {
        delete subscriber;
    }

    PySubscriber* const subscriber;
};/*}}}*/

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void Process::subscribe(
        bp::object obj, const std::string& path, double interval, int id)
{
    Subscriber& s = bp::extract<Subscriber&>(obj);
    PdCom::Process::subscribe(s.subscriber, path, interval, id);
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
Subscription::Subscription(
        PdCom::Subscriber* subscriber,
        const PdCom::Variable::Subscription* s):
    subscriber(subscriber), subscription(s)
{/*{{{*/
    static const GetValue_t func[] = {
        &Subscription::getValueFunc<    bool>,
        &Subscription::getValueFunc< uint8_t>,
        &Subscription::getValueFunc<  int8_t>,
        &Subscription::getValueFunc<uint16_t>,
        &Subscription::getValueFunc< int16_t>,
        &Subscription::getValueFunc<uint32_t>,
        &Subscription::getValueFunc< int32_t>,
        &Subscription::getValueFunc<uint64_t>,
        &Subscription::getValueFunc< int64_t>,
        &Subscription::getValueFunc<  double>,
        &Subscription::getValueFunc<   float>,
    };
    log_debug("ctor: Subscription");
    m_getValue = func[subscription->variable->type];
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
template <typename T>
struct container_to_pylist
{/*{{{*/
    static PyObject* convert(const T& container)
    {
        bp::list result;
        BOOST_FOREACH(const typename T::value_type& value, container)
            result.append(value);
        return bp::incref(result.ptr());
    }
};/*}}}*/

///////////////////////////////////////////////////////////////////////////
template <typename T>
struct convert_container_to_pylist:
    bp::to_python_converter<T, container_to_pylist<const T> > { };

///////////////////////////////////////////////////////////////////////////
BOOST_PYTHON_MODULE(pdcom)
{
    convert_container_to_pylist<std::vector<size_t> > ();

    {
        bp::list libversion;
        libversion.append(PDCOM_MAJOR);
        libversion.append(PDCOM_MINOR);
        libversion.append(PDCOM_RELEASE);

        bp::list pylibversion;
        pylibversion.append(PYLIB_MAJOR);
        pylibversion.append(PYLIB_MINOR);

        bp::list version;
        version.append(libversion);
        version.append(pylibversion);

        bp::scope().attr("version") = version;
    }

    {
        bp::scope process =
            bp::class_<Process, Process, boost::noncopyable>("Process")
            .def("reset",           &Process::reset)
            .def("asyncData",       &Process::asyncData)
            .def("list",            &Process::list)
            .def("find",            &Process::find)
            .def("login",           &Process::login)
            .def("ping",            &Process::ping)
            .def("name",            &Process::name)
            .def("version",         &Process::version)
            .def("messageHistory",  &Process::messageHistory)
            .def("subscribe",       &Process::subscribe)
            .def("parameterMonitor", &Process::parameterMonitor)
            ;

        bp::enum_<PdCom::Process::LogLevel_t>("LogLevel")
            .value("Emergency",     PdCom::Process::Emergency)
            .value("Alert",         PdCom::Process::Alert)
            .value("Critical",      PdCom::Process::Critical)
            .value("Error",         PdCom::Process::Error)
            .value("Warn",          PdCom::Process::Warn)
            .value("Info",          PdCom::Process::Info)
            .value("Debug",         PdCom::Process::Debug)
            .value("Trace",         PdCom::Process::Trace)
            .export_values()
            ;
    }

    bp::class_<Subscriber, Subscriber, boost::noncopyable>("Subscriber")
        ;

    {
        bp::scope variable =
            bp::class_<Variable,
            boost::noncopyable, boost::shared_ptr<Variable> >("Variable",
                    bp::no_init)
                .def_readonly("type",       &Variable::type)
                .def_readonly("ctype",      &Variable::ctype)
                .def_readonly("sampleTime", &Variable::sampleTime)
                .def_readonly("writeable",  &Variable::writeable)
                .def_readonly("alias",      &Variable::alias)
                .def_readonly("path",       &Variable::vpath)
                .def_readonly("name",       &Variable::vname)
                .def_readonly("nelem",      &Variable::nelem)
                .def_readonly("dim",        &Variable::dim)
                .def_readonly("task",       &Variable::task)
                .def_readonly("isScalar",   &Variable::isScalar)
                .def_readonly("isVector",   &Variable::isVector)

                .def("setValue",    &Variable::setValue)
                ;

        bp::class_<Subscription,
            boost::noncopyable, boost::shared_ptr<Subscription> >(
                    "Subscription", bp::no_init)
                .def_readonly("id",         &Subscription::id)
                .def_readonly("variable",   &Subscription::variable)
                .def_readonly("decimation", &Subscription::decimation)
                .def_readonly("time",       &Subscription::time)

                .def("poll",        &Subscription::poll)
                .def("getValue",    &Subscription::getValue)
                .def("cancel",      &Subscription::cancel)
                ;

        bp::enum_<PdCom::Variable::Type>("Type")
            .value("boolean_T", PdCom::Variable::boolean_T)
            .value("uint8_T",   PdCom::Variable::  uint8_T)
            .value( "int8_T",   PdCom::Variable::   int8_T)
            .value("uint16_T",  PdCom::Variable:: uint16_T)
            .value( "int16_T",  PdCom::Variable::  int16_T)
            .value("uint32_T",  PdCom::Variable:: uint32_T)
            .value( "int32_T",  PdCom::Variable::  int32_T)
            .value("uint64_T",  PdCom::Variable:: uint64_T)
            .value( "int64_T",  PdCom::Variable::  int64_T)
            .value("double_T",  PdCom::Variable:: double_T)
            .value("single_T",  PdCom::Variable:: single_T)
            .export_values()
            ;
    }
}
