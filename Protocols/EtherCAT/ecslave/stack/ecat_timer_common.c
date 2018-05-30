#include "xgeneral.h"
#include "ethercattype.h"
#include "ecat_timer.h"
#include "ecs_slave.h"
#include "ec_regs.h"

void ecat_calibrate_localtime(ecat_slave *esv, uint32_t *systime32)
{
	uint32_t dt;
	uint32_t t;
	uint32_t sec;
	uint32_t nsec;
	uint32_t localtime;
	struct timespec tm;

	clock_gettime(CLOCK_REALTIME, &tm);
	t = TIMESPEC2NS(tm); 
	localtime =  (t & 0xFFFFFFFF);

	dt = (localtime + ecat_systime_offset() - ecat_propagation_delay(esv)) 
			- *systime32;
	sec  = dt/NSEC_PER_SEC;
	nsec = dt % NSEC_PER_SEC;	
	
	if (dt > 0) { 
		/* slow down , local time is faster than system time */
		tm.tv_sec -= sec;
		tm.tv_nsec -= nsec;
	}else { 		
		/* speed up, local time is slower than system time */
		tm.tv_sec += sec;
		tm.tv_nsec += nsec;
	}
	clock_settime(CLOCK_REALTIME, &tm);
	ec_printf("%s %d\n",__func__, dt);
}


uint32_t ecat_local_time(void)
{
	uint32_t t;
	struct timespec tm;

	clock_gettime(CLOCK_REALTIME, &tm);
	t = TIMESPEC2NS(tm); 
	return (t & 0xFFFFFFFF);
}

void ecat_set_rx_time(void *p)
{
	int32_t *reg = p;
	*reg  = ecat_local_time();
}
