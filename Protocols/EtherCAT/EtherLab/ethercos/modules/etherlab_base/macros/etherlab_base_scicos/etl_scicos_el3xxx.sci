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

function [x,y,typ]=etl_scicos_el3xxx(job,arg1,arg2)
x=[];y=[];typ=[];
select job
case 'plot' then
  exprs=arg1.graphics.exprs;
  STYP=exprs(1)
  MID=evstr(exprs(2))
  DID=evstr(exprs(3))
  SLA=evstr(exprs(4))
  SLP=evstr(exprs(5))
  SLST=evstr(exprs(6))
  STYPID=evstr(exprs(7))
  standard_draw(arg1)
case 'getinputs' then
  [x,y,typ]=standard_inputs(arg1)
case 'getoutputs' then
  [x,y,typ]=standard_outputs(arg1)
case 'getorigin' then
  [x,y]=standard_origin(arg1)
case 'set' then
  x=arg1;
  graphics=arg1.graphics;exprs=graphics.exprs
  model=arg1.model;
  while %t do
   STYP=exprs(1)
   MID=evstr(exprs(2))
   DID=evstr(exprs(3))
   SLA=evstr(exprs(4))
   SLP=evstr(exprs(5))
   SLST=evstr(exprs(6))
   STYPID=evstr(exprs(7))
   [ln,fun]=where(); 
   if (fun(3) == "clickin") then
       [loop,STYP,STYPID,MID,DID,SLA,SLP,SLST] = set_etl_slave_el3xxx(STYP,STYPID,MID,DID,SLA,SLP,SLST)
    else
       loop = %f;
   end
   //SDO Config
   //Datatype Index Subindex Value
   slave_sdoconfig = int32([]);
   valid_slave = %f; //Flag for Loop handling
   slave_desc = getslavedesc('EtherCATInfo_el3xxx');
       //Index subindex vectorlength valuetype bitlength Channelno Direction TypeCode Fullrange Scale Offset
	//valuetype:
	//    si_double_T = 0, si_single_T =1, si_uint8_T = 2, si_sint8_T = 3, 
    	//    si_uint16_T = 4, si_sint16_T = 5,si_uint32_T = 6, si_sint32_T = 7,
	//    si_boolean_T =8, 
	//TypeCode 0=Analog 1=RawScale 2=Digital
	//Direction 1=Slave send to Master, eg EL3162,EL1004
	//Direction 2= Master send to Slave, eg EL4102,EL2032
	//Fullrange 2^n (Rawbitvalues)
	//Scale in UnitValue, ex. EL3102: 20 for +/-10V (Faktor from 0-1 to Range)
	//Offset in UnitValue, ex. EL3102: -10 for -10V
	//Calculation:
	//Output = RawValue/FullRange *Scale +Offset

   //Clear Channel List
   clear channel

   if STYP == 'EL3102' then
   	slave_revision = hex2dec('00000000');	
       	slave_type = 'EL3102';
       	slave_outputs = [1 1; 1 1]; //[rows input 1 colums input 1;...]
	slave_output_types = [1;1] //all real [Type Input 1; Type Input 2]
	slave_inputs = [];
	slave_input_types = [];
	channel(1).index = hex2dec('3101');
	channel(1).subindex = hex2dec('2');
	channel(1).vectorlength = 1;
	channel(1).valuetype = 'si_sint16_T';
	channel(1).typecode = 'analog';
	channel(1).direction = 'getvalues';
	channel(1).channelno = 1;
	channel(1).scale = 10.0;
	channel(1).offset = 0.0;
	channel(1).fullrange = 15;
	channel(2) = channel(1);
	channel(2).index = hex2dec('3102');
	channel(2).subindex = hex2dec('2');
	channel(2).channelno = 2;

	if SLST == 1 then
		channel(3).index = hex2dec('3101');
		channel(3).subindex = hex2dec('1');
		channel(3).vectorlength = 1;
		channel(3).valuetype = 'si_uint8_T';
		channel(3).typecode = 'digital';
		channel(3).direction = 'getvalues';
		channel(3).channelno = 3;
		channel(3).scale = 0.0;
		channel(3).offset = 0.0;
		channel(3).fullrange = 0;
		channel(4) = channel(3);
		channel(4).index = hex2dec('3102');
		channel(4).subindex = hex2dec('1');
		channel(4).channelno = 4;
	        slave_outputs = [1 1; 1 1; 1 1; 1 1];
	        slave_output_types = [1; 1; 1; 1];	
        end
	[slave_pdomapping,slave_pdomapping_scale,valid_mapping]=build_mapstruct(channel);
   end
   if STYP == 'EL3152' then
   	slave_revision = hex2dec('00000000');	
       	slave_type = 'EL3152';
       	slave_outputs = [1 1; 1 1]; //[rows input 1 colums input 1;...]
	slave_output_types = [1;1] //all real [Type Input 1; Type Input 2]
	slave_inputs = [];
	slave_input_types = [];
	channel(1).index = hex2dec('3101');
	channel(1).subindex = hex2dec('2');
	channel(1).vectorlength = 1;
	channel(1).valuetype = 'si_uint16_T';
	channel(1).typecode = 'analog';
	channel(1).direction = 'getvalues';
	channel(1).channelno = 1;
	channel(1).scale = 16.0;
	channel(1).offset = 0.0;
	channel(1).fullrange = 15;
	channel(2) = channel(1);
	channel(2).index = hex2dec('3102');
	channel(2).subindex = hex2dec('2');
	channel(2).channelno = 2;
	if SLST == 1 then
		channel(3).index = hex2dec('3101');
		channel(3).subindex = hex2dec('1');
		channel(3).vectorlength = 1;
		channel(3).valuetype = 'si_uint8_T';
		channel(3).typecode = 'digital';
		channel(3).direction = 'getvalues';
		channel(3).channelno = 3;
		channel(3).scale = 0.0;
		channel(3).offset = 0.0;
		channel(3).fullrange = 0;
		channel(4) = channel(3);
		channel(4).index = hex2dec('3102');
		channel(4).subindex = hex2dec('1');
		channel(4).channelno = 4;
	 	slave_outputs = [1 1; 1 1; 1 1; 1 1];
	 	slave_output_types = [1; 1; 1; 1];	
     	end
	[slave_pdomapping,slave_pdomapping_scale,valid_mapping]=build_mapstruct(channel);
   end
   if STYP == 'EL3162' then
   	slave_revision = hex2dec('00000000');	
       	slave_type = 'EL3162';
       	slave_outputs = [1 1; 1 1]; //[rows input 1 colums input 1;...]
	slave_output_types = [1;1] //all real [Type Input 1; Type Input 2]
	slave_inputs = [];
	slave_input_types = [];
	channel(1).index = hex2dec('3101');
	channel(1).subindex = hex2dec('2');
	channel(1).vectorlength = 1;
	channel(1).valuetype = 'si_uint16_T';
	channel(1).typecode = 'analog';
	channel(1).direction = 'getvalues';
	channel(1).channelno = 1;
	channel(1).scale = 10.0;
	channel(1).offset = 0.0;
	channel(1).fullrange = 15;
	channel(2) = channel(1);
	channel(2).index = hex2dec('3102');
	channel(2).subindex = hex2dec('2');
	channel(2).channelno = 2;
	if SLST == 1 then
		channel(3).index = hex2dec('3101');
		channel(3).subindex = hex2dec('1');
		channel(3).vectorlength = 1;
		channel(3).valuetype = 'si_uint8_T';
		channel(3).typecode = 'digital';
		channel(3).direction = 'getvalues';
		channel(3).channelno = 3;
		channel(3).scale = 0.0;
		channel(3).offset = 0.0;
		channel(3).fullrange = 0;
		channel(4) = channel(3);
		channel(4).index = hex2dec('3102');
		channel(4).subindex = hex2dec('1');
		channel(4).channelno = 4;
		slave_outputs = [1 1; 1 1; 1 1; 1 1];
	 	slave_output_types = [1; 1; 1; 1];	
     	end;
	[slave_pdomapping,slave_pdomapping_scale,valid_mapping]=build_mapstruct(channel);
   end

   if valid_mapping < 0 then
	disp('No valid Mapping Configuration');
   end;
   slave_typeid = getslavedesc_checkslave(slave_desc,slave_type,slave_revision);
   if slave_typeid > 0 then
   	slave_config= getslavedesc_getconfig(slave_desc,slave_typeid);
	[slave_smconfig,slave_pdoconfig,slave_pdoentry,valid_slave] = getslavedesc_buildopar(slave_config,0,0); //Default Configurartion
   else
	disp('Can not find valid Configuration.');
   end
	
   if isempty(slave_inputs) then
	slave_input_list = list();
   else
	slave_input_list = list(slave_inputs,slave_input_types);
   end   
   if isempty(slave_outputs) then
	slave_output_list = list();
   else
	slave_output_list = list(slave_outputs,slave_output_types);
   end   

   [model,graphics,ok]=set_io(model,graphics,slave_input_list,slave_output_list,[1],[])
   if and([~loop ok valid_slave]) then
     exprs(1)=STYP;
     exprs(2)=sci2exp(MID);
     exprs(3)=sci2exp(DID);
     exprs(4)=sci2exp(SLA);
     exprs(5)=sci2exp(SLP);
     exprs(6)=sci2exp(SLST);
     exprs(7)=sci2exp(STYPID);
     graphics.exprs=exprs;
     DEBUG=1; //DEBUG =1 => Debug the calculation function 
     model.ipar=[DEBUG;MID;DID;SLA;SLP];  //transmit integer variables to the c block 
     model.rpar=[];       		 //transmit double variables to the c block
     slave_vendor = getslavedesc_vendor(slave_desc);
     slave_productcode = getslavedesc_productcode(slave_desc,slave_typeid);
     slave_generic = int32([slave_vendor; slave_productcode]);
     model.opar = list(slave_smconfig,slave_pdoconfig,slave_pdoentry,slave_sdoconfig,slave_pdomapping,slave_generic, slave_pdomapping_scale);
     model.dstate=[1];
     model.dep_ut=[%t, %f]
     x.graphics=graphics;x.model=model
     break
    end
  end	
