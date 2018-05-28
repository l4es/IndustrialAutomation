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

#include <streambuf>
#include <cerrno>       // ENAMETOOLONG
#include <climits>      // HOST_NAME_MAX
#include <unistd.h>     // gethostname
#include <log4cplus/ndc.h>
#include <log4cplus/loggingmacros.h>

#include "config.h"

#include "../Debug.h"

#include "../Main.h"
#include "../Task.h"
#include "../Signal.h"
#include "../Parameter.h"
#include "../DataType.h"

#include "Session.h"
#include "Server.h"
#include "Event.h"
#include "Channel.h"
#include "Parameter.h"
#include "XmlElement.h"
#include "XmlParser.h"
#include "SubscriptionManager.h"

using namespace MsrProto;

/////////////////////////////////////////////////////////////////////////////
Session::Session( Server *server, ost::TCPSocket *socket):
    TCPSession(*socket),
    PdServ::Session(server->main, server->log), server(server),
    xmlstream(static_cast<PdServ::Session*>(this))
{
    inBytes = 0;
    outBytes = 0;

    timeTask = 0;

    std::list<const PdServ::Task*> taskList(main->getTasks());
    subscriptionManager.reserve(taskList.size());
    for (; taskList.size(); taskList.pop_front()) {
        const PdServ::Task *task = taskList.front();

        subscriptionManager.push_back(new SubscriptionManager(this, task));

        if (!timeTask or timeTask->task->sampleTime > task->sampleTime)
            timeTask = subscriptionManager.back();
    }

    // Do not throw on error
    ost::Socket::setError(false);

    // Setup some internal variables
    writeAccess = false;
    echoOn = false;     // FIXME: echoOn is not yet implemented
    quiet = false;
    polite = false;
    aicDelay = 0;

    xmlstream.imbue(std::locale::classic());

    detach();
}

/////////////////////////////////////////////////////////////////////////////
Session::~Session()
{
    server->sessionClosed(this);

    for (SubscriptionManagerVector::iterator it = subscriptionManager.begin();
            it != subscriptionManager.end(); ++it)
        delete *it;
}

/////////////////////////////////////////////////////////////////////////////
void Session::getSessionStatistics(PdServ::SessionStatistics &stats) const
{
    std::ostringstream os;
    if (remoteHostName.empty())
        stats.remote = peer();
    else
        stats.remote = remoteHostName + " (" + peer() +')';
    stats.client = client;
    stats.countIn = inBytes;
    stats.countOut = outBytes;
    stats.connectedTime = connectedTime;
}

/////////////////////////////////////////////////////////////////////////////
const struct timespec *Session::getTaskTime (const PdServ::Task* task) const
{
    return subscriptionManager[task->index]->taskTime;
}

/////////////////////////////////////////////////////////////////////////////
const PdServ::TaskStatistics *Session::getTaskStatistics (
        const PdServ::Task* task) const
{
    return subscriptionManager[task->index]->taskStatistics;
}

/////////////////////////////////////////////////////////////////////////////
void Session::broadcast(Session *, const struct timespec& ts,
        const std::string& action, const std::string &message)
{
    ost::MutexLock lock(mutex);

    if (!polite) {
        Broadcast *b = new Broadcast;
        b->ts = ts;
        b->action = action;
        b->message = message;
        broadcastList.push_back(b);
    }
}

/////////////////////////////////////////////////////////////////////////////
void Session::setAIC(const Parameter *p)
{
    ost::MutexLock lock(mutex);
    if (!polite)
        aic.insert(p->mainParam);

    if (!aicDelay)
        aicDelay = 5;  // 2Hz AIC
}

/////////////////////////////////////////////////////////////////////////////
void Session::parameterChanged(const Parameter *p)
{
    ost::MutexLock lock(mutex);
    if (!polite)
        changedParameter.insert(p);
}

