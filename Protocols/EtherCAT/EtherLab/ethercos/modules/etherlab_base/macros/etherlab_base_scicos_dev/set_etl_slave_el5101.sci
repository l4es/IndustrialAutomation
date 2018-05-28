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

function [repeat_dialog,selslave,selectid,MID,DID,SLA,SLP,SLST,ENABLEREGISTERRELOAD,ENABLEREGISTERRESET,ENABLEFWDCOUNT,GATETYP,FREQWINDOW,COUNTERRELOADVALUE,CONTROLINPUTPORT,PRESETINPUTPORT,COUNTEROUTPUTPORT,LATCHOUTPUTPORT,FREQOUTPUTPORT,PERIODOUTPUTPORT,WINDOWOUTPUTPORT] = set_etl_slave_el5101(selslave,selectid,MID,DID,SLA,SLP,SLST,ENABLEREGISTERRELOAD,ENABLEREGISTERRESET,ENABLEFWDCOUNT,GATETYP,FREQWINDOW,COUNTERRELOADVALUE,CONTROLINPUTPORT,PRESETINPUTPORT,COUNTEROUTPUTPORT,LATCHOUTPUTPORT,FREQOUTPUTPORT,PERIODOUTPUTPORT,WINDOWOUTPUTPORT)
  check_values = 0;
  fd = mopen([getenv('ETLPATH')+'/modules/etherlab_base/src/tcl/'+'etl_scicos_el5101.tcl'],'r');
  txt = mgetl(fd,-1);
  mclose(fd);

  initvalues = ['array set slavestring { 0 EL5101}' 
		'array set slavestate { 0 1}'; 
		'set slaveselected '+selslave;
		'set selectid '+string(selectid);
		'set masterid '+string(MID);
		'set domainid '+string(DID);
		'set slavealias '+string(SLA);
		'set slaveposition '+string(SLP);
		'set showstate '+string(SLST);
		'set enableregisterreload '+string(ENABLEREGISTERRELOAD);
		'set enableregisterreset '+string(ENABLEREGISTERRESET);
		'set enablefwdcount '+string(ENABLEFWDCOUNT);
		'set gatetyp '+string(GATETYP);
		'set freqwindow '+string(FREQWINDOW);
		'set counterreloadvalue '+string(COUNTERRELOADVALUE);
		'set controlinputport '+string(CONTROLINPUTPORT);
		'set presetinputport '+string(PRESETINPUTPORT);
		'set counteroutputport '+string(COUNTEROUTPUTPORT);
		'set latchoutputport '+string(LATCHOUTPUTPORT);
		'set freqoutputport '+string(FREQOUTPUTPORT);
		'set periodoutputport '+string(PERIODOUTPUTPORT);
		'set windowoutputport '+string(WINDOWOUTPUTPORT);
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

  ENABLEREGISTERRELOAD = check_pos_integer('Enable Register Reload','enableregisterreload');
  if ENABLEREGISTERRELOAD<0  then
    repeat_dialog = %t;
  end
  ENABLEREGISTERRESET = check_pos_integer('Enable Register Reset','enableregisterreset');
  if ENABLEREGISTERRESET<0  then
    repeat_dialog = %t;
  end
  ENABLEFWDCOUNT = check_pos_integer('Enable Forward Counter','enablefwdcounter');
  if ENABLEFWDCOUNT<0  then
    repeat_dialog = %t;
  end
  GATETYP = check_pos_integer('Gatetype','gatetyp');
  if GATETYP<0  then
    repeat_dialog = %t;
  end
  FREQWINDOW = check_pos_integer('Frequency Window','freqwindow');
  if FREQWINDOW<0  then
    repeat_dialog = %t;
  end
  COUNTERRELOADVALUE = check_pos_integer('Counter Reload Value','counterreloadvalue');
  if COUNTERRELOADVALUE<0  then
    repeat_dialog = %t;
  end
  CONTROLINPUTPORT = check_pos_integer('Enable Control Input Port','controlinputport');
  if CONTROLINPUTPORT<0  then
    repeat_dialog = %t;
  end
  PRESETINPUTPORT = check_pos_integer('Enable Reset Inputport','presetinputport');
  if PRESETINPUTPORT<0  then
    repeat_dialog = %t;
  end
  COUNTEROUTPUTPORT = check_pos_integer('Enable Counter Output Port','counteroutptport');
  if COUNTEROUTPUTPORT<0  then
    repeat_dialog = %t;
  end
  LATCHOUTPUTPORT = check_pos_integer('Enable Latch Output Port','latchoutputport');
  if LATCHOUTPUTPORT<0  then
    repeat_dialog = %t;
  end
  FREQOUTPUTPORT = check_pos_integer('Enable Frequency Outputport','freqoutputport');
  if FREQOUTPUTPORT<0  then
    repeat_dialog = %t;
  end
  PERIODOUTPUTPORT = check_pos_integer('Enable Period Outputport','periodoutputport');
  if PERIODOUTPUTPORT<0  then
    repeat_dialog = %t;
  end
  WINDOWOUTPUTPORT = check_pos_integer('Enable Window Outputport','windowoutputport');
  if WINDOWOUTPUTPORT<0  then
    repeat_dialog = %t;
  end

  TCL_EvalStr('destroy $eltop')
 endfunction
