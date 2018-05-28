#-----------------------------------------------------------------------------------
 #  Copyright (C) 2008  Andreas Stewering
 #  Copyright (C) 2008  Holger Nahrstaedt (HART-Project)
 #
 #  This file is part of Etherlab.
 #
 #  Etherlab is free software; you can redistribute it and/or modify
 #  it under the terms of the GNU Lesser General Public License as published by
 #  the Free Software Foundation; either version 2 of the License, or
 #  (at your option) any later version.
 #
 #  Etherlab is distributed in the hope that it will be useful,
 #  but WITHOUT ANY WARRANTY; without even the implied warranty of
 #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 #  GNU General Public License for more details.
 #
 #  You should have received a copy of the GNU Lesser General Public License
 #  along with Etherlab; if not, write to the Free Software
 #  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 #--------------------------------------------------------------------------------- 

SCILAB_VERSION = 4.1.2
SCILAB_DIR = /usr/local/scilab-$(SCILAB_VERSION)
SCILAB = $(SCILAB_DIR)/bin/scilab


MACROSRC = *.bin names lib *.cosf
HELPSRC   = *.htm

MACROS_DIR = $(PWD)
all:
	$(SCILAB)  -nb -nw -f builder.sce;
install:
	echo "This is only a Module for the Etherlab-Toolbox. Please install this Toolbox!"
clean:
	cd macros; rm $(MACROSRC); 
	cd help/en_US; rm $(HELPSRC); 
	cd src; make clean

