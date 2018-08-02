#ifndef _SYS_DIRENTH
#define _SYS_DIRENTH

#if 0 /* libc5 definition */
struct dirent				/* data from getdents()/readdir() */
	{
	long		d_ino;		/* inode number of entry */
	off_t		d_off;		/* offset of disk directory entry */
	unsigned short	d_reclen;	/* length of this record */
	char		d_name[255+1];	/* name of file */	/* non-POSIX */
	};

#else /* libc6 definition */
struct dirent				/* data from getdents()/readdir() */
	{
	long		d_ino;		/* inode number of entry */
	off_t		d_off;		/* offset of disk directory entry */
	unsigned short	d_reclen;	/* length of this record */
	unsigned char   d_type;         /* libc6 extension */
	char		d_name[255+1];	/* name of file */	/* non-POSIX */
	};
#endif


#endif /* _SYS_DIRENTH */






