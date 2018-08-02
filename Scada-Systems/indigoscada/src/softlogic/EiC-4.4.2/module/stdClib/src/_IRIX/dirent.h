#ifndef _SYS_DIRENTH
#define _SYS_DIRENTH

struct dirent				/* data from getdents()/readdir() */
	{
	long		d_ino;		/* inode number of entry */
	off_t		d_off;		/* offset of disk directory entry */
	unsigned short	d_reclen;	/* length of this record */
	char		d_name[1];	/* name of file */	/* non-POSIX */
	};

#endif /* _SYS_DIRENTH */






