/*****************************************************************************
 *
 * $Id$
 *
 ****************************************************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h> // gethostbyname()
#include <string.h> // memset()
#include <syslog.h>
#include <signal.h>

#include <iostream>
#include <sstream>
#include <cstring>
using namespace std;

#include "Process.h"
#include "Variable.h"

#include "../common/Yaml.h"

#define DEBUG 0
#define DEBUG_DATA 0

/****************************************************************************/

/** Constructor.
 */
Process::Process():
    Common::Process(),
    PdCom::Process(),
    finished(false),
    reconnectPeriod(5),
    keepalivePeriod(-1),
    receiveTimeout(-1),
    fd(-1),
    writeRequest(false),
    restoreVar(NULL),
    restoreData(0),
    restoreArmed(false)
{
    lastConnectTime.tv_sec = 0U;
    lastConnectTime.tv_usec = 0U;
    keepaliveTime.tv_sec = 0U;
    keepaliveTime.tv_usec = 0U;
    lastReceiveTime.tv_sec = 0U;
    lastReceiveTime.tv_usec = 0U;

    openlog("pdpersistd", LOG_PID, LOG_DAEMON);

    stringstream msg;
    msg << "Starting pdpersistd.";
    log(LOG_INFO, msg.str());
}

/****************************************************************************/

/** Destructor.
 */
Process::~Process()
{
    disconnect();

    if (restoreVar) {
        restoreVar->unsubscribe(this);
        restoreVar = NULL;
    }

    if (!pidPath.empty()) {
        removePidFile();
    }

    stringstream msg;
    msg << "Exiting.";
    log(LOG_INFO, msg.str());
    closelog();
}

/****************************************************************************/

/** Mark all variables for restoring (ignored if restore variable is used).
 */
void Process::restore()
{
    if (restorePath != "") {
        // if restore path punctionality shall be used, delay restoring until
        // checked after connection
        return;
    }

    VariableList::iterator it;
    for (it = variables.begin(); it != variables.end(); it++) {
        Variable *v = dynamic_cast<Variable *>(*it);
        v->restore();
    }
}

/****************************************************************************/

/** Execute.
 */
void Process::run()
{
    struct timeval t, now;
    fd_set read_fds, write_fds, except_fds;
    int max_fd, ret;
    VariableList::iterator it;

    while (!finished) {
        FD_ZERO(&read_fds);
        FD_ZERO(&write_fds);
        FD_ZERO(&except_fds);
        max_fd = -1;

        if (fd != -1) {
            FD_SET(fd, &read_fds);

            if (writeRequest) {
                FD_SET(fd, &write_fds);
            }

            max_fd = fd;
        }

        t.tv_sec = 1;
        t.tv_usec = 0;
        ret = select(max_fd + 1, &read_fds, &write_fds, &except_fds, &t);
        if (ret == -1) {
            if (errno == EINTR) {
                continue;
            } else {
                stringstream err;
                err << "select() failed: " << strerror(errno);
                log(LOG_ERR, err.str());
                throw Exception(err.str());
            }
        }

        gettimeofday(&now, NULL);

        if (fd != -1) { // connected

            if (FD_ISSET(fd, &read_fds)) { // new data
                readSocket();
            }

            if (fd != -1 && FD_ISSET(fd, &write_fds)) { // ready to write
                writeSocket();
            }

            if (keepalivePeriod >= 0) {
                if (now.tv_sec >= keepaliveTime.tv_sec + keepalivePeriod) {
                    const char *data = "<ping/>\n";
                    sendData(data, strlen(data));
                    keepaliveTime = now;
                }
            }

            if (receiveTimeout >= 0) {
                if (now.tv_sec >= lastReceiveTime.tv_sec + receiveTimeout) {
                    stringstream err;
                    err << "Receive timeout expired. Closing connection.";
                    log(LOG_ERR, err.str());
                    disconnect();
                    if (restorePath == "") {
                        throw Exception(err.str());
                    }
                }
            }

        } else {
            if (now.tv_sec >= lastConnectTime.tv_sec + reconnectPeriod) {
                lastConnectTime = now;
                connect();
            }
        }

        for (it = variables.begin(); it != variables.end(); it++) {
            Variable *v = dynamic_cast<Variable *>(*it);
            v->checkInsert();
        }
    }
}

