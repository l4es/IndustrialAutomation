#! gmake

#
# Copyright (C) 2018 Lars Elgtvedt Susaas
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

CAN_DRIVER = can_peak_linux
PEAK_API_FILENAME = peak-linux-driver.1.37.tar.gz
PRE_COMPILE_TARGETS = ${DRIVER_API}/lib/libpcan.h
DRIVER_INCLUDE = -I./${DRIVER_API}/lib/ -I./${DRIVER_API}/driver/
driverCleanup=yes
DRIVER_APID=$(subst .tar.gz,,${PEAK_API_FILENAME})

#TODO: A little to hacky
DRIVER_API=$(subst .1,-1,${DRIVER_APID})



include ../commonDriver

${DRIVER_API}/lib/libpcan.h:
	wget -nc https://www.peak-system.com/fileadmin/media/linux/files/${PEAK_API_FILENAME}
	tar -xzf ${PEAK_API_FILENAME}

driverCleanup:
	rm -f $(PEAK_API_FILENAME)
	rm -f -r ${DRIVER_API}
