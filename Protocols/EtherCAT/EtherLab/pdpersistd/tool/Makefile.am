#-----------------------------------------------------------------------------
#
# $Id$
#
#-----------------------------------------------------------------------------

bin_PROGRAMS = pdpersist

pdpersist_SOURCES = \
	Process.cpp \
	main.cpp

pdpersist_CXXFLAGS = \
	-Wall \
	$(YAML_CFLAGS) \
	$(PDCOM_CPPFLAGS) \
	$(SQLITE_CFLAGS)

pdpersist_LDFLAGS = \
	$(PDCOM_LDFLAGS) \
	$(SQLITE_LDFLAGS)

pdpersist_LDADD = \
	$(top_srcdir)/common/libcommon.a \
	$(PDCOM_LIBS) \
	$(YAML_LIBS) \
	$(SQLITE_LIBS)

noinst_HEADERS = \
	Process.h

#-----------------------------------------------------------------------------
