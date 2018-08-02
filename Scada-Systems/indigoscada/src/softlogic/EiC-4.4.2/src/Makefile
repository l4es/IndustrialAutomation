#########################
# Part 1: LOCAL DEFINITIONS

       TOP = ..
     EXTRA = 
  WARNINGS = 
  INCLUDES = -I$(INCLUDE_DIR) -I.  $(INCLUDE_FLAGS)
       LIB =  libeic.a

  DEP_LIBS = $(LIB_DIR)/$(LIB)  $(LIB_LIST)
      LIBS =  -lm

#########################
# Part 2: COMMON MAKE PARAMETERS

include $(TOP)/make.proj

# Standard bourne shell doesn't update PWD on directory change.
#       SHELL = /bin/ksh

#ifneq ($(PLATFORM),_SUNOS)
#ifneq ($(PLATFORM),_NETBSD)
#       SHELL = /usr/bin/ksh
#else
#       SHELL = /bin/ksh
#endif
#endif 



#########################
# Part 3: LOCAL SOURCES 

libSRCS = eicmod.c  symbol.c lexer.c preproc.c parser.c  starteic.c\
       typemod.c typesets.c cdecl.c interpre.c emitter.c ymem.c error.c \
	preexpr.c readline.c optomizer.c func.c stab.c \
	callback.c reset.c

libOBJS = $(LIB)(eicmod.o) \
 $(LIB)(symbol.o)\
 $(LIB)(lexer.o)\
 $(LIB)(error.o)\
 $(LIB)(preproc.o)\
 $(LIB)(parser.o)\
 $(LIB)(starteic.o)\
 $(LIB)(typemod.o)\
 $(LIB)(typesets.o)\
 $(LIB)(cdecl.o)\
 $(LIB)(interpre.o)\
 $(LIB)(emitter.o)\
 $(LIB)(ymem.o)\
 $(LIB)(preexpr.o)\
 $(LIB)(readline.o) \
 $(LIB)(optomizer.o)\
 $(LIB)(func.o) \
 $(LIB)(stab.o) \
 $(LIB)(callback.o)\
 $(LIB)(reset.o)\


 

SRCS = $(libSRCS)

#########################
# Part 4: LOCAL TARGETS  

#### LIBEIC.A
$(LIB): $(libOBJS) 
	$(RUNLIBCMD)
lib::$(LIB)
all::$(LIB)
install:: 
	@if [ -f $(LIB) ]; then \
		echo PATH1;\
		$(MAKE) lib ;\
		echo $(INSTALL) $(LIB) $(LIB_DIR) ;\
		$(INSTALL) $(LIB) $(LIB_DIR);\
	else \
		echo PATH2;\
		$(MAKE) $(LIB_DIR)/$(LIB) ;\
	fi
$(LIB_DIR)/$(LIB): $(addprefix $(LIB_DIR)/,$(libOBJS))
	echo $(MAKE) $(LIB_DIR)/$(LIB)
	$(RUNLIBCMD)
clobber::
	$(RM) $(LIB_DIR)/$(LIB) $(LIB)
clean::
####


### STARTUP.H
all::
install:: $(INCLUDE_DIR)/starteic.h

$(INCLUDE_DIR)/starteic.h:starteic.src
	$(COPY) starteic.src $(INCLUDE_DIR)/starteic.h

clobber::
	$(RM) $(INCLUDE_DIR)/starteic.h
clean::
####

####### HEADERS
HEADERS = eic.h xalloc.h eicval.h

instH: $(HEADERS)
	echo $(INCLUDE_DIR)
	$(COPY) $? $(INCLUDE_DIR)
	$(DUMMY_TARGET)

headers:: instH

install:: instH 

clobber::
	@for i in $(HEADERS) ;\
	do \
		echo $(RM) $(INCLUDE_DIR)/$$i;\
		$(RM) $(INCLUDE_DIR)/$$i;\
	done
	$(RM) instH
clean::
####


### EiC

eic::install
eic::
	cd ../main ; $(MAKE) install


#########################
# Part 5: COMMON RULES and 
#     and default Targets

include $(CONFIG_DIR)/make.rules

#########################
# Part 6: DEPENDENCIES

