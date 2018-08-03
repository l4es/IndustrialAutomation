#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT(plcemu, 1.6, kalamara@users.sourceforge.net)
AM_INIT_AUTOMAKE(plcemu, 1.6)
AC_CONFIG_SRCDIR([src/plcemu.c])

# Checks for programs.
AC_PROG_CC
# Checks for libraries.
LT_INIT

# Checks for libraries.

# a function in `-lm':
AC_CHECK_LIB(m, floor)
# a function in `-lncurses':
AC_CHECK_LIB(ncurses, init_pair)
#  a function in `-lcomedi':
AC_CHECK_LIB(comedi, comedi_close)
# Checks for header files.

AC_CHECK_HEADERS([fcntl.h stdlib.h string.h, comedi.h])

# Checks for typedefs, structures, and compiler characteristics.
#AC_CHECK_HEADER_STDBOOL

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([gettimeofday memset strchr strdup strrchr])

AC_ARG_ENABLE([sim],
[  --enable-sim    simulated hardware],
[case "${enableval}" in
  yes) sim=true ;;
  no)  sim=false ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-sim]) ;;
esac],[sim=true])
AM_CONDITIONAL([SIMULATION], [test x$sim = xtrue])

AC_ARG_ENABLE([uspace],
[  --enable-uspace    direct user space hw control],
[case "${enableval}" in
  yes) uspace=true ;;
  no)  uspace=false ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-uspace]) ;;
esac],[uspace=false])
AM_CONDITIONAL([USPACE], [test x$uspace = xtrue])


AC_ARG_ENABLE([ui],
[  --disable-ui   user interface],
[case "${enableval}" in
  yes) ui=true ;;
  no)  ui=false ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-ui]) ;;
esac],[ui=true])
AM_CONDITIONAL([UI], [test x$ui = xtrue])


AC_CONFIG_FILES([Makefile])
AC_OUTPUT
