#ifndef SYSSTATH_
#define SYSSTATH_

#define _need_time_t
#include "sys/stdtypes.h"
#undef _need_time_t


typedef struct timestruc {
        time_t  tv_sec;         /* seconds */
        long    tv_nsec;        /* and nanoseconds */
} timestruc_t;

#define st_atime        st_atim.tv_sec
#define st_mtime        st_mtim.tv_sec
#define st_ctime        st_ctim.tv_sec

#define  _ST_FSTYPSZ 16          /* array size for file system type name */

struct  stat {
        dev_t   st_dev;
        long    st_pad1[3];     /* reserved for network id */
        ino_t   st_ino;
        mode_t  st_mode;
        nlink_t st_nlink;
        uid_t   st_uid;
        gid_t   st_gid;
        dev_t   st_rdev;
        long    st_pad2[2];     /* dev and off_t expansion */
        off_t   st_size;
        long    st_pad3;        /* future off_t expansion */
        timestruc_t st_atim;    
        timestruc_t st_mtim;    
        timestruc_t st_ctim;    
        long    st_blksize;
        long    st_blocks;
        char    st_fstype[_ST_FSTYPSZ];
        long    st_pad4[8];     /* expansion area */
};


#define S_IREAD         S_IRUSR /* read permission, owner */
#define S_IWRITE        S_IWUSR /* write permission, owner */
#define S_IEXEC         S_IXUSR /* execute/search permission, owner */

#define S_IRGRP 00040           /* read permission: group */
#define S_IROTH 00004           /* read permission: other */
#define S_IRUSR 00400           /* read permission: owner */
#define S_IRWXG 00070           /* read, write, execute: group */
#define S_IRWXO 00007           /* read, write, execute: other */
#define S_IRWXU 00700           /* read, write, execute: owner */
#define S_ISBLK(mode)   ((mode&0xF000) == 0x6000)
#define S_ISCHR(mode)   ((mode&0xF000) == 0x2000)
#define S_ISDIR(mode)   ((mode&0xF000) == 0x4000)
#define S_ISFIFO(mode)  ((mode&0xF000) == 0x1000)
#define S_ISGID         0x400   /* set group id on execution */
#define S_ISREG(mode)   ((mode&0xF000) == 0x8000)
#define S_ISUID         0x800   /* set user id on execution */
#define S_IWGRP 00020           /* write permission: group */
#define S_IWOTH 00002           /* write permission: other */
#define S_IWUSR 00200           /* write permission: owner */
#define S_IXGRP 00010           /* execute permission: group */
#define S_IXOTH 00001           /* execute permission: other */
#define S_IXUSR 00100           /* execute permission: owner */

int chmod(const char * path, mode_t mode);   
int fstat(int filedes, struct stat *buf);
int mkdir(const char *path, mode_t mode);
int mkfifo(const char * path, mode_t mode);
int stat(const char *file_name, struct stat *buf);
mode_t umask(mode_t cmask);


#endif







