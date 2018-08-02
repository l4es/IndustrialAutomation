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

include $(ROOT)/linux_gcc_rules.mak

LIB_PATH = $(ROOT)/lib/$(BIN_PATH)/$(CC)/$(BUILD_TARGET)

UA_LIBS = uastack
LIB_EXTENSION = a

LIB_FILES = $(patsubst %,$(LIB_PATH)/lib%.$(LIB_EXTENSION),$(UA_LIBS))
LIB_FLAGS = $(patsubst %,-l%,$(UA_LIBS))

TARGET = $(ROOT)/bin/$(BIN_PATH)/$(CC)/$(BUILD_TARGET)/AnsiCServer

STACK_DIRS = core stackcore securechannel \
       proxystub/clientproxy proxystub/serverstub platforms/linux

INCLUDE_DIRS = $(patsubst %,../Stack/%,$(STACK_DIRS))

CFLAGS = -Wall -pthread \
         $(patsubst %,-I%,$(INCLUDE_DIRS)) $(EXTRA_CFLAGS)

SOURCES = $(wildcard *.c)

OBJECTS = $(SOURCES:%.c=./$(BIN_PATH)/$(CC)/$(BUILD_TARGET)/%.o)

DEPS = $(OBJECTS:%.o=%.d)

all: $(TARGET)

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
$(OBJECTS): linux_gcc.mak $(ROOT)/linux_gcc_rules.mak
endif

strip:
	$(STRIP) -g $(TARGET)

clean:
	$(RM) $(OBJECTS)
	$(RM) $(DEPS)
	$(RM) $(TARGET)

$(TARGET): $(OBJECTS) $(LIB_FILES)
	$(MKDIR) $(dir $@)
	$(CC) $(EXTRA_CFLAGS) -o $@ $(OBJECTS) -L$(LIB_PATH) $(LIB_FLAGS) -lssl -lcrypto -lpthread -lrt -lm -ldl

./$(BIN_PATH)/$(CC)/$(BUILD_TARGET)/%.o : %.c
	$(MKDIR) $(dir $@)
	$(CC) -c $(CFLAGS) -MMD -MP -MT $@ -MF $(@:%.o=%.d) -o $@ $<
