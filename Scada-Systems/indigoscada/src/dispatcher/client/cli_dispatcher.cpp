/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2009 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */

//-------------------------------------------------------------------*--------*
// Call level interface client dispatcher server implementation
//-------------------------------------------------------------------*--------*

#include "stdtp.h"
#include "sockio.h"
#include "sync.h"
#include "cli_dispatcher.h"
#include "cliproto_dispatcher.h"
#include "IndentedTrace.h"

BEGIN_GIGABASE_NAMESPACE

static descriptor_table_dispatcher<session_desc_dispatcher>   dispatch_sessions;
static connection_pool_dispatcher dispatch_connections;

END_GIGABASE_NAMESPACE
USE_GIGABASE_NAMESPACE

int cli_open_dispatcher(char_t const*   server_url,
	     int           max_connect_attempts,
	     int           reconnect_timeout_sec,
	     char_t const* user_name,
	     char_t const* password,
	     int           pooled_connection)
{
    IT_IT("cli_open_dispatcher");
	
	if (pooled_connection) 
	{
		IT_COMMENT("pooled_connection");

		session_desc_dispatcher* session = dispatch_connections.new_connection(server_url, user_name, password);
		if (session != NULL) 
		{ 
			return session->id;
		}
    }

	IT_COMMENT("not_pooled_connection");

    socket_t* sock = socket_t::connect(server_url,
				       socket_t::sock_any_domain,
				       max_connect_attempts,
				       reconnect_timeout_sec);
    if (!sock->is_ok()) {
	char_t buf[256];
	sock->get_error_text(buf, sizeof buf);
	fprintf(stderr, "Failed to connect to server: %s\n", buf);
	delete sock;
	return cli_connection_refused;
    }
    size_t msg_size = sizeof(cli_request_dispatcher) + (STRLEN(user_name) + STRLEN(password) + 2)*sizeof(char_t);

    dbSmallBuffer<char_t> buf(msg_size);
    char_t* p = (char_t*)buf.base();
    cli_request_dispatcher* req = (cli_request_dispatcher*)p;
    req->length  = msg_size;
    req->cmd     = cli_cmd_login;
	req->stmt_id = 0;
    p += sizeof(cli_request_dispatcher);
    p = pack_str(p, user_name);
    p = pack_str(p, password);
	req->pack();
    if (sock->write(req, msg_size)) {
	int4 response;
	if (!sock->read(&response, sizeof response)) {
	    return cli_network_error;
	}
	unpack4(response);
	if (response == cli_ok) {
	    session_desc_dispatcher* session = dispatch_sessions.allocate();
	    session->sock = sock;
	    
	    if (pooled_connection) 
		{ 
			session->pool = &dispatch_connections;
			session->user = new char_t[STRLEN(user_name)+1];
			STRCPY(session->user, user_name);
			session->password = new char_t[STRLEN(password)+1];
			STRCPY(session->password, password);
	    }

	    return session->id;
	}
	return response;
    } else { 
	return cli_network_error;
    }
}


int cli_close_dispatcher(int session)
{
    IT_IT("cli_close_dispatcher");
	
	session_desc_dispatcher* s = dispatch_sessions.get(session);

    if (s == NULL) 
	{
		return cli_bad_descriptor;
    }

    if (s->pool != NULL) 
	{ 
		s->pool->release(s);
		return cli_ok;
    }

    cli_request_dispatcher req;
    req.length = sizeof(req);
    req.cmd = cli_cmd_close_session;
	req.stmt_id = 0;
    req.pack();
    int result = cli_ok;
    if (!s->sock->write(&req, sizeof req)) {
	result = cli_network_error;
    }
    dispatch_sessions.free(s);
    return result;
}

void cli_clear_connection_pool_dispatcher()
{
    IT_IT("cli_clear_connection_pool_dispatcher");
	
	dispatch_connections.close();
}

int cli_write_notification(int session, int notif_code,  const char_t *data, int data_length)  
{
	IT_IT("cli_write_notification");
	
	session_desc_dispatcher* s = dispatch_sessions.get(session);

    if (s == NULL) 
	{
        return cli_bad_descriptor;
    }
	
	int len = sizeof(cli_request_dispatcher) + data_length;
    dbSmallBuffer<char_t> buf(len);
    cli_request_dispatcher* req = (cli_request_dispatcher*)buf.base();
    req->length = len;
    req->cmd    = notif_code;
    req->stmt_id = 0;
    memcpy((char_t*)(req+1), data, data_length);

    req->pack();
    if (!s->sock->write(buf, len)) 
	{
		return cli_network_error;
    }

    return cli_ok;
}

session_desc_dispatcher* cli_get_session_desc(int session)
{
	IT_IT("cli_get_session_desc");
	
	session_desc_dispatcher* s = dispatch_sessions.get(session);
	return s;
}