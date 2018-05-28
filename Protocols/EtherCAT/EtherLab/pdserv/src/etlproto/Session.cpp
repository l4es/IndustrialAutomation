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

#include "Session.h"
#include "../Main.h"
#include "../Variable.h"
#include "../Signal.h"

#include <iostream>
#include <limits>

#include <iomanip>
using std::cout;
using std::cerr;
using std::endl;

using namespace EtlProto;

/////////////////////////////////////////////////////////////////////////////
Session::Session( ost::SocketService *ss,
        ost::TCPSocket &socket, PdServ::Main *main):
    SocketPort(0, socket), main(main),
    signals(main->getSignals()), crlf("\r\n"),
    signal_ptr_start(0) // FIXME main->getSignalPtrStart())
{
    std::ostream s(this);
    setCompletion(false);

    s << "PdCom Server v1.0" << crlf
        << "Name: " << main->name << crlf
        << "Version: " << main->version << crlf
        << "Sampletime: " << main->baserate << crlf
        << "TaskDecimations: 1";
    for (unsigned int i = 0; i < main->nst - 1; i++) {
        s << "," << main->decimation[i];
    }
    s << crlf << std::flush;

    attach(ss);

    xmlchar = 0;
    xmlcharlen = 0;
    encoding = xmlFindCharEncodingHandler("ISO-8859-1");

    signal_ptr = signal_ptr_start;
    subscribed.resize(main->nst);
    dataOffset.resize(signals.size());
    decimation.resize(signals.size());
    dirty.resize(main->nst);
    //subscribers.resize(main->nst);

    expired();
}

/////////////////////////////////////////////////////////////////////////////
int Session::sync()
{
    if (!buf.empty())
        setDetectOutput(true);
    return 0;
}

/////////////////////////////////////////////////////////////////////////////
int Session::overflow(int c)
{
    buf.append(1,c);
    return c;
}

/////////////////////////////////////////////////////////////////////////////
std::streamsize Session::xsputn ( const char * s, std::streamsize n )
{
    buf.append(s,n);
    return n;
}

/////////////////////////////////////////////////////////////////////////////
void Session::expired()
{
    cout << __func__ << __LINE__ << endl;

    while (*signal_ptr) {
        cout << __func__ << ' ' << *signal_ptr << endl;
        if (*signal_ptr == PdServ::Main::Restart) {
            signal_ptr = signal_ptr_start;
            continue;
        }

        const size_t blockLen = signal_ptr[1];

        switch (*signal_ptr) {
            case PdServ::Main::SubscriptionList:
                {
                    size_t headerLen = 4;
                    size_t n = blockLen - headerLen;
                    unsigned int tid = signal_ptr[2];
                    //unsigned int decimation = signal_ptr[3];
                    size_t offset = 0;

                    cout << "Main::SubscriptionList " << tid << ' ' << n << endl;

                    for (unsigned int i = 0; i < n; i++) {
                        size_t sigIdx = signal_ptr[i + headerLen];
                        PdServ::Signal *s = signals[sigIdx];

                        dataOffset[sigIdx] = offset;
                        offset += s->memSize;

                        for (std::set<size_t>::iterator it = signalDecimation[s].begin();
                                it != signalDecimation[s].end(); it++) {
                            subscribed[tid][*it].insert(s);
                            dirty[tid][*it] = true;
                        }
                    }
                }
                break;

            case PdServ::Main::SubscriptionData:
                {
                    unsigned int tid = signal_ptr[2];
                    //unsigned int iterationNo = signal_ptr[3];
                    char *dataPtr = reinterpret_cast<char*>(&signal_ptr[4]);

                    struct timespec time =
                        *reinterpret_cast<struct timespec*>(dataPtr);
                    dataPtr += sizeof(struct timespec);

                    cout << "Main::SubscriptionData " << tid << endl;

                    for (DecimationMap::iterator it = decimation[tid].begin();
                            it != decimation[tid].end(); it++) {
                        cout << tid << ' ' << it->first << ' ' << it->second << endl;
                        if (--it->second)
                            continue;

                        it->second = it->first;

                        std::ostream s(this);

                        if (dirty[tid][it->first]) {
                            s << "VARIABLELIST " << tid
                                << ' ' << it->first
                                << ' ' << subscribed[tid][it->first].size()
                                << crlf;
                            for (std::set<PdServ::Signal*>::iterator it2 =
                                    subscribed[tid][it->first].begin();
                                    it2 != subscribed[tid][it->first].end();
                                    it2++) {
                                PdServ::Variable *v = *it2;

                                s << v->index;
                                if (sent[v]) {
                                    s << crlf;
                                    continue;
                                }
                                sent[v] = true;

                                s << ' ' << v->decimation
                                    << ' ' << getDTypeName(v->dtype)
                                    << ' ' << v->ndims;

                                const size_t *dim = v->getDim();
                                for (size_t i = 0; i < v->ndims; i++) {
                                    s << ' ' << dim[i];
                                }
                                s << ' ' << v->alias
                                    << ' ' << v->path
                                    << crlf;

                            }
                            dirty[tid][it->first] = false;
                        }

                        s << "TID " << tid
                            << ' ' << it->first
                            << ' ' << time.tv_sec
                            << ' ' << time.tv_nsec
                            << crlf << std::flush;

                        for (std::set<PdServ::Signal*>::iterator it2 =
                                subscribed[tid][it->first].begin();
                                it2 != subscribed[tid][it->first].end();
                                it2++) {
                            printVariable(this, *it2,
                                    dataPtr + dataOffset[(*it2)->index]);
                        }
                    }
                }
                break;
        }

        signal_ptr += blockLen;
    }

    sync();
    setTimer(100);
}

