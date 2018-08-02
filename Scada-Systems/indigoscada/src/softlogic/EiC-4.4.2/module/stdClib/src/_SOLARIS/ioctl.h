#ifnedef _IOCTLH
#define  _IOCTLH


/*
   EJB. This header file is by no means compliant
   to any standard. All I have done
   is thrown most of the termios.h 4.3 BSD stuff
   in here plus other ioctl.h things.
*/


#define	ECHOCTL	0001000
#define	ECHOKE	0004000
#define	ECHOPRT	0002000
#define	FLUSHO	0020000
#define	LOBLK	0040000

#define	PAREXT	04000000
#define	PENDIN	0040000
#define	RCV1EN	0010000
#define	TCFLSH	(_TIOC|7)
#define	TCGETA	(_TIOC|1)
#define	TCSBRK	(_TIOC|5)
#define	TCSETA	(_TIOC|2)
#define	TCSETAF	(_TIOC|4)
#define	TCSETAW	(_TIOC|3)
#define	TCXONC	(_TIOC|6)
#define	TIOC	_TIOC
#define	XCASE	0000004
#define	XCLUDE	0100000		/* *V7* exclusive use coming fron XENIX */
#define	XMT1EN	0020000


/* c_iflag */
#define	DOSMODE	0100000  /* for 386 compatibility */
#define	IMAXBEL 0020000
#define	IUCLC	0001000

/* ?? */
#define	CBAUD	0000017
#define	CIBAUD	03600000
#define	CREAD	0000200
#define	CRTSCTS	020000000000
#define	DEFECHO	0010000




pid_t tcgetsid(); /*???*/




/* c_oflag */
#define	OLCUC	0000002
#define	ONLCR	0000004
#define	OCRNL	0000010
#define	ONOCR	0000020
#define	ONLRET	0000040
#define	OFILL	0000100
#define	OFDEL	0000200
#define	NLDLY	0000400
#define	NL0	0
#define	NL1	0000400
#define	CRDLY	0003000
#define	CR0	0
#define	CR1	0001000
#define	CR2	0002000
#define	CR3	0003000
#define	TABDLY	0014000
#define	TAB0	0
#define	TAB1	0004000
#define	TAB2	0010000
#define	TAB3	0014000
#define	XTABS	0014000
#define	BSDLY	0020000
#define	BS0	0
#define	BS1	0020000
#define	VTDLY	0040000
#define	VT0	0
#define	VT1	0040000
#define	FFDLY	0100000
#define	FF0	0
#define	FF1	0100000
#define	PAGEOUT 0200000
#define	WRAP	0400000


/* termios ioctls */

#define	TCGETS		(_TIOC|13)
#define	TCSETS		(_TIOC|14)
#define	TCSETSW		(_TIOC|15)
#define	TCSETSF		(_TIOC|16)



/* TIOC ioctls for BSD, ptys, job control and modem control */



#define	TIOCGETD	(tIOC|0)
#define	TIOCSETD	(tIOC|1)
#define	TIOCHPCL	(tIOC|2)
#define	TIOCGETP	(tIOC|8)
#define	TIOCSETP  	(tIOC|9)
#define	TIOCSETN	(tIOC|10)
#define	TIOCEXCL	(tIOC|13)
#define	TIOCNXCL	(tIOC|14)
#define	TIOCFLUSH	(tIOC|16)
#define	TIOCSETC	(tIOC|17)
#define	TIOCGETC	(tIOC|18)


/* POSIX job control ioctls */

#define	TIOCGPGRP	(tIOC|20)	/* get pgrp of tty */
#define	TIOCSPGRP	(tIOC|21)	/* set pgrp of tty */
#define	TIOCGSID	(tIOC|22)	/* get session id on ctty */
#define	TIOCSSID	(tIOC|24)	/* set session id on ctty */

/* Miscellanous */
#define	TIOCSTI		(tIOC|23)	/* simulate terminal input */

/* Modem control */
#define	TIOCMSET	(tIOC|26)	/* set all modem bits */
#define	TIOCMBIS	(tIOC|27)	/* bis modem bits */
#define	TIOCMBIC	(tIOC|28)	/* bic modem bits */
#define	TIOCMGET	(tIOC|29)	/* get all modem bits */
#define	TIOCM_LE	0001		/* line enable */
#define	TIOCM_DTR	0002		/* data terminal ready */
#define	TIOCM_RTS	0004		/* request to send */
#define	TIOCM_ST	0010		/* secondary transmit */
#define	TIOCM_SR	0020		/* secondary receive */
#define	TIOCM_CTS	0040		/* clear to send */
#define	TIOCM_CAR	0100		/* carrier detect */
#define	TIOCM_CD	TIOCM_CAR
#define	TIOCM_RNG	0200		/* ring */
#define	TIOCM_RI	TIOCM_RNG
#define	TIOCM_DSR	0400		/* data set ready */


struct winsize {
	unsigned short ws_row;	        /* rows, in characters */
	unsigned short ws_col;	        /* columns, in character */
	unsigned short ws_xpixel;	/* horizontal size, pixels */
	unsigned short ws_ypixel;	/* vertical size, pixels */
};


#define	VSWTCH	7

#endif /* _IOCTLH */












