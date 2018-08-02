struct termios {
  cc_t		c_cc[NCCS];
  tcflag_t	c_cflag;
  tcflag_t	c_iflag;
  tcflag_t	c_lflag;
  tcflag_t	c_oflag;
  speed_t	c_ispeed;
  speed_t	c_ospeed;
};