/////////////////////////////////////////////////////////////////////////////
void Session::initial()
{
    TCPSession::initial();

    log4cplus::getNDC().push(LOG4CPLUS_STRING_TO_TSTRING(peer()));

    LOG4CPLUS_INFO_STR(server->log, LOG4CPLUS_TEXT("New session"));

    // Get the hostname
    char hostname[HOST_NAME_MAX+1];
    if (gethostname(hostname, HOST_NAME_MAX)) {
        if (errno == ENAMETOOLONG)
            hostname[HOST_NAME_MAX] = '\0';
        else
            strcpy(hostname,"unknown");
    }

    // Greet the new client
    {
        XmlElement greeting(createElement("connected"));
        XmlElement::Attribute(greeting, "name") << "MSR";
        XmlElement::Attribute(greeting, "host")
            << reinterpret_cast<const char*>(hostname);
        XmlElement::Attribute(greeting, "app") << main->name;
        XmlElement::Attribute(greeting, "appversion") << main->version;
        XmlElement::Attribute(greeting, "version") << MSR_VERSION;
        XmlElement::Attribute(greeting, "features") << MSR_FEATURES
#ifdef GNUTLS_FOUND
            ",tls"
#endif
            ;
        XmlElement::Attribute(greeting, "recievebufsize") << 100000000;
    }
}

/////////////////////////////////////////////////////////////////////////////
void Session::final()
{
    LOG4CPLUS_INFO_STR(server->log, LOG4CPLUS_TEXT("Finished session"));
    log4cplus::getNDC().remove();
}

/////////////////////////////////////////////////////////////////////////////
void Session::run()
{
    XmlParser parser(
            std::max(server->getMaxInputBufferSize() + 1024UL, 8192UL));

    while (*server->active) {
        if (!xmlstream.good()) {
            LOG4CPLUS_FATAL_STR(server->log,
                    LOG4CPLUS_TEXT("Error occurred in output stream"));
            return;
        }

        xmlstream.flush();

        if (isPending(pendingInput, 40)) {
            if (!parser.read(static_cast<PdServ::Session*>(this))) {
                if (PdServ::Session::eof())
                    return;

                if (parser.invalid()) {
                    LOG4CPLUS_FATAL_STR(server->log,
                            LOG4CPLUS_TEXT(
                                "Input buffer overflow in XML parser"));
                    return;
                }
            }
//                LOG4CPLUS_TRACE(server->log,
//                        LOG4CPLUS_TEXT("Rx: ")
//                        << LOG4CPLUS_STRING_TO_TSTRING(
//                            std::string(inbuf.bufptr(), n)));

            while (parser) {
                parser.getString("id", commandId);
                processCommand(&parser);

                if (!commandId.empty()) {
                    XmlElement ack(createElement("ack"));
                    XmlElement::Attribute(ack,"id")
                        .setEscaped(commandId);

                    commandId.clear();
                }
            }
        }

        // Collect all asynchronous events while holding mutex
        ParameterSet cp;
        BroadcastList broadcastList;
        {
            // Create an environment for mutex lock. This lock should be kept
            // as short as possible, and especially not when writing to the
            // output stream
            ost::MutexLock lock(mutex);

            if (aicDelay)
                --aicDelay;

            ParameterSet::iterator it2, it = changedParameter.begin();
            while (it != changedParameter.end()) {
                it2 = it++;
                if (!aicDelay or aic.find((*it2)->mainParam) == aic.end()) {
                    cp.insert(*it2);
                    changedParameter.erase(it2);
                }
            }

            std::swap(this->broadcastList, broadcastList);
        }

        // Write all asynchronous events to the client
        {
            for ( ParameterSet::iterator it = cp.begin();
                    it != cp.end(); ++it) {
                XmlElement pu(createElement("pu"));
                XmlElement::Attribute(pu, "index") << (*it)->index;
            }

            for ( BroadcastList::const_iterator it = broadcastList.begin();
                    it != broadcastList.end(); ++it) {

                XmlElement broadcast(createElement("broadcast"));

                XmlElement::Attribute(broadcast, "time") << (*it)->ts;

                if (!(*it)->action.empty())
                    XmlElement::Attribute(broadcast, "action")
                        .setEscaped((*it)->action);

                if (!(*it)->message.empty())
                    XmlElement::Attribute(broadcast, "text")
                        .setEscaped((*it)->message);

                delete *it;
            }
        }

        for (SubscriptionManagerVector::iterator it = subscriptionManager.begin();
                it != subscriptionManager.end(); ++it)
            (*it)->rxPdo(quiet);

        PdServ::EventData e;
        do {
            e = main->getNextEvent(this);
        } while (Event::toXml(this, e));
    }
}

