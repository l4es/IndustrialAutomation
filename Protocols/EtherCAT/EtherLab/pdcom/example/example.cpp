/*****************************************************************************
 *
 * $Id$
 *
 * A sample implementation using the PdCom library.
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

#include "pdcom/Process.h"
#include "pdcom/Variable.h"
#include "pdcom/Subscriber.h"

#include <unistd.h>     // read(), getattr() tcsetattr()
#include <termios.h>    // tcgetattr() tcsetattr()
#include <stdlib.h>     // atoi()
#include <stdio.h>      // fflush(), fwrite()
#include <string>
#include <sstream>      // istringstream
#include <algorithm>    // copy()
#include <stdexcept>
#include <iostream>     // cin, cout, cerr
#include <iterator>     // ostream_iterator
#include <cstring>      // memset()
#include <arpa/inet.h>  // inet_ntop()
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <netdb.h>
#include <errno.h>
#include <map>
#include <ostream>

#ifdef SASL
#       include <sasl/sasl.h>
#       include <sasl/saslutil.h>
int sasl_init = sasl_client_init(0);
#endif

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
// Here are some helper functions. These can be ignored for the time
// being. Skip on straight to PdCom::Process subclassing
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
// Here is a template function that will print any data type to std::ostream
template <class T>
void print(std::ostream& os, T const* p)
{/*{{{*/
    os << *p;
}/*}}}*/

// Specialize the template function to print numeric value of uint8_t,
// otherwise the char will be printed which is probably not what you want
template <>
void print(std::ostream& os, uint8_t const* p)
{/*{{{*/
    os << unsigned(*p);
}/*}}}*/

// Specialize the template function to print numeric value of int8_t,
// otherwise the char will be printed which is probably not what you want
template <>
void print(std::ostream& os, int8_t const* p)
{/*{{{*/
    os << int(*p);
}/*}}}*/

// Here a template class that prints a range of numbers of any data type
// to std::ostream
template <class T>
struct Printer
{/*{{{*/
    Printer(const T* begin, const T* end):
        begin(begin), end(end) {}
    friend std::ostream& operator<<(std::ostream& os, const Printer& obj) {
        char delim = 0;
        for (const T* p = obj.begin; p != obj.end; ++p) {
            if (delim)
                os << delim;
            delim = ',';

            print(os, p);
        }

        return os;
    }

    const T* begin;
    const T* const end;
};/*}}}*/

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
// Here is a template function that will read any data type from std::istream
template <class T>
bool read(const char* input, T* p, size_t nelem)
{/*{{{*/
    std::istringstream is(input);

    do {
        is >> *p++;
        if (!--nelem)
            return is.fail();
        is.ignore(1);
    } while (true);
}/*}}}*/

// Specialize the template function from read numeric value of uint8_t,
// otherwise the char will be read which is probably not what you want
template <>
bool read(const char* input, uint8_t* p, size_t nelem)
{/*{{{*/
    std::istringstream is(input);
    unsigned val;

    do {
        is >> val;
        *p++ = val;

        if (!--nelem)
            return is.fail();
        is.ignore(1);
    } while (true);
}/*}}}*/

