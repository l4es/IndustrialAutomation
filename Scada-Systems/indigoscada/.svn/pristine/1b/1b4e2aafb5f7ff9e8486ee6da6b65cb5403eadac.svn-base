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
// Multithreaded dispatcher server implementation
//-------------------------------------------------------------------*--------*

#include "stdtp.h"
#include "sync.h"
#include "dispatcher.h"
#include "sockio.h"
#include "cli_dispatcher.h"
#include "cliproto_dispatcher.h"
#include "server_dispatcher.h"
#include "IndentedTrace.h"

#ifndef SECURE_SERVER
#error "SECURE_SERVER not defined"
#endif

#if !defined(_WIN32) && defined(NO_PTHREADS)
#error Server requires multithreading support
#endif

dispServer* dispServer::chain;

void thread_proc dispServer::serverThread(void* arg)
{
    IT_IT("dispServer::serverThread");

	((dispServer*)arg)->serveClient();
}

void thread_proc dispServer::acceptLocalThread(void* arg)
{
    IT_IT("dispServer::acceptLocalThread");

	dispServer* server = (dispServer*)arg;
    server->acceptConnection(server->localAcceptSock);
}

void thread_proc dispServer::acceptGlobalThread(void* arg)
{
    IT_IT("dispServer::acceptGlobalThread");

	dispServer* server = (dispServer*)arg;
    server->acceptConnection(server->globalAcceptSock);
}

dispServer::dispServer(char_t const* serverURL,
		   int optimalNumberOfThreads,
		   int connectionQueueLen)
{
    IT_IT("dispServer::dispServer");
	
	next = chain;
    chain = this;
    this->optimalNumberOfThreads = optimalNumberOfThreads;
    this->connectionQueueLen = connectionQueueLen;
#ifdef UNICODE 
    char buf[256];
    wcstombs(buf, serverURL, sizeof buf);
    URL = new char_t[STRLEN(serverURL) + 1];
    STRCPY(URL, serverURL);
    address = buf;
#else
    URL = new char[STRLEN(serverURL) + 1];
    strcpy(URL, serverURL);
#endif
    freeList = activeList = waitList = NULL;
}

dispServer* dispServer::find(char_t const* URL)
{
    IT_IT("dispServer::find");
	
	for (dispServer* server = chain; server != NULL; server = server->next) {
	if (STRCMP(URL, server->URL) == 0) {
	    return server;
	}
    }
    return NULL;
}

void dispServer::cleanup()
{
    IT_IT("dispServer::cleanup");
	
	dispServer *server, *next;
    for (server = chain; server != NULL; server = next) {
	next = server->next;
	delete server;
    }
}

void dispServer::start()
{
    IT_IT("dispServer::start");
	
	nActiveThreads = nIdleThreads = 0;
    cancelWait = cancelSession = cancelAccept = false;
    go.open();
    done.open();

    globalAcceptSock = socket_t::create_global(URL, connectionQueueLen);

    if (!globalAcceptSock->is_ok()) 
	{
		char_t errbuf[64];
		globalAcceptSock->get_error_text(errbuf, sizeof errbuf);
	//	dbTrace(STRLITERAL("Failed to create global socket: %s\n"), errbuf);
		delete globalAcceptSock;
		globalAcceptSock = NULL;
    } 
	else 
	{ 
		globalAcceptThread.create(acceptGlobalThread, this);
    }

    localAcceptSock = socket_t::create_local(URL, connectionQueueLen);

    if (!localAcceptSock->is_ok()) 
	{
		char_t errbuf[64];
		localAcceptSock->get_error_text(errbuf, sizeof errbuf);
	//	dbTrace(STRLITERAL("Failed to create local socket: %s\n"), errbuf);
		delete localAcceptSock;
		localAcceptSock = NULL;
    } 
	else 
	{ 
		localAcceptThread.create(acceptLocalThread, this);
    }
}

void dispServer::stop()
{
    IT_IT("dispServer::stop");
	
	cancelAccept = true;
    if (globalAcceptSock != NULL) 
	{
		globalAcceptSock->cancel_accept();
		globalAcceptThread.join();
		delete globalAcceptSock;
		globalAcceptSock = NULL;
    }
    if (localAcceptSock != NULL) 
	{
		localAcceptSock->cancel_accept();
		localAcceptThread.join();
		delete localAcceptSock;
		localAcceptSock = NULL;
    }

    dbCriticalSection cs(mutex);
    cancelSession = true;

    while (activeList != NULL) 
	{
		activeList->sock->shutdown();
		done.wait(mutex);
    }

    cancelWait = true;

    while (nIdleThreads != 0) 
	{
		go.signal();
		done.wait(mutex);
    }

    while (waitList != NULL) 
	{
		dispSession* next = waitList->next;
		delete waitList->sock;
		waitList->next = freeList;
		freeList = waitList;
		waitList = next;
    }

    assert(nActiveThreads == 0);
    done.close();
    go.close();
}


