
// embedded filesystem used to store project, files datas (events, ...), and to get statistics on it
//#define EMBEDDED_DISK_DEVICE "/"
#define EMBEDDED_DISK_DEVICE "/dev/sda2"

//security in case of bad mix defines...
#ifndef GTK_INTERFACE

// for hardware particularities... commented if not used.
#define NBR_HARD_LIFE_USERS_LEDS (1+NBR_USERS_LEDS)
// life led + users leds, can use a same inputs/outputs x86 register port (of course not same bit...)
// Here ClassicLadderPLC x86 i/o ports values:
#define HARD_LIFE_USERS_LEDS_PORTS {0x7B,0x7B}
#define HARD_LIFE_USERS_LEDS_BITS {(1<<6),(1<<5)}
#define HARD_LIFE_USERS_LEDS_DIR_PORTS {0x9B,0x9B}
#define HARD_LIFE_USERS_LEDS_CMD_INVERTED

//for a 'ro' embedded filesystem, requiring 'rw' remount before files write on it...
//("/tmp" directory independently should be always writable)
//comment if not usefull on your embedded target..., or adjust parameters!
#define EMBEDDED_MOUNT_START_PARAMS_CMD EMBEDDED_DISK_DEVICE,"/","",MS_REMOUNT,""
#define EMBEDDED_MOUNT_END_PARAMS_CMD EMBEDDED_DISK_DEVICE,"/","",MS_REMOUNT|MS_RDONLY,""

// script to launch after update soft received on target... comment if not used
#define SCRIPT_TO_RUN_AFTER_UPDATE_RECEIVED "/usr/local/classicladder/classicladder_update.script"

#endif
