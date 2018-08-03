#
# ClassicLadder project - Makefile
# Copyright (C) 2001-2017 Marc Le Douarain
# http://www.sourceforge.net/projects/classicladder
# http://sites.google.com/site/classicladder
#
# Last update : December 2015
#

# Comment if you do not want the GTK interface (for an embedded version)
GTK_INTERFACE = 1

# Compile support for Xenomai (real-time extension for Linux, see www.xenomai.org)
#XENOMAI_SUPPORT = 1

# Compile for Windows
#WINDOWS = 1

# Uncomment if you want to debug a 'segmentation fault' with gdb or KDbg for example... ;-)
#DEBUG = 1

# To add extra features like events log, monitor protocol (perhaps not required when only rung/grafcet motor engine used)
COMPLETE_PLC = 1

# Uncomment if you want to build a static library
# BUILD_LIB = 1

# Comment if you do not want the I/O access for the x86 platform
# Now on Windows using library "inpout32.dll" !
MAKE_X86_IO_ACCESS = 1
#MAKE_EMBEDDED_PLC_486 = 1

# Also comment MAKE_X86_IO_ACCESS/MAKE_EMBEDDED_PLC_486 above for Raspberry or for AtmelSAM, should be exclusive...
#MAKE_RASPBERRYPI_GPIO_ACCESS = 1
#MAKE_ATMEL_SAM_GPIO_ACCESS = 1

# Uncomment the following flag to make the MAT connection version
####MAT_CONNECTION = 1

# Uncomment if you want to be able to use comedi drivers (see www.comedi.org)
###export COMEDI_SUPPORT = 1

# Comment if you do not want to support this language
export SEQUENTIAL_SUPPORT = 1
# Comment if you do not want to have the old timers/monostables blocks (usefull for compatibility with old projects before IEC timers)
OLD_TIMERS_MONOS = 1
# for EMC usage, compatibility with old versions without this input
###FORCE_INP_CONTROL_OLD_TIMERS_FLAG = 1


# Cross-compilation possible here...
#-----------------------------------
CROSS = 
#CROSS = arm-linux-gnueabi-
###CROSS = /usr/local/arm/4.5.1/bin/arm-linux-
CC = $(CROSS)gcc
AR = $(CROSS)ar
PREFIX=/usr
BINDIR=${PREFIX}/bin
LIBDIR=${PREFIX}/lib
INCLUDEDIR=${PREFIX}/include

# Embedded special flags...
ifndef GTK_INTERFACE
OWN_CFLAGS = -march=i486
OWN_LIBS = -march=i486
endif


OWN_LIBS += -lz

#GTK2 or GTK3 choice (exclusive!!!)
#Default GTK2 Version
ifdef GTK_INTERFACE
OWN_CFLAGS += -Wall `pkg-config gtk+-2.0 --cflags` `pkg-config pango --cflags` `pkg-config gthread-2.0 --cflags` -DGTK_INTERFACE
OWN_LIBS += `pkg-config gtk+-2.0 --cflags --libs` `pkg-config pango --cflags --libs` `pkg-config gthread-2.0 --cflags --libs`
###Flags that can added to see warnings for future GTK3...
###OWN_CFLAGS += -DGSEAL_ENABLE
endif
#For GTK3 Version
ifdef GTK_INTERFACE
#OWN_CFLAGS += -Wall `pkg-config gtk+-3.0 --cflags` `pkg-config pango --cflags` -DGTK_INTERFACE
#OWN_LIBS += `pkg-config gtk+-3.0 --cflags --libs` `pkg-config pango --cflags --libs`
endif

#Windows
ifdef WINDOWS
OWN_LIBS += -lwsock32
#Dec.2016, for MSYS2/Win32
OWN_CFLAGS += -D__WIN32__
endif

OWN_CFLAGS += -DMODBUS_IO_MASTER

