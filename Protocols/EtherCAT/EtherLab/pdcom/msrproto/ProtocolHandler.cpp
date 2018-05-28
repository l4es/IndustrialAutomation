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

#include "ProtocolHandler.h"

#include "../pdcom/Process.h"
#include "../pdcom/Subscriber.h"
#include "../Debug.h"
#include "../SpyLayer.h"
#include "Channel.h"
#include "Parameter.h"
#include "DataGroup.h"
#include "DirNode.h"
#include "Request.h"

#include <cstring>
#include <typeinfo>
#include <ostream>
#include <stdexcept>
#include <errno.h>

#ifdef PDC_DEBUG
#    include <stdlib.h>     // getenv(), atoi()
#endif

struct Attribute
{/*{{{*/
    Attribute(const char **atts): atts(atts) {
    }

    const char* find(const char* arg) {
        for (size_t i = 0; arg and atts[i]; i+= 2) {
            if (!strcmp(atts[i], arg))
                return atts[i+1];
        }

        return nullStr;
    }

    bool getString(const char* arg, const char*& val) {
        const char* attr = find(arg);

        if (attr == nullStr)
            return false;

        val = attr;
        return true;
    };

    bool getUInt(const char* arg, unsigned int& val) {
        const char* attr = find(arg);

        // Only test for numbers, even leading '+' is ignored
        if (*attr < '0' or *attr > '9')
            return false;

        val = atol(attr);
        return true;
    }

    bool getInt(const char* arg, int& val) {
        const char* attr = find(arg);

        // Only test for numbers, even leading '+' is ignored
        if ((*attr < '0' or *attr > '9') and *attr != '-')
            return false;

        val = atoi(attr);
        return true;
    }

    bool isTrue(const char* arg) {
        unsigned int val;
        return getUInt(arg, val) and val;
    }

    bool getDouble(const char* arg, double& val) {
        const char* attr = find(arg);

        if (*attr)
            val = atof(attr);

        return *attr;
    }

    uint64_t getTime_ns(const char* arg) {
        const char* attr = find(arg);

        uint64_t time_ns = 0;
        size_t i = 0;
        do {
            if (!*attr or (!i and *attr == '.')) {
                if (!i)
                    i = 10;
            }
            else if (*attr < '0' or *attr > '9')
                return false;

            if (*attr != '.') {
                if (*attr or i != 10)
                    time_ns *= 10;

                if (*attr)
                    time_ns += *attr - '0';
            }

            if (*attr)
                ++attr;

        } while (*attr == '.' or !i or --i);

        return time_ns;
    }

    static const char* const nullStr;

    const char **atts;
};/*}}}*/

const char* const Attribute::nullStr = "";

struct XmlCommand
{/*{{{*/
    struct Attribute {
        // Create an attribute for a command. 
        Attribute(XmlCommand& command, const char *name): cout(command.cout) {
            // send the preamble
            cout << ' ' << name << "=\"";
        }
        ~Attribute() {
            // Postamble
            cout << '"';
        }

        std::ostream& cout;
    };

    XmlCommand(std::ostream& cout, const char *command):
        cout(cout), p_flush(true) {
        cout << '<' << command;
    }

    ~XmlCommand() {
        cout << ">\r\n";
        if (p_flush)
            cout << std::flush;
    }

    XmlCommand& addEscapedAttribute(
            const char *name, const std::string& str) {
        size_t len = str.size();
        size_t ptr = 0, idx = 0;
        char c;

        cout << ' ' << name << '=' << '"';

        while (idx < len) {
            c = str[idx++];
            if (c == '\\' or c == '"') {
                cout << str.substr(ptr, idx - ptr - 1) << '\\' << c;
                ptr = idx;
            }
        }

        cout << str.substr(ptr) << '"';

        return *this;
    }

    template <typename T>
        XmlCommand& addAttribute(
                const char *name, T const& val, bool active = true) {
            if (active)
                cout << ' ' << name << "=\"" << val << '"';
            return *this;
        }

    XmlCommand& noFlush() {
        p_flush = false;
        return *this;
    }

//    XmlCommand& setId(const char* prefix, int id) {
//        cout << " id=\"";
//        if (prefix)
//            cout << prefix;
//        cout << id << '"';
//
//        return *this;
//    }

    XmlCommand& setId(const char* id) {
        if (id and *id)
            cout << " id=\"" << id << '"';

        return *this;
    }

    std::ostream& cout;
    bool p_flush;
};/*}}}*/

/****************************************************************************
 * Wrapper to get from static function calls to class method calls
 */
