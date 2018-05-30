#ifndef  __ECAT_TIMER_H__
#define  __ECAT_TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

struct ecat_event {
        long* __private;
	void (*action)(void *);
        LIST_ENTRY(ecat_event) list;
};

struct __ecat_slave__;
struct ec_device;

void ecat_add_event_to_device(struct ec_device *, 
		struct ecat_event *, 
		void (*action)(void *), 
		void *__private);
void ecat_schedule_event(void *__private,struct ecat_event *, void (*action)(void *));
void ecat_create_timer(void);
void ecat_wake_timer(void);
void ecat_calibrate_localtime(struct __ecat_slave__*,uint32_t *systime32);
void ecat_set_rx_time(void *p);
uint32_t ecat_local_time(void);

#ifdef __cplusplus
}
#endif

#endif
