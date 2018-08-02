//-< SHMEM.CPP >-----------------------------------------------------*--------*
// SHMEM                      Version 1.0        (c) 1998  GARRET    *     ?  *
// (Shared Memory Manager)                                           *   /\|  *
//                                                                   *  /  \  *
//                          Created:     28-Mar-98    K.A. Knizhnik  * / [] \ *
//                          Last update: 12-Apr-98    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Shared memory implementation
//-------------------------------------------------------------------*--------*

#include "shmem.h"
#include <stdio.h>
#include <string.h>

p_call_exit_handler global_func_log;

#ifdef _WIN32
#include <malloc.h>

#define MAX_SEM_VALUE 1000000

#ifdef USE_BASED_POINTERS
void* shared_memory_base_pointer;
#endif

inline bool shared_memory::enter_critical_section(status& result)
{
    if (InterlockedDecrement(&pMonitor->cs) != 0) { 
	// another process is in critical section
	int rc = WaitForSingleObject(hCS, INFINITE);
	if (rc != WAIT_OBJECT_0) { 
	    result = status(GetLastError());
	    return false;
	}
    }
    return true;
}

inline bool shared_memory::leave_critical_section(status& result)
{
    if (InterlockedIncrement(&pMonitor->cs) <= 0) { 
	// some other processes try to enter critical section
	SetEvent(hCS);
    }
    return true;
}

shared_memory::status shared_memory::lock(lock_descriptor& lck, unsigned msec)
{
    //assert(lck.mode == lck_shared || lck.mode == lck_exclusive);

	if(!(lck.mode == lck_shared || lck.mode == lck_exclusive))
			global_func_log(__LINE__,__FILE__, NULL);

    unsigned self = GetCurrentThreadId();
    status result;

    if (self == owner) 
	{ 
	//assert(n_nested_exclusive_locks != 0 && pMonitor->n_writers == 1);

	if(!(n_nested_exclusive_locks != 0 && pMonitor->n_writers == 1))
			global_func_log(__LINE__,__FILE__, NULL);

	if (lck.mode == lck_exclusive) { 
	    n_nested_exclusive_locks += 1;
	} else {
	    pMonitor->n_readers += 1;
	    lck.owner = self;
	    lck.next = shared_lock_chain;
	    shared_lock_chain = &lck;
	}
	return ok;
    }
    if (!enter_critical_section(result)) { 
	return result;
    }
    if (lck.mode == lck_shared) { 
	while (pMonitor->n_writers != 0) { 
	    // storage is exclusivly locked by another process
	    if (msec != 0) { 
		if (pMonitor->n_waiters++ == 0) { 
		    ResetEvent(hEvent);
		}
	    }
	    if (!leave_critical_section(result)) { 
		return result;
	    }
	    if (msec == 0) { 
		return timeout_expired;
	    }
	    int rc = WaitForSingleObject(hEvent, msec);  
	    if (rc == WAIT_TIMEOUT) { 
		msec = 0;
	    } else if (rc != WAIT_OBJECT_0) {
		return status(GetLastError());
	    }		 
	    ReleaseSemaphore(hSemaphore, 1, NULL);
	    if (!enter_critical_section(result)) { 
		return result;
	    }
	} 
	pMonitor->n_readers += 1;
	lck.owner = self;
	lck.next = shared_lock_chain;
	shared_lock_chain = &lck;
    } else { // exclusive lock
	while (pMonitor->n_writers != 0 
	       || (pMonitor->n_readers != 0 
		   && (pMonitor->n_readers != 1 
		       || shared_lock_chain == NULL
		       || shared_lock_chain->owner != self))) 
        { 
	    // storage is locked by another process
	    if (msec != 0) { 
		if (pMonitor->n_waiters++ == 0) { 
		    ResetEvent(hEvent);
		}
	    }
	    if (!leave_critical_section(result)) { 
		return result;
	    }
	    if (msec == 0) { 
		return timeout_expired;
	    }
	    int rc = WaitForSingleObject(hEvent, msec);  
	    if (rc == WAIT_TIMEOUT) { 
		msec = 0;
	    } else if (rc != WAIT_OBJECT_0) {
		return status(GetLastError());
	    }		 
	    ReleaseSemaphore(hSemaphore, 1, NULL);
	    if (!enter_critical_section(result)) { 
		return result;
	    }
	} 
	pMonitor->n_writers = 1;
	n_nested_exclusive_locks = 1;
	owner = self;
    }
    return leave_critical_section(result) ? ok : result;
}
    


shared_memory::status shared_memory::unlock(lock_descriptor& lck)
{
    status result;
    //assert(lck.mode == lck_shared || lck.mode == lck_exclusive);

	if(!(lck.mode == lck_shared || lck.mode == lck_exclusive))
		global_func_log(__LINE__,__FILE__, NULL);


    if (lck.mode == lck_exclusive) 
	{ 
	//assert(GetCurrentThreadId() == owner && n_nested_exclusive_locks > 0);
	if(!(GetCurrentThreadId() == owner && n_nested_exclusive_locks > 0))
		global_func_log(__LINE__,__FILE__, NULL);

	if (--n_nested_exclusive_locks != 0) { 
	    return ok;
	}
	owner = 0;
    }

    if (!enter_critical_section(result)) { 
	return result;
    }
    
    if (lck.mode == lck_shared) { 
	lock_descriptor *lp, **lpp = &shared_lock_chain;
	while ((lp = *lpp) != &lck) { 
	    //assert(lp != NULL/*invalid lock descriptor*/);
		if(!(lp != NULL))
			global_func_log(__LINE__,__FILE__, NULL);

	    lpp = &lp->next;
	}
	//assert(pMonitor->n_readers > 0 && lp->owner == GetCurrentThreadId());
	if(!(pMonitor->n_readers > 0 && lp->owner == GetCurrentThreadId()))
			global_func_log(__LINE__,__FILE__, NULL);

	*lpp = lp->next;
	pMonitor->n_readers -= 1;
    } else { 
	//assert(pMonitor->n_writers == 1);
	if(!(pMonitor->n_writers == 1))
			global_func_log(__LINE__,__FILE__, NULL);

	pMonitor->n_writers = 0;
    }
    if (pMonitor->n_waiters != 0) { 
	SetEvent(hEvent);
	do { 
	    if (WaitForSingleObject(hSemaphore, INFINITE) != WAIT_OBJECT_0) {
		return status(GetLastError());
	    }
	} while (--pMonitor->n_waiters != 0);
    }
    return leave_critical_section(result) ? ok : result;
}