# the pthread is included with gthread2.0 of pkg-config
ifndef GTK_INTERFACE
# v0.9.30, for clock_gettime() also used under Linux! OWN_LIBS += -lpthread
OWN_LIBS += -lpthread -lrt
endif

ifdef SEQUENTIAL_SUPPORT
OWN_CFLAGS += -DSEQUENTIAL_SUPPORT
endif
ifdef OLD_TIMERS_MONOS
OWN_CFLAGS += -DOLD_TIMERS_MONOS_SUPPORT
endif
ifdef FORCE_INP_CONTROL_OLD_TIMERS_FLAG
OWN_CFLAGS += -DFORCE_INP_CONTROL_OLD_TIMERS
endif
ifdef COMPLETE_PLC
OWN_CFLAGS += -DCOMPLETE_PLC
#ifndef GTK_INTERFACE
##OWN_LIBS += -lm
#OWN_LIBS += -lz
#endif
endif

ifdef COMEDI_SUPPORT
OWN_CFLAGS += -DCOMEDI_SUPPORT
COMEDI_LIB = -lcomedi
endif

ifdef MAKE_X86_IO_ACCESS
OWN_CFLAGS += -DX86_IO_ACCESS
ifdef MAKE_EMBEDDED_PLC_486
OWN_CFLAGS += -DEMBEDDED_PLC_486
endif
endif

ifdef MAKE_RASPBERRYPI_GPIO_ACCESS
OWN_CFLAGS += -DRASPBERRY_GPIO_ACCESS
OWN_LIBS += -lwiringPi
endif

ifdef XENOMAI_SUPPORT
OWN_CFLAGS += `xeno-config --skin posix --cflags`
OWN_LIBS += `xeno-config --skin posix --ldflags`
endif

ifdef DEBUG
OWN_CFLAGS += -g -DNO_SEGV_TRAP
endif

RM = rm

OBJ_COMMON = calc_.o files.o files_project.o arithm_eval_.o manager_.o arrays_.o socket_server.o protocol_modbus_slave_.o symbols.o vars_system.o tasks.o time_and_rtc.o
ifdef COMPLETE_PLC
OBJ_COMMON += log_events.o monitor_protocol.o monitor_transfer.o base64.o monitor_protocol_adds_serial.o monitor_threads.o monitor_sockets_udp.o cJSON.o preferences.o modem.o frames_log_buffers.o
endif
OBJ_COMMON += socket_modbus_master.o protocol_modbus_master.o
ifdef GTK_INTERFACE
#GTK3 OBJ_GTK_INTERFACE = drawing.o edit.o edit_gtk.o editproperties_gtk.o manager_gtk.o classicladder_gtk.o config_gtk.o vars_names.o symbols_gtk.o spy_vars_gtk.o print_gtk.o edit_copy.o menu_and_toolbar_gtk.o monitor_windows_gtk.o search.o icons_gtk.o network_config_window_gtk.o monitor_serial_config_window_gtk.o
OBJ_GTK_INTERFACE = drawing.o edit.o editproperties_gtk.o manager_gtk.o config_gtk.o vars_names.o symbols_gtk.o spy_vars_gtk.o print_gtk.o edit_copy.o menu_and_toolbar_gtk.o monitor_windows_gtk.o search.o network_config_window_gtk.o monitor_serial_config_window_gtk.o edit_gtk.o classicladder_gtk.o icons_gtk.o 
ifdef COMPLETE_PLC
OBJ_GTK_INTERFACE += log_events_gtk.o 
endif
OBJ_CLASSICLADDER = $(OBJ_COMMON) $(OBJ_GTK_INTERFACE) vars_access_.o
else
OBJ_CLASSICLADDER = $(OBJ_COMMON) vars_access_.o
endif
ifdef WINDOWS
OBJ_CLASSICLADDER += serial_win.o
else
OBJ_CLASSICLADDER += serial_linux.o
endif
#i18n
XGETTEXT_C_FILE_LIST = classicladder.c edit.c edit_sequential.c editproperties_gtk.c  manager_gtk.c config_gtk.c vars_names.c symbols_gtk.c spy_vars_gtk.c print_gtk.c menu_and_toolbar_gtk.c monitor_windows_gtk.c search.c network_config_window_gtk.c monitor_serial_config_window_gtk.c edit_gtk.c classicladder_gtk.c monitor_threads.c monitor_protocol.c monitor_transfer.c log_events_gtk.c

