/* startup.h */

/* string.h */
val_t eic_memcpy(void);
val_t eic_memmove(void);
val_t eic_strcpy(void);
val_t eic_strncpy(void);
val_t eic_strcat(void);
val_t eic_strncat(void);
val_t eic_memcmp(void);
val_t eic_strcmp(void);
val_t eic_strcoll(void);
val_t eic_strncmp(void);
val_t eic_strxfrm(void);
val_t eic_memchr(void);
val_t eic_strchr(void);
val_t eic_strcspn(void);
val_t eic_strpbrk(void);
val_t eic_strrchr(void);
val_t eic_strspn(void);
val_t eic_strstr(void);
val_t eic_strtok(void);
val_t eic_memset(void);
val_t eic_strerror(void);
val_t eic_strlen(void);


val_t eic_strrev(void);
val_t eic_fftoa(void);


/* CTYPE.H STUFF */
val_t _get_ctype(void);

/* FCNTL.H STUFF */
val_t eic_creat(void);
val_t eic_open(void);
val_t eic_fcntl(void);


/* STDLIB.H */
val_t eic_system(void);
val_t eic_malloc(void);
val_t eic_calloc(void);
val_t eic_realloc(void);
val_t eic_free(void);
val_t eic_strtod(void);
val_t eic_strtol(void);
val_t eic_strtoul(void);
val_t eic_rand(void);
val_t eic_srand(void);
val_t eic_atoi(void);
val_t eic_atof(void);
val_t eic_atol(void);
val_t eic_getenv(void);
val_t eic_exit(void);
val_t eic_abort(void);
val_t eic_itoa(void);
val_t eic_ltoa(void);


/* DIRENT.H STUFF */
val_t eic_opendir(void);
val_t eic_readdir(void);
val_t eic_telldir(void);
val_t eic_seekdir(void);
val_t eic_rewinddir(void);
val_t eic_closedir(void);


/* STAT.H STUFF */
val_t eic_chmod(void);
val_t eic_fstat(void);
val_t eic_mkdir(void);
val_t eic_mkfifo(void);
val_t eic_stat(void);
val_t eic_umask(void);


/* ASSERT.H STUFF */
val_t eic_assert(void);


/* TIME.H STUFF */

val_t eic_asctime(void);
val_t eic_clock(void);
val_t eic_ctime(void);
val_t eic_difftime(void);
val_t eic_gmtime(void);
val_t eic_localtime(void);
val_t eic_mktime(void);
val_t eic_strftime(void);
val_t eic_time(void);


/* UNISTD.H STUFF */

val_t eic_access(void);
val_t eic_close(void);
val_t eic_dup(void);
val_t eic_dup2(void);
val_t eic_fork(void);
val_t eic_getpid(void);
val_t eic_link(void);
val_t eic_lseek(void);
val_t eic_pipe(void);
val_t eic_read(void);
val_t eic_rmdir(void);
val_t eic_chdir(void);
val_t eic_sleep(void);
val_t eic_unlink(void);
val_t eic_write(void);


/* ERRNO.H STUFF */
val_t _get_errno(void);

/* STDARG.H STUFF */
#ifndef NO_STDARG
val_t _get_AR_t_size(void);
val_t _StArT_Va(void);
#endif