/////////////////////////////////////////////////////////////////////////////
void Session::processCommand(const XmlParser* parser)
{
    const char *command = parser->tag();
    size_t commandLen = strlen(command);

    static const struct {
        size_t len;
        const char *name;
        void (Session::*func)(const XmlParser*);
    } cmds[] = {
        // First list most common commands
        { 4, "ping",                    &Session::ping                  },
        { 2, "rs",                      &Session::readStatistics        },
        { 2, "wp",                      &Session::writeParameter        },
        { 2, "rp",                      &Session::readParameter         },
        { 4, "xsad",                    &Session::xsad                  },
        { 4, "xsod",                    &Session::xsod                  },
        { 4, "echo",                    &Session::echo                  },

        // Now comes the rest
        { 2, "rc",                      &Session::readChannel           },
        { 2, "rk",                      &Session::readChannel           },
        { 3, "rpv",                     &Session::readParamValues       },
        { 4, "list",                    &Session::listDirectory         },
#ifdef GNUTLS_FOUND
        { 8, "starttls",                &Session::startTLS              },
#endif
        { 9, "broadcast",               &Session::broadcast             },
        {11, "remote_host",             &Session::remoteHost            },
        {12, "read_kanaele",            &Session::readChannel           },
        {12, "read_statics",            &Session::readStatistics        },
        {14, "read_parameter",          &Session::readParameter         },
        {15, "read_statistics",         &Session::readStatistics        },
        {15, "message_history",         &Session::messageHistory        },
        {15, "write_parameter",         &Session::writeParameter        },
        {17, "read_param_values",       &Session::readParamValues       },
        {0,  0,                         0},
    };

    // Go through the command list
    for (size_t idx = 0; cmds[idx].len; idx++) {
        // Check whether the lengths fit and the string matches
        if (commandLen == cmds[idx].len
                and !strcmp(cmds[idx].name, command)) {

            LOG4CPLUS_TRACE_STR(server->log,
                    LOG4CPLUS_C_STR_TO_TSTRING(cmds[idx].name));

            // Call the method
            (this->*cmds[idx].func)(parser);

            // Finished
            return;
        }
    }

    LOG4CPLUS_WARN(server->log,
            LOG4CPLUS_TEXT("Unknown command <")
            << LOG4CPLUS_C_STR_TO_TSTRING(command)
            << LOG4CPLUS_TEXT(">"));


    // Unknown command warning
    XmlElement warn(createElement("warn"));
    XmlElement::Attribute(warn, "num") << 1000;
    XmlElement::Attribute(warn, "text") << "unknown command";
    XmlElement::Attribute(warn, "command").setEscaped(command);
}

/////////////////////////////////////////////////////////////////////////////
void Session::broadcast(const XmlParser* parser)
{
    struct timespec ts;
    std::string action, text;

    main->gettime(&ts);
    parser->getString("action", action);
    parser->getString("text", text);

    server->broadcast(this, ts, action, text);
}

/////////////////////////////////////////////////////////////////////////////
void Session::echo(const XmlParser* parser)
{
    echoOn = parser->isTrue("value");
}

/////////////////////////////////////////////////////////////////////////////
void Session::ping(const XmlParser* /*parser*/)
{
    createElement("ping");
}

