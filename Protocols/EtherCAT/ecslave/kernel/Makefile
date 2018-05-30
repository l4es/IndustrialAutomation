TARGET= ecslave
OBJS  = \
../kernel/ec_slave.o \
../kernel/ec_net.o \
../kernel/ecat_protocol.o \
../kernel/ec_debug.o \
../kernel/ecat_timer.o \
../kernel/ec_device.o \
../stack/ec_cmd_apwr.o \
../stack/ec_cmd_aprd.o \
../stack/ec_cmd_aprw.o \
../stack/ec_cmd_armw.o \
../stack/ec_cmd_brd.o \
../stack/ec_cmd_brw.o \
../stack/ec_cmd_bwr.o \
../stack/ec_cmd_fprd.o \
../stack/ec_cmd_fprw.o \
../stack/ec_cmd_fpwr.o \
../stack/ec_cmd_frmw.o \
../stack/ec_cmd_lrw.o \
../stack/ec_cmd_lrd.o \
../stack/ec_cmd_lwr.o \
../stack/ec_sii.o \
../stack/ec_regs.o \
../stack/ec_coe.o \
../stack/ec_cmd.o \
../stack/ec_mbox.o \
../stack/ec_process_data.o \
../stack/ecat_timer_common.o

CURRENT=`uname -r`
KDIR = /lib/modules/$(CURRENT)/build/

obj-m  := $(TARGET).o
ccflags-y += -I$(PWD)/../include/ -I$(PWD)/../stack/ -I$(PWD)/../devices/ -I$(PWD)/../kernel/
$(TARGET)-objs := $(OBJS)

default:
	make -C $(KDIR) M=$(PWD) SUBDIRS=$(PWD) modules

$(TARGET).o: $(OBJS)
	$(LD) $(LD_RFLAG) -r -o $@ $(OBJS)
clean:
	-rm -rf  ../stack/*.o  *.o .*.o.cmd .*.ko.cmd *.[ch]~ Modules.symvers *.order *.*.ko *.ko *.mod.c .*.c  Module.symvers .tmp_versions/
indent:
	indent -kr -i8 *.[ch]
