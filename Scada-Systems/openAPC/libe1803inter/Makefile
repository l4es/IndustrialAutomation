DBGFLAGS = -O2 -g0 -DNDEBUG
ifeq ($(DEBUG),1)
DBGFLAGS = -O0 -g3 -DNOSPLASH -D_DEBUG
endif
ifeq ($(PROFILE),1)
DBGFLAGS = -O2 -g3
endif

CFLAGS = -DCNCO_NO_WX -DENV_LINUX -I../liboapc/ -I../../CNConstruct/libcnco/ -I../libe1803/
LFLAGS = -Wl,-whole-archive \
	     -lcnco -loapc -le1803 \
         -Wl,-no-whole-archive

CCOMPILER=libtool --mode=compile g++ -Wall -Wno-unused -fPIC -shared -fvisibility=hidden $(DBGFLAGS) -DE1803_EXPORTS -D_REENTRANT -DENV_LINUX -I.  -I.. -I ../../OpenAPC/plugins/ -I ../../OpenIPC/plugins/ $(CFLAGS) $(LFLAGS)

SYSLIBRARIES=

LIBS = -ldl -lrt \
		-Wl,-whole-archive \
		-le1803 \
		-Wl,-no-whole-archive

EXECUTABLE=libe1803inter.la

LINK=libtool --mode=link g++ -rpath /usr/lib
# -fvisibility=hidden -shared -export-symbols-regex 'E1803_' -rpath /usr/lib

OBJECTS = libe1803_e1701.lo libe1803_olsc.lo libe1803_ezaudac.lo libe1803_easylase.lo libe1803_etherdream.lo libe1803_rtc4.lo libe1803_rtc4_empty.lo libe1803_rtc4_wrapped.lo

default: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(LINK) $(SYSLDFLAGS) $(LDFLAGS) -o $(EXECUTABLE) $(OBJECTS) -Wl,-whole-archive $(LIBS) -Wl,-no-whole-archive
	cp -ax .libs/libe1803inter.so* ../
	sudo cp -ax .libs/libe1803inter.so* /usr/lib/ &
	sudo cp -ax .libs/libe1803inter.so* /usr/lib64/

%.lo: %.cpp
	$(CCOMPILER) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)


