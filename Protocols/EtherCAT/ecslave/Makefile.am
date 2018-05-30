bin_PROGRAMS = ec_slave
ec_slave_SOURCES = \
stack/ec_cmd_apwr.c \
stack/ec_cmd_aprd.c \
stack/ec_cmd_aprw.c \
stack/ec_cmd_armw.c \
stack/ec_cmd_brd.c \
stack/ec_cmd_brw.c \
stack/ec_cmd_bwr.c \
stack/ec_cmd_fprd.c \
stack/ec_cmd_fprw.c \
stack/ec_cmd_fpwr.c \
stack/ec_cmd_frmw.c \
stack/ec_cmd_lrw.c \
stack/ec_cmd_lrd.c \
stack/ec_cmd_lwr.c \
stack/ec_sii.c \
stack/ec_regs.c \
stack/ec_coe.c \
stack/ec_mbox.c \
stack/ec_cmd.c \
stack/ecat_timer_common.c \
stack/ec_process_data.c \
user/ec_slave.c \
user/ecat_timer.c \
user/ec_net.c \
user/ec_com.c \
user/ec_device.c \
user/ec_debug.c

ec_slave_CFLAGS  = -Wall -Werror -g3 -O0 -Iinclude/ -Iuser/ -Istack/
if HAVE_DEBUG
ec_slave_CFLAGS += -D__MAKE_DEBUG__
endif
ec_slave_LDFLAGS= -pthread
ec_slave_LDADD = -lrt -lpcap 