void dispServer::serveClient()
{
	IT_IT("dispServer::serveClient");
	
	while (true) 
	{
		dispSession* session;

		{
			dbCriticalSection cs(mutex); //enter critical section
			do {
				go.wait(mutex);
				if (cancelWait) 
				{
					nIdleThreads -= 1;
					done.signal();
					return;
				}
			} while (waitList == NULL);

			session = waitList;
			waitList = waitList->next;
			session->next = activeList;
			activeList = session;
			nIdleThreads -= 1;
			nActiveThreads += 1;

		}//exit critical section

		cli_request_dispatcher req;
		int4 response = cli_ok;
		bool online = true;
		bool authenticated = false;

		while (online && session->sock->read(&req, sizeof req)) 
		{
			req.unpack();

			int length = req.length - sizeof(req);
			dbSmallBuffer<char> msg(length);
			if (length > 0) 
			{
				if (!session->sock->read(msg, length)) 
				{
					break;
				}
				
				IT_COMMENT2("req.cmd = %d,msg = %s", req.cmd, (char_t *)msg);
				IT_COMMENT1("session = %p", session);
				//IT_DUMP_STR((char_t *)msg, length);

				#ifdef SECURE_SERVER
//------------------------------------------------------------------------------------------------------------
// step 4 msg signature checking. Offers some degree of protection against naive crackers or worms
// I agree with you that security layer should be the correct answer. priority devel?

                int i, s = req.length + req.cmd + req.stmt_id;
                char_t *p = (char_t *)msg;
                for (i = 0; i < length; i++, p++) {
                   s += (*p << 7) + (*p << 3) + i;
//                 s += *p + i;  alternative more simple checking if above can't be
//                               implemented on other languages
                }
                if (s != req.sig) break;
//------------------------------------------------------------------------------------------------------------
				#endif

			}

			if (req.cmd != cli_cmd_login && !authenticated) 
			{ 
				response = cli_login_failed;
				pack4(response);
				session->sock->write(&response, sizeof response);
				break;
			}

			switch(req.cmd) 
			{
				case cli_cmd_login:
				{
					if (authenticate(msg)) 
					{ 
						authenticated = true;
						response = cli_ok;
					} 
					else 
					{ 
						online = false;
						response = cli_login_failed;
					}
					pack4(response);
					session->sock->write(&response, sizeof response);
				}
				break;
				case cli_cmd_close_session:
					online = false;
				break;
				default:
				{
					//broadcast the message
					
					int len = sizeof(cli_request_dispatcher) + length;
					dbSmallBuffer<char> buf(len);
					cli_request_dispatcher* notification = (cli_request_dispatcher*)buf.base();
					notification->length = len;
					notification->cmd    = req.cmd;
					notification->stmt_id = 0;
					memcpy((char*)(notification+1), msg, length);
					notification->pack();  
    					
					{
						dbCriticalSection cs(mutex); //enter critical section
						dispSession** spp;
					    for (spp = &activeList; *spp != NULL; spp = &(*spp)->next)
						{
							(*spp)->sock->write(buf, len);
						}

					}//exit critical section
				}
				break;
			}
		}

		// Finish session
		{
			dbCriticalSection cs(mutex);
			dispSession** spp;
			delete session->sock;
			for (spp = &activeList; *spp != session; spp = &(*spp)->next);
			*spp = session->next;
			session->next = freeList;
			freeList = session;
			nActiveThreads -= 1;

			if (cancelSession) 
			{
				done.signal();
				break;
			}

			if (nActiveThreads + nIdleThreads >= optimalNumberOfThreads) 
			{
				break;
			}
			nIdleThreads += 1;
		}
    }
}

void dispServer::acceptConnection(socket_t* acceptSock)
{
    IT_IT("dispServer::acceptConnection");
	
	while (true) 
	{
		socket_t* sock = acceptSock->accept();
		dbCriticalSection cs(mutex);
		if (cancelAccept) 
		{
			return;
		}
		if (sock != NULL) 
		{
			if (freeList == NULL) 
			{
				freeList = new dispSession;
				freeList->next = NULL;
			}

			dispSession* session = freeList;
			freeList = session->next;
			session->sock = sock;
			session->next = waitList;
			waitList = session;
			if (nIdleThreads == 0) 
			{
				dbThread thread;
				nIdleThreads = 1;
				thread.create(serverThread, this);
				thread.detach();
			}
			go.signal();
		}
    }
}

#include "inifile.h"

bool dispServer::authenticate(char* buf)
{
    IT_IT("dispServer::authenticate");
	
	char_t* user = (char_t*)buf;
    buf = unpack_str(user, buf);
    char_t* password = (char_t*)buf;
    unpack_str(password, buf);

	char ini_file[256];

	#ifdef WIN32
	ini_file[0] = '\0';
	if(GetModuleFileName(NULL, ini_file, _MAX_PATH))
	{
		*(strrchr(ini_file, '\\')) = '\0';        // Strip \\filename.exe off path
		*(strrchr(ini_file, '\\')) = '\0';        // Strip \\bin off path
	}
	#endif
	
	strcat(ini_file, "\\project\\dispatcher.ini");
	Inifile iniFile(ini_file);


	if( iniFile.find("user","dispatcherserver") &&
		iniFile.find("password","dispatcherserver") )
	{
	
		if( !strcmp(user, iniFile.find("user","dispatcherserver")) && 
			!strcmp(password, iniFile.find("password","dispatcherserver")) )
		{
			IT_COMMENT("Login success");
			return true;
		}
	}
	
	IT_COMMENT("Login failure");
	return false;
}


dispServer::~dispServer()
{
    IT_IT("dispServer::~dispServer");
	
	dispServer** spp;
    for (spp = &chain; *spp != this; spp = &(*spp)->next);
    *spp = next;
    delete globalAcceptSock;
    delete localAcceptSock;
    delete[] URL;
}



