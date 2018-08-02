//-< SHMEM.H >-------------------------------------------------------*--------*
// SHMEM                      Version 1.0        (c) 1998  GARRET    *     ?  *
// (Shared Memory Manager)                                           *   /\|  *
//                                                                   *  /  \  *
//                          Created:     28-Mar-98    K.A. Knizhnik  * / [] \ *
//                          Last update: 12-Apr-98    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Shered Memory interface
//-------------------------------------------------------------------*--------*

#ifndef __SHMEM_H__
#define __SHMEM_H__

#include <stdlib.h>
#include <string.h>
#include <assert.h>

//#if defined(BUILDING_DLL)
//    #define SHMEM_DLL_ENTRY __declspec( dllexport )
//#elif defined(USING_DLL)
//    #define SHMEM_DLL_ENTRY __declspec( dllimport )
//#else
    #define SHMEM_DLL_ENTRY
//#endif

#ifdef _WIN32
#include <windows.h>
#else
#include "semp.h"
#define INFINITE (~0U)
#endif

typedef void (*p_call_exit_handler)(int line, char* file, char* reason);

extern p_call_exit_handler global_func_log;

#ifdef USE_BASED_POINTERS
extern SHMEM_DLL_ENTRY void* shared_memory_base_pointer;
#define REF(type) type __based(shared_memory_base_pointer)* 
#else
#define REF(type) type*
#endif

class SHMEM_DLL_ENTRY shared_memory {
  public:
    enum lck_t { lck_shared, lck_exclusive };
    enum open_mode { read_only, read_write };
    
    class SHMEM_DLL_ENTRY lock_descriptor { 
		friend class shared_memory;
		  public:
		const lck_t mode;
		lock_descriptor(lck_t lck) : mode(lck) {}
		  protected:
		lock_descriptor* next;
		unsigned         owner;
    };

    // Status returned by shared_memory methods are either connstants 
    // specified in the following enum or system error returned by GetLastError
    enum status {
	ok = 0, 
	timeout_expired = -1,
    };
    //
    // Lock stotorage either in shared either in exclusive mode.
    // Several processes can lock storage in shared mode, while only
    // one in exclusive. Parameter "msec" specifies timeout for 
    // waiting until lock will be granted. 
    //
    status lock(lock_descriptor& lck, unsigned msec = INFINITE);
    
    status unlock(lock_descriptor& lck);

    // alloc() function returns NULL if there are not enough free memory
    void*  allocate(size_t size, bool initialize_by_zero = true);
    void*  reallocate(void* ptr, size_t size, bool initialize_by_zero = true);
    void   free(void* ptr);

    void deallocate(void* obj) 
	{ 
		free(obj);
    }

    status open(const char* file_name, const char* shared_name,
		size_t max_size, open_mode mode = read_write,
		void* desired_address = NULL);
    status flush();
    void   close();

    char*  get_error_text(status code, char* buf, size_t buf_size) const;

    void*  get_root_object() const { return pHdr->root_object; }
    void   set_root_object(void* root) { pHdr->root_object = (REF(void))root; }

    void   check_heap() const;

    shared_memory();

  protected:
    
    struct allocation_block {
	// Offsets to next and previous object. If objects is not free
	// then these offsets are negative, otherwise positive.
	int backward;
	int forward;
    };
    struct free_block : public allocation_block { 
	REF(free_block) next;
	REF(free_block) prev;
	void link_after(free_block* after) { 
	    (next = after->next)->prev = (REF(free_block))this;
	    (prev = (REF(free_block))after)->next = (REF(free_block))this;
	}
	void unlink() {
	    prev->next = next;
	    next->prev = prev;
	}
    };
    struct monitor { 
	// Fields used for syncronization of access to shared memory
	long   n_readers;
	long   n_writers;
	long   n_waiters;
#ifdef _WIN32
	long   cs;
#else
	shmsem_data cs;
#endif
	long   n_sessions;
	size_t max_file_size;
	void*  base_address;
    };
    struct header { 
	void*       base_address;
	REF(void)       root_object;
	REF(free_block) free_list_pos;
	free_block  free_list;
	size_t      size;
	int         barrier;
    };
    
    header*  pHdr;
    monitor* pMonitor;

#ifdef _WIN32
    HANDLE hFile;
    HANDLE hCS;
    HANDLE hEvent;
    HANDLE hSemaphore;
    HANDLE hMutex;
    HANDLE hMap;
    HANDLE hMonitor;
#else
    int      fd;
    int      sem;
    int      shmid;
    int      monid;
    semp_t   mutex;
    shmsem_t cs;

    bool wait_resource(status&, lck_t, unsigned self, unsigned msec);
#endif

    open_mode mode;

    lock_descriptor* shared_lock_chain;
    int n_nested_exclusive_locks;
    unsigned owner; // thread owner of exclusive lock

    bool enter_critical_section(status&);
    bool leave_critical_section(status&);
};


//
// Convinient locking mechanism, based on C++ local objects
//
class SHMEM_DLL_ENTRY exclusive_lock : shared_memory::lock_descriptor {
  protected:
    shared_memory& shmem;
  public:
    exclusive_lock(shared_memory& shm) 
    : shared_memory::lock_descriptor(shared_memory::lck_exclusive), 
      shmem(shm)
    {
	shared_memory::status rc = shmem.lock(*this);
	//assert(rc == shared_memory::ok);
	if(!(rc == shared_memory::ok))
		global_func_log(__LINE__,__FILE__, NULL);
    }
    ~exclusive_lock() { 
	shared_memory::status rc = shmem.unlock(*this);
	//assert(rc == shared_memory::ok);
	if(!(rc == shared_memory::ok))
		global_func_log(__LINE__,__FILE__, NULL);
    }
};

class shared_lock : shared_memory::lock_descriptor { 
  protected:
    shared_memory& shmem;
  public:
    shared_lock(shared_memory& shm)
    : shared_memory::lock_descriptor(shared_memory::lck_shared), 
      shmem(shm)
    {
	shared_memory::status rc = shmem.lock(*this);
	//assert(rc == shared_memory::ok);
	if(!(rc == shared_memory::ok))
		global_func_log(__LINE__,__FILE__, NULL);

    }
    ~shared_lock() { 
	shared_memory::status rc = shmem.unlock(*this);
	//assert(rc == shared_memory::ok);
	if(!(rc == shared_memory::ok))
		global_func_log(__LINE__,__FILE__, NULL);

    }
};

//
// System independent indetrface for synchronization primitives
//

class SHMEM_DLL_ENTRY semaphore { // classical sempahore
  public:    
    bool wait(unsigned msec = INFINITE);
    void signal(unsigned inc = 1);
    bool open(char const* name, unsigned init_value = 0);
    void close();
 
  private:
#ifdef _WIN32
    HANDLE s;
#else
    semp_t s;
#endif
};

class SHMEM_DLL_ENTRY event { // event with manual reset
  public:    
    bool wait(unsigned msec = INFINITE);
    void signal();
    void reset();
    
    bool open(char const* name, bool signaled = false);
    void close();
 
  private:
#ifdef _WIN32
    HANDLE e;
#else
    semp_t e;
#endif
};

#endif


