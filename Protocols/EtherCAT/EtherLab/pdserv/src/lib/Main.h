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

#ifndef LIB_MAIN_H
#define LIB_MAIN_H

#include <set>
#include <list>
#include <cc++/thread.h>

#include "../Main.h"

struct EventData;

namespace PdServ {
    class Signal;
    class DataType;
}

class Parameter;
class Session;
class Signal;
class Event;
class Task;

class Main: public PdServ::Main, public ost::Thread {
    public:
        Main( const char *name, const char *version,
                int (*gettime)(struct timespec*));
        ~Main();

        void setConfigFile(const char *file);
        int setup();

        Task* addTask(double sampleTime, const char *name);

        Event* addEvent(const char *path, int prio, size_t n);
        void setEvent(Event* event,
                size_t element, bool state, const timespec* t);

        Parameter* addParameter( const char *path,
                unsigned int mode, const PdServ::DataType& datatype,
                void *addr, size_t n, const size_t *dim);

        int getValue(const Signal* s, void* dst, struct timespec* time);

        static const double bufferTime;

    private:
        typedef std::list<Task*> TaskList;
        TaskList task;

        ost::Mutex sdoMutex;
        int ipcRx;
        int ipcTx;
        int terminatePipe;
        bool ipc_error;

        int pid;
        std::string configFile;
        PdServ::Config m_config;

        size_t shmem_len;
        void *shmem;

        char* signalData;

        /* Structure where event changes are written to in shmem */
        struct ::EventData **eventDataWp;   // Pointer to next write location
        struct ::EventData *eventDataStart; // First valid block
        struct ::EventData *eventDataEnd;   // Last valid block
        ost::Mutex eventMutex;

        char *parameterData;

        int (* const rttime)(struct timespec*);

        typedef std::list<Event*> EventList;
        EventList events;

        typedef std::list<Parameter*> ParameterList;
        ParameterList parameters;

        int readConfiguration();

        // Reimplemented from PdServ::Main
        int prefork_init();
        int postfork_rt_setup();
        int postfork_nrt_setup();
        int gettime(struct timespec *) const;
        std::list<const PdServ::Task*> getTasks() const;
        std::list<const PdServ::Event*> getEvents() const;
        std::list<const PdServ::Parameter*> getParameters() const;
        void prepare(PdServ::Session *session) const;
        void cleanup(const PdServ::Session *session) const;
        const PdServ::Event *getNextEvent(const PdServ::Session* session,
                size_t *index, bool *state, struct timespec *t) const;
        void initializeParameter(PdServ::Parameter* p,
                const char* data, const struct timespec* mtime,
                const PdServ::Signal* s);
        bool getPersistentSignalValue(const PdServ::Signal *s,
                char* buf, struct timespec* time);
        PdServ::Parameter* findParameter(const std::string& path) const;
        PdServ::Config config(const char*) const;
        int setValue(const PdServ::ProcessParameter* p,
                const char* buf, size_t offset, size_t count);

        // Reimplemented from ost::Thread
        void run();
        void final();
};

#endif // LIB_MAIN_H
