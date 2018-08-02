#ifnedef _IOCTLH
#define  _IOCTLH


/*
   EJB. This header file is by no means compliant
   to any standard. All I have done
   is thrown most of the termios.h 4.3 BSD stuff
   in here plus other ioctl.h things.
*/

#define	VSWTCH		7

#define	CBAUD	0x0000000f
#define	CREAD	0x00000080

#define	TIOCGWINSZ	_IOR(t, 104, struct winsize)	/* get window size */
#define	TIOCSWINSZ	_IOW(t, 103, struct winsize)	/* set window size */

/*
 * Sun version of same.
 */
struct ttysize {
	int	ts_lines;	/* number of lines on terminal */
	int	ts_cols;	/* number of columns on terminal */
};

#define	TIOCSSIZE	_IOW(t,37,struct ttysize)/* set tty size */
#define	TIOCGSIZE	_IOR(t,38,struct ttysize)/* get tty size */


#define	_CTRL(c)	('c'&037)


#define	IUCLC	0x00000200
#define	IXANY	0x00000800
#define	IMAXBEL	0x00002000


/* output modes */
#define	OLCUC	0x00000002
#define	ONLCR	0x00000004
#define	OCRNL	0x00000008
#define	ONOCR	0x00000010
#define	ONLRET	0x00000020
#define	OFILL	0x00000040
#define	OFDEL	0x00000080
#define	NLDLY	0x00000100
#define	NL0	0
#define	NL1	0x00000100
#define	CRDLY	0x00000600
#define	CR0	0
#define	CR1	0x00000200
#define	CR2	0x00000400
#define	CR3	0x00000600
#define	TABDLY	0x00001800
#define	TAB0	0
#define	TAB1	0x00000800
#define	TAB2	0x00001000
#define	XTABS	0x00001800
#define	TAB3	XTABS
#define	BSDLY	0x00002000
#define	BS0	0
#define	BS1	0x00002000
#define	VTDLY	0x00004000
#define	VT0	0
#define	VT1	0x00004000
#define	FFDLY	0x00008000
#define	FF0	0
#define	FF1	0x00008000
#define	PAGEOUT	0x00010000
#define	WRAP	0x00020000


#define	LOBLK	0x00001000
#define	CIBAUD	0x000f0000
#define	CRTSCTS	0x80000000

#define	IBSHIFT	16


/* line discipline 0 modes */

#define	XCASE	0x00000004
#define	ECHOCTL	0x00000200
#define	ECHOPRT	0x00000400
#define	ECHOKE	0x00000800
#define	DEFECHO	0x00001000
#define	FLUSHO	0x00002000
#define	PENDIN	0x00004000


/*
 * codes 1 through 5, not shown here, are old "termio" calls
 */
#define	TCXONC		_IO(T, 6)
#define	TCFLSH		_IO(T, 7)
#define	TCGETS		_IOR(T, 8, struct termios)
#define	TCSETS		_IOW(T, 9, struct termios)
#define	TCSETSW		_IOW(T, 10, struct termios)
#define	TCSETSF		_IOW(T, 11, struct termios)



struct winsize {
	unsigned short	ws_row;		/* rows, in characters */
	unsigned short	ws_col;		/* columns, in characters */
	unsigned short	ws_xpixel;	/* horizontal size, pixels - not used */
	unsigned short	ws_ypixel;	/* vertical size, pixels - not used */
};


#define	EXTA	14
#define	EXTB	15


#endif /* _IOCTLH */
