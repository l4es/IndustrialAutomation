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

#ifndef MAIN_H
#define MAIN_H

#include <map>
#include <list>
#include <string>
#include <vector>
#include <cc++/thread.h>
#include <log4cplus/logger.h>

#include "Config.h"
#include "Event.h"
#include "config.h"

#ifdef GNUTLS_FOUND
#include <gnutls/gnutls.h>
#endif

struct timespec;
class TlsSessionDB;

namespace MsrProto {
    class Server;
}

class Blacklist;
class TlsSessionDB;

namespace PdServ {

class Signal;
class Event;
class Parameter;
class ProcessParameter;
class Variable;
class Task;
class Session;
class SessionStatistics;

class Main {
    public:
        Main(const std::string& name, const std::string& version);
        virtual ~Main();

        const std::string name;         /**< Application name */
        const std::string version;      /**< Application version */

        void startServers();
        void stopServers();

#ifdef GNUTLS_FOUND
        void initTlsSessionData(gnutls_session_t,
                const Blacklist** blacklist) const;
#endif

        // Get the current system time.
        // Reimplement this method in the class specialization
        virtual int gettime(struct timespec *) const;

        virtual std::list<const Task*> getTasks() const = 0;
        virtual std::list<const Event*> getEvents() const = 0;
        virtual std::list<const Parameter*> getParameters() const = 0;
        virtual void prepare(Session *session) const = 0;
        virtual void cleanup(const Session *session) const = 0;

        EventData getNextEvent(Session* session) const;
        std::list<EventData> getEventHistory(Session* session) const;

        // Setting a parameter has various steps:
        // 1) client calls parameter->setValue(session, ...)
        //    This virtual method is implemented by ProcessParameter
        // 2) ProcessParameter calls
        //        main->setValue(processParameter, session, ...)
        //    so that main can check whether session is allowed to set it
        // 3) main calls Main::setValue(...) virtual method to do the setting
        // 4) on success, main can do various functions, e.g.
        //      - inform clients of a value change
        //      - save persistent parameter
        //      - etc
        int setValue(const ProcessParameter* p, const Session *session,
                const char* buf, size_t offset, size_t count);

    protected:
        void setupLogging();

        static int localtime(struct timespec *);

        void savePersistent();
        unsigned int setupPersistent();

        void newEvent(Event* event,
                size_t element, bool state, const struct timespec* time);

        // virtual functions to be implemented in derived classes
        virtual void initializeParameter(Parameter* p,
                const char* data, const struct timespec* mtime,
                const Signal* s) = 0;
        virtual bool getPersistentSignalValue(const Signal *s,
                char* buf, struct timespec* time) = 0;
        virtual Config config(const char* section) const = 0;
        virtual Parameter* findParameter(const std::string& path) const = 0;
        virtual int setValue(const ProcessParameter* p,
                const char* buf, size_t offset, size_t count) = 0;

    private:
        std::vector<EventData> eventList;
        std::vector<EventData>::iterator eventPtr;
        mutable ost::ThreadLock eventMutex;

        const log4cplus::Logger parameterLog;
        const log4cplus::Logger eventLog;

        bool persistentLogTraceOn;
        log4cplus::Logger persistentLog;
        log4cplus::Logger persistentLogTrace;
        PdServ::Config persistentConfig;
        typedef std::map<const Parameter*, const Signal*> PersistentMap;
        PersistentMap persistentMap;

        MsrProto::Server *msrproto;
//        EtlProto::Server etlproto(this);

        void consoleLogging();
        void syslogLogging();

#ifdef GNUTLS_FOUND
        ost::Mutex tls_mutex;
        bool verifyClient;
        gnutls_certificate_credentials_t tls;
        gnutls_priority_t priority_cache;
        gnutls_dh_params_t dh_params;

        TlsSessionDB *tlsSessionDB;
        Blacklist* blacklist;

        int setupTLS(Config, log4cplus::Logger&);
        void destroyTLS();
        void loadTrustFile(log4cplus::Logger&, const char* cafile);
        void loadCrlFile(log4cplus::Logger&, const char* cafile);
        void parseCertConfigDir(log4cplus::Logger&, const char* path,
                void (Main::*func)(log4cplus::Logger&, const char*));
        void parseCertConfigItem(log4cplus::Logger&, Config config,
                void (Main::*func)(log4cplus::Logger&, const char*));

        static int gnutls_db_store_func(
                void *, gnutls_datum_t key, gnutls_datum_t data);
        static int gnutls_db_remove_func(void *, gnutls_datum_t key);
        static gnutls_datum_t gnutls_db_retr_func(void *, gnutls_datum_t key);
        static void gnutls_log_func(int, const char*);
#endif
};

}
#endif // MAIN_H
