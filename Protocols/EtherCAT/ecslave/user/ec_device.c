#include "xgeneral.h"
#include "ec_device.h"

void ec_init_device(struct ec_device *ec)
{
	pthread_mutexattr_t mta;

	pthread_mutex_init(&ec->events_sync, &mta);
	LIST_INIT(&ec->events);
	memset(&ec->rx_time, 0x00, sizeof(ec->rx_time) );
}

void ecat_add_event_to_device(struct ec_device *ec, 
		struct ecat_event* ev,
		void (*action)(void *), 
		void *private)
{
	if (ev->action) {
		printf("%s double assignment\n",__func__);
		return;
	}
	ev->action = action;
	ev->__private = private;
	pthread_mutex_lock(&ec->events_sync);
        LIST_INSERT_HEAD(&ec->events, ev, list);
	pthread_mutex_unlock(&ec->events_sync);
}