/////////////////////////////////////////////////////////////////////////////
void Session::pending()
{
    cout << __func__ << endl;
    char buf[1024];

    ssize_t n = receive(buf, sizeof(buf));
    if (n <= 0) {
        delete this;
        return;
    }

    inbuf.append(buf,n);
    size_t bufptr = 0;
    bool cont = true;

    while (cont) {
        cout << "Command " << inbuf.substr(bufptr)  << endl;
        switch (state) {
            case Idle:
                {
                    size_t eol = inbuf.find('\n', bufptr);

                    if (eol == inbuf.npos) {
                        cont = false;
                    }
                    else {
                        std::string instruction(inbuf, bufptr, eol - bufptr);
                        state = ParseInstruction(instruction);
                        bufptr = eol + 1;
                    }

                    break;
                }
        }
    }

    inbuf.erase(0,bufptr);
}

/////////////////////////////////////////////////////////////////////////////
Session::ParseState_t Session::ParseInstruction(const std::string& s)
{
    std::istringstream is(s);
    std::string instruction;

    is >> instruction;

    if (!instruction.compare("ls")) {
        list();
    }
    else if (!instruction.compare("set")) {
    }
    else if (!instruction.compare("subscribe")) {
        unsigned int n;
        std::string path;

        is >> n;
        while (isspace(is.peek()))
            is.ignore(1);

        std::getline(is, path);

        if (!is.fail()) {
//            size_t sigIdx = main->subscribe(path);
//            if (sigIdx != ~0U) {
//                PdServ::Signal *signal = (main->getSignals())[sigIdx];
//                unsigned int tid = signal[sigIdx].tid;
//                signalDecimation[signal].insert(n);
//
//                if (decimation[tid].find(n) == decimation[tid].end()) {
//                    // FIXME: This has to be reworked to fit into the the
//                    // other decimations sometime ;)
//                    decimation[tid][n] = 1;
//                }
//            }
        }
    }

    return Idle;
}

/////////////////////////////////////////////////////////////////////////////
void Session::subscribe(const std::string& path, unsigned int decimation)
{
}

/////////////////////////////////////////////////////////////////////////////
xmlChar* Session::utf8(const std::string &s)
{
    int inlen, outlen;

    inlen = s.length();

    if (xmlcharlen < 2*inlen + 2) {
        xmlcharlen = 2*inlen + 2;
        delete[] xmlchar;
        xmlchar = new xmlChar[xmlcharlen];
    }
    outlen = xmlcharlen;

    int n =
        encoding->input(xmlchar, &outlen, (const unsigned char*)s.c_str(), &inlen);
    xmlchar[outlen] = '\0';
    return n >= 0 ? xmlchar : 0;
}

/////////////////////////////////////////////////////////////////////////////
#define TESTOUT(t) if(!(t)) goto out;
void Session::list()
{
    int size;
    xmlChar *mem = 0;
    xmlDocPtr xmldoc;
    xmlNodePtr root, node;

    const PdServ::Main::VariableMap& variables = main->getVariableMap();

    TESTOUT(xmldoc = xmlNewDoc(BAD_CAST "1.0"));

    TESTOUT(root = xmlNewNode(NULL,(xmlChar*)"signals"));
    xmlDocSetRootElement(xmldoc,root);

    for (std::map<std::string,PdServ::Variable*>::const_iterator it = variables.begin();
            it != variables.end(); it++) {
        PdServ::Variable *v = it->second;

        TESTOUT(node = xmlNewNode(NULL,(xmlChar*)"signal"));
        xmlAddChild(root, node);

        TESTOUT(xmlNewProp(node, BAD_CAST "name",  utf8(v->path)));
        TESTOUT(xmlNewProp(node, BAD_CAST "alias", utf8(v->alias)));
        TESTOUT(xmlNewProp(node, BAD_CAST "unit",  BAD_CAST ""));

        TESTOUT(xmlNewProp(node,
                    BAD_CAST "datatype", BAD_CAST getDTypeName(v->dtype)));

        std::ostringstream d;
        const size_t *dim = v->getDim();
        for (size_t i = 0; i < v->ndims; i++) {
            if (i)
                d << ',';
            d << dim[i];
        }
        TESTOUT(xmlNewProp(node,
                    BAD_CAST "dimension", BAD_CAST d.str().c_str()));

        TESTOUT(xmlNewProp(node, BAD_CAST "permission",
                    BAD_CAST (v->decimation ? "r-r-r-" : "rwrwrw")));

        d.str(std::string());
        d << v->decimation;
        TESTOUT(xmlNewProp(node,
                    BAD_CAST "decimation", BAD_CAST d.str().c_str()));
    }

    xmlDocDumpFormatMemory(xmldoc, &mem, &size, 1);
    TESTOUT(mem);

    sputn(reinterpret_cast<const char*>(mem),size);
    pubsync();

out:
    xmlFreeDoc(xmldoc);
    xmlFree(mem);
    delete[] xmlchar;
}

