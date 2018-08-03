
// embedded filesystem used to store project, files datas (events, ...), and to get statistics on it
#define EMBEDDED_DISK_DEVICE "/dev/mmcblk0p2"

// for hardware particularities... commented if not used.
#define NBR_HARD_LIFE_USERS_LEDS (1+NBR_USERS_LEDS)

// for Arietta (Atmel AT91Sam9), GPIO leds (life & users) list, ports and pins
#include "../wiringSam/wiringSam.h"
#define HARD_LIFE_USERS_LEDS_PORTS { WSAM_PIO_A ,WSAM_PIO_A };
#define HARD_LIFE_USERS_LEDS_GPIO { 22, 21 };

//for a 'ro' embedded filesystem, requiring 'rw' remount before files write on it...
//("/tmp" directory independently should be always writable)
//comment if not usefull on your embedded target..., or adjust parameters!
#define EMBEDDED_MOUNT_START_PARAMS_CMD EMBEDDED_DISK_DEVICE,"/","",MS_REMOUNT,""
#define EMBEDDED_MOUNT_END_PARAMS_CMD EMBEDDED_DISK_DEVICE,"/","",MS_REMOUNT|MS_RDONLY,""

// script to launch after update soft received on target... comment if not used
#define SCRIPT_TO_RUN_AFTER_UPDATE_RECEIVED "/usr/local/classicladder/classicladder_update.script"

