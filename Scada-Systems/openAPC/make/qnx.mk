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

CCOMPILER=qcc -fno-exceptions -Wall -pedantic -Wno-unused-result $(DBGFLAGS) -D_REENTRANT -DENV_QNX -DOAPC_EXT_API

SYSINCLUDES= -I.  -I../BeamSDK/include/ -Iliboapc/
	
SYSLIBRARIES= -loapc
	
LINK=qcc
	
default: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(LINK) -o $(EXECUTABLE) $(OBJECTS) $(SYSLIBRARIES) $(CUSTLIBRARIES)

%.o: %.cpp
	$(CCOMPILER) $(SYSINCLUDES) $(CUSTINCLUDES) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
