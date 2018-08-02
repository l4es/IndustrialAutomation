PROGRAM = OpenIServer

INCLUDEDIRS = -I.. -I../../plugins/ -I../../liboapc/


LIBDIRS = -L/usr/lib

LIBS = -loapc -lrt

CXXSOURCES = OpenIServerInc.cpp

DBGFLAGS = -O2 -g0 -DNDEBUG
ifeq ($(DEBUG),1)
DBGFLAGS = -O0 -g3 -D_DEBUG
endif
ifeq ($(PROFILE),1)
DBGFLAGS = -O2 -g3
endif

CXXOBJECTS = $(CXXSOURCES:.cpp=.o)
CXXFLAGS = -Wall -DESRI_UNIX $(INCLUDEDIRS) -DENV_ISERVER -DENV_LINUX -DEXLUDE_WX $(DBGFLAGS)
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


