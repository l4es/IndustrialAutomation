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
# Global rules for all GCC builds

CC = $(CROSS_COMPILE)gcc
CXX = $(CROSS_COMPILE)g++
AR = $(CROSS_COMPILE)ar
STRIP = $(CROSS_COMPILE)strip
MKDIR = mkdir -p

ifndef MACHINE_TYPE
ifndef CROSS_COMPILE
	TMP_MACHINE_TYPE = $(shell uname -i)
	ifeq ($(TMP_MACHINE_TYPE),x86_64)
		MACHINE_TYPE = x86_64
	else
	ifeq ($(TMP_MACHINE_TYPE),i386)
		MACHINE_TYPE = i386
	else
	ifeq ($(TMP_MACHINE_TYPE),i686)
		MACHINE_TYPE = i386
	else
		MACHINE_TYPE = unknown
	endif
	endif
	endif
else
	MACHINE_TYPE = unknown
endif
endif

ifndef MACHINE_OPT
ifndef CROSS_COMPILE
	ifeq ($(MACHINE_TYPE),x86_64)
		MACHINE_OPT = -m64
	else
	ifeq ($(MACHINE_TYPE),i386)
		MACHINE_OPT = -m32
	endif
	endif
endif
endif

BIN_PATH = linux/$(MACHINE_TYPE)

ifndef BUILD_TARGET
	BUILD_TARGET = release
endif

ifeq ($(findstring debug,$(BUILD_TARGET)),debug)
	EXTRA_CFLAGS = -D_DEBUG -g $(MACHINE_OPT)
else
	EXTRA_CFLAGS = -O3 $(MACHINE_OPT)
endif
