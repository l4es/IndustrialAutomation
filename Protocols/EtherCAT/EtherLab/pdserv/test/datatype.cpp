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

#include "DataType.h"
#include <assert.h>
#include <stddef.h>

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

using namespace PdServ;

struct _s2 {
    uint32_t f1;
    char y[4];
};

struct _s {
    uint16_t field1;
    uint8_t field4[5];
    struct _s2 v[2];
    double d2;
};

int main(int /*argc*/, const char * /*argv*/[])
{
    DataType* dt = new DataType("struct", sizeof(struct _s));
    DataType* dts2 = new DataType("struct2", sizeof(struct _s2));
    struct _s s[] = {
        {55, {88,}, {{98, {1,2,3,4}}, {77, {8,7,6,5}}}, 3.14},
        {16, {89,}, {}, 6.14},
    };
    struct _s s3[4];

    dt->addField("field1", DataType::uint16, offsetof(struct _s, field1));
    dt->addField("field4", DataType::uint8, offsetof(struct _s, field4), 5);
    dt->addField("v", *dts2, offsetof(struct _s, v), 2);
    dt->addField("d2", DataType::float64, offsetof(struct _s, d2));

    dts2->addField("f1", DataType::uint32, offsetof(struct _s2, f1));
    dts2->addField("y", DataType::int8, offsetof(struct _s2, y), 4);

    cout << (*dt)(s,2) << endl;
    cout << (*dt)(s3,4) << endl;

    return 0;
}
