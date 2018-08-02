//-< SYNC.H >--------------------------------------------------------*--------*
// GigaBASE                  Version 1.0         (c) 1999  GARRET    *     ?  *
// (Post Relational Database Management System)                      *   /\|  *
//                                                                   *  /  \  *
//                          Created:     20-Nov-98    K.A. Knizhnik  * / [] \ *
//                          Last update:  8-Feb-99    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Intertask synchonization primitives
//-------------------------------------------------------------------*--------*

#ifndef __SYNC_H__
#define __SYNC_H__


BEGIN_GIGABASE_NAMESPACE

#if defined(_WIN32)
class GIGABASE_DLL_ENTRY dbMutex {
    CRITICAL_SECTION     cs;
    bool                 initialized;
  public:
    dbMutex() {
        InitializeCriticalSection(&cs);
        initialized = true;
    }
    ~dbMutex() {
        DeleteCriticalSection(&cs);
        initialized = false;
    }
    bool isInitialized() { 
        return initialized;
    }
    void lock() {
        if (initialized) { 
            EnterCriticalSection(&cs);
        }
    }
    void unlock() {
        if (initialized) { 
            LeaveCriticalSection(&cs);
        }
    }
};

#define thread_proc WINAPI

class GIGABASE_DLL_ENTRY dbThread {
    HANDLE h;
  public:
    enum ThreadPriority { 
        THR_PRI_LOW, 
        THR_PRI_HIGH
    };

    void setPriority(ThreadPriority pri) { 
        SetThreadPriority(h, pri == THR_PRI_LOW ? THREAD_PRIORITY_IDLE : THREAD_PRIORITY_HIGHEST);
    }

    typedef void (thread_proc* thread_proc_t)(void*);

    void create(thread_proc_t f, void* arg) {
        DWORD threadid;
        h = CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(f), arg, 0, &threadid);
    }
    void join() {
        WaitForSingleObject(h, INFINITE);
        CloseHandle(h);
        h = NULL;
    }
    void detach() {
        if (h != NULL) {
            CloseHandle(h);
            h = NULL;
        }
    }
    dbThread() {
        h = NULL;
    }
    ~dbThread() {
        if (h != NULL) {
            CloseHandle(h);
        }
    }
    static int numberOfProcessors() {
        SYSTEM_INFO sysinfo;
        GetSystemInfo(&sysinfo);
        return sysinfo.dwNumberOfProcessors;
    }
};

const int dbMaxSemValue = 1000000;


class GIGABASE_DLL_ENTRY dbSemaphore {
    HANDLE s;
  public:
    void wait(dbMutex& mutex, time_t timeout = INFINITE) {
        mutex.unlock();
        int rc = WaitForSingleObject(s, (DWORD)(timeout == (time_t)INFINITE ? timeout : timeout*1000));
        assert(rc == WAIT_OBJECT_0 || rc == WAIT_TIMEOUT);
        mutex.lock();
    }
    void signal(unsigned inc = 1) {
        if (inc != 0) {
            ReleaseSemaphore(s, inc, NULL);
        }
    }
    void open(unsigned initValue = 0) {
        s = CreateSemaphore(NULL, initValue, dbMaxSemValue, NULL);
        assert(s != NULL);
    }
    void close() {
        CloseHandle(s);
    } 
    dbSemaphore() { 
        s = NULL;
    }
};