shared_memory::status shared_memory::open(const char* file_name, 
					  const char* shared_name,
					  size_t max_size,
					  open_mode mode,
					  void* desired_address)
{
    int len = strlen(shared_name);
    int error;
    char* global_name = (char*)alloca(len + 2);
    memcpy(global_name, shared_name, len);
    global_name[len+1] = '\0';
    
    this->mode = mode;

    if (file_name != NULL) { 
	hFile = CreateFile(file_name, GENERIC_READ|GENERIC_WRITE,
			   FILE_SHARE_READ|FILE_SHARE_WRITE,
			   NULL, OPEN_ALWAYS, 
			   FILE_FLAG_WRITE_THROUGH|FILE_FLAG_RANDOM_ACCESS,
			   NULL);
	if (hFile == INVALID_HANDLE_VALUE) { 
	    goto return_error;
	}	
    } 
    global_name[len] = '1';
    hMutex = CreateMutex(NULL, true, global_name);
    if (hMutex == NULL) {
	goto return_error;
    }
    global_name[len] = '2';
    if (GetLastError() == ERROR_ALREADY_EXISTS) { 
	if (WaitForSingleObject(hMutex, INFINITE) != WAIT_OBJECT_0) { 
	    goto return_error;
	}
	hMonitor = OpenFileMapping(FILE_MAP_ALL_ACCESS, false, global_name);
	if (hMonitor == NULL) { 
	    goto return_error;
	}
	pMonitor = (monitor*)MapViewOfFile(hMonitor,FILE_MAP_ALL_ACCESS,0,0,0);
	if (pMonitor == NULL) { 
	    goto return_error;
	}
	int access = (mode == read_only) ? FILE_MAP_READ : FILE_MAP_ALL_ACCESS;
	global_name[len] = '3';
	hMap = OpenFileMapping(access, false, global_name);
	if (hMap == NULL) { 
	    goto return_error;
	}
	pHdr = (header*)MapViewOfFileEx(hMap, access, 0, 0, 0,
					pMonitor->base_address);
#ifdef USE_BASED_POINTERS
	//assert(shared_memory_base_pointer == NULL);
	if(!(shared_memory_base_pointer == NULL))
		global_func_log(__LINE__,__FILE__, NULL);

	// only one opened section is possible
	if (pHdr == NULL) { 
	    pHdr = (header*)MapViewOfFileEx(hMap, access, 0, 0, 0, NULL);
	    if (pHdr == NULL) { 
		goto return_error;
	    }
	    shared_memory_base_pointer = 
		(void*)((char*)pHdr - (char*)pMonitor->base_address);
	}
#else
	if (pHdr == NULL) { 
	    goto return_error;
	}
#endif
	pMonitor->n_sessions += 1;
    } else {  // intialized shared memory
	hMonitor = CreateFileMapping(INVALID_HANDLE_VALUE,
				     NULL, PAGE_READWRITE, 0, 
				     sizeof(monitor), global_name);
	if (hMonitor == NULL) { 
	    goto return_error;
	}
	pMonitor = (monitor*)MapViewOfFile(hMonitor,FILE_MAP_ALL_ACCESS,0,0,0);
	if (pMonitor == NULL) { 
	    goto return_error;
	}
	pMonitor->n_readers = 0;
	pMonitor->n_writers = 0;
	pMonitor->n_waiters = 0;
	pMonitor->cs = 1;   
	bool initialized = false;
	if (hFile != INVALID_HANDLE_VALUE) { 
	    header hdr;
	    DWORD read_bytes;
	    if (!ReadFile(hFile, &hdr, sizeof hdr, &read_bytes, NULL)) { 
		goto return_error;
	    }
	    if (read_bytes == sizeof hdr) { 
		if (hdr.size > max_size) { 
		    max_size = hdr.size;
		}
		desired_address = hdr.base_address;
		initialized = true;
	    }
	}
	pMonitor->max_file_size = max_size;
	global_name[len] = '3';
	hMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, 
				 max_size, global_name);
	if (hMap == NULL) { 
	    goto return_error;
	}
	pHdr = (header*)MapViewOfFileEx(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0,
					desired_address);
#ifdef USE_BASED_POINTERS
	//assert(shared_memory_base_pointer == NULL); 
	if(!(shared_memory_base_pointer == NULL))
		global_func_log(__LINE__,__FILE__, NULL);

	// only one opened section is possible
	if (pHdr == NULL) { 
	    pHdr = (header*)MapViewOfFileEx(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0,
					    NULL);
	    if (pHdr == NULL) { 
		goto return_error;
	    }
	    pMonitor->base_address = pHdr;
	    if (initialized) { 
		shared_memory_base_pointer = 
		    (void*)((char*)pHdr - (char*)desired_address);
		pMonitor->base_address = (header*)desired_address;
	    }
	} else { 
	    pMonitor->base_address = pHdr;
	} 
#else
	if (pHdr == NULL) { 
	    goto return_error;
	}
	pMonitor->base_address = pHdr;
#endif
	pMonitor->n_sessions = 1;
	if (!initialized) { 
	    // initialize file
	    pHdr->base_address = pHdr;
	    pHdr->size = sizeof(header);
	    pHdr->root_object = NULL;
	    pHdr->barrier = -1;
	    pHdr->free_list.forward = 0;
	    pHdr->free_list.next = pHdr->free_list.prev = (REF(free_block))&pHdr->free_list;
	    pHdr->free_list_pos = (REF(free_block))&pHdr->free_list;
	}
    }
    global_name[len] = '4';
    hCS = CreateEvent(NULL, false, false, global_name);
    if (hCS == NULL) { 
	goto return_error;
    }
    global_name[len] = '5';
    hEvent = CreateEvent(NULL, true, false, global_name);
    if (hEvent == NULL) { 
	goto return_error;
    }
    global_name[len] = '6';
    hSemaphore = CreateSemaphore(NULL, 0, MAX_SEM_VALUE, global_name);
    if (hSemaphore == NULL) { 
	goto return_error;
    }
    owner = 0;
    n_nested_exclusive_locks = 0;
    shared_lock_chain = NULL;

    ReleaseMutex(hMutex);

    return ok;

  return_error:
    error = GetLastError();
    if (hMutex != NULL) { 
	ReleaseMutex(hMutex);
    }
    close();
    return status(error);
}


