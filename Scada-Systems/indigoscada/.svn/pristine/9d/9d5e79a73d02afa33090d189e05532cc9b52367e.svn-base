# -*- makefile -*-
# Makefile for Unix with gcc compiler

CC=gcc
CCPP=g++
# if you use egcs-2.90.* version of GCC please add option -fno-exceptions 
# to reduce code size and increase performance

# Debug version
CFLAGS = -c -Wall -O0 -g 

# Optimized version
#CFLAGS = -c -Wall -O6 -g 

# Optimized version with switched off asserts
#CFLAGS = -c -Wall -O6 -g -DNDEBUG

LFLAGS=-g

all: tstshmem fifo semstat

shmem.o: shmem.cpp shmem.h
	$(CCPP) $(CFLAGS) shmem.cpp

shmemc.o: shmemc.cpp shmem.h shmemc.h
	$(CC) $(CFLAGS) shmemc.cpp

tstshmem.o: tstshmem.cpp shmem.h semp.h
	$(CC) $(CFLAGS) tstshmem.cpp

fifo.o: fifo.cpp shmem.h semp.h
	$(CC) $(CFLAGS) fifo.cpp

semp.o: semp.c semp.h
	$(CC) $(CFLAGS) semp.c

semstat.o: semstat.c
	$(CC) $(CFLAGS) semstat.c

tstshmem: tstshmem.o shmem.o semp.o
	$(CCPP) $(LFLAGS) -o tstshmem tstshmem.o shmem.o semp.o

fifo: fifo.o shmem.o semp.o
	$(CCPP) $(LFLAGS) -o fifo fifo.o shmem.o semp.o

semstat: semstat.o
	$(CC) $(LFLAGS) -o semstat semstat.c

clean: 
	rm -f  *.o *.odb *.mon core *~ tstshmem semstat fifo


tgz: clean
	cd ..; tar cvzf shmem.tgz shmem

copytgz: tgz
	mcopy -o ../shmem.tgz a:
