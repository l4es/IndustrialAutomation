/*****************************************************************************
 *
 * $Id$
 *
 ****************************************************************************/

#include <iostream>
#include <sstream>
using namespace std;

#include "Process.h"
#include "../common/Variable.h"

/****************************************************************************/

void Process::showVariables()
{
    VariableList::const_iterator v;

    for (v = variables.begin(); v != variables.end(); v++) {
        cerr << (*v)->getPath() << endl;
    }
}

/****************************************************************************/

Common::Variable *Process::createVariable(sqlite3 *db,
                const Yaml &config)
{
    return new Common::Variable(db, config);
}

/****************************************************************************/