shared_memory::status shared_memory::flush()
{
    if (!FlushViewOfFile(pHdr, pHdr->size)) { 
	return status(GetLastError());
    }
    return ok;
}



void shared_memory::close()
{
    if (hMutex != NULL) { 
	WaitForSingleObject(hMutex, INFINITE);
    }

    size_t file_size = 0;
    bool reset_file_size = false;
    if (pHdr != NULL) { 
	file_size = pHdr->size;
	if (--pMonitor->n_sessions == 0) { 
	    reset_file_size = true;
	}
	UnmapViewOfFile(pHdr);
	pHdr = NULL;
    }
#ifdef USE_BASED_POINTERS
    shared_memory_base_pointer = NULL;
#endif
    if (pMonitor != NULL) { 
	UnmapViewOfFile(pMonitor);
	pMonitor = NULL;
    }
    if (hMap != NULL) { 
	CloseHandle(hMap);
	hMap = NULL;
    }
    if (hMonitor != NULL) { 
	CloseHandle(hMonitor);
	hMonitor = NULL;
    }
    if (hEvent != NULL) { 
	CloseHandle(hEvent);
	hEvent = NULL;
    }
    if (hSemaphore != NULL) { 
	CloseHandle(hSemaphore);
	hSemaphore = NULL;
    }
    if (hCS != NULL) { 
	CloseHandle(hCS);
	hCS = NULL;
    }
    if (hFile != INVALID_HANDLE_VALUE) { 
	if (reset_file_size) { 
	    SetFilePointer(hFile, file_size, NULL, FILE_BEGIN);
	    SetEndOfFile(hFile);
	} 
	CloseHandle(hFile);
	hFile = INVALID_HANDLE_VALUE;
    }
    if (hMutex != NULL) { 
	HANDLE h = hMutex;
	hMutex = NULL;
	ReleaseMutex(h);
	CloseHandle(h);
    }
}

shared_memory::shared_memory()
{
    hFile = INVALID_HANDLE_VALUE;
    hMutex = NULL;
    hEvent = NULL;
    hSemaphore = NULL;
    hCS = NULL;
    hMonitor = NULL;
    hMap = NULL;

    pMonitor = NULL;
    pHdr = NULL;
}

char* shared_memory::get_error_text(status code, char* buf, size_t buf_size) 
const 
{
    char* err_txt;
    char errbuf[64];
    switch (code) {
      case ok: 
	err_txt = "no error";
	break;
      case timeout_expired:
	err_txt = "lock timeout expired";
	break;
      default:
	if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
			  NULL, int(code), 0,
			  buf, buf_size, NULL) > 0)
	{
	    return buf;
	} else { 
	    sprintf(errbuf, "unknown error code %u", int(code));
	    err_txt = errbuf;
	}
    }
    return strncpy(buf, err_txt, buf_size);
}

//
// Synchronization primitives implementations
//

bool semaphore::wait(unsigned msec)
{
    int rc = WaitForSingleObject(s, msec);
    
	if(rc == WAIT_FAILED) //in chiusura di programma si finisce qui
	{
		/*
		LPVOID lpMsgBuf;
			FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL 
		);
		// Process any inserts in lpMsgBuf.
		// ...
		// Display the string.
		MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
		// Free the buffer.
		LocalFree( lpMsgBuf );
		*/

		return rc == WAIT_OBJECT_0;
	}

	//assert(rc == WAIT_OBJECT_0 || rc == WAIT_TIMEOUT);

	if(!(rc == WAIT_OBJECT_0 || rc == WAIT_TIMEOUT))
		global_func_log(__LINE__,__FILE__, NULL);

    return rc == WAIT_OBJECT_0;
}

void semaphore::signal(unsigned inc)
{
    if (inc != 0) { 
	ReleaseSemaphore(s, inc, NULL);
    }
}

bool semaphore::open(char const* name, unsigned init_value)
{
    s = CreateSemaphore(NULL, init_value, MAX_SEM_VALUE, name);
    return s != NULL; 
}

void semaphore::close()
{
    CloseHandle(s);
}

bool event::wait(unsigned msec)
{
    int rc = WaitForSingleObject(e, msec);
    //assert(rc == WAIT_OBJECT_0 || rc == WAIT_TIMEOUT);
	if(!(rc == WAIT_OBJECT_0 || rc == WAIT_TIMEOUT))
		global_func_log(__LINE__,__FILE__, NULL);

    return rc == WAIT_OBJECT_0;
}

