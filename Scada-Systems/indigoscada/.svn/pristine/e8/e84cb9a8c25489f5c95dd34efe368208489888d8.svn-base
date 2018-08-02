# ========================================================================
# * Copyright (c) 2005-2016 The OPC Foundation, Inc. All rights reserved.
# *
# * OPC Foundation MIT License 1.00
# * 
# * Permission is hereby granted, free of charge, to any person
# * obtaining a copy of this software and associated documentation
# * files (the "Software"), to deal in the Software without
# * restriction, including without limitation the rights to use,
# * copy, modify, merge, publish, distribute, sublicense, and/or sell
# * copies of the Software, and to permit persons to whom the
# * Software is furnished to do so, subject to the following
# * conditions:
# * 
# * The above copyright notice and this permission notice shall be
# * included in all copies or substantial portions of the Software.
# * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
# * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
# * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
# * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# * OTHER DEALINGS IN THE SOFTWARE.
# *
# * The complete license agreement can be found here:
# * http://opcfoundation.org/License/MIT/1.00/
#=======================================================================
ROOT = ..
ODIR = win32
OPENSSLDIR = $(ROOT)\openssl
TARGET_DIR = $(ROOT)\bin\$(ODIR)

TARGET = $(TARGET_DIR)\AnsiCServer.exe

OPENSSLINC = $(OPENSSLDIR)\include

CFLAGS = /MT /Ox /W3 /Gs0 /GF /Gy /nologo /Zi /Fd$(TARGET).pdb \
         /I$(ROOT)\Stack\core /I$(ROOT)\Stack\stackcore /I$(ROOT)\Stack\securechannel \
         /I$(ROOT)\Stack\proxystub\clientproxy /I$(ROOT)\Stack\proxystub\serverstub /I$(ROOT)\Stack\platforms\win32 /I$(OPENSSLINC)

LIBS = $(ROOT)\lib\$(ODIR)\uastack.lib $(OPENSSLDIR)\lib\ssleay32.lib $(OPENSSLDIR)\lib\libeay32.lib \
       rpcrt4.lib ws2_32.lib gdi32.lib advapi32.lib crypt32.lib user32.lib

OBJECTS = \
	$(ODIR)\ansicservermain.obj \
	$(ODIR)\browsenext.obj \
	$(ODIR)\browseservice.obj \
	$(ODIR)\init_variables_of_addressspace.obj \
	$(ODIR)\readservice.obj \

all: $(TARGET)

clean:
	-del $(TARGET) $(TARGET).pdb 2>nul
	-del $(OBJECTS) 2>nul
	-rmdir $(ODIR) 2>nul

$(TARGET_DIR):
	-mkdir $(ROOT)\bin
	mkdir $(ROOT)\bin\$(ODIR)

$(ODIR):
	mkdir $(ODIR)

$(TARGET): $(TARGET_DIR) $(ODIR) $(OBJECTS)
	link /nologo /subsystem:console /opt:ref /debug /out:$(TARGET) $(OBJECTS) $(LIBS)

{}.c{$(ODIR)}.obj:
	$(CC) $(CFLAGS) /Fo$@ /c $<
