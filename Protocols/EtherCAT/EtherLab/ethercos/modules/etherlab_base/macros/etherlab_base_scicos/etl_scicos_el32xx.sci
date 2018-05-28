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

function [x,y,typ]=etl_scicos_el32xx(job,arg1,arg2)
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
  ENFI=evstr(exprs(8))
  RTDTYP1=evstr(exprs(9))
  COTYP1=evstr(exprs(10))
  RTDTYP2=evstr(exprs(11))
  COTYP2=evstr(exprs(12))
  RTDTYP3=evstr(exprs(13))
  COTYP3=evstr(exprs(14))
  RTDTYP4=evstr(exprs(15))
  COTYP4=evstr(exprs(16))
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
   ENFI=evstr(exprs(8))
   RTDTYP1=evstr(exprs(9))
   COTYP1=evstr(exprs(10))
   RTDTYP2=evstr(exprs(11))
   COTYP2=evstr(exprs(12))
   RTDTYP3=evstr(exprs(13))
   COTYP3=evstr(exprs(14))
   RTDTYP4=evstr(exprs(15))
   COTYP4=evstr(exprs(16))
   [ln,fun]=where(); 
   if (fun(3) == "clickin") then
       [loop,STYP,STYPID,MID,DID,SLA,SLP,SLST,ENFI,COTYP1,COTYP2,COTYP3,COTYP4,RTDTYP1,RTDTYP2,RTDTYP3,RTDTYP4] = set_etl_slave_el32xx(STYP,STYPID,MID,DID,SLA,SLP,SLST,ENFI,COTYP1,COTYP2,COTYP3,COTYP4,RTDTYP1,RTDTYP2,RTDTYP3,RTDTYP4)
    else
       loop = %f;
   end
   //SDO Config
   //Datatype Index Subindex Value
   slave_sdoconfig = int32([]);
   valid_slave = %f; //Flag for Loop handling
   slave_desc = getslavedesc('EtherCATInfo_el3xxx');

   //Clear Channel List
   clear channel
   clear sdo
   sdoinc = 1;
   
   select STYP
   case 'EL3201' then
	sdoloop = [8000];
   case 'EL3202' then
	sdoloop = [8000 8010];
   case 'EL3204' then
	sdoloop = [8000 8010 8020 8040];
   end

   //Generell SDO Configurations

   for sdocount = sdoloop
   	sdo(sdoinc).index = hex2dec(string(sdocount));
   	sdo(sdoinc).subindex = hex2dec('1');
   	sdo(sdoinc).datatype = 'si_uint8_T';
   	sdo(sdoinc).value =  0; // Disable user scaling Channel 
	sdoinc = sdoinc +1;
        sdo(sdoinc).index = hex2dec(string(sdocount));
	sdo(sdoinc).subindex = hex2dec('2');
	sdo(sdoinc).datatype = 'si_uint8_T';
	sdo(sdoinc).value =  0; // Value as signed int 16 Channel 
	sdoinc = sdoinc +1;
        sdo(sdoinc).index = hex2dec(string(sdocount));
	sdo(sdoinc).subindex = hex2dec('7');
	sdo(sdoinc).datatype = 'si_uint8_T';
	sdo(sdoinc).value =  0; // Disable Limit 1 Channel 
	sdoinc = sdoinc +1;
        sdo(sdoinc).index = hex2dec(string(sdocount));
	sdo(sdoinc).subindex = hex2dec('8');
	sdo(sdoinc).datatype = 'si_uint8_T';
	sdo(sdoinc).value =  0; // Disable Limit 2 Channel 
	sdoinc = sdoinc +1;
        sdo(sdoinc).index = hex2dec(string(sdocount));
	sdo(sdoinc).subindex = hex2dec('A');
	sdo(sdoinc).datatype = 'si_uint8_T';
	sdo(sdoinc).value =  0; // Disable user callibration Channel 
	sdoinc = sdoinc +1;
        sdo(sdoinc).index = hex2dec(string(sdocount));
	sdo(sdoinc).subindex = hex2dec('B');
	sdo(sdoinc).datatype = 'si_uint8_T';
	sdo(sdoinc).value =  1; // Enable vendor callibration Channel 
	sdoinc = sdoinc +1;
   end

   sdo(sdoinc).index = hex2dec('8000');
   sdo(sdoinc).subindex = hex2dec('6');
   sdo(sdoinc).datatype = 'si_uint8_T'; 
   sdo(sdoinc).value = ENFI; // Enable Filter for all Channels
   sdoinc = sdoinc +1;
   sdo(sdoinc).index = hex2dec('8000');
   sdo(sdoinc).subindex = hex2dec('16');
   sdo(sdoinc).datatype = 'si_uint16_T'; 
   sdo(sdoinc).value = 0; // Default Filter Freq=50Hz
   sdoinc = sdoinc +1;

   if STYP == 'EL3201' then
   	slave_revision = hex2dec('00100000');	
       	slave_type = 'EL3201';
       	slave_outputs = [1 1]; //[rows input 1 colums input 1;...]
	slave_output_types = [1] //all real [Type Input 1; Type Input 2]
	slave_inputs = [];
	slave_input_types = [];
	channel(1).index = hex2dec('6000');
	channel(1).subindex = hex2dec('11');
	channel(1).vectorlength = 1;
	channel(1).valuetype = 'si_sint16_T';
	channel(1).typecode = 'raw';
	channel(1).direction = 'getvalues';
	channel(1).channelno = 1;
	channel(1).scale = 0.1;
	channel(1).offset = 0.0;
	channel(1).fullrange = 16;

	if SLST == 1 then
		channel(2).index = hex2dec('6000');
		channel(2).subindex = hex2dec('7');
		channel(2).vectorlength = 1;
		channel(2).valuetype = 'si_uint8_T';
		channel(2).typecode = 'digital';
		channel(2).direction = 'getvalues';
		channel(2).channelno = 2;
		channel(2).scale = 0.0;
		channel(2).offset = 0.0;
		channel(2).fullrange = 0;
	        slave_outputs = [1 1; 1 1];
	        slave_output_types = [1; 1];	
        end
	
        sdo(sdoinc).index = hex2dec('8000');
	sdo(sdoinc).subindex = hex2dec('19');
	sdo(sdoinc).datatype = 'si_uint16_T';
	sdo(sdoinc).value = RTDTYP1; 
	sdoinc = sdoinc +1
        sdo(sdoinc).index = hex2dec('8000');
	sdo(sdoinc).subindex = hex2dec('1A');
	sdo(sdoinc).datatype = 'si_uint16_T';
	sdo(sdoinc).value = COTYP1;
	sdoinc = sdoinc +1;

	[slave_sdoconfig,valid_sdoconfig]=build_sdostruct(sdo)
	//disp(slave_sdoconfig)
	[slave_pdomapping,slave_pdomapping_scale,valid_mapping]=build_mapstruct(channel);
   end

   if STYP == 'EL3202' then
   	slave_revision = hex2dec('00100000');	
       	slave_type = 'EL3202';
       	slave_outputs = [1 1;1 1]; //[rows input 1 colums input 1;...]
	slave_output_types = [1; 1] //all real [Type Input 1; Type Input 2]
	slave_inputs = [];
	slave_input_types = [];
	channel(1).index = hex2dec('6000');
	channel(1).subindex = hex2dec('11');
	channel(1).vectorlength = 1;
	channel(1).valuetype = 'si_sint16_T';
	channel(1).typecode = 'raw';
	channel(1).direction = 'getvalues';
	channel(1).channelno = 1;
	channel(1).scale = 0.1;
	channel(1).offset = 0.0;
	channel(1).fullrange = 16;
	channel(2)= channel(1);
	channel(2).index = hex2dec('6010');
	channel(2).channelno = 2;

	if SLST == 1 then
		channel(3).index = hex2dec('6000');
		channel(3).subindex = hex2dec('7');
		channel(3).vectorlength = 1;
		channel(3).valuetype = 'si_uint8_T';
		channel(3).typecode = 'digital';
		channel(3).direction = 'getvalues';
		channel(3).channelno = 3;
		channel(3).scale = 0.0;
		channel(3).offset = 0.0;
		channel(3).fullrange = 0;
		channel(4) = channel(3);
		channel(4).index = hex2dec('6010');
		channel(4).channelno = 4;
	        slave_outputs = [1 1; 1 1; 1 1; 1 1];
	        slave_output_types = [1; 1; 1; 1];	
        end
	
        sdo(sdoinc).index = hex2dec('8000');
	sdo(sdoinc).subindex = hex2dec('19');
	sdo(sdoinc).datatype = 'si_uint16_T';
	sdo(sdoinc).value = RTDTYP1; 
	sdoinc = sdoinc +1
        sdo(sdoinc).index = hex2dec('8000');
	sdo(sdoinc).subindex = hex2dec('1A');
	sdo(sdoinc).datatype = 'si_uint16_T';
	sdo(sdoinc).value = COTYP1;
	sdoinc = sdoinc +1;
        sdo(sdoinc).index = hex2dec('8010');
	sdo(sdoinc).subindex = hex2dec('19');
	sdo(sdoinc).datatype = 'si_uint16_T';
	sdo(sdoinc).value = RTDTYP2; 
	sdoinc = sdoinc +1
        sdo(sdoinc).index = hex2dec('8010');
	sdo(sdoinc).subindex = hex2dec('1A');
	sdo(sdoinc).datatype = 'si_uint16_T';
	sdo(sdoinc).value = COTYP2;
	sdoinc = sdoinc +1;

	[slave_sdoconfig,valid_sdoconfig]=build_sdostruct(sdo)
	disp(slave_sdoconfig)
	[slave_pdomapping,slave_pdomapping_scale,valid_mapping]=build_mapstruct(channel);
   end

   if STYP == 'EL3204' then
   	slave_revision = hex2dec('00100000');	
       	slave_type = 'EL3204';
       	slave_outputs = [1 1;1 1;1 1;1 1]; //[rows input 1 colums input 1;...]
	slave_output_types = [1; 1; 1; 1] //all real [Type Input 1; Type Input 2]
	slave_inputs = [];
	slave_input_types = [];
	channel(1).index = hex2dec('6000');
	channel(1).subindex = hex2dec('11');
	channel(1).vectorlength = 1;
	channel(1).valuetype = 'si_sint16_T';
	channel(1).typecode = 'raw';
	channel(1).direction = 'getvalues';
	channel(1).channelno = 1;
	channel(1).scale = 0.1;
	channel(1).offset = 0.0;
	channel(1).fullrange = 16;
	channel(2)= channel(1);
	channel(2).index = hex2dec('6010');
	channel(2).channelno = 2;
	channel(3)= channel(1);
	channel(3).index = hex2dec('6020');
	channel(3).channelno = 3;
	channel(4)= channel(1);
	channel(4).index = hex2dec('6030');
	channel(4).channelno = 4;

	if SLST == 1 then
		channel(5).index = hex2dec('6000');
		channel(5).subindex = hex2dec('7');
		channel(5).vectorlength = 1;
		channel(5).valuetype = 'si_uint8_T';
		channel(5).typecode = 'digital';
		channel(5).direction = 'getvalues';
		channel(5).channelno = 5;
		channel(5).scale = 0.0;
		channel(5).offset = 0.0;
		channel(5).fullrange = 0;
		channel(6) = channel(5);
		channel(6).index = hex2dec('6010');
		channel(6).channelno = 6;
		channel(7) = channel(5);
		channel(7).index = hex2dec('6020');
		channel(7).channelno = 7;
		channel(8) = channel(5);
		channel(8).index = hex2dec('6030');
		channel(8).channelno = 8;
	        slave_outputs = [1 1; 1 1; 1 1; 1 1; 1 1; 1 1; 1 1; 1 1];
	        slave_output_types = [1; 1; 1; 1; 1; 1; 1; 1];	
        end
	
        sdo(sdoinc).index = hex2dec('8000');
	sdo(sdoinc).subindex = hex2dec('19');
	sdo(sdoinc).datatype = 'si_uint16_T';
	sdo(sdoinc).value = RTDTYP1; 
	sdoinc = sdoinc +1;
        sdo(sdoinc).index = hex2dec('8000');
	sdo(sdoinc).subindex = hex2dec('1A');
	sdo(sdoinc).datatype = 'si_uint16_T';
	sdo(sdoinc).value = COTYP1;
	sdoinc = sdoinc +1;
        sdo(sdoinc).index = hex2dec('8010');
	sdo(sdoinc).subindex = hex2dec('19');
	sdo(sdoinc).datatype = 'si_uint16_T';
	sdo(sdoinc).value = RTDTYP2; 
	sdoinc = sdoinc +1;
        sdo(sdoinc).index = hex2dec('8010');
	sdo(sdoinc).subindex = hex2dec('1A');
	sdo(sdoinc).datatype = 'si_uint16_T';
	sdo(sdoinc).value = COTYP2;
	sdoinc = sdoinc +1;
        sdo(sdoinc).index = hex2dec('8020');
	sdo(sdoinc).subindex = hex2dec('19');
	sdo(sdoinc).datatype = 'si_uint16_T';
	sdo(sdoinc).value = RTDTYP3; 
	sdoinc = sdoinc +1;
        sdo(sdoinc).index = hex2dec('8020');
	sdo(sdoinc).subindex = hex2dec('1A');
	sdo(sdoinc).datatype = 'si_uint16_T';
	sdo(sdoinc).value = COTYP3;
	sdoinc = sdoinc +1;
        sdo(sdoinc).index = hex2dec('8030');
	sdo(sdoinc).subindex = hex2dec('19');
	sdo(sdoinc).datatype = 'si_uint16_T';
	sdo(sdoinc).value = RTDTYP4; 
	sdoinc = sdoinc +1;
        sdo(sdoinc).index = hex2dec('8030');
	sdo(sdoinc).subindex = hex2dec('1A');
	sdo(sdoinc).datatype = 'si_uint16_T';
	sdo(sdoinc).value = COTYP4;
	sdoinc = sdoinc +1;

	[slave_sdoconfig,valid_sdoconfig]=build_sdostruct(sdo)
	disp(slave_sdoconfig)
	[slave_pdomapping,slave_pdomapping_scale,valid_mapping]=build_mapstruct(channel);
   end



   if valid_sdoconfig < 0 then
	disp('No valid SDO Configuration');
   end;

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
     exprs(8)=sci2exp(ENFI);
     exprs(9)=sci2exp(RTDTYP1);
     exprs(10)=sci2exp(COTYP1);
     exprs(11)=sci2exp(RTDTYP2);
     exprs(12)=sci2exp(COTYP2);
     exprs(13)=sci2exp(RTDTYP3);
     exprs(14)=sci2exp(COTYP3);
     exprs(15)=sci2exp(RTDTYP4);
     exprs(16)=sci2exp(COTYP4);
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
  STYP='EL3201';
  ENFI=0;
  STYPID=0;
  RTDTYP1=0;
  COTYP1=0;
  RTDTYP2=0;
  COTYP2=0;
  RTDTYP3=0;
  COTYP3=0;
  RTDTYP4=0;
  COTYP4=0;
  slave_type = 'EL3201';
  //SDO Config
  //Datatype Index Subindex Value	
  slave_sdoconfig = int32([]);
  rpar=[];
  ipar=[DEBUG;MID;DID;SLA;SLP];
  model.evtin=1
  model.evtout=[]
  model.out=[1]
  model.out2=[1]
  model.outtyp=[1]
  model.in=[]
  model.rpar=rpar;
  model.ipar=ipar;
  model.dstate=[1];
  model.blocktype='d'
  model.dep_ut=[%t, %f]

  //SDO-Configuration
  sdoinc=1
  sdo(sdoinc).index = hex2dec('8000');
  sdo(sdoinc).subindex = hex2dec('1');
  sdo(sdoinc).datatype = 'si_uint8_T';
  sdo(sdoinc).value =  0; // Disable user scaling Channel 
  sdoinc = sdoinc +1;
  sdo(sdoinc).index = hex2dec('8000');
  sdo(sdoinc).subindex = hex2dec('2');
  sdo(sdoinc).datatype = 'si_uint8_T';
  sdo(sdoinc).value =  0; // Value as signed int 16 Channel 
  sdoinc = sdoinc +1;
  sdo(sdoinc).index = hex2dec('8000');
  sdo(sdoinc).subindex = hex2dec('7');
  sdo(sdoinc).datatype = 'si_uint8_T';
  sdo(sdoinc).value =  0; // Disable Limit 1 Channel 
  sdoinc = sdoinc +1;
  sdo(sdoinc).index = hex2dec('8000');
  sdo(sdoinc).subindex = hex2dec('8');
  sdo(sdoinc).datatype = 'si_uint8_T';
  sdo(sdoinc).value =  0; // Disable Limit 2 Channel 
  sdoinc = sdoinc +1;
  sdo(sdoinc).index = hex2dec('8000');
  sdo(sdoinc).subindex = hex2dec('A');
  sdo(sdoinc).datatype = 'si_uint8_T';
  sdo(sdoinc).value =  0; // Disable user callibration Channel 
  sdoinc = sdoinc +1;
  sdo(sdoinc).index = hex2dec('8000');
  sdo(sdoinc).subindex = hex2dec('B');
  sdo(sdoinc).datatype = 'si_uint8_T';
  sdo(sdoinc).value =  1; // Enable vendor callibration Channel 
  sdoinc = sdoinc +1;
  sdo(sdoinc).index = hex2dec('8000');
  sdo(sdoinc).subindex = hex2dec('19');
  sdo(sdoinc).datatype = 'si_uint16_T';
  sdo(sdoinc).value = RTDTYP1; 
  sdoinc = sdoinc +1
  sdo(sdoinc).index = hex2dec('8000');
  sdo(sdoinc).subindex = hex2dec('1A');
  sdo(sdoinc).datatype = 'si_uint16_T';
  sdo(sdoinc).value = COTYP1;
  sdoinc = sdoinc +1;
  sdo(sdoinc).index = hex2dec('8000');
  sdo(sdoinc).subindex = hex2dec('6');
  sdo(sdoinc).datatype = 'si_uint8_T'; 
  sdo(sdoinc).value = ENFI; // Enable Filter for all Channels
  sdoinc = sdoinc +1;
  sdo(sdoinc).index = hex2dec('8000');
  sdo(sdoinc).subindex = hex2dec('16');
  sdo(sdoinc).datatype = 'si_uint16_T'; 
  sdo(sdoinc).value = 0; // Default Filter Freq=50Hz
  sdoinc = sdoinc +1;

  [slave_sdoconfig,valid_sdoconfig]=build_sdostruct(sdo)

	
  //Set Default Slave
  slave_desc = getslavedesc('EtherCATInfo_el3xxx');	
  slave_revision = hex2dec('00100000');
  slave_typeid = getslavedesc_checkslave(slave_desc,slave_type,slave_revision);
  slave_config= getslavedesc_getconfig(slave_desc,slave_typeid);	
  [slave_smconfig,slave_pdoconfig,slave_pdoentry,valid_slave] = getslavedesc_buildopar(slave_config,0,0); //Default Configurartion
  slave_vendor = getslavedesc_vendor(slave_desc);
  slave_productcode = getslavedesc_productcode(slave_desc,slave_typeid);
   //Index subindex vectorlength valuetype bitlength Channelno Direction TypeCode Fullrange Scale Offset
  clear channel;
  channel(1).index = hex2dec('6000');
  channel(1).subindex = hex2dec('11');
  channel(1).vectorlength = 1;
  channel(1).valuetype = 'si_sint16_T';
  channel(1).typecode = 'raw';
  channel(1).direction = 'getvalues';
  channel(1).channelno = 1;
  channel(1).scale = 0.1;
  channel(1).offset = 0.0;
  channel(1).fullrange = 16;
  [slave_pdomapping,slave_pdomapping_scale,valid_mapping]=build_mapstruct(channel);	

  if valid_mapping < 0 then
	disp('No valid Mapping Configuration');
  end;
  slave_generic = int32([slave_vendor; slave_productcode]);
  model.opar = list(slave_smconfig,slave_pdoconfig,slave_pdoentry,slave_sdoconfig,slave_pdomapping,slave_generic, slave_pdomapping_scale);

  exprs=[STYP,sci2exp(ipar(2)),sci2exp(ipar(3)),sci2exp(ipar(4)),sci2exp(ipar(5)),sci2exp(SLST),sci2exp(STYPID),sci2exp(ENFI),sci2exp(RTDTYP1),sci2exp(COTYP1),sci2exp(RTDTYP2),sci2exp(COTYP2),sci2exp(RTDTYP3),sci2exp(COTYP3),sci2exp(RTDTYP4),sci2exp(COTYP4)]
  gr_i=['xstringb(orig(1),orig(2),[''Ethercat ''+STYP;''Master ''+string(MID)+'' Pos ''+string(SLP);''Alias ''+string(SLA)],sz(1),sz(2),''fill'');']

  x=standard_define([4 2],model,exprs,gr_i)
  x.graphics.id=["EL320X"]
end     	
endfunction
