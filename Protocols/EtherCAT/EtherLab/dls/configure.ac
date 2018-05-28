#------------------------------------------------------------------------------
#
# This file is part of the Data Logging Service (DLS).
#
# DLS is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# DLS is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along
# with DLS. If not, see <http://www.gnu.org/licenses/>.
#
#------------------------------------------------------------------------------

#
# Release instructions
# - Update version below
# - Update LIBDLS_VERSION below
# - Update NEWS file
# - Update ChangeLog (hg log --branch default --style=changelog > ChangeLog)
# - commit
# - make dist-bzip2
# - add tag release-x.x.x
#
AC_INIT([dls],[1.4.0-rc2],[fp@igh.de])
AC_CONFIG_AUX_DIR([autoconf])
AM_INIT_AUTOMAKE([-Wall -Werror dist-bzip2 subdir-objects])

# support older autotools versions
ifdef([AC_CONFIG_MACRO_DIR], AC_CONFIG_MACRO_DIR([m4]))
ifdef([AS_HELP_STRING], [], [AC_DEFUN([AS_HELP_STRING])])

# automake >= 1.12 needs AM_PROG_AR, older versions don't know it
ifdef([AM_PROG_AR], [AM_PROG_AR])

AC_PROG_CXX
AC_PROG_LIBTOOL
AC_PREFIX_DEFAULT([/usr/local])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([config.h.in])
AC_LIB_PREFIX
AM_PATH_XML2

PKG_CHECK_MODULES([PCRE], [libpcre >= 8])

# https://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html#Updating-version-info
#
# current[:revision[:age]]
#
# - If the library source code has changed at all since the last update, then
# increment revision (‘c:r:a’ becomes ‘c:r+1:a’).
# - If any interfaces have been added, removed, or changed since the last
# update, increment current, and set revision to 0.
# - If any interfaces have been added since the last public release, then
# increment age.
# - If any interfaces have been removed or changed since the last public
# release, then set age to 0.
#
AC_SUBST(LIBDLS_VERSION, [4:0:0])

#------------------------------------------------------------------------------
# Daemon
#------------------------------------------------------------------------------

AC_MSG_CHECKING([whether to build the daemon dlsd])

AC_ARG_ENABLE([daemon],
    AS_HELP_STRING([--enable-daemon], [Enable the daemon dlsd]),
    [
        case "${enableval}" in
            yes) enabledaemon=1
                ;;
            no) enabledaemon=0
                ;;
            *) AC_MSG_ERROR([Invalid value for --enable-daemon])
                ;;
        esac
    ],
    [enabledaemon=1]
)

if test "x$enabledaemon" = "x1"; then
    AC_MSG_RESULT([yes])
else
    AC_MSG_RESULT([no])
fi

AM_CONDITIONAL(ENABLE_DAEMON, test "x$enabledaemon" = "x1")
AC_SUBST(ENABLE_DAEMON, [$enabledaemon])

if test "x$enabledaemon" = "x1"; then

AC_MSG_CHECKING([for PdCom 3.0 library])

AC_LANG(C++)
AC_COMPILE_IFELSE([
AC_LANG_SOURCE([
#include <pdcom.h>
#if PDCOM_MAJOR == 3 && PDCOM_MINOR == 0
/* FINE */
#else
# error need PdCom 3.0.x
#endif
])], AC_MSG_RESULT([found]), AC_MSG_ERROR([not found]))

fi

#------------------------------------------------------------------------------
# FLTK
#------------------------------------------------------------------------------

AC_MSG_CHECKING([whether to build the FLTK GUIs])

AC_ARG_ENABLE([fltk],
    AS_HELP_STRING([--enable-fltk], [Enable FLTK GUIs]),
    [
        case "${enableval}" in
            yes) enablefltk=1
                ;;
            no) enablefltk=0
                ;;
            *) AC_MSG_ERROR([Invalid value for --enable-fltk])
                ;;
        esac
    ],
    [enablefltk=1]
)

if test "x$enablefltk" = "x1"; then
    AC_MSG_RESULT([yes])
else
    AC_MSG_RESULT([no])
fi

AM_CONDITIONAL(ENABLE_FLTK, test "x$enablefltk" = "x1")
AC_SUBST(ENABLE_FLTK, [$enablefltk])

AC_ARG_WITH([fltk-dir],
    AC_HELP_STRING(
        [--with-fltk-dir=<fltk-dir>],
        [FLTK directory [[/usr/local]]]),
    [fltkdir=$withval], [fltkdir="/usr"])

if test "x$enablefltk" = "x1"; then
    AC_PATH_PROG([fltkconf], [fltk-config],, [$fltkdir/bin:$PATH])

    if test -z "$fltkconf"; then
        AC_MSG_ERROR([fltk-config not found. Please use --with-fltk-dir.]);
    else
        FLTK_CPPFLAGS=$($fltkconf --cxxflags)
        FLTK_LDFLAGS=$($fltkconf --ldflags)
    fi
fi

AC_SUBST(FLTK_CPPFLAGS)
AC_SUBST(FLTK_LDFLAGS)

#------------------------------------------------------------------------------
# FFTW3
#------------------------------------------------------------------------------

AC_MSG_CHECKING(FFTW3)

