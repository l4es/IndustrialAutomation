/****************************************************************************
 *
 * $Id$
 *
 ***************************************************************************/

#include <string.h> // memcpy()
#include <syslog.h>

#include <iostream>
#include <sstream>
#include <iostream>
using namespace std;

#include <pdcom/Variable.h>

#include <Variable.h>

#define DEBUG 0
#define DEBUG_NOTIFY 0

/***************************************************************************/

Variable::Variable(
        sqlite3 *db,
        const Yaml &config
        ):
    Common::Variable(db, config),
    state(ListenOnly),
    var(NULL),
    writeVar(NULL),
    applyVar(NULL),
    storedData(NULL),
    storedDataPresent(false),
    receivedData(NULL),
    receivedDataPresent(false),
    applyValues(NULL),
    insertRequest(false)
{
    lastInsertTime.tv_sec = 0;
    lastInsertTime.tv_usec = 0;
    lastChangeTime.tv_sec = 0;
    lastChangeTime.tv_usec = 0;

    queryVariable();

    if (inDb()) {
        storedData = new uint8_t[memSize()];

        try {
            storedDataPresent = queryLastData(storedData);
        } catch (Exception &e) {
            delete [] storedData;
            throw e;
        }
    }
}

/***************************************************************************/

Variable::~Variable()
{
    if (insertRequest) {
        insertData(lastChangeTime, receivedData);
    }

    if (var) {
        var->unsubscribe(this);
        var = NULL;
    }

    if (applyVar) {
        applyVar->unsubscribe(this);
        applyVar = NULL;
    }

    if (storedData) {
        delete [] storedData;
    }

    if (receivedData) {
        delete [] receivedData;
    }

    if (applyValues) {
        delete [] applyValues;
    }

    reset();
}

/****************************************************************************/

/** Reset the variable to ListenOnly state.
 */
void Variable::listenOnly()
{
#if DEBUG
    cerr << __func__ << "(" << getPath() << ")" << endl;
#endif

    state = ListenOnly;
}

/****************************************************************************/

/** Start inserting new data on new value reception.
 */
void Variable::startInserting()
{
#if DEBUG
    cerr << __func__ << "(" << getPath() << ")" << endl;
#endif

    state = InsertData;

    if (storedDataPresent && receivedDataPresent) {
        if (dataDiffer(receivedData, storedData)) {
            insertRequest = true;
            checkInsert();
        }
    }
}

/****************************************************************************/

/** Restore data, or mark for later restoring.
 */
void Variable::restore()
{
#if DEBUG
    cerr << __func__ << "(" << getPath() << ")" << endl;
    cerr << "stored=" << storedDataPresent
        << " rec=" << receivedDataPresent << endl;
#endif

    if (storedDataPresent) {
        if (receivedDataPresent) {
            if (dataDiffer(receivedData, storedData)) {
                restoreData();
            }
            state = InsertData;
        }
        else {
            // delay restoring until data have arrived
            state = RestoreData;
        }
    }
    else {
        // value will be inserted for the first time later
        state = InsertData;
    }
}

/***************************************************************************/