void event::signal()
{
    SetEvent(e);
}

void event::reset()
{
    ResetEvent(e);
}

bool event::open(char const* name, bool signaled)
{
    e = CreateEvent(NULL, true, signaled, name);
    return e != NULL; 
}

void event::close()
{
    CloseHandle(e);
}

#else // __unix__


#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <signal.h>

#define READERS_SEM 0
#define WRITERS_SEM 1

#define N_SEMS      2

union semun {
    int val;                    /* value for SETVAL */
    struct semid_ds *buf;       /* buffer for IPC_STAT, IPC_SET */
    unsigned short int *array;  /* array for GETALL, SETALL */
    struct seminfo *__buf;      /* buffer for IPC_INFO */
};

//
// It will takes a lot of time and disk space to produce core file
// for program with huge memory mapped segment. This option allows you
// to automatically start debugger when some exception is happened in program.
//
#define CATCH_SIGNALS 1

#ifdef CATCH_SIGNALS
#include <signal.h>

#define WAIT_DEBUGGER_TIMEOUT 10

static void fatal_error_handler(int signo) { 
    char  buf[64];
    char* exe_name = getenv("_");
    fprintf(stderr, "\
Program catch signal %d.\n\
Input path to executable file to debug or press Ctrl/C to terminate program:\n\
[%s] ", 
	    signo, exe_name);
    int pid = getpid();
    if (fgets(buf, sizeof buf, stdin)) { 
	char pid_str[16];
	int len = strlen(buf);
	if (len > 1) { 
	    buf[len-1] = '\0'; // truncate '\n'
	    exe_name = buf;
	}
	sprintf(pid_str, "%d", pid); 
	if (fork()) { 
	    sleep(WAIT_DEBUGGER_TIMEOUT);
	} else { 
	    execlp("gdb", "gdb", "-q", "-s", exe_name, exe_name, pid_str, NULL);
	}
    } else { 
	kill(pid, SIGKILL);
    }
}

class install_handlers { 
  public:
    struct sigaction sigact; 

    install_handlers() { 
	sigact.sa_flags = 0;
	sigact.sa_handler = fatal_error_handler;
	sigaction(SIGSEGV, &sigact, NULL);
	sigaction(SIGBUS, &sigact, NULL);
	sigaction(SIGILL, &sigact, NULL);
	sigaction(SIGABRT, &sigact, NULL);
    }
};

static install_handlers catch_all;
#endif


inline bool shared_memory::enter_critical_section(status& result)
{
    if (shmsem_lock(&cs) != 0) { 
	result = status(errno);
	return false;
    }
    return true;
}

inline bool shared_memory::leave_critical_section(status& result)
{
    if (shmsem_unlock(&cs) != 0) {
	result = status(errno);
	return false;
    }
    return true;
}


enum wait_status { wait_ok, wait_timeout_expired, wait_error };

static wait_status wait_semaphore(semp_t& sem, unsigned msec, 
				  struct sembuf* sops, int n_sops)
{
    if (msec != INFINITE) { 
	struct timeval start;
	struct timeval stop;
	gettimeofday(&start, NULL);
	unsigned long usec = start.tv_usec + msec % 1000 * 1000;
	stop.tv_usec = usec % 1000000;
	stop.tv_sec = start.tv_sec + msec / 1000 + usec / 1000000;

	while (true) { 
	    struct itimerval it;
	    it.it_interval.tv_sec = 0;
	    it.it_interval.tv_usec = 0;
	    it.it_value.tv_sec = stop.tv_sec - start.tv_sec;
	    it.it_value.tv_usec = stop.tv_usec - start.tv_usec;
	    if (stop.tv_usec < start.tv_usec) { 
		it.it_value.tv_usec += 1000000;
		it.it_value.tv_sec -= 1;
	    }
	    if (setitimer(ITIMER_REAL, &it, NULL) < 0) { 
		return wait_error;
	    }
	    if (semop(sem, sops, n_sops) == 0) { 
		break;
	    }
	    if (errno != EINTR) { 
		return wait_error;
	    }
	    gettimeofday(&start, NULL);
	    if (stop.tv_sec < start.tv_sec || 
	       (stop.tv_sec == start.tv_sec && stop.tv_usec < start.tv_sec))
	    {
		return wait_timeout_expired;
	    }
	}
    } else { 
	while (semop(sem, sops, n_sops) < 0) { 
	    if (errno != EINTR) { 
		return wait_error;
	    }
	}
    }
    return wait_ok;
}