OBJ_CLASSICLADDER_LIB = $(OBJ_COMMON) ladderlib.o

ifdef SEQUENTIAL_SUPPORT
OBJ_CLASSICLADDER_LIB += files_sequential.o calc_sequential_.o
OBJ_CLASSICLADDER += files_sequential.o calc_sequential_.o
ifdef GTK_INTERFACE
OBJ_CLASSICLADDER += drawing_sequential.o edit_sequential.o
endif
endif

ifdef MAKE_ATMEL_SAM_GPIO_ACCESS
OWN_CFLAGS += -DATMEL_SAM_GPIO_ACCESS
OBJ_CLASSICLADDER += ../wiringSam/wiringSam.o
endif

# Some objects names end with _ because there are not identical if compiled for RTLinux or not.

ifdef MAT_CONNECTION
MAT = ../..
OBJ_CLASSICLADDER_MAT = $(OBJ_COMMON) vars_mat.o $(MAT)/lib/libmatplc.la -lrt
all: classicladder classicladder-mat
else
ifdef BUILD_LIB
all: classicladder-lib
else
ifdef WINDOWS
all: ../classicladder
else
all: ../classicladder ../classiclauncher
endif
endif
endif

../classicladder: $(OBJ_CLASSICLADDER) classicladder_.o hardware_.o
	$(CC) -o ../classicladder $(OBJ_CLASSICLADDER) hardware_.o classicladder_.o $(OWN_LIBS) $(COMEDI_LIB)
	
../classiclauncher: classiclauncher.c
	$(CC) -o $@ classiclauncher.c

../serial_test: serial_test_rs485.o  serial_linux.o
	$(CC) -o ../serial_test serial_test_rs485.o serial_linux.o
	
../time_test: time_test_rtc.o time_and_rtc.o
	$(CC) -o ../time_test time_test_rtc.o time_and_rtc.o
	
########
# i18n #
########
# -> xgettext: generate the ".pot" base model (after modifying strings language in the .c sources)
# -> msginit_XX: FIRST TIME ONLY (!!!!) to create a blank ".po"
# -> msgmerge_XX : later to complete and update a po translation already done !
# -> msgfmt : compile 'po' (text) files in 'mo' (binary) 
xgettext:
	xgettext --keyword=_ --keyword=N_ -o ../po/classicladder.pot $(XGETTEXT_C_FILE_LIST)
msginit_fr:
	msginit --locale=fr_FR.utf8 --input=../po/classicladder.pot --output=../po/fr.po
msgmerge_fr:
	msgmerge -v -U ../po/fr.po ../po/classicladder.pot
msgfmt:
	-mkdir ../po/fr_FR
	-mkdir ../po/fr_FR/LC_MESSAGES
	msgfmt ../po/fr.po -o ../po/fr_FR/LC_MESSAGES/classicladder.mo

classicladder-mat: $(OBJ_CLASSICLADDER_MAT) classicladder_mat.o
	libtool $(CC) -o classicladder-mat $(OBJ_CLASSICLADDER_MAT) classicladder_mat.o $(OWN_LIBS)

classicladder-lib: libladder.a

libladder.a: $(OBJ_CLASSICLADDER_LIB)
	$(AR) -r libladder.a $(OBJ_CLASSICLADDER_LIB)

