
// embedded filesystem used to store project, files datas (events, ...), and to get statistics on it
#define EMBEDDED_DISK_DEVICE "/"

// for hardware particularities... commented if not used.
#define NBR_HARD_LIFE_USERS_LEDS (1+NBR_USERS_LEDS)

// for RapsberryPI, GPIO leds (life & users) list, instead of previous ports/bits/dirports 3 lines
#define HARD_LIFE_USERS_LEDS_GPIO { 27, 17 };

//for a 'ro' embedded filesystem, requiring 'rw' remount before files write on it...
//("/tmp" directory independently should be always writable)
//comment if not usefull on your embedded target..., or adjust parameters!
//#define EMBEDDED_MOUNT_START_PARAMS_CMD EMBEDDED_DISK_DEVICE,"/","",MS_REMOUNT,""
//#define EMBEDDED_MOUNT_END_PARAMS_CMD EMBEDDED_DISK_DEVICE,"/","",MS_REMOUNT|MS_RDONLY,""

// script to launch after update soft received on target... comment if not used
#define SCRIPT_TO_RUN_AFTER_UPDATE_RECEIVED "/usr/local/classicladder/classicladder_update.script"