bool shared_memory::wait_resource(status& result, lck_t lck, 
				  unsigned self, unsigned msec)
{
    struct sembuf* sops;
    int n_sops;
    if (msec != 0) { 
	if (pMonitor->n_waiters++ == 0) { 
	    union semun u;
	    ushort semval[2];
	    semval[READERS_SEM] = pMonitor->n_readers;
	    semval[WRITERS_SEM] = pMonitor->n_writers;
	    u.array = semval;
	    if (semctl(sem, 0, SETALL, u) < 0) {
		result = status(errno);
		return false;
	    }
	}
    }
    if (lck == lck_shared) { 
	static struct sembuf set_shared_lock[2] = {
	    {WRITERS_SEM, 0, 0}, 
	    {READERS_SEM, 1, 0}
	};
	sops = set_shared_lock;
	n_sops = items(set_shared_lock);
    } else { 
	bool upgrade = false;
	for(lock_descriptor* lp = shared_lock_chain; lp != NULL; lp = lp->next)
	{
	    if (lp->owner == self) { 
		upgrade = true;
		break;
	    }
	}
	if (upgrade) { 
	    static struct sembuf upgrade_shared_lock[] = {
		{WRITERS_SEM, 0, 0}, 
		{READERS_SEM, -1, IPC_NOWAIT},
		{READERS_SEM, 0, 0},
		{READERS_SEM, 1, 0},
		{WRITERS_SEM, 1, 0}
	    };
	    sops = upgrade_shared_lock;
	    n_sops = items(upgrade_shared_lock);
	} else { 
	    static struct sembuf set_exclusive_lock[] = {
		{WRITERS_SEM, 0, 0}, 
		{READERS_SEM, 0, 0}, 
		{WRITERS_SEM, 1, 0}
	    };
	    sops = set_exclusive_lock;
	    n_sops = items(set_exclusive_lock);
	}
    } 
    if (!leave_critical_section(result)) { 
	return false;
    }
    if (msec == 0) { 
	result = timeout_expired;
	return false;
    }
    wait_status ws = wait_semaphore(sem, msec, sops, n_sops);
    if (ws == wait_error) { 
	result = status(errno);
	return false;
    }
    if (!enter_critical_section(result)) { 
	return false;
    }
    pMonitor->n_waiters -= 1;
    if (ws == wait_timeout_expired) { 
	leave_critical_section(result);
	result = timeout_expired;
	return false;
    }
    return true;
}


#ifdef _REENTRANT
#include <ptheard.h>
#else
#define pthread_self() 1
#endif


shared_memory::status shared_memory::lock(lock_descriptor& lck, unsigned msec)
{
    //assert(lck.mode == lck_shared || lck.mode == lck_exclusive);

	if(!(lck.mode == lck_shared || lck.mode == lck_exclusive))
		global_func_log(__LINE__,__FILE__,""); //TODO: find good cause

    unsigned self = pthread_self();
    status result;

    if (self == owner && lck.mode == lck_exclusive) { 
	//assert(n_nested_exclusive_locks != 0 && pMonitor->n_writers == 1);

	if(!(n_nested_exclusive_locks != 0 && pMonitor->n_writers == 1))
		global_func_log(__LINE__,__FILE__,""); //TODO: find good cause

	n_nested_exclusive_locks += 1;
	return ok;
    }
    if (!enter_critical_section(result)) { 
	return result;
    }
    if (self == owner) { 
	//assert(lck.mode == lck_shared);

	if(!(lck.mode == lck_shared))
		global_func_log(__LINE__,__FILE__,""); //TODO: find good cause

	if (pMonitor->n_waiters != 0) { 
	    static struct sembuf sops[] = {{READERS_SEM, 1, 0}}; 
	    if (semop(sem, sops, 1) < 0) {
		return status(errno);
	    }
	}
	pMonitor->n_readers += 1;
	lck.owner = self;
	lck.next = shared_lock_chain;
	shared_lock_chain = &lck;
    } else { 
	if (pMonitor->n_waiters != 0 
	    || (lck.mode == lck_shared && pMonitor->n_writers != 0) 
	    || (lck.mode == lck_exclusive 
		&& (pMonitor->n_writers != 0 
		    || (pMonitor->n_readers != 0 
			&& (pMonitor->n_readers != 1 
			    || shared_lock_chain == NULL
			    || shared_lock_chain->owner != self)))))
	    { 
	    if (!wait_resource(result, lck.mode, self, msec)) { 
		return result;
	    }
	}
	if (lck.mode == lck_shared) { 
	    //assert(pMonitor->n_writers == 0);

		if(!(pMonitor->n_writers == 0))
			global_func_log(__LINE__,__FILE__,""); //TODO: find good cause

	    pMonitor->n_readers += 1;
	    lck.owner = self;
	    lck.next = shared_lock_chain;
	    shared_lock_chain = &lck;
	} else { // exclusive lock
	    //assert(pMonitor->n_writers == 0 
		//   && (pMonitor->n_readers == 0 
		//       || (pMonitor->n_readers == 1 
		//	   && shared_lock_chain != NULL
		//	   && shared_lock_chain->owner == self)));

			if(!(pMonitor->n_writers == 0 
			   && (pMonitor->n_readers == 0 
				   || (pMonitor->n_readers == 1 
				   && shared_lock_chain != NULL
				   && shared_lock_chain->owner == self))))
			{
				global_func_log(__LINE__,__FILE__,""); //TODO: find good cause
			}

	    { 
		pMonitor->n_writers = 1;
		n_nested_exclusive_locks = 1;
		owner = self;
	    }
	}
    }
    return leave_critical_section(result) ? ok : result;
}
    


shared_memory::status shared_memory::unlock(lock_descriptor& lck)
{
    status result;
    //assert(lck.mode == lck_shared || lck.mode == lck_exclusive);

	if(!(lck.mode == lck_shared || lck.mode == lck_exclusive))
		global_func_log(__LINE__,__FILE__,""); //TODO: find good cause

    if (lck.mode == lck_exclusive) { 
	//assert(pthread_self() == owner && n_nested_exclusive_locks > 0);

	if(!(pthread_self() == owner && n_nested_exclusive_locks > 0))
		global_func_log(__LINE__,__FILE__,""); //TODO: find good cause

	if (--n_nested_exclusive_locks != 0) { 
	    return ok;
	}
	owner = 0;
    }

    if (!enter_critical_section(result)) { 
	return result;
    }
    
    if (lck.mode == lck_shared) { 
	lock_descriptor *lp, **lpp = &shared_lock_chain;
	while ((lp = *lpp) != &lck) { 
	    //assert(lp != NULL/*invalid lock descriptor*/);

		if(!(lp != NULL))
			global_func_log(__LINE__,__FILE__,""); //TODO: find good cause

	    lpp = &lp->next;
	}
	//assert(pMonitor->n_readers > 0 && lp->owner == pthread_self());

	if(!(pMonitor->n_readers > 0 && lp->owner == pthread_self()))
		global_func_log(__LINE__,__FILE__,""); //TODO: find good cause

	*lpp = lp->next;
	pMonitor->n_readers -= 1;
    } else { 
	//assert(pMonitor->n_writers == 1);

	if(!(pMonitor->n_writers == 1))
		global_func_log(__LINE__,__FILE__,""); //TODO: find good cause

	pMonitor->n_writers = 0;
    }
    if (pMonitor->n_waiters != 0) { 
	static struct sembuf sops[2] = {
	    {WRITERS_SEM, -1, IPC_NOWAIT}, 
	    {READERS_SEM, -1, IPC_NOWAIT}
	}; 
	if (semop(sem, sops + (lck.mode == lck_shared), 1) < 0) {
	    return status(errno);
	}
    }
    return leave_critical_section(result) ? ok : result;
}



