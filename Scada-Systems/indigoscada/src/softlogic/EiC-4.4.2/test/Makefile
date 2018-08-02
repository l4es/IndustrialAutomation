#########################
# Part 1: LOCAL DEFINITIONS

       TOP = ..
     EXTRA = 
  WARNINGS = 
  INCLUDES = 
       LIB = 

  DEP_LIBS = 
      LIBS = 
 
#################################################
# INSTALL_DIR can be any directory
# that is in your normal search path;
# such as, /usr/local/bin, etc
#################################################

INSTALL_DIR = $(HOME)/bin



#########################
# Part 2: COMMON MAKE PARAMETERS

include $(TOP)/make.proj

#########################
# Part 3: LOCAL SOURCES 

TSTDIRS = gnutests testcode lcctest posix.1


#########################
# Part 4: LOCAL TARGETS  

#### runtest and shell scripts

$(INSTALL_DIR)/eictester:runtest
	cp runtest $(INSTALL_DIR)/eictester
all::
install::$(INSTALL_DIR)/eictester 
clean::
	find . -name "*.diff" | xargs $(RM)
	find . -name "*.out"  | xargs $(RM)

clobber::clean
	$(RM) $(INSTALL_DIR)/eictester

####### HEADERS
headers::

#########################
# Part 5: COMMON RULES and 
#     and default Targets

include $(CONFIG_DIR)/make.rules

#########################
# Part 6: DEPENDENCIES

