#-----------------------------------------------------------------------------
#
# $Id$
#
#-----------------------------------------------------------------------------

INSTALL (FILES
        ecrt_support.h
        DESTINATION ${CMAKE_INSTALL_PREFIX}/rtw/include
)

INSTALL (FILES
        ecrt_support.c
        DESTINATION ${CMAKE_INSTALL_PREFIX}/rtw/src
)

#INCLUDE_DIRECTORIES (${CMAKE_INSTALL_PREFIX}/include)
#
#FIND_LIBRARY (LIBETHERCAT ethercat
#    ${CMAKE_INSTALL_PREFIX}/lib
#)
#
#ADD_LIBRARY( ecrtsupport SHARED
#    ecrt_support.c ecrt_support.h
#)
#
#TARGET_LINK_LIBRARIES (ecrtsupport ${LIBETHERCAT})
#
#INSTALL (TARGETS ecrtsupport
#    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib LIBRARY
#)

# EXTRA_DIST = \
# 	ecrt_support.c \
# 	raise_catch.h \
# 	ecrt_support.h \
# 	libc.c \
# 	raise_catch.c
# 
# #-----------------------------------------------------------------------------
# # libetherlab.a
# #-----------------------------------------------------------------------------
# 
# if APPCORE
# 
# include_rtwdir = $(prefix)/$(RTW_DIR)/include
# include_rtw_HEADERS = raise_catch.h
# 
# mylibdir = $(prefix)/$(subdir)
# mylib_LIBRARIES = libetherlab.a
# mylib_DATA =
# 
# libetherlab_a_SOURCES =
# libetherlab_a_LIBADD = raise_catch.o
# 
# if ETHERCAT
# 
# include_rtw_HEADERS += ecrt_support.h
# libetherlab_a_LIBADD += ecrt_support.o
# 
# if HAVE_ETHERCAT_SYMVERS
# mylib_DATA += @ETHERCAT_SYMVERS@
# endif
# 
# endif
# 
# if LIBC
# libetherlab_a_LIBADD += libc.o
# include_rtw_HEADERS += appcore_libc.h string_redefine.h
# endif
# 
# WINDOWS_FILES = $(mylib_DATA)
# WINDOWS_DIRS = $(DIST_SUBDIRS)
# 
# include $(top_srcdir)/Makefile.windows
# 
# libc.o: Kbuild
# 	make -C "$(LINUX_DIR)" M="@abs_builddir@" \
# 		V="$(VERBOSE)" TGT=$@ "_module_@abs_builddir@"
# 	@touch $@
# 
# raise_catch.o: Kbuild
# 	make -C "$(LINUX_DIR)" M="@abs_builddir@" \
# 		V="$(VERBOSE)" TGT=$@ "_module_@abs_builddir@"
# 	@touch $@
# 
# ecrt_support.o: Kbuild
# 	make -C "$(LINUX_DIR)" M="@abs_builddir@" \
# 		V="$(VERBOSE)" ETHERCAT_DIR=$(ETHERCAT_DIR) TGT=$@ \
# 		"_module_@abs_builddir@"
# 	@touch $@
# 
# clean-local:
# 	make -C "$(LINUX_DIR)" M="@abs_builddir@" \
# 		V="$(VERBOSE)" clean
# 
# endif # APPCORE
