DBGFLAGS = -O2 -g0 -DNDEBUG
ifeq ($(DEBUG),1)
DBGFLAGS = -O0 -g3
endif

CCOMPILER=g++ -Wall -fPIC -shared -Wno-unused $(DBGFLAGS) -D_REENTRANT -DENV_LINUX -I. -I../../plugins -I../../liboapc

SYSLIBRARIES=

LIBS = $(SYSLIBRARIES)

EXECUTABLE=libjwarrior.so

LINK=g++ -shared

OBJECTS = ../../plugins/libio_jwarrior/libio_jwarrior.o libjwarrior.o

default: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(LINK) $(SYSLDFLAGS) $(LDFLAGS) -o $(EXECUTABLE) $(OBJECTS) $(LIBS) 

%.o: %.cpp
	$(CCOMPILER) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