class GIGABASE_DLL_ENTRY dbEvent {
    HANDLE e;
    int    nWaitingThreads;
    int    nPulses;
  public:
    void wait(dbMutex& mutex, time_t timeout = INFINITE) {
        nWaitingThreads += 1;
        mutex.unlock();
        int rc = WaitForSingleObject(e, (DWORD)(timeout == (time_t)INFINITE ? timeout : timeout*1000));
        assert(rc == WAIT_OBJECT_0 || rc == WAIT_TIMEOUT);
        mutex.lock();
        nWaitingThreads -= 1;
        if (nPulses > 0) { 
            nPulses -= 1;
            ResetEvent(e);
        }
    }
    void signal() {
        SetEvent(e);
    }
    void reset() {
        ResetEvent(e);
    }
    void pulse() { 
        if (nWaitingThreads > 0) { 
            nPulses += 1;
            SetEvent(e);
        }
    }    
    void open(bool initValue = false) {
        e = CreateEvent(NULL, true, initValue, NULL);
        nWaitingThreads = 0;
        nPulses = 0;
    }
    void close() {
        CloseHandle(e);
    }
    dbEvent() { 
        e = NULL;
    }
};

template<class T>
class dbThreadContext {
    int index;
  public:
    T* get() {
        return (T*)TlsGetValue(index);
    }
    void set(T* value) {
        TlsSetValue(index, value);
    }
    dbThreadContext() {
        index = TlsAlloc();
        assert(index != (int)TLS_OUT_OF_INDEXES);
    }
    ~dbThreadContext() {
        TlsFree(index);
    }
};

#else // Unix

#define thread_proc

#ifndef NO_PTHREADS

END_GIGABASE_NAMESPACE

#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>

BEGIN_GIGABASE_NAMESPACE

class dbMutex {
    friend class dbEvent;
    friend class dbSemaphore;
    pthread_mutex_t cs;
    bool            initialized;
  public:
    dbMutex() {
        pthread_mutex_init(&cs, NULL);
        initialized = true;
    }
    ~dbMutex() {
        pthread_mutex_destroy(&cs);
        initialized = false;
    }
    bool isInitialized() { 
        return initialized;
    }
    void lock() {
        if (initialized) { 
            pthread_mutex_lock(&cs);
        }
    }
    void unlock() {
        if (initialized) { 
            pthread_mutex_unlock(&cs);
        }
    }
};


const size_t dbThreadStackSize = 1024*1024;

class dbThread {
    pthread_t thread;
  public:
    typedef void (thread_proc* thread_proc_t)(void*);

    void create(thread_proc_t f, void* arg) {
        pthread_attr_t attr;
        pthread_attr_init(&attr);
#if !defined(__linux__)
        pthread_attr_setstacksize(&attr, dbThreadStackSize);
#endif
#if defined(_AIX41)
        // At AIX 4.1, 4.2 threads are by default created detached
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_UNDETACHED);
#endif
        pthread_create(&thread, &attr, (void*(*)(void*))f, arg);
        pthread_attr_destroy(&attr);
    }

    enum ThreadPriority { 
        THR_PRI_LOW, 
        THR_PRI_HIGH
    };
#if defined(PRI_OTHER_MIN) && defined(PRI_OTHER_MAX)
    void setPriority(ThreadPriority pri) { 
        struct sched_param sp;
        sp.sched_priority = pri == THR_PRI_LOW ? IPRI_OTHER_MIN : PRI_OTHER_MAX;
        pthread_setschedparam(thread, SCHED_OTHER, &sp); 
    }
#else
    void setPriority(ThreadPriority) {}
#endif



    void join() {
        void* result;
        pthread_join(thread, &result);
    }
    void detach() {
        pthread_detach(thread);
    }
    static int numberOfProcessors();
};

