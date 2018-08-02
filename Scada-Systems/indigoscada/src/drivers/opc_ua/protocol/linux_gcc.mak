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
# generate Stack and Test Server

# parameters:
#	BUILD_TARGET=debug|release
#	MACHINE_TYPE=x86_64|i386
#	MACHINE_OPT=

all:
	$(MAKE) -C Stack -f linux_gcc.mak all
	$(MAKE) -C AnsiCSample -f linux_gcc.mak all
	
clean:
	$(MAKE) -C Stack -f linux_gcc.mak clean
	$(MAKE) -C AnsiCSample -f linux_gcc.mak clean

strip:
	$(MAKE) -C Stack -f linux_gcc.mak strip
	$(MAKE) -C AnsiCSample -f linux_gcc.mak strip
