DBGFLAGS = -O2 -g0 -DNDEBUG
ifeq ($(DEBUG),1)
 DBGFLAGS = -O0 -g3
 #-D_DEBUG
endif
ifeq ($(PROFILE),1)
 DBGFLAGS = -O2 -g3
endif

PWD=$(shell pwd)
DIR=$(shell basename $(PWD))
OBJECT=$(DIR).o
EXECUTABLE=../../flowplugins/$(DIR).so

CCOMPILER=qcc -fno-exceptions -Wall -pedantic -fPIC -DPIC -Wno-unused-result $(DBGFLAGS) -D_REENTRANT -DENV_QNX -DOAPC_EXT_EXPORTS

SYSINCLUDES= -I.  -I.. -I../../liboapc/
	
SYSLIBRARIES= -loapc
	
LINK=qcc -shared
	
default: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECT) $(CUSTOBJECTS)
	$(LINK) -o $(EXECUTABLE) $(OBJECT) $(CUSTOBJECTS) $(SYSLIBRARIES) $(CUSTLIBRARIES)

%.o: %.cpp
	$(CCOMPILER) $(SYSINCLUDES) $(CUSTINCLUDES) -c $< -o $@

clean:
	rm -f $(OBJECT) $(CUSTOBJECTS) $(EXECUTABLE)
