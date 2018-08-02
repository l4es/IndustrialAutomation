PWD=$(shell pwd)
DIR=$(shell basename $(PWD))
OBJECT=$(DIR).o
ifeq ($(strip $(EXECUTABLE)),)
 EXECUTABLE=../../hmiplugins/$(DIR).so
endif

DBGFLAGS = -O2 -g0 -DNDEBUG
ifeq ($(DEBUG),1)
 DBGFLAGS = -O0 -g3 -D_DEBUG
 STRIP=pwd
endif
ifeq ($(PROFILE),1)
 DBGFLAGS = -O2 -g3
endif

ifeq ($(strip $(STRIP)),)
 STRIP=pwd
endif

CCOMPILER=g++ -std=c++11 -fno-exceptions -fvisibility=hidden -Wall -fPIC -DPIC -Wno-unused-local-typedefs -Wno-unused-result -Wno-variadic-macros -Wno-long-long \
          $(DBGFLAGS) -D_REENTRANT -DENV_LINUX -DOAPC_EXT_HMI_EXPORTS -DOAPC_EXT_EXPORTS

SYSINCLUDES= -I.  -I.. -I../../liboapc/ $(shell wx-config --cflags)
	
SYSLIBRARIES= -loapc $(shell wx-config --libs)
	
LINK=g++ -shared
	
default: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECT) $(CUSTOBJECTS)
	$(LINK) -o $(EXECUTABLE) $(OBJECT) $(CUSTOBJECTS) \
	-Wl,-whole-archive \
	$(SYSLIBRARIES) $(CUSTLIBRARIES) \
	-Wl,-no-whole-archive
	$(STRIP)
	sudo cp $(EXECUTABLE) /usr/lib/openapc/hmiplugins/ &
	sudo cp $(EXECUTABLE) /usr/lib64/openapc/hmiplugins/ &

%.o: %.cpp
	$(CCOMPILER) $(SYSINCLUDES) $(CUSTINCLUDES) -c $< -o $@

clean:
	rm -f $(OBJECT) $(CUSTOBJECTS) $(EXECUTABLE)