case 'define' then
  model=scicos_model()
  model.sim=list('etl_scicos',4) // name of the c-function
  DEBUG=0;//No Debuging
  MID=0;
  DID=0;
  SLA=0;
  SLP=0;
  SLST=0;
  STYP='EL3102';
  STYPID=0;
  slave_type = 'EL3102';
  //SDO Config
  //Datatype Index Subindex Value	
  slave_sdoconfig = int32([]);
  rpar=[];
  ipar=[DEBUG;MID;DID;SLA;SLP];
  model.evtin=1
  model.evtout=[]
  model.out=[1;1]
  model.out2=[1;1]
  model.outtyp=[1;1]
  model.in=[]
  model.rpar=rpar;
  model.ipar=ipar;
  model.dstate=[1];
  model.blocktype='d'
  model.dep_ut=[%t, %f]
  //Set Default Slave
  slave_desc = getslavedesc('EtherCATInfo_el3xxx');	
  slave_revision = 0;
  slave_typeid = getslavedesc_checkslave(slave_desc,slave_type,slave_revision);
  slave_config= getslavedesc_getconfig(slave_desc,slave_typeid);	
  [slave_smconfig,slave_pdoconfig,slave_pdoentry,valid_slave] = getslavedesc_buildopar(slave_config,0,0); //Default Configurartion
  slave_vendor = getslavedesc_vendor(slave_desc);
  slave_productcode = getslavedesc_productcode(slave_desc,slave_typeid);
   //Index subindex vectorlength valuetype bitlength Channelno Direction TypeCode Fullrange Scale Offset
  clear channel;
  channel(1).index = hex2dec('3101');
  channel(1).subindex = hex2dec('2');
  channel(1).vectorlength = 1;
  channel(1).valuetype = 'si_sint16_T';
  channel(1).typecode = 'analog';
  channel(1).direction = 'getvalues';
  channel(1).channelno = 1;
  channel(1).scale = 10.0;
  channel(1).offset = 0.0;
  channel(1).fullrange = 15;
  channel(2) = channel(1);
  channel(2).index = hex2dec('3102');
  channel(2).subindex = hex2dec('2');
  channel(2).channelno = 2;
  [slave_pdomapping,slave_pdomapping_scale,valid_mapping]=build_mapstruct(channel);	
  if valid_mapping < 0 then
	disp('No valid Mapping Configuration');
  end;
  slave_generic = int32([slave_vendor; slave_productcode]);
  model.opar = list(slave_smconfig,slave_pdoconfig,slave_pdoentry,slave_sdoconfig,slave_pdomapping,slave_generic, slave_pdomapping_scale);
  exprs=[STYP,sci2exp(ipar(2)),sci2exp(ipar(3)),sci2exp(ipar(4)),sci2exp(ipar(5)),sci2exp(SLST),sci2exp(STYPID)]
  gr_i=['xstringb(orig(1),orig(2),[''Ethercat ''+STYP;''Master ''+string(MID)+'' Pos ''+string(SLP);''Alias ''+string(SLA)],sz(1),sz(2),''fill'');']

  x=standard_define([4 2],model,exprs,gr_i)
  x.graphics.id=["EL3XXX"]
end     	
endfunction
