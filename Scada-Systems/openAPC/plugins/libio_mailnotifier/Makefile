CUSTLIBRARIES= $(shell wx-config --libs)

CUSTINCLUDES= -IwxSMTP/include $(shell wx-config --cflags) -DWXUSINGDLL -Wno-long-long -Wno-variadic-macros
	
CUSTOBJECTS= wxSMTP/src/wxcmdprot.o wxSMTP/src/wxemail.o wxSMTP/src/wxsmtp.o wxSMTP/src/wxmime.o wxSMTP/src/wxstates.o

include ../linux.mk



