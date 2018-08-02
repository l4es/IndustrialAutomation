#ifndef SYSSTATH_
#define SYSSTATH_

#define _need_time_t
#include "sys/stdtypes.h"
#undef _need_time_t

typedef unsigned short __site_t;
typedef unsigned short __cnode_t;
typedef long key_t;

struct stat
   {
        dev_t   st_dev;
        ino_t   st_ino;
        mode_t  st_mode;
        nlink_t st_nlink;
        unsigned short st_reserved1;
        unsigned short st_reserved2;
        dev_t   st_rdev;
        off_t   st_size;
        time_t  st_atime;
        int     st_spare1;
        time_t  st_mtime;
        int     st_spare2;
        time_t  st_ctime;
        int     st_spare3;
        long    st_blksize;
        long    st_blocks;
        /* unsigned int    st_pad:30;
           unsigned int    st_acl:1;
           unsigned int    st_remote:1; 
        */
        unsigned int    st_pad_acl_remote; 
        dev_t   st_netdev;

        ino_t   st_netino;
        __cnode_t       st_cnode;
        __cnode_t       st_rcnode;

        __site_t        st_netsite;
        short   st_fstype;

        dev_t   st_realdev;

        unsigned short  st_basemode;
        unsigned short  st_spareshort;

        uid_t   st_uid;
        gid_t   st_gid;


        long    st_spare4[3 ];
   };


#ifdef NEVER
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
#endif 

#    define S_ISUID 0004000     /* set user ID on execution */
#    define S_ISGID 0002000     /* set group ID on execution */

#    define S_IRWXU 0000700     /* read, write, execute permission (owner) */
#    define S_IRUSR 0000400     /* read permission (owner) */
#    define S_IWUSR 0000200     /* write permission (owner) */
#    define S_IXUSR 0000100     /* execute permission (owner) */

#    define S_IRWXG 0000070     /* read, write, execute permission (group) */
#    define S_IRGRP 0000040     /* read permission (group) */
#    define S_IWGRP 0000020     /* write permission (group) */
#    define S_IXGRP 0000010     /* execute permission (group) */

#    define S_IRWXO 0000007     /* read, write, execute permission (other) */
#    define S_IROTH 0000004     /* read permission (other) */
#    define S_IWOTH 0000002     /* write permission (other) */
#    define S_IXOTH 0000001     /* execute permission (other) */

#  define _S_IFMT   0170000     /* type of file */
#  define _S_IFREG  0100000     /* regular */
#  define _S_IFBLK  0060000     /* block special */
#  define _S_IFCHR  0020000     /* character special */
#  define _S_IFDIR  0040000     /* directory */
#  define _S_IFIFO  0010000     /* pipe or FIFO */


#  define S_ISDIR(_M)  ((_M & _S_IFMT)==_S_IFDIR) /* test for directory */
#  define S_ISCHR(_M)  ((_M & _S_IFMT)==_S_IFCHR) /* test for char special */
#  define S_ISBLK(_M)  ((_M & _S_IFMT)==_S_IFBLK) /* test for block special */
#  define S_ISREG(_M)  ((_M & _S_IFMT)==_S_IFREG) /* test for regular file */
#  define S_ISFIFO(_M) ((_M & _S_IFMT)==_S_IFIFO) /* test for pipe or FIFO */
#

int chmod(const char * path, mode_t mode);   
int fstat(int filedes, struct stat *buf);
int mkdir(const char *path, mode_t mode);
int mkfifo(const char * path, mode_t mode);
int stat(const char *file_name, struct stat *buf);
mode_t umask(mode_t cmask);

/********

#define S_IFBLK  0060000
#define S_IFCHR  0020000
#define S_IFDIR  0040000
#define S_IFIFO  0010000
#define S_IFLNK  0120000
#define S_IFREG  0100000
#define S_IFSOCK 0140000
#define _S_IFMT  0170000

#define S_ISLNK(m)      (((m) & S_IFMT) == S_IFLNK)
#define S_ISVTX  0001000
#define S_ISSOCK(m)     (((m) & S_IFMT) == S_IFSOCK)

**********/



#endif







