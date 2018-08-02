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

#include "shmem.h"
#include "shmemc.h"

SHMEM_DLL_ENTRY shmem_h shmem_open(char const* file_name, char const* shared_name, 
                                   size_t max_size, shmem_open_mode_t mode, void* desired_address)
{
    shared_memory* shmem = new shared_memory();
    if (shmem->open(file_name, shared_name, max_size, (shared_memory::open_mode)mode, desired_address) == shared_memory::ok) {
        return (shmem_h)shmem;
    }
    delete shmem;
    return NULL;
}
        
SHMEM_DLL_ENTRY void* shmem_malloc(shmem_h hnd, size_t size)
{
    return((shared_memory*)hnd)->allocate(size);
}

SHMEM_DLL_ENTRY void* shmem_realloc(shmem_h hnd, void* ptr, size_t size)
{
    return ((shared_memory*)hnd)->reallocate(ptr, size);
}

SHMEM_DLL_ENTRY void shmem_free(shmem_h hnd, void* ptr)
{
	((shared_memory*)hnd)->deallocate(ptr);
}

SHMEM_DLL_ENTRY shmem_status_t shmem_close(shmem_h hnd)
{
    ((shared_memory*)hnd)->close();
    delete (shared_memory*)hnd;
    return SHMEM_OK;
}

SHMEM_DLL_ENTRY shmem_status_t shmem_flush(shmem_h hnd)
{
    return (shmem_status_t)((shared_memory*)hnd)->flush();
}

SHMEM_DLL_ENTRY shmem_status_t shmem_lock(shmem_h hnd, shmem_lock_descriptor* desc, unsigned msec)
{
    return (shmem_status_t)((shared_memory*)hnd)->lock(*(shared_memory::lock_descriptor*)desc, msec);
}

SHMEM_DLL_ENTRY shmem_status_t shmem_unlock(shmem_h hnd, shmem_lock_descriptor* desc)
{
    return (shmem_status_t)((shared_memory*)hnd)->unlock(*(shared_memory::lock_descriptor*)desc);
}

SHMEM_DLL_ENTRY void* shmem_get_root(shmem_h hnd)
{
    return ((shared_memory*)hnd)->get_root_object();
}

SHMEM_DLL_ENTRY void shmem_set_root(shmem_h hnd, void* root)
{
    ((shared_memory*)hnd)->set_root_object(root);
}
