#########################
# Part 1: LOCAL DEFINITIONS

       TOP = ..
     EXTRA = 
  WARNINGS = 
  INCLUDES = 
       LIB = 

  DEP_LIBS = 
      LIBS = 

#      DIRS = `find . -name "CVS" -prune -o -type d -maxdepth 1 -print`
      DIRS = `ls -1 -F | grep / | grep -v CVS`
  MODS_FILE = modules.calls 
  LINKS_FILE = link.libs

#########################
# Part 2: COMMON MAKE PARAMETERS

include $(TOP)/make.proj

#########################
# Part 3: LOCAL SOURCES 


#########################
# Part 4: LOCAL TARGETS  

#### scripts

all::

install::
	$(RM) $(MODS_FILE) 
	touch $(MODS_FILE)
	$(RM) $(LINKS_FILE) 
	touch $(LINKS_FILE)



clean::
clobber::clean
	$(RM) $(LINKS_FILE) $(MODS_FILE)

####### HEADERS
headers::

#########################
# Part 5: COMMON RULES and 
#     and default Targets

include $(CONFIG_DIR)/NormalNodeTargets
include $(CONFIG_DIR)/make.rules

#########################
# Part 6: DEPENDENCIES

