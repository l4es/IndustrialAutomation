############################
# Part 1: LOCAL DEFINITIONS

       TOP = ..
  INCLUDES =
   DEFINES =
      LIBS =

#############################
# Part 2: COMMON MAKE PARAMETERS

include $(TOP)/make.proj

#############################
# Part 3: LOCAL SOURCES 

    
HEADER_DIR=$(TOP)/include

SYS_HEADERS = stdio.h stdlib.h errno.h\
              stdtypes.h float.h termdefs.h\
	      types.h


HEADERS = signal.h limits.h fcntl.h


##############################
# Part 4: LOCAL TARGETS  

lib: 

#### CHANGE

genstdio: genstdio.c
	$(CC) $(CFLAGS) -o $@  $@.c $(LIBDIRS) $(LIBS) 

all::
headers:: genstdio 
	./genstdio
	@for i in $(HEADERS) ;\
	do \
		$(MV) ./$$i $(HEADER_DIR) ;\
	done
	@for i in $(SYS_HEADERS) ;\
	do \
		$(MV) ./$$i $(HEADER_DIR)/sys ;\
	done
	$(DUMMY_TARGET)


clobber::
	$(RM) headers

install::
clobber::
	$(RM) ./genstdio
	@for i in $(HEADERS) ;\
	do \
		$(RM) $(HEADER_DIR)/$$i ;\
	done
	@for i in $(SYS_HEADERS) ;\
	do \
		$(RM) $(HEADER_DIR)/sys/$$i ;\
	done
	$(RM) $<

clean::



#############################
# Part 5: COMMON RULES and 
#     and default Targets

include $(CONFIG_DIR)/make.rules

#############################
# Part 6: DEPENDENCIES





