void Variable::connectVariables(PdCom::Process *p)
{
    double period;

    var = p->findVariable(getPath());
    if (!var) {
        stringstream err;
        err << "Path does not exist!";
        log(LOG_ERR, err.str());
        throw Exception(this, err.str());
    }

    if (inDb()) {
        /* variable already stored in database. check type and dimension */

        if (var->type != getType()) {
            stringstream err;
            err << "Process variable type mismatch (stored " << getType()
                << ", online " << var->type << ")!";
            var = NULL;
            throw Exception(this, err.str());
        }

        if (var->dimension.getElementCount() != getElementCount()) {
            stringstream err;
            err << "Process variable dimension mismatch (stored "
                << getElementCount() << ", online "
                << var->dimension.getElementCount() << ")!";
            var = NULL;
            throw Exception(this, err.str());
        }
    }

    writeVar = p->findVariable(getWritePath());
    if (!writeVar) {
        stringstream err;
        var = NULL;
        err << "Write path \"" << getWritePath() << "\" does not exist!";
        throw Exception(this, err.str());
    }

    if (!writeVar->isWrite()) {
        stringstream err;
        err << writeVar->path << " is not writable!";
        var = NULL;
        writeVar = NULL;
        throw Exception(this, err.str());
    }

    if (writeVar->type != var->type) {
        stringstream err;
        err << "Write path \"" << writeVar->path << "\" type mismatch!";
        var = NULL;
        writeVar = NULL;
        throw Exception(this, err.str());
    }

    if (writeVar->dimension != var->dimension) {
        stringstream err;
        err << "Write path \"" << writeVar->path << "\" dimension mismatch!";
        var = NULL;
        writeVar = NULL;
        throw Exception(this, err.str());
    }

    if (!getApplyPath().empty()) {
        applyVar = p->findVariable(getApplyPath());
        if (!applyVar) {
            stringstream err;
            var = NULL;
            writeVar = NULL;
            err << "Apply path \"" << getApplyPath() << "\" does not exist!";
            throw Exception(this, err.str());
        }

        if (!applyVar->isWrite()) {
            stringstream err;
            var = NULL;
            writeVar = NULL;
            applyVar = NULL;
            err << getApplyPath() << " is not writable!";
            throw Exception(this, err.str());
        }

        size_t nelem = applyVar->dimension.getElementCount();
        if (applyVar->dimension != var->dimension && nelem != 1) {
            stringstream err;
            err << "Apply variable has invalid dimension!";
            throw Exception(this, err.str());
        }

        applyValues = new int[nelem];
    }

    if (!inDb()) {
        /* Variable is not yet in database.
         * Now it is the time to insert it. */
        insertVariable(var->type, var->dimension.getElementCount());
        log(LOG_INFO, "Inserted.");

        storedData = new uint8_t[memSize()];
    }

    receivedData = new uint8_t[memSize()];

    if (var->samplePeriod == 0.0) {
        period = 0.0;
    } else {
        period = 1.0;
    }

    var->subscribe(this, period);
    var->poll(this);

    if (applyVar) {
        if (applyVar->samplePeriod == 0.0) {
            period = 0.0;
        } else {
            period = 1.0;
        }

        applyVar->subscribe(this, period);
        applyVar->poll(this);
    }
}

/***************************************************************************/

void Variable::notify(PdCom::Variable *v)
{
    if (v == var) {
#if DEBUG_NOTIFY
        cerr << __func__ << "(\"" << var->path << "\"): "
            << output(var->getDataPtr(), getType(), getElementCount())
               << endl;
#endif

        if (!receivedDataPresent ||
                dataDiffer(var->getDataPtr(), receivedData)) {
            gettimeofday(&lastChangeTime, NULL);
        }

        memcpy(receivedData, var->getDataPtr(), memSize());
        receivedDataPresent = true;

        if (storedDataPresent) {
            if (dataDiffer(receivedData, storedData)) {
#if not DEBUG
                if (!var->samplePeriod)
#endif
                {
                    stringstream msg;
                    msg << "Changed. Stored "
                        << output(storedData, getType(), getElementCount())
                        << "; received "
                        << output(receivedData, getType(), getElementCount())
                        << ".";
                    log(LOG_INFO, msg.str());
                }

                // data differ from stored
                if (state == RestoreData) {
                    restoreData();
                    state = InsertData;
                } else if (state == InsertData) {
                    insertRequest = true;
                    checkInsert();
                }
            } else {
                if (state == RestoreData) {
                    // data equal; skip restoring
                    state = InsertData;
                } else if (state == InsertData) {
                    // data equal again -> reset request
                    insertRequest = false;
                }
            }
        } else {
            // if no data are in the database up to now;
            // insert the first data
            insertData(lastChangeTime, receivedData);
            state = InsertData;
        }
    }

    if (applyVar && v == applyVar) {
        size_t nelem = applyVar->dimension.getElementCount();
#if DEBUG_NOTIFY
        cerr << __func__ << "(\"" << applyVar->path << "\") apply: "
            << output(applyVar->getDataPtr(), applyVar->type, nelem) << endl;
#endif
        applyVar->getValue(applyValues, nelem);
    }
}

/***************************************************************************/