#define INVALID_ADDRESS    ((void*)-1)
#define INVALID_DESCRIPTOR (-1)

static void alarm_handler(int){}

shared_memory::status shared_memory::open(const char* file_name, 
					  const char* shared_name,
					  size_t max_size,
					  open_mode mode,
					  void* desired_address)
{
    bool initialized = false;
    int error;
    key_t key;
    int   md;
    int   key_len;
    char* key_file_name;

    this->mode = mode;

    static struct sigaction sigact; 
    sigact.sa_handler = alarm_handler;
    sigaction(SIGALRM, &sigact, NULL);

    if (file_name != NULL) { 
	fd = ::open(file_name, O_RDWR|O_CREAT|O_SYNC, 0777);
	if (fd < 0) { 
	    goto return_error;
	}	
    }

    key_len = strlen(shared_name);
    key_file_name = new char[key_len+12];
    if (strchr(shared_name, '/') < 0) { 
	key_len += 5;
	strcat(strcpy(key_file_name, "/tmp/"), shared_name);
    } else { 
	strcpy(key_file_name, shared_name);
    }
    strcpy(key_file_name+key_len, ".mutex");

    if (semp_init(&mutex, key_file_name, 1) < 0) { 
	goto return_error;
    }
    if (semp_wait(&mutex) < 0) { 
	goto return_error;
    }
    strcpy(key_file_name+key_len, ".mon");
    md = ::open(key_file_name, O_RDWR|O_CREAT, 0777);
    if (md < 0) { 
	goto return_error;
    } 
    ::close(md);
    key = ftok(key_file_name, '0');
    if (key < 0) {
	goto return_error;
    }
    sem = semget(key, N_SEMS, IPC_CREAT|0777);
    if (sem < 0) { 
	goto return_error;
    }
    monid = shmget(key, sizeof(monitor), IPC_CREAT|0777);
    if (monid < 0) { 
	goto return_error;
    }
    pMonitor = (monitor*)shmat(monid, NULL, 0);
    if (pMonitor == INVALID_ADDRESS) { 
	goto return_error;
    }
    struct shmid_ds shm_desc;
    if (shmctl(monid, IPC_STAT, (struct shmid_ds*)&shm_desc) < 0) { 
	goto return_error;
    }
    if (shm_desc.shm_nattch == 1 || pMonitor->base_address == NULL) { 
	if (fd >= 0) { 
	    header hdr;
	    ssize_t rc = read(fd, &hdr, sizeof hdr);
	    if (rc < 0) {
		goto return_error;
	    }
	    if (size_t(rc) == sizeof hdr) { 
		if (hdr.size > max_size) { 
		    max_size = hdr.size;
		} else if (hdr.size < max_size) { 
		    if (ftruncate(fd, max_size) != 0) { 
			goto return_error;
		    }
		}
		desired_address = hdr.base_address;
		initialized = true;
	    } else { 
		if (ftruncate(fd, max_size) != 0) { 
		    goto return_error;
		}
	    }
	}
	pMonitor->max_file_size = max_size;
	pMonitor->n_readers = 0;
	pMonitor->n_writers = 0;
	pMonitor->n_waiters = 0;
	pMonitor->n_sessions = 0;
	if (shmsem_init(&cs, &pMonitor->cs) != 0) { 
	    goto return_error;
	}
    } else { 
	desired_address = pMonitor->base_address;
	max_size = pMonitor->max_file_size;
	if (shmsem_open(&cs, &pMonitor->cs) != 0) { 
	    goto return_error;
	}
	initialized = true;
    }
    if (fd >= 0) { 
	pHdr = (header*)mmap((char*)desired_address, max_size,
			     PROT_READ | (mode == read_only ? 0 : PROT_WRITE),
			     MAP_SHARED
			     | (desired_address ? MAP_FIXED : 0), fd, 0);
	if (pHdr == INVALID_ADDRESS) { 
	    goto return_error;
	}
    } else { 
	strcpy(key_file_name+key_len, ".shm");
	md = ::open(key_file_name, O_RDWR|O_CREAT, 0777);
	if (md < 0) { 
	    goto return_error;
	} 
	::close(md);
	key = ftok(key_file_name, '0');
	if (key < 0) {
	    goto return_error;
	}
	shmid = shmget(key, max_size, IPC_CREAT|0777);
	if (shmid < 0) { 
	    goto return_error;
	}
	pHdr = (header*)shmat(shmid, (char*)desired_address, 
			      mode == read_only ? SHM_RDONLY : 0);
	if (pHdr == INVALID_ADDRESS) {
	    goto return_error;
	}
	if (pHdr->base_address != NULL) { 
	    initialized = true;
	}
    }
    if (!initialized) { 
	// initialize file
	pHdr->base_address = pHdr;
	pHdr->size = sizeof(header);
	pHdr->root_object = NULL;
	pHdr->barrier = -1;
	pHdr->free_list.forward = 0;
	pHdr->free_list.next = pHdr->free_list.prev = &pHdr->free_list;
	pHdr->free_list_pos = &pHdr->free_list;
    }
    pMonitor->base_address = pHdr;
    pMonitor->n_sessions += 1;
    
    owner = 0;
    n_nested_exclusive_locks = 0;
    shared_lock_chain = NULL;

    if (semp_post(&mutex) < 0) { 
	goto return_error;
    }
    delete[] key_file_name;
    return ok;

  return_error:
    error = errno;
    delete[] key_file_name;
    if (mutex >= 0) { 
	semp_post(&mutex);
    }
    close();
    return status(error);
}


