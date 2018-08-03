# Makefile for PLCEMU
# (c) 2014

AM_CFLAGS=-Wall -I./src -I./src/vm -I./src/ui -I./src/hw


bin_PROGRAMS=plcemu 

plcemu_SOURCES= src/plcemu.c src/plcemu.h \
                src/config.c src/config.h \
                src/util.c src/util.h \
                src/vm/plclib.c src/vm/plclib.h \
                src/vm/parser-il.c src/vm/parser-il.h \
                src/vm/parser-ld.c src/vm/parser-ld.h \
                src/vm/parser-tree.c src/vm/parser-tree.h \
                src/vm/codegen.c src/vm/codegen.h \
                src/vm/init.c src/vm/init.h \
                src/hw/hardware.h \
                src/ui/ui.h \
                src/project.h src/project.c
#NCURSES GUI
if UI
plcemu_SOURCES+= src/ui/ui-nc.c src/ui/greek.c src/ui/greek.h \
                 src/ui/args.c src/ui/args.h src/ui/wselect.c src/ui/wedit.h
else
plcemu_SOURCES+= src/ui/ui-std.c
endif
#hardware modules supported
if SIMULATION
plcemu_SOURCES+=src/hw/hardware-sim.c
else 
if USPACE
plcemu_SOURCES+=src/hw/hardware-uspace.c
else
plcemu_SOURCES+=src/hw/hardware-comedi.c
endif
endif

nodes:; mknod plcpipe p; mknod plcresponse p; 