// Specialize the template function from read numeric value of int8_t,
// otherwise the char will be read which is probably not what you want
template <>
bool read(const char* input, int8_t* p, size_t nelem)
{/*{{{*/
    std::istringstream is(input);
    int val;

    do {
        is >> val;
        *p++ = val;

        if (!--nelem)
            return is.fail();
        is.ignore(1);
    } while (true);
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int usage(const char* progname)
{/*{{{*/
    std::cout << "Usage: " << progname
        << " addr"
        << " <command> <command arguments>"
        << std::endl
        << "Arguments:" << std::endl
        << "    addr:     server<:port> (default port = 2345)" << std::endl
        << "    command:  (query, login, set, list, find, stream)" << std::endl
        << std::endl
        << " query <path>" << std::endl
        << "    Query a process variable" << std::endl
        << std::endl
        << " login <user>" << std::endl
        << "    Login to the server" << std::endl
        << std::endl
        << " set <path> <value>" << std::endl
        << "    Set a process parameter value" << std::endl
        << std::endl
        << " find <path>" << std::endl
        << "    Find a single variable. " << std::endl
        << std::endl
        << " list <path>" << std::endl
        << "    List process variables in path. " << std::endl
        << "        empty|'':  List all process variables" << std::endl
        << "        '/'     :  List root directory" << std::endl
        << "        '/dir'  :  List directory /dir" << std::endl
        << std::endl
        << " stream <count> <decimation> <path>+" << std::endl
        << "    Subscribe to a variable/s with decimation" << std::endl
        << "    Exit after <count> data has arrived" << std::endl
        << std::endl;
    return 0;
}/*}}}*/


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
// First step is to subclass PdCom::Process
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//
// PdCom::Process does not communicate directly with the server. It relies on
// the user to subclass it and to reimplement read(), write() and flush()
// methods for its communication.  Thus it is the users responsiblity to
// manage the network connection.
//
// Furthermore, the processMessage() and protocolLog() can be reimplemented
// to receive emergency messages from the server and protocol stack
// respectively.
// 
// When the connection is established, PdCom::Process::connect() must be
// called to initialize communication with the server.
//
// Once initialized(), the methods login(), list() and find() can
// be used to discover the real time task running on the server
//

struct Process: PdCom::Process
{/*{{{*/
    int sfd;            // File descriptor (for unbuffered reading)
    FILE *stream;       // File stream of fd (for buffered writing)
    bool init;
    bool busy;

    std::string localIP;
    std::string remoteIP;
    std::string serverFQDN;
    bool loginFinished;
    bool loginSuccess;
    const char* sasl_data;
    const PdCom::Variable* foundVariable;

    Process()
    {
        init = false;
        loginFinished = false;
        foundVariable = 0;
    }

    // Need to reimplement read()
    // This should place at most n characters to buf from the stream
    int read(       char *buf, size_t n)
    {
        return ::read(sfd, buf, n);
    }

    // Need to reimplement write()
    // This must put n characters from buf to the stream
    void write(const char *buf, size_t n)
    {
        ::fwrite(buf, 1, n, stream);
    }

    // Need to reimplement flush()
    // Flush stream, in case it is buffered (which it should be!)
    void flush()
    {
        ::fflush(stream);
    }

    // Need to reimplement connected()
    void connected()
    {
        init = true;
    }

#ifdef SASL
    /*{{{*/
    ///////////////////////////////////////////////////////////////////////////
    // man: sasl_getopt_t
    // SASL options
    std::map<std::string, std::string> sasl_option;
    static int cb_getopt(void *context, const char *plugin_name,
            const char *option, const char **result, unsigned *len)
    {
        Process* process = reinterpret_cast<Process*>(context);
        printf("%s(context=%p, plugin_name=%s, option=%s, result=%p len=%p)\n",
                __func__, context, plugin_name, option, result, len);

        std::string& opt = process->sasl_option[option];
        std::getline(std::cin, opt);
        if (!opt.empty()) {
            *result = opt.c_str();
            if (len)
                *len = opt.size();
            return SASL_OK;
        }

        return SASL_BADPARAM;
    }

    ///////////////////////////////////////////////////////////////////////////
    // man: sasl_getsimple_t
    static int cb_simple(void *context, int id,
            const char **result, unsigned *len)
    {
        Process* process = reinterpret_cast<Process*>(context);
        printf("%s, context=%p, id=%X, result=%p, len=%p\n",
                __func__, context, id, result, len);

        const char* option;
        switch (id) {
            case SASL_CB_USER:
                option = "userid";
                break;

            case SASL_CB_AUTHNAME:
                option = "userid";
                break;

            case SASL_CB_LANGUAGE:
                option = "language";
                *result = 0;
                break;

            default:
                return SASL_BADPARAM;
        }

        //*result = 0;        // User cancel
        std::cout << option << ": " << std::flush;
        std::string& opt = process->sasl_option[option];
        std::getline(std::cin, opt);
        if (opt.size()) {
            *result = opt.c_str();
            if (len)
                *len = opt.size();
        }

        return SASL_OK;
    }

    ///////////////////////////////////////////////////////////////////////////
    // man: sasl_getsecret_t
    char pass_buffer[100];
    static int cb_getsecret(sasl_conn_t* /*conn*/, void *context, int id,
            sasl_secret_t **psecret)
    {
        Process* process = reinterpret_cast<Process*>(context);
        printf("%s, context=%p, id=%i, psecret=%p\n",
                __func__, context, id, psecret);

        sasl_secret_t *secret = 
            reinterpret_cast<sasl_secret_t*>(process->pass_buffer);

        std::cout << "secret: " << std::flush;
        std::string str;
        std::getline(std::cin, str);
        std::copy(str.begin(), str.end(), (char*)secret->data);
        secret->len = str.size();

        *psecret = secret;

        return SASL_OK;
    }

    ///////////////////////////////////////////////////////////////////////////
    // man: sasl_chalprompt_t
    static int cb_chalprompt(void *context, int id,
            const char *challenge,
            const char *prompt, const char *defresult,
            const char **result, unsigned *len)
    {
        Process* process = reinterpret_cast<Process*>(context);
        printf("%s\n", __func__);
        *result = 0;        // User cancel
        std::cout << challenge << std::endl;
        std::cout << prompt << " [" << defresult << "]: " << std::flush;

        const char* option;
        switch (id) {
            case SASL_CB_ECHOPROMPT:
                option = "chalprompt";
                break;

            case SASL_CB_NOECHOPROMPT:
            default:
                option = "chalnoprompt";
                break;
        };

        std::string& opt = process->sasl_option[option];
        std::getline(std::cin, opt);
        if (opt.size()) {
            *result = opt.c_str();
            if (len)
                *len = opt.size();
        }


        return SASL_OK;
    }

    ///////////////////////////////////////////////////////////////////////////
    // man: sasl_getrealm_t
    static int cb_getrealm(void* /*context*/, int /*id*/,
            const char** /*availrealms*/, const char** result)
    {
        //Process* process = reinterpret_cast<Process*>(context);
        printf("%s\n", __func__);
    //    debugFunction(Debug::make_pair("context",context)
    //            << Debug::make_pair("id", id));
                //<< Debug::make_pair("availrealms", _r));

        *result = 0;

        return SASL_OK;
    }

    /*}}}*/
#endif //SASL

    ///////////////////////////////////////////////////////////////////////////
    void login()
    {/*{{{*/
#ifdef SASL
        typedef int (*cb_t)(void);
        sasl_callback_t cb[] = {
            //{SASL_CB_GETOPT,        0,  this},
            {SASL_CB_USER,          0,  this},
            {SASL_CB_AUTHNAME,      0,  this},
            {SASL_CB_LANGUAGE,      0,  this},
            {SASL_CB_PASS,          0,  this},
            {SASL_CB_ECHOPROMPT,    0,  this},
            {SASL_CB_NOECHOPROMPT,  0,  this},
            {SASL_CB_GETREALM,      0,  this},
            {SASL_CB_LIST_END, 0, 0},
            {SASL_CB_GETOPT,        cb_t(cb_getopt),    this},
            {SASL_CB_USER,          cb_t(cb_simple),    this},
            {SASL_CB_AUTHNAME,      cb_t(cb_simple),    this},
            {SASL_CB_LANGUAGE,      cb_t(cb_simple),    this},
            {SASL_CB_PASS,          cb_t(cb_getsecret), this},
            {SASL_CB_ECHOPROMPT,    cb_t(cb_chalprompt), this},
            {SASL_CB_NOECHOPROMPT,  cb_t(cb_chalprompt), this},
            {SASL_CB_GETREALM,      cb_t(cb_getrealm),  this},
            {SASL_CB_LIST_END, 0, 0},
        };

        if (sasl_init != SASL_OK)
            return;

        std::cout
            << "Creating new client instance: sasl_client_new()"
            << std::endl;
        sasl_conn_t *conn = NULL;
        int result = sasl_client_new("pdserv",
                serverFQDN.c_str(),
                localIP.c_str(),
                remoteIP.c_str(),
                cb,
                SASL_SUCCESS_DATA,
                &conn);
        if (result != SASL_OK)
            return;

        std::cout
            << "Waiting for mechanism list from server..."
            << std::flush;
        PdCom::Process::login(0,0);
        sasl_data = 0; while (!sasl_data) asyncData();
        std::cout << sasl_data << std::endl;

        std::cout
            << "Starting sasl client: sasl_client_start()"
            << std::endl;
        sasl_interact_t *prompt = 0;
        const char* clientout = 0, *mech;
        unsigned int clientoutlen;
        do {
            result = sasl_client_start(conn, sasl_data,
                    &prompt, &clientout, &clientoutlen, &mech);
            std::cout << "sasl_client_start() result(" << __LINE__ << ")="
                << sasl_errstring(result,0,0) << std::endl;

            if (result == SASL_INTERACT) {
                std::cout << "sasl_client_start() needs data:" << std::endl
                    << prompt->challenge
                    << ' ' << prompt->prompt
                    << " [" << prompt->defresult << "] :" << std::flush;
                std::string& opt =
                    sasl_option[std::string(prompt->challenge) + prompt->prompt];
                std::getline(std::cin, opt);
                prompt->result = opt.c_str();
                prompt->len = opt.size();
            }
        } while (result == SASL_INTERACT);

        if (mech)
            std::cout << "Using mechanism: " << mech << std::endl;

        while (mech or clientout) {
            unsigned int len = (clientoutlen+2)/3*4+1;
            char data[len];

            result = sasl_encode64(clientout, clientoutlen, data, len, &len);
            std::cout << "sasl_encode64() result(" << __LINE__ << ")="
                << sasl_errstring(result,0,0) << std::endl;
            if (clientout and clientoutlen) {
                std::cout << "Data to server:"
                    << std::string(clientout, clientoutlen)
                    << " (" << data << ')' << std::endl;
            }

            PdCom::Process::login(mech, data);
            mech = 0;

            sasl_data = 0; while (!(sasl_data or loginFinished)) asyncData();

            if (!sasl_data)
                break;

            len = strlen(sasl_data);
            unsigned int serveroutlen = (len+3)/4*3+1;
            char serverout[serveroutlen];
            result = sasl_decode64(sasl_data, len,
                    serverout, serveroutlen, &serveroutlen);
            std::cout << "sasl_decode64() result(" << __LINE__ << ")="
                << sasl_errstring(result,0,0) << std::endl;

            prompt = 0;
            do {
                result = sasl_client_step(conn,
                        serverout, serveroutlen,
                        &prompt, &clientout, &clientoutlen);
                std::cout << "sasl_client_step() result(" << __LINE__ << ")="
                    << sasl_errstring(result,0,0) << std::endl;

                if (result == SASL_INTERACT) {
                    for (sasl_interact_t *p = prompt; p->id != SASL_CB_LIST_END; p++) {
                        std::cout << "sasl_client_start() needs data:" << std::endl
                            << p->challenge;

                        if (p->prompt)
                            std::cout << ' ' << p->prompt;

                        if (p->defresult)
                            std::cout << " [" << p->defresult << "] :";

                        std::cout << std::flush;

                        std::string key;
                        key += p->challenge;
                        key += p->prompt;

                        std::string& opt = sasl_option[key];
                        std::getline(std::cin, opt);
                        p->result = opt.c_str();
                        p->len = opt.size();
                    }
                }
            } while (result == SASL_INTERACT);
        }

        if (!loginFinished)
            std::cout << "Hmm, problem here!!!!!!!!!!!!!!" << std::endl;

        if (result != SASL_OK)
            std::cout << "Result is not SASL_OK!!!!!!" << std::endl;

        if (loginSuccess) {
            std::cout << "YAY!!!!!!!!! Logged in" << std::endl;
        }
        else
            std::cout << "NAH, Login unsuccessful" << std::endl;
#endif
    }/*}}}*/

    void loginReply(const char* serverData, bool finished, bool success)
    {/*{{{*/
#ifdef SASL
        sasl_data = serverData;
        loginFinished = finished;
        loginSuccess = success;
        std::cout << __func__ << __LINE__;
        if (serverData)
            std::cout << " serverdata=" << serverData;
        std::cout
            << " finished=" << finished
            << " success=" << success << std::endl;
//        switch (state) {
//            case 0:
//                loginFinished = true;
//                break;
//
//            case 1:
//                {
//                    std::cout << "Choose mechanism: " << reply << std::cout;
//
//                    std::string mechanism;
//                    std::cin >> mechanism;
//
//                    loginStep(mechanism);
//                }
//                break;
//
//            case 2:
//            case 3:
//                {
//                    struct termios termios;
//
//                    std::cout << "Q: " << reply
//                        << " [" << defaultReply << "]: ";
//
//                    if (state == 3) {
//                        tcgetattr(STDIN_FILENO, &termios);
//                        struct termios termios_noecho = termios;
//
//                        termios_noecho.c_lflag &= ~ECHO | ECHONL;
//                        tcsetattr(STDIN_FILENO, TCSAFLUSH, &termios_noecho);
//                    }
//
//                    std::string answer;
//                    std::cin >> answer;
//
//                    if (state == 3)
//                        tcsetattr(STDIN_FILENO, TCSANOW, &termios);
//
//                    loginStep(answer);
//                }
//                break;
//
//            case 4:
//                loginFinished = true;
//                break;
//        }
#endif
    }/*}}}*/

    void findReply(const PdCom::Variable* var)
    {
        busy = false;
        foundVariable = var;
        if (var) {
            std::cout << "Found the following variables: " << std::endl;
            std::cout << var->path() << std::endl;
        }
        else
           std::cout << "Could not find variable" << std::endl;
    }

    void listReply(
            std::list<const PdCom::Variable*>& variableList,
            std::list<std::string>& directoryList)
    {
        busy = false;
        std::cout << "Found the following variables: " << std::endl;
        while (variableList.size()) {
            const PdCom::Variable *v = variableList.front();
            variableList.pop_front();
            std::cout << v->path() << std::endl;
        }

        std::cout << "Found the following directories: " << std::endl;
        std::copy(directoryList.begin(), directoryList.end(),
                std::ostream_iterator<std::string>(std::cout, "/\n"));

    }

    // Open a network socket to the server
    // Argument:
    //          url: String of the form "host:port"
    //
    // Essentially this method prepares a file discriptor for receiving
    // data from the server and a file stream for sending commands to the
    // server.
    // When successful, PdCom::Process::connect() is called to initialise
    // the library
    int connect(const std::string& addr)
    {/*{{{*/
        std::istringstream is(addr);
        std::string host, port;
        struct addrinfo hints;
        struct addrinfo *result, *rp;
        int s;
        int rv;

        // Get host part of addr
        if (is.peek() == '[') {
            // IPv6 style address
            is.get();
            std::getline(is, host, ']');
            if (is.peek() == ':')
                is.get();
        }
        else {
            std::getline(is, host, ':');
        }

        if (is.fail())
            return -EINVAL;

        // Get port part of addr
        if (is.eof())
            port = "2345";
        else if (std::getline(is, port).fail())
            return -EINVAL;

        /* Obtain address(es) matching host/port */

        ::memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
        hints.ai_socktype = SOCK_STREAM; /* TCP socket */
        hints.ai_flags = 0;
        hints.ai_protocol = 0;          /* Any protocol */

        s = ::getaddrinfo(host.c_str(), port.c_str(), &hints, &result);
        if (s != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", ::gai_strerror(s));
            return -EINVAL;
        }

        /* getaddrinfo() returns a list of address structures.
           Try each address until we successfully connect(2).
           If socket(2) (or connect(2)) fails, we (close the socket
           and) try the next address. */

        for (rp = result; rp != NULL; rp = rp->ai_next) {
            sfd = ::socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            if (sfd == -1)
                continue;

            if (::connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1) {

                // Set server FQDN
                char host[NI_MAXHOST], service[NI_MAXSERV];
                int rv = getnameinfo(rp->ai_addr, rp->ai_addrlen,
                        host, NI_MAXHOST,
                        service, NI_MAXSERV,
                        NI_NUMERICSERV);
                if (rv) {
                    std::cerr << gai_strerror(rv) << std::endl;
                }
                else {
                    serverFQDN = host;
                }

                // Set remote IP
                char ipaddr[INET6_ADDRSTRLEN];
                const void *field = 0;
                switch (rp->ai_family) {
                    case AF_INET:
                        field = &((struct sockaddr_in*)rp->ai_addr)->sin_addr;
                        break;
                    case AF_INET6:
                        field = &((struct sockaddr_in6*)rp->ai_addr)->sin6_addr;
                        break;
                }
                if (field and inet_ntop(rp->ai_family, field, 
                            ipaddr, INET6_ADDRSTRLEN)) {
                    remoteIP = ipaddr;
                    remoteIP.append(1,';');
                    remoteIP.append(service);
                }

                // Set local IP
                struct sockaddr_storage localaddr;
                socklen_t len = sizeof(localaddr);
                uint16_t port = 0;
                field = 0;
                if (!getsockname(sfd, (struct sockaddr*)&localaddr, &len)) {
                    switch (rp->ai_family) {
                        case AF_INET:
                            { 
                                struct sockaddr_in* in =
                                    (struct sockaddr_in*)&localaddr;
                                port = htons(in->sin_port);
                                field = &in->sin_addr;
                            }
                            break;

                        case AF_INET6:
                            { 
                                struct sockaddr_in6* in6 =
                                    (struct sockaddr_in6*)&localaddr;
                                port = htons(in6->sin6_port);
                                field = &in6->sin6_addr;
                            }
                            break;
                    }
                }
                if (field and inet_ntop(rp->ai_family, field, 
                            ipaddr, INET6_ADDRSTRLEN)) {
                    std::ostringstream os;
                    os << ipaddr << ';' << port;
                    localIP = os.str();
                }
                std::cout << serverFQDN << ' '
                    << remoteIP << ' ' << localIP << std::endl;

                break;                  /* Success */
            }

            ::close(sfd);
        }

        freeaddrinfo(result);

        if (rp == NULL) {               /* No address succeeded */
            fprintf(stderr, "Could not connect\n");
            return -ENODEV;
        }

        if (!(stream = ::fdopen(sfd, "r+"))) {
            rv = errno;
            ::perror("fdopen()");
            return rv;
        }

        // Finally call PdCom::Process::connect()
        while (!init)
            if ((rv = asyncData()) < 0)
                return rv;

        return 0;
    }/*}}}*/
};/*}}}*/

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
// Now that a general Process is defined, here are typical usage cases.
//
// Once a command is issued, process.asyncData() needs to be called to fetch
// data from the socket and parse it.
//
// PdCom uses a completely asynchronous communication model. Calls to Process
// either return immediately or result in a message being sent to the server
// upon which the call returns immediately.
//
// In a typical interactive application, you will run in an endless event
// loop, checking the socket for input data and then call asyncData().
//
// Or you have a single threaded application like a logger that subscribes to
// variables and then logs these, you can even call asyncData() and then block
// on the network socket read() function, just like the examples shown here.
//
// Since the examples shown here must return some time, an internal flag
// Process::busy is used to exit the loop.

///////////////////////////////////////////////////////////////////////////
// PdCom::Process::list()
//
// The PdCom::Process::list() call requires a path string and returns
// a set of strings of variable or directory paths.
//
// This includes variables, such as when a vector is split into its elements.
int list(Process& process, int argc, const char* const* argv)
{/*{{{*/
    // The PdCom::Process::list() call returns a set of strings
    process.list(argc ? *argv : std::string());
    process.busy = true;
    while (process.busy)
        process.asyncData();

    return argc > 0;
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
// PdCom::Process::find()
//
// The PdCom::Process::find() call requires a path string and returns
// a single variable immediately if it is found. If it has to query
// the server, it is returned in listReply as a single variable in
// @variableList
int find(Process& process, int argc, const char* const* argv)
{/*{{{*/
    // The PdCom::Process::list() call returns a set of strings
    if (argc and *argv) {
        if (!process.find(*argv)) {
            process.busy = true;
            while (process.busy)
                process.asyncData();
        }
    }

    return argc > 0;
}/*}}}*/

// Here is the call to login, which uses the LoginRequestHandler defined
// above
int login(Process& process, int /*argc*/, const char* const* /*argv*/)
{/*{{{*/

    process.login();


    return 0;
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
// PdCom::Process::find() and PdCom::Variable::getValue()
//
// find() searches for a variable given a path. If successful, the
// PdCom::Variable pointer is returned.
//
// With the pointer, various aspects can be queried:
//      - data type
//      - sample time
//      - dimensions
//      - number of elements
//      - bytes needed to store the value
//
// The variable pointer also allows synchronous fetching and modification
// of the value.
//
// There are two basic ways of getting the process value:
//      - string representation:
//         - v->getStringValue(delimiter)
//           Create a std::string, using delimiter to separate elements for
//           non-scalar variables
//
//         - std::ostream << PdCom::Variable::iostream(v,delimiter)
//           Send variable to a stream. This gives more control
//           over printing of double and float variables.
//
//           An optional delimiter character can be supplied when calling
//           PdCom::Variable::iostream(), which is used to separate values
//           when printing lists.
//
//      - numeric representation
//         - v->getValue(buffer)
//           where: buffer is any typed buffer with enough memory to hold the
//                  variable. Use v->nelem to find out how many elements the
//                  variable has. The data is implicitly converted to buffer's
//                  type.
//
//         Fetches the value into the supplied buffer casting the
//         value into the buffer's data type.
//
// Note that the value is fetched from the process for every call!
//
int query(Process& process, int argc, const char* const* argv)
{/*{{{*/
    // First define a Subscriber
    struct Subscriber: PdCom::Subscriber {
        Subscriber() {
            notvalid = false;
            subscription = 0;
            time_ns = 0;
        }

        void newValue(const PdCom::Variable::Subscription* s) {
            this->time_ns = **s->time_ns;
            printf("1 new values %zu %p\n", time_ns, s);
        };

        void newGroupValue(uint64_t time_ns) {
            this->time_ns = time_ns;
            printf("2 new values %zu %p\n", time_ns, subscription);
        };

        void active(const std::string& /*path*/,
                const PdCom::Variable::Subscription* s) {
            subscription = s;
        }

        void invalid(const std::string& path, int id) {
            std::cout << "Could not find " << path
                << " with request id "  << id << std::endl;
            notvalid = true;
        }

        bool notvalid;
        const PdCom::Variable::Subscription* subscription;
        uint64_t time_ns;
    };

    if (argc < 1)
        return 0;

    Subscriber subscriber;
    process.subscribe(&subscriber, *argv, -1, 25);

    while (!subscriber.notvalid and !subscriber.subscription
            and !subscriber.time_ns and process.asyncData() > 0);

    if (subscriber.subscription) {
        // Print some attributes of the variable
        const PdCom::Variable* var = subscriber.subscription->variable;
        std::cout
            << "name: " << var->name() << std::endl
            << "path: " << var->path() << std::endl
            << "sample time: " << var->sampleTime << std::endl;

        if (!subscriber.time_ns and subscriber.subscription->poll())
            std::cout << "Cannot poll "
                << subscriber.subscription->variable->path() << std::endl;

        while (!subscriber.time_ns and process.asyncData() > 0);

        // Query the variable's value returning a string
        std::cout << "getString value = "
            << subscriber.subscription->getStringValue(',') << std::endl;

        // Query the variable's value and writing it directly to std::ostream.
        // Almost the same as getStringValue(), except that the user has more
        // control over printing of double and float variables by modifying
        // the stream flags
        std::cout << "  ostream value = "
            << *subscriber.subscription << std::endl;

        // Implicit boolean conversion
        {
            bool val[var->nelem];
            subscriber.subscription->getValue(val, 0, var->nelem);
            std::cout << "  boolean value = "
                << Printer<bool>(val, val + var->nelem) << std::endl;
        }

        // Implicit uint16_t conversion
        {
            uint16_t val[var->nelem];
            subscriber.subscription->getValue(val, 0, var->nelem);
            std::cout << " uint16_t value = "
                << Printer<uint16_t>(val, val + var->nelem) << std::endl;
        }

        // Implicit float conversion
        {
            float val[var->nelem];
            subscriber.subscription->getValue(val, 0, var->nelem);
            std::cout << "    float value = "
                << Printer<float>(val, val + var->nelem) << std::endl;
        }
    }

    process.unsubscribe(&subscriber);

    return 1;
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
// PdCom::Variable::setValue()
//
// Similar to getting the value of a variable, PdCom::Variable has 2 generic
// methods for changing the value of a parameter:
//      - string representation:
//         - v->setStringValue(string, delimiter)
//           Set the parameter using a std::string value, using
//           delimiter to separate elements for non-scalar variables
//
//         - std::istream >> PdCom::Variable::iostream(v,delimiter)
//           Set the variable using std::istream. This gives more control
//           over printing of double and float variables.
//
//           An optional delimiter character can be supplied when calling
//           PdCom::Variable::iostream(), which is used to separate values
//           when reading from lists.
//
//      - numeric representation
//         - v->setValue(buffer)
//           where: buffer is any typed buffer with enough elements.
//                  The data is implicitly converted to buffer's type.
//
//         Sets the value of a parameter from any data type, implicitly
//         converting to the variable's data type.
//
// The string and iostream methods have the advantage of implicitly using
// the variable's native data type when the values are available as strings.
// 
// The numeric methods have the advantage of being able to be interpreted
// programatically.
//
int set(Process& process, int argc, const char* const* argv)
{/*{{{*/
    if (argc < 2)
        return argc;

    process.foundVariable = 0;
    if (!process.find(*argv)) {
        process.busy = true;
        while (process.busy)
            process.asyncData();
    }

    const PdCom::Variable* v = process.foundVariable;

    if (v) {
        {
            // Read the values into a double vector and use it to set
            // the variable's value
            double data[v->nelem];
            if (read(argv[1], data, v->nelem)
                    or v->setValue(data, 0, v->nelem))
                std::cout << "Error: could not parse " << argv[1]
                    << " as a double list with " << v->nelem
                    << " elements" <<  std::endl;
        }

        // Set the value from a std::string, using the variable's native data
        // type when interpreting the string.
        if (v->setStringValue(argv[1]))
            std::cout << "Error: could not parse " << argv[1]
                << " as a " << v->ctype << " list with " << v->nelem
                << " elements" <<  std::endl;
    }
    else
        std::cout << *argv << ": not found" << std::endl;

    return 2;
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
// Subscriber() and Process::asyncData()
int stream(Process& p, const int argc, const char* const* argv)
{/*{{{*/
    struct Subscriber: PdCom::Subscriber
    {/*{{{*/
        Subscriber(): count(0)
        {
        }

        // Reimplemented from PdCom::Subscriber
        // Called when any of the subscriptions have new data
        void newValue(const PdCom::Variable::Subscription* subscription)
        {
            std::cout << **subscription->time_ns << ": ";

            std::cout << subscription->variable->path()
                << " = " << subscription->getStringValue() << std::endl;

            count++;
        }

        // Reimplemented from PdCom::Subscriber
        // Called when any of the subscriptions have new data
        void newGroupValue(uint64_t time_ns)
        {
            std::cout << time_ns << ": ";

            for (SubscriptionList::iterator it = list.begin();
                    it != list.end(); ++it) {
                SubscriptionList::value_type& subscription = *it;
                std::cout << subscription->variable->path()
                    << " = " << subscription->getStringValue() << std::endl;
            }

            count++;
        }

        // Reimplemented from PdCom::Subscriber
        // Called when a PdCom::Variable::Subscription corresponding to a
        // PdCom::Process::Request is active
        void active(const std::string& path,
                const PdCom::Variable::Subscription* s)
        {
            std::cout << "Subscription " << path
                << " is active" << std::endl;
            list.push_back(s);
        }

        // Reimplemented from PdCom::Subscriber
        // Called when a PdCom::Process::Request is invalid because no
        // variable with the requested path exists
        void invalid(const std::string& path, int id)
        {
            std::cout << "Request path " << path << " with id " << id
                << " does not exist" << std::endl;

        }

        int count;

        typedef std::list<const PdCom::Variable::Subscription*>
            SubscriptionList;
        SubscriptionList list;
    };/*}}}*/

    if (argc < 3)
        return argc;

    // First argument is the how many times newValues should be called
    int n = ::atoi(argv[0]);

    // Second argument is the interval
    double interval = ::atof(argv[1]);

    {
        // Create a new Subscriber
        Subscriber subscriber;

        // The rest are paths to subscribe
        for (int i = 2; i < argc; ++i)
            p.subscribe(&subscriber, argv[i], interval, i);

        // Call asyncData() as often as requested
        while (subscriber.count < n)
            p.asyncData();

        p.unsubscribe(&subscriber);
    }

    return argc;
}/*}}}*/

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
// main() function
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
    if (argc < 2)
        return usage(argv[0]);

    Process p;
    int rv;

    if ((rv = p.connect(argv[1])))
        return rv;

    typedef int (*command_T)(Process& p, int argc, const char* const* argv);
    command_T command = 0;
    int it = 2;
    while (it != argc) {
        if (!strcmp(argv[it], "query"))
            command = query;
        else if (!strcmp(argv[it], "login"))
            command = login;
        else if (!strcmp(argv[it], "set"))
            command = set;
        else if (!strcmp(argv[it], "list"))
            command = list;
        else if (!strcmp(argv[it], "find"))
            command = find;
        else if (!strcmp(argv[it], "stream"))
            command = stream;
        else {
            std::cerr << "Unknown command " << argv[it] << std::endl;
            usage(argv[0]);
            return 0;
        }

        ++it;

        if (command)
            it += command(p, argc - it, argv + it);
    }

    return 0;
}
