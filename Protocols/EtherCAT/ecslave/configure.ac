AC_PREREQ([2.65])
AC_INIT([ec_slave], [0.1], [raziebe@gmail.com])
AC_CONFIG_SRCDIR([stack/])
AC_CONFIG_SRCDIR([user/])
AC_CONFIG_FILES([Makefile])
AM_INIT_AUTOMAKE(ec_slave, 0.1)
# Checks for programs.
AC_PROG_CC
AM_INIT_AUTOMAKE([subdir-objects])
# Checks for debug
     AC_ARG_ENABLE([debug],
     [  --enable-debug    Turn on debugging],
     [case "${enableval}" in
       yes) debug=true ;;
       no)  debug=false ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
     esac],[debug=false])
     AM_CONDITIONAL([HAVE_DEBUG], [test x$debug = xtrue])

AC_CHECK_HEADERS([sys/socket.h])
AC_CHECK_HEADERS([stdlib.h])
AC_CHECK_HEADERS([arpa/inet.h])

AC_C_INLINE
AM_PROG_CC_C_O
AC_CHECK_FUNCS([socket])

AC_OUTPUT
