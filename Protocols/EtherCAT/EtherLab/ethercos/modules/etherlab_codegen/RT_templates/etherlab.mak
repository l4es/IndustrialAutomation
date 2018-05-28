# Makefile generate from template etherlab.mak
# ========================================

all: ../$$MODEL$$

ETHERLABDIR = /opt/etherlab
C_FLAGS = 
SCIDIR = $$SCILAB_DIR$$
#ETHERLABBASEDIR = $(SCIDIR)/contrib/etherlab
ETHERLABBASEDIR = $$ETLPATH$$
ETHERLABBASEMODULEDIR = $(ETHERLABBASEDIR)/modules/etherlab_base
ETHERLABCODEGENDIR = $(ETHERLABBASEDIR)/modules/etherlab_codegen

RM = rm -f
FILES_TO_CLEAN = *.o ../$$MODEL$$

CC = gcc
CC_OPTIONS = -O2 --static -g -DNDEBUG -Dlinux -DNARROWPROTO -D_GNU_SOURCE -fno-stack-protector   

MODEL = $$MODEL$$
OBJSSTAN = rtmain.o  $$MODEL$$.o $$OBJ$$ msr_reg_scicos.o common.o

SCILIBS = $(ETHERLABCODEGENDIR)/libs_codegen/scicos.a $(ETHERLABCODEGENDIR)/libs_codegen/poly.a $(ETHERLABCODEGENDIR)/libs_codegen/calelm.a $(ETHERLABCODEGENDIR)/libs_codegen/blas.a $(ETHERLABCODEGENDIR)/libs_codegen/lapack.a $(ETHERLABCODEGENDIR)/libs_codegen/os_specific.a
OTHERLIBS = 
ULIBRARY = $(ETHERLABBASEMODULEDIR)/src/c/libetherlab_basis.a $(ETHERLABBASEDIR)/src/c/libetherlab.a $(ETHERLABDIR)/lib/libethercat.a

MSRSRC = rt_lib/msr-core/msr_main.c \
       	 rt_lib/msr-core/msr_charbuf.c \
         rt_lib/msr-core/msr_reg.c \
         rt_lib/msr-core/msr_interpreter.c \
         rt_lib/msr-utils/msr_utils.c \
         rt_lib/msr-core/msr_messages.c \
         rt_lib/msr-core/msr_error_reg.c \
         rt_lib/msr-core/msr.c \
         rt_lib/msr-core/msr_attributelist.c \
         rt_lib/msr-math/msr_base64.c \
         rt_lib/msr-math/msr_hex_bin.c 


CFLAGS = $(CC_OPTIONS) -I$(SCIDIR)/routines $(C_FLAGS) -DMODEL=$(MODEL) -DMODELN=$(MODEL).c -I$(ETHERLABDIR)/include -I$(ETHERLABBASEDIR)/includes -I./rt_lib/include

rtmain.c: $(ETHERLABCODEGENDIR)/RT_templates/rtmain.c $(MODEL).c rt_lib msr_reg_scicos.c msr_reg_scicos.h
	cp $< .

scicos_block4.h : $(ETHERLABBASEDIR)/includes/scicos_block4.h
	cp $< .

machine.h : $(ETHERLABBASEDIR)/includes/machine.h
	cp $< .

msr_reg_scicos.c: $(ETHERLABCODEGENDIR)/RT_templates/msr_reg_scicos.c 
	cp $< .

msr_reg_scicos.h: $(ETHERLABCODEGENDIR)/RT_templates/msr_reg_scicos.h 
	cp $< .

rt_lib: $(ETHERLABCODEGENDIR)/RT_templates/rt_lib
	cp -r $< .

../$$MODEL$$: $(OBJSSTAN) $(MSRSRC:.c=.o) scicos_block4.h machine.h
	gcc -static -g -o $@  $(OBJSSTAN) $(SCILIBS) $(ULIBRARY) $(MSRSRC:.c=.o) -lpthread  -lstdc++ -lrt -lm
	@echo "### Created executable: $(MODEL) ###"

clean::
	@$(RM) $(FILES_TO_CLEAN)
