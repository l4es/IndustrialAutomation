#ifnedef _IOCTLH
#define  _IOCTLH


/*
   EJB. This header file is by no means compliant
   to any standard. All I have done
   is thrown most of the termios.h 4.3 BSD stuff
   in here plus other ioctl.h things.
*/


#define CBAUD	0010017
#define EXTA    19200
#define EXTB    38400


/* c_lflag bits */
#define ALTWERASE       0x00000200      /* use alternate WERASE algorithm */
#define ECHOCTL         0x00000040      /* echo control chars as ^(Char) */
#define ECHOKE          0x00000001      /* visual erase for line kill */
#define ECHOPRT         0x00000020      /* visual erase mode for hardcopy */
#define FLUSHO          0x00800000      /* output being flushed (state) */
#define MDMBUF          0x00100000      /* flow control output via Carrier */
#define NOHANG          0x01000000      /* XXX this should go away */
#define NOKERNINFO      0x40000000      /* Disable printing kernel info */
#define PENDIN          0x20000000      /* retype pending input (state) */
#define XCASE           0x00004000      /* Cononical upper/lower presentation*/


/* c_iflag bits */
#define IMAXBEL	0020000
#define IUCLC           0x00001000      /* DUMMY VALUE Map upper to lower */
#define IXANY           0x00000800      /* any char will restart after stop */

/* c_oflag bits */
#define ONLCR           0x00000002      /* map NL to CR-NL (ala CRMOD) */
#define OLCUC           0x00000004      /* Map lower case to upper on output */
#define OCRNL           0x00000008      /* Map CR to NL on output */
#define ONOCR           0x00000010      /* No CR output at column 0 */
#define ONLRET          0x00000020      /* NL performs CR function */
#define OFILL           0x00000040      /* Use fill characters for delay */
#define OFDEL           0x00000080      /* fill is DEL, else NUL */
#define NLDLY           0x00000300      /* \n delay */
#define         NL0     0x00000000
#define         NL1     0x00000100      /* tty 37 */
#define         NL2     0x00000200      /* vt05 */
#define         NL3     0x00000300
#define TABDLY          0x00000c00      /* horizontal tab delay */
#define         TAB0    0x00000000
#define         TAB1    0x00000400      /* tty 37 */
#define         TAB2    0x00000800
#define         TAB3    0x00000C00      /* expand tabs on output */
#define CRDLY           0x00003000      /* \r delay */
#define         CR0     0x00000000
#define         CR1     0x00001000      /* tn 300 */
#define         CR2     0x00002000      /* tty 37 */
#define         CR3     0x00003000      /* concept 100 */
#define FFDLY           0x00004000      /* Form feed delay */
#define         FF0     0x00000000
#define         FF1     0x00004000
#define BSDLY           0x00008000      /* \b delay */
#define         BS0     0x00000000
#define         BS1     0x00008000
#define VTDLY           0x00010000      /* vertical tab delay */
#define         VT0     0x00000000
#define         VT1     0x00010000      /* tty 37 */


/* net yet */
#define CBAUDEX 0010000
#define  B57600  0010001
#define  B115200 0010002
#define  B230400 0010003
#define CIBAUD	  002003600000	/* input baud rate (not used) */
#define CRTSCTS	  020000000000		/* flow control */


/* c_cflag */
#define CREAD           0x00000800      /* enable receiver */


#define _O_TIOCSSIZE    _IOW('t',103,struct swsize)     /* set tty size */
#define _O_TIOCGSIZE    _IOR('t',102,struct swsize)     /* get tty size */
#define _N_TIOCSSIZE    _IOW('t',37,struct swsize)      /* set tty size */
#define _N_TIOCGSIZE    _IOR('t',38,struct swsize)      /* get tty size */

#define IOCPARM_MASK    0x1fff          /* parameter length, at most 13 bits */
#define IOCPARM_LEN(x)  (((x) >> 16) & IOCPARM_MASK)
#define IOCBASECMD(x)   ((x) & ~IOCPARM_MASK)
#define IOCGROUP(x)     (((x) >> 8) & 0xff)

#define IOCPARM_MAX     NBPG            /* max size of ioctl, mult. of NBPG */
#define IOC_VOID        0x20000000      /* no parameters */
#define IOC_OUT         0x40000000      /* copy out parameters */
#define IOC_IN          0x80000000      /* copy in parameters */
#define IOC_INOUT       (IOC_IN|IOC_OUT)
#define IOC_DIRMASK     0xe0000000      /* mask for IN/OUT/VOID */

#define _IOC(inout,group,num,len) \
        (inout | ((len & IOCPARM_MASK) << 16) | ((group) << 8) | (num))
#define _IO(g,n)        _IOC(IOC_VOID,  (g), (n), 0)
#define _IOR(g,n,t)     _IOC(IOC_OUT,   (g), (n), sizeof(t))
#define _IOW(g,n,t)     _IOC(IOC_IN,    (g), (n), sizeof(t))
#define _IOWR(g,n,t)    _IOC(IOC_INOUT, (g), (n), sizeof(t))



/*
 * tty ioctl commands
 */
                                                /* 0-2 compat */
