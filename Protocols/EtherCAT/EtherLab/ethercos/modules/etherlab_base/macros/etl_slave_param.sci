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

function [selslave,selectid,MID,DID,SLA,SLP,SLST,configs] = etl_slave_param(dialogname,slavetype,selslave,selectid,MID,DID,SLA,SLP,SLST,configs)
  
repeat = %t;

baseinit(1).value = selslave;
baseinit(1).name = 'Slave Name';
baseinit(2).value = selectid;        //selectid
baseinit(2).name = 'Slave ID';
baseinit(3).value = MID;             //MID
baseinit(3).name = 'Master ID';
baseinit(4).value = DID;             //DID  
baseinit(4).name = 'Domain ID';
baseinit(5).value = SLA;             //SLA
baseinit(5).name = 'Slave Alias';
baseinit(6).value = SLP;             //SLP
baseinit(6).name = 'Slave Position';
baseinit(7).value = SLST;             //Showstate
baseinit(7).name = 'Show State';

while repeat == %t do
  [repeat, baseinit, configs]=etl_build_tcl_interface(dialogname,slavetype,baseinit,configs);
end

selslave = baseinit(1).value;
selectid = baseinit(2).value;        //selectid
MID = baseinit(3).value;             //MID
DID = baseinit(4).value;             //DID  
SLA = baseinit(5).value;             //SLA
SLP = baseinit(6).value;             //SLP
SLST = baseinit(7).value;             //Showstate

  
  
  
endfunction