#if defined(_SC_NPROCESSORS_ONLN)
inline int dbThread::numberOfProcessors() {
    return sysconf(_SC_NPROCESSORS_ONLN);
}
#elif defined(__linux__)
END_GIGABASE_NAMESPACE
#include <linux/smp.h>
BEGIN_GIGABASE_NAMESPACE
inline int dbThread::numberOfProcessors() { return smp_num_cpus; }
#elif defined(__FreeBSD__) || defined(__bsdi__) || defined(__OpenBSD__)
#if defined(__bsdi__) || defined(__OpenBSD__)
END_GIGABASE_NAMESPACE
#include <sys/param.h>
BEGIN_GIGABASE_NAMESPACE
#endif
END_GIGABASE_NAMESPACE
#include <sys/sysctl.h>
BEGIN_GIGABASE_NAMESPACE
inline int dbThread::numberOfProcessors() {
    int mib[2],ncpus=0;
    size_t len=sizeof(ncpus);
    mib[0]= CTL_HW;
    mib[1]= HW_NCPU;
    sysctl(mib,2,&ncpus,&len,NULL,0);
    return ncpus;
}
#else
#warning Do not know how to detect number of processors: assuming 1
inline int dbThread::numberOfProcessors() { return 1; }
#endif

class dbEvent {
    pthread_cond_t   cond;
    int              signaled;
    long             n_signals;

  public:
    void wait(dbMutex& mutex) {
        long before_n_signals = n_signals;
        while (!signaled && n_signals == before_n_signals) {
            pthread_cond_wait(&cond, &mutex.cs);
        }
    }

    void wait(dbMutex& mutex, time_t timeout) {
        long before_n_signals = n_signals;
        while (!signaled && n_signals == before_n_signals) {
            struct timespec abs_ts; 
#ifdef PTHREAD_GET_EXPIRATION_NP
            struct timespec rel_ts; 
            rel_ts.tv_sec = timeout; 
            rel_ts.tv_nsec = 0;
            pthread_get_expiration_np(&rel_ts, &abs_ts);
#else
            struct timeval cur_tv;
            gettimeofday(&cur_tv, NULL);
            abs_ts.tv_sec = cur_tv.tv_sec + timeout; 
            abs_ts.tv_nsec = cur_tv.tv_usec*1000;
#endif
            pthread_cond_timedwait(&cond, &mutex.cs, &abs_ts);
        }
    }

    void signal() {
        signaled = true; 
        n_signals += 1;
        pthread_cond_broadcast(&cond);
    }

    void pulse() { 
        n_signals += 1;
        pthread_cond_broadcast(&cond);
    }
        
    void reset() {
        signaled = false;
    }
    void open(bool initValue = false) {
        signaled = initValue;
        n_signals = 0;
        pthread_cond_init(&cond, NULL);
    }
    void close() {
        pthread_cond_destroy(&cond);
    }
};

class dbSemaphore {
    pthread_cond_t   cond;
    int              count;
  public:
    void wait(dbMutex& mutex) {
        while (count == 0) {
            pthread_cond_wait(&cond, &mutex.cs);
        }
        count -= 1;
    }

    void wait(dbMutex& mutex, time_t timeout) {
        while (count == 0) {
            struct timespec abs_ts; 
#ifdef PTHREAD_GET_EXPIRATION_NP
            struct timespec rel_ts; 
            rel_ts.tv_sec = timeout; 
            rel_ts.tv_nsec = 0;
            pthread_get_expiration_np(&rel_ts, &abs_ts);
#else
            struct timeval cur_tv;
            gettimeofday(&cur_tv, NULL);
            abs_ts.tv_sec = cur_tv.tv_sec + timeout; 
            abs_ts.tv_nsec = cur_tv.tv_usec*1000;
#endif
            pthread_cond_timedwait(&cond, &mutex.cs, &abs_ts);
        }
        count -= 1;
    }

    void signal(unsigned inc = 1) {
        count += inc;
        if (inc > 1) {
            pthread_cond_broadcast(&cond);
        } else if (inc == 1) {
            pthread_cond_signal(&cond);
        }
    }
    void open(unsigned initValue = 0) {
        pthread_cond_init(&cond, NULL);
        count = initValue;
    }
    void close() {
        pthread_cond_destroy(&cond);
    }
};

template<class T>
class dbThreadContext {
    pthread_key_t key;
  public:
    T* get() {
        return (T*)pthread_getspecific(key);
    }
    void set(T* value) {
        pthread_setspecific(key, value);
    }
    dbThreadContext() {
        pthread_key_create(&key, NULL);
    }
    ~dbThreadContext() {
        pthread_key_delete(key);
    }
};