/****************************************************************************/

/** Finish.
 */
void Process::signal(int sig)
{
    stringstream msg;
    msg << "Received signal " << sig << ": " << strsignal(sig) << ".";
    log(LOG_INFO, msg.str());

    switch (sig) {
        case SIGINT:
        case SIGTERM:
            finished = true;
            break;
        default:
            break;
    }
}

/****************************************************************************/

void Process::createPidFile(const string &path)
{
    int fd, ret;
    stringstream str;

    fd = open(path.c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if (fd == -1) {
        stringstream err;
        err << "Failed to create PID file \"" << path << "\": "
            << strerror(errno);
        log(LOG_ERR, err.str());
        return;
    }

    // remember path for deletion on exit
    pidPath = path;

    str << getpid() << endl;

    ret = write(fd, str.str().c_str(), str.str().length());
    if (ret == -1) {
        stringstream err;
        err << "Failed to write to PID file \"" << path << "\": "
            << strerror(errno);
        log(LOG_ERR, err.str());
        goto out_close;
    }

    if ((unsigned int) ret != str.str().length()) {
        stringstream err;
        err << "Failed to write to PID file \"" << path << "\". Written "
            << ret << " of " << str.str().length() << " bytes.";
        log(LOG_ERR, err.str());
        goto out_close;
    }

out_close:
    close(fd);
}

/****************************************************************************/

void Process::processConfig(const Yaml &config, const std::string &host,
        uint16_t port, const std::string &database)
{
    Common::Process::processConfig(config, host, port, database);

    try {
        restorePath = (std::string) config["restore"];
    } catch (Yaml::NotFoundException &e) {
        // unset, default = empty
    } catch (Yaml::Exception &e) {
        stringstream err;
        err << "YAML exception: " << e.msg;
        throw Exception(err.str());
    }

    try {
        reconnectPeriod = config["reconnect"];
    } catch (Yaml::NotFoundException &e) {
        // unset, use default (see constructor)
    } catch (Yaml::Exception &e) {
        stringstream err;
        err << "YAML exception: " << e.msg;
        throw Exception(err.str());
    }

    try {
        keepalivePeriod = config["keepalive"];
    } catch (Yaml::NotFoundException &e) {
        // unset, use default (see constructor)
    } catch (Yaml::Exception &e) {
        stringstream err;
        err << "YAML exception: " << e.msg;
        throw Exception(err.str());
    }

    try {
        receiveTimeout = config["timeout"];
    } catch (Yaml::NotFoundException &e) {
        // unset, use default (see constructor)
    } catch (Yaml::Exception &e) {
        stringstream err;
        err << "YAML exception: " << e.msg;
        throw Exception(err.str());
    }

    stringstream msg;
    if (restorePath == "") {
        msg << "Restore variable unset. Auto-reconnect disabled!";

        // Mark all variables to store data once connected. If restoring is
        // commanded (--restore), this state will be changed later on.
        VariableList::iterator it;
        for (it = variables.begin(); it != variables.end(); it++) {
            Variable *v = dynamic_cast<Variable *>(*it);
            v->startInserting();
        }
    }
    else {
        msg << "Restore variable is \"" << restorePath << "\".";
    }
    log(LOG_INFO, msg.str());
}

/****************************************************************************/

/** Connect.
 */
void Process::connect()
{
    stringstream err;
    struct sockaddr_in serverAddress;
    struct hostent *hostent;
    int flags;

    memset(&serverAddress, 0, sizeof(serverAddress));

    // Get a TCP file descriptor
    if ((fd = ::socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        err << "Failed to open socket: " << strerror(errno);
        log(LOG_ERR, err.str());
        throw Exception(err.str());
    }

    // Set the keepalive option
    int optval = 1;
    socklen_t optlen = sizeof(optval);
    if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen) < 0) {
        stringstream msg;
        msg << "Failed to set SO_KEEPALIVE flag: " << strerror(errno);
        log(LOG_WARNING, msg.str());
    }

    // Resolve hostname
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    if (!(hostent = gethostbyname(host.c_str()))) {
        disconnect();
        err << "Failed to resolve hostname " << host << ".";
        log(LOG_ERR, err.str());
        return;
    }

#if 0
    cerr << "Connecting to " << host << ":" << port << "..." << endl;
#endif

    memcpy((char *) &serverAddress.sin_addr, hostent->h_addr,
            sizeof(struct in_addr));
    if (::connect(fd, (const struct sockaddr *) &serverAddress,
                sizeof(serverAddress))) {
        disconnect();
        err << "Failed to connect: " << strerror(errno);
        log(LOG_ERR, err.str());
        return;
    }

#if 1
    {
        stringstream msg;
        msg << "Connected to " << host << ":" << port << ".";
        log(LOG_INFO, msg.str());
    }
#endif

    // Make socket non-blocking here. This is not required by
    // the library, but it may be necessary in the implementation
    // environment, for example in a desktop application where
    // the event loop should never be blocked.
    if ((flags = fcntl(fd, F_GETFL, 0)) < 0) {
        disconnect();
        err << "Failed to get flags: " << strerror(errno);
        log(LOG_ERR, err.str());
        throw Exception(err.str());
    }

    flags |= O_NONBLOCK;

    if (fcntl(fd, F_SETFL, flags) < 0) {
        disconnect();
        err << "Failed to set flags: " << strerror(errno);
        log(LOG_ERR, err.str());
        throw Exception(err.str());
    }

    writeRequest = false;
}

