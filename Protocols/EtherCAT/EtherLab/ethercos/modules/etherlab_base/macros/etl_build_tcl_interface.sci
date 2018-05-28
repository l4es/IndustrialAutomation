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

function [repeat, baseinit, configoptions] = etl_build_tcl_interface(dialogname,slavetypes,baseinit,configoptions)
  
  repeat = %f;

mode(1);
  statecheck = [
  'proc check_stateport {} { global base1; global base2; global .eltop; global slavestring; global slavestate; global base7;  set base1 [.eltop.list get [.eltop.list curselection]]; set base2 [.eltop.list curselection]; set selstate  $slavestate([.eltop.list curselection]); if {$selstate == 1} then { .eltop.showstatus configure -state normal } else {.eltop.showstatus configure -state disable; set base7 0;}; }'
      ];
  
  slavetypestr=[];
  statestr=[];
  for i=1:max(size(slavetypes))
      slavetypestr= slavetypestr +' '+ string(i-1)+' '+slavetypes(i).name;
      statestr = statestr + ' '+ string(i-1)+' '+string(slavetypes(i).checkstate);
 end


 
initvalues=[
		statecheck;
            	'array set slavestring {'+slavetypestr+'}' 
		'array set slavestate {'+statestr+'}'; 
		'set base1 '+baseinit(1).value;    //slaveselected
		'set base2 '+string(baseinit(2).value); //selectid
		'set base3 '+string(baseinit(3).value); //MID
		'set base4 '+string(baseinit(4).value); //DID
		'set base5 '+string(baseinit(5).value); //SLA
		'set base6 '+string(baseinit(6).value); //SLP
		'set base7 '+string(baseinit(7).value); //Showstate
  ];  

  