#else

class dbMutex {
    bool initialized;

   public:
    dbMutex() {
        initialized = true;
    }

    ~dbMutex() { 
        initialized = false;
    }

    bool isInitialized() { 
        return initialized;
    }

    void lock() {}
    void unlock() {}
};

class dbThread {
  public:
    typedef void (thread_proc* thread_proc_t)(void*);
    void create(thread_proc_t f, void* arg) { f(arg); }
    void join() {}
    void detach() {}
    
    enum ThreadPriority { 
        THR_PRI_LOW, 
        THR_PRI_HIGH
    };
    void setPriority(ThreadPriority) {}

    static int numberOfProcessors() { return 1; }
};

class dbSemaphore {
    int count;
  public:
    void wait(dbMutex&, time_t=0) {
        assert (count > 0);
        count -= 1;
    }
    void signal(unsigned inc = 1) {
        count += inc;
    }
    void open(unsigned initValue = 0) {
        count = initValue;
    }
    void close() {}
};

class dbEvent {
    bool signaled;
  public:
    void wait(dbMutex&, time_t=0) {
        assert(signaled);
    }
    void signal() {
        signaled = true;
    }
    void reset() {
        signaled = false;
    }
    void open(bool initValue = false) {
        signaled = initValue;
    }
    void pulse() {}
    void close() {}
};

template<class T>
class dbThreadContext {
    T* value;
  public:
    T* get() {
        return value;
    }
    void set(T* value) {
        this->value = value;
    }
    dbThreadContext() { value = NULL; }
};


#endif

#endif

class GIGABASE_DLL_ENTRY dbCriticalSection {
  private:
    dbMutex& mutex;
  public:
    dbCriticalSection(dbMutex& guard) : mutex(guard) {
        mutex.lock();
    }
    ~dbCriticalSection() {
        mutex.unlock();
    }
};

#define SMALL_BUF_SIZE 512

template<class T>
class dbSmallBuffer {
  protected:
    T      smallBuf[SMALL_BUF_SIZE];
    T*     buf;
    size_t used;

  public:
    dbSmallBuffer(size_t size) {
        if (size > SMALL_BUF_SIZE) {
            buf = new T[size];
        } else {
            buf = smallBuf;
        }
        used = size;
    }

    dbSmallBuffer() { 
        used = 0;
        buf = smallBuf;
    }

    void put(size_t size) { 
        if (size > SMALL_BUF_SIZE && size > used) { 
            if (buf != smallBuf) { 
                delete[] buf;
            }
            buf = new T[size];
            used = size;
        }
    }

    operator T*() { return buf; }
    T* base() { return buf; }

    ~dbSmallBuffer() {
        if (buf != smallBuf) {
            delete[] buf;
        }
    }
};


class GIGABASE_DLL_ENTRY dbPooledThread { 
  private:
    friend class dbThreadPool;

    dbThread                thread;
    dbThreadPool*           pool;
    dbPooledThread*         next;
    dbThread::thread_proc_t f;
    void*                   arg;
    bool                    running;
    dbSemaphore             startSem;
    dbSemaphore             readySem;
    
    static void thread_proc  pooledThreadFunc(void* arg);

    void run();
    void stop();

    dbPooledThread(dbThreadPool* threadPool); 
    ~dbPooledThread(); 
};

class GIGABASE_DLL_ENTRY dbThreadPool { 
    friend class dbPooledThread;
    dbPooledThread* freeThreads;
    dbMutex         mutex;

  public:
    dbPooledThread* create(dbThread::thread_proc_t f, void* arg);
    void join(dbPooledThread* thr);
    dbThreadPool();
    ~dbThreadPool();
};    

END_GIGABASE_NAMESPACE

#endif
