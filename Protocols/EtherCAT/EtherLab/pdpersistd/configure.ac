#-----------------------------------------------------------------------------
#
# $Id$
#
#-----------------------------------------------------------------------------

AC_PREREQ([2.59])

#
# Release instructions
# - Update version below
# - Update NEWS file
# - Update ChangeLog (hg log --branch default --style=changelog > ChangeLog)
# - commit
# - make dist-bzip2
# - add tag release-x.x.x (hg tag release-x.x.x)
#
AC_INIT([pdpersistd], [1.0.2], [fp@igh.de])
AC_CONFIG_AUX_DIR(autoconf)
AM_INIT_AUTOMAKE([-Wall -Werror])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# automake >= 1.12 needs AM_PROG_AR, older versions don't know it
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

# Checks for programs
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_RANLIB
AC_LIB_PREFIX

# Checks for libraries
AC_CHECK_PDCOM

PKG_CHECK_MODULES([SQLITE], [sqlite3 >= 3.4],,
    AC_MSG_ERROR([sqlite 3 or newer not found.])
)
PKG_CHECK_MODULES([YAML], [yaml-0.1 >= 0.1.4],,
    AC_MSG_ERROR([yaml not found.])
)

# Output files
AC_CONFIG_FILES([
      Makefile
      common/Makefile
      daemon/Makefile
      pdpersistd.spec
      tool/Makefile
      ])
AC_OUTPUT

#-----------------------------------------------------------------------------