/****************************************************************************/

/** Disconnect.
 */
void Process::disconnect()
{
    reset();

    if (fd != -1) {
        ::close(fd);
        fd = -1;
    }
}

/****************************************************************************/

/** Read data from TCP socket.
 *
 * If new data have arrived, send them down to PdCom::Process by calling
 * newData().
 */
void Process::readSocket()
{
    int ret;
    char buf[4096];

    ret = ::read(fd, buf, sizeof(buf));
    if (ret > 0) {
#if DEBUG_DATA
        cerr << "read: " << string(buf, ret) << endl;
#endif
        gettimeofday(&lastReceiveTime, NULL);
        keepaliveTime = lastReceiveTime;

        try {
            newData(buf, ret);
        } catch (PdCom::Exception &e) {
            stringstream err;
            err << e.what() << ", last data: " << string(buf, ret);
            log(LOG_ERR, err.str());
            throw Exception(err.str());
        }
    } else if (ret < 0) {
        if (errno == ECONNRESET) {
            stringstream msg;
            int level = restorePath == "" ? LOG_ERR : LOG_INFO;
            msg << "Connection reset by peer.";
            log(level, msg.str());

            disconnect();

            if (restorePath == "") {
                throw Exception(msg.str());
            }
        }
        else if (errno != EINTR) {
            stringstream err;
            err << "read() failed: " << strerror(errno);
            log(LOG_ERR, err.str());
            throw Exception(err.str());
        }
    } else { // ret == 0
        stringstream msg;
        int level = restorePath == "" ? LOG_ERR : LOG_INFO;
        msg << "Connection closed by server.";
        log(level, msg.str());

        disconnect();

        if (restorePath == "") {
            throw Exception(msg.str());
        }
    }
}

/*****************************************************************************/

/* Write data to the TCP socket.
 */
void Process::writeSocket()
{
    int ret;

    ret = writeReady();
    if (ret < 0) {
        stringstream err;
        err << "sendData() failed.";
        log(LOG_ERR, err.str());
        throw Exception(err.str());
    } else if (!ret) {
        // No more data to send
        writeRequest = false;
    }
}

/****************************************************************************/

Common::Variable *Process::createVariable(sqlite3 *db,
                const Yaml &config)
{
    return new Variable(db, config);
}

/****************************************************************************/

/** New data to send.
 *
 * This method is called within the newData() call and signals that there are
 * new data to be sent to the process.
 *
 * When the socket is ready to send the data, writeReady() must be called.
 */
void Process::sendRequest()
{
#if 0
    cerr << __func__ << "()" << endl;
#endif
    writeRequest = true;
}

/****************************************************************************/

/** Request to send data.
 *
 * This method is called within the context of writeReady().
 *
 * \return Number of bytes that were successfully transferred.
 */
int Process::sendData(
        const char *buf, /**< Buffer with data to send. */
        size_t len /**< Number of bytes in \a buf. */
        )
{
#if DEBUG_DATA
    cerr << __func__ << "(): " << string(buf, len) << endl;
#endif
    return ::write(fd, buf, len);
}

