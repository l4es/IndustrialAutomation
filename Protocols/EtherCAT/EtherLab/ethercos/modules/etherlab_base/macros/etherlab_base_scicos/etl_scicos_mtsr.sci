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

function [x,y,typ]=etl_scicos_mtsr(job,arg1,arg2)
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
  SENSLEN=evstr(exprs(8))
  OFFSET=evstr(exprs(9))
  DOREVERT=evstr(exprs(10))
  VELOCITYOUTPUT=evstr(exprs(11))
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
   SENSLEN=evstr(exprs(8))
   OFFSET=evstr(exprs(9))
   DOREVERT=evstr(exprs(10))
   VELOCITYOUTPUT=evstr(exprs(11))


   [ln,fun]=where(); 
   if (fun(3) == "clickin") then
        slavetypes(1).name = '1-Magnet'
        slavetypes(1).checkstate = 1;
// Todo Vector Input Output
//        slavetypes(2).name = '2-Magnet'
//        slavetypes(2).checkstate = 1;
//        slavetypes(3).name = '3-Magnet'
//        slavetypes(3).checkstate = 1;
//        slavetypes(4).name = '4-Magnet'
//        slavetypes(4).checkstate = 1;
//        slavetypes(5).name = '5-Magnet'
//        slavetypes(5).checkstate = 1;
        configs = [];
        configs = etl_slave_param_edit(configs,'senslen','Sensor Length',SENSLEN);
        configs = etl_slave_param_edit(configs,'offset','Length Offset',OFFSET);
        configs = etl_slave_param_check(configs,'dorevert','Revert Direction',DOREVERT);
        configs = etl_slave_param_check(configs,'velocityport','Enable Velocity Output',VELOCITYOUTPUT);

        [STYP,STYPID,MID,DID,SLA,SLP,SLST,configs] = etl_slave_param('Configure MTS-R-Series',slavetypes,STYP,STYPID,MID,DID,SLA,SLP,SLST,configs);
        [SENSLEN,b] = etl_slave_param_get(configs,'senslen');
        [OFFSET,b] = etl_slave_param_get(configs,'offset');
        [DOREVERT,b] = etl_slave_param_get(configs,'dorevert');
        [VELOCITYOUTPUT,b] = etl_slave_param_get(configs,'velocityport');
        loop=%f;
    else
       loop = %f;
    end

    //SDO Config
    //Datatype Index Subindex Value
    slave_sdoconfig = int32([]);
    valid_slave = %f; //Flag for Loop handling
    slave_desc = getslavedesc('EtherCATInfo_mts');

    //Clear Channel List
    clear channel
    clear sdo
    sdoinc = 1;
   


    //Generell SDO Configurations
    sdo(sdoinc).index = hex2dec('200A');
    sdo(sdoinc).subindex = 0;  
    sdo(sdoinc).datatype = 'si_uint32_T';
    sdo(sdoinc).value = DOREVERT;
    sdoinc = sdoinc +1;


    slave_type = 'MTS Temposonics';
    slave_revision = hex2dec('3020101');

    numoutports = 1+SLST+VELOCITYOUTPUT;
    numoutportssize = (STYPID+1);

    //Block Configuration
    slave_outputs = ones(numoutports,2); //[rows input 1 colums input 1;...];
    slave_output_types = ones(numoutports,1); //all real [Type Input 1; Type Input 2]
    slave_inputs = [];
    slave_input_types = [];


    portno=1; //Portnumbercounter

    //First all Outputs

    //Position Output
    channel(portno).index = hex2dec('3101');
    channel(portno).subindex = hex2dec('2');
    channel(portno).vectorlength = 1;
    channel(portno).valuetype = 'si_uint32_T';
    channel(portno).typecode = 'analog';
    channel(portno).direction = 'getvalues';
    channel(portno).channelno = portno;
    channel(portno).scale = SENSLEN;
    channel(portno).offset = OFFSET;
    channel(portno).fullrange = 32;
    portno = portno + 1;	

    if VELOCITYOUTPUT == 1 then
     channel(portno).index = hex2dec('3101');
     channel(portno).subindex = hex2dec('3');
     channel(portno).vectorlength = 1;
     channel(portno).valuetype = 'si_uint32_T';
     channel(portno).typecode = 'analog';
     channel(portno).direction = 'getvalues';
     channel(portno).channelno = portno;
     channel(portno).scale = 1.0; // m/s
     channel(portno).offset = 0.0;
     channel(portno).fullrange = 30; // Bug in MTS, seems to be 32 Bit
     portno = portno + 1;
    end	


   if SLST == 1 then
    channel(portno).index = hex2dec('3101');
    channel(portno).subindex = hex2dec('1');
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

      
   [slave_sdoconfig,valid_sdoconfig]=build_sdostruct(sdo)
   [slave_pdomapping,slave_pdomapping_scale,valid_mapping]=build_mapstruct(channel);




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
     exprs(8)=sci2exp(SENSLEN);
     exprs(9)=sci2exp(OFFSET);
     exprs(10)=sci2exp(DOREVERT);
     exprs(11)=sci2exp(VELOCITYOUTPUT);


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
 end //while
