#ifndef _SYS_DIRENTH
#define _SYS_DIRENTH

struct  dirent {
        ino_t    d_ino;               /* file number of entry */
        unsigned short d_reclen;            /* length of this record */
        unsigned short d_namlen;            /* length of string in d_name */
        char    d_name[256];            /* DUMMY NAME LENGTH */
                                        /* the real maximum length is */
                                        /* returned by pathconf() */
                                        /* At this time, this MUST */
                                        /* be 256 -- the kernel */
                                        /* requires it */
};
#endif /* _SYS_DIRENTH */






