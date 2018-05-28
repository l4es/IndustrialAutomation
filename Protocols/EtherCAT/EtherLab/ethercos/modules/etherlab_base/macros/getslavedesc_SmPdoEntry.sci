// Copyright (C) 2008  Andreas Stewering
//
// This file is part of Etherlab.
//
// Etherlab is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// Etherlab is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Etherlab; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA//
// ====================================================================

function [index,subindex,bitlen,datatype] = getslavedesc_SmPdoEntry(slave_desc,deviceindex,direction,pdoindex,entryindex)
    index = 0;
    subindex = 0;
    bitlen = 0;
    datatype = 0;
    if direction == 1 //TxPdo, Slave send to Master
      pdo = slave_desc.Descriptions.Devices.Device(deviceindex).TxPdo(pdoindex);
    end
    if direction == 2 //RxPdo, Master send to Slave
      pdo = slave_desc.Descriptions.Devices.Device(deviceindex).RxPdo(pdoindex);
    end
    index = pdo.Entry(entryindex).Index;
    subindex = pdo.Entry(entryindex).SubIndex;
    bitlen = pdo.Entry(entryindex).BitLen;
    datatype = pdo.Entry(entryindex).DataType;
endfunction
