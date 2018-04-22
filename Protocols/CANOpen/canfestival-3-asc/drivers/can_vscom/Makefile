#! gmake

#
# Copyright (C) 2006 Laurent Bessard
# 
# This file is part of canfestival, a library implementing the canopen
# stack
# 
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
# 
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
# 

CAN_DRIVER = can_vscom
VSCAN_API_FILENAME=VSCAN_API_latest.zip
PRE_COMPILE_TARGETS = VSCAN/vs_can_api.h
DRIVER_INCLUDE = -I./VSCAN
driverCleanup=yes
include ../commonDriver

VSCAN/vs_can_api.h:
	wget http://www.vscom.de/download/multiio/linux/driver/${VSCAN_API_FILENAME}
	@if which unzip &> /dev/null; then \
	unzip -o ${VSCAN_API_FILENAME} -d VSCAN; \
	else \
	echo "No unzip found. Please decompress ${VSCAN_API_FILENAME} manually"; \
	exit 1; \
	fi;

driverCleanup:
	rm -f ${VSCAN_API_FILENAME}
	rm -f -r VSCAN
