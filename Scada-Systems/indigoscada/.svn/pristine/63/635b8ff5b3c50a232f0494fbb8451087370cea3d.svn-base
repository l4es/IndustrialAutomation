struct flock {
#if !defined(_EiC) || defined(__LP64__)
	off_t	l_start;	/* starting offset */
	off_t	l_len;		/* len = 0 means until end of file */
#else
# if __LITTLE_ENDIAN__
	off_t	l_start;
	int	__pad0;
	off_t	l_len;
	int	__pad1;
# else
	int	__pad0;
	off_t	l_start;
	int	__pad1;
	off_t	l_len;
# endif
#endif
	pid_t	l_pid;		/* lock owner */
	short	l_type;		/* lock type: read/write, etc. */
	short	l_whence;	/* type of l_start */
};
