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
#include "Debug.h"

#include <cerrno>
#include <cstdio>       // fread(), fileno()
#include <unistd.h>     // fork(), getpid(), chdir, sysconf, close, fstat()
                        // stat()
#include <signal.h>     // signal()
#include <limits.h>     // _POSIX_OPEN_MAX
#include <sys/types.h>  // umask(), fstat(), stat(), opendir()
#include <sys/stat.h>   // umask(), fstat(), stat()
#include <dirent.h>     // opendir(), readdir_r()
#include <iomanip>      // std::setw(), std::setfill()

#include <log4cplus/syslogappender.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/streams.h>
#include <log4cplus/configurator.h>
#include <log4cplus/loggingmacros.h>

#include "Main.h"
#include "Task.h"
#include "Signal.h"
#include "ProcessParameter.h"
#include "Database.h"
#include "Config.h"
#include "Event.h"
#include "Session.h"
//#include "etlproto/Server.h"
#include "msrproto/Server.h"

#ifndef _GNU_SOURCE
#    define _GNU_SOURCE
#endif
#include <string.h>     // basename()

#ifdef GNUTLS_FOUND
#include "TLS.h"
log4cplus::Logger tlsLogger;
#endif

using namespace PdServ;

/////////////////////////////////////////////////////////////////////////////
Main::Main(const std::string& name, const std::string& version):
    name(name), version(version),
    parameterLog(
            log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("parameter"))),
    eventLog(log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("event")))
{
    msrproto = 0;
}

/////////////////////////////////////////////////////////////////////////////
Main::~Main()
{
#ifdef GNUTLS_FOUND
    destroyTLS();
#endif
}

/////////////////////////////////////////////////////////////////////////////
void Main::setupLogging()
{
#ifdef PDS_DEBUG
    consoleLogging();
#endif

    unsigned int history = config("eventhistory").toUInt(100);
    eventList.resize(history);
    eventPtr = eventList.begin();

    if (config("logging")) {
        typedef std::basic_istringstream<log4cplus::tchar> tistringstream;
        tistringstream is(
                LOG4CPLUS_STRING_TO_TSTRING(config("logging").toString()));
        log4cplus::PropertyConfigurator(is).configure();
    }
    else {
        syslogLogging();
    }

    LOG4CPLUS_INFO(log4cplus::Logger::getRoot(),
            LOG4CPLUS_TEXT("Started application ")
            << LOG4CPLUS_STRING_TO_TSTRING(name)
            << LOG4CPLUS_TEXT(", Version ")
            << LOG4CPLUS_STRING_TO_TSTRING(version));
}

/////////////////////////////////////////////////////////////////////////////
void Main::consoleLogging()
{
    log4cplus::BasicConfigurator::doConfigure();
//    log4cplus::SharedAppenderPtr cerr(new log4cplus::ConsoleAppender(true));
//    cerr->setLayout(
//        std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout(
//                LOG4CPLUS_TEXT("%D %p %c %x: %m"))));
//
//    log4cplus::Logger::getRoot().addAppender(cerr);
}

/////////////////////////////////////////////////////////////////////////////
void Main::syslogLogging()
{
    log4cplus::helpers::Properties p;
    p.setProperty(LOG4CPLUS_TEXT("ident"),
            LOG4CPLUS_STRING_TO_TSTRING(name));
    p.setProperty(LOG4CPLUS_TEXT("facility"),LOG4CPLUS_TEXT("local0"));

    log4cplus::SharedAppenderPtr appender( new log4cplus::SysLogAppender(p));
    appender->setLayout(
            std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout(
                    LOG4CPLUS_TEXT("%-5p %c <%x>: %m"))));

    log4cplus::Logger root = log4cplus::Logger::getRoot();
    root.addAppender(appender);
    root.setLogLevel(log4cplus::INFO_LOG_LEVEL);
}

/////////////////////////////////////////////////////////////////////////////
int Main::gettime(struct timespec* t) const
{
    return localtime(t);
}