/////////////////////////////////////////////////////////////////////////////
void Session::readChannel(const XmlParser* parser)
{
    const Channel *c = 0;
    bool shortReply = parser->isTrue("short");
    std::string name;
    unsigned int index;

    if (parser->getString("name", name)) {
        c = server->find<Channel>(name);
        if (!c)
            return;
    }
    else if (parser->getUnsigned("index", index)) {
        c = server->getChannel(index);
        if (!c)
            return;
    }

    // A single signal was requested
    if (c) {
        char buf[c->signal->memSize];
        struct timespec time;
        int rv = static_cast<const PdServ::Variable*>(c->signal)
            ->getValue(this, buf, &time);

        XmlElement channel(createElement("channel"));
        c->setXmlAttributes(channel, shortReply, rv ? 0 : buf, 16, &time);

        return;
    }

    // A list of all channels
    const Server::Channels& chanList = server->getChannels();
    XmlElement channels(createElement("channels"));
    for (Server::Channels::const_iterator it = chanList.begin();
            it != chanList.end(); it++) {
        if ((*it)->hidden)
            continue;

        XmlElement el(channels.createChild("channel"));
        (*it)->setXmlAttributes( el, shortReply, 0, 16, 0);
    }
}

/////////////////////////////////////////////////////////////////////////////
void Session::listDirectory(const XmlParser* parser)
{
    const char *path;

    if (!parser->find("path", &path))
        return;

    XmlElement element(createElement("listing"));
    server->listDir(this, element, path);
}