/****************************************************************************/

/**
 */
bool Process::clientInteraction(
        const string &,
        const string &,
        const string &,
        list<ClientInteraction> &interactionList
        )
{
    list<ClientInteraction>::iterator it;

    for (it = interactionList.begin(); it != interactionList.end(); it++) {
        if (it->prompt == "Username") {
            if (!login.empty()) {
                it->response = login;
            }
        } else if (it->prompt == "Hostname") {
            char hostname[256];
            if (!gethostname(hostname, sizeof(hostname))) {
                it->response = hostname;
            }
        } else if (it->prompt == "Application") {
            it->response = "pdpersistd";
        }
    }

    return true;
}

/****************************************************************************/

void Process::sigConnected()
{
    VariableList::iterator i;
    unsigned int errors = 0;

    stringstream msg;
    msg << "Fetched variables.";
    log(LOG_INFO, msg.str());

    restoreArmed = true;

    // connect restore variable
    if (restorePath != "") {
        restoreVar = findVariable(restorePath);
        if (!restoreVar) {
            stringstream err;
            err << "Restore variable \"" << restorePath
                << "\" does not exist!";
            log(LOG_ERR, err.str());
            throw Exception(err.str());
        }

        restoreVar->subscribe(this, 0.0);
        restoreVar->poll(this);

        // set variables to idle mode until it is clear whether to restore or
        // insert.
        for (i = variables.begin(); i != variables.end(); i++) {
            Variable *v = dynamic_cast<Variable *>(*i);
            v->listenOnly();
        }
    }

    for (i = variables.begin(); i != variables.end(); i++) {
        Variable *v = dynamic_cast<Variable *>(*i);
        try {
            v->connectVariables(this);
        } catch (Variable::Exception &e) {
            stringstream err;
            err << "Variable \"" << e.path << "\": " << e.msg;
            log(LOG_WARNING, err.str());
            errors++;
        }
    }

    if (errors) {
        stringstream err;
        log(LOG_WARNING, "");
        err << "### WARNING: " << errors << " variable"
            << (errors == 1 ? " is" : "s are")
            << " not monitored. ###";
        log(LOG_WARNING, err.str());
        log(LOG_WARNING, "");
    }
}

/****************************************************************************/

void Process::processMessage(
        const PdCom::Time &time,
        LogLevel_t level,
        unsigned int messageNo,
        const std::string& message
        ) const
{
    // do not output process messages
}

/****************************************************************************/

void Process::protocolLog(
        LogLevel_t level,
        const string &message
        ) const
{
    // do not output protocol logs
}

/***************************************************************************/

void Process::notify(PdCom::Variable *v)
{
    if (v == restoreVar) {
        v->getValue(&restoreData, 1);

#if DEBUG
        cerr << __func__ << "(\"" << v->path << "\"): Changed to "
            << (int) restoreData << " armed=" << restoreArmed << endl;
#endif

        if (restoreArmed) {
            VariableList::iterator it;

            stringstream msg;
            if (restoreData == 0) {
                msg << "Restore variable is zero: Restoring data.";
            }
            else {
                msg << "Restore variable is non-zero. Data already restored.";
            }
            log(LOG_INFO, msg.str());

            for (it = variables.begin(); it != variables.end(); it++) {
                Variable *v = dynamic_cast<Variable *>(*it);

                if (restoreData == 0) {
                    v->restore();
                }
                else {
                    v->startInserting();
                }
            }

            if (restoreData == 0) {
                int nonZero = 1;
                restoreVar->setValue(&nonZero, 1);
            }
        }

        restoreArmed = false;
    }
}

/***************************************************************************/

void Process::notifyDelete(PdCom::Variable *v)
{
    if (v == restoreVar) {
        restoreVar = NULL;
    }
}

/****************************************************************************/

void Process::log(int prio, const string &msg)
{
    syslog(prio, "%s", msg.c_str());
    cerr << msg << endl;
}

/****************************************************************************/

void Process::removePidFile()
{
    int ret;

    ret = unlink(pidPath.c_str());
    if (ret == -1) {
        stringstream err;
        err << "Failed to remove PID file \"" << pidPath << "\": "
            << strerror(errno);
        log(LOG_ERR, err.str());
    }

    pidPath = "";
}

/****************************************************************************/
