############################
# Part 1: LOCAL DEFINITIONS
############################

       TOP = ../../..
  INCLUDES = -I$(TOP)/include  -I. $(INCLUDE_FLAGS)


#############################
# Part 2: COMMON MAKE PARAMETERS


include $(TOP)/make.proj

#############################
# Part 3: LOCAL SOURCES 

LIB = libstdClib.a

HEADERS = ctype.h  float.h math.h  \
          stdarg.h stddef.h stdio.h stdlib.h  string.h \
	  dirent.h unistd.h assert.h time.h \
	  termios.h errno.h setjmp.h iso646.h

SYS_HEADERS = \
	$(PLATFORM)/dirent.h\
	$(PLATFORM)/stat.h\
	$(PLATFORM)/time.h \
	$(PLATFORM)/termios.h\
	$(PLATFORM)/fcntl.h \




SRCS = ctype.c eicfftoa.c itoa.c \
       stdClib.c stdlib.c strrev.c strround.c stdio.c math.c\
	fcntl.c string.c dirent.c assert.c  time.c unistd.c\
	stat.c  stdarg.c errno.c termios.c signal.c 



libOBJS = $(LIB)(ctype.o)\
 $(LIB)(eicfftoa.o)\
 $(LIB)(itoa.o)\
 $(LIB)(stdClib.o)\
 $(LIB)(stdlib.o)\
 $(LIB)(strrev.o)\
 $(LIB)(strround.o)\
 $(LIB)(stdio.o)\
 $(LIB)(math.o)\
 $(LIB)(fcntl.o)\
 $(LIB)(string.o)\
 $(LIB)(dirent.o)\
 $(LIB)(assert.o)\
 $(LIB)(time.o)\
 $(LIB)(unistd.o)\
 $(LIB)(stat.o)\
 $(LIB)(stdarg.o)\
 $(LIB)(errno.o)\
 $(LIB)(termios.o)\
 $(LIB)(signal.o)\



	   



##############################
# Part 4: LOCAL TARGETS  

headers:: instH  instSysH

#### HEADERS
instH: $(HEADERS)
	echo $(INCLUDE_DIR)
	$(COPY) $? $(INCLUDE_DIR)
	$(DUMMY_TARGET)
all::
install::
	$(MAKE) instH
clobber::
	@for i in $(HEADERS) ;\
	do \
		echo $(RM) $(INCLUDE_DIR)/$$i;\
		$(RM) $(INCLUDE_DIR)/$$i;\
	done
	$(RM) instH
clean::
####


#### SYS_HEADERS
instSysH: $(SYS_HEADERS)
	@for i in ${?F} ;\
	do \
		echo $(COPY) $(PLATFORM)/$$i $(INCLUDE_DIR)/sys ;\
		$(COPY) $(PLATFORM)/$$i $(INCLUDE_DIR)/sys ;\
	done
	$(DUMMY_TARGET)
install:: instSysH
clobber::$(SYS_HEADERS)
	@for i in $(?F) ;\
	do \
		echo $(RM) $(INCLUDE_DIR)/sys/$$i ;\
		$(RM) $(INCLUDE_DIR)/sys/$$i ;\
	done
	$(RM) instSysH
clean::
####


#### LIB.A
$(LIB): $(libOBJS)
	$(RUNLIBCMD)
lib::$(LIB)
all::$(LIB)

install:: 
	@if [ -f $(LIB) ]; then \
		$(MAKE) lib ;\
		echo $(INSTALL) $(LIB) $(LIB_DIR) ;\
		$(INSTALL) $(LIB) $(LIB_DIR);\
	else \
		echo $(MAKE) $(LIB_DIR)/$(LIB);\
		$(MAKE) $(LIB_DIR)/$(LIB) ;\
	fi
$(LIB_DIR)/$(LIB): $(addprefix $(LIB_DIR)/,$(libOBJS))
	$(RUNLIBCMD)
clobber::
	$(RM) $(LIB_DIR)/$(LIB) $(LIB)
clean::
####

#############################
# Part 5: COMMON RULES and 
#     and default Targets

include $(CONFIG_DIR)/make.rules

#############################
# Part 6: DEPENDENCIES
$(LIB_DIR)/$(LIB)(ctype.o): ctype.c ../../../include/ctype.h ../../../include/eic.h \
 ../../../include/eicval.h stdliblocal.h
$(LIB_DIR)/$(LIB)(eicfftoa.o): eicfftoa.c stdliblocal.h
$(LIB_DIR)/$(LIB)(itoa.o): itoa.c stdliblocal.h
$(LIB_DIR)/$(LIB)(stdClib.o): stdClib.c
$(LIB_DIR)/$(LIB)(stdlib.o): stdlib.c ../../../include/eic.h ../../../include/eicval.h \
 stdliblocal.h ../../../include/xalloc.h
$(LIB_DIR)/$(LIB)(strrev.o): strrev.c
$(LIB_DIR)/$(LIB)(strround.o): strround.c
$(LIB_DIR)/$(LIB)(stdio.o): stdio.c ../../../include/eic.h ../../../include/eicval.h \
 stdliblocal.h
$(LIB_DIR)/$(LIB)(math.o): math.c ../../../include/eic.h ../../../include/eicval.h
$(LIB_DIR)/$(LIB)(fcntl.o): fcntl.c ../../../include/eic.h ../../../include/eicval.h
$(LIB_DIR)/$(LIB)(string.o): string.c ../../../include/eic.h ../../../include/eicval.h \
 stdliblocal.h
$(LIB_DIR)/$(LIB)(dirent.o): dirent.c ../../../include/eic.h ../../../include/eicval.h \
 stdliblocal.h
$(LIB_DIR)/$(LIB)(assert.o): assert.c ../../../include/eic.h ../../../include/eicval.h \
 stdliblocal.h
$(LIB_DIR)/$(LIB)(time.o): time.c ../../../include/eic.h ../../../include/eicval.h \
 stdliblocal.h
$(LIB_DIR)/$(LIB)(unistd.o): unistd.c ../../../include/eic.h ../../../include/eicval.h \
 stdliblocal.h
$(LIB_DIR)/$(LIB)(stat.o): stat.c ../../../include/eic.h ../../../include/eicval.h \
 stdliblocal.h
$(LIB_DIR)/$(LIB)(stdarg.o): stdarg.c ../../../include/eic.h ../../../include/eicval.h \
 stdliblocal.h
$(LIB_DIR)/$(LIB)(errno.o): errno.c ../../../include/eic.h ../../../include/eicval.h \
 stdliblocal.h
