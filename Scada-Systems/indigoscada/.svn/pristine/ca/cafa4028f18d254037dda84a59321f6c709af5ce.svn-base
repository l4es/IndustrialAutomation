//-< SYNC.CPP >------------------------------------------------------*--------*
// GigaBASE                  Version 1.0         (c) 1999  GARRET    *     ?  *
// (Post Relational Database Management System)                      *   /\|  *
//                                                                   *  /  \  *
//                          Created:     06-Nov-2002  K.A. Knizhnik  * / [] \ *
//                          Last update: 06-Nov-2002  K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Intertask synchonization primitives
//-------------------------------------------------------------------*--------*

#define INSIDE_GIGABASE

#include "stdtp.h"
#include "sync.h"

BEGIN_GIGABASE_NAMESPACE

void thread_proc dbPooledThread::pooledThreadFunc(void* arg)
{
    ((dbPooledThread*)arg)->run();
}

dbPooledThread::dbPooledThread(dbThreadPool* threadPool)
{
    pool = threadPool;
    startSem.open();
    readySem.open();
    next = NULL;
    running = true;
    thread.create(&pooledThreadFunc, this);
}

dbPooledThread::~dbPooledThread()
{
    startSem.close();
    readySem.close();
}

void dbPooledThread::stop() 
{
    running = false;
    startSem.signal(); 
    readySem.wait(pool->mutex);
}

void dbPooledThread::run() 
{
    dbCriticalSection cs(pool->mutex);
    while (true) { 
        startSem.wait(pool->mutex);
        if (!running) { 
            break;
        }
        (*f)(arg);
        readySem.signal();
    }
    readySem.signal();
}
    
void dbThreadPool::join(dbPooledThread* thr) 
{ 
    dbCriticalSection cs(mutex);
    thr->readySem.wait(mutex);
    thr->next = freeThreads;
    freeThreads = thr;
}


dbPooledThread* dbThreadPool::create(dbThread::thread_proc_t f, void* arg)
{
    dbCriticalSection cs(mutex);
    dbPooledThread* t = freeThreads;
    if (t == NULL) { 
        t = freeThreads = new dbPooledThread(this);
    }
    freeThreads = t->next;
    t->f = f;
    t->arg = arg;
    t->startSem.signal();
    return t;
}


dbThreadPool::dbThreadPool()
{
    freeThreads = NULL;
}
   
dbThreadPool::~dbThreadPool()
{
    dbCriticalSection cs(mutex);
    dbPooledThread *t, *next;
    for (t = freeThreads; t != NULL; t = next) { 
        next = t->next;
        t->stop();
        delete t;
    }        
}


END_GIGABASE_NAMESPACE