AC_ARG_WITH([fftw3-dir],
    AC_HELP_STRING(
        [--with-fftw3-dir=<fftw3-dir>],
        [FFTW3 directory [[/usr/local:/usr]]]),
    [
        test -r "$withval"/include/fftw3.h ||\
            AC_MSG_ERROR([FFTW3 not installed in $withval])
    ],
    [
	    if test -r /usr/local/include/fftw3.h; then
	        withval=/usr/local
	    elif test -r /usr/include/fftw3.h; then
            withval=/usr
        else
            AC_MSG_ERROR([FFTW3 not installed in /usr/local or /usr!])
        fi
    ])

AC_SUBST(FFTW3_CFLAGS,[-I$withval/include])
AC_SUBST(FFTW3_LDFLAGS,[-L$withval/$acl_libdirstem])
AC_SUBST(FFTW3_LIBS,[-lfftw3])
AC_MSG_RESULT([$withval])

#------------------------------------------------------------------------------
# ZLib
#------------------------------------------------------------------------------

AC_MSG_CHECKING(for ZLib)

AC_ARG_WITH([zlib-dir],
    AC_HELP_STRING(
        [--with-zlib-dir=<zlib-dir>],
        [ZLib directory [[/usr/local:/usr]]]),
    [
        test -r "$withval"/include/zlib.h ||\
            AC_MSG_ERROR([ZLib not installed in $withval])
    ],
    [
	    if test -r /usr/local/include/zlib.h; then
	        withval=/usr/local
	    elif test -r /usr/include/zlib.h; then
            withval=/usr
        else
            AC_MSG_ERROR([ZLib not installed in /usr/local or /usr!])
        fi
    ])

AC_SUBST(ZLIB_CFLAGS, [-I$withval/include])
AC_SUBST(ZLIB_LDFLAGS, [-L$withval/$acl_libdirstem])
AC_SUBST(ZLIB_LIBS, [-lz])
AC_MSG_RESULT([$withval])

#------------------------------------------------------------------------------
# Command-line tool
#------------------------------------------------------------------------------

AC_MSG_CHECKING([whether to build the command-line tool dls])

AC_ARG_ENABLE([tool],
    AS_HELP_STRING([--enable-tool], [Enable the command-line tool dls]),
    [
        case "${enableval}" in
            yes) enabletool=1
                ;;
            no) enabletool=0
                ;;
            *) AC_MSG_ERROR([Invalid value for --enable-tool])
                ;;
        esac
    ],
    [enabletool=1]
)

if test "x$enabletool" = "x1"; then
    AC_MSG_RESULT([yes])
else
    AC_MSG_RESULT([no])
fi

AM_CONDITIONAL(ENABLE_TOOL, test "x$enabletool" = "x1")
AC_SUBST(ENABLE_TOOL, [$enabletool])

#------------------------------------------------------------------------------
# uriparser
#------------------------------------------------------------------------------

URIPARSER_MISSING="Please install uriparser."
AC_CHECK_LIB(uriparser, uriParseUriA,, AC_MSG_ERROR(${URIPARSER_MISSING}))
AC_CHECK_HEADER(uriparser/Uri.h,, AC_MSG_ERROR(${URIPARSER_MISSING}))

#------------------------------------------------------------------------------
# TCP server functionality
#------------------------------------------------------------------------------

AC_MSG_CHECKING([whether to build the DLS protocol server])

AC_ARG_ENABLE([server],
    AS_HELP_STRING([--enable-server], [Enable the server protocol]),
    [
        case "${enableval}" in
            yes) enableserver=1
                ;;
            no) enableserver=0
                ;;
            *) AC_MSG_ERROR([Invalid value for --enable-tool])
                ;;
        esac
    ],
    [enableserver=1]
)

if test "x$enableserver" = "x1"; then
    AC_MSG_RESULT([yes])
    AC_DEFINE([DLS_SERVER], [1], [TCP server enabled])
else
    AC_MSG_RESULT([no])
fi

AM_CONDITIONAL(ENABLE_SERVER, test "x$enableserver" = "x1")

#------------------------------------------------------------------------------
# Enable protocol debugging
#------------------------------------------------------------------------------

AC_MSG_CHECKING([whether to build with protocol debug output])

AC_ARG_ENABLE([proto-debug],
    AS_HELP_STRING([--enable-proto-debug], [Enable protocol debugging]),
    [
        case "${enableval}" in
            yes) enableprotodebug=1
                ;;
            no) enableprotodebug=0
                ;;
            *) AC_MSG_ERROR([Invalid value for --enable-proto-debug])
                ;;
        esac
    ],
    [enableprotodebug=0]
)

if test "x$enableprotodebug" = "x1"; then
    AC_MSG_RESULT([yes])
    AC_DEFINE([DLS_PROTO_DEBUG], [1], [protocol debugging enabled])
else
    AC_MSG_RESULT([no])
fi

#------------------------------------------------------------------------------

AC_DEFINE([BINDIR], [], [Fully expanded binary directory.])
AX_DEFINE_DIR([BINDIR], [bindir])

#------------------------------------------------------------------------------

AC_CONFIG_FILES([
    Makefile
    daemon/Makefile
    dls.spec
    fltkguis/Makefile
    fltkguis/images/Makefile
    lib/LibDLS/Makefile
    lib/Makefile
    proto/Makefile
    script/Makefile
    script/dlsd.service
    script/init.sh
    tool/Makefile
])
AC_OUTPUT

#------------------------------------------------------------------------------
