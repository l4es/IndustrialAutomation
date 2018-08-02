PROGRAM = gtrace

INCLUDEDIRS = -I ~/OpenIPC -I../../liboapc -I ../../plugins -I../libjwarrior

LIBDIRS = -L/usr/lib

LIBS = -lgps -L../../liboapc/ -loapc -L../libjwarrior/ -ljwarrior

DBGFLAGS = -O2 -g0
ifeq ($(DEBUG),1)
DBGFLAGS = -O0 -g3 -D_DEBUG
endif

CXXSOURCES = gtrace.cpp

CXXOBJECTS = $(CXXSOURCES:.cpp=.o)
CXXFLAGS = -Wall -DESRI_UNIX -DENV_LINUX $(INCLUDEDIRS) $(DBGFLAGS)
CXX = g++

LDFLAGS = $(LIBDIRS) $(LIBS)

all: $(PROGRAM)

$(PROGRAM): $(CXXOBJECTS)
	$(CXX) -o $@ $(CXXOBJECTS) $(LDFLAGS)
	strip $(PROGRAM)

clean:
	$(RM) -f $(CXXOBJECTS) $(PROGRAM)

