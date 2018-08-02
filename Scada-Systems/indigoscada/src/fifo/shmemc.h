//-< SHMEMC.H >------------------------------------------------------*--------*
// SHMEM                      Version 1.0        (c) 1998  GARRET    *     ?  *
// (Shared Memory Manager)                                           *   /\|  *
//                                                                   *  /  \  *
//                          Created:     22-Aug-2005  K.A. Knizhnik  * / [] \ *
//                          Last update: 22-Aug-2005  K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Shered Memory C interface
//-------------------------------------------------------------------*--------*

#ifndef __SHMEMC_H__
#define __SHMEMC_H__

#include <stdlib.h>

//#ifndef SHMEM_DLL_ENTRY
//    #if defined(BUILDING_DLL)
//        #define SHMEM_DLL_ENTRY __declspec( dllexport )
//    #elif defined(USING_DLL)
//        #define SHMEM_DLL_ENTRY __declspec( dllimport )
//    #else
        #define SHMEM_DLL_ENTRY
//    #endif
//#endif

#ifdef __cplusplus
extern "C" { 
#endif

enum shmem_lock_t { 
    SHMEM_LOCK_SHARED, 
    SHMEM_LOCK_EXCLUSIVE 
};

enum shmem_open_mode_t { 
    SHMEM_READ_ONLY, 
    SHMEM_READ_WRITE 
};

enum shmem_status_t {
    SHMEM_OK = 0, 
    SHMEM_TIMEOUT_EXPIRED = -1,
};

typedef struct shmem_lock_descriptor { 
    enum shmem_lock_t mode;
    struct shmem_lock_descriptor* next;
    unsigned owner;
} shmem_lock_descriptor;

typedef struct shmem_t* shmem_h;

#define WAIT_INFINITE (~0U)

extern SHMEM_DLL_ENTRY shmem_h shmem_open(char const* file_name, char const* shared_name, 
                                          size_t max_size, shmem_open_mode_t mode, void* desired_address);

extern SHMEM_DLL_ENTRY void* shmem_malloc(shmem_h hnd, size_t size);
extern SHMEM_DLL_ENTRY void* shmem_realloc(shmem_h hnd, void* ptr, size_t size);
extern SHMEM_DLL_ENTRY void shmem_free(shmem_h hnd, void* ptr);

extern SHMEM_DLL_ENTRY shmem_status_t shmem_close(shmem_h hnd);
extern SHMEM_DLL_ENTRY shmem_status_t shmem_flush(shmem_h hnd);

extern SHMEM_DLL_ENTRY shmem_status_t shmem_lock(shmem_h hnd, shmem_lock_descriptor* desc, unsigned msec);
extern SHMEM_DLL_ENTRY shmem_status_t shmem_unlock(shmem_h hnd, shmem_lock_descriptor* desc);

extern SHMEM_DLL_ENTRY void* shmem_get_root(shmem_h hnd);
extern SHMEM_DLL_ENTRY void shmem_set_root(shmem_h hnd, void* root);

#ifdef __cplusplus
}
#endif

#endif

