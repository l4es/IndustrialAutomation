
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

#include "config.h"

#include "../Debug.h"

#include <iostream>
#include <unistd.h>     // exit(), sleep()
#include <cerrno>       // errno
#include <cstdio>       // perror()
#include <sys/mman.h>   // mmap(), munmap()
#include <signal.h>     // signal()
#include <cerrno>       // EIO

#include "pdserv.h"
#include "Main.h"
#include "Task.h"
#include "Parameter.h"
#include "Signal.h"
#include "Event.h"
#include "Pointer.h"
#include "ShmemDataStructures.h"
#include "../Session.h"
#include "../Config.h"
#include "../Database.h"

/////////////////////////////////////////////////////////////////////////////
struct SDO {
    enum {ParamChange = 1, PollSignal} type;

    union {
        struct {
            const Parameter *parameter;
            unsigned int offset;
            unsigned int count;
        } paramChangeReq;

        struct {
            int rv;
            struct timespec time;
        } paramChangeAck;

        const Signal* signal;
        struct timespec time;
    };
};

/////////////////////////////////////////////////////////////////////////////
const double Main::bufferTime = 2.0;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Main::Main( const char *name, const char *version,
        int (*gettime)(struct timespec*)):
    PdServ::Main(name, version),
    rttime(gettime ? gettime : &PdServ::Main::localtime)
{
    shmem_len = 0;
    shmem = 0;
}

/////////////////////////////////////////////////////////////////////////////
Main::~Main()
{
    terminate();

    while (task.size()) {
        delete task.front();
        task.pop_front();
    }

    while (parameters.size()) {
        delete parameters.front();
        parameters.pop_front();
    }

    // close pipes
    ::close(terminatePipe);
    ::close(ipcTx);
    ::close(ipcRx);

    ::munmap(shmem, shmem_len);
}

/////////////////////////////////////////////////////////////////////////////
void Main::setConfigFile(const char *file)
{
    configFile = file;
}

/////////////////////////////////////////////////////////////////////////////
int Main::setup()
{
    int rv;
    int ipc_pipe[3][2];
    time_t persistTimeout;
    fd_set fds;
    struct timeval timeout, now, delay;

    readConfiguration();
    setupLogging();
    persistTimeout = setupPersistent();

    // Initialize library
    rv = prefork_init();
    if (rv)
        return rv;

    // Open a pipe between the two processes. This is used to inform the
    // child that the parent has died
    if (::pipe(ipc_pipe[0]) or ::pipe(ipc_pipe[1]) or ::pipe(ipc_pipe[2])) {
        rv = errno;
        ::perror("pipe()");
        return rv;
    }

    // Immediately split off a child. The parent returns to the caller so
    // that he can get on with his job.
    //
    // The child continues from here.
    //
    // It is intentional that the child has the same process group as
    // the parent so that it gets all the signals too.
    pid = ::fork();
    if (pid < 0) {
        // Some error occurred
        ::perror("fork()");
        return errno;
    }
    else if (pid) {
        // Parent here. Return to the caller
        ::close(ipc_pipe[0][0]);
        ipcTx = ipc_pipe[0][1];

        ipcRx = ipc_pipe[1][0];
        ::close(ipc_pipe[1][1]);

        ::close(ipc_pipe[2][0]);
        terminatePipe = ipc_pipe[2][1];

        // Send PID to the child, indicating that parent is running
        if (::write(terminatePipe, &pid, sizeof(pid)) != sizeof(pid))
            std::runtime_error("Main::setup(): pid ::write() failed");

        return postfork_rt_setup();
    }

    // Only child runs after this point
    ipcRx = ipc_pipe[0][0];
    ::close(ipc_pipe[0][1]);

    ::close(ipc_pipe[1][0]);
    ipcTx = ipc_pipe[1][1];

    terminatePipe = ipc_pipe[2][0];
    ::close(ipc_pipe[2][1]);

    // Wait till main thread has been initialized
    if (::read(terminatePipe, &pid, sizeof(pid)) != sizeof(pid)
            or pid != getpid())
        std::runtime_error("Main::setup(): pid ::read() failed");

    // Ignore common terminating signals
    ::signal(SIGINT, SIG_IGN);
    ::signal(SIGTERM, SIG_IGN);

    postfork_nrt_setup();

    startServers();

    ::gettimeofday(&timeout, 0);
    timeout.tv_sec += persistTimeout;

    FD_ZERO(&fds);

    // Stay in this loop until real-time thread exits, in which case
    // ipc_pipe[0] becomes readable
    struct ::EventData* eventData = eventDataStart;
    ipc_error = false;
    do {
        for (TaskList::iterator it = task.begin();
                it != task.end(); ++it)
            static_cast<Task*>(*it)->nrt_update();

        if (persistTimeout) {
            if (::gettimeofday(&now, 0)) {
                rv = errno;
                break;
            }

            if ( now.tv_sec >= timeout.tv_sec) {
                timeout.tv_sec += persistTimeout;
                savePersistent();
            }
        }

        while (eventData != *eventDataWp) {
            newEvent(eventData->event, eventData->index,
                    eventData->state, &eventData->time);
            if (++eventData == eventDataEnd)
                eventData = eventDataStart;
        }

        FD_SET(terminatePipe, &fds);
        delay.tv_sec = 1;
        delay.tv_usec = 0;
        rv = ::select(terminatePipe + 1, &fds, 0, 0, &delay);
    } while (!(rv or ipc_error));

    // Ignore rv if ipc_pipe[0] is readable
    if (rv == 1)
        rv = 0;

    stopServers();

    ::exit(rv);
}

