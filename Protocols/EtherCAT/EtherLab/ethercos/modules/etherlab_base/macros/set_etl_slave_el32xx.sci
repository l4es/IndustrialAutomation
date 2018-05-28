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

function [repeat_dialog,selslave,selectid,MID,DID,SLA,SLP,SLST,ENFILT,WIRE1,WIRE2,WIRE3,WIRE4,RTD1,RTD2,RTD3,RTD4] = set_etl_slave_el32xx(selslave,selectid,MID,DID,SLA,SLP,SLST,ENFILT,WIRE1,WIRE2,WIRE3,WIRE4,RTD1,RTD2,RTD3,RTD4)
  check_values = 0;
  fd = mopen([getenv('ETLPATH')+'/modules/etherlab_base/src/tcl/'+'etl_scicos_el32xx.tcl'],'r');
  txt = mgetl(fd,-1);
  mclose(fd);

  initvalues = ['array set slavestring { 0 EL3201 1 EL3202 2 EL3204}' 
		'array set slavestate { 0 1 1 1 2 1}'; 
		'set slaveselected '+selslave;
		'set selectid '+string(selectid);
		'set masterid '+string(MID);
		'set domainid '+string(DID);
		'set slavealias '+string(SLA);
		'set slaveposition '+string(SLP);
		'set showstate '+string(SLST);
		'set enFilter '+string(ENFILT);
		'set wire_1 '+string(WIRE1);
		'set wire_2 '+string(WIRE2);
		'set wire_3 '+string(WIRE3);
		'set wire_4 '+string(WIRE4);
		'set rtdtyp_0 '+string(RTD1);
		'set rtdtyp_1 '+string(RTD2);
		'set rtdtyp_3 '+string(RTD3);
		'set rtdtyp_4 '+string(RTD4);
		];
   listentry = ['.eltop.list insert end $slavestring(0)';
		'.eltop.list insert end $slavestring(1)';
		'.eltop.list insert end $slavestring(2)';
		'.eltop.list see $selectid';
		'.eltop.list selection set $selectid';
	];
   tclscript = [txt;
		initvalues;
	        listentry
	];
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
  SLST = check_pos_integer('Slave Stateoutput','showstate');
  if SLST<0  then
    repeat_dialog = %t;
  end
  ENFILT = check_pos_integer('Enable Filter','enFilter');
  if ENFILT<0  then
    repeat_dialog = %t;
  end
  WIRE1 = check_pos_integer('Wire Connection 1','wire_1');
  if WIRE1<0  then
    repeat_dialog = %t;
  end
  WIRE2 = check_pos_integer('Wire Connection 2','wire_2');
  if WIRE2<0  then
    repeat_dialog = %t;
  end
  WIRE3 = check_pos_integer('Wire Connection 3','wire_3');
  if WIRE3<0  then
    repeat_dialog = %t;
  end
  WIRE4 = check_pos_integer('Wire Connection 4','wire_4');
  if WIRE4<0  then
    repeat_dialog = %t;
  end
  RTD1 = check_pos_integer('RTD Typ 1','rtdtyp_0');
  if RTD1<0  then
    repeat_dialog = %t;
  end
  RTD2 = check_pos_integer('RTD Typ 2','rtdtyp_1');
  if RTD2<0  then
    repeat_dialog = %t;
  end
  RTD3 = check_pos_integer('RTD Typ 3','rtdtyp_3');
  if RTD3<0  then
    repeat_dialog = %t;
  end
  RTD4 = check_pos_integer('RTD Typ 4','rtdtyp_4');
  if RTD4<0  then
    repeat_dialog = %t;
  end

  TCL_EvalStr('destroy $eltop')
 endfunction
