/*-< POSIXSEM.H >---------------------------------------------------*--------*/
/* SHMEM                      Version 1.0        (c) 1998  GARRET   *     ?  */
/* (Shared Memory Manager)                                          *   /\|  */
/*                                                                  *  /  \  */
/*                          Created:      2-Apr-98    K.A. Knizhnik * / [] \ */
/*                          Last update: 12-Apr-98    K.A. Knizhnik * GARRET */
/*------------------------------------------------------------------*--------*/
/* Posix semaphores interface                                       *        */
/*------------------------------------------------------------------*--------*/

#ifndef __POSIXSEM_H__
#define __POSIXSEM_H__

#include <errno.h>

#ifdef __cplusplus
extern "C" { 
#endif

#define items(x) (sizeof(x)/sizeof(*x))

typedef int semp_t;

extern int semp_init(semp_t* sem, char* name, unsigned init_value);
extern int semp_open(semp_t* sem, char* name);
extern int semp_wait(semp_t* sem);
extern int semp_trywait(semp_t* sem);
extern int semp_post(semp_t* sem);
extern int semp_destroy(semp_t* sem);

typedef struct { 
    int count;
    int key;
} shmsem_data;

typedef struct { 
    shmsem_data* data;
    semp_t       id;
} shmsem_t;

extern int shmsem_init(shmsem_t* sem, shmsem_data* data);
extern int shmsem_open(shmsem_t* sem, shmsem_data* data);
extern int shmsem_destroy(shmsem_t* sem);

#if defined(__GNUC__) && defined(__i386__)

/*
 * Make sure gcc doesn't try to be clever and move things around
 * on us. We need to use _exactly_ the address the user gave us,
 * not some alias that contains the same information.
 */
typedef struct { int a[100]; } __atomic_fool_gcc_dummy_type;
#define __atomic_fool_gcc(x) (*(__atomic_fool_gcc_dummy_type*)x)

static __inline__ int shmsem_lock(shmsem_t* sem)
{
    int inc = 1;
    int* p = &sem->data->count;
    __asm__ __volatile__(
			"lock; xadd %0,%1"
			:"=r" (inc), "=m" (__atomic_fool_gcc(p))
			:"r" (inc), "m" (__atomic_fool_gcc(p)));
    if (inc != 0) { 
	return semp_wait(&sem->id);
    }
    return 0;
}

static __inline__ int shmsem_trylock(shmsem_t* sem)
{
    int lock = 1;
    int res = 0;
    int* p = &sem->data->count;
    __asm__ __volatile__(
			"lock; cmpxchg %3,%1"
			: "=a" (res), "=m" (__atomic_fool_gcc(p))
			:"a" (res), "d" (lock), "m" (__atomic_fool_gcc(p))
			:"ax");
    if (res != 0) { 
	errno = EAGAIN;
	return -1;
    }
    return 0;
}

static __inline__ int shmsem_unlock(shmsem_t* sem)
{
    int inc = -1;
    int* p = &sem->data->count;
    __asm__ __volatile__(
			"lock; xadd %0,%1"
			:"=d" (inc), "=m" (__atomic_fool_gcc(p))
			:"d" (inc), "m" (__atomic_fool_gcc(p)));
    if (inc != 1) { 
	/* some other processes waiting to enter critical section */
	return semp_post(&sem->id);
    }
    return 0;
}

#else

#define shmsem_lock(shmsem)    semp_wait(&(shmsem)->id)
#define shmsem_trylock(shmsem) semp_trywait(&(shmsem)->id)
#define shmsem_unlock(shmsem)  semp_post(&(shmsem)->id)

#endif

#ifdef __cplusplus
} 
#endif

#endif



