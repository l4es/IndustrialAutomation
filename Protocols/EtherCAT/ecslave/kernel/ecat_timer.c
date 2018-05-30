#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/hrtimer.h>
#include <linux/hardirq.h>
#include <linux/ktime.h>

#include "xgeneral.h"
#include "ethercattype.h"
#include "ecs_slave.h"
#include "ec_regs.h"
#include "ecat_timer.h"

DEFINE_SPINLOCK(timer_sync);
static struct list_head ecat_events;
static struct hrtimer ecat_hrtimer;
static int ecat_delay_ns = 0;
static ktime_t ecat_ktime;

void (*stop_timer)(void) = 0;

enum hrtimer_restart ecat_timer(struct hrtimer *timer)
{
	struct ecat_event *ev;
	struct list_head* temp;
	struct list_head* temp2;

	hrtimer_add_expires_ns(&ecat_hrtimer, ecat_delay_ns);
	list_for_each_safe(temp, temp2, &ecat_events){
		ev = list_entry(temp, struct ecat_event, list);
		ev->action(ev->__private);
		list_del_init(&ev->list);
	}
	return HRTIMER_RESTART;
}

void ecat_schedule_timed_event(void *private,struct ecat_event *ev, void (*action)(void *))
{
	ev->action = action;
	ev->__private = private;

	spin_lock_bh(&timer_sync);
        list_add_tail(&ecat_events, &ev->list);
	spin_unlock_bh(&timer_sync);
}

void ecat_hrtimer_exit(void)
{
	int ret;

	ret = hrtimer_cancel(&ecat_hrtimer);
	if (ret) {
		printk("The timer was still in use...\n");
		return;
	}
	printk("HR Timer module uninstalling\n");
}


void ecat_create_timer(void)
{
	
	long nsec, sec;
	
	INIT_LIST_HEAD(&ecat_events);
	sec = ecat_get_dcstart(0);
	nsec = do_div(sec, NSEC_PER_SEC);

	ecat_ktime = ktime_set(sec, nsec);
	hrtimer_init(&ecat_hrtimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	ecat_hrtimer.function = ecat_timer;
	ecat_delay_ns = nsec;
	stop_timer = ecat_hrtimer_exit;
}

void ecat_wake_timer()
{
//	hrtimer_start(&ecat_hrtimer, ecat_ktime, HRTIMER_MODE_REL);
	printk("skipping hrtimer\n");
}

void ecat_timer_exit(void)
{
	if (stop_timer)
		stop_timer();
}
