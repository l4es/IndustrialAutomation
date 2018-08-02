#################################################
# HOMEofEiC is the directory above the EiC
# include, lib, and main directories. It will be 
# usually the directory that gets generated when EiC's
# distribution is untarred. If you have su authority, 
# then you might untar EiC in /usr/local, and in which case:
#
# HOMEofEiC = /usr/local/EiC
#
# or you may use one of your own directories.
#################################################

HOMEofEiC :=  $(shell pwd)/..

    TOP = $(HOMEofEiC)


#########################
# Part 2: COMMON MAKE PARAMETERS

include $(TOP)/make.proj
include $(TOP)/make.rules

#################################################
# INSTALL_DIR can be any directory
# that is in your normal search path;
# such as, /usr/local/bin, etc
#################################################


ifneq ($(PLATFORM),_DJGPP)
       INSTALL_DIR = $(HOME)/bin
       EXT=""
else
       INSTALL_DIR = c:/djgpp/bin
       EXT=.exe
endif


################################################# 
# LIBDIRS and LIBS are used to specify the 
# libraries, which EiC must link against to compile. 
################################################# 

LINKFILE = $(TOP)/module/link.libs

LIBDIRS = -L$(HOMEofEiC)/lib 
LIBS = -lstdClib -leic -lm 


DEP_LIBS = $(HOMEofEiC)/lib/libeic.a \
	$(HOMEofEiC)/lib/libstdClib.a \


#################################################
# INCLUDES is used to specify the directories that
# the compiler should add to its search path 
# when looking for `.h' files.
#################################################

INCLUDES = -I. -I$(HOMEofEiC)/include $(INCLUDE_FLAGS)

#################################################
# eicSTR is used to set up the minimum expected set
# of defaults for when eic is launched.
# If these defaults are not set, then the behaviour
# of EiC at runtime will be undefined.
################################################# 

eicSTRold = '$(HOMEofEiCC)/eicc -D_EiC -D$(PLATFORM) \
         -I$(HOMEofEiC)/include  $${1+"$$@"}'

#################################################
# There should be nothing to change from
# This point onwards. :)
#################################################


eicSRCS = main.c
eicOBJS = main.o

SRCS =  $(eicSRCS)


####  EiC
PPLATFORM = P$(PLATFORM) 
DEFINES = -DPLATFORM=$(PPLATFORM)  -DHM=$(HOMEofEiC)
INSTALL_EiC = $(INSTALL) eicc$(EXT) $(INSTALL_DIR)/eic$(EXT)

EXTRALIBS := $(shell cat $(LINKFILE))

eicc: $(eicOBJS) $(DEP_LIBS) 
	$(CC) $(CFLAGS) -o $@  $(eicOBJS) $(LIBDIRS) $(LIBS) $(EXTRALIBS)

all:: eicc
eic::install

install:: $(INSTALL_DIR)/eic$(EXT)
$(INSTALL_DIR)/eic$(EXT): $(DEP_LIBS) $(eicOBJS)
	$(MAKE) all
	$(INSTALL_EiC) 

clobber::
	$(RM) $(eicOBJS) $(INSTALL_DIR)/eic$(EXT)
clean::
	$(RM) $(eicOBJS)

sentinel: $(eicOBJS) $(DEP_LIBS) 
	sentinel $(CC) $(CFLAGS) -static -o $(INSTALL_DIR)/eic $(eicOBJS) $(LIBDIRS) $(LIBS) $(EXTRALIBS)

installBinary:
	mv eicc$(EXT) $(INSTALL_DIR)/eic$(EXT)  

headers::

####













