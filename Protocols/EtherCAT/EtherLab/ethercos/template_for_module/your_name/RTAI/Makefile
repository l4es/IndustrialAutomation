LIB = libsciblk2.a
all: $(LIB)

RTAIDIR = $(rtai-config --prefix)
RTAIDIR = /usr/realtime/
COMEDIDIR = `rtai-config --comedi-dir`
COMEDIDIR = /usr/local/src/comedi
SCILAB_VERSION = 4.1.2
SCILAB_DIR = /usr/local/scilab-$(SCILAB_VERSION)

SRCALL = $(wildcard *.c *.cpp)

ifeq ($(strip $(COMEDIDIR)),)
EXCLUDE = 
else
EXCLUDE =
endif
SRC=$(filter-out $(EXCLUDE),$(SRCALL))

OBJ = $(SRC:.c=.o) $(SRC:.cpp=.o) 
DBG = -g

CC_FLAGS = -c $(DBG) -D RTAI=1 -I. -I$(RTAIDIR)/include -O2 -fno-stack-protector $(CC_PICFLAGS)  -I../../../includes -L../../../src -lhart

%.o: %.c
	cc $(CC_FLAGS) $<

%.o: %.cpp
	cc $(CC_FLAGS) $<
        

$(LIB): $(OBJ)
	ar -r $(LIB) $(OBJ)

install: $(LIB)
	#cp $(LIB) $(RTAIDIR)/lib
	echo "Install rtai-lib from hart/RTAI!"

clean:
	rm -f $(LIB) $(OBJ)