rtl_support: OWN_CFLAGS+=-DRT_SUPPORT -DRTLINUX
rtl_support: $(OBJ_CLASSICLADDER) classicladder_rtl_support.o
	$(CC) -o classicladder_rtl_support $(OBJ_CLASSICLADDER) classicladder_rtl_support.o $(OWN_LIBS)
	make -f Makefile_rtlinux

rtai_support: OWN_CFLAGS+=-DRT_SUPPORT -DRTAI -I/usr/src/rtai/include
rtai_support: $(OBJ_CLASSICLADDER) classicladder_rtai_support.o
	$(CC) -o classicladder_rtl_support $(OBJ_CLASSICLADDER) classicladder_rtai_support.o $(OWN_LIBS)
	make -f Makefile_rtai
	make test2 -f Makefile_rtai

test:
	make test -f Makefile_rtlinux

classicladder_.o : classicladder.c classicladder.h global.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)
classicladder_rtl_support.o : classicladder.c classicladder.h global.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS) -DRT_SUPPORT -DRTLINUX
classicladder_rtai_support.o : classicladder.c classicladder.h global.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS) -DRT_SUPPORT -DRTAI
classicladder_mat.o : classicladder.c classicladder.h global.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS) -DMAT_CONNECTION

classicladder_gtk.o : classicladder_gtk.c classicladder.h global.h ../icons/IconClassicLadderApplication.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

calc_.o : calc.c classicladder.h global.h calc.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

###config.o : config.c classicladder.h global.h config.h
###	$(CC) -c $< -o $@ $(OWN_CFLAGS)

arrays_.o : arrays.c classicladder.h global.h calc.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

drawing.o : drawing.c classicladder.h global.h drawing.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

drawing_sequential.o : drawing_sequential.c classicladder.h global.h drawing_sequential.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

edit.o : edit.c classicladder.h global.h edit.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

edit_gtk.o : edit_gtk.c classicladder.h global.h edit_gtk.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

editproperties_gtk.o : editproperties_gtk.c classicladder.h global.h editproperties_gtk.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

manager_gtk.o : manager_gtk.c classicladder.h global.h manager_gtk.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

config_gtk.o : config_gtk.c classicladder.h global.h config_gtk.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

files.o : files.c classicladder.h global.h files.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

files_project.o : files_project.c classicladder.h global.h files.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

files_sequential.o : files_sequential.c classicladder.h global.h files_sequential.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

ladderlib.o : ladderlib.c ladderlib.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

ladderlib.h: classicladder.h global.h ladderlib.head ladderlib.tail
	cat ladderlib.head classicladder.h global.h ladderlib.tail >ladderlib.h

vars_access_.o : vars_access.c classicladder.h global.h vars_access.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

arithm_eval_.o : arithm_eval.c classicladder.h global.h vars_access.h arithm_eval.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

manager_.o : manager.c classicladder.h global.h vars_access.h manager.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

vars_mat.o : vars_mat.c
	$(CC) -c $< -o $@ $(OWN_CFLAGS) -DMAT_CONNECTION

calc_sequential_.o : calc_sequential.c classicladder.h global.h calc_sequential.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

hardware_.o : hardware.c classicladder.h global.h hardware.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

edit_sequential.o : edit_sequential.c classicladder.h global.h edit_sequential.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

socket_server.o : socket_server.c classicladder.h global.h socket_server.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

protocol_modbus_slave_.o : protocol_modbus_slave.c classicladder.h global.h protocol_modbus_slave.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

socket_modbus_master.o : socket_modbus_master.c classicladder.h global.h socket_modbus_master.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

protocol_modbus_master.o : protocol_modbus_master.c classicladder.h global.h protocol_modbus_master.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

serial_linux.o : serial_linux.c serial_common.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)
serial_win.o : serial_win.c serial_common.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)
serial_test_rs485.o : serial_test_rs485.c serial_common.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

###print_gnome.o : print_gnome.c classicladder.h global.h print_gnome.h
###	$(CC) -c $< -o $@ $(OWN_CFLAGS)

