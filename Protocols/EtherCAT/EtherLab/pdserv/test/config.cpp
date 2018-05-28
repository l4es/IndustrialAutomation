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

#include <iostream>
#include "../src/Config.h"

using namespace std;

int main(int argc, char **argv)
{
    PdServ::Config conf;

    if (argc > 1) {
        const char *err = conf.load(argv[1]);
        if (err) {
            cout << err << endl;
            return 1;
        }
    }

    PdServ::Config c(conf["lan"]);
    cout << (int)conf["lan"] << endl;
    cout << (int)conf["lan"]["msr"]["port"] << endl;


    return 0;
}
