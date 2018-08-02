#ifndef SYSSTATH_
#define SYSSTATH_

#define _need_time_t
#include "sys/stdtypes.h"
#undef _need_time_t

struct stat {
        dev_t           st_dev;
        unsigned short  __pad1;
        ino_t           st_ino;
        mode_t          st_mode;
        nlink_t         st_nlink;
        uid_t           st_uid;
        gid_t           st_gid;
        dev_t           st_rdev;
        unsigned short  __pad2;
        off_t           st_size;
        unsigned long   st_blksize;
        unsigned long   st_blocks;
        time_t          st_atime;
        unsigned long   __unused1;
        time_t          st_mtime;
        unsigned long   __unused2;
        time_t          st_ctime;
        unsigned long   __unused3;
        unsigned long   __unused4;
        unsigned long   __unused5;
};

int chmod(const char * path, mode_t mode);   
int fstat(int filedes, struct stat *buf);
int mkdir(const char *path, mode_t mode);
int mkfifo(const char * path, mode_t mode);
int stat(const char *file_name, struct stat *buf);
mode_t umask(mode_t cmask);


#define S_ISBLK(mode)   ((mode&0xF000) == 0x6000)
#define S_ISCHR(mode)   ((mode&0xF000) == 0x2000)
#define S_ISDIR(mode)   ((mode&0xF000) == 0x4000)
#define S_ISFIFO(mode)  ((mode&0xF000) == 0x1000)
#define S_ISREG(mode)   ((mode&0xF000) == 0x8000)


#define S_IREAD         S_IRUSR /* read permission, owner */
#define S_IWRITE        S_IWUSR /* write permission, owner */
#define S_IEXEC         S_IXUSR /* execute/search permission, owner */

#define S_IRGRP 00040           /* read permission: group */
#define S_IROTH 00004           /* read permission: other */
#define S_IRUSR 00400           /* read permission: owner */
#define S_IRWXG 00070           /* read, write, execute: group */
#define S_IRWXO 00007           /* read, write, execute: other */
#define S_IRWXU 00700           /* read, write, execute: owner */
#define S_ISGID         0x400   /* set group id on execution */
#define S_ISUID         0x800   /* set user id on execution */
#define S_IWGRP 00020           /* write permission: group */
#define S_IWOTH 00002           /* write permission: other */
#define S_IWUSR 00200           /* write permission: owner */
#define S_IXGRP 00010           /* execute permission: group */
#define S_IXOTH 00001           /* execute permission: other */
#define S_IXUSR 00100           /* execute permission: owner */

/********

#define S_IFBLK  0060000
#define S_IFCHR  0020000
#define S_IFDIR  0040000
#define S_IFIFO  0010000
#define S_IFLNK  0120000
#define S_IFMT   0xf000
#define S_IFREG  0100000
#define S_IFSOCK 0140000
#define S_ISLNK(m)      (((m) & S_IFMT) == S_IFLNK)
#define S_ISVTX  0001000
#define S_ISSOCK(m)     (((m) & S_IFMT) == S_IFSOCK)

**********/



#endif







