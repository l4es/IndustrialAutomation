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

function [repeat_dialog,selslave,selectid,MID,DID,SLA,SLP,SLST,FRAMEERROR,INHIBIT,SSICODE,BAUD,FRAME,FRAMESIZE,LENGTH,INHIBITTIME] = set_etl_slave_el5001(selslave,selectid,MID,DID,SLA,SLP,SLST,FRAMEERROR,INHIBIT,SSICODE,BAUD,FRAME,FRAMESIZE,LENGTH,INHIBITTIME)
  check_values = 0;
  fd = mopen([getenv('ETLPATH')+'/modules/etherlab_base/src/tcl/'+'etl_scicos_el5001.tcl'],'r');
  txt = mgetl(fd,-1);
  mclose(fd);

  initvalues = ['array set slavestring { 0 EL5001V1 1 EL5001V2}' 
		'array set slavestate { 0 1 1 1}'; 
		'set slaveselected '+selslave;
		'set selectid '+string(selectid);
		'set masterid '+string(MID);
		'set domainid '+string(DID);
		'set slavealias '+string(SLA);
		'set slaveposition '+string(SLP);
		'set showstate '+string(SLST);
		'set frameerror '+string(FRAMEERROR);
		'set inhibit '+string(INHIBIT);
		'set ssicode '+string(SSICODE);
		'set baud '+string(BAUD);
		'set frame '+string(FRAME);
		'set framesize '+string(FRAMESIZE);
		'set datalength '+string(LENGTH);
		'set inhibittime '+string(INHIBITTIME);
		];
   listentry = ['.eltop.list insert end $slavestring(0)';
		'.eltop.list insert end $slavestring(1)';
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
  FRAMEERROR = check_pos_integer('Frame Error','frameerror');
  if FRAMEERROR<0  then
    repeat_dialog = %t;
  end
  INHIBIT = check_pos_integer('Enable Inhibit','inhibit');
  if INHIBIT<0  then
    repeat_dialog = %t;
  end
  SSICODE = check_pos_integer('SSI Mode','ssicode');
  if SSICODE<0  then
    repeat_dialog = %t;
  end
  BAUD = check_pos_integer('Baud Rate','baud');
  if BAUD<0  then
    repeat_dialog = %t;
  end
  FRAME = check_pos_integer('Frame Type','frame');
  if FRAME<0  then
    repeat_dialog = %t;
  end
  FRAMESIZE = check_pos_integer('Frame Size','framesize');
  if FRAMESIZE<0  then
    repeat_dialog = %t;
  end
  LENGTH = check_pos_integer('Data Length','datalength');
  if LENGTH<0  then
    repeat_dialog = %t;
  end
  INHIBITTIME = check_pos_integer('Inhibit Time','inhibittime');
  if INHIBITTIME<0  then
    repeat_dialog = %t;
  end

  TCL_EvalStr('destroy $eltop')
 endfunction
