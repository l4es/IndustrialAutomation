/*****************************************************************************
 *
 *  $Id$
 *
 *  Copyright 2016      Richard Hacker (lerichi at gmx dot net)
 *
 *  This file is part of the pdcom library.
 *
 *  The pdcom library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or (at
 *  your option) any later version.
 *
 *  The pdcom library is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 *  License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with the pdcom library. If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/

#include "SASLClient.h"

#include <cstring>
#include <cstdio>

int SASLClient::init = sasl_client_init(NULL);

///////////////////////////////////////////////////////////////////////////
SASLClient::SASLClient(const std::string& localIP,
        const std::string& remoteIP,
        const std::string& serverFQDN):
    conn(0)
{
    int result;

    if (init != SASL_OK)
        throw Exception("sasl_client_init()");

    result = sasl_client_new("pdserv",
            serverFQDN.c_str(),
            localIP.c_str(),
            remoteIP.c_str(),
            prompt_cb(),
            SASL_SUCCESS_DATA,
            &conn);
    if (result != SASL_OK)
        throw Exception("sasl_client_new()");
}

///////////////////////////////////////////////////////////////////////////
SASLClient::~SASLClient()
{
    if (conn)
        sasl_dispose(&conn);
}

///////////////////////////////////////////////////////////////////////////
void SASLClient::setMechList(const char* list)
{
    mechList = list;
}


int SASLClient::start(const std::string& mech, const char** out, int *outlen)
{
    sasl_interact_t *client_interact = NULL;

    switch (sasl_client_start(conn, mech.c_str(), 0, 0, out, outlen)) {
        case SASL_OK:
            return 1;

        case SASL_CONTINUE:
            return 0;

        case SASL_INTERACT:
            process->loginReply(5, 

        default:
            return -1;
    }
}

int SASLClient::step(const std::string& mech, const char** out, int *outlen)
{
    sasl_interact_t *client_interact = NULL;
    switch (
    int result;

    do {

    } while (result == SASL_INTERACT);
    switch (sasl_server_start(conn, mech.c_str(), 0, 0, out, outlen)) {
        case SASL_OK:
            return 1;

        case SASL_CONTINUE:
            return 0;

        default:
            return -1;
    }
}

// ///////////////////////////////////////////////////////////////////////////
// bool SASLClient::step(const char* mech,
//         const char* clientout, unsigned int clientoutlen,
//         const char** out, unsigned int* outlen)
// {
//     int result;
//     if (mech) {
//         result = sasl_server_start(conn,
//                 mech,
//                 clientout, clientoutlen,
//                 out, outlen);
//     }
//     else {
//         result = sasl_server_step(conn,
//                 clientout, clientoutlen,
//                 out, outlen);
//     }
// 
//     if (result == SASL_OK)
//         return false;
//     else if (result == SASL_CONTINUE)
//         return true;
//     else
//         throw Exception(
//                 std::string(mech
//                     ? "sasl_server_start(): " : "sasl_server_step(): ")
//                 + sasl_errdetail(conn));
// }

///////////////////////////////////////////////////////////////////////////
const sasl_callback_t* SASLClient::prompt_cb()
{
    static const sasl_callback_t _cb[CB_SIZE] = {
        {SASL_CB_GETOPT,     cb_t(cb_getopt),           0},
        {SASL_CB_GETPATH,    cb_t(cb_getpath),          0},
        {SASL_CB_LIST_END,   0,                         0},
        {SASL_CB_CANON_USER, cb_t(cb_canon_user),       0},
    };

    std::copy(_cb, _cb + CB_SIZE, cb);
    for (size_t i = 0; i < CB_SIZE; ++i)
        cb[i].context = this;
    
    return cb;
}

///////////////////////////////////////////////////////////////////////////
// man: sasl_getopt_t
int SASLClient::cb_getopt(void* context, const char* plugin_name,
        const char* option, const char** result, unsigned* len)
{
    int rv = SASL_BADPARAM;
    printf("SASLClient::%s(context=%p, plugin_name=%s, option=%s, result=%p len=%p)\n",
            __func__, context, plugin_name, option, result, len);

//    if (!strcmp(option, "pwcheck_method")) {
//        *result = "saslauthd";
//        rv = SASL_OK;
//    }

    if (rv == SASL_OK and len)
        *len = strlen(*result);

    return rv;
}

///////////////////////////////////////////////////////////////////////////
// man: sasl_getpath_t
int SASLClient::cb_getpath (void* /*context*/, char** /*path*/)
{
    printf("%s\n", __func__);
    return SASL_FAIL;
}

///////////////////////////////////////////////////////////////////////////
// man: sasl_canon_user_t
int SASLClient::cb_canon_user(sasl_conn_t* /*conn*/,
        void* /*context*/,
        const char* /*in*/, unsigned /*inlen*/,
        unsigned /*flags*/,
        const char* /*user_realm*/,
        char* /*out*/,
        unsigned /*out_max*/, unsigned* /*out_len*/)
{
//    printf("%s context=%p in=%s inlen=%u flags=%u user_realm=%s\n",
//            __func__, context, in, inlen, flags, user_realm);
    return SASL_FAIL;
}

