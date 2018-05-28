#-----------------------------------------------------------------------------
#
# $Id$
#
#-----------------------------------------------------------------------------

noinst_LIBRARIES = libcommon.a

libcommon_a_SOURCES = \
	Process.cpp \
	Variable.cpp \
	Yaml.cpp

libcommon_a_CXXFLAGS = \
	-Wall
	$(PDCOM_CPPFLAGS) \
	$(YAML_CFLAGS) \
	$(SQLITE_CPPFLAGS)

noinst_HEADERS = \
	Process.h \
	Variable.h \
	Yaml.h

#-----------------------------------------------------------------------------