case 'define' then
  model=scicos_model()
  model.sim=list('etl_scicos',4) // name of the c-function
  DEBUG=0;//No Debuging
  MID=0;
  DID=0;
  SLA=0;
  SLP=0;
  SLST=0;
  STYP='1-Magnet';
  STYPID=0;
  //Default Configuration
  SENSLEN=100; //in mm
  OFFSET=0; //in mm
  DOREVERT=0;
  VELOCITYOUTPUT=0;

  slave_type = 'MTS Temposonics';
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
 sdo(sdoinc).index = hex2dec('200A');
 sdo(sdoinc).subindex = 0;  
 sdo(sdoinc).datatype = 'si_uint32_T';
 sdo(sdoinc).value = DOREVERT;
 sdoinc = sdoinc +1;


  [slave_sdoconfig,valid_sdoconfig]=build_sdostruct(sdo)

   if valid_sdoconfig < 0 then
	disp('No valid SDO Configuration');
   end;

	
  //Set Default Slave
  slave_desc = getslavedesc('EtherCATInfo_mts');	
  slave_revision = hex2dec('3020101');
  slave_typeid = getslavedesc_checkslave(slave_desc,slave_type,slave_revision);
  slave_config= getslavedesc_getconfig(slave_desc,slave_typeid);	
  [slave_smconfig,slave_pdoconfig,slave_pdoentry,valid_slave] = getslavedesc_buildopar(slave_config,0,0); //Default Configurartion
  slave_vendor = getslavedesc_vendor(slave_desc);
  slave_productcode = getslavedesc_productcode(slave_desc,slave_typeid);
   //Index subindex vectorlength valuetype bitlength Channelno Direction TypeCode Fullrange Scale Offset
  clear channel;


  portno=1; //Portnumbercounter

  //Default only Positionoutput
  channel(portno).index = hex2dec('3101');
  channel(portno).subindex = hex2dec('2');
  channel(portno).vectorlength = 1;
  channel(portno).valuetype = 'si_uint32_T';
  channel(portno).typecode = 'analog';
  channel(portno).direction = 'getvalues';
  channel(portno).channelno = portno;
  channel(portno).scale = SENSLEN;
  channel(portno).offset = OFFSET;
  channel(portno).fullrange = 32;
  portno = portno + 1;	


  [slave_pdomapping,slave_pdomapping_scale,valid_mapping]=build_mapstruct(channel);	

  if valid_mapping < 0 then
	disp('No valid Mapping Configuration');
  end;
  slave_generic = int32([slave_vendor; slave_productcode]);
  model.opar = list(slave_smconfig,slave_pdoconfig,slave_pdoentry,slave_sdoconfig,slave_pdomapping,slave_generic, slave_pdomapping_scale);

  exprs=[STYP,sci2exp(ipar(2)),sci2exp(ipar(3)),sci2exp(ipar(4)),sci2exp(ipar(5)),sci2exp(SLST),sci2exp(STYPID), sci2exp(SENSLEN), sci2exp(OFFSET), sci2exp(DOREVERT), sci2exp(VELOCITYOUTPUT)]
  gr_i=['xstringb(orig(1),orig(2),[''MTS R-Series'';''Ethercat ''+STYP;''Master ''+string(MID)+'' Pos ''+string(SLP);''Alias ''+string(SLA)],sz(1),sz(2),''fill'');']

  x=standard_define([4 3],model,exprs,gr_i)
  x.graphics.id=["MTS-R-Series"]
end     	
endfunction
