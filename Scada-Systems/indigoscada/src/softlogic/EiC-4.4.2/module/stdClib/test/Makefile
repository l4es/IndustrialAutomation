#######################################
# define local information first 

       TOP = ../../..
INCLUDES =  -I$(TOP)/include  -I$(TOP)/src  -I. -I-
LOCAL_LIB =  $(TOP)/lib/libstdClib.a	 
  
########################################

include $(TOP)/make.proj

########################################

SRCS = $(fopenSRCS) $(ctypeSRCS) $(printfSRCS) $(scanfSRCS)

install::

#############
fopenSRCS = testfopen.c
fopenOBJS = $(fopenSRCS:.c=.o)
testfopen: $(fopenOBJS)
	$(CC) -o $@ $(fopenOBJS) $(LOCAL_LIB) -lm
all::testfopen
clean::
	$(RM) testfopen $(fopenOBJS)
clobber::clean
##############

#############
ctypeSRCS = testctype.c
ctypeOBJS = $(ctypeSRCS:.c=.o)
testctype: $(ctypeOBJS)
	$(CC) -o $@ $(ctypeOBJS) $(LOCAL_LIB) -lm
all::testctype
clean::
	$(RM) testctype $(ctypeOBJS)
clobber::clean
############

#############
printfSRCS = testprintf.c
printfOBJS = $(printfSRCS:.c=.o)
testprintf: $(printfOBJS)
	$(CC) -o $@ $(printfOBJS) $(LOCAL_LIB) -lm
all::testprintf
clean::
	$(RM) testprintf $(printfOBJS)
clobber::clean
############

#############
scanfSRCS = testscanf.c
scanfOBJS = $(scanfSRCS:.c=.o)
testscanf: $(scanfOBJS)
	$(CC) -o $@ $(scanfOBJS) $(LOCAL_LIB) -lm
all::testscanf
clean::
	$(RM) testscanf $(scanfOBJS)
clobber::clean
############


############################################################

include $(CONFIG_DIR)/make.rules








