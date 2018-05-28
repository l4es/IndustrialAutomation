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

#ifndef MSRSESSION_H
#define MSRSESSION_H

// Version der MSR_LIBRARIES
#define _VERSION  6
#define _PATCHLEVEL 0
#define _SUBLEVEL  10


#define MSR_VERSION (((_VERSION) << 16) + ((_PATCHLEVEL) << 8) + (_SUBLEVEL))

//Liste der Features der aktuellen rtlib-Version, wichtig, muß aktuell gehalten werden
//da der Testmanager sich auf die Features verläßt

#define MSR_FEATURES "pushparameters,binparameters,eventchannels,statistics,pmtime,aic,messages,polite,list"

/* pushparameters: Parameter werden vom Echtzeitprozess an den Userprozess gesendet bei Änderung
   binparameters: Parameter können Binär übertragen werden
   eventchannels: Kanäle werden nur bei Änderung übertragen
   statistics: Statistische Informationen über die anderen verbundenen Echtzeitprozesses
   pmtime: Die Zeit der Änderungen eines Parameters wird vermerkt und übertragen
   aic: ansychrone Input Kanäle
   messages: Kanäle mit dem Attribut: "messagetyp" werden von der msr_lib überwacht und bei Änderung des Wertes als Klartextmeldung verschickt V:6.0.10
   polite: Server will not send any messages such as <pu> or <log> by itself
   list: Server understands <list> command
*/

#include "../Session.h"
#include "XmlParser.h"
#include "XmlElement.h"

#include <cc++/thread.h>
#include <cc++/socketport.h>
#include <vector>
#include <set>
#include <cstdio>

struct timespec;

namespace PdServ {
    class Task;
    class Parameter;
    class TaskStatistics;
    class SessionStatistics;
}

namespace MsrProto {

class SubscriptionManager;
class Server;
class Parameter;

class Session:
    public ost::TCPSession,
    public PdServ::Session {
    public:
        Session( Server *s, ost::TCPSocket *socket);
        ~Session();

        void broadcast(Session *s, const struct timespec& ts,
                const std::string &action, const std::string &element);
        void parameterChanged(const Parameter*);
        void setAIC(const Parameter* p);
        void getSessionStatistics(PdServ::SessionStatistics &stats) const;
        XmlElement createElement(const char *name);

        const struct timespec *getTaskTime(const PdServ::Task* task) const;
        const PdServ::TaskStatistics *getTaskStatistics(
                const PdServ::Task* task) const;

        double *getDouble() const {
            return &tmp.dbl;
        }

    private:
        Server * const server;

        size_t inBytes;
        size_t outBytes;
        std::string peer() const;

        std::ostream xmlstream;

        std::string commandId;

        // Protection for inter-session communication
        ost::Mutex mutex;

        // List of parameters that have changed
        typedef std::set<const Parameter*> ParameterSet;
        ParameterSet changedParameter;

        // Asynchronous input channels.
        // These are actually parameters that are misused as input streams.
        // Parameters in this set are not announced as changed as often as
        // real parameters are.
        typedef std::set<const PdServ::Parameter*> MainParameterSet;
        MainParameterSet aic;
        size_t aicDelay;        // When 0, notify that aic's have changed

        // Broadcast list.
        typedef struct {
            struct timespec ts;
            std::string action;
            std::string message;
        } Broadcast;
        typedef std::list<const Broadcast*> BroadcastList;
        BroadcastList broadcastList;

        typedef std::vector<SubscriptionManager*> SubscriptionManagerVector;
        SubscriptionManagerVector subscriptionManager;
        const SubscriptionManager *timeTask;

        // Temporary memory space needed to handle statistic channels
        union {
            uint32_t uint32;
            double dbl;
        } mutable tmp;

        // Reimplemented from ost::Thread
        void initial();
        void run();
        void final();

        // Reimplemented from PdServ::Session
        ssize_t write(const void* buf, size_t len);
        ssize_t read(       void* buf, size_t len);

        void processCommand(const XmlParser*);
        // Management variables
        bool writeAccess;
        bool quiet;
        bool polite;
        bool echoOn;
        std::string remoteHostName;
        std::string client;

        // Here are all the commands the MSR protocol supports
        void broadcast(const XmlParser*);
        void echo(const XmlParser*);
        void ping(const XmlParser*);
        void readChannel(const XmlParser*);
        void listDirectory(const XmlParser*);
        void readParameter(const XmlParser*);
        void readParamValues(const XmlParser*);
        void readStatistics(const XmlParser*);
        void messageHistory(const XmlParser*);
        void remoteHost(const XmlParser*);
        void startTLS(const XmlParser*);
        void writeParameter(const XmlParser*);
        void xsad(const XmlParser*);
        void xsod(const XmlParser*);
};

}
#endif //MSRSESSION_H
