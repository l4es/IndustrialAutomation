CC      = gcc
CFLAGS  = -I ../../liboapc -I ../../plugins -Wall -g0 -O2 -DENV_LINUX -D_REENTRANT -D_DEBUG -DOAPC_EXT_API="" -fno-strict-aliasing -fPIC

LDFLAGS  = -lpthread -loapc  $(shell wx-config --libs)

PNAME=iswatcher

OBJ = iswatcher.o
SRC = iswatcher.c

all: $(OBJ)
	$(CC) $(CFLAGS) -o $(PNAME) $(OBJ) $(LDFLAGS)

clean:
	rm *.o
	rm $(PNAME)
