/*****************************************************************************
 *
 *  $Id$
 *
 *  Copyright 2016      Richard Hacker (lerichi at gmx dot net)
 *
 *  This file is part of the PdCom library.
 *
 *  The PdCom library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or (at
 *  your option) any later version.
 *
 *  The PdCom library is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 *  License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with the PdCom library. If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/

#ifndef PDCOM_SASLCLIENT_H
#define PDCOM_SASLCLIENT_H

#include <sasl/sasl.h>
#include <string>

class SASLClient {
    public:
        typedef std::runtime_error Exception;

        SASLClient(const std::string& localIP,
                const std::string& remoteIP,
                const std::string& serverFQDN);
        ~SASLClient();

        bool SASLClient::start(const std::string& mech,
                const char** out, int *outlen)
        bool clientStep(const std::string& clientData,
                const std::string** serverData);
        void setMechList(const char* list);
//        const char* mechanisms() const;
//        bool serverStep(const char* data, const std::string**  query);
//        void clientStep(const std::string& reply);

    private:

        static int init;

        std::string mechList;

        sasl_conn_t *conn;

        typedef int (*cb_t)(void);
        static const size_t CB_SIZE = 4;
        sasl_callback_t cb[CB_SIZE];

        const sasl_callback_t* prompt_cb();

        // static callbacks
        static int cb_getopt(void *context,
                const char *plugin_name,
                const char *option,
                const char **result, unsigned *len);
        static int cb_getpath (void *context,
                char **path);
        static int cb_canon_user(sasl_conn_t *conn,
                void *context,
                const char *in, unsigned inlen,
                unsigned flags,
                const char *user_realm,
                char *out,
                unsigned out_max, unsigned *out_len);

};

#endif //PDCOM_SASLCLIENT_H
