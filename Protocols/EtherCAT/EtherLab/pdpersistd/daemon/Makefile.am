#-----------------------------------------------------------------------------
#
# $Id$
#
#-----------------------------------------------------------------------------

bin_PROGRAMS = pdpersistd

pdpersistd_SOURCES = \
	Process.cpp \
	Variable.cpp \
	main.cpp

pdpersistd_CXXFLAGS = \
	-Wall \
	$(YAML_CFLAGS) \
	$(PDCOM_CPPFLAGS)

pdpersistd_LDFLAGS = \
	$(PDCOM_LDFLAGS)

pdpersistd_LDADD = \
	$(top_srcdir)/common/libcommon.a \
	$(PDCOM_LIBS) \
	$(YAML_LIBS) \
	$(SQLITE_LIBS)

noinst_HEADERS = \
	Process.h \
	Variable.h

#-----------------------------------------------------------------------------
