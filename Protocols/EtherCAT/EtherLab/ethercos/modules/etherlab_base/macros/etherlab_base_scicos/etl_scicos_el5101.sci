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

function [x,y,typ]=etl_scicos_el5101(job,arg1,arg2)
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
  ENABLEREGISTERRELOAD=evstr(exprs(8))
  ENABLEREGISTERRESET=evstr(exprs(9))
  ENABLEFWDCOUNT=evstr(exprs(10))
  GATETYP=evstr(exprs(11))
  FREQWINDOW=evstr(exprs(12))
  COUNTERRELOADVALUE=evstr(exprs(13))
  CONTROLINPUTPORT=evstr(exprs(14))
  PRESETINPUTPORT=evstr(exprs(15))
  COUNTEROUTPUTPORT=evstr(exprs(16))
  LATCHOUTPUTPORT=evstr(exprs(17))
  FREQOUTPUTPORT=evstr(exprs(18))
  PERIODOUTPUTPORT=evstr(exprs(19))
  WINDOWOUTPUTPORT=evstr(exprs(20))
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
   ENABLEREGISTERRELOAD=evstr(exprs(8))
   ENABLEREGISTERRESET=evstr(exprs(9))
   ENABLEFWDCOUNT=evstr(exprs(10))
   GATETYP=evstr(exprs(11))
   FREQWINDOW=evstr(exprs(12))
   COUNTERRELOADVALUE=evstr(exprs(13))
   CONTROLINPUTPORT=evstr(exprs(14))
   PRESETINPUTPORT=evstr(exprs(15))
   COUNTEROUTPUTPORT=evstr(exprs(16))
   LATCHOUTPUTPORT=evstr(exprs(17))
   FREQOUTPUTPORT=evstr(exprs(18))
   PERIODOUTPUTPORT=evstr(exprs(19))
   WINDOWOUTPUTPORT=evstr(exprs(20))

   [ln,fun]=where(); 
   if (fun(3) == "clickin") then
        slavetypes(1).name = 'EL5101'
        slavetypes(1).checkstate = 1;
        configs = [];
        configs = etl_slave_param_check(configs,'registerreset','Enable Register reset',ENABLEREGISTERRESET);
        configs = etl_slave_param_check(configs,'registerreload','Enable Register reload',ENABLEREGISTERRELOAD);
        configs = etl_slave_param_check(configs,'fwdcount','Enable Forwoard Counter',ENABLEFWDCOUNT);
        configs = etl_slave_param_edit(configs,'reloadvalue','Counter Reload Value',COUNTERRELOADVALUE);
        configs = etl_slave_param_edit(configs,'freqwindow','Frequency Window',FREQWINDOW);
        configs = etl_slave_param_radio(configs,'gatetyp','Select Gate Type',GATETYP,list('None',0),list('Positive',1),list('Negative',2));
        configs = etl_slave_param_check(configs,'presetport','Enable P-Reset Input',PRESETINPUTPORT);
        configs = etl_slave_param_check(configs,'controlport','Enable Control Input',CONTROLINPUTPORT);
        configs = etl_slave_param_check(configs,'counterport','Enable Counter Output',COUNTEROUTPUTPORT);
        configs = etl_slave_param_check(configs,'latchport','Enable Latch Output',LATCHOUTPUTPORT);
        configs = etl_slave_param_check(configs,'freqport','Enable Frequency Output',FREQOUTPUTPORT);
        configs = etl_slave_param_check(configs,'periodport','Enable Period Output',PERIODOUTPUTPORT);
        configs = etl_slave_param_check(configs,'windowport','Enable Window Output',WINDOWOUTPUTPORT);
        [STYP,STYPID,MID,DID,SLA,SLP,SLST,configs] = etl_slave_param('Configure EL5101',slavetypes,STYP,STYPID,MID,DID,SLA,SLP,SLST,configs);
        [ENABLEREGISTERRESET,b] = etl_slave_param_get(configs,'registerreset');
        [ENABLEREGISTERRELOAD,b] = etl_slave_param_get(configs,'registerreload');
        [ENABLEFWDCOUNT,b] = etl_slave_param_get(configs,'fwdcount');
        [COUNTERRELOADVALUE,b] = etl_slave_param_get(configs,'reloadvalue');
        [FREQWINDOW,b] = etl_slave_param_get(configs,'freqwindow');
        [GATETYP,b] = etl_slave_param_get(configs,'gatetyp');
        [PRESETINPUTPORT,b] = etl_slave_param_get(configs,'presetport');
        [CONTROLINPUTPORT,b] = etl_slave_param_get(configs,'controlport');
        [COUNTEROUTPUTPORT,b] = etl_slave_param_get(configs,'counterport');
        [LATCHOUTPUTPORT,b] = etl_slave_param_get(configs,'latchport');
        [FREQOUTPUTPORT,b] = etl_slave_param_get(configs,'freqport');
        [PERIODOUTPUTPORT,b] = etl_slave_param_get(configs,'periodport');
        [WINDOWOUTPUTPORT,b] = etl_slave_param_get(configs,'windowport');
        loop=%f;
    else
       loop = %f;
   end
   //SDO Config
   //Datatype Index Subindex Value
   slave_sdoconfig = int32([]);
   valid_slave = %f; //Flag for Loop handling
   slave_desc = getslavedesc('EtherCATInfo_el5xxx');

   //Clear Channel List
   clear channel
   clear sdo
   sdoinc = 1;
   


   //Generell SDO Configurations
   sdo(sdoinc).index = hex2dec('8000');
   sdo(sdoinc).subindex = 1;  
   sdo(sdoinc).datatype = 'si_uint8_T';
   sdo(sdoinc).value = ENABLEREGISTERRELOAD;
   sdoinc = sdoinc +1;
   sdo(sdoinc).index = hex2dec('8000');
   sdo(sdoinc).subindex = 2;  
   sdo(sdoinc).datatype = 'si_uint8_T';
   sdo(sdoinc).value = ENABLEREGISTERRESET;
   sdoinc = sdoinc +1;
   sdo(sdoinc).index = hex2dec('8000');
   sdo(sdoinc).subindex = 3;  
   sdo(sdoinc).datatype = 'si_uint8_T';
   sdo(sdoinc).value = ENABLEFWDCOUNT;
   sdoinc = sdoinc +1;
   sdo(sdoinc).index = hex2dec('8000');
   sdo(sdoinc).subindex = 4;  
   sdo(sdoinc).datatype = 'si_uint8_T';
   sdo(sdoinc).value = bool2s(GATETYP==1);//Positive Gate
   sdoinc = sdoinc +1;
   sdo(sdoinc).index = hex2dec('8000');
   sdo(sdoinc).subindex = 5;  
   sdo(sdoinc).datatype = 'si_uint8_T';
   sdo(sdoinc).value = bool2s(GATETYP==2);//Negative Gate
   sdoinc = sdoinc +1;
   sdo(sdoinc).index = hex2dec('8001');
   sdo(sdoinc).subindex = 1;  
   sdo(sdoinc).datatype = 'si_uint16_T';
   sdo(sdoinc).value = FREQWINDOW;
   sdoinc = sdoinc +1;
   sdo(sdoinc).index = hex2dec('8001');
   sdo(sdoinc).subindex = 2;  
   sdo(sdoinc).datatype = 'si_uint16_T';
   sdo(sdoinc).value = COUNTERRELOADVALUE;
   sdoinc = sdoinc +1;



   if STYP == 'EL5101' then
	slave_revision = hex2dec('00010000');	
       	slave_type = 'EL5101';    
	
	numoutports = SLST+COUNTEROUTPUTPORT+LATCHOUTPUTPORT+FREQOUTPUTPORT+PERIODOUTPUTPORT+WINDOWOUTPUTPORT;
	numinports = CONTROLINPUTPORT+PRESETINPUTPORT;



   	//Block Configuration
   	slave_outputs = ones(numoutports,2); //[rows input 1 colums input 1;...];
   	slave_output_types = ones(numoutports,1); //all real [Type Input 1; Type Input 2]
   	slave_inputs = ones(numinports,2);
  	slave_input_types = ones(numinports,1);


	portno=1; //Portnumbercounter

	//First all Outputs

	if COUNTEROUTPUTPORT == 1 then
	  channel(portno).index = hex2dec('6000');
  	  channel(portno).subindex = hex2dec('2');
  	  channel(portno).vectorlength = 1;
  	  channel(portno).valuetype = 'si_uint16_T';
  	  channel(portno).typecode = 'raw';
  	  channel(portno).direction = 'getvalues';
  	  channel(portno).channelno = portno;
  	  channel(portno).scale = 1.0;
  	  channel(portno).offset = 0.0;
  	  channel(portno).fullrange = 16;
	  portno = portno + 1;
	end	

	if LATCHOUTPUTPORT == 1 then
	  channel(portno).index = hex2dec('6000');
  	  channel(portno).subindex = hex2dec('3');
  	  channel(portno).vectorlength = 1;
  	  channel(portno).valuetype = 'si_uint16_T';
  	  channel(portno).typecode = 'raw';
  	  channel(portno).direction = 'getvalues';
  	  channel(portno).channelno = portno;
  	  channel(portno).scale = 1.0;
  	  channel(portno).offset = 0.0;
  	  channel(portno).fullrange = 16;
	  portno = portno + 1;
	end	

	if FREQOUTPUTPORT == 1 then
	  channel(portno).index = hex2dec('6000');
  	  channel(portno).subindex = hex2dec('4');
  	  channel(portno).vectorlength = 1;
  	  channel(portno).valuetype = 'si_uint32_T';
  	  channel(portno).typecode = 'raw';
  	  channel(portno).direction = 'getvalues';
  	  channel(portno).channelno = portno;
  	  channel(portno).scale = 0.01;
  	  channel(portno).offset = 0.0;
  	  channel(portno).fullrange = 32;
	  portno = portno + 1;
	end	

	if PERIODOUTPUTPORT == 1 then
	  channel(portno).index = hex2dec('6000');
  	  channel(portno).subindex = hex2dec('5');
  	  channel(portno).vectorlength = 1;
  	  channel(portno).valuetype = 'si_uint16_T';
  	  channel(portno).typecode = 'raw';
  	  channel(portno).direction = 'getvalues';
  	  channel(portno).channelno = portno;
  	  channel(portno).scale = 0.5; //Umwandlung in nues
  	  channel(portno).offset = 0.0;
  	  channel(portno).fullrange = 16;
	  portno = portno + 1;
	end	

	if WINDOWOUTPUTPORT == 1 then
	  channel(portno).index = hex2dec('6000');
  	  channel(portno).subindex = hex2dec('5');
  	  channel(portno).vectorlength = 1;
  	  channel(portno).valuetype = 'si_uint16_T';
  	  channel(portno).typecode = 'raw';
  	  channel(portno).direction = 'getvalues';
  	  channel(portno).channelno = portno;
  	  channel(portno).scale = 1.0; //Anzahl Ticks im Zeitfenter
  	  channel(portno).offset = 0.0;
  	  channel(portno).fullrange = 16;
	  portno = portno + 1;
	end	


	if SLST == 1 then
	  channel(portno).index = hex2dec('6000');
  	  channel(portno).subindex = hex2dec('1');
  	  channel(portno).vectorlength = 1;
  	  channel(portno).valuetype = 'si_uint8_T';
  	  channel(portno).typecode = 'analog';
  	  channel(portno).direction = 'getvalues';
  	  channel(portno).channelno = portno;
  	  channel(portno).scale = 256.0;
  	  channel(portno).offset = 0.0;
  	  channel(portno).fullrange = 8;
	  portno = portno + 1;
	end


	//All Input Ports
	inputportno=1	

	if CONTROLINPUTPORT == 1 then
	  channel(portno).index = hex2dec('7000');
  	  channel(portno).subindex = hex2dec('1');
  	  channel(portno).vectorlength = 1;
  	  channel(portno).valuetype = 'si_uint8_T';
  	  channel(portno).typecode = 'raw';
  	  channel(portno).direction = 'setvalues';
  	  channel(portno).channelno = inputportno;
  	  channel(portno).scale = 1.0; 
  	  channel(portno).offset = 0.0;
  	  channel(portno).fullrange = 8;
	  portno = portno + 1;
	  inputportno = inputportno + 1;
	end	

	if PRESETINPUTPORT == 1 then
	  channel(portno).index = hex2dec('7000');
  	  channel(portno).subindex = hex2dec('2');
  	  channel(portno).vectorlength = 1;
  	  channel(portno).valuetype = 'si_uint16_T';
  	  channel(portno).typecode = 'raw';
  	  channel(portno).direction = 'setvalues';
  	  channel(portno).channelno = inputportno;
  	  channel(portno).scale = 1.0;
  	  channel(portno).offset = 0.0;
  	  channel(portno).fullrange = 16;
	  portno = portno + 1;
	  inputportno = inputportno + 1;
	end	
      
	[slave_sdoconfig,valid_sdoconfig]=build_sdostruct(sdo)
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
     exprs(8)=sci2exp(ENABLEREGISTERRELOAD);
     exprs(9)=sci2exp(ENABLEREGISTERRESET);
     exprs(10)=sci2exp(ENABLEFWDCOUNT);
     exprs(11)=sci2exp(GATETYP);
     exprs(12)=sci2exp(FREQWINDOW);
     exprs(13)=sci2exp(COUNTERRELOADVALUE);
     exprs(14)=sci2exp(CONTROLINPUTPORT);
     exprs(15)=sci2exp(PRESETINPUTPORT);
     exprs(16)=sci2exp(COUNTEROUTPUTPORT);
     exprs(17)=sci2exp(LATCHOUTPUTPORT);
     exprs(18)=sci2exp(FREQOUTPUTPORT);
     exprs(19)=sci2exp(PERIODOUTPUTPORT);
     exprs(20)=sci2exp(WINDOWOUTPUTPORT);

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
  STYP='EL5101';
  STYPID=0;
  //Default Configuration
  ENABLEREGISTERRELOAD=0;
  ENABLEREGISTERRESET=0;
  ENABLEFWDCOUNT=0;
  GATETYP=0; //Type None
  FREQWINDOW=100; //Default
  COUNTERRELOADVALUE=65535; //Default
  CONTROLINPUTPORT=0;
  PRESETINPUTPORT=0;
  COUNTEROUTPUTPORT=1;
  LATCHOUTPUTPORT=0;
  FREQOUTPUTPORT=0;
  PERIODOUTPUTPORT=0;
  WINDOWOUTPUTPORT=0;

  slave_type = 'EL5101';
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

 //Generell SDO Configurations
 sdo(sdoinc).index = hex2dec('8000');
 sdo(sdoinc).subindex = 1;  
 sdo(sdoinc).datatype = 'si_uint8_T';
 sdo(sdoinc).value = ENABLEREGISTERRELOAD;
 sdoinc = sdoinc +1;
 sdo(sdoinc).index = hex2dec('8000');
 sdo(sdoinc).subindex = 2;  
 sdo(sdoinc).datatype = 'si_uint8_T';
 sdo(sdoinc).value = ENABLEREGISTERRESET;
 sdoinc = sdoinc +1;
 sdo(sdoinc).index = hex2dec('8000');
 sdo(sdoinc).subindex = 3;  
 sdo(sdoinc).datatype = 'si_uint8_T';
 sdo(sdoinc).value = ENABLEFWDCOUNT;
 sdoinc = sdoinc +1;
 sdo(sdoinc).index = hex2dec('8000');
 sdo(sdoinc).subindex = 4;  
 sdo(sdoinc).datatype = 'si_uint8_T';
 sdo(sdoinc).value = bool2s(GATETYP==1);//Positive Gate
 sdoinc = sdoinc +1;
 sdo(sdoinc).index = hex2dec('8000');
 sdo(sdoinc).subindex = 5;  
 sdo(sdoinc).datatype = 'si_uint8_T';
 sdo(sdoinc).value = bool2s(GATETYP==2);//Negative Gate
 sdoinc = sdoinc +1;
 sdo(sdoinc).index = hex2dec('8001');
 sdo(sdoinc).subindex = 1;  
 sdo(sdoinc).datatype = 'si_uint16_T';
 sdo(sdoinc).value = FREQWINDOW;
 sdoinc = sdoinc +1;
 sdo(sdoinc).index = hex2dec('8001');
 sdo(sdoinc).subindex = 2;  
 sdo(sdoinc).datatype = 'si_uint16_T';
 sdo(sdoinc).value = COUNTERRELOADVALUE;
 sdoinc = sdoinc +1;

  [slave_sdoconfig,valid_sdoconfig]=build_sdostruct(sdo)

   if valid_sdoconfig < 0 then
	disp('No valid SDO Configuration');
   end;

	
  //Set Default Slave
  slave_desc = getslavedesc('EtherCATInfo_el5xxx');	
  slave_revision = hex2dec('00010000');
  slave_typeid = getslavedesc_checkslave(slave_desc,slave_type,slave_revision);
  slave_config= getslavedesc_getconfig(slave_desc,slave_typeid);	
  [slave_smconfig,slave_pdoconfig,slave_pdoentry,valid_slave] = getslavedesc_buildopar(slave_config,0,0); //Default Configurartion
  slave_vendor = getslavedesc_vendor(slave_desc);
  slave_productcode = getslavedesc_productcode(slave_desc,slave_typeid);
   //Index subindex vectorlength valuetype bitlength Channelno Direction TypeCode Fullrange Scale Offset
  clear channel;


  portno=1; //Portnumbercounter

  //Default only Counteroutput
  channel(portno).index = hex2dec('6000');
  channel(portno).subindex = hex2dec('2');
  channel(portno).vectorlength = 1;
  channel(portno).valuetype = 'si_uint16_T';
  channel(portno).typecode = 'raw';
  channel(portno).direction = 'getvalues';
  channel(portno).channelno = portno;
  channel(portno).scale = 1.0;
  channel(portno).offset = 0.0;
  channel(portno).fullrange = 16;
  portno = portno + 1;	


  [slave_pdomapping,slave_pdomapping_scale,valid_mapping]=build_mapstruct(channel);	

  if valid_mapping < 0 then
	disp('No valid Mapping Configuration');
  end;
  slave_generic = int32([slave_vendor; slave_productcode]);
  model.opar = list(slave_smconfig,slave_pdoconfig,slave_pdoentry,slave_sdoconfig,slave_pdomapping,slave_generic, slave_pdomapping_scale);

  exprs=[STYP,sci2exp(ipar(2)),sci2exp(ipar(3)),sci2exp(ipar(4)),sci2exp(ipar(5)),sci2exp(SLST),sci2exp(STYPID), sci2exp(ENABLEREGISTERRELOAD), sci2exp(ENABLEREGISTERRESET), sci2exp(ENABLEFWDCOUNT), sci2exp(GATETYP), sci2exp(FREQWINDOW), sci2exp(COUNTERRELOADVALUE), sci2exp(CONTROLINPUTPORT), sci2exp(PRESETINPUTPORT), sci2exp(COUNTEROUTPUTPORT), sci2exp(LATCHOUTPUTPORT), sci2exp(FREQOUTPUTPORT), sci2exp(PERIODOUTPUTPORT), sci2exp(WINDOWOUTPUTPORT)]
  gr_i=['xstringb(orig(1),orig(2),[''Ethercat ''+STYP;''Master ''+string(MID)+'' Pos ''+string(SLP);''Alias ''+string(SLA)],sz(1),sz(2),''fill'');']

  x=standard_define([4 2],model,exprs,gr_i)
  x.graphics.id=["EL5101"]
end     	
endfunction
