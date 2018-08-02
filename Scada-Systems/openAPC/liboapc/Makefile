DBGFLAGS = -O2 -g0 -DNDEBUG
ifeq ($(DEBUG),1)
DBGFLAGS = -O0 -g3 -DNOSPLASH -D_DEBUG
endif
ifeq ($(PROFILE),1)
DBGFLAGS = -O2 -g3
endif


ifeq ($(EXCLUDE_WX),1)
 CFLAGS = -DEXCLUDE_WX
 LFLAGS = 
else
 CFLAGS = $(shell wx-config --cflags)
 LFLAGS = $(SYSLIBRARIES) $(shell wx-config --libs)
endif

CCOMPILER=libtool --mode=compile g++ -Wall -Wno-unused -fPIC -shared -fvisibility=hidden $(DBGFLAGS) -DOAPC_EXT_EXPORTS -D_REENTRANT -DENV_LINUX -I.  -I.. -I ../plugins/ $(CFLAGS)

SYSLIBRARIES=

LIBS = -ldl -lpthread -lrt

EXECUTABLE=liboapc.la

LINK=libtool --mode=link g++ -fvisibility=hidden -shared -export-symbols-regex 'oapc_' -rpath /usr/lib

OBJECTS = liboapcInc.lo

default: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(LINK) $(SYSLDFLAGS) $(LDFLAGS) -o $(EXECUTABLE) $(OBJECTS) -Wl,-whole-archive $(LIBS) -Wl,-no-whole-archive
	cp -ax .libs/liboapc.so* ../
	sudo cp -ax .libs/liboapc.so* /usr/lib/ &
	sudo cp -ax .libs/liboapc.so* /usr/lib64/

%.lo: %.cpp
	$(CCOMPILER) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)