shared_memory::status shared_memory::flush()
{
    if (fd >= 0 && msync((char*)pHdr, pHdr->size, MS_SYNC) != 0) { 
	return status(errno);
    }
    return ok;
}


void shared_memory::close()
{
    if (mutex >= 0) { 
	semp_wait(&mutex); 
    }

    size_t file_size = 0;
    bool cleanup = false;
    if (pHdr != INVALID_ADDRESS) { 
	file_size = pHdr->size;
	if (--pMonitor->n_sessions == 0) { 
	    cleanup = true;
	}
	if (fd >= 0) { 
	    munmap((char*)pHdr, pMonitor->max_file_size);
	} else { 
	    shmdt((char*)pHdr);
	}
	pHdr = (header*)INVALID_ADDRESS;
    }
    if (pMonitor != INVALID_ADDRESS) { 
	shmdt((char*)pMonitor);
	pMonitor = (monitor*)INVALID_ADDRESS;
    }
    if (cleanup) { 
	if (cs.id >= 0) { 
	    shmsem_destroy(&cs);
	}
	if (shmid >= 0) { 
	    shmctl(shmid, IPC_RMID, NULL);
	}
	if (monid >= 0) { 
	    shmctl(monid, IPC_RMID, NULL);
	}
	if (sem >= 0) { 
	    union semun u;
	    u.val = 0;
	    semctl(sem, 0, IPC_RMID, u);
	}
    }
    cs.id = INVALID_DESCRIPTOR;
    shmid = INVALID_DESCRIPTOR;
    monid = INVALID_DESCRIPTOR;
    sem   = INVALID_DESCRIPTOR;

    if (fd >= 0) { 
	if (cleanup) { 
	    ftruncate(fd, file_size);
	}
	::close(fd);
	fd = INVALID_DESCRIPTOR;
    }
    if (mutex >= 0) { 
	if (cleanup) { 
	    semp_destroy(&mutex);
	} else { 
	    semp_post(&mutex);
	}
	mutex = INVALID_DESCRIPTOR;
    }
}

shared_memory::shared_memory()
{
    fd = INVALID_DESCRIPTOR;
    sem = INVALID_DESCRIPTOR;
    shmid = INVALID_DESCRIPTOR;
    monid = INVALID_DESCRIPTOR;
    cs.id = INVALID_DESCRIPTOR;
    mutex = INVALID_DESCRIPTOR;

    pMonitor = (monitor*)INVALID_ADDRESS;
    pHdr = (header*)INVALID_ADDRESS;
}

char* shared_memory::get_error_text(status code, char* buf, size_t buf_size) 
const 
{
    char* err_txt;
    switch (code) {
      case ok: 
	err_txt = "no error";
	break;
      case timeout_expired:
	err_txt = "lock timeout expired";
	break;
      default:
	err_txt = strerror(code);
    }
    return strncpy(buf, err_txt, buf_size);
}

//
// Synchronization primitives implementations
//

bool semaphore::wait(unsigned msec)
{
    static struct sembuf sops[] = {{0, -1, 0}};
    wait_status ws = wait_semaphore(s, msec, sops, items(sops));
    //assert(ws != wait_error);

	if(!(ws != wait_error))
		global_func_log(__LINE__,__FILE__,""); //TODO: find good cause

    return ws == wait_ok;
}

void semaphore::signal(unsigned inc)
{
    if (inc != 0) { 
	struct sembuf sops[1];
	sops[0].sem_num = 0;
	sops[0].sem_op  = inc;
	sops[0].sem_flg = 0;
	int rc = semop(s, sops, 1);
	//assert(rc == 0); 

	if(!(rc == 0))
		global_func_log(__LINE__,__FILE__,""); //TODO: find good cause

    }
}

bool semaphore::open(char const* name, unsigned init_value)
{
    return semp_init(&s, (char*)name, init_value) == 0;
}

void semaphore::close()
{
    // semp_destroy will delete semaphore even if other processes are using it
    // semp_destroy(&s);
}

bool event::wait(unsigned msec)
{
    static struct sembuf sops[] = {{0, -1, 0}, {0, 1, 0}};
    wait_status ws = wait_semaphore(e, msec, sops, items(sops));
   //assert(ws != wait_error);

	if(!(ws != wait_error))
		global_func_log(__LINE__,__FILE__,""); //TODO: find good cause

    return ws == wait_ok;
}

void event::signal()
{
    static struct sembuf sops[] = {{0, 0, IPC_NOWAIT}, {0, 1, 0}};
    int rc = semop(e, sops, items(sops));
    //assert(rc == 0 || errno == EAGAIN);

	if(!(rc == 0 || errno == EAGAIN))
		global_func_log(__LINE__,__FILE__,""); //TODO: find good cause
}

