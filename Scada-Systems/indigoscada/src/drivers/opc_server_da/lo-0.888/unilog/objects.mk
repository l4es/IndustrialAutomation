#################################################
#   Objects declarations for a Makefile
#################################################

OBJ = unilog.$(O) \
sockerr.$(O) ssperr.$(O) raserr.$(O) \
reglog.$(O) advini.$(O) logger.$(RES) 
#dllmain.$(O)

#################################################
#   Common targets and rules 
#################################################

all: $(ALLTARG)

clean-all: 	clean
	-del $(ALLTARG) unilog.lib unilog.map

clean:
	-del $(OBJ)
	-del logger.h msg00001.bin dllmain.$(O) unilog.exp

.cpp.$(O):
	$(CC) -c $(CFLAGS) $(INCLUDES) $<

.c.$(O):
	$(CC) -c $(CFLAGS) $(INCLUDES) $<

.rc.$(RES):
	$(RC) $<

logger.h logger.rc msg00001.bin: logger.mc
	mc logger.mc

include depends.mk

##################################################
