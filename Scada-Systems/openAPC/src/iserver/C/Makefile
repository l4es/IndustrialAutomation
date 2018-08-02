CC      = gcc
CFLAGS  = -I .. -I ../../ -Wall -g0 -O2 -DENV_LINUX -D_REENTRANT -D_DEBUG -DOAPC_EXT_API="" -fstack-protector -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -fno-strict-aliasing -fPIC

LDFLAGS  = -lpthread -loapc -lm  $(shell wx-config --libs)

PNAME=iserver_example1

OBJ = iserver_example1.o
SRC = iserver_example1.c

all: $(OBJ)
	$(CC) $(CFLAGS) -o $(PNAME) $(OBJ) $(LDFLAGS)
	strip $(PNAME)

clean:
	rm *.o
	rm $(PNAME)
