PROGRAM = luaPLC

INCLUDEDIRS = -Wall $(DBGFLAGS) -DENV_LINUX -D_REENTRANT -L/usr/lib/ -L/usr/lib/i386-linux-gnu/ -L/usr/lib/x86_64-linux-gnu/ -L.. -I .. -I . -I../../plugins -I../../liboapc/ -I/usr/include/lua5.2/

LIBDIRS = -L/usr/lib -L/usr/lib/i386-linux-gnu

LIBS = -loapc -llua5.2 -lreadline -lrt

CXXSOURCES = lua.cpp ispace.cpp

DBGFLAGS = -O2 -g0 -DNDEBUG
ifeq ($(DEBUG),1)
DBGFLAGS = -O0 -g3 -D_DEBUG
endif
ifeq ($(PROFILE),1)
DBGFLAGS = -O2 -g3
endif

CXXOBJECTS = $(CXXSOURCES:.cpp=.o)
CXXFLAGS = -Wall -DESRI_UNIX $(INCLUDEDIRS) -DENV_ISERVER -DENV_LINUX -DEXLUDE_WX -DLUA_COMPAT_5_1 -DLUA_COMPAT_ALL $(DBGFLAGS)
CXX = g++

LDFLAGS = $(LIBDIRS) $(LIBS)

all: $(PROGRAM)

$(PROGRAM): $(CXXOBJECTS)
	$(CXX) -o $@ $(CXXOBJECTS) $(LDFLAGS)
	sudo cp $(PROGRAM) /usr/bin/

clean:
	$(RM) -f $(CXXOBJECTS) $(PROGRAM)

run:
	./$(PROGRAM)


