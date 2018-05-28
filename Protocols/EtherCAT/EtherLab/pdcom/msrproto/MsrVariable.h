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

#ifndef MSR_VARIABLE
#define MSR_VARIABLE

#include "../pdcom/Variable.h"
#include "DirNode.h"
#include <string>

namespace msr {
    class ProtocolHandler;
}

struct Request;

class MsrVariable: public PdCom::Variable, public DirNode {
    public:
        MsrVariable(msr::ProtocolHandler* handler,
                unsigned int index,
                PdCom::Variable::Type type,
                unsigned taskId,
                bool writeable,
                double sampleTime,
                const char* alias,
                size_t ndim,
                const size_t *dims,
                bool isDir);

        const unsigned int index;

        std::string path() const {
            return DirNode::path();
        }
        std::string name() const {
            return DirNode::name();
        }

        struct Subscription: PdCom::Variable::Subscription {
            Subscription(PdCom::Process* process,
                    Request* request,
                    PdCom::Variable* var,
                    int decimation,
                    const uint64_t* const* time,
                    const char* const* data);
            virtual ~Subscription();

            Request* const request;

            // Reimplemented from PdCom::Variable::Subscription
            void cancel() const;
        };

        virtual Subscription* subscribe(Request*) = 0;

    protected:
        msr::ProtocolHandler* const handler;

        std::string hexValue(const void* src, size_t n) const;
        bool readFromHexDec(char* val, const char* s) const;
        bool readFromString(char* val, const char* s) const;
};

#endif //MSR_VARIABLE