void event::reset()
{
    static struct sembuf sops[] = {{0, -1, IPC_NOWAIT}};
    int rc = semop(e, sops, items(sops));
    //assert(rc == 0 || errno == EAGAIN);

	if(!(rc == 0 || errno == EAGAIN))
		global_func_log(__LINE__,__FILE__,""); //TODO: find good cause
}

bool event::open(char const* name, bool signaled)
{
    return semp_init(&e, (char*)name, signaled) == 0;
}

void event::close()
{
    // semp_destroy will delete semaphore even if other processes are using it
    // semp_destroy(&e);
}


#endif
 
void* shared_memory::allocate(size_t size, bool initialize_by_zero)
{
    //assert(n_nested_exclusive_locks != 0);

	if(!(n_nested_exclusive_locks != 0))
		global_func_log(__LINE__,__FILE__, NULL);

    //assert(mode != read_only);

	if(!(mode != read_only))
		global_func_log(__LINE__,__FILE__, NULL);

    if (size == 0) {
	size = 1;
    }
    size = (size + 7) & ~7; // align on 8-byte boundary

    free_block* curr = pHdr->free_list_pos;
    free_block* bp = curr;

    while (true) { 
	if (size_t(bp->forward) >= size) { 
	    if (size_t(bp->forward) >= size + sizeof(free_block)) { 
		pHdr->free_list_pos = (REF(free_block))bp;
		int offs = bp->forward - size;
		bp->forward = offs - sizeof(allocation_block);
		bp = (free_block*)((char*)bp + offs);
		bp->backward = offs;
	    } else { 
		bp->unlink();
		size = bp->forward;
		pHdr->free_list_pos = bp->next;
	    }
	    break;
	} 	    
	bp = bp->next;
	if (bp == curr) { // hole not found
	    if (pHdr->size + size + sizeof(allocation_block) 
		> pMonitor->max_file_size)
	    {
		return NULL; // not enough memory
	    }
	    bp = (free_block*)((char*)pHdr + pHdr->size - sizeof(int));
	    pHdr->size += size + sizeof(allocation_block); 
	    break;
	}
    }
    bp->forward = -int(size);
    ((free_block*)((char*)bp + size + sizeof(allocation_block)))->backward =
	-int(size + sizeof(allocation_block));

    void* p = (char*)bp + sizeof(allocation_block);
    if (initialize_by_zero) { 
	memset(p, 0, size);
    }
    return p;
}

void* shared_memory::reallocate(void* ptr, size_t new_size, bool initialize_by_zero)
{
    free_block* bp = (free_block*)((char*)ptr - sizeof(allocation_block));
    size_t size = -bp->forward;
    if (new_size > size) { 
        void* new_ptr = allocate(new_size, initialize_by_zero);
        memcpy(new_ptr, ptr, size);
        free(ptr);
        return new_ptr;
    }
    return ptr;
}
   
void shared_memory::free(void* ptr)
{
    //assert(n_nested_exclusive_locks != 0);

	if(!(n_nested_exclusive_locks != 0))
		global_func_log(__LINE__,__FILE__, NULL);

    //assert(mode != read_only);

	if(!(mode != read_only))
		global_func_log(__LINE__,__FILE__, NULL);

    free_block* bp = (free_block*)((char*)ptr - sizeof(allocation_block));
    size_t size = -bp->forward;

    //assert(((free_block*)((char*)bp + size + sizeof(allocation_block)))->
	//   backward == -int(size + sizeof(allocation_block)));

	if(!(((free_block*)((char*)bp + size + sizeof(allocation_block)))->
	   backward == -int(size + sizeof(allocation_block))))
	{
		global_func_log(__LINE__,__FILE__, NULL);
	}


    if (bp->backward > 0) { 
	// previous block is free
	size += bp->backward;
	bp = (free_block*)((char*)bp - bp->backward);
    } else { 
	// insert block in l2-list of free blocks
	bp->link_after(&pHdr->free_list);
    }
    pHdr->free_list_pos = (REF(free_block))bp;

    size_t offs = (char*)bp - (char*)pHdr + sizeof(int);
    if (offs + size + sizeof(allocation_block) < pHdr->size) { 
	free_block* next = 
	    (free_block*)((char*)bp + size + sizeof(allocation_block));
	if (next->forward > 0) { 
	    // next block is free
	    next->unlink();
	    size += next->forward + sizeof(allocation_block);
	    bp->forward = size;
	    ((free_block*)((char*)bp + size + sizeof(allocation_block)))->
		backward = size + sizeof(allocation_block);
	} else { 
	    bp->forward = size;
	    next->backward = size + sizeof(allocation_block); 
	}
    } else { // last block in file
	//assert(offs + size + sizeof(allocation_block) == pHdr->size);

	if(!(offs + size + sizeof(allocation_block) == pHdr->size))
		global_func_log(__LINE__,__FILE__, NULL);

	// unlink block from l2-list of free blocks
	bp->unlink(); 	    
	// truncate file if object at the end of file was deallocated
	pHdr->size = offs;
	pHdr->free_list_pos = pHdr->free_list.next;
    }
}


void shared_memory::check_heap() const
{
    free_block* bp = (free_block*)((char*)pHdr + sizeof(header) - sizeof(int));
    free_block* eof = (free_block*)((char*)pHdr + pHdr->size - sizeof(int));

    while (bp < eof) {
	int size = bp->forward;
	if (size > 0) { // free object
	    size += sizeof(allocation_block);
	    bp = (free_block*)((char*)bp + size);
	} else { 
	    size -= sizeof(allocation_block);
	    bp = (free_block*)((char*)bp - size);
	}   
	//assert(size == bp->backward);
	if(!(size == bp->backward))
		global_func_log(__LINE__,__FILE__, NULL);
    }
    //assert(bp == eof);

	if(!(bp == eof))
		global_func_log(__LINE__,__FILE__, NULL);
}
