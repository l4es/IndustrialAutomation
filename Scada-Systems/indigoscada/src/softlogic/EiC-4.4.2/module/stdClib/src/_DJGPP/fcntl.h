struct flock {
    off_t l_len;
    pid_t l_pid;
    off_t l_start;
    short l_type;
    short l_whence;
};
