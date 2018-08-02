struct  flock {
  short   l_type;
  short   l_whence;
  off_t   l_start;
  off_t   l_len;          /* len = 0 means until end of file */
  pid_t   l_pid;
};
