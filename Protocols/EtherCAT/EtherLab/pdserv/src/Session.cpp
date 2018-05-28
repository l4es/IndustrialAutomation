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
#include "Main.h"
#include "Debug.h"

#include <cerrno>
#include <cstring>      // strerror()
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

#ifdef GNUTLS_FOUND
#    include <gnutls/x509.h>
#    include "TLS.h"
#endif

using namespace PdServ;

/////////////////////////////////////////////////////////////////////////////
Session::Session(const Main *m, log4cplus::Logger& log, size_t bufsize)
: main(m), log(log)
{
    p_eof = false;
    state = NoTLS;

#ifdef GNUTLS_FOUND
    tls_session = 0;
#endif

    main->gettime(&connectedTime);
    main->prepare(this);
    eventId = ~0U;
    main->getNextEvent(this);

    char* buf = new char[bufsize];
    setp(buf, buf + bufsize);
}

/////////////////////////////////////////////////////////////////////////////
Session::~Session()
{
#ifdef GNUTLS_FOUND
    if (tls_session)
        gnutls_deinit(tls_session);
#endif

    main->cleanup(this);
    delete[] pbase();
}

/////////////////////////////////////////////////////////////////////////////
int Session::startTLS()
{
#ifdef GNUTLS_FOUND
    int result;

    result = gnutls_init(&tls_session, GNUTLS_SERVER);
    if (result) {
        LOG4CPLUS_FATAL(log,
                LOG4CPLUS_TEXT("gnutls_init() failed: ")
                << LOG4CPLUS_C_STR_TO_TSTRING(gnutls_strerror(result))
                << LOG4CPLUS_TEXT(" (")
                << LOG4CPLUS_C_STR_TO_TSTRING(
                    gnutls_strerror_name(result))
                << LOG4CPLUS_TEXT(")"));
        return result;
    }

    main->initTlsSessionData(tls_session, &blacklist);

    gnutls_session_set_ptr(tls_session, this);

    gnutls_transport_set_ptr(tls_session, this);
    gnutls_transport_set_push_function(tls_session, gnutls_push_func);
    gnutls_transport_set_pull_function(tls_session, gnutls_pull_func);

    state = InitTLS;

    return 0;
#else
    return -1;
#endif
}

/////////////////////////////////////////////////////////////////////////////
bool Session::eof() const
{
    return p_eof;
}

/////////////////////////////////////////////////////////////////////////////
int Session::overflow(int value)
{
    char c = value;
    return xsputn(&c, 1) ? c : traits_type::eof();
}

/////////////////////////////////////////////////////////////////////////////
std::streamsize Session::xsputn(const char * buf, std::streamsize count)
{
    const char* ptr = buf;

    do {
        // Put data into buffer
        size_t n = std::min(epptr() - pptr(), count);
        std::copy(ptr, ptr + n, pptr());

        // Update pointers
        pbump(n);
        ptr += n;
        count -= n;

    } while (!(pptr() == epptr() and flush(true)) and count);

    return ptr - buf;
}

/////////////////////////////////////////////////////////////////////////////
int Session::sync()
{
    return flush(false);
}

/////////////////////////////////////////////////////////////////////////////
// Flush output buffer.
//
// partial: true: do only one flush pass
int Session::flush(bool partial)
{
    const char* buf = pbase();
    size_t count = pptr() - buf;

    if (p_eof)
        return -1;
    else if (!count)
        return 0;

//    log_debug("flushing %i %zu", partial, count);
    int result = 0;
    do {
#ifdef GNUTLS_FOUND
        switch (state) {
            case NoTLS:
                result = write(buf, count);
                break;

            case InitTLS:
                // Ignore partial flush request, called from Session::sync()
                if (!partial)
                    return 0;

                LOG4CPLUS_FATAL_STR(log,
                        LOG4CPLUS_TEXT(
                            "Output buffer overflow during TLS handshake"));
                result = -ENOSPC;
                break;

            case RunTLS:
                result = gnutls_record_send(tls_session, buf, count);
                break;
        }
#else
        result = write(buf, count);
#endif

//        log_debug("flushing result %i", result);
        if (result <= 0)
            break;

        buf   += result;
        count -= result;

    } while (count and !partial);

    // Copy remaining data to buffer start and 
    // update std::streambuf's current put pointer pptr()
    std::copy(buf, const_cast<const char*>(pptr()), pbase());
    pbump(pbase() - buf);

    // Calculate EOF
    if (state == NoTLS)
        p_eof |= result <= 0 and result != -EAGAIN and result != -EINTR;
#ifdef GNUTLS_FOUND
    else if (gnutls_error_is_fatal(result) or !result)
        p_eof = true;
#endif

    if (!p_eof)
        return 0;

    // Interpret EOF
    if (!result)
        LOG4CPLUS_INFO_STR(log,
                LOG4CPLUS_TEXT("Client closed connection"));
    else if (state != RunTLS)
        LOG4CPLUS_ERROR(log,
                LOG4CPLUS_TEXT("Network write() error: ")
                << LOG4CPLUS_C_STR_TO_TSTRING(strerror(-result)));
#ifdef GNUTLS_FOUND
    else
        LOG4CPLUS_ERROR(log,
                LOG4CPLUS_TEXT("gnutls_record_send(")
                << count
                << LOG4CPLUS_TEXT("): ")
                << LOG4CPLUS_C_STR_TO_TSTRING(gnutls_strerror(result))
                << LOG4CPLUS_TEXT(" (")
                << LOG4CPLUS_C_STR_TO_TSTRING(
                    gnutls_strerror_name(result))
                << ')');
#endif

    return -1;
}

