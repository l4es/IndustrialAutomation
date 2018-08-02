DBGFLAGS = -O2 -g0 -DNDEBUG
STRIP=strip
ifeq ($(DEBUG),1)
 DBGFLAGS = -O0 -g3
 #-D_DEBUG
 STRIP=ls -al
endif
ifeq ($(PROFILE),1)
 DBGFLAGS = -O2 -g3
 STRIP=ls -al
endif

CCOMPILER=g++ -std=c++11 -fno-exceptions -fvisibility=hidden -Wall -Wno-unused-local-typedefs -Wno-unused-result -Wno-long-long -Wno-variadic-macros $(DBGFLAGS) -D_REENTRANT -DENV_LINUX 

SYSINCLUDES= -I. -I../BeamSDK/include/ -IBeamSDK/include/ -Iliboapc/ -Iplugins/ -I../../liboapc/ -I../../plugins/ $(shell wx-config --cflags)
	
SYSLIBRARIES= -loapc -loapcwx $(shell wx-config --libs)
	
LINK=g++
	
default: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(LINK) -o $(EXECUTABLE) $(OBJECTS) \
	-Wl,-whole-archive \
	$(SYSLIBRARIES) $(CUSTLIBRARIES) \
	-Wl,-no-whole-archive

%.o: %.cpp
	$(CCOMPILER) $(SYSINCLUDES) $(CUSTINCLUDES) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
