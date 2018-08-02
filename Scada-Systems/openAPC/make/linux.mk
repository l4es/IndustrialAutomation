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

CCOMPILER=g++ -std=c++11 -fno-exceptions -fvisibility=hidden -Wall -Wno-unused-local-typedefs -pedantic -Wno-unused-result $(DBGFLAGS) -D_REENTRANT -DENV_LINUX

SYSINCLUDES= -I.  -I../BeamSDK/include/ -Iliboapc/
	
SYSLIBRARIES= -loapc
	
LINK=g++
	
default: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(LINK) -o $(EXECUTABLE) $(OBJECTS)
	-Wl,-whole-archive \
	$(SYSLIBRARIES) $(CUSTLIBRARIES) \
	-Wl,-no-whole-archive
	$(STRIP) $(EXECUTABLE)

%.o: %.cpp
	$(CCOMPILER) $(SYSINCLUDES) $(CUSTINCLUDES) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
