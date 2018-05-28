/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2007-2016  Richard Hacker (lerichi at gmx dot net)
 *                          Florian Pose <fp@igh.de>
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

#include "pdcom/Process.h"
#include "IOLayer.h"

#include "msrproto/ProtocolHandler.h"
#include "Debug.h"

using namespace PdCom;

struct ProcessIOLayer: IOLayer {
    ProcessIOLayer(PdCom::Process* process)
        : IOLayer(0), process(process) {}

    inline int read(char* buf, size_t n) { return process->read(buf, n); }
    inline void write(const char* buf, size_t n) { process->write(buf, n); }
    inline void flush() { process->flush(); }

    PdCom::Process* const process;
};

///////////////////////////////////////////////////////////////////////////
Process::Process(): io(new ProcessIOLayer(this))
{
    protocolHandler = 0;
}

///////////////////////////////////////////////////////////////////////////
Process::~Process()
{
    log_debug("Del %p", this);

    reset();
    delete io;
}

///////////////////////////////////////////////////////////////////////////
void Process::reset()
{
    io->rxbytes = 0;
    io->txbytes = 0;

    delete protocolHandler;
    protocolHandler = 0;
}

///////////////////////////////////////////////////////////////////////////
int Process::asyncData()
{
    if (protocolHandler)
        return protocolHandler->asyncData();

    char buf[1024];
    int n = io->read(buf, sizeof(buf));
    if (n > 0) {
        initialBuffer.append(buf, n);

        do {
            protocolHandler =
                msr::ProtocolHandler::create(this, io, initialBuffer);
            if (protocolHandler) {
                protocolLog(Info, "Detected MSR protocol");
                break;
            }

        } while (false);
    }

    if (protocolHandler)
        initialBuffer.clear();

    return n;
}

///////////////////////////////////////////////////////////////////////////
std::string Process::applicationName() const
{
    return "pdcom";
}

///////////////////////////////////////////////////////////////////////////
std::string Process::name() const
{
    return protocolHandler->name();
}

///////////////////////////////////////////////////////////////////////////
std::string Process::version() const
{
    return protocolHandler->version();
}

///////////////////////////////////////////////////////////////////////////
std::string Process::hostname() const
{
    return std::string();
}

///////////////////////////////////////////////////////////////////////////
bool Process::list(const std::string& dir) const
{
    return protocolHandler->list(dir);
}

///////////////////////////////////////////////////////////////////////////
bool Process::find(const std::string& path) const
{
    return protocolHandler->find(path);
}

///////////////////////////////////////////////////////////////////////////
void Process::findReply(const Variable* /*v*/)
{
}

///////////////////////////////////////////////////////////////////////////
bool Process::login(const char* mech, const char* clientData) const
{
    return protocolHandler->login(mech, clientData);
}

///////////////////////////////////////////////////////////////////////////
void Process::logout() const
{
    protocolHandler->logout();
}

///////////////////////////////////////////////////////////////////////////
void Process::loginReply(const char* /*mechlist*/,
        const char* /*serverData*/, int /*finished*/)
{
    log_debug("skipped");
}

///////////////////////////////////////////////////////////////////////////
void Process::listReply(
        std::list<const Variable*>& /*variables*/,
        std::list<std::string>& /*path*/)
{
}

///////////////////////////////////////////////////////////////////////////
void Process::pingReply()
{
}

///////////////////////////////////////////////////////////////////////////
void Process::ping() const
{
    protocolHandler->ping();
}

///////////////////////////////////////////////////////////////////////////
void Process::startTLSReply()
{
}

///////////////////////////////////////////////////////////////////////////
bool Process::startTLS() const
{
    return protocolHandler->startTLS();
}

///////////////////////////////////////////////////////////////////////////
bool Process::alive()
{
    return true;
}

///////////////////////////////////////////////////////////////////////////
void Process::processMessage(
        LogLevel_t         /*level*/,
        const std::string& /*path*/,
        int                /*index*/,
        bool               /*state*/,
        uint64_t           /*time*/)
{
}

///////////////////////////////////////////////////////////////////////////
void Process::messageHistory(unsigned int limit) const
{
    protocolHandler->messageHistory(limit);
}

///////////////////////////////////////////////////////////////////////////
void Process::protocolLog(LogLevel_t /*level*/,
        const std::string& /*message*/)
{
}

///////////////////////////////////////////////////////////////////////////
void Process::subscribe(Subscriber* subscriber,
        const std::string& path, double interval, int id) const
{
    protocolHandler->subscribe(subscriber, path, interval, id);
}

///////////////////////////////////////////////////////////////////////////
void Process::unsubscribe(Subscriber* s) const
{
    protocolHandler->unsubscribe(s);
}

///////////////////////////////////////////////////////////////////////////
void Process::parameterMonitor(Subscriber* subscriber, bool state) const
{
    protocolHandler->parameterMonitor(subscriber, state);
}
