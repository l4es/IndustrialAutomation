AC_PREREQ(2.61)
AC_INIT([opendax],[0.6])
AC_CONFIG_SRCDIR([dax/opendax.c])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR([m4])

AC_USE_SYSTEM_EXTENSIONS
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE($PACKAGE_NAME, $PACKAGE_VERSION)

AC_PROG_CC
AC_PROG_LIBTOOL
LT_INIT

AC_ARG_ENABLE([debug],[  --enable-debug    Enable debugging information],
              USE_DEBUG="yes", USE_DEBUG="no")

# These lines are used for the conditional compilation of different modules
AC_ARG_ENABLE([modbus], 
[  --disable-modbus   Do Not Compile Modbus Module], 
[case "${enableval}" in 
    yes) modbus=true ;; 
    no) modbus=false ;;
    *) AC_MSG_ERROR([bad value ${enableval} for --enable-modbus]) ;; 
    esac],[modbus=true]) 
AM_CONDITIONAL([MODBUS], [test x$modbus = xtrue])

AC_ARG_ENABLE([test], 
[  --disable-test   Do Not Compile Test Module], 
[case "${enableval}" in 
    yes) test=true ;; 
    no) test=false ;;
    *) AC_MSG_ERROR([bad value ${enableval} for --enable-test]) ;; 
    esac],[test=true]) 
AM_CONDITIONAL([TEST], [test x$test = xtrue])

AC_ARG_ENABLE([wio], 
[  --disable-wio   Do Not Compile Wireless I/O Module], 
[case "${enableval}" in 
    yes) wio=true ;; 
    no) wio=false ;;
    *) AC_MSG_ERROR([bad value ${enableval} for --enable-wio]) ;; 
    esac],[wio=true]) 
AM_CONDITIONAL([WIO], [test x$wio = xtrue])

AC_ARG_ENABLE([daxlua], 
[  --disable-daxlua   Do Not Compile Dax Lua Module], 
[case "${enableval}" in 
    yes) daxlua=true ;; 
    no) daxlua=false ;;
    *) AC_MSG_ERROR([bad value ${enableval} for --enable-daxlua]) ;; 
    esac],[daxlua=true]) 
AM_CONDITIONAL([DAXLUA], [test x$daxlua = xtrue])

AC_ARG_ENABLE([arduino], 
[  --disable-arduino   Do Not Compile Arduino Module], 
[case "${enableval}" in 
    yes) arduino=true ;; 
    no) arduino=false ;;
    *) AC_MSG_ERROR([bad value ${enableval} for --enable-arduino]) ;; 
    esac],[arduino=true]) 
AM_CONDITIONAL([ARDUINO], [test x$arduino = xtrue])

AC_ARG_ENABLE([histlog], 
[  --disable-histlog   Do Not Compile Historical Logging Module], 
[case "${enableval}" in 
    yes) histlog=true ;; 
    no) histlog=false ;;
    *) AC_MSG_ERROR([bad value ${enableval} for --enable-histlog]) ;; 
    esac],[histlog=true]) 
AM_CONDITIONAL([HISTLOG], [test x$histlog = xtrue])

#These lines will check for the library and add the right stuff to config.h
AC_CHECK_LIB([pthread], [pthread_create], [got_pthread=true])
AC_CHECK_LIB([readline], [readline], [got_readline=true])
AC_CHECK_LIB([mysqlclient], [mysql_init], [got_msyql=true])
AC_CHECK_LIB([dl], [dlopen], [got_dl=true])
AC_CHECK_LIB([ltdl], [lt_dlopen], [got_ltdl=true])
AC_CHECK_LIB([lua],[luaL_newstate], [got_lua=true], [got_lua=false])
AC_CHECK_LIB([lua50],[luaL_newstate], [got_lua50=true], [got_lua50=false])
AC_CHECK_LIB([lua5.1],[luaL_newstate], [got_lua5_1=true], [got_lua5_1=false])
AC_CHECK_LIB([lua51],[luaL_newstate], [got_lua51=true], [got_lua51=false])

