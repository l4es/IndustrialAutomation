/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2016       Richard Hacker (lerichi at gmx dot net)
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

#ifndef MSR_REQUEST_H
#define MSR_REQUEST_H

#include "../pdcom/Variable.h"

namespace PdCom {
    struct Subscriber;
}

struct Request {
    Request(PdCom::Subscriber* subscriber,
            const std::string& path, double interval,
            int subscriptionId);

    PdCom::Subscriber* const subscriber;
    const std::string path;
    const double interval;

    int subscriptionId;

    PdCom::Variable::Subscription* subscription;
};

#endif //MSR_REQUEST_H
