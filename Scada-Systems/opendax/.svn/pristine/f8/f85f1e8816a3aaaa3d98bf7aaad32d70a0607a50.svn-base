bin_PROGRAMS = opendax
opendax_SOURCES = opendax.c options.c options.h \
    func.c func.h module.c module.h \
    message.c message.h tagbase.c tagbase.h \
    crc.c crc.h daxtypes.h libcommon.h buffer.c events.c
#opendax_LDFLAGS = -lpthread
opendax_LDADD = -lpthread @LUALIB@
opendax_DEPENDENCIES = ../common.h