/////////////////////////////////////////////////////////////////////////////
int Main::localtime(struct timespec* t)
{
    struct timeval tv;

    if (gettimeofday(&tv, 0))
        return errno;
    t->tv_sec = tv.tv_sec;
    t->tv_nsec = tv.tv_usec * 1000;

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
void Main::startServers()
{
    log4cplus::Logger logger = log4cplus::Logger::getRoot();

#ifdef GNUTLS_FOUND
    if (setupTLS(config("tls"), logger)) {
        LOG4CPLUS_FATAL_STR(logger,
                LOG4CPLUS_TEXT("Fatal TLS error."));
        return;
    }
#endif

    LOG4CPLUS_INFO_STR(log4cplus::Logger::getRoot(),
            LOG4CPLUS_TEXT("Starting servers"));

    msrproto   = new   MsrProto::Server(this, config("msr"));

//    EtlProto::Server etlproto(this);
}

/////////////////////////////////////////////////////////////////////////////
void Main::stopServers()
{
    delete msrproto;

    savePersistent();

    LOG4CPLUS_INFO_STR(log4cplus::Logger::getRoot(),
            LOG4CPLUS_TEXT("Shut down servers"));
}

/////////////////////////////////////////////////////////////////////////////
int Main::setValue(const ProcessParameter* p, const Session* /*session*/,
        const char* buf, size_t offset, size_t count)
{
    // Ask the implementation to change value.
    int rv = setValue(p, buf, offset, count);
    if (rv)
        return rv;

    msrproto->parameterChanged(p, offset, count);

    PersistentMap::iterator it = persistentMap.find(p);
    bool persistent = it != persistentMap.end();
    bool log = parameterLog.isEnabledFor(log4cplus::INFO_LOG_LEVEL);
    std::string logString;

    // Setup logString when parameter has to be logged
    if ((persistent and persistentLogTraceOn) or log) {
        std::ostringstream os;
        os.imbue(std::locale::classic());
        os << p->path;

        os << " = ";

        static_cast<const ProcessParameter*>(p)->print(os, 0, p->memSize);

        logString = os.str();
    }

    // Save persistent parameter
    if (persistent) {
        char data[p->memSize];
        struct timespec time;

        p->copyValue(data, &time);

        PdServ::Database(persistentLog,
                persistentConfig["database"].toString())
            .save(p, data, &time);

        if (persistentLogTraceOn)
            LOG4CPLUS_INFO_STR(persistentLogTrace,
                    LOG4CPLUS_STRING_TO_TSTRING(logString));

        if (it->second) {
            LOG4CPLUS_WARN(persistentLog,
                    LOG4CPLUS_TEXT("Persistent parameter ")
                    << LOG4CPLUS_STRING_TO_TSTRING(p->path)
                    << LOG4CPLUS_TEXT(" is coupled to signal ")
                    << LOG4CPLUS_STRING_TO_TSTRING(it->second->path)
                    << LOG4CPLUS_TEXT(". Manually setting a parameter-signal"
                        " pair removes this coupling"));
            it->second = 0;
        }
    }

    // Log parameter change
    if (log) {
        parameterLog.forcedLog(log4cplus::INFO_LOG_LEVEL,
                LOG4CPLUS_STRING_TO_TSTRING(logString));
    }

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
unsigned int Main::setupPersistent()
{
    std::set<std::string> keys;

    persistentConfig = config("persistent");
    if (!persistentConfig)
        return 0;

    // Get variable list
    PdServ::Config varList(persistentConfig["variables"]);
    if (!varList[size_t(0)]) {
        LOG4CPLUS_INFO_STR(persistentLog,
                LOG4CPLUS_TEXT("Persistent variable list is empty"));
        return 0;
    }

    persistentLog = log4cplus::Logger::getRoot();
    persistentLogTraceOn = persistentConfig["trace"].toUInt();
    if (persistentLogTraceOn)
        persistentLogTrace =
            log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("persistent"));

    const std::string databasePath(persistentConfig["database"].toString());
    if (databasePath.empty()) {
        LOG4CPLUS_WARN_STR(persistentLog,
                LOG4CPLUS_TEXT("No persistent database path spedified"));
        return 0;
    }

    Database dataBase(persistentLog, databasePath);

    // Copy signals from tasks
    typedef std::map<std::string, const Signal*> SignalMap;
    SignalMap signalMap;

    typedef std::list<const Task*> TaskList;
    TaskList task = getTasks();
    for (TaskList::iterator it = task.begin();
            it != task.end(); ++it) {
        typedef std::list<const PdServ::Signal*> SignalList;

        SignalList signals =
            static_cast<const PdServ::Task*>(*it)->getSignals();

        for (SignalList::iterator it = signals.begin();
                it != signals.end(); ++it)
            signalMap[(*it)->path] = *it;
    }


    // Go through the variables section of the configuration to
    // find parameters to watch. There are 2 ways of specifying a variable
    // 1) string only pointing to a parameter to watch
    // 2) map with "parameter:" string pointing to a parameter to watch
    // 2) map with "parameter" and "signal" of a pair to watch
    // e.g.
    // variables:
    //     - "/path/to/parameter"
    //     - parameter: "/path/to/parameter"
    //     - parameter: /path/to/integrator/offset
    //       signal: "/path/to/integrator"
    for (size_t i = 0; varList[i]; ++i) {
        const PdServ::Config& item = varList[i];
        Parameter* param;
        const Signal* signal = 0;

        std::string path =
            (item.isMapping() ? item["parameter"] : item).toString();

        param = findParameter(path);
        if (!param) {
            // Parameter does not exist
            LOG4CPLUS_WARN(persistentLog,
                    LOG4CPLUS_TEXT("Persistent parameter ")
                    << LOG4CPLUS_STRING_TO_TSTRING(path)
                    << LOG4CPLUS_TEXT(" does not exist."));
            continue;
        }

        if (persistentMap.find(param) != persistentMap.end()) {
            LOG4CPLUS_INFO(persistentLog,
                    LOG4CPLUS_TEXT("Persistent parameter ")
                    << LOG4CPLUS_STRING_TO_TSTRING(path)
                    << LOG4CPLUS_TEXT(" traced already."));
            continue;
        }

        if (item.isMapping() and item["signal"]) {
            path = item["signal"].toString();

            if (!path.empty()) {
                // Parameter <-> Signal pair
                SignalMap::const_iterator srcIt(signalMap.find(path));

                if (srcIt == signalMap.end()) {
                    // Signal does not exist
                    LOG4CPLUS_WARN(persistentLog,
                            LOG4CPLUS_TEXT("Signal ")
                            << LOG4CPLUS_STRING_TO_TSTRING(path)
                            << LOG4CPLUS_TEXT(
                                " of persistent pair does not exist."));
                    continue;
                }
                signal = srcIt->second;
            }
        }

        if (signal) {
            // Check whether signal is compatable to parameter
            if (signal->dtype != param->dtype) {
                // Data types do not match
                LOG4CPLUS_WARN(persistentLog,
                        LOG4CPLUS_TEXT("Data type of signal ")
                        << LOG4CPLUS_STRING_TO_TSTRING(signal->path)
                        << LOG4CPLUS_TEXT(" and parameter ")
                        << LOG4CPLUS_STRING_TO_TSTRING(param->path)
                        << LOG4CPLUS_TEXT(" does not match"));
                continue;
            }
            else if (signal->dim != param->dim) {
                // Data dimensions do not match
                LOG4CPLUS_WARN(persistentLog,
                        LOG4CPLUS_TEXT("Data dimension of signal ")
                        << LOG4CPLUS_STRING_TO_TSTRING(signal->path)
                        << LOG4CPLUS_TEXT(" and parameter ")
                        << LOG4CPLUS_STRING_TO_TSTRING(param->path)
                        << LOG4CPLUS_TEXT(" does not match"));
                continue;
            }
        }

        persistentMap[param] = signal;
        keys.insert(param->path);
        if (signal)
            LOG4CPLUS_DEBUG(persistentLog,
                    LOG4CPLUS_TEXT("Added persistent parameter-signal pair: ")
                    << LOG4CPLUS_STRING_TO_TSTRING(signal->path)
                    << LOG4CPLUS_TEXT(" -> ")
                    << LOG4CPLUS_STRING_TO_TSTRING(param->path));
        else
            LOG4CPLUS_DEBUG(persistentLog,
                    LOG4CPLUS_TEXT("Added persistent parameter: ")
                    << LOG4CPLUS_STRING_TO_TSTRING(param->path));

        // Last but not least, read peristent value from database and
        // set parameter
        const struct timespec* mtime;
        const char* value;
        if (dataBase.read(param, &value, &mtime)) {
            initializeParameter(param, value, mtime, signal);
            if (persistentLogTraceOn) {
                std::ostringstream os;
                os.imbue(std::locale::classic());
                os << param->path;

                os << " = ";

                param->dtype.print(os, value, value, value + param->memSize);

                LOG4CPLUS_INFO_STR(persistentLogTrace,
                        LOG4CPLUS_STRING_TO_TSTRING(os.str()));
            }
        }
        else if (signal)
            initializeParameter(param, 0, 0, signal);
    }

    // Purge unneeded parameters from database, unless disabled by
    // configuration
    if (persistentConfig["cleanup"].toUInt(1))
        dataBase.checkKeys(keys);

    return persistentConfig["interval"].toUInt();
}

/////////////////////////////////////////////////////////////////////////////
void Main::savePersistent()
{
    if (persistentMap.empty())
        return;

    LOG4CPLUS_INFO_STR(persistentLog,
            LOG4CPLUS_TEXT("Saving persistent parameters"));

    // Open database and return if there are problems
    PdServ::Database db(persistentLog,
            persistentConfig["database"].toString());
    if (!db)
        return;

    // Only save signal/parameter pairs. Persistent paremters are saved as
    // they are changed
    for (PersistentMap::iterator it = persistentMap.begin();
            it != persistentMap.end(); ++it) {
        const Signal* signal = it->second;
        if (!signal)
            continue;

        const Parameter* param = it->first;
        char buf[param->memSize];
        struct timespec time;

        log_debug("Save %s", signal->path.c_str());
        if (getPersistentSignalValue(signal, buf, &time)) {
            db.save(param, buf, &time);

            if (persistentLogTraceOn) {
                std::ostringstream os;
                os.imbue(std::locale::classic());
                os << param->path;

                os << " = ";

                param->dtype.print(os, buf, buf, buf + param->memSize);

                LOG4CPLUS_INFO_STR(persistentLogTrace,
                        LOG4CPLUS_STRING_TO_TSTRING(os.str()));
            }

            LOG4CPLUS_TRACE(persistentLog,
                    LOG4CPLUS_TEXT("Saved persistent parameter: ")
                    << LOG4CPLUS_STRING_TO_TSTRING(param->path));
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
void Main::newEvent(Event* event,
        size_t index, bool state, const struct timespec* time)
{
    ost::WriteLock lock(eventMutex);

    if (state) {
        static const timespec zero = {0,0};

        event->setTime[index] = *time;
        event->resetTime[index] = zero;
    }
    else
        event->resetTime[index] = *time;

    eventPtr->event = event;
    eventPtr->index = index;
    eventPtr->state = state;
    eventPtr->time = *time;
    if (++eventPtr == eventList.end())
        eventPtr = eventList.begin();

    if (!state)
        return;

    log4cplus::LogLevel prio;
    switch (event->priority) {
        case PdServ::Event::Emergency:
        case PdServ::Event::Alert:
        case PdServ::Event::Critical:
            prio = log4cplus::FATAL_LOG_LEVEL;
            break;
        case PdServ::Event::Error:
            prio = log4cplus::ERROR_LOG_LEVEL;
            break;
        case PdServ::Event::Warning:
            prio = log4cplus::WARN_LOG_LEVEL;
            break;
        case PdServ::Event::Notice:
        case PdServ::Event::Info:
            prio = log4cplus::INFO_LOG_LEVEL;
            break;
        case PdServ::Event::Debug:
        default:
            prio = log4cplus::DEBUG_LOG_LEVEL;
            break;
    }

    log4cplus::tostringstream os;

    os << time->tv_sec << '.'
        << std::setw(6) << std::setfill('0') << time->tv_nsec/1000
        << std::setw(0)
        << LOG4CPLUS_TEXT(": ");

    os << LOG4CPLUS_STRING_TO_TSTRING(event->path);
    if (event->nelem > 1)
        os << '[' << index << ']';

    eventLog.log(prio, os.str());
}

/////////////////////////////////////////////////////////////////////////////
EventData Main::getNextEvent(Session* session) const
{
    ost::ReadLock lock(eventMutex);

    if (session->eventId >= eventList.size())
        session->eventId = eventPtr - eventList.begin();

    if (ssize_t(session->eventId) == eventPtr - eventList.begin()) {
        static const EventData d;
        return d;
    }

    const EventData& eventData = eventList[session->eventId];
    if (++session->eventId == eventList.size())
        session->eventId = 0;

    return eventData;
}

/////////////////////////////////////////////////////////////////////////////
std::list<EventData> Main::getEventHistory(Session* session) const
{
    ost::ReadLock lock(eventMutex);
    std::list<EventData> list;
    std::vector<EventData>::const_iterator it =
        eventPtr->event ? eventPtr : eventList.begin();

    if ( it >= eventPtr)
        list.insert(list.end(), it, eventList.end());

    list.insert(list.end(),
            eventList.begin(), eventList.begin() + session->eventId);

    return list;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#ifdef GNUTLS_FOUND
/////////////////////////////////////////////////////////////////////////////
int Main::setupTLS(Config tlsConf, log4cplus::Logger& logger)
{
    int result;

    // NULL pointers
    tls = 0;
    dh_params = 0;
    priority_cache = 0;
    blacklist = 0;
    tlsSessionDB = 0;
    verifyClient = false;

    if (!tlsConf)
        return 0;

    LOG4CPLUS_INFO_STR(logger,
            LOG4CPLUS_TEXT("Starting TLS configuration"));

    tlsSessionDB = new TlsSessionDB(&tls_mutex, TLS_DB_SIZE);
    blacklist = new Blacklist;

    // GnuTLS logging facility
    int logLevel = tlsConf["loglevel"].toUInt();
    if (logLevel > 0) {
        gnutls_global_set_log_level(logLevel);
        gnutls_global_set_log_function(gnutls_log_func);
        ::tlsLogger = logger;
    }

    result = gnutls_global_init();
    if (result) {
        LOG4CPLUS_FATAL(logger,
                LOG4CPLUS_TEXT("gnutls_global_init() failed: ")
                << LOG4CPLUS_C_STR_TO_TSTRING(gnutls_strerror(result))
                << LOG4CPLUS_TEXT(" (")
                << LOG4CPLUS_C_STR_TO_TSTRING(
                    gnutls_strerror_name(result))
                << LOG4CPLUS_TEXT(")"));
        return result;
    }

    result = gnutls_dh_params_init (&dh_params); // Allocate memory
    if (result) {
        LOG4CPLUS_FATAL(logger,
                LOG4CPLUS_TEXT("gnutls_dh_params_init() failed: ")
                << LOG4CPLUS_C_STR_TO_TSTRING(gnutls_strerror(result))
                << LOG4CPLUS_TEXT(" (")
                << LOG4CPLUS_C_STR_TO_TSTRING(
                    gnutls_strerror_name(result))
                << LOG4CPLUS_TEXT(")"));
        return result;
    }

    Config dh(tlsConf["dh"]);
    if (dh) {
        FILE* fd = fopen(dh.toString().c_str(), "r");
        struct stat sb;

        if (!fd or fstat(fileno(fd), &sb)) {
            const char* func = fd ? "fstat" : "fdopen";
            LOG4CPLUS_WARN(logger,
                    LOG4CPLUS_C_STR_TO_TSTRING(func)
                    << LOG4CPLUS_TEXT("(")
                    << LOG4CPLUS_STRING_TO_TSTRING(dh.toString())
                    << LOG4CPLUS_TEXT(") failed: ")
                    << LOG4CPLUS_C_STR_TO_TSTRING(strerror(errno))
                    << LOG4CPLUS_TEXT(" (")
                    << errno
                    << LOG4CPLUS_TEXT(")"));
            sb.st_size = 0;
        }
        else {
            unsigned char buf[sb.st_size];
            gnutls_datum_t params;
            params.size = fread(buf, 1, sb.st_size, fd);
            params.data = buf;

            result = gnutls_dh_params_import_pkcs3(
                    dh_params, &params, GNUTLS_X509_FMT_PEM);

            if (result) {
                LOG4CPLUS_FATAL(logger,
                        LOG4CPLUS_TEXT("gnutls_dh_params_import_pkcs3(")
                        << LOG4CPLUS_STRING_TO_TSTRING(dh.toString())
                        << LOG4CPLUS_TEXT(") failed: ")
                        << LOG4CPLUS_C_STR_TO_TSTRING(gnutls_strerror(result))
                        << LOG4CPLUS_TEXT(" (")
                        << LOG4CPLUS_C_STR_TO_TSTRING(
                            gnutls_strerror_name(result))
                        << LOG4CPLUS_TEXT(")"));
                return result;
            }
            else {
                LOG4CPLUS_INFO_STR(logger,
                        LOG4CPLUS_TEXT(
                            "Successfully loaded DH parameters from file"));
            }
        }

        if (fd)
            fclose(fd);
    }
    else {
        unsigned int bits(tlsConf["dh-bits"].toUInt(1024));
        result = gnutls_dh_params_generate2 (dh_params, bits);
        if (result) {
            LOG4CPLUS_FATAL(logger,
                    LOG4CPLUS_TEXT("gnutls_dh_params_generate2(")
                    << bits
                    << LOG4CPLUS_TEXT(") failed: ")
                    << LOG4CPLUS_C_STR_TO_TSTRING(gnutls_strerror(result))
                    << LOG4CPLUS_TEXT(" (")
                    << LOG4CPLUS_C_STR_TO_TSTRING(
                        gnutls_strerror_name(result))
                    << LOG4CPLUS_TEXT(")"));
            return result;
        }
        else {
            LOG4CPLUS_INFO(logger,
                    LOG4CPLUS_TEXT(
                        "Successfully generated DH parameters with ")
                    << bits
                    << LOG4CPLUS_TEXT(" bits"));
        }
    }

    // Set priority string
    std::string priority(tlsConf["priority"].toString("NORMAL"));
    const char* errpos = 0;
    result = gnutls_priority_init (&priority_cache, priority.c_str(), &errpos);
    if (result) {
        LOG4CPLUS_FATAL(logger,
                LOG4CPLUS_TEXT("gnutls_priority_init(")
                << LOG4CPLUS_STRING_TO_TSTRING(priority)
                << LOG4CPLUS_TEXT(") failed at position ")
                << (errpos - priority.c_str())
                << LOG4CPLUS_TEXT(": ")
                << LOG4CPLUS_C_STR_TO_TSTRING(gnutls_strerror(result))
                << LOG4CPLUS_TEXT(" (")
                << LOG4CPLUS_C_STR_TO_TSTRING(
                    gnutls_strerror_name(result))
                << LOG4CPLUS_TEXT(")"));
        return result;
    }
    else {
        LOG4CPLUS_INFO_STR(logger,
                LOG4CPLUS_TEXT("Successfully initialized priority string"));
    }

    result = gnutls_certificate_allocate_credentials(&tls);
    if (result) {
        LOG4CPLUS_FATAL(logger,
                LOG4CPLUS_TEXT(
                    "gnutls_certificate_allocate_credentials() failed: ")
                << LOG4CPLUS_C_STR_TO_TSTRING(gnutls_strerror(result))
                << LOG4CPLUS_TEXT(" (")
                << LOG4CPLUS_C_STR_TO_TSTRING(
                    gnutls_strerror_name(result))
                << LOG4CPLUS_TEXT(")"));
        return result;
    }

    gnutls_certificate_set_dh_params (tls, dh_params);

    std::string cert(tlsConf["cert"].toString());
    std::string key(tlsConf["key"].toString());
    result = gnutls_certificate_set_x509_key_file(
            tls, cert.c_str(), key.c_str(), GNUTLS_X509_FMT_PEM);
    if (result) {
        LOG4CPLUS_FATAL(logger,
                LOG4CPLUS_TEXT("gnutls_certificate_set_x509_key_file(")
                << LOG4CPLUS_TEXT("key=")
                << LOG4CPLUS_STRING_TO_TSTRING(key)
                << LOG4CPLUS_TEXT(", cert=")
                << LOG4CPLUS_STRING_TO_TSTRING(cert)
                << LOG4CPLUS_TEXT(") failed: ")
                << LOG4CPLUS_C_STR_TO_TSTRING(gnutls_strerror(result))
                << LOG4CPLUS_TEXT(" (")
                << LOG4CPLUS_C_STR_TO_TSTRING(
                    gnutls_strerror_name(result))
                << LOG4CPLUS_TEXT(")"));
        return result;
    }
    else {
        LOG4CPLUS_INFO_STR(logger,
                LOG4CPLUS_TEXT(
                    "Successfully loaded server key and certificate"));
    }

    parseCertConfigItem(logger, tlsConf["ca"],  &Main::loadTrustFile);
    parseCertConfigItem(logger, tlsConf["crl"], &Main::loadCrlFile);

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
void Main::destroyTLS()
{
    delete tlsSessionDB;
    delete blacklist;

    if (tls)
        gnutls_certificate_free_credentials (tls);
    if (dh_params)
        gnutls_dh_params_deinit (dh_params);
    if (priority_cache)
        gnutls_priority_deinit (priority_cache);
    gnutls_global_deinit();
}

/////////////////////////////////////////////////////////////////////////////
void Main::loadTrustFile(log4cplus::Logger& logger, const char* cafile)
{
    int result = gnutls_certificate_set_x509_trust_file(
            tls, cafile, GNUTLS_X509_FMT_PEM);
    if (result >= 0) {
        LOG4CPLUS_INFO(logger,
                LOG4CPLUS_TEXT("Successfully loaded ")
                << result
                << LOG4CPLUS_TEXT(" certificates from ")
                << LOG4CPLUS_C_STR_TO_TSTRING(cafile));
    }
    else {
        LOG4CPLUS_WARN(logger,
                LOG4CPLUS_TEXT("gnutls_certificate_set_x509_trust_file(")
                << LOG4CPLUS_STRING_TO_TSTRING(cafile)
                << LOG4CPLUS_TEXT(") failed: ")
                << LOG4CPLUS_C_STR_TO_TSTRING(gnutls_strerror(result))
                << LOG4CPLUS_TEXT(" (")
                << LOG4CPLUS_C_STR_TO_TSTRING(
                    gnutls_strerror_name(result))
                << LOG4CPLUS_TEXT(")"));
    }
}

/////////////////////////////////////////////////////////////////////////////
void Main::loadCrlFile(log4cplus::Logger& logger, const char* crlfile)
{
    struct stat sb;

    LOG4CPLUS_DEBUG(logger,
            LOG4CPLUS_TEXT("Considering ")
            << LOG4CPLUS_C_STR_TO_TSTRING(crlfile)
            << LOG4CPLUS_TEXT(" as CRL file"));

    if (stat(crlfile, &sb) or !S_ISREG(sb.st_mode) or sb.st_size == 0) {
        const char* name = ::basename(crlfile);
        if (name) {
            datum_string str(name);

            blacklist->insert(str);
            LOG4CPLUS_INFO(logger,
                    LOG4CPLUS_TEXT("Added Key ID ")
                    << LOG4CPLUS_STRING_TO_TSTRING(std::string(str))
                    << LOG4CPLUS_TEXT(" to blacklist"));
        }
    }
    else {
        int result = gnutls_certificate_set_x509_crl_file(
                tls, crlfile, GNUTLS_X509_FMT_PEM);
        if (result >= 0) {
            LOG4CPLUS_INFO(logger,
                    LOG4CPLUS_TEXT("Successfully loaded ")
                    << result
                    << LOG4CPLUS_TEXT(" revoked certificates from ")
                    << LOG4CPLUS_C_STR_TO_TSTRING(crlfile));
        }
        else {
            LOG4CPLUS_WARN(logger,
                    LOG4CPLUS_TEXT("gnutls_certificate_set_x509_crl_file(")
                    << LOG4CPLUS_STRING_TO_TSTRING(crlfile)
                    << LOG4CPLUS_TEXT(") failed: ")
                    << LOG4CPLUS_C_STR_TO_TSTRING(gnutls_strerror(result))
                    << LOG4CPLUS_TEXT(" (")
                    << LOG4CPLUS_C_STR_TO_TSTRING(
                        gnutls_strerror_name(result))
                    << LOG4CPLUS_TEXT(")"));
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
void Main::parseCertConfigDir(log4cplus::Logger& logger, const char* path,
        void (Main::*loadFunc)(log4cplus::Logger&, const char*))
{
    DIR* dir = opendir(path);
    struct dirent entry, *result;
    int rv;

    if (dir)
        while (!(rv = readdir_r(dir, &entry, &result)) and result) {
            if (entry.d_name[0] != '.') {
                std::string name(path);
                name.append(1,'/');
                name.append(entry.d_name);
                parseCertConfigDir(logger, name.c_str(), loadFunc);
            }
        }
    else
        (this->*loadFunc)(logger, path);
}

/////////////////////////////////////////////////////////////////////////////
void Main::parseCertConfigItem(log4cplus::Logger& logger, Config config,
        void (Main::*loadFunc)(log4cplus::Logger&, const char*))
{
    if (!config)
        return;

    verifyClient = true;
    gnutls_certificate_set_verify_function(tls, Session::gnutls_verify_client);

    size_t i = 0;
    while (Config item = config[i++])
        parseCertConfigDir(logger, item.toString().c_str(), loadFunc);
}

/////////////////////////////////////////////////////////////////////////////
void Main::initTlsSessionData(gnutls_session_t session,
        const Blacklist** blacklist) const
{
    gnutls_priority_set(session, priority_cache);
    gnutls_credentials_set(session, GNUTLS_CRD_CERTIFICATE, tls);
    if (verifyClient)
        gnutls_certificate_server_set_request(session, GNUTLS_CERT_REQUIRE);

    gnutls_db_set_retrieve_function (session, gnutls_db_retr_func);
    gnutls_db_set_remove_function (session, gnutls_db_remove_func);
    gnutls_db_set_store_function (session, gnutls_db_store_func);
    gnutls_db_set_ptr (session, tlsSessionDB);

    *blacklist = this->blacklist;
}

/////////////////////////////////////////////////////////////////////////////
int Main::gnutls_db_store_func(
        void *ptr, gnutls_datum_t key, gnutls_datum_t data)
{
    return reinterpret_cast<TlsSessionDB*>(ptr)->store(key, data);
}

/////////////////////////////////////////////////////////////////////////////
int Main::gnutls_db_remove_func(void *ptr, gnutls_datum_t key)
{
    return reinterpret_cast<TlsSessionDB*>(ptr)->erase(key);
}

/////////////////////////////////////////////////////////////////////////////
gnutls_datum_t Main::gnutls_db_retr_func(void *ptr, gnutls_datum_t key)
{
    return reinterpret_cast<TlsSessionDB*>(ptr)->retrieve(key);
}

void Main::gnutls_log_func(int /*prio*/, const char* err)
{
    tlsLogger.forcedLog(log4cplus::INFO_LOG_LEVEL,
            LOG4CPLUS_C_STR_TO_TSTRING(err));
}
#endif

