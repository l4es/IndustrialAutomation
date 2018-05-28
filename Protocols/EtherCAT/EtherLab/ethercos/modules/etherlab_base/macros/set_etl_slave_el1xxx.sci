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

function [repeat_dialog,selslave,selectid,MID,DID,SLA,SLP] = set_etl_slave_el1xxx(selslave,selectid,MID,DID,SLA,SLP)
  check_values = 0;
  fd = mopen([getenv('ETLPATH')+'/modules/etherlab_base/src/tcl/'+'etl_scicos_el1xxx.tcl'],'r');
  txt = mgetl(fd,-1);
  mclose(fd);
  
  initvalues = ['array set slavestring { 0 EL1002V1 1 EL1002V2 2 EL1004V1 3 EL1004V2 4 EL1008 5 EL1012V1 6 EL1012V2 7 EL1014V1 8 EL1014V2 9 EL1018 10 EL1024 11 EL1034}' 
		'set slaveselected '+selslave;
		'set selectid '+string(selectid);
		'set masterid '+string(MID);
		'set domainid '+string(DID);
		'set slavealias '+string(SLA);
		'set slaveposition '+string(SLP);
		];
   listentry = ['.eltop.list insert end $slavestring(0)';
		'.eltop.list insert end $slavestring(1)';
		'.eltop.list insert end $slavestring(2)';
		'.eltop.list insert end $slavestring(3)';
		'.eltop.list insert end $slavestring(4)';
		'.eltop.list insert end $slavestring(5)';
		'.eltop.list insert end $slavestring(6)';
		'.eltop.list insert end $slavestring(7)';
		'.eltop.list insert end $slavestring(8)';
		'.eltop.list insert end $slavestring(9)';
		'.eltop.list insert end $slavestring(10)';
		'.eltop.list insert end $slavestring(11)';
		'.eltop.list see $selectid';
		'.eltop.list selection set $selectid';
	];
   tclscript = [txt;
		initvalues;
	        listentry
	];
  // fd2 = mopen('/home/ab/test.tcl','w')
  //mputl(tclscript,fd2);
  //mclose(fd2)
  TCL_EvalStr(tclscript); //  call TCL interpretor to create widget	

  while %t do
    ok = TCL_GetVar('okstate');
    cancel = TCL_GetVar('cancelstate');
    if evstr(ok) == 1 then 
      break
    end
    if evstr(cancel) == 1 then
      break
    end
   sleep(100);
  end
  repeat_dialog = %f;
  selslave = TCL_GetVar('slaveselected')
  selectid = evstr(TCL_GetVar('selectid'))
  MID = check_pos_integer('Master ID','masterid');
  if MID<0  then
    repeat_dialog = %t;
  end
  //disp(MID);
  DID = check_pos_integer('Domain ID','domainid');
  if DID<0  then
   repeat_dialog = %t;
  end
  SLA = check_pos_integer('Slave Alias','slavealias');
  if SLA<0  then
    repeat_dialog = %t;
  end
  SLP = check_pos_integer('Slave Position','slaveposition');
  if SLA<0  then
    repeat_dialog = %t;
  end


  TCL_EvalStr('destroy $eltop')
 endfunction
