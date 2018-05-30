#ifndef __EC_DEVICE_H__
#define __EC_DEVICE_H__

struct ec_device {
        int index;
        struct ecat_event rx_time;
};


#endif