symbols_gtk.o : symbols_gtk.c classicladder.h global.h symbols_gtk.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

symbols.o : symbols.c classicladder.h global.h symbols.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

spy_vars_gtk.o : spy_vars_gtk.c classicladder.h global.h spy_vars_gtk.h ../icons/IconWindowSpyBoolVars.h ../icons/IconWindowSpyFreeVars.h ../icons/IconOutputOff.h ../icons/IconOutputOn.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

vars_names.o : vars_names.c classicladder.h global.h vars_names.h vars_names_list.c
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

print_gtk.o : print_gtk.c classicladder.h global.h print_gtk.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

edit_copy.o : edit_copy.c classicladder.h global.h edit_copy.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

vars_system.o : vars_system.c classicladder.h global.h vars_system.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

time_and_rtc.o : time_and_rtc.c classicladder.h global.h time_and_rtc.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

log_events.o : log_events.c classicladder.h global.h log_events.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

log_events_gtk.o : log_events_gtk.c classicladder.h global.h log_events_gtk.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

menu_and_toolbar_gtk.o : menu_and_toolbar_gtk.c classicladder.h global.h menu_and_toolbar_gtk.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

cJSON.o : cJSON.c cJSON.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)
monitor_protocol.o : monitor_protocol.c classicladder.h global.h monitor_protocol.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)
monitor_threads.o : monitor_threads.c classicladder.h global.h monitor_threads.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)
monitor_sockets_udp.o : monitor_sockets_udp.c classicladder.h global.h monitor_sockets_udp.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)
monitor_protocol_adds_serial.o : monitor_protocol_adds_serial.c classicladder.h global.h monitor_protocol_adds_serial.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)
monitor_windows_gtk.o : monitor_windows_gtk.c classicladder.h global.h monitor_windows_gtk.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)
monitor_transfer.o : monitor_transfer.c classicladder.h global.h monitor_transfer.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)
base64.o : base64.c base64.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

preferences.o : preferences.c preferences.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

search.o : search.c search.h classicladder.h global.h 
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

tasks.o : tasks.c tasks.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

network_config_window_gtk.o : network_config_window_gtk.c classicladder.h global.h network_config_window_gtk.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

monitor_serial_config_window_gtk.o : monitor_serial_config_window_gtk.c classicladder.h global.h monitor_serial_config_window_gtk.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

modem.o : modem.c modem.h global.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

frames_log_buffers.o : frames_log_buffers.c frames_log_buffers.h global.h
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

../wiringSam/wiringSam.o: ../wiringSam/wiringSam.c
	$(CC) -c  $< -o $@ $(OWN_CFLAGS)

ICONS_LIST = ../icons/IconClassicLadderApplication.h ../icons/IconCallCoil.h ../icons/IconClosedContact.h ../icons/IconCoil.h ../icons/IconCoilNot.h ../icons/IconCompareBox.h \
   ../icons/IconConnection.h ../icons/IconCounterBox.h ../icons/IconFallingEdge.h ../icons/IconIECTimerBox.h ../icons/IconJumpCoil.h ../icons/IconOldMonoBox.h \
   ../icons/IconOldTimerBox.h ../icons/IconOpenContact.h ../icons/IconOperateBox.h ../icons/IconRegisterBox.h ../icons/IconResetCoil.h ../icons/IconRisingEdge.h ../icons/IconSetCoil.h \
   ../icons/IconWindowSpyBoolVars.h ../icons/IconWindowSpyFreeVars.h ../icons/IconOutputOff.h ../icons/IconOutputOn.h \
   ../icons/IconWire.h ../icons/IconWireLong.h ../icons/IconEraser.h \
   ../icons/IconSeqStep.h ../icons/IconSeqInitStep.h ../icons/IconSeqTransi.h ../icons/IconSeqStepAndTransi.h \
   ../icons/IconSeqTransisOr1.h ../icons/IconSeqTransisOr2.h ../icons/IconSeqStepsAnd1.h ../icons/IconSeqStepsAnd2.h \
   ../icons/IconSeqLink.h ../icons/IconSeqComment.h 

