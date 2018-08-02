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
// Multithreaded dispatcher server class
//-------------------------------------------------------------------*--------*

#ifndef __SERVER_DISPATCHER_H__
#define __SERVER_DISPATCHER_H__

#include "sockio.h"

USE_GIGABASE_NAMESPACE

class dispSession {
  public:
    dispSession*     next;
    socket_t*      sock;
};

class dispServer {
  protected:
    static dispServer* chain;
    dispServer*        next;
    char_t*          URL;
    dispSession*       freeList;
    dispSession*       waitList;
    dispSession*       activeList;
	int              optimalNumberOfThreads;
    int              connectionQueueLen;
    int              nActiveThreads;
    int              nIdleThreads;
    bool             cancelWait;
    bool             cancelAccept;
    bool             cancelSession;
    dbMutex          mutex;
    dbSemaphore      go;
    dbSemaphore      done;
    socket_t*        globalAcceptSock;
    socket_t*        localAcceptSock;
    dbThread         localAcceptThread;
    dbThread         globalAcceptThread;

    static void thread_proc serverThread(void* arg);
    static void thread_proc acceptLocalThread(void* arg);
    static void thread_proc acceptGlobalThread(void* arg);

    void serveClient();
    void acceptConnection(socket_t* sock);

    bool authenticate(char* buf);

  public:

    static dispServer* find(char_t const* serverURL);
    static void      cleanup();

    void stop();
    void start();

    dispServer(char_t const* serverURL,
	     int optimalNumberOfThreads = 8,
	     int connectionQueueLen = 64);
    ~dispServer();
};

#endif
