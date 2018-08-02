# Copyright (c) 1996-2017, OPC Foundation. All rights reserved.
#
#   The source code in this file is covered under a dual-license scenario:
#     - RCL: for OPC Foundation members in good-standing
#     - GPL V2: everybody else
#
#   RCL license terms accompanied with this source code. See http://opcfoundation.org/License/RCL/1.00/
#
#   GNU General Public License as published by the Free Software Foundation;
#   version 2 of the License are accompanied with this source code. See http://opcfoundation.org/License/GPLv2
#
#   This source code is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
ROOT = ..

include $(ROOT)/linux_gcc_rules.mak

TARGET = $(ROOT)/lib/$(BIN_PATH)/$(CC)/$(BUILD_TARGET)/libuastack.a

DIRS = core stackcore securechannel transport/tcp transport/https \
       proxystub/clientproxy proxystub/serverstub platforms/linux

CFLAGS = -Wall -Wextra -Wshadow -Wundef -pthread \
         $(patsubst %,-I%,$(DIRS)) $(EXTRA_CFLAGS)

SOURCES = $(wildcard $(patsubst %,%/*.c,$(DIRS)))

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

$(TARGET): $(OBJECTS)
	$(MKDIR) $(dir $@)
	$(AR) rcs $@ $^

./$(BIN_PATH)/$(CC)/$(BUILD_TARGET)/%.o : %.c
	$(MKDIR) $(dir $@)
	$(CC) -c $(CFLAGS) -MMD -MP -MT $@ -MF $(@:%.o=%.d) -o $@ $<