/////////////////////////////////////////////////////////////////////////////
int Main::readConfiguration()
{
    const char *env;
    const char *err = 0;

    // Load custom configuration file
    if (!configFile.empty()) {
        err = m_config.load(configFile.c_str());
        if (err)
            std::cout
                << "Error loading configuration file "
                << configFile << " specified on command line: " << err << std::endl;
        else {
            log_debug("Loaded specified configuration file %s",
                    configFile.c_str());
        }
    }
    else if ((env = ::getenv("PDSERV_CONFIG")) and ::strlen(env)) {
        // Try to load environment configuration file
        err = m_config.load(env);

        if (err)
            std::cout << "Error loading config file " << env
                << " specified in environment variable PDSERV_CONFIG: "
                << err << std::endl;
        else {
            log_debug("Loaded ENV config %s", env);
        }
    }
    else {
        // Try to load default configuration file
        const char *f = QUOTE(SYSCONFDIR) "/pdserv.conf";
        err = m_config.load(f);
        if (err) {
            std::cout << "Error loading default config file " << f << ": "
                << (::access(f, R_OK)
                        ? ": File is not readable"
                        : err)
                << std::endl;
        }
        else {
            log_debug("Loaded default configuration file %s", f);
        }
    }

    if (!m_config or err) {
        log_debug("No configuration loaded");
    }

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
PdServ::Config Main::config(const char* key) const
{
    return m_config[key];
}

/////////////////////////////////////////////////////////////////////////////
Task* Main::addTask(double sampleTime, const char *name)
{
    task.push_back(new Task(this, task.size(), sampleTime, name));
    return task.back();
}

/////////////////////////////////////////////////////////////////////////////
int Main::gettime(struct timespec* t) const
{
    return rttime(t);
}

/////////////////////////////////////////////////////////////////////////////
Event* Main::addEvent (const char *path, int prio, size_t nelem)
{
    PdServ::Event::Priority eventPrio;
    switch (prio) {
        case 1: eventPrio = PdServ::Event::Alert;       break;
        case 2: eventPrio = PdServ::Event::Critical;    break;
        case 3: eventPrio = PdServ::Event::Error;       break;
        case 4: eventPrio = PdServ::Event::Warning;     break;
        case 5: eventPrio = PdServ::Event::Notice;      break;
        case 6: eventPrio = PdServ::Event::Info;        break;
        default:
            eventPrio = prio <= 0
                ? PdServ::Event::Emergency
                : PdServ::Event::Debug;
    }

    events.push_back(new Event(this, path, eventPrio, nelem));

    return events.back();
}

/////////////////////////////////////////////////////////////////////////////
Parameter* Main::addParameter( const char *path,
        unsigned int mode, const PdServ::DataType& datatype,
        void *addr, size_t n, const size_t *dim)
{
    parameters.push_back(
            new Parameter(this, addr, path, mode, datatype, n, dim));

    return parameters.back();
}

/////////////////////////////////////////////////////////////////////////////
void Main::setEvent(Event* event,
        size_t element, bool state, const timespec *time)
{
    ost::MutexLock lock(eventMutex);

    struct ::EventData *eventData = *eventDataWp;
    eventData->event = event;
    eventData->index = element;
    eventData->state = state;
    eventData->time  = *time;

    if (++eventData == eventDataEnd)
        eventData = eventDataStart;
    *eventDataWp = eventData;
}

/////////////////////////////////////////////////////////////////////////////
int Main::setValue(const PdServ::ProcessParameter* p,
        const char* buf, size_t offset, size_t count)
{
    ost::MutexLock lock(sdoMutex);
    const Parameter* param = static_cast<const Parameter*>(p);
    char* shmAddr = param->shmAddr + offset;

    // Backup old values in case of write failure
    char backup[count];
    std::copy(shmAddr, shmAddr + count, backup);

    // Copy new data to shared memory
    std::copy(buf, buf + count, shmAddr);

    // Setup change request
    struct SDO sdo;
    sdo.type = SDO::ParamChange;
    sdo.paramChangeReq.parameter = param;
    sdo.paramChangeReq.offset = offset;
    sdo.paramChangeReq.count = count;

    if (::write(ipcTx, &sdo, sizeof(sdo)) != sizeof(sdo)) {
        log_debug("Main::setValue(): SDO ::write() failed");
        ipc_error = true;
        return -EIO;
    }

    if (::read(ipcRx, &sdo, sizeof(sdo)) != sizeof(sdo)) {
        log_debug("Main::setValue(): SDO ::read() failed");
        ipc_error = true;
        return -EIO;
    }

    if (!sdo.paramChangeAck.rv)
        param->mtime = sdo.paramChangeAck.time; // Save time of update
    else
        // Write failure. Restore data
        std::copy(backup, backup + count, shmAddr);

    return sdo.paramChangeAck.rv;
}

/////////////////////////////////////////////////////////////////////////////
void Main::initializeParameter(PdServ::Parameter* p,
        const char* data, const struct timespec* mtime,
        const PdServ::Signal* s)
{
    if (data) {
        log_debug("Restoring %s", p->path.c_str());
        const Parameter *parameter = static_cast<const Parameter*>(p);
        std::copy(data, data + parameter->memSize, parameter->addr);
        parameter->mtime = *mtime;
    }

    if (s) {
        const Signal* signal = static_cast<const Signal*>(s);
        signal->task->makePersistent(signal);
    }
}

/////////////////////////////////////////////////////////////////////////////
bool Main::getPersistentSignalValue(const PdServ::Signal *s,
        char* buf, struct timespec* time)
{
    const struct timespec* t;
    bool rv = static_cast<const Signal*>(s)->task->getPersistentValue(
            s, buf, &t);

    if (rv and time)
        *time = *t;

    return rv;
}

/////////////////////////////////////////////////////////////////////////////
int Main::getValue(const Signal *signal, void* dest, struct timespec* time)
{
    ost::MutexLock lock(sdoMutex);
    struct SDO sdo;

    sdo.type = SDO::PollSignal;
    sdo.signal = signal;

    if (::write(ipcTx, &sdo, sizeof(sdo)) != sizeof(sdo)) {
        log_debug("Main::getValue(): SDO ::write() failed");
        ipc_error = true;
        return -EIO;
    }

    if (::read(ipcRx, &sdo, sizeof(sdo)) != sizeof(sdo)) {
        log_debug("Main::getValue(): SDO ::read() failed");
        ipc_error = true;
        return -EIO;
    }

    std::copy(signalData, signalData + signal->memSize,
            reinterpret_cast<char*>(dest));

    if (time)
        *time = sdo.time;

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
PdServ::Parameter* Main::findParameter(const std::string& path) const
{
    for (ParameterList::const_iterator it = parameters.begin();
            it != parameters.end(); ++it)
        if ((*it)->path == path)
            return *it;
    return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Orangization of shared memory:
//      struct SDOStruct        sdo
//      char                    parameterData (binary data of all parameters)
//      char                    pdoData
//      struct EventData        eventDataStart
//
int Main::prefork_init()
{
    size_t numTasks = task.size();
    size_t taskMemSize[numTasks];
    size_t i, eventCount;
    size_t maxSignalSize = 0;

    // Find out the largest signal size to reserve space in
    // shared memory for polling
    for (TaskList::const_iterator it = task.begin();
            it != task.end(); ++it) {
        std::list<const PdServ::Signal*> signals =
            static_cast<PdServ::Task*>(*it)->getSignals();

        while (!signals.empty()) {
            const PdServ::Signal* s = signals.front();

            if (s->memSize > maxSignalSize)
                maxSignalSize = s->memSize;

            signals.pop_front();
        }

    }
    shmem_len += maxSignalSize;

    // The following two variables are used to organize parameters according
    // to the size of their elements so that their data type alignment is
    // correct.
    //
    // dataTypeIndex[] maps the data type to the index in parameterDataOffset,
    // e.g. a parameter with data type double (sizeof() = 8) will then go into
    // container parameterDataOffset[dataTypeIndex[8]]
    //
    // parameterDataOffset[] holds the start index of a data types with
    // 8, 4, 2 and 1 bytes alignment
    const size_t dataTypeIndex[PdServ::DataType::maxWidth+1] = {
        3 /*0*/, 3 /*1*/, 2 /*2*/, 3 /*3*/,
        1 /*4*/, 3 /*5*/, 3 /*6*/, 3 /*7*/, 0 /*8*/
    };
    size_t parameterDataOffset[5] = {0, 0, 0, 0, 0};    // need one extra!

    for (ParameterList::iterator it = parameters.begin();
            it != parameters.end(); it++) {
        const Parameter *p = static_cast<const Parameter*>(*it);

        // Push the next smaller data type forward by the parameter's
        // memory requirement
        parameterDataOffset[dataTypeIndex[p->dtype.align()] + 1] += p->memSize;
    }

    // Accumulate the offsets so that they follow each other in the shared
    // data space. This also has the effect, that the value of
    // parameterDataOffset[4] is the total memory requirement of all
    // parameters
    for (i = 1; i < 5; ++i)
        parameterDataOffset[i] += parameterDataOffset[i-1];

    // Extend shared memory size with the parameter memory requirement
    // and as many sdo's for every parameter.
    shmem_len += parameterDataOffset[4];

    // Now check how much memory is required for events
    eventCount = 0;
    for (EventList::iterator it = events.begin(); it != events.end(); ++it)
        eventCount += (*it)->nelem;

    // Increase shared memory by the number of events as well as
    // enough capacity to store eventDataLen event changes
    const size_t eventLen = 10;     // Arbitrary
    shmem_len += sizeof(*eventDataStart) * eventLen * eventCount;

    shmem_len += sizeof(*eventDataWp);  // Memory location for write pointer

    // Find out the memory requirement for the tasks to pipe their variables
    // out of the real time environment
    i = 0;
    for (TaskList::const_iterator it = task.begin();
            it != task.end(); ++it) {
        taskMemSize[i] = ptr_align(
                static_cast<const Task*>(*it)->getShmemSpace(bufferTime));
        shmem_len += taskMemSize[i++];
    }

    // Fudge factor. Every ptr_align can silently increase the pointer in
    // shmem by sizeof(unsigned long).
    // At the moment there are roughly 6 ptr_align's, take 10 to make sure!
    shmem_len += (10 + task.size())*sizeof(unsigned long);

    shmem = ::mmap(0, shmem_len, PROT_READ | PROT_WRITE,
            MAP_SHARED | MAP_ANON, -1, 0);
    if (MAP_FAILED == shmem) {
        // log(LOGCRIT, "could not mmap
        // err << "mmap(): " << strerror(errno);
        ::perror("mmap()");
        return errno;
    }

    // Clear memory; at the same time prefault it, so it does not
    // get swapped out
    ::memset(shmem, 0, shmem_len);

    // Now spread the shared memory for the users thereof

    // 1: Parameter data
    parameterData = ptr_align<char>(shmem);
    for (ParameterList::iterator it = parameters.begin();
            it != parameters.end(); it++) {
        Parameter *p = *it;
        p->shmAddr = parameterData
            + parameterDataOffset[dataTypeIndex[p->dtype.align()]];
        parameterDataOffset[dataTypeIndex[p->dtype.align()]] += p->memSize;

        std::copy(p->addr, p->addr + p->memSize, p->shmAddr);
    }

    // 2: Signal data area for polling
    signalData = ptr_align<char>(parameterData + parameterDataOffset[4]);

    // 3: Streaming data for tasks
    char* buf = ptr_align<char>(signalData + maxSignalSize);
    i = 0;
    for (TaskList::iterator it = task.begin(); it != task.end(); ++it) {
        static_cast<Task*>(*it)->prepare(buf, buf + taskMemSize[i]);
        buf = ptr_align<char>(buf + taskMemSize[i++]);
    }

    // 4: Event data
    eventDataWp    = ptr_align<struct ::EventData*>(buf);
    eventDataStart = ptr_align<struct ::EventData>(eventDataWp + 1);
    eventDataEnd   = eventDataStart + eventLen * eventCount;
    *eventDataWp   = eventDataStart;

    log_debug("shmem=%p shmem_end=%p(%zu)\n"
            "param=%p(%zi)\n"
            "stream=%p(%zi)\n"
            "end=%p(%zi)\n",
            shmem, (char*)shmem + shmem_len, shmem_len,
            signalData, (char*)signalData - (char*)shmem,
            eventDataWp, (char*)eventDataWp - (char*)shmem,
            eventDataEnd, (char*)eventDataEnd - (char*)shmem);

    if ((void*)(eventDataEnd + 1) > (void*)((char*)shmem + shmem_len)) {
        log_debug("Not enough memory");
        return ENOMEM;
    }

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
int Main::postfork_rt_setup()
{
    // Parent here; go back to the caller
    for (TaskList::iterator it = task.begin();
            it != task.end(); ++it)
        static_cast<Task*>(*it)->rt_init();

    // Start supervisor thread
    start();

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
int Main::postfork_nrt_setup()
{
    // Parent here; go back to the caller
    for (TaskList::iterator it = task.begin();
            it != task.end(); ++it)
        static_cast<Task*>(*it)->nrt_init();
    return 0;
}

/////////////////////////////////////////////////////////////////////////////
std::list<const PdServ::Parameter*> Main::getParameters() const
{
    return std::list<const PdServ::Parameter*>(
            parameters.begin(), parameters.end());
}

/////////////////////////////////////////////////////////////////////////////
std::list<const PdServ::Event*> Main::getEvents() const
{
    return std::list<const PdServ::Event*>(events.begin(), events.end());
}

/////////////////////////////////////////////////////////////////////////////
std::list<const PdServ::Task*> Main::getTasks() const
{
    return std::list<const PdServ::Task*>(task.begin(), task.end());
}

/////////////////////////////////////////////////////////////////////////////
void Main::prepare(PdServ::Session* /*session*/) const
{
}

/////////////////////////////////////////////////////////////////////////////
void Main::cleanup(const PdServ::Session* /*session*/) const
{
}

/////////////////////////////////////////////////////////////////////////////
void Main::run()
{
    const PdServ::Variable *variable;
    struct SDO sdo;

    while (true) {
        if (::read(ipcRx, &sdo, sizeof(sdo)) != sizeof(sdo))
            std::runtime_error("Main::run(): SDO ::read() failed");

        switch (sdo.type) {
            case SDO::ParamChange:
                variable = sdo.paramChangeReq.parameter;
                sdo.paramChangeAck.rv = 
                    sdo.paramChangeReq.parameter->write_cb(
                            reinterpret_cast<const pdvariable*>(variable),
                            (sdo.paramChangeReq.parameter->addr
                             + sdo.paramChangeReq.offset),
                            (sdo.paramChangeReq.parameter->shmAddr
                             + sdo.paramChangeReq.offset),
                            sdo.paramChangeReq.count,
                            &sdo.paramChangeAck.time,
                            sdo.paramChangeReq.parameter->priv_data);

                log_debug("Parameter change of %s; rv=%i\n",
                        variable->path.c_str(),
                        sdo.paramChangeAck.rv);

                break;

            case SDO::PollSignal:
                variable = sdo.signal;
                sdo.signal->read_cb(
                        reinterpret_cast<const pdvariable*>(variable),
                        signalData,
                        sdo.signal->addr,
                        sdo.signal->memSize,
                        &sdo.time,
                        sdo.signal->priv_data);
//                log_debug("Signal poll of %s\n", variable->path.c_str());
                break;
        };

        if (::write(ipcTx, &sdo, sizeof(sdo)) != sizeof(sdo))
            std::runtime_error("Main::run(): SDO ::write() failed");
    }
}

/////////////////////////////////////////////////////////////////////////////
void Main::final()
{
    ::close(ipcRx);
    ::close(ipcTx);
}
