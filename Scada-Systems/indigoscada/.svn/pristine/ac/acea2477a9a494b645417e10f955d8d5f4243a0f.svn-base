#ifndef _SYS_STATH
#define _SYS_STATH

#define uint_t unsigned int

#define _need_time_t
#include "sys/stdtypes.h"
#undef _need_time_t


struct  stat
{
        dev_t   st_dev;                 /* ID of device containing a directory*/
                                        /*   entry for this file.  File serial*/
                                        /*   no + device ID uniquely identify */
                                        /*   the file within the system */
        ino_t   st_ino;                 /* File serial number */
        mode_t  st_mode;                /* File mode; see #define's below */
        nlink_t st_nlink;               /* Number of links */
        uid_t   st_uid;                 /* User ID of the file's owner */
        gid_t   st_gid;                 /* Group ID of the file's group */
        dev_t   st_rdev;                /* ID of device */
                                        /*   This entry is defined only for */
                                        /*   character or block special files */
        off_t   st_size;                /* File size in bytes */
        time_t  st_atime;               /* Time of last access */
        int     st_spare1;
        time_t  st_mtime;               /* Time of last data modification */
        int     st_spare2;
        time_t  st_ctime;               /* Time of last file status change */
        int     st_spare3;
                                        /* Time measured in seconds since */
                                        /*   00:00:00 GMT, Jan. 1, 1970 */
        uint_t  st_blksize;             /* Size of block in file */
        int    st_blocks;              /* blocks allocated for file */
        uint_t  st_flags;               /* user defined flags for file */
        uint_t  st_gen;                 /* file generation number */

};


#define S_IREAD         S_IRUSR /* read permission, owner */
#define S_IWRITE        S_IWUSR /* write permission, owner */
#define S_IEXEC         S_IXUSR /* execute/search permission, owner */


/*
#define S_IFBLK  0060000
#define S_IFCHR  0020000
#define S_IFDIR  0040000
#define S_IFIFO  0010000
#define S_IFLNK  0120000
#define S_IFMT  00170000
#define S_IFREG  0100000
#define S_IFSOCK 0140000
#define S_ISLNK(m)      (((m) & S_IFMT) == S_IFLNK)
#define S_ISSOCK(m)     (((m) & S_IFMT) == S_IFSOCK)
#define S_ISVTX  0001000
*/

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