/////////////////////////////////////////////////////////////////////////////
int Session::underflow()
{
    char c;
    return xsgetn(&c, 1) == 1 ? c : traits_type::eof();
}

/////////////////////////////////////////////////////////////////////////////
std::streamsize Session::xsgetn(char* s, std::streamsize n)
{
    ssize_t result = 0;

#ifdef GNUTLS_FOUND
//    log_debug("tlsstate = %i", state);
    switch (state) {
        case NoTLS:
            result = read(s, n);
            break;

        case InitTLS:
            result = gnutls_handshake(tls_session);
            log_debug("gnutls_handshake = %zi", result);

            // break out while still handshaking
            if (result != GNUTLS_E_SUCCESS)
                break;

            state = RunTLS;
            sync();

            // return successfully if there is no data available
            n = gnutls_record_check_pending(tls_session);
            if (!n)
                return 0;

            // no break

        case RunTLS:
            result = gnutls_record_recv(tls_session, s, n);
//            log_debug("gnutls_record_recv = %zi", result);
            break;
    }
#else
    result = read(s, n);
#endif

    if (result > 0)
        return result;

    // Calculate EOF
    if (state == NoTLS)
        p_eof |= result <= 0 and result != -EAGAIN and result != -EINTR;
#ifdef GNUTLS_FOUND
    else if (gnutls_error_is_fatal(result) or !result)
        p_eof = true;
#endif

    // Interpret EOF
    if (p_eof) {
        if (!result)
            LOG4CPLUS_INFO_STR(log,
                    LOG4CPLUS_TEXT("Client closed connection"));
        else if (state == NoTLS)
            LOG4CPLUS_ERROR(log,
                    LOG4CPLUS_TEXT("Network error: ")
                    << LOG4CPLUS_C_STR_TO_TSTRING(strerror(-result)));
#ifdef GNUTLS_FOUND
        else {
            const char* func = state == InitTLS
                ? "gnutls_handshake" : "gnutls_record_recv";
            LOG4CPLUS_ERROR(log,
                    LOG4CPLUS_TEXT("TLS error during ")
                    << LOG4CPLUS_C_STR_TO_TSTRING(func)
                    << LOG4CPLUS_TEXT("(): ")
                    << LOG4CPLUS_C_STR_TO_TSTRING(gnutls_strerror(result))
                    << LOG4CPLUS_TEXT(" (")
                    << LOG4CPLUS_C_STR_TO_TSTRING(
                        gnutls_strerror_name(result))
                    << ')');
        }
#endif

        result = 0;
    }

    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#ifdef GNUTLS_FOUND
/////////////////////////////////////////////////////////////////////////////
int Session::gnutls_verify_client(gnutls_session_t tls_session)
{
    int result;
    Session* session =
        reinterpret_cast<Session*>(gnutls_session_get_ptr(tls_session));
    const Blacklist* blacklist = session->blacklist;

    unsigned int list_size = ~0U;
    const gnutls_datum_t* certs =
        gnutls_certificate_get_peers (tls_session, &list_size);
    if (list_size == ~0U or !certs or !list_size) {
        LOG4CPLUS_FATAL_STR(session->log,
                LOG4CPLUS_TEXT(
                    "gnutls_certificate_get_peers() failed: "
                    "Client did not send a certificate or "
                    "there was an error retrieving it"));
        return -1;
    }
    else {
        LOG4CPLUS_INFO(session->log,
                LOG4CPLUS_TEXT("Received ")
                << list_size
                << LOG4CPLUS_TEXT(" certificates from peer"));
    }

    gnutls_x509_crt_t crt;
    result = gnutls_x509_crt_init (&crt);
    if (result) {
        LOG4CPLUS_FATAL(session->log,
                LOG4CPLUS_TEXT("gnutls_x509_crt_init() failed: ")
                << LOG4CPLUS_C_STR_TO_TSTRING(gnutls_strerror(result))
                << LOG4CPLUS_TEXT(" (")
                << LOG4CPLUS_C_STR_TO_TSTRING(
                    gnutls_strerror_name(result))
                << LOG4CPLUS_TEXT(")"));
        return -1;
    }

    for (unsigned int i = 0; !result and i < list_size; ++i) {
        result = gnutls_x509_crt_import (crt, certs++, GNUTLS_X509_FMT_DER);
        if (result) {
            LOG4CPLUS_FATAL(session->log,
                    LOG4CPLUS_TEXT("gnutls_x509_crt_import(")
                    << i
                    << LOG4CPLUS_TEXT(") failed: ")
                    << LOG4CPLUS_C_STR_TO_TSTRING(gnutls_strerror(result))
                    << LOG4CPLUS_TEXT(" (")
                    << LOG4CPLUS_C_STR_TO_TSTRING(
                        gnutls_strerror_name(result))
                    << LOG4CPLUS_TEXT(")"));
            break;
        }

        gnutls_datum_t cinfo = { NULL, 0 };
        result = gnutls_x509_crt_print(
                crt, GNUTLS_CRT_PRINT_ONELINE, &cinfo);
        if (result == GNUTLS_E_SUCCESS) {
            LOG4CPLUS_INFO(session->log,
                    LOG4CPLUS_TEXT("Certificate[")
                    << i
                    << LOG4CPLUS_TEXT("].info: ")
                    << LOG4CPLUS_C_STR_TO_TSTRING((const char*)cinfo.data));
        }
        else {
            LOG4CPLUS_INFO(session->log,
                    LOG4CPLUS_TEXT("gnutls_x509_crt_print(")
                    << i
                    << LOG4CPLUS_TEXT(") failed: ")
                    << LOG4CPLUS_C_STR_TO_TSTRING(gnutls_strerror(result))
                    << LOG4CPLUS_TEXT(" (")
                    << LOG4CPLUS_C_STR_TO_TSTRING(
                        gnutls_strerror_name(result))
                    << LOG4CPLUS_TEXT(")"));
        }
        gnutls_free(cinfo.data);

        if (!blacklist->empty()) {
            unsigned char buf[100];
            size_t len = sizeof(buf);
            result = gnutls_x509_crt_get_key_id(crt, 0, buf, &len);
            if (result or !len) {
                LOG4CPLUS_FATAL(session->log,
                        LOG4CPLUS_TEXT("gnutls_x509_crt_get_key_id(")
                        << i
                        << LOG4CPLUS_TEXT(") failed: ")
                        << LOG4CPLUS_C_STR_TO_TSTRING(gnutls_strerror(result))
                        << LOG4CPLUS_TEXT(" (")
                        << LOG4CPLUS_C_STR_TO_TSTRING(
                            gnutls_strerror_name(result))
                        << LOG4CPLUS_TEXT(")"));
                result = -1;
                break;
            }
            else {
                Blacklist::const_iterator it =
                    blacklist->find(datum_string(buf, len));
                if (it != blacklist->end()) {
                    LOG4CPLUS_FATAL(session->log,
                            LOG4CPLUS_TEXT(
                                "Certificate is blacklisted. Public Key Id = ")
                            << LOG4CPLUS_STRING_TO_TSTRING(std::string(*it)));
                    result = -1;
                    break;
                }
            }
        }
    }

    gnutls_x509_crt_deinit (crt);

    if (result)
        return result;

    unsigned int status;
    result = gnutls_certificate_verify_peers2(tls_session, &status);
    if (result) {
        // Complain
        LOG4CPLUS_FATAL(session->log,
                LOG4CPLUS_TEXT("gnutls_certificate_verify_peers2() failed: ")
                << LOG4CPLUS_C_STR_TO_TSTRING(gnutls_strerror(result))
                << LOG4CPLUS_TEXT(" (")
                << LOG4CPLUS_C_STR_TO_TSTRING(
                    gnutls_strerror_name(result))
                << LOG4CPLUS_TEXT(")"));
        return -1;
    }
    else if (status) {
        std::ostringstream os;
        os << "Verification of peer's certificate failed: ";
        if (status & GNUTLS_CERT_INVALID)
            os << "Certificate is invalid";

        if (status & GNUTLS_CERT_REVOKED)
            os << "Certificate is revoked";

        if (status & GNUTLS_CERT_SIGNER_NOT_FOUND)
            os << "Certificate signer is not found";

        if (status & GNUTLS_CERT_SIGNER_NOT_CA)
            os << "Certificate signer is not a certification authority";

        if (status & GNUTLS_CERT_INSECURE_ALGORITHM)
            os << "Certificate uses an insecure algorithm";

        if (status & GNUTLS_CERT_NOT_ACTIVATED)
            os << "Certificate is not activated";

        if (status & GNUTLS_CERT_EXPIRED)
            os << "Certificate expired";

        LOG4CPLUS_FATAL_STR(session->log,
                LOG4CPLUS_STRING_TO_TSTRING(os.str()));

        return -1;
    }

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
ssize_t Session::gnutls_push_func(
                gnutls_transport_ptr_t ptr, const void* buf, size_t count)
{
    Session* session = reinterpret_cast<Session*>(ptr);
    ssize_t result = session->write(buf, count);

//    log_debug("result = %zi", result);
    if (result >= 0)
        return result;

    gnutls_transport_set_errno (session->tls_session, -result);

    return -1;
}

/////////////////////////////////////////////////////////////////////////////
ssize_t Session::gnutls_pull_func(
                gnutls_transport_ptr_t ptr, void* buf, size_t count)
{
    Session* session = reinterpret_cast<Session*>(ptr);
    ssize_t result = session->read(buf, count);

//    log_debug("result = %zi", result);
    if (result >= 0)
        return result;

    gnutls_transport_set_errno (session->tls_session, -result);

    return -1;
}
#endif
