DBGFLAGS = -O2 -g0 -DNDEBUG
ifeq ($(DEBUG),1)
DBGFLAGS = -O0 -g3 -DNOSPLASH -D_DEBUG
endif
ifeq ($(PROFILE),1)
DBGFLAGS = -O2 -g3
endif

CFLAGS = -DCNCO_NO_WX -DENV_LINUX -I../liboapc/ -I../../CNConstruct/libcnco/ -I../libe1701/
LFLAGS = -Wl,-whole-archive \
	     -lcnco -loapc -le1701 \
         -Wl,-no-whole-archive

CCOMPILER=libtool --mode=compile g++ -Wall -Wno-unused -fPIC -shared -fvisibility=hidden $(DBGFLAGS) -DE1701_EXPORTS -D_REENTRANT -DENV_LINUX -I.  -I.. -I ../../OpenAPC/plugins/ -I ../../OpenIPC/plugins/ $(CFLAGS) $(LFLAGS)

SYSLIBRARIES=

LIBS = -ldl -lrt \
		-Wl,-whole-archive \
		-le1701 \
		-Wl,-no-whole-archive

EXECUTABLE=libe1701inter.la

LINK=libtool --mode=link g++ -rpath /usr/lib
# -fvisibility=hidden -shared -export-symbols-regex 'E1701_' -rpath /usr/lib

OBJECTS = libe1701_olsc.lo libe1701_ezaudac.lo libe1701_easylase.lo libe1701_etherdream.lo libe1701_rtc4.lo libe1701_rtc4_empty.lo libe1701_rtc4_wrapped.lo

default: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(LINK) $(SYSLDFLAGS) $(LDFLAGS) -o $(EXECUTABLE) $(OBJECTS) -Wl,-whole-archive $(LIBS) -Wl,-no-whole-archive
	cp -ax .libs/libe1701inter.so* ../
	sudo cp -ax .libs/libe1701inter.so* /usr/lib/ &
	sudo cp -ax .libs/libe1701inter.so* /usr/lib64/

%.lo: %.cpp
	$(CCOMPILER) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)