#Some Automake conditionals that we can use later
AM_CONDITIONAL([HAVE_PTHREAD], test $got_pthread = true)
AM_CONDITIONAL([HAVE_READLINE], test $got_readline = true)
AM_CONDITIONAL([HAVE_MYSQL], test $got_mysql = true)
AM_CONDITIONAL([HAVE_LUA], test $got_lua = true)
AM_CONDITIONAL([HAVE_LUA51], test $got_lua51 = true)
AM_CONDITIONAL([HAVE_LUA5_1], test $got_lua5_1 = true)

if test $got_lua = true ; then
  LUALIB='-llua'
elif test $got_lua51 = true ; then
  LUALIB='-llua51'
elif test $got_lua5_1 = true ; then
  LUALIB='-llua5.1'
else
  LUALIB=
  echo "--ERROR---------------------------------"
  echo "Lua Libraries are Required to"
  echo "Build OpenDAX!  Install Lua development"
  echo "Packages and try again"
  echo "----------------------------------------"
  (exit 1); exit 1;
fi

AC_SUBST(LUALIB)

# Checks for header files.
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netinet/in.h stdlib.h string.h sys/socket.h sys/ioctl.h sys/param.h sys/time.h syslog.h termios.h unistd.h readline/readline.h signal.h sys/select.h])
AC_CHECK_HEADERS([mysql/mysql.h])
AC_CHECK_HEADERS([ltdl.h dlfcn.h])
AC_CHECK_HEADERS([lua5.1/lua.h lua5.1/lauxlib.h lua5.1/lualib.h], [lua_include = 'lua5.1'])
AC_CHECK_HEADERS([lua51/lua.h lua51/lauxlib.h lua51/lualib.h], [lua_include = 'lua51'])
AC_CHECK_HEADERS([lua/lua.h lua/lauxlib.h lua/lualib.h], [lua_include = 'lua'])
AC_CHECK_HEADERS([lua.h lauxlib.h lualib.h], [lua_include = ''])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_HEADER_TIME

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_TYPE_SIGNAL
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([dup2 gettimeofday memmove memset socket strcasecmp strdup strerror strpbrk strtol strtof strtoul])

CFLAGS="-Wall -DETC_DIR=\\\"$sysconfdir/$PACKAGE\\\"   "

if test $USE_DEBUG = yes ; then
#   DEBUG=1
   CFLAGS="$CFLAGS -DDEBUG=1 -g"
else
#   DEBUG=0
   CFLAGS="$CFLAGS -O3"
fi

AC_SUBST(ac_aux_dir)

#I'll use this once I start on the hmi module.  It's different enough that it justifies it's own conigure.in
#dnl AC_CONFIG_SUBDIRS([modules/hmi])

AC_CONFIG_FILES([Makefile
                 dax/Makefile
                 lib/Makefile
                 lib/lua/Makefile
                 modules/Makefile
                 modules/modbus/Makefile
                 modules/modbus/lib/Makefile
                 modules/daxc/Makefile
                 modules/daxlua/Makefile
                 modules/test/Makefile
                 modules/wio/Makefile
                 modules/arduino/Makefile
                 modules/arduino/lib/Makefile
                 modules/histlog/Makefile
                 modules/histlog/plugins/Makefile
                 modules/histlog/plugins/file/Makefile
                 modules/histlog/plugins/mysql/Makefile
                 modules/skel/Makefile])
AC_OUTPUT

if test $got_msyql != true; then
echo "--WARNING----------------------------------"
echo "Unable to find MySQL Libraries"
echo "Not building MySQL data plugin for the"
echo "Historical Logging Module"
echo "-------------------------------------------"
fi

# Print Build Information


echo \
"-------------------------------------------------

 ${PACKAGE_NAME} Version ${PACKAGE_VERSION}

 Prefix: '${prefix}'.
 Compiler: '${CC} ${CFLAGS} ${CPPFLAGS}'

 Modules to be built:
   modbus:  ${modbus}
   daxlua:  ${daxlua}
   test:    ${test}
   wio:     ${wio}
   arduino: ${arduino}
   histlog: ${histlog}
   
 Now type 'make @<:@<target>@:>@'
   where the optional <target> is:
     all                - build all binaries
     install            - install everything

 Currently the Python Extensions are not built
 from this configuration.  See the file
 lib/python/README for more information.

--------------------------------------------------"

