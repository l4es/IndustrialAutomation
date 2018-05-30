#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

/****************************************************************************/
#include "ecrt.h"
#include "slaves.h"
/****************************************************************************/
#if __BYTE_ORDER == __LITTLE_ENDIAN
#pragma "using little endian"
#endif

// Application parameters
#define FREQUENCY 100
// Optional features

#define TIMESPEC2NS(T) ((uint64_t) (T).tv_sec * NSEC_PER_SEC + (T).tv_nsec)
#define CLOCK_TO_USE CLOCK_REALTIME
#define NSEC_PER_SEC (1000000000L)
#define PERIOD_NS (NSEC_PER_SEC / FREQUENCY)
#define TWO_SLAVES
/****************************************************************************/

// EtherCAT
static ec_master_t *master = NULL;
static ec_master_state_t master_state = {};

static ec_domain_t *domain1 = NULL;
static ec_domain_state_t domain1_state = {};

const struct timespec cycletime = {0, PERIOD_NS};
/****************************************************************************/

// process data
static uint8_t *domain1_pd = NULL;

#define AnaInSlavePos1 0, 0
#define AnaInSlavePos2 0, 1

#define LIBIX_VP 0x000001ee, 0x0000000e /* LIBIX_VP = VENDOR PRODUCT */

// offsets for PDO entries

static unsigned int off_ana_in[2]={-1};
static unsigned int off_ana_out[2]={-1};

const static ec_pdo_entry_reg_t domain1_regs[] = {
    {AnaInSlavePos1,  LIBIX_VP, 0x1a00, 0x02, &off_ana_out[0]},
    {AnaInSlavePos1,  LIBIX_VP, 0x1600, 0x02, &off_ana_in[0]},
#ifdef TWO_SLAVES
    {AnaInSlavePos2,  LIBIX_VP, 0x1a00, 0x02, &off_ana_out[1]},
    {AnaInSlavePos2,  LIBIX_VP, 0x1600, 0x02, &off_ana_in[1]},
#endif
    {}
};

static unsigned int counter = 0;
static unsigned int blink = 0;

struct timespec timespec_add(struct timespec time1, struct timespec time2)
{
	struct timespec result;

	if ((time1.tv_nsec + time2.tv_nsec) >= NSEC_PER_SEC) {
		result.tv_sec = time1.tv_sec + time2.tv_sec + 1;
		result.tv_nsec = time1.tv_nsec + time2.tv_nsec - NSEC_PER_SEC;
	} else {
		result.tv_sec = time1.tv_sec + time2.tv_sec;
		result.tv_nsec = time1.tv_nsec + time2.tv_nsec;
	}

	return result;
}
/*****************************************************************************/

int config_slaves(void)
{
    ec_slave_config_t *sc1;
    ec_slave_config_t *sc2;
    
    master = ecrt_request_master(0);
    if (!master)
        return -1;

    domain1 = ecrt_master_create_domain(master);
    if (!domain1)
        return -1;

    if (!(sc1 = ecrt_master_slave_config(
                    master, AnaInSlavePos1, LIBIX_VP))) {
        fprintf(stderr, "Failed to get slave configuration.\n");
        return -1;
    }
#ifdef TWO_SLAVES
    if (!(sc2 = ecrt_master_slave_config(
                    master, AnaInSlavePos2, LIBIX_VP))) {
        fprintf(stderr, "Failed to get slave configuration.\n");
        return -1;
    }
#endif
    printf("Configuring PDOs...\n");
    if (ecrt_slave_config_pdos(sc1, EC_END, slave_0_syncs)) {
        fprintf(stderr, "Failed to configure PDOs.\n");
        return -1;
    }

#ifdef TWO_SLAVES
    if (ecrt_slave_config_pdos(sc2, EC_END, slave_1_syncs)) {
        fprintf(stderr, "Failed to configure PDOs.\n");
        return -1;
    }
#endif
    if (ecrt_domain_reg_pdo_entry_list(domain1, domain1_regs)) {
        fprintf(stderr, "PDO entry registration failed!\n");
        return -1;
    }

    ecrt_slave_config_dc(sc1, 0x0700, PERIOD_NS, 4400000, 0, 0);
#ifdef TWO_SLAVES
    ecrt_slave_config_dc(sc2, 0x0700, PERIOD_NS, 4400000, 0, 0);
#endif
    printf("Activating master...\n");
    if (ecrt_master_activate(master))
        return -1;

    if (!(domain1_pd = ecrt_domain_data(domain1))) {
        return -1;
    }

    printf("Offsets in=%d,%d out=%d,%d\n",
	off_ana_in[0], off_ana_in[1],
	off_ana_out[0], off_ana_out[1]);
    return 0;
}

void check_domain1_state(void)
{
    ec_domain_state_t ds;

    ecrt_domain_state(domain1, &ds);

    if (ds.working_counter != domain1_state.working_counter)
        printf("Domain1: WC %u.\n", ds.working_counter);
    if (ds.wc_state != domain1_state.wc_state)
        printf("Domain1: State %u.\n", ds.wc_state);

    domain1_state = ds;
}

/*****************************************************************************/

void check_master_state(void)
{
    ec_master_state_t ms;

    ecrt_master_state(master, &ms);

	if (ms.slaves_responding != master_state.slaves_responding)
        printf("%u slave(s).\n", ms.slaves_responding);
    if (ms.al_states != master_state.al_states)
        printf("AL states: 0x%02X.\n", ms.al_states);
    if (ms.link_up != master_state.link_up)
        printf("Link is %s.\n", ms.link_up ? "up" : "down");

    master_state = ms;
}

int debug = 0;

void cyclic_task(void)
{
    struct timespec wakeupTime, time;

    // get current time
    clock_gettime(CLOCK_TO_USE, &wakeupTime);

    while(1) {
	wakeupTime = timespec_add(wakeupTime, cycletime);
       	clock_nanosleep(CLOCK_TO_USE, TIMER_ABSTIME, &wakeupTime, NULL);
	// receive process data
	ecrt_master_receive(master);
	ecrt_domain_process(domain1);

	// check process data state (optional)
	check_domain1_state();

	// write application time to master
	clock_gettime(CLOCK_TO_USE, &time);
	if (counter) {
		counter--;
	} else { // do this at 1 Hz
		counter = FREQUENCY;
		// check for master state (optional)
		check_master_state();
		printf("time=%u\n",
			(uint32_t)TIMESPEC2NS(time));
		if (debug++ ==  15)
			exit(0);
	}
	ecrt_master_application_time(master, TIMESPEC2NS(time));
	/* master writes to 910  */
	ecrt_master_sync_reference_clock(master);
	/* 
	 *	the reference slave propagarte its time to all other slaves. 
	 *	note that this is actually the master time.
	*/
	ecrt_master_sync_slave_clocks(master);
	// send process data
	ecrt_domain_queue(domain1);
	ecrt_master_send(master);
	}
}

/****************************************************************************/

int main(int argc, char **argv)
{
    if (config_slaves())
	return -1;
    cyclic_task();
    return 0;
}

