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

#ifndef PD_MSRPROTOCOLHANDLER_H
#define PD_MSRPROTOCOLHANDLER_H

#include "../ProtocolHandler.h"
#include "../StreambufLayer.h"
#include "../pdcom/Process.h"
#include "MsrVariable.h"

#include <expat_external.h>
#include <expat.h>                                                              
#include <stddef.h>
#include <string>
#include <ostream>
#include <queue>
#include <vector>
#include <map>
#include <set>

class Channel;
class Parameter;
struct RootNode;
class DataGroup;
struct Request;

namespace msr {

class ProtocolHandler:
    public ::ProtocolHandler,
    public ::StreambufLayer
{
    public:
        ProtocolHandler(PdCom::Process* process, IOLayer* io);
        ~ProtocolHandler();

        static ::ProtocolHandler* create(PdCom::Process*,
                IOLayer* io, const std::string& data);

        MsrVariable::Subscription* subscribe(Channel*, Request* r);
        void unsubscribe(Channel*, int groupId);
        void unsubscribe(const PdCom::Variable::Subscription* s);

        void pollChannel(size_t index);
        void pollParameter(size_t index, bool verbose, bool parameterMonitor);
        void set(const Parameter* p, size_t offset,
                const std::string& s);

    private:
        size_t parse(const char *buf, size_t n);
        XML_Parser xmlParser;
        int level;
        bool polite;
        bool tls;
        bool protocolError;
        uint64_t dataTime;

        std::string m_name;
        std::string m_version;

        std::ostream cout;
        RootNode* const root;

        // Structures required for list command
        std::queue<std::string> listQ;
        bool processListRequest();
        enum {
            Uncached, InProgress, Cached
        } fullListing;

        std::queue<std::string> findQ;
        bool processFindRequest();

        // Structures required for login
        void processLogin(const char** atts);

        void processLogMessage(
                const PdCom::Process::LogLevel_t& level,
                const char** atts);

        struct Feature {
            bool pushparameters, binparameters, eventchannels,
                 statistics, pmtime, aic, messages,
                 quiet, list, exclusive, polite, xsadgroups, tls;
            int login;
            void operator=(const char* list);
        };
        Feature feature;

        typedef std::queue<Request*> RequestQ;
        typedef std::map<std::string, RequestQ> PendingMap;
        PendingMap pendingMap;
        std::queue<PendingMap::iterator> pendingQ;
        std::queue<DataGroup*> xsadQ;

        typedef std::set<Request*> RequestSet;
        RequestSet requestSet;

        typedef std::map<PdCom::Subscriber*, RequestSet>
            SubscriberMap;
        SubscriberMap subscriberMap;

        typedef std::set<PdCom::Subscriber*> SubscriberSet;
        SubscriberSet parameterMonitorSet;

        typedef std::map<unsigned, Channel*> ChannelMap;
        ChannelMap channel;
        typedef std::map<unsigned, Parameter*> ParameterMap;
        ParameterMap parameter;

        std::queue<Channel*> eventQ;
        std::queue<Request*> requestQ;

        std::vector<DataGroup*> dataGroup;
        DataGroup* currentDataGroup;

        struct Task {
            std::map<unsigned, DataGroup*> dataGroup;
        };
        std::map<unsigned, Task> task;

        enum {StartUp, Idle, GetListing,
            WaitForConnected, GetVariableFields, ReadData, ReadEvent} state;

        void setPolite(bool state);

        /** Read a variable tag */
        bool getDataType(const char** atts,
                PdCom::Variable::Type& type, size_t& ndim, size_t *dim);
        Parameter* getParameter(const char** atts);
        Channel*   getChannel(  const char** atts);

        void startElement(const XML_Char *name, const XML_Char **atts);
        void endElement(const XML_Char *name);
        void characterData(const XML_Char *name, int len);

        static void XMLCALL ExpatStartElement(void *userData,
                const XML_Char *name, const XML_Char **atts);
        static void XMLCALL ExpatEndElement(void *userData,
                const XML_Char *name);
        static void XMLCALL ExpatCharacterData(void *userData,
                const XML_Char *name, int len);

        void xsadAck();
        void eventAck();
        void pendingAck(MsrVariable*);

        // Reimplemented from PdCom::ProtocolHandler
        int connect();
        int asyncData();
        bool find(const std::string& path);
        bool list(const std::string& directory);
        void logout();
        bool login(const char* mech, const char* clientData);
        void   subscribe(PdCom::Subscriber*,
                const std::string& path, double interval, int id);
        void unsubscribe(PdCom::Subscriber*);
        void parameterMonitor(PdCom::Subscriber*, bool state);
        void messageHistory(unsigned int limit);
        bool startTLS();
        void ping();
        std::string name() const;
        std::string version() const;
};

} // namespace

#endif // PD_MSRPROTOCOLHANDLER_H