icons_gtk.o : icons_gtk.c classicladder.h global.h icons_gtk.h  $(ICONS_LIST)
	$(CC) -c $< -o $@ $(OWN_CFLAGS)

../icons/IconClassicLadderApplication.h : ../icons/IconClassicLadderApplication.ico
	gdk-pixbuf-csource --raw --name=IconClassicLadderApplication $< > $@
../icons/IconCallCoil.h : ../icons/IconCallCoil.ico
	gdk-pixbuf-csource --raw --name=IconCallCoil $< > $@
../icons/IconClosedContact.h : ../icons/IconClosedContact.ico
	gdk-pixbuf-csource --raw --name=IconClosedContact $< > $@
../icons/IconCoil.h : ../icons/IconCoil.ico
	gdk-pixbuf-csource --raw --name=IconCoil $< > $@
../icons/IconCoilNot.h : ../icons/IconCoilNot.ico
	gdk-pixbuf-csource --raw --name=IconCoilNot $< > $@
../icons/IconCompareBox.h : ../icons/IconCompareBox.ico
	gdk-pixbuf-csource --raw --name=IconCompareBox $< > $@
../icons/IconConnection.h : ../icons/IconConnection.ico
	gdk-pixbuf-csource --raw --name=IconConnection $< > $@
../icons/IconCounterBox.h : ../icons/IconCounterBox.ico
	gdk-pixbuf-csource --raw --name=IconCounterBox $< > $@
../icons/IconFallingEdge.h : ../icons/IconFallingEdge.ico
	gdk-pixbuf-csource --raw --name=IconFallingEdge $< > $@
../icons/IconIECTimerBox.h : ../icons/IconIECTimerBox.ico
	gdk-pixbuf-csource --raw --name=IconIECTimerBox $< > $@
../icons/IconJumpCoil.h : ../icons/IconJumpCoil.ico
	gdk-pixbuf-csource --raw --name=IconJumpCoil $< > $@
../icons/IconOldMonoBox.h : ../icons/IconOldMonoBox.ico
	gdk-pixbuf-csource --raw --name=IconOldMonoBox $< > $@
../icons/IconOldTimerBox.h : ../icons/IconOldTimerBox.ico
	gdk-pixbuf-csource --raw --name=IconOldTimerBox $< > $@
../icons/IconOpenContact.h : ../icons/IconOpenContact.ico
	gdk-pixbuf-csource --raw --name=IconOpenContact $< > $@
../icons/IconOperateBox.h : ../icons/IconOperateBox.ico
	gdk-pixbuf-csource --raw --name=IconOperateBox $< > $@
../icons/IconRegisterBox.h : ../icons/IconRegisterBox.ico
	gdk-pixbuf-csource --raw --name=IconRegisterBox $< > $@
../icons/IconResetCoil.h : ../icons/IconResetCoil.ico
	gdk-pixbuf-csource --raw --name=IconResetCoil $< > $@
../icons/IconRisingEdge.h : ../icons/IconRisingEdge.ico
	gdk-pixbuf-csource --raw --name=IconRisingEdge $< > $@
../icons/IconSetCoil.h : ../icons/IconSetCoil.ico
	gdk-pixbuf-csource --raw --name=IconSetCoil $< > $@
../icons/IconWindowSpyBoolVars.h : ../icons/IconWindowSpyBoolVars.ico
	gdk-pixbuf-csource --raw --name=IconWindowSpyBoolVars $< > $@
../icons/IconWindowSpyFreeVars.h : ../icons/IconWindowSpyFreeVars.ico
	gdk-pixbuf-csource --raw --name=IconWindowSpyFreeVars $< > $@
../icons/IconOutputOff.h : ../icons/IconOutputOff.ico
	gdk-pixbuf-csource --raw --name=IconOutputOff $< > $@