/////////////////////////////////////////////////////////////////////////////
void Session::output()
{
    cout << __func__ << endl;
    ssize_t n = send(buf.c_str(), buf.length());
    if (n > 0)
        buf.erase(0,n);
    else {
        delete this;
        return;
    }

    if (buf.empty())
        setDetectOutput(false);
}

/////////////////////////////////////////////////////////////////////////////
void Session::printVariable(std::streambuf *sb,
        PdServ::Variable *v, const char *dataPtr)
{
    std::ostream s(sb);
    const char punct = ';';

    switch (v->dtype) {
        case si_double_T:
            {
                const double *ptr = reinterpret_cast<const double*>(dataPtr);
                const double *end = ptr + v->nelem;
                s << *ptr++;
                while (ptr < end)
                    s << punct << *ptr++;
            }
            break;

        case si_single_T:
            {
                const float *ptr = reinterpret_cast<const float*>(dataPtr);
                const float *end = ptr + v->nelem;
                s << *ptr++;
                while (ptr < end)
                    s << punct << *ptr++;
            }
            break;

        case si_uint8_T:
            {
                const uint8_t *ptr = reinterpret_cast<const uint8_t*>(dataPtr);
                const uint8_t *end = ptr + v->nelem;
                s << *ptr++;
                while (ptr < end)
                    s << punct << *ptr++;
            }
            break;

        case si_sint8_T:
            {
                const int8_t *ptr = reinterpret_cast<const int8_t*>(dataPtr);
                const int8_t *end = ptr + v->nelem;
                s << *ptr++;
                while (ptr < end)
                    s << punct << *ptr++;
            }
            break;

        case si_uint16_T:
            {
                const uint16_t *ptr = reinterpret_cast<const uint16_t*>(dataPtr);
                const uint16_t *end = ptr + v->nelem;
                s << *ptr++;
                while (ptr < end)
                    s << punct << *ptr++;
            }
            break;

        case si_sint16_T:
            {
                const int16_t *ptr = reinterpret_cast<const int16_t*>(dataPtr);
                const int16_t *end = ptr + v->nelem;
                s << *ptr++;
                while (ptr < end)
                    s << punct << *ptr++;
            }
            break;

        case si_uint32_T:
            {
                const uint32_t *ptr = reinterpret_cast<const uint32_t*>(dataPtr);
                const uint32_t *end = ptr + v->nelem;
                s << *ptr++;
                while (ptr < end)
                    s << punct << *ptr++;
            }
            break;

        case si_sint32_T:
            {
                const int32_t *ptr = reinterpret_cast<const int32_t*>(dataPtr);
                const int32_t *end = ptr + v->nelem;
                s << *ptr++;
                while (ptr < end)
                    s << punct << *ptr++;
            }
            break;

        case si_uint64_T:
            {
                const uint64_t *ptr = reinterpret_cast<const uint64_t*>(dataPtr);
                const uint64_t *end = ptr + v->nelem;
                s << *ptr++;
                while (ptr < end)
                    s << punct << *ptr++;
            }
            break;

        case si_sint64_T:
            {
                const int64_t *ptr = reinterpret_cast<const int64_t*>(dataPtr);
                const int64_t *end = ptr + v->nelem;
                s << *ptr++;
                while (ptr < end)
                    s << punct << *ptr++;
            }
            break;


        case si_boolean_T:
            {
                const bool *ptr = reinterpret_cast<const bool*>(dataPtr);
                const bool *end = ptr + v->nelem;
                s << *ptr++;
                while (ptr < end)
                    s << punct << *ptr++;
            }
            break;
        default:
            break;
    }
    s << crlf;
}

/////////////////////////////////////////////////////////////////////////////
void Session::disconnect()
{
    cout << __func__ << endl;
    delete this;
}

/////////////////////////////////////////////////////////////////////////////
const char *Session::getDTypeName(const enum si_datatype_t& t)
{
    switch (t) {
        case si_boolean_T: return "BOOL";
        case si_uint8_T:   return "UINT8";
        case si_sint8_T:   return "INT8";
        case si_uint16_T:  return "UINT16";
        case si_sint16_T:  return "INT16";
        case si_uint32_T:  return "UINT32";
        case si_sint32_T:  return "INT32";
        case si_uint64_T:  return "UINT64";
        case si_sint64_T:  return "INT64";
        case si_single_T:  return "FLOAT32";
        case si_double_T:  return "FLOAT64";
        default:           return "";
    }
}