void Variable::notifyDelete(PdCom::Variable *v)
{
    if (v == var) {
        if (insertRequest) {
            insertRequest = false;
            insertData(lastChangeTime, receivedData);
        }
        var = NULL;
        receivedDataPresent = false;
        if (receivedData) {
            delete [] receivedData;
            receivedData = NULL;
        }
    }

    if (v == writeVar) {
        // Attention: in most cases, writeVar is not subscribed!
        writeVar = NULL;
    }

    if (v == applyVar) {
        applyVar = NULL;
        if (applyValues) {
            delete [] applyValues;
            applyValues = NULL;
        }
    }
}

/***************************************************************************/

bool Variable::dataDiffer(const void *a, const void *b) const
{
    unsigned int i;

    for (i = 0; i < memSize(); i++) {
        if (((const uint8_t *) a)[i] != ((const uint8_t *) b)[i]) {
            return true;
        }
    }

    return false;
}

/***************************************************************************/

void Variable::restoreData()
{
    stringstream msg;

    msg << "Restoring data.";
    log(LOG_INFO, msg.str());

    switch (getType()) {
        case PdCom::Data::bool_T:
            writeVar->setValue((const bool *) storedData,
                    getElementCount());
            break;
        case PdCom::Data::uint8_T:
            writeVar->setValue((const uint8_t *) storedData,
                    getElementCount());
            break;
        case PdCom::Data::sint8_T:
            writeVar->setValue((const int8_t *) storedData,
                    getElementCount());
            break;
        case PdCom::Data::uint16_T:
            writeVar->setValue((const uint16_t *) storedData,
                    getElementCount());
            break;
        case PdCom::Data::sint16_T:
            writeVar->setValue((const int16_t *) storedData,
                    getElementCount());
            break;
        case PdCom::Data::uint32_T:
            writeVar->setValue((const uint32_t *) storedData,
                    getElementCount());
            break;
        case PdCom::Data::sint32_T:
            writeVar->setValue((const int32_t *) storedData,
                    getElementCount());
            break;
        case PdCom::Data::uint64_T:
            writeVar->setValue((const uint64_t *) storedData,
                    getElementCount());
            break;
        case PdCom::Data::sint64_T:
            writeVar->setValue((const int64_t *) storedData,
                    getElementCount());
            break;
        case PdCom::Data::single_T:
            writeVar->setValue((const float *) storedData,
                    getElementCount());
            break;
        case PdCom::Data::double_T:
            writeVar->setValue((const double *) storedData,
                    getElementCount());
            break;
    }

    if (applyVar) {
        stringstream msg;
        size_t nelem = applyVar->dimension.getElementCount();
        int inc[nelem];
        msg << "Applying data.";
        log(LOG_INFO, msg.str());
        for (size_t i = 0; i < nelem; i++) {
            inc[i] = applyValues[i] + 1;
        }
        applyVar->setValue(inc, nelem);
    }
}

/***************************************************************************/

void Variable::checkInsert()
{
    struct timeval now, start;
    bool minTimeElapsed;

    if (!insertRequest || !var) {
        return;
    }

    gettimeofday(&now, NULL);
    if (getWaitForStable()) {
        start = lastChangeTime;
    } else {
        start = lastInsertTime;
    }
    minTimeElapsed = (now.tv_sec - start.tv_sec) >= (int) getUpdatePeriod();

    if ((getUpdatePeriod() == 0 && var->samplePeriod == 0.0) ||
            (getUpdatePeriod() > 0 && minTimeElapsed)) {
        insertRequest = false;
        insertData(lastChangeTime, receivedData);
    }
}

/***************************************************************************/

void Variable::insertData(const struct timeval &time, const void *data)
{
    stringstream msg;

    try {
        Common::Variable::insertData(time, data);
    } catch (Exception &e) {
        msg << "Failed to store data: " << e.msg;
        log(LOG_ERR, msg.str());
        return;
    }

    gettimeofday(&lastInsertTime, NULL);
    memcpy(storedData, data, memSize());
    storedDataPresent = true;

    msg << "Stored data.";
#if DEBUG
    msg << " sto=" << output(storedData, getType(), getElementCount())
        << " rec=" << output(receivedData, getType(), getElementCount());
#endif
    log(LOG_INFO, msg.str());
}

/***************************************************************************/

void Variable::log(int prio, const string &msg)
{
    string varMsg = "Variable \"" + getPath() + "\": " + msg;
    syslog(prio, varMsg.c_str());
    cerr << varMsg << endl;
}

/***************************************************************************/
