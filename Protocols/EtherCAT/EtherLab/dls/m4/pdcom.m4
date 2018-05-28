dnl pdcom.m4 -- Find the path to PdCom library
dnl $Id$
dnl
dnl This file provides AC_CHECK_PDCOM, which defines the --with-pdcom-dir
dnl command-line option and probes for libpdcom in that directory if it
dnl is used.
dnl
dnl If --with-pdcom-dir is not supplied, it probes in the default locations
dnl /usr, /usr/local.
dnl
dnl It then makes sure that libpdcom will link, and exports PDCOM_LDFLAGS, 
dnl PDCOM_CPPFLAGS, and PDCOM_LIBS

AC_DEFUN([AC_CHECK_PDCOM], [

dnl header file to search for
headerfile=pdcom.h

dnl libtool library file to search for
la_libfile=libpdcom.la

dnl usual shared object file and function to search for
so_libfile=pdcom
dnl so_func_search=XML_ParserCreate

dnl define configure command line argument
AC_ARG_WITH([pdcom-dir],
    [AC_HELP_STRING(--with-pdcom-dir=PATH,
        [Path to PdCom installation])],
    [ DIR="$with_pdcom_dir" ],
    [ DIR=]
)

AC_MSG_CHECKING([for $headerfile])
if test "x$DIR" = x; then
    dnl if pdcom-dir was not specified, check default header location
    for dir in /usr/local /usr ; do
        dirheader="$dir/include/$headerfile"
        if test -f $dirheader ; then
            found=yes
            local_CPPFLAGS="-I$dir/include"
            AC_MSG_RESULT([$dirheader])
            break
        fi
    done
    if test "x$found" = "x"; then
        AC_MSG_ERROR([$headerfile not found])
    fi
else
    dnl check whether DIR/include contains header. If not, bail out
    if test -f "$DIR/include/$headerfile" ; then
        local_CPPFLAGS="-I$DIR/include"
        AC_MSG_RESULT([$DIR/include/$headerfile])
    else
        AC_MSG_ERROR([Could not find $DIR/include/$headerfile])
    fi
fi

dnl Check for supplied libtool library
if test "x$la_libfile" != x; then
    AC_MSG_CHECKING([for $la_libfile])
    la_LIBS=
    for dir in "$DIR" /usr/local /usr; do
        pdcom_libdir="$dir/$acl_libdirstem"
        lib_la="$pdcom_libdir/$la_libfile"
        if test -f "$lib_la"; then
            la_LIBS="$lib_la"
            AC_MSG_RESULT([$la_LIBS])
            break;
        fi
    done
    if test "x$la_LIBS" = x; then
         AC_MSG_RESULT(not found)
    fi
fi

if test "x$so_libfile" != x; then
    so_file=lib${so_libfile}.so
    AC_MSG_CHECKING([for $so_file])
    so_LIBS=
    for dir in "$DIR" /usr/local /usr; do
        pdcom_libdir="$dir/$acl_libdirstem"
        lib_so="$pdcom_libdir/$so_file"
        if test -f "$lib_so"; then
            so_LIBS="-l$so_libfile"
            so_LDFLAGS="-L$pdcom_libdir -Wl,--rpath -Wl,$pdcom_libdir"
            AC_MSG_RESULT([$lib_so])
            break;
        fi
    done
    if test "x$so_LIBS" = x; then
         AC_MSG_RESULT(not found)
    fi
fi

dnl if test "x$la_LIBS" != x; then
dnl     local_LIBS="$la_LIBS"
dnl     local_LDFLAGS=
if test "x$so_LIBS" != x; then
    local_LIBS="$so_LIBS"
    local_LDFLAGS=$so_LDFLAGS
else
    AC_MSG_ERROR([could not find library to link to])
fi

PDCOM_CPPFLAGS="$local_CPPFLAGS"
PDCOM_LDFLAGS="$local_LDFLAGS"
PDCOM_LIBS="$local_LIBS"

dnl AC_MSG_NOTICE([
dnl PDCOM_CPPFLAGS = "$PDCOM_CPPFLAGS"
dnl PDCOM_LDFLAGS = "$PDCOM_LDFLAGS"
dnl PDCOM_LIBS = "$PDCOM_LIBS"
dnl ])

AC_SUBST([PDCOM_CPPFLAGS])
AC_SUBST([PDCOM_LDFLAGS])
AC_SUBST([PDCOM_LIBS])

])
