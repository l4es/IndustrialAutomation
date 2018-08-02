struct termios {
        tcflag_t        c_iflag;        /* input flags */
        tcflag_t        c_oflag;        /* output flags */
        tcflag_t        c_cflag;        /* control flags */
        tcflag_t        c_lflag;        /* local flags */
        cc_t            c_cc[NCCS];     /* control chars */
        int             c_ispeed;       /* input speed */
        int             c_ospeed;       /* output speed */
};