#define TIOCMODG        _IOR('t', 3, int)       /* get modem control state */
#define TIOCMODS        _IOW('t', 4, int)       /* set modem control state */
                                                /* 8-10 compat */
#define TIOCEXCL        _IO('t', 13)            /* set exclusive use of tty */
#define TIOCNXCL        _IO('t', 14)            /* reset exclusive use of tty */
                                                /* 15 unused */
#define TIOCFLUSH       _IOW('t', 16, int)      /* flush buffers */
                                                /* 17-18 compat */
#define TIOCGETA        _IOR('t', 19, struct termios) /* get termios struct */
#define TIOCSETA        _IOW('t', 20, struct termios) /* set termios struct */
#define TIOCSETAW       _IOW('t', 21, struct termios) /* drain output, set */
#define TIOCSETAF       _IOW('t', 22, struct termios) /* drn out, fls in, set */
/* System V tty ioctls */

#define TCGETS          TIOCGETA
#define TCSETS          TIOCSETA
#define TCSETSW         TIOCSETAW
#define TCSETSF         TIOCSETAF
#define TCGETA          _IOR('t', 23, struct termio) /* get termio struct */
#define TCSETA          _IOW('t', 24, struct termio) /* set termio struct */
#define TCSETAW         _IOW('t', 25, struct termio) /* drain output, set */
#define TCSETAF         _IOW('t', 28, struct termio) /* drn out, flsh, set */
#define TCSBREAK        _IO('t', 29)            /* Send break */
#define TCSBRK          TCSBREAK
#define TCXONC          _IO('t', 30)            /* Set flow control */
#define TCFLSH          _IO('t', 31)            /* Flush queue */

#define TIOCGETD        _IOR('t', 26, int)      /* get line discipline */
#define TIOCSETD        _IOW('t', 27, int)      /* set line discipline */


/* locals, from 127 down */
                                                /* 127-124 compat */
#define TIOCSBRK        _IO('t', 123)           /* set break bit */
#define TIOCCBRK        _IO('t', 122)           /* clear break bit */
#define TIOCSDTR        _IO('t', 121)           /* set data terminal ready */
#define TIOCCDTR        _IO('t', 120)           /* clear data terminal ready */
#define TIOCGPGRP       _IOR('t', 119, pid_t)   /* get pgrp of tty */
#define TIOCSPGRP       _IOW('t', 118, pid_t)   /* set pgrp of tty */
                                                /* 117-116 compat */
#define TIOCOUTQ        _IOR('t', 115, int)     /* output queue size */
#define TIOCSTI         _IOW('t', 114, char)    /* simulate terminal input */
#define TIOCNOTTY       _IO('t', 113)           /* void tty association */
#define TIOCPKT         _IOW('t', 112, int)     /* pty: set/clear packet mode */
#define TIOCSTOP        _IO('t', 111)           /* stop output, like ^S */
#define TIOCSTART       _IO('t', 110)           /* start output, like ^Q */
#define TIOCMSET        _IOW('t', 109, int)     /* set all modem bits */
#define TIOCMBIS        _IOW('t', 108, int)     /* bis modem bits */
#define TIOCMBIC        _IOW('t', 107, int)     /* bic modem bits */
#define TIOCMGET        _IOR('t', 106, int)     /* get all modem bits */
#define TIOCREMOTE      _IOW('t', 105, int)     /* remote input editing */
#define TIOCGWINSZ      _IOR('t', 104, struct winsize)  /* get window size */
#define TIOCSWINSZ      _IOW('t', 103, struct winsize)  /* set window size */
#define TIOCUCNTL       _IOW('t', 102, int)     /* pty: set/clr usr cntl mode */
#define UIOCCMD(n)      _IO('u', n)             /* usr cntl op "n" */
#define TIOCCONS        _IOW('t', 98, int)     /* become virtual console */
#define TIOCSCTTY       _IO('t', 97)            /* become controlling tty */
#define TIOCEXT         _IOW('t', 96, int)      /* pty: external processing */
#define TIOCSIG         _IO('t', 95)            /* pty: generate signal */



/* Used for packet mode */
#define TIOCPKT_DATA		 0
#define TIOCPKT_FLUSHREAD	 1
#define TIOCPKT_FLUSHWRITE	 2
#define TIOCPKT_STOP		 4
#define TIOCPKT_START		 8
#define TIOCPKT_NOSTOP		16
#define TIOCPKT_DOSTOP		32

struct winsize {
	unsigned shot ws_row;
	unsigned short ws_col;
	unsigned short ws_xpixel;
	unsigned short ws_ypixel;
};

/* not yet: modem lines */
#define         TIOCM_LE        0001            /* line enable */
#define         TIOCM_DTR       0002            /* data terminal ready */
#define         TIOCM_RTS       0004            /* request to send */
#define         TIOCM_ST        0010            /* secondary transmit */
#define         TIOCM_SR        0020            /* secondary receive */
#define         TIOCM_CTS       0040            /* clear to send */
#define         TIOCM_CAR       0100            /* carrier detect */
#define         TIOCM_CD        TIOCM_CAR
#define         TIOCM_RNG       0200            /* ring */
#define         TIOCM_RI        TIOCM_RNG
#define         TIOCM_DSR       0400            /* data set ready */


#endif






