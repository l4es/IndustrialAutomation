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

#include <sstream>
#include <functional>
#include <cerrno>
#include <cstdlib>              // exit()
#include <sys/ioctl.h>          // ioctl()
#include <unistd.h>             // fork()
#include <sys/mman.h>
#include <sys/select.h>         // select()
#include <app_taskstats.h>
#include <log4cplus/loggingmacros.h>
#include <dsignal.h>

#include "Main.h"
#include "Task.h"
#include "Signal.h"
#include "Event.h"
#include "EventQ.h"
#include "Parameter.h"
#include "../Config.h"
#include "../Session.h"

/////////////////////////////////////////////////////////////////////////////
struct SessionData {
    size_t eventReadPointer;
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Main::Main(const struct app_properties& p,
        const PdServ::Config& config, int fd):
    PdServ::Main(p.name, p.version),
    app_properties(p), log(log4cplus::Logger::getRoot()),
    m_config(config), pid(::getpid()), fd(fd),
    parameterBuf(0)
{
}

/////////////////////////////////////////////////////////////////////////////
Main::~Main()
{
    delete mainTask;

    while (parameters.size()) {
        delete parameters.front();
        parameters.pop_front();
    }
}

/////////////////////////////////////////////////////////////////////////////
int Main::serve()
{
    fd_set fds;
    int maxfd, sfd, rv;
    time_t persistTimeout;
    struct timeval timeout, now;
    int photoPtr;

    setupLogging();
    postfork_nrt_setup();
    persistTimeout = setupPersistent();
    readPointer = ioctl(fd, RESET_BLOCKIO_RP);
    photoPtr = readPointer;
    startServers();

    // In foreground mode, only catch SIGCHLD
    if (daemon_signal_init(SIGHUP, SIGINT, SIGTERM, -1) < 0) {
        rv = errno;
        goto out;
    }

    sfd = daemon_signal_fd();
    maxfd = (sfd > fd ? sfd : fd) + 1;

    ::gettimeofday(&timeout, 0);

    FD_ZERO(&fds);
    while (true) {
        int n;

        FD_SET(fd, &fds);
        FD_SET(sfd, &fds);

        n = ::select(maxfd, &fds, 0, 0, 0);
        if (n <= 0) {
            // Some error occurred in select()
            rv = errno;
            LOG4CPLUS_FATAL(log,
                    LOG4CPLUS_TEXT("Received fatal error in select() = "
                        << errno));
            goto out;
        }

        if (FD_ISSET(fd, &fds)) {
            // New data pages
            struct data_p data_p;
            ioctl(fd, GET_WRITE_PTRS, &data_p);

            if (data_p.wp == -ENOSPC) {
                photoPtr = ioctl(fd, RESET_BLOCKIO_RP);
                continue;
            }

            for (int i = photoPtr; i < data_p.wp; ++i) {

                // Mark photo as ready
                photoReady[i] = photoCount++;
                readPointer = i;

                // Go through event list to see if something happened
                for (EventList::const_iterator it = events.begin();
                        it != events.end(); ++it)
                    eventQ->test(*it, i);
            }

            // Tell kernel that reading has finished
            photoPtr = ioctl(fd, SET_BLOCKIO_RP, data_p.wp);
        }

        if (FD_ISSET(sfd, &fds)) {
            int signo = daemon_signal_next();

            switch (signo) {
                case SIGHUP:
                    LOG4CPLUS_INFO_STR(log,
                            LOG4CPLUS_TEXT("SIGHUP received. Reloading config"));
                    m_config.reload();
                    savePersistent();
                    break;

                default:
                    LOG4CPLUS_INFO(log,
                            LOG4CPLUS_TEXT("Terminating on ")
                            << LOG4CPLUS_C_STR_TO_TSTRING(
                                strsignal(signo))); 
                    stopServers();
                    daemon_signal_done();
                    return 0;
            }
        }

        // Get current time and save persistent variables on timeout
        if (::gettimeofday(&now, 0)) {
            rv = errno;
            break;
        }
        if (timercmp(&now, &timeout, <))
            timeout = now;

        if (persistTimeout
                and now.tv_sec >= timeout.tv_sec + persistTimeout) {
            timeout.tv_sec += persistTimeout;
            savePersistent();
        }
    }

out:
    LOG4CPLUS_FATAL(log,
            LOG4CPLUS_TEXT("Failed: ")
            << LOG4CPLUS_C_STR_TO_TSTRING(strerror(errno)));
    stopServers();
    daemon_signal_done();

    return rv;
}

/////////////////////////////////////////////////////////////////////////////
int Main::postfork_nrt_setup()
{
    size_t shmem_len;
    size_t eventCount = 0;

    LOG4CPLUS_INFO(log,
            LOG4CPLUS_TEXT("Started new application server (pid=")
                << pid << LOG4CPLUS_TEXT(")"));
    LOG4CPLUS_INFO(log,
            LOG4CPLUS_TEXT("   Name: ") << app_properties.name);
    LOG4CPLUS_INFO(log,
            LOG4CPLUS_TEXT("   Block count: ") << app_properties.rtB_count);
    LOG4CPLUS_INFO(log,
            LOG4CPLUS_TEXT("   Block size: ") << app_properties.rtB_size);
    LOG4CPLUS_INFO(log,
            LOG4CPLUS_TEXT("   Parameter size: ") << app_properties.rtP_size);
    LOG4CPLUS_INFO(log,
            LOG4CPLUS_TEXT("   Number of tasks: ") << app_properties.num_tasks);
    LOG4CPLUS_INFO(log,
            LOG4CPLUS_TEXT("   Sample period (us): ")
            << app_properties.sample_period);
    LOG4CPLUS_INFO(log,
            LOG4CPLUS_TEXT("   Number of parameters: ")
            << app_properties.param_count);
    LOG4CPLUS_INFO(log,
            LOG4CPLUS_TEXT("   Number of signals: ")
            << app_properties.signal_count);

    // Go through the list of events and set up a map path->id
    // of paths and their corresponding id's to watch
    const PdServ::Config eventList = m_config["events"];
    typedef std::map<std::string, PdServ::Config> EventMap;
    EventMap eventMap;
    PdServ::Config eventConf;
    size_t i;
    for (i = 0, eventConf = eventList[i];
            eventConf; eventConf = eventList[++i]) {

        std::string name = eventConf["name"].toString();
        eventMap[name] = eventConf;
        LOG4CPLUS_DEBUG(log,
                LOG4CPLUS_TEXT("Added event ")
                << LOG4CPLUS_STRING_TO_TSTRING(name));
    }

    // Get a copy of the parameters
    parameterBuf = new char[app_properties.rtP_size];
    if (::ioctl(fd, GET_PARAM, parameterBuf))
        goto out;

    char path[app_properties.variable_path_len+1];
    struct signal_info si;
    si.path = path;
    si.path_buf_len = app_properties.variable_path_len + 1;
    LOG4CPLUS_DEBUG_STR(log, LOG4CPLUS_TEXT("Adding parameters:"));
    for (size_t i = 0; i < app_properties.param_count; i++) {

        si.index = i;
        if (::ioctl(fd, GET_PARAM_INFO, &si) or !si.dim[0]) {
            LOG4CPLUS_WARN(log,
                    LOG4CPLUS_TEXT("Getting parameter properties for ")
                    << i << LOG4CPLUS_TEXT(" failed."));
            continue;
        }

        Parameter *p = new Parameter( this, parameterBuf + si.offset,
                SignalInfo(app_properties.name, &si));
        parameters.push_back(p);
        LOG4CPLUS_TRACE_STR(log, LOG4CPLUS_STRING_TO_TSTRING(p->path));
    }

    photoReady = new unsigned int[app_properties.rtB_count];
    std::fill_n(photoReady, app_properties.rtB_count, 0);
    photoCount = 0;

    shmem_len = app_properties.rtB_size * app_properties.rtB_count;
    shmem = ::mmap(0, shmem_len, PROT_READ, MAP_PRIVATE, fd, 0);
    if (shmem == MAP_FAILED)
        goto out;
    LOG4CPLUS_DEBUG(log,
            LOG4CPLUS_TEXT("Mapped shared memory segment with ") << shmem_len
            << LOG4CPLUS_TEXT(" and ") << app_properties.rtB_count
            << LOG4CPLUS_TEXT(" snapshots."));

    photoAlbum = reinterpret_cast<const char *>(shmem);

    mainTask = new Task(this, 1.0e-6 * app_properties.sample_period,
            photoReady, photoAlbum, &app_properties);
    LOG4CPLUS_DEBUG(log,
            LOG4CPLUS_TEXT("Added Task with sample time ")
            << mainTask->sampleTime);

    LOG4CPLUS_DEBUG_STR(log, LOG4CPLUS_TEXT("Adding signals:"));
    for (size_t i = 0; i < app_properties.signal_count; i++) {

        si.index = i;
        if (ioctl(fd, GET_SIGNAL_INFO, &si) or !si.dim[0]) {
            LOG4CPLUS_WARN(log,
                    LOG4CPLUS_TEXT("Getting signals properties for ")
                    << i << LOG4CPLUS_TEXT(" failed"));
            continue;
        }

        const Signal *s =
            mainTask->addSignal( SignalInfo(app_properties.name, &si));

        // Check whether this signal is actually an event
        EventMap::iterator it = eventMap.find(s->path);
        if (it != eventMap.end()
                and s->dtype.primary() == s->dtype.double_T) {

            Event *e = new Event(s, events.size(),
                    it->second.toUInt(), it->second["priority"].toString());
            events.push_back(e);
            eventCount += e->nelem;
            LOG4CPLUS_DEBUG(log,
                    LOG4CPLUS_TEXT("Watching as event: ")
                    << LOG4CPLUS_STRING_TO_TSTRING(s->path.c_str()));

            // Reset config to indicate that the path was found
            it->second = PdServ::Config();
        }

        LOG4CPLUS_TRACE_STR(log, LOG4CPLUS_STRING_TO_TSTRING(s->path));
    }

    // Report whether some events were not discovered
    for (EventMap::const_iterator it = eventMap.begin();
            it != eventMap.end(); ++it) {
        if (it->second)
            LOG4CPLUS_WARN(log,
                    LOG4CPLUS_TEXT("Event ")
                    << LOG4CPLUS_STRING_TO_TSTRING(it->first)
                    << LOG4CPLUS_TEXT(" is not found"));
    }

    eventQ = new EventQ(eventCount * 2, photoAlbum, &app_properties);

    return 0;

out:
    LOG4CPLUS_FATAL(log,
            LOG4CPLUS_TEXT("Failed: ")
            << LOG4CPLUS_C_STR_TO_TSTRING(strerror(errno)));
    ::exit(errno);
}

/////////////////////////////////////////////////////////////////////////////
void Main::getValue(
        const Signal *signal, void* dest, struct timespec *time) const
{
    const char *data = photoAlbum + readPointer * app_properties.rtB_size;

    std::copy(data + signal->offset,
            data + signal->offset + signal->memSize,
            reinterpret_cast<char*>(dest));

    if (time) {
        size_t statsOffset =
            app_properties.rtB_size
            - app_properties.num_tasks*sizeof(struct task_stats);
        const struct task_stats *task_stats =
            reinterpret_cast<const struct task_stats*>(data + statsOffset);

#ifdef HAVE_TIMESPEC
        *time = task_stats[0].time;
#else
        time->tv_sec = task_stats[0].time.tv_sec;
        time->tv_nsec = 1000*task_stats[0].time.tv_usec;
#endif
    }
}

/////////////////////////////////////////////////////////////////////////////
int Main::setValue(const PdServ::ProcessParameter* p,
        const char* buf, size_t offset, size_t count)
{
    const Parameter* parameter = static_cast<const Parameter*>(p);
    char* addr = parameter->valueBuf + offset;

    // Backup old values in case of write failure
    char backup[count];
    std::copy(addr, addr + count, backup);

    // Copy new data to shared memory
    std::copy(buf, buf + count, addr);

    struct param_change delta;
    delta.pos = addr - parameterBuf;
    delta.rtP = parameterBuf;
    delta.len = count;
    delta.count = 0;

    if (ioctl(fd, CHANGE_PARAM, &delta)) {
        // Write failure. Restore data
        std::copy(backup, backup + count, addr);
        return errno;
    }

    gettime(&parameter->mtime);

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
void Main::initializeParameter(PdServ::Parameter* p,
        const char* data, const struct timespec* mtime,
        const PdServ::Signal* signal)
{
    if (!data)
        return;

    Parameter *parameter = static_cast<Parameter*>(p);

    // Set modify time unconditionally
    parameter->mtime = *mtime;

    // Make sure parameter value is unset for signal/parameter pairs
    if (signal) {
        for (const char* p = parameter->valueBuf;
                p < parameter->valueBuf + parameter->memSize; ++p)
            if (*p)
                return;
    }

    std::copy(data, data + parameter->memSize, parameter->valueBuf);
    setValue(static_cast<PdServ::ProcessParameter*>(parameter),
            parameter->valueBuf, 0, parameter->memSize);
}

/////////////////////////////////////////////////////////////////////////////
bool Main::getPersistentSignalValue(const PdServ::Signal *s,
        char* buf, struct timespec* time)
{
    getValue(static_cast<const Signal*>(s), buf, time);
    return true;
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
PdServ::Config Main::config(const char* key) const
{
    return m_config[key];
}

/////////////////////////////////////////////////////////////////////////////
std::list<const PdServ::Task*> Main::getTasks() const
{
    std::list<const PdServ::Task*> taskList;
    taskList.push_back(mainTask);
    return taskList;
}

/////////////////////////////////////////////////////////////////////////////
std::list<const PdServ::Event*> Main::getEvents() const
{
    return std::list<const PdServ::Event*>(events.begin(), events.end());
}

/////////////////////////////////////////////////////////////////////////////
std::list<const PdServ::Parameter*> Main::getParameters() const
{
    return std::list<const PdServ::Parameter*>(
            parameters.begin(), parameters.end());
}

/////////////////////////////////////////////////////////////////////////////
void Main::prepare(PdServ::Session* /*session*/) const
{
}

/////////////////////////////////////////////////////////////////////////////
void Main::cleanup(const PdServ::Session* /*session*/) const
{
}