/////////////////////////////////////////////////////////////////////////////
void Session::readParameter(const XmlParser* parser)
{
    bool shortReply = parser->isTrue("short");
    bool hex = parser->isTrue("hex");
    std::string name;
    unsigned int index;

    const Parameter *p = 0;
    if (parser->getString("name", name)) {
        p = server->find<Parameter>(name);
        if (!p)
            return;
    }
    else if (parser->getUnsigned("index", index)) {
        p = server->getParameter(index);
        if (!p)
            return;
    }

    if (p) {
        char buf[p->mainParam->memSize];
        struct timespec ts;

        p->mainParam->getValue(this, buf, &ts);

        std::string id;
        parser->getString("id", id);

        XmlElement xml(createElement("parameter"));
        p->setXmlAttributes(xml, buf, ts, shortReply, hex, 16);

        return;
    }

    XmlElement parametersElement(createElement("parameters"));

    const Server::Parameters& parameters = server->getParameters();
    Server::Parameters::const_iterator it = parameters.begin();
    while ( it != parameters.end()) {
        const PdServ::Parameter* mainParam = (*it)->mainParam;
        char buf[mainParam->memSize];
        struct timespec ts;

        if ((*it)->hidden) {
            ++it;
            continue;
        }

        mainParam->getValue(this, buf, &ts);

        while (it != parameters.end() and mainParam == (*it)->mainParam) {
            XmlElement xml(parametersElement.createChild("parameter"));
            (*it++)->setXmlAttributes(xml, buf, ts, shortReply, hex, 16);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
void Session::readParamValues(const XmlParser* /*parser*/)
{
    XmlElement param_values(createElement("param_values"));
    XmlElement::Attribute values(param_values, "value");

    const Server::Parameters& parameters = server->getParameters();
    Server::Parameters::const_iterator it = parameters.begin();
    while ( it != parameters.end()) {
        const PdServ::Parameter* mainParam = (*it)->mainParam;
        char buf[mainParam->memSize];
        struct timespec ts;

        mainParam->getValue(this, buf, &ts);

        if (it != parameters.begin())
            values << ';';
        values.csv(*it, buf, 1, 16);

        while (it != parameters.end() and mainParam == (*it)->mainParam)
            ++it;
    }
}

/////////////////////////////////////////////////////////////////////////////
void Session::messageHistory(const XmlParser* /*parser*/)
{
    std::list<PdServ::EventData> list(main->getEventHistory(this));

    while (!list.empty()) {
        Event::toXml(this, list.front());
        list.pop_front();
    }
}

/////////////////////////////////////////////////////////////////////////////
void Session::readStatistics(const XmlParser* /*parser*/)
{
    // <clients>
    //   <client index="0" name="lansim"
    //           apname="Persistent Manager, Version: 0.3.1"
    //           countin="19908501" countout="27337577"
    //           connectedtime="1282151176.659208"/>
    //   <client index="1" .../>
    // </clients>
    typedef std::list<PdServ::SessionStatistics> StatList;
    StatList stats;
    server->getSessionStatistics(stats);

    XmlElement clients(createElement("clients"));
    for (StatList::const_iterator it = stats.begin();
            it != stats.end(); it++) {
        XmlElement client(clients.createChild("client"));
        XmlElement::Attribute(client,"name")
            .setEscaped((*it).remote.size() ? (*it).remote : "unknown");
        XmlElement::Attribute(client,"apname")
            .setEscaped((*it).client.size() ? (*it).client : "unknown");
        XmlElement::Attribute(client,"countin") << (*it).countIn;
        XmlElement::Attribute(client,"countout") << (*it).countOut;
        XmlElement::Attribute(client,"connectedtime") << (*it).connectedTime;
    }
}

/////////////////////////////////////////////////////////////////////////////
void Session::startTLS(const XmlParser* /*parser*/)
{
    createElement("tls");
    xmlstream.flush();

    PdServ::Session::startTLS();
}

/////////////////////////////////////////////////////////////////////////////
void Session::remoteHost(const XmlParser* parser)
{
    parser->getString("name", remoteHostName);

    parser->getString("applicationname", client);

    if (parser->find("access"))
        writeAccess = parser->isEqual("access", "allow")
            or parser->isTrue("access");

    // Check whether stream should be polite, i.e. not send any data
    // when not requested by the client.
    // This is used for passive clients that do not check their streams
    // on a regular basis causing the TCP stream to congest.
    {
        ost::MutexLock lock(mutex);
        polite = parser->isTrue("polite");
        if (polite) {
            changedParameter.clear();
            aic.clear();
            while (!broadcastList.empty()) {
                delete broadcastList.front();
                broadcastList.pop_front();
            }
        }
    }

    LOG4CPLUS_INFO(server->log,
            LOG4CPLUS_TEXT("Logging in ")
            << LOG4CPLUS_STRING_TO_TSTRING(remoteHostName)
            << LOG4CPLUS_TEXT(" application ")
            << LOG4CPLUS_STRING_TO_TSTRING(client)
            << LOG4CPLUS_TEXT(" writeaccess=")
            << writeAccess);
}

/////////////////////////////////////////////////////////////////////////////
void Session::writeParameter(const XmlParser* parser)
{
    if (!writeAccess) {
        XmlElement warn(createElement("warn"));
        XmlElement::Attribute(warn, "text") << "No write access";
        return;
    }

    const Parameter *p = 0;

    unsigned int index;
    std::string name;
    if (parser->getString("name", name)) {
        p = server->find<Parameter>(name);
    }
    else if (parser->getUnsigned("index", index)) {
        p = server->getParameter(index);
    }

    if (!p)
        return;

    unsigned int startindex = 0;
    if (parser->getUnsigned("startindex", startindex)) {
        if (startindex >= p->dim.nelem)
            return;
    }

    if (parser->isTrue("aic"))
        server->setAic(p);

    int errnum;
    const char *s;
    if (parser->find("hexvalue", &s)) {
        errnum = p->setHexValue(this, s, startindex);
    }
    else if (parser->find("value", &s)) {
        errnum = p->setDoubleValue(this, s, startindex);
    }
    else
        return;

    if (errnum) {
        // If an error occurred, tell this client to reread the value
        parameterChanged(p);
    }
}

/////////////////////////////////////////////////////////////////////////////
void Session::xsad(const XmlParser* parser)
{
    unsigned int reduction, blocksize, precision, group;
    bool base64 = parser->isEqual("coding", "Base64");
    bool event = parser->isTrue("event");
    bool foundReduction = false;
    std::list<unsigned int> indexList;
    const Server::Channels& channel = server->getChannels();

    if (parser->isTrue("sync")) {
        for (SubscriptionManagerVector::iterator it = subscriptionManager.begin();
                it != subscriptionManager.end(); ++it)
            (*it)->sync();
        quiet = false;
    }
    else {
        // Quiet will stop all transmission of <data> tags until
        // sync is called
        quiet = parser->isTrue("quiet");
    }

    if (!parser->getUnsignedList("channels", indexList))
        return;

    if (parser->getUnsigned("reduction", reduction)) {
        if (!reduction) {
            XmlElement warn(createElement("warn"));
            XmlElement::Attribute(warn, "command") << "xsad";
            XmlElement::Attribute(warn, "text")
                << "specified reduction=0, choosing reduction=1";

            reduction = 1;
        }

        foundReduction = true;
    }

    // Discover blocksize
    if (event) {
        // blocksize of zero is for event channels
        blocksize = 0;
    }
    else if (parser->getUnsigned("blocksize", blocksize)) {
        if (!blocksize) {
            XmlElement warn(createElement("warn"));
            XmlElement::Attribute(warn, "command") << "xsad";
            XmlElement::Attribute(warn, "text")
                << "specified blocksize=0, choosing blocksize=1";

            blocksize = 1;
        }
    }
    else {
        // blocksize was not supplied, possibly human input
        blocksize = 1;
    }

    if (!parser->getUnsigned("precision", precision))
        precision = 16;

    if (!parser->getUnsigned("group", group))
        group = 0;

    for (std::list<unsigned int>::const_iterator it = indexList.begin();
            it != indexList.end(); it++) {
        if (*it >= channel.size())
            continue;

        const Channel *c = channel[*it];
        const PdServ::Signal *mainSignal = c->signal;

        if (event) {
            if (!foundReduction)
                // If user did not supply a reduction, limit to a
                // max of 10Hz automatically
                reduction = static_cast<unsigned>(
				0.1/mainSignal->sampleTime() + 0.5);
        }
        else if (!foundReduction) {
            // Quite possibly user input; choose reduction for 1Hz
            reduction = static_cast<unsigned>(
                    1.0/mainSignal->sampleTime() / blocksize + 0.5);
        }

        subscriptionManager[c->signal->task->index]->subscribe(
                c, group, reduction, blocksize, base64, precision);
    }
}

/////////////////////////////////////////////////////////////////////////////
void Session::xsod(const XmlParser* parser)
{
    std::list<unsigned int> intList;

    //cout << __LINE__ << "xsod: " << endl;

    if (parser->getUnsignedList("channels", intList)) {
        const Server::Channels& channel = server->getChannels();
        unsigned int group;

        if (!parser->getUnsigned("group", group))
            group = 0;

        for (std::list<unsigned int>::const_iterator it = intList.begin();
                it != intList.end(); it++) {
            if (*it < channel.size()) {
                size_t taskIdx = channel[*it]->signal->task->index;
                subscriptionManager[taskIdx]->unsubscribe(channel[*it], group);
            }
        }
    }
    else
        for (SubscriptionManagerVector::iterator it = subscriptionManager.begin();
                it != subscriptionManager.end(); ++it)
            (*it)->clear();
}

/////////////////////////////////////////////////////////////////////////////
XmlElement Session::createElement(const char* name)
{
    return XmlElement(name, xmlstream, 0, &commandId);
}

///////////////////////////////////////////////////////////////////////////
std::string Session::peer() const
{
    ost::tpport_t port;
    ost::IPV4Host peer = getPeer(&port);

    std::ostringstream os;
    os << peer << ':' << port;
    return os.str();
}

/////////////////////////////////////////////////////////////////////////////
ssize_t Session::read(void* buf, size_t count)
{
//    log_debug("%zu", count);
    if (!isPending(pendingInput, 0)) {
        log_debug("No data");
        return -EAGAIN;
    }

    ssize_t result = readData(buf, count);
    if (result < 0)
        result = getErrorNumber() == errInput ? -errno : -EAGAIN;

//    log_debug("result = %zi", result);
    return result;
}

/////////////////////////////////////////////////////////////////////////////
ssize_t Session::write(const void* buf, size_t count)
{
//    log_debug("%zu", count);
    ssize_t result = writeData(buf, count);
    if (result < 0)
        result = -errno;
//    log_debug("result = %zi", result);
    return result;
}
