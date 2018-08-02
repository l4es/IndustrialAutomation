#ifndef _SYS_DIRENTH
#define _SYS_DIRENTH

struct dirent {				/* data from getdents()/readdir() */
	unsigned	d_ino;		/* file number of entry */
	unsigned short	d_reclen;	/* length of this record */
	unsigned char	d_type;		/* file type */
	unsigned char	d_namlen;	/* length of string in d_name */
	char		d_name[255+1];	/* name of file */
};

#endif /* _SYS_DIRENTH */
