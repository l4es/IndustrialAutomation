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

#ifndef PDCOM_PROTOCOLHANDLER_H
#define PDCOM_PROTOCOLHANDLER_H

#include <stddef.h>
#include <string>

namespace PdCom {
    class Process;
    struct Subscriber;
}

class ProtocolHandler {
    public:
        ProtocolHandler(PdCom::Process* process);
        virtual ~ProtocolHandler();

        static int create(PdCom::Process*, const char *buf, size_t n);

        virtual int connect() = 0;
        virtual int asyncData() = 0;
        virtual bool find(const std::string& path) = 0;
        virtual bool list(const std::string& path) = 0;
        virtual bool login(const char* mech, const char* clientData) = 0;
        virtual void logout() = 0;
        virtual void   subscribe(PdCom::Subscriber*,
                const std::string& path, double interval, int id) = 0;
        virtual void unsubscribe(PdCom::Subscriber*) = 0;
        virtual void parameterMonitor(
                PdCom::Subscriber*, bool state) = 0;
        virtual bool startTLS() = 0;
        virtual void ping() = 0;
        virtual void messageHistory(unsigned int limit) = 0;
        virtual std::string name() const = 0;
        virtual std::string version() const = 0;

        PdCom::Process* const process;
};

#endif // PDCOM_PROTOCOLHANDLER_H
