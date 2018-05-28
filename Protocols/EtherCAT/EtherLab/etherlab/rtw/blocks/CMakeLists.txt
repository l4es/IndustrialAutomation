#############################################################################
#
#  $Id$
#
#  Copyright 2012 Richard Hacker (lerichi at gmx dot net)
#
#  This file is part of the PdServ-MATLAB package.
#
#  PdServ-MATLAB is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  PdServ-MATLAB is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with PdServ-MATLAB. See COPYING. If not, see
#  <http://www.gnu.org/licenses/>.
#
#############################################################################



INSTALL (FILES
    ethercat_mini.png tools.png 
    ETL.tlc
    findidx.c findidx.tlc 
    event.c event.tlc 
    etherlab_lib.mdl 
    etherlab_help_path.m
    setup.m 
    blockpath.m

    EtherLAB_IO.tlc 
    RtIPC.tlc
    rtipc_tx.c rtipc_tx.tlc 
    rtipc_rx.c rtipc_rx.tlc 
    message.c message.tlc
    world_time.c world_time.tlc
    raise.c raise.tlc RaiseCatch.tlc 
    get_string.h test_var.m

    DESTINATION rtw/blocks)

ADD_SUBDIRECTORY (EtherCAT)

INSTALL (DIRECTORY html
    DESTINATION rtw/blocks
    FILES_MATCHING PATTERN "*.html" PATTERN "*.png" PATTERN "*.css"
)