baselayout=[
            'set okstate 0'
            'set cancelstate 0'
            'set eltop .eltop'
            'toplevel .eltop'
            'wm title .eltop '"'+dialogname+''"'
            'label .eltop.ellabel -text '"Selected Slavetype:'" -justify center'
            'entry .eltop.showslave -textvariable base1 -justify center'
            'scrollbar .eltop.h -orient horizontal -command '".eltop.list xview'"'
            'scrollbar .eltop.v -command '".eltop.list yview'"'
            'listbox .eltop.list -selectmode single -width 20 -height 10 -setgrid 1 -xscroll '".eltop.h set'" -yscroll '".eltop.v set'"'
            'checkbutton .eltop.showstatus -text {Enable Statusport} -justify center -variable base7' 
            'label .eltop.masteridlabel -text '"Master ID:'" -justify right'
            'entry .eltop.masteridentry -textvariable base3 -justify left'
            'label .eltop.domainidlabel -text '"Domain ID:'" -justify right'
            'entry .eltop.domainidentry -textvariable base4 -justify left'
            'label .eltop.slavealiaslabel -text '"Slave Alias:'" -justify right'
            'entry .eltop.slavealiasentry -textvariable base5 -justify left'
            'label .eltop.slavepositionlabel -text '"Slave Position:'" -justify right'
            'entry .eltop.slavepositionentry -textvariable base6 -justify left'
            'button .eltop.setok -text '"Ok'" -justify center -command {global okstate; set okstate 1}'
            'button .eltop.setcancel -text '"Cancel'" -justify center -command {global okstate; global cancelstate; set okstate 0; set cancelstate 1}'
       	    'frame .eltop.frameslaveconfig  -width 0.5i -height 0.5i -relief ridge -bd 2'
  ];
  

  gridlayout = [
            'grid .eltop.ellabel -row 0 -column 0 -sticky '"ew'"';
            'grid .eltop.showslave -row 0 -column 1 -sticky '"ew'"';
           'grid .eltop.list -row 1 -column 0 -rowspan 8 -columnspan 2 -sticky '"nsew'"';
           'grid .eltop.v -row 1 -column 2 -rowspan 8 -sticky '"ns'"';
            'grid .eltop.h -row 9 -column 0 -columnspan 2 -sticky '"ew'"';
            'grid .eltop.masteridlabel -row 1 -column 3';
            'grid .eltop.masteridentry -row 1 -column 4 -sticky '"ew'"'; 
            'grid .eltop.domainidlabel -row 2 -column 3';  
            'grid .eltop.domainidentry -row 2 -column 4 -sticky '"ew'"'; 
            'grid .eltop.slavealiaslabel -row 3 -column 3'; 
            'grid .eltop.slavealiasentry -row 3 -column 4 -sticky '"ew'"'; 
            'grid .eltop.slavepositionlabel -row 4 -column 3'; 
            'grid .eltop.slavepositionentry -row 4 -column 4 -sticky '"ew'"'; 
            'grid .eltop.showstatus -row 5 -column 4 -columnspan 2 -sticky '"w'"';
            'grid .eltop.setok -row 7 -column 3'; 
            'grid .eltop.setcancel -row 7 -column 4';
            'grid columnconfigure .eltop 0 -weight 1';
            'grid rowconfigure .eltop 0 -weight 1';
	      'grid .eltop.frameslaveconfig -row 10 -column 0 -columnspan 5 -sticky '"w'" ';
  ];

  setdefaultvalue=[];  
  slavelayout = [];
  slavegrid = [];
  for i=1:max(size(configoptions))
	select configoptions(i).valuetype
		case 'edit' then
			setdefaultvalue=[
				setdefaultvalue;
				'set var'+string(i)+' '+string(configoptions(i).value);			
			];
			slavelayout = [
				slavelayout;
				'entry .eltop.frameslaveconfig.entry'+string(i)+' -textvariable var'+string(i)+' -justify left';
            			'label .eltop.frameslaveconfig.label'+string(i)+' -text '"'+configoptions(i).text+''" -justify right';
				];
			gridlayout = [
				gridlayout;
				'grid .eltop.frameslaveconfig.entry'+string(i)+' -row '+string(i)+' -column 0 -sticky '"e'"';	
				'grid .eltop.frameslaveconfig.label'+string(i)+' -row '+string(i)+' -column 1 -sticky '"w'"';
				];
		case 'check' then
			setdefaultvalue=[
				setdefaultvalue;
				'set var'+string(i)+' '+string(configoptions(i).value);			
			];
			slavelayout = [
				slavelayout;
				'checkbutton .eltop.frameslaveconfig.check'+string(i)+' -variable var'+string(i)+' -justify right';
            			'label .eltop.frameslaveconfig.label'+string(i)+' -text '"'+configoptions(i).text+''" -justify right';
				];
			gridlayout = [
				gridlayout;
				'grid .eltop.frameslaveconfig.check'+string(i)+' -row '+string(i)+' -column 0 -sticky '"e'"';	
				'grid .eltop.frameslaveconfig.label'+string(i)+' -row '+string(i)+' -column 1 -sticky '"w'"';
				];
		case 'radio' then
			slavelayout = [
				slavelayout;
				'radiobutton .eltop.frameslaveconfig.rb'+string(i)+' -text '"'+configoptions(i).text+''" -variable '+configoptions(i).varname+' -value '+string(configoptions(i).value)+' -justify left'; 
				];
			gridlayout = [
				gridlayout;
				'grid .eltop.frameslaveconfig.rb'+string(i)+' -row '+string(i)+' -column 2 -sticky '"w'"';	
				];
		case 'radiodefault' then
			setdefaultvalue=[
				setdefaultvalue;
				'set '+configoptions(i).varname+' '+string(configoptions(i).value);		
			];

		case 'label' then	
			slavelayout = [
				slavelayout;
				'label .eltop.frameslaveconfig.label'+string(i)+' -text '"'+configoptions(i).text+''" -justify left';
				];
			gridlayout = [
				gridlayout;
				'grid .eltop.frameslaveconfig.label'+string(i)+' -row '+string(i)+' -column 0 -columnspan 2 -sticky '"ew'"';
				];
	end

  end

  listenentrys=[];
  for i=1:max(size(slavetypes))
    listenentrys=[
             listenentrys;
		'.eltop.list insert end $slavestring('+string(i-1)+')'      
    ];    
  end

  
  listenentrys = [
            listenentrys
            'bind .eltop.list <ButtonRelease-1> '"check_stateport '"'
            'raise .eltop'
  ];





//Build TCL Scipt
tclscript = [
	statecheck;
	initvalues;
	setdefaultvalue;
	baselayout;	
	slavelayout;
	gridlayout;
	listenentrys;
	];



  //disp(tclscript);
  //fd = mopen('test.tcl','wb');
  //mputl(tclscript,fd);
  //mclose(fd);

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

  baseinit(1).value = TCL_GetVar('base1');
  baseinit(2).value = check_pos_integer('Dummy','base2');
 
  for i=3:max(size(baseinit))
    varvalue = check_pos_integer(baseinit(i).name,'base'+string(i));
    if varvalue < 0 then
      repeat = %t;    
    else
      baseinit(i).value = varvalue;
    end
  end

  for i=1:max(size(configoptions))
    select configoptions(i).valuetype
      case 'radiodefault' then
        varvalue = check_pos_integer('Dummy Radiobutton',configoptions(i).varname);
        if varvalue < 0 then
          repeat = %t;    
        else
          configoptions(i).value = varvalue;
        end
      case 'edit' then
        varvalue = check_pos_integer(configoptions(i).text,'var'+string(i));
        if varvalue < 0 then
          repeat = %t;    
        else
          configoptions(i).value = varvalue;
        end
      case 'check' then
        varvalue = check_pos_integer(configoptions(i).text,'var'+string(i));
        if varvalue < 0 then
          repeat = %t;    
        else
          configoptions(i).value = varvalue;
        end      
    end
  end


  TCL_EvalStr('destroy $eltop');

  endfunction
