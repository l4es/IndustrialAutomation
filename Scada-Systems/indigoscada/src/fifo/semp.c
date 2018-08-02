/*-< POSIXSEM.C >---------------------------------------------------*--------*/
/* SHMEM                      Version 1.0        (c) 1998  GARRET   *     ?  */
/* (Shared Memory Manager)                                          *   /\|  */
/*                                                                  *  /  \  */
/*                          Created:      2-Apr-98    K.A. Knizhnik * / [] \ */
/*                          Last update: 12-Apr-98    K.A. Knizhnik * GARRET */
/*------------------------------------------------------------------*--------*/
/* Implementation of Posix semaphores                               *        */
/*------------------------------------------------------------------*--------*/

#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <malloc.h>
#include <alloca.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include "semp.h"



int semp_init(semp_t* sem, char* name, unsigned init_value)
{
    key_t key = IPC_PRIVATE;
    int semid;
    struct sembuf sops[3];
    sops[0].sem_num = 1;
    sops[0].sem_op  = 0; /* check if semaphore is already initialized */
    sops[0].sem_flg = IPC_NOWAIT;
    sops[1].sem_num = 1;
    sops[1].sem_op  = 1; /* mark sempahore as initialized */
    sops[1].sem_flg = 0;
    sops[2].sem_num = 0;
    sops[2].sem_op  = init_value;
    sops[2].sem_flg = 0;
    if (name != NULL) { 
	int fd;
	char* path = name;
	if (*name != '/') { 
	    path = (char*)alloca(strlen(name)+6);
	    strcat(strcpy(path, "/tmp/"), name);
	}
	fd = open(path, O_WRONLY|O_CREAT, 0777);
	if (fd < 0) {
	    return -1;
	}
	close(fd);
	key = ftok(path, '0');
	if (key < 0) {
	    return -1;
	}
    }
    semid = semget(key, 2, IPC_CREAT|0777);
    if (semid < 0) { 
	return -1;
    }
    if (semop(semid, sops, items(sops)) && errno != EAGAIN) { 
	return -1;
    }
    *sem = semid;
    return 0;
}

int semp_open(semp_t* sem, char* name)
{
    key_t key;
    char* path = name;
    if (*name != '/') { 
	path = (char*)alloca(strlen(name)+6);
	strcat(strcpy(path, "/tmp/"), name);
    }
    key = ftok(path, '0');
    if (key < 0) {
	return -1;
    }
    *sem = semget(key, 1, 0);
    return (*sem < 0) ? -1 : 0;
}

int semp_wait(semp_t* sem)
{
    static struct sembuf sops[] = {{0, -1, SEM_UNDO}};
    while (semop(*sem, sops, items(sops)) < 0) { 
	if (errno != EINTR) { 
	    return -1;
	}
    }
    return 0;
}

int semp_trywait(semp_t* sem)
{
    static struct sembuf sops[] = {{0, -1, SEM_UNDO|IPC_NOWAIT}};
    if (semop(*sem, sops, items(sops)) < 0) { 
	return -1;
    }
    return 0;
}

int semp_post(semp_t* sem)
{
    static struct sembuf sops[] = {{0, 1, SEM_UNDO}};
    return semop(*sem, sops, items(sops));
}

int semp_destroy(semp_t* sem) 
{
    return semctl(*sem, 0, IPC_RMID, 0);
}

int shmsem_init(shmsem_t* sem, shmsem_data* data)
{
    key_t key = (key_t)sem; /* use virtual address as a key */
    int semid;
    while ((semid = semget(key, 1, IPC_CREAT|IPC_EXCL|0777)) < 0) { 
	if (errno != EEXIST) { 
	    return -1;
	}
	key += 1;
    }
    sem->id = semid;
    sem->data = data;
    data->count = 0;
    data->key = key;
#if defined(__GNUC__) && defined(__i386__)
    return 0;
#else
    { 
	static struct sembuf sops[] = {{0, 1, 0}};
	return semop(semid, sops, items(sops));
    }
#endif
}

int shmsem_open(shmsem_t* sem, shmsem_data* data)
{
    int semid = semget(data->key, 1, 0);
    if (semid < 0) { 
	return -1;
    }
    sem->id = semid;
    sem->data = data;
    return 0;
}

int shmsem_destroy(shmsem_t* sem)
{
    return semp_destroy(&sem->id);
}