$(LIB_DIR)/$(LIB)(eicmod.o): eicmod.c typemod.h global.h datastruct.h stab.h \
 $(TOP)/include/eicval.h lexer.h func.h \
 $(TOP)/include/xalloc.h typesets.h preproc.h cdecl.h \
 error.h emitter.h symbol.h
$(LIB_DIR)/$(LIB)(symbol.o): symbol.c typemod.h MachSet.h global.h datastruct.h stab.h \
 $(TOP)/include/eicval.h lexer.h \
 $(TOP)/include/xalloc.h preproc.h symbol.h
$(LIB_DIR)/$(LIB)(lexer.o): lexer.c global.h typemod.h datastruct.h stab.h \
 $(TOP)/include/eicval.h \
 $(TOP)/include/xalloc.h error.h symbol.h lexer.h
$(LIB_DIR)/$(LIB)(preproc.o): preproc.c stab.h global.h typemod.h datastruct.h \
 $(TOP)/include/eicval.h \
 $(TOP)/include/xalloc.h preproc.h symbol.h
$(LIB_DIR)/$(LIB)(parser.o): parser.c MachSet.h global.h typemod.h datastruct.h stab.h \
 $(TOP)/include/eicval.h lexer.h func.h \
 $(TOP)/include/xalloc.h typesets.h cdecl.h error.h \
 symbol.h
$(LIB_DIR)/$(LIB)(starteic.o): starteic.c MachSet.h global.h typemod.h datastruct.h \
 stab.h $(TOP)/include/eicval.h lexer.h func.h \
 $(TOP)/include/xalloc.h preproc.h emitter.h parser.h \
 symbol.h
$(LIB_DIR)/$(LIB)(typemod.o): typemod.c global.h typemod.h datastruct.h stab.h \
 $(TOP)/include/eicval.h func.h \
 $(TOP)/include/xalloc.h metric.h
$(LIB_DIR)/$(LIB)(typesets.o): typesets.c MachSet.h global.h typemod.h datastruct.h \
 stab.h $(TOP)/include/eicval.h lexer.h \
 $(TOP)/include/xalloc.h typesets.h symbol.h error.h
$(LIB_DIR)/$(LIB)(cdecl.o): cdecl.c MachSet.h global.h typemod.h datastruct.h stab.h \
 $(TOP)/include/eicval.h lexer.h func.h \
 $(TOP)/include/xalloc.h cdecl.h preproc.h error.h \
 typesets.h parser.h symbol.h
$(LIB_DIR)/$(LIB)(interpre.o): interpre.c MachSet.h global.h typemod.h datastruct.h \
 stab.h $(TOP)/include/eicval.h \
 $(TOP)/include/xalloc.h symbol.h
$(LIB_DIR)/$(LIB)(emitter.o): emitter.c MachSet.h global.h typemod.h datastruct.h stab.h \
 $(TOP)/include/eicval.h
$(LIB_DIR)/$(LIB)(ymem.o): ymem.c $(TOP)/include/xalloc.h assertp.h \
 global.h typemod.h datastruct.h stab.h \
 $(TOP)/include/eicval.h
$(LIB_DIR)/$(LIB)(error.o): error.c global.h typemod.h datastruct.h stab.h \
 $(TOP)/include/eicval.h lexer.h \
 $(TOP)/include/xalloc.h preproc.h error.h
$(LIB_DIR)/$(LIB)(preexpr.o): preexpr.c $(TOP)/include/xalloc.h \
 preproc.h
$(LIB_DIR)/$(LIB)(readline.o): readline.c
$(LIB_DIR)/$(LIB)(optomizer.o): optomizer.c MachSet.h global.h typemod.h datastruct.h \
 stab.h $(TOP)/include/eicval.h typesets.h error.h
$(LIB_DIR)/$(LIB)(func.o): func.c global.h typemod.h datastruct.h stab.h \
 $(TOP)/include/eicval.h func.h \
 $(TOP)/include/xalloc.h error.h symbol.h
$(LIB_DIR)/$(LIB)(stab.o): stab.c $(TOP)/include/xalloc.h stab.h
$(LIB_DIR)/$(LIB)(callback.o): callback.c MachSet.h global.h typemod.h datastruct.h \
 stab.h $(TOP)/include/eicval.h func.h \
 $(TOP)/include/xalloc.h typesets.h error.h symbol.h \
 preproc.h $(TOP)/include/eic.h