../icons/IconOutputOn.h : ../icons/IconOutputOn.ico
	gdk-pixbuf-csource --raw --name=IconOutputOn $< > $@
../icons/IconWire.h : ../icons/IconWire.ico
	gdk-pixbuf-csource --raw --name=IconWire $< > $@
../icons/IconWireLong.h : ../icons/IconWireLong.ico
	gdk-pixbuf-csource --raw --name=IconWireLong $< > $@
../icons/IconEraser.h : ../icons/IconEraser.ico
	gdk-pixbuf-csource --raw --name=IconEraser $< > $@
../icons/IconSeqStep.h : ../icons/IconSeqStep.ico
	gdk-pixbuf-csource --raw --name=IconSeqStep $< > $@
../icons/IconSeqInitStep.h : ../icons/IconSeqInitStep.ico
	gdk-pixbuf-csource --raw --name=IconSeqInitStep $< > $@
../icons/IconSeqTransi.h : ../icons/IconSeqTransi.ico
	gdk-pixbuf-csource --raw --name=IconSeqTransi $< > $@
../icons/IconSeqStepAndTransi.h : ../icons/IconSeqStepAndTransi.ico
	gdk-pixbuf-csource --raw --name=IconSeqStepAndTransi $< > $@
../icons/IconSeqTransisOr1.h : ../icons/IconSeqTransisOr1.ico
	gdk-pixbuf-csource --raw --name=IconSeqTransisOr1 $< > $@
../icons/IconSeqTransisOr2.h : ../icons/IconSeqTransisOr2.ico
	gdk-pixbuf-csource --raw --name=IconSeqTransisOr2 $< > $@
../icons/IconSeqStepsAnd1.h : ../icons/IconSeqStepsAnd1.ico
	gdk-pixbuf-csource --raw --name=IconSeqStepsAnd1 $< > $@
../icons/IconSeqStepsAnd2.h : ../icons/IconSeqStepsAnd2.ico
	gdk-pixbuf-csource --raw --name=IconSeqStepsAnd2 $< > $@
../icons/IconSeqLink.h : ../icons/IconSeqLink.ico
	gdk-pixbuf-csource --raw --name=IconSeqLink $< > $@
../icons/IconSeqComment.h : ../icons/IconSeqComment.ico
	gdk-pixbuf-csource --raw --name=IconSeqComment $< > $@


PHONY: all install clean dist

install: all
	install -d ${DESTDIR}${BINDIR}
	install -d ${DESTDIR}${INCLUDEDIR}
	install -d ${DESTDIR}${LIBDIR}
	install classicladder ${DESTDIR}${BINDIR}
#	install ladderlib.h ${DESTDIR}${INCLUDEDIR}
#	install libladder.a ${DESTDIR}${LIBDIR}


clean:
	$(RM) -f *.o
	$(RM) -f ../wiringSam/*.o
	$(RM) -f classicladder_rtl_support
	$(RM) -f classicladder-mat
#	$(RM) -f classicladder
	$(RM) -f ladderlib.h
	$(RM) -f libladder.a
	$(RM) -f ../classicladder.exe
	$(RM) -f ../classicladder_embedded.tar.gz
	$(RM) -f ../serial_test
	$(RM) -f ../time_test
	$(RM) -f ../classiclauncher
	$(RM) -f ../icons/*.h

dist: clean
	(cd ../..;rm -f classicladder.tar.gz;tar -cvzf classicladder.tar.gz classicladder/*)
dist_zip: clean
	(cd ../..;rm -f classicladder.zip;zip -r classicladder.zip classicladder/*)

dist_embedded:
	rm -f ../classicladder_embedded.tar.gz
	(cd ..;tar -cf classicladder_embedded.tar classicladder classiclauncher classicladder_run.script classicladder_update.script classicladder_update_next.script)
	gzip ../classicladder_embedded.tar
