#include "xgeneral.h"
#include "ethercattype.h"
#include "ecs_slave.h"
#include "ec_mbox.h"
#include "ec_net.h"
#include "ec_sii.h"
#include "ecat_timer.h"
#include "ec_regs.h"
#include "ec_device.h"
#include <semaphore.h>
#include <pthread.h>

static sem_t timersem;
static int ecat_timer_run  = 0;
static pthread_mutex_t timer_sync;

LIST_HEAD(events_head, ecat_event) ecat_events;

extern ecat_slave slaves[];

void *ecat_timer(void *dummy __attribute__ ((unused)) )
{
	uint32_t t;
	struct timespec tm ={0};
	struct timespec rem = {0};
	struct ecat_event *ev;
	uint32_t local_starttime;
	ecat_slave *ecs = &slaves[0];

	ecat_timer_run = 1;
	sem_wait(&timersem);
	/* 
	 * get starting time delay 
	*/
	local_starttime = ecat_get_dcstart(0,ecs) + ecat_systime_offset();
	tm.tv_nsec = local_starttime % NSEC_PER_SEC;
	tm.tv_sec = local_starttime/NSEC_PER_SEC;

	nanosleep(&tm, &rem);
	t = ecat_local_time();
	ecat_set_dcstart(0,(uint8_t *)&t, sizeof(t));

	while(1) {
		pthread_mutex_lock(&timer_sync);
		while (LIST_FIRST(&ecat_events) != NULL) {
		      	ev = LIST_FIRST(&ecat_events);
			ev->action(ev->__private);
			LIST_REMOVE(ev, list);
			ev->action = 0; /* for debug purpose*/
		}
		pthread_mutex_unlock(&timer_sync);
		/* calculate next interval */
		tm.tv_sec  = 0;
		tm.tv_nsec = ecat_cyclic_interval_ns();
		nanosleep(&tm, &rem);
		t = ecat_local_time();
		ecat_set_dcstart(0, (uint8_t *)&t,sizeof(t));
	}
}

void  ecat_schedule_timed_event(void *private,struct ecat_event *event, void (*action)(void *))
{
	event->action = action;
	event->__private = private;
	pthread_mutex_lock(&timer_sync);
        LIST_INSERT_HEAD(&ecat_events, event, list);
	pthread_mutex_unlock(&timer_sync);
}

void ecat_create_timer(void)
{
	int ret;
	pthread_t thread;
	pthread_attr_t attr;
	pthread_mutexattr_t mta;

	if (ecat_timer_run){
		return;
	}
	sem_init(&timersem, 0, 0);
	pthread_mutexattr_init(&mta);
	pthread_mutex_init(&timer_sync, &mta);


	pthread_attr_init(&attr);
	LIST_INIT(&ecat_events);
	ret = pthread_create(&thread , &attr, &ecat_timer, 0);
	if (ret) {
		printf("failed to create"
		" timer thread %s\n",strerror(ret));
		sleep(1);
		exit(0);
	}
}

void ecat_wake_timer(void)
{
	uint32_t interval_ns;

	if (ecat_timer_run == 0){
		printf("Insane 980 before 9a0\n");
		return;
	}
	interval_ns = ecat_cyclic_interval_ns();
	printf("interval %dns\n", interval_ns);
	if (interval_ns < 1000000){
		printf("WARNING: user space ecat cannot "
			"handle less than 1ms intervals");
	}
	sem_post(&timersem);
}