void msr::ProtocolHandler::ExpatStartElement(void *data,
        const XML_Char *name, const XML_Char **atts)
{/*{{{*/
    reinterpret_cast<ProtocolHandler*>(data)->startElement(name, atts);
}/*}}}*/

/****************************************************************************
 * Wrapper to get from static function calls to class method calls
 */
void msr::ProtocolHandler::ExpatEndElement(void *data, const XML_Char *name)
{/*{{{*/
    reinterpret_cast<ProtocolHandler*>(data)->endElement(name);
}/*}}}*/

/****************************************************************************
 * Wrapper to get from static function calls to class method calls
 */
void msr::ProtocolHandler::ExpatCharacterData(
        void *data, const XML_Char *s, int len)
{/*{{{*/
    reinterpret_cast<ProtocolHandler*>(data)->characterData(s, len);
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
msr::ProtocolHandler::ProtocolHandler(PdCom::Process *process, IOLayer* io) :
    ::ProtocolHandler(process),
    ::StreambufLayer(io),
    cout(this),
    root(new RootNode)
{/*{{{*/

#if PDC_DEBUG
    {
        char* spy = getenv("SPY");
        if (spy and atoi(spy))
            insert(new SpyLayer(io));
    }
#endif

    state = StartUp;
    level = 0;
    tls = false;
    polite = false;
    protocolError = false;
    fullListing = Uncached;

    if (!(xmlParser = XML_ParserCreate("UTF-8")))
        throw std::runtime_error("Could not create XML parser");

    XML_SetUserData(xmlParser, this);
    XML_SetElementHandler(xmlParser, ExpatStartElement, ExpatEndElement);
    XML_SetCharacterDataHandler(xmlParser, ExpatCharacterData);

    parse("<xml>", 5);
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
msr::ProtocolHandler::~ProtocolHandler()
{/*{{{*/
    XML_ParserFree(xmlParser);

    log_debug("killed %p", this);
    // Directory handler deletes variables, so don't do this here!

//    setEOF();

    while (!requestSet.empty()) {
        Request* request = *requestSet.begin();

        process->protocolLog(
                PdCom::Process::Warn,
                std::string("Variable ")
                .append(request->subscription
                    ? request->subscription->variable->path()
                    : request->path)
                .append(" was not unsubscribed before object deletion."));

        if (request->subscription)
            unsubscribe(request->subscription);
        else
            delete request;
    }

    for (size_t i = 0; i < dataGroup.size(); ++i)
        delete dataGroup[i];

    delete root;
    log_debug("requestSet=%zu", requestSet.size());
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
size_t msr::ProtocolHandler::parse(const char *buf, size_t n)
{/*{{{*/
    if (XML_STATUS_OK != XML_Parse(xmlParser, buf, n, 0)) {
        if (state != StartUp and state != WaitForConnected)
            process->protocolLog(PdCom::Process::Emergency,
                    std::string("Fatal XML parsing error: ")
                    + XML_ErrorString(XML_GetErrorCode(xmlParser)));

        return 0;
    }

    return n;
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
::ProtocolHandler* msr::ProtocolHandler::create(
        PdCom::Process* process, IOLayer* io, const std::string& data)
{/*{{{*/
    msr::ProtocolHandler* h = new msr::ProtocolHandler(process, io);
    if (h->parse(data.c_str(), data.size()) == data.size())
        return h;

    delete h;
    return 0;
}/*}}}*/

#if PDC_DEBUG
const char *statename[] = {
    "StartUp",
    "Idle",
    "GetListing",
    "WaitForConnected",
};
#endif

///////////////////////////////////////////////////////////////////////////
void msr::ProtocolHandler::startElement(
        const XML_Char *name, const XML_Char **atts)
{/*{{{*/
    const char *id;
//    bool chan, param;
//    log_debug("%s s=%s l=%i", name, statename[state], level);

    ++level;

    switch (state) {
        case StartUp:
            if (!strcmp(name,"xml") and level == 1)
                state = WaitForConnected;
            break;

        case WaitForConnected:
            if (!strcmp(name,"connected") and level == 2) {
                Attribute a(atts);
                feature = a.find("features");
                m_name = a.find("app");
                m_version = a.find("appversion");

                // Send login if mandatory
                if (feature.login == 2)
                    login(0,0);

                XmlCommand(cout, "remote_host")
                    //.addAttribute("name", data)
                    .addAttribute("access", "allow")
                    .addEscapedAttribute("applicationname",
                            process->applicationName())
                    .addEscapedAttribute("hostname", process->hostname())
                    .setId("init");

                state = Idle;
            }
            break;

        case Idle:
            if (level != 2) {
                process->protocolLog(PdCom::Process::Warn,
                        std::string("Unknown XML tag in Idle: ") + name);
                break;
            }

            // find() returns "" if id is not found
            id = Attribute(atts).find("id");

            if (!strcmp(name,"data")) {
                Attribute a(atts);
                unsigned int dataGroupId;

                dataTime = a.getTime_ns("time");

                if (!a.getUInt("group", dataGroupId))
                    dataGroupId = 0;
                
                currentDataGroup = 0;
                if (!dataGroupId) {
                    // Attribute "group" not found. Event transmission
                    state = ReadEvent;
                }
                else if (dataGroupId <= dataGroup.size()) {
                    currentDataGroup = dataGroup[dataGroupId-1];

                    // dataGroup vector may have gaps with a zero pointer.
                    // Test currentDataGroup before continuing
                    if (currentDataGroup) {
                        state = ReadData;
                        currentDataGroup->setTime(dataTime);
                    }
                }
            }
            else if (!strcmp(name,"pu")) {
                unsigned int index;

                if (Attribute(atts).getUInt("index", index)) {
                    ParameterMap::iterator it = parameter.find(index);

                    if (!parameterMonitorSet.empty())
                        pollParameter(index, it == parameter.end(), true);
                    else if (it != parameter.end() and it->second->update())
                        pollParameter(index, it == parameter.end(), false);
                }
            }
            else if (!strcmp(name,"parameter")) {
                Parameter *p = getParameter(atts);
                if (p) {
                    Attribute a(atts);
                    const char* value = a.find("hexvalue");

                    if (!strcmp(id, "pendingParameter"))
                        pendingAck(p);

                    if (*value)
                        p->newValue(a.getTime_ns("mtime"), value,
                                (!strcmp(id,"pm")
                                 and !parameterMonitorSet.empty()
                                 ? &parameterMonitorSet : 0));
                }
            }
            else if (!strcmp(name,"channel")) {
                Channel* c = getChannel(atts);
                if (c) {
                    if (!strcmp(id, "poll")) {
                        Attribute a(atts);
                        c->pollData(a.getTime_ns("time"), a.find("value"));
                    }
                    else if (!strcmp(id, "pendingChannel"))
                        pendingAck(c);
                }
            }
            else if (!strcmp(name,"ack")) {
                // Check which ack was called
                if (!strcmp(id, "xsadQ"))
                    xsadAck();
                else if (!strcmp(id, "eventQ"))
                    eventAck();
                else if (!strcmp(id, "pendingChannel"))
                    pendingAck(0);
                else if (!strcmp(id, "findQ"))
                    processFindRequest();
                else if (!strcmp(id, "listQ")) {
                    fullListing = Cached;

                    // Process all pending list and find requests
                    while (processListRequest());
                    while (processFindRequest());
                }
                else if (!strcmp(id, "init"))
                    process->connected();
            }
            else if (!strcmp(name,"listing")
                    or !strcmp(name, "channels")
                    or !strcmp(name, "parameters")) {
                state = GetListing;
            }
            else if (!strcmp(name,"saslauth"))
                processLogin(atts);
            else if (!strcmp(name,"crit_error"))
                processLogMessage(PdCom::Process::Critical, atts);
            else if (!strcmp(name,"error"))
                processLogMessage(PdCom::Process::Error, atts);
            else if (!strcmp(name,"warn"))
                processLogMessage(PdCom::Process::Warn, atts);
            else if (!strcmp(name,"info"))
                processLogMessage(PdCom::Process::Info, atts);
            else if (!strcmp(name,"ping"))
                process->pingReply();
            else if (!strcmp(name,"tls"))
                tls = true;
            break;

        case GetListing:
            if (level != 3) {
                process->protocolLog(PdCom::Process::Warn,
                        std::string("Unknown XML tag in GetListing: ") + name);
                break;
            }

            if (!strcmp(name,"dir")) {
                const char *path;
                if (Attribute(atts).getString("path", path)
                        and !root->find(path)) {
                    DirNode *node = new DirNode;
                    root->insert(node, path);
                    log_debug("Create dir %s", node->path().c_str());
                }
            }
            else if (!strcmp(name,"channel"))
                getChannel(atts);
            else if (!strcmp(name,"parameter"))
                getParameter(atts);
            break;

        case ReadData:
            if (level != 3) {
                process->protocolLog(PdCom::Process::Warn,
                        std::string("Unknown XML tag in ReadData: ") + name);
                break;
            }

            if (name[0] == 'F') {
                Attribute a(atts);
                unsigned int index;
                if (a.getUInt("c", index)) {
                    ChannelMap::iterator it = channel.find(index);
                    if (it != channel.end())
                        protocolError |= currentDataGroup->newValue(
                                it->second, a.find("d"));
                }
            }
            else if (!strcmp(name, "time")) {
                protocolError |= currentDataGroup->setTimeVector(
                        Attribute(atts).find("d"));
            }

            break;

        case ReadEvent:
            if (level != 3) {
                process->protocolLog(PdCom::Process::Warn,
                        std::string("Unknown XML tag in ReadEvent: ") + name);
                break;
            }

            if (name[0] == 'E') {
                log_debug("Event data");
                Attribute a(atts);
                unsigned int index;
                if (a.getUInt("c", index)) {
                    ChannelMap::iterator it = channel.find(index);
                    if (it != channel.end())
                        protocolError |=
                            it->second->eventData(dataTime, a.find("d"));
                }
            }

            break;

        default:
            break;
    };
//    log_debug("%s s=%s l=%i", name, statename[state], level);
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
void msr::ProtocolHandler::endElement(const XML_Char *name)
{/*{{{*/
//    log_debug("%s s=%s l=%i", name, statename[state], level);
    switch (state) {
        case GetListing:
            if (level == 2) {
                state = Idle;

                // name is either
                //      - channels
                //      - parameters
                //      - listing
                if (!strcmp(name,"listing"))
                    processListRequest();
            }

            break;

        case ReadData:
            if (level == 2 and !strcmp(name,"data")) {
                currentDataGroup->process();
                state = Idle;
            }
            break;

        case ReadEvent:
            if (level == 2 and !strcmp(name,"data")) {
                state = Idle;
            }
            break;

        default:
            break;
    }
    --level;

//    log_debug("%s s=%s l=%i", name, statename[state], level);
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
void msr::ProtocolHandler::characterData(const XML_Char *s, int len)
{/*{{{*/
    if (tls and state == Idle and level == 1) {
        while (len--)
            if (*s++ == '\n') {
//                log_debug("tls found");
                process->startTLSReply();
                tls = false;
            }
    }
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
// Arguments:
//     var: if var == 0, this is a test to check whether path was found
//          if var != 0, found the variable --> subscribe it
void msr::ProtocolHandler::pendingAck(MsrVariable *var)
{/*{{{*/
//    if (pendingQ.empty()) {
//        protocolError = 1;
//        // FIXME: protocollog("error")
//        return;
//    }

    RequestQ& requestQ = pendingQ.front()->second;

    while (!requestQ.empty()) {
        Request* request = requestQ.front();
        requestQ.pop();

        // Test whether the request is still valid
        if (!requestSet.count(request))
            continue;

        if (var) {
            MsrVariable::Subscription* subscription = var->subscribe(request);
            if (requestSet.count(request)) {
                request->subscription = subscription;

//                if (request->subscription->decimation < 0 and atts) {
//                    Attribute a(atts);
//                    static_cast<Channel*>(var)->pollData(
//                            a.getTime_ns("time"), a.find("value"));
//                }
            }
        }
        else {
            request->subscriber->invalid(
                    request->path, request->subscriptionId);
            subscriberMap[request->subscriber].erase(request);
            requestSet.erase(request);
            delete request;
        }
    }

    if (!var) {
        // End of processing; clean up structures
        pendingMap.erase(pendingQ.front());
        pendingQ.pop();
    }
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
void msr::ProtocolHandler::xsadAck()
{/*{{{*/
//    if (xsadQ.empty()) {
//        // FIXME: protocollog("error")
//        protocolError = true;
//        return;
//    }

    protocolError |= xsadQ.front()->xsadReady();
    xsadQ.pop();
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
void msr::ProtocolHandler::eventAck()
{/*{{{*/
    eventQ.front()->eventReady();
    eventQ.pop();
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
Parameter* msr::ProtocolHandler::getParameter(const char **_atts)
{/*{{{*/
    Attribute atts(_atts);
    unsigned index;

    if (!atts.getUInt("index", index))
        return 0;

    Parameter*& param = parameter[index];
    if (!param) {
        // index="0" name="/Constant/Value" datasize="8" typ="TDBL_LIST"
        // anz="10" cnum="10" rnum="1" orientation="VECTOR" flags="7"
        const char* name = atts.find("name");

        // Check index and name
        if (!*name) {
            log_debug("Missing index or name for variable. Ignoring");
            return 0;
        }

        // Check whether variable exists
        if (dynamic_cast<const MsrVariable*>(root->find(name))) {
            log_debug("%s exists. Ignoring", name);
            return 0;
        }

        size_t ndim, dim[2];
        PdCom::Variable::Type dt;
        if (!getDataType(_atts, dt, ndim, dim)) {
            log_debug("Datatype for %s unspecified. Ignoring", name);
            return 0;
        }

        unsigned int flags;
        if (!atts.getUInt("flags", flags)) {
            log_debug("Flags for %s unspecified. Ignoring", name);
            return 0;
        }

        param = new Parameter(this, index, dt, flags,
                atts.find("alias"), ndim, dim, atts.isTrue("dir"));
        root->insert(param, name);
    }

    return param;
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
Channel* msr::ProtocolHandler::getChannel(const char** _atts)
{/*{{{*/
    Attribute atts(_atts);
    unsigned index;

    if (!atts.getUInt("index", index))
        return 0;

    Channel*& chan = channel[index];
    if (!chan) {
//        log_debug("new chanel %u", index);

        // index="0" name="/Constant/Value" datasize="8" typ="TDBL_LIST"
        // anz="10" cnum="10" rnum="1" orientation="VECTOR" flags="7"
        const char* name = atts.find("name");

        // Check index and name
        if (!*name) {
            log_debug("Missing index or name for variable. Ignoring");
            return 0;
        }

        // Check whether variable exists
        if (dynamic_cast<const MsrVariable*>(root->find(name))) {
            log_debug("%s exists. Ignoring", name);
            return 0;
        }

        size_t ndim, dim[2];
        PdCom::Variable::Type dt;
        if (!getDataType(_atts, dt, ndim, dim)) {
            log_debug("Datatype for %s unspecified. Ignoring", name);
            return 0;
        }

        unsigned int bufsize;
        if (!atts.getUInt("bufsize", bufsize)) {
            log_debug("bufsize for %s unspecified. Ignoring", name);
            return 0;
        }

        double freq;
        if (!atts.getDouble("HZ", freq) or !freq) {
            log_debug("HZ for %s unspecified or zero. Ignoring", name);
            return 0;
        }

        unsigned int taskId = 0;
        atts.getUInt("task", taskId);

        chan = new Channel(this, index, dt, taskId, 1.0 / freq,
                atts.find("alias"), bufsize, ndim, dim, atts.isTrue("dir"));
        root->insert(chan, name);
    }

    return chan;
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
bool msr::ProtocolHandler::getDataType(const char** atts,
        PdCom::Variable::Type& type, size_t& ndim, size_t *dim)
{/*{{{*/
    unsigned int rnum = 0;
    unsigned int cnum = 0;
    unsigned int count = 0;
    const char* orientation = 0;
    size_t typeIdx;
    static struct {
        const char *msr_name;
        size_t len;
        PdCom::Variable::Type type;
    } msr_dtypemap[] = {
        { "TDBL",     4, PdCom::Variable::double_T },
        { "TINT",     4, PdCom::Variable::int32_T  },
        { "TUINT",    5, PdCom::Variable::uint32_T },
        { "TCHAR",    5, PdCom::Variable::int8_T   },
        { "TUCHAR",   6, PdCom::Variable::uint8_T  },
        { "TSHORT",   6, PdCom::Variable::int16_T  },
        { "TUSHORT",  7, PdCom::Variable::uint16_T },
        { "TLINT",    5, PdCom::Variable::int64_T  },
        { "TULINT",   6, PdCom::Variable::uint64_T },
        { "TFLT",     4, PdCom::Variable::single_T },

        { NULL,       0, PdCom::Variable::double_T }
    };

    const char* typeStr = Attribute(atts).find("typ");

    // Try and find the name in msr_dtypemap
    typeIdx = 0;
    while (strncmp(typeStr, msr_dtypemap[typeIdx].msr_name,
                    msr_dtypemap[typeIdx].len)) {
        if (!msr_dtypemap[++typeIdx].msr_name)
            return false;
    }
    type = msr_dtypemap[typeIdx].type;
//    log_debug("Found datatype %s", msr_dtypemap[typeIdx].msr_name);

    // cache variable data
    for (int i = 0; atts[i]; i += 2) {

        if (!strcmp(atts[i], "rnum")) {
            rnum = atoi(atts[i+1]);
        }
        else if (!strcmp(atts[i], "cnum")) {
            cnum = atoi(atts[i+1]);
        }
        else if (!strcmp(atts[i], "anz")) {
            count = atoi(atts[i+1]);
        }
        else if (!strcmp(atts[i], "orientation")) {
            orientation = atts[i+1];
        }
    }

    if (orientation) {
        if (!strcmp(orientation, "VECTOR")) {
            if (!count)
                return false;

            dim[0] = count;
            ndim = 1;
        }
        else if (orientation and !strncmp(orientation, "MATRIX", 6)) {
            if (!(rnum and cnum))
                return false;

            dim[0] = rnum;
            dim[1] = cnum;
            ndim = 2;
        }
        else
            return false;
    }
    else {
        if (rnum > 1 or cnum > 1 or count > 1)
            return false;

        dim[0] = 1;
        ndim = 1;
    }

    return true;
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
bool msr::ProtocolHandler::login(const char* mech, const char* clientData)
{/*{{{*/
    if (!feature.login)
        return true;

    XmlCommand sasl(cout, "auth");
    if (mech and *mech)
        sasl.addAttribute("mech", mech);

    if (clientData and *clientData)
        sasl.addAttribute("clientdata", clientData);

    return false;
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
void msr::ProtocolHandler::logout()
{/*{{{*/
    XmlCommand(cout, "auth")
        .addAttribute("logout", 1);
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
void msr::ProtocolHandler::processLogin(const char** _atts)
{/*{{{*/
    Attribute atts(_atts);
    int finished = 0;
    if (atts.getInt("success", finished))
        if (!finished)
            finished = -1;

    const char* mechlist = 0;
    atts.getString("mechlist", mechlist);

    const char* serverdata = 0;
    atts.getString("serverdata", serverdata);

    process->loginReply(mechlist, serverdata, finished);
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
void msr::ProtocolHandler::processLogMessage(
        const PdCom::Process::LogLevel_t& level,
        const char** _atts)
{/*{{{*/
    Attribute atts(_atts);

    int index = -1;
    atts.getInt("index", index);

    unsigned int state = 1;
    atts.getUInt("state", state);

    process->processMessage(level,
            atts.find("name"), index, bool(state), atts.getTime_ns("time"));
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
int msr::ProtocolHandler::connect()
{/*{{{*/
    setPolite(true);

    return 0;
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
void msr::ProtocolHandler::setPolite(bool state)
{/*{{{*/
    if (state != polite and feature.polite) {
        polite = state;

        XmlCommand(cout,"remote_host")
            .addAttribute("polite",state);
    }
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
int msr::ProtocolHandler::asyncData()
{/*{{{*/
    char buf[1024];

    int n = StreambufLayer::read(buf,sizeof(buf));
    if (!protocolError and n > 0)
        parse(buf, n);

    return n;
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
bool msr::ProtocolHandler::find(const std::string& path)
{/*{{{*/
    const MsrVariable* var = dynamic_cast<const MsrVariable*>(root->find(path));
    if (var or path.empty()) {
        process->findReply(var);
        return true;
    }

    findQ.push(path);

    if (fullListing == Uncached) {
        // Do not flush next command
        XmlCommand(cout, "rp")
            .addEscapedAttribute("name", path)
            .noFlush();

        XmlCommand(cout, "rk")
            .addEscapedAttribute("name", path)
            .setId("findQ");
    }

    return false;
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
bool msr::ProtocolHandler::processFindRequest()
{/*{{{*/
    if (findQ.empty()) // required by listReply!
        return false;

    DirNode *node = root->find(findQ.front());
    process->findReply(dynamic_cast<const PdCom::Variable*>(node));
    findQ.pop();

    return !findQ.empty();
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
bool msr::ProtocolHandler::list(const std::string& dir)
{/*{{{*/
    listQ.push(dir);

    if (fullListing != Uncached) {
        if (fullListing == Cached)
            processListRequest();
    }
    else if (dir.empty() or !feature.list) {
        // List everything. First send a <rp> and the state machine
        // will send a <rp> thereafter
        XmlCommand(cout, "rk").noFlush();
        XmlCommand(cout, "rp").setId("listQ");

        fullListing = InProgress;
    }
    else
        XmlCommand(cout, "list")
            .addEscapedAttribute("path", dir);

    return fullListing == Cached;
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
bool msr::ProtocolHandler::processListRequest()
{/*{{{*/
    std::string path = listQ.front();
    listQ.pop();

    std::list<const PdCom::Variable*> varList;
    std::list<std::string> dirList;

    DirNode::List list;
    bool listall = path.empty();
    if (listall)
        root->getChildren(&list, true);
    else {
        // Directory list
        DirNode *node = root->find(path);
        if (node)
            node->getChildren(&list, false);
    }

    for (DirNode::List::iterator it = list.begin(); it != list.end(); ++it) {
        const DirNode* node = *it;
        if (!listall and node->hasChildren())
            dirList.push_back(node->path());

        const PdCom::Variable *v = dynamic_cast<const PdCom::Variable*>(node);
        if (v)
            varList.push_back(v);
    }

    process->listReply(varList, dirList);

    return !listQ.empty();
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
void msr::ProtocolHandler::pollChannel(size_t index)
{/*{{{*/
    XmlCommand(cout, "rk")
        .addAttribute("index", index)
        .addAttribute("short", 1)
        .setId("poll");
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
void msr::ProtocolHandler::pollParameter(
        size_t index, bool verbose, bool monitor)
{/*{{{*/
    XmlCommand(cout, "rp")
        .addAttribute("index", index)
        .addAttribute("short", !verbose)
        .addAttribute("hex", 1)
        .setId(monitor ? "pm" : "poll");
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
void msr::ProtocolHandler::unsubscribe(Channel* c, int groupId)
{/*{{{*/
    XmlCommand(cout, "xsod")
        .addAttribute("channels", c->index)
        .addAttribute("group", groupId);
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
MsrVariable::Subscription* msr::ProtocolHandler::subscribe(
        Channel* c, Request* request)
{/*{{{*/
    setPolite(false);

    if (!request) {
        XmlCommand(cout, "xsad")
            .addAttribute("channels", c->index)
            .addAttribute("group", 0)
            .addAttribute("coding", "Base64")
            .addAttribute("blocksize", 1)
            .addAttribute("event", 1)
            .setId("eventQ");
        eventQ.push(c);
        return 0;
    }

    // Essentially send <xsad> if necessary

    unsigned decimation = request->interval / c->sampleTime + 0.5;
    decimation += request->interval > 0.0 and !decimation;

    DataGroup*& group = task[c->task].dataGroup[decimation];

    if (!group) {
        // Create a new data group
        unsigned blocksize = 0;

        if (decimation > 0) {
            blocksize = 1.0 / c->sampleTime / decimation / 25.0 + 0.5;
            blocksize = std::min(c->bufsize / decimation, blocksize);
        }

        if (!blocksize)
            blocksize = 1;

        // Set id with offset of 1. Id 0 is used for events
        // The DataGroups are never deleted. However, since they get
        // reused all the time, this does not really matter
        group = new DataGroup(dataGroup.size()+1, blocksize,
                c->sampleTime * decimation);
        dataGroup.push_back(group);
    }

    MsrVariable::Subscription* subscription;
    if (group->subscribe(request, c, decimation, subscription)) {
        XmlCommand(cout, "xsad")
            .addAttribute("channels", c->index)
            .addAttribute("group", group->id, group->id)
            .addAttribute("coding", "Base64")
            .addAttribute("reduction", decimation, decimation)
            .addAttribute("blocksize", group->blocksize, decimation)
            .addAttribute("event", 1, !decimation)
            .setId("xsadQ");
        xsadQ.push(group);
    }

    return subscription;
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
void msr::ProtocolHandler::subscribe(PdCom::Subscriber* subscriber,
        const std::string& path, double interval, int id)
{/*{{{*/
    setPolite(false);

    // Create a new request to manage the subscription
    Request* request = new Request(subscriber, path, interval, id);

    requestSet.insert(request);

    subscriberMap[subscriber].insert(request);

    MsrVariable* var = dynamic_cast<MsrVariable*>(root->find(request->path));
    if (var) {
        // Poll parameter if it does not have any subscribers
        const Parameter* param = dynamic_cast<const Parameter*>(var);
        if (param and !param->update())
            pollParameter(param->index, false, false);

        MsrVariable::Subscription* subscription = var->subscribe(request);

        // Make sure request was not cancelled in the meantime
        if (requestSet.count(request))
            request->subscription = subscription;

        return;
    }

    // Variable is unknown. Find it first. This requires interaction
    // with the server, which automatically postpones calls to
    // request->subscriber to a later point in time

    std::pair<PendingMap::iterator,bool> insertResult =
        pendingMap.insert(std::make_pair(request->path, RequestQ()));
    RequestQ& rqueue = insertResult.first->second;

    if (insertResult.second) {
        // Discovery not yet active

        XmlCommand(cout, "rp")
            .addAttribute("name", request->path)
            .addAttribute("hex", 1)
            .setId("pendingParameter")
            .noFlush();

        XmlCommand(cout, "rk")
            .addAttribute("name", request->path)
            .setId("pendingChannel");

        pendingQ.push(insertResult.first);
    }
    else if (rqueue.empty()) {
        // The variable was searched for unsuccessfully before.
        subscriber->invalid(path,id);
        return;
    }

    rqueue.push(request);
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
void msr::ProtocolHandler::unsubscribe(const PdCom::Variable::Subscription* s)
{/*{{{*/
    const MsrVariable::Subscription* subscription =
        static_cast<const MsrVariable::Subscription*>(s);
    Request* request = subscription->request;

    // ParameterMonitor subscriptions don't have a request
    if (!request)
        return;

    subscriberMap[request->subscriber].erase(request);
    requestSet.erase(request);

    delete subscription;
    delete request;
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
void msr::ProtocolHandler::unsubscribe(PdCom::Subscriber* s)
{/*{{{*/
    SubscriberMap::iterator it = subscriberMap.find(s);
    if (it == subscriberMap.end())
        return;

    RequestSet& set = it->second;
    for (RequestSet::const_iterator it = set.begin();
            it != set.end(); ++it) {
        Request* request = *it;

        requestSet.erase(request);

        delete
            static_cast<const MsrVariable::Subscription*>(request->subscription);
        delete request;
    }

    subscriberMap.erase(it);

    parameterMonitor(s, false);
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
void msr::ProtocolHandler::parameterMonitor(
        PdCom::Subscriber* s, bool state)
{/*{{{*/
    if (state) {
        parameterMonitorSet.insert(s);
    }
    else if (!parameterMonitorSet.empty()) {
        parameterMonitorSet.erase(s);

        if (parameterMonitorSet.empty()) {
            for (ParameterMap::iterator it = parameter.begin();
                    it != parameter.end(); ++it)
                it->second->stopMonitor();
        }
    }
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
void msr::ProtocolHandler::messageHistory(unsigned int limit)
{/*{{{*/
    XmlCommand msg(cout, "message_history");

    if (limit)
        msg.addAttribute("limit", limit);
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
void msr::ProtocolHandler::ping()
{/*{{{*/
    XmlCommand(cout, "ping");
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
bool msr::ProtocolHandler::startTLS()
{/*{{{*/
    if (feature.tls) {
        XmlCommand(cout, "starttls")
            .addAttribute("hostname", process->hostname());
        return false;
    }
    else
        return true;
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
std::string msr::ProtocolHandler::name() const
{/*{{{*/
    return m_name;
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
std::string msr::ProtocolHandler::version() const
{/*{{{*/
    return m_version;
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
void msr::ProtocolHandler::set(const Parameter* p,
        size_t offset, const std::string& hexvalue)
{/*{{{*/
    XmlCommand(cout, "wp")
        .addAttribute("index", p->index)
        .addAttribute("startindex", offset, offset)
        .addAttribute("hexvalue", hexvalue)
        .setId("wp");
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void msr::ProtocolHandler::Feature::operator=(
        const char* str)
{/*{{{*/
    pushparameters = binparameters = eventchannels =
        statistics = pmtime = aic = messages =
        quiet = list = exclusive = polite = false;
    login = 0;

    if (!str)
        return;

    const char* p = str;
    size_t len;

    do {
        p = str;
        str = strchr(str, ',');

        len = str ? str++ - p : strlen(p);

        if (len) {
            if (!strncmp(p, "pushparameters", len))
                pushparameters = true;
            
            if (!strncmp(p, "binparameters", len))
                binparameters = true;
            
            if (!strncmp(p, "eventchannels", len))
                eventchannels = true;
            
            if (!strncmp(p, "statistics", len))
                statistics = true;
            
            if (!strncmp(p, "pmtime", len))
                pmtime = true;
            
            if (!strncmp(p, "aic", len))
                aic = true;
            
            if (!strncmp(p, "messages", len))
                messages = true;
            
            if (!strncmp(p, "quiet", len))
                quiet = true;
            
            if (!strncmp(p, "list", len))
                list = true;
            
            if (!strncmp(p, "exclusive", len))
                exclusive = true;

            if (!strncmp(p, "polite", len))
                polite = true;

            if (!strncmp(p, "xsadgroups", len))
                xsadgroups = true;

            if (!strncmp(p, "tls", len))
                tls = true;

            if (!strncmp(p, "login", 5))
                login = 1 + !strncmp(p, "login=mandatory", len);
        }

    } while (str);
}/*}}}*/
