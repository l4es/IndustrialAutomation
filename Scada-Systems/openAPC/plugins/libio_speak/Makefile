PREFIX=/usr
BINDIR=$(PREFIX)/bin
LIBDIR=$(PREFIX)/lib
DATADIR=$(PREFIX)/share/openapc/espeak-data

AUDIO = portaudio

#AUDIO = pulseaudio
#AUDIO = sada

ifeq ($(AUDIO),pulseaudio)
WAVE=wave_pulse.cpp
LIB_AUDIO=-lpulse
USE_AUDIO=-DUSE_PULSEAUDIO
else
ifeq ($(AUDIO),portaudio)
WAVE=wave.cpp
LIB_AUDIO=-lportaudio
USE_AUDIO=-DUSE_PORTAUDIO
else
ifeq ($(AUDIO),sada)
WAVE=wave_sada.cpp
USE_AUDIO=-DUSE_SADA
else
WAVE=wave.cpp
endif
endif
endif

CCOMPILER=g++ -fno-exceptions -Wall -pedantic -fPIC -shared -Wno-unused-result -g0 -O2 $(USE_AUDIO) -D_REENTRANT -DENV_LINUX -DPATH_ESPEAK_DATA=\"$(DATADIR)\" -I. -I.. -I../../liboapc/ -I/usr/include -I../plugins

SYSLIBRARIES=

LIBS = $(SYSLIBRARIES)

EXECUTABLE=../../flowplugins/libio_speak.so

# want maybe to debug
LINK=g++ -shared $(LIB_AUDIO)

OBJECTS = speak.o compiledict.o dictionary.o intonation.o readclause.o setlengths.o numbers.o synth_mbrola.o \
	      synthdata.o synthesize.o translate.o tr_languages.o voices.o wavegen.o phonemelist.o klatt.o \
	      libio_speak.o

default: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(LINK) $(SYSLDFLAGS) $(LDFLAGS) -o $(EXECUTABLE) $(OBJECTS) $(LIBS) 
	sudo cp $(EXECUTABLE) /usr/lib/openapc/flowplugins/


%.o: %.cpp
	$(CCOMPILER) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)



