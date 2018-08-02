/*-< SEMSTAT.C >----------------------------------------------------*--------*/
/* SHMEM                      Version 1.0        (c) 1998  GARRET   *     ?  */
/* (Shared Memory Manager)                                          *   /\|  */
/*                                                                  *  /  \  */
/*                          Created:     12-Apr-98    K.A. Knizhnik * / [] \ */
/*                          Last update: 12-Apr-98    K.A. Knizhnik * GARRET */
/*------------------------------------------------------------------*--------*/
/* Utility for dumping semaphore values                             *        */
/*------------------------------------------------------------------*--------*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>

union semun {
    int val;                    /* value for SETVAL */
    struct semid_ds *buf;       /* buffer for IPC_STAT, IPC_SET */
    unsigned short int *array;  /* array for GETALL, SETALL */
    struct seminfo *__buf;      /* buffer for IPC_INFO */
};

int main(int argc, char* argv[]) 
{
    int i, j, n;
    if (argc < 2) { 
	fprintf(stderr, "Usage: semstat semid {sem-id ...}\n"); 
	return 1;
    } 
    for (i = 1; i < argc; i++) { 
	int semid = atoi(argv[i]);
	union semun u;
	struct semid_ds sem_desc; 
	u.buf = &sem_desc;

	if (semctl(semid, 0, IPC_STAT, u) < 0) { 
	    fprintf(stderr, "Failed to get information about semaphore %d\n", 
		    semid);
	} else { 
	    n = sem_desc.sem_nsems;
#ifdef __linux__
	    printf("Semaphore key=0x%x, id=%d\n", 
		   sem_desc.sem_perm.__key, semid);
#else
	    printf("Semaphore key=0x%x, id=%d\n", 
		   sem_desc.sem_perm.key, semid);
#endif
	    printf("number  ");
	    for (j = 0; j < n; j++) { 
		printf("%8d", j);
	    }
	    printf("\nvalue   "); 
	    for (j = 0; j < n; j++) { 
		printf("%8d", semctl(semid, j, GETVAL, u));
	    }
	    printf("\nprocess "); 
	    for (j = 0; j < n; j++) { 
		printf("%8d", semctl(semid, j, GETPID, u));
	    }
	    printf("\n\n");
	}
    }
    return 0;
}




