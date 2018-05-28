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

function [x,y,typ] = etl_scicos_realtime(job,arg1,arg2)
  x=[];y=[];typ=[];
  select job
  case 'plot' then
    exprs=arg1.graphics.exprs;
    standard_draw(arg1)
  case 'getinputs' then
    [x,y,typ]=standard_inputs(arg1)
  case 'getoutputs' then
    [x,y,typ]=standard_outputs(arg1)
  case 'getorigin' then
    [x,y]=standard_origin(arg1)
  case 'set' then
    x=arg1
    model=arg1.model;graphics=arg1.graphics;
    exprs=graphics.exprs;
    while %t do
       [ok,RT,TA,exprs]=getvalue(..
	'Select properties:',..  
	[' RT scheduler (0-> no RT sch., 1-> use RT sch.) ',..
           'sampletime in ms:'],..
	   list('vec',-1,'vec',-1),exprs);
      if ~ok then break,end
      outport=3;
      if exists('outport') then out=ones(outport,1), in=[], else out=1, in=[], end
      [model,graphics,ok]=check_io(model,graphics,in,out,1,[])
      if ok then
        graphics.exprs=exprs;
        model.ipar=[RT;TA];
        model.rpar=[];
        model.dstate=[0];
        x.graphics=graphics;x.model=model
        break
      end
    end
  case 'define' then
    RT=1;
    TA=100;
    model=scicos_model()
    model.sim=list('etl_scicos_realtime',4)
    outport=3;
    if exists('outport') then model.out=ones(outport,1), model.in=[], else model.out=1, model.in=[], end
    model.evtin=1
    model.rpar=[]
    model.ipar=[RT;TA];
    model.dstate=[];
    model.blocktype='d'
    model.dep_ut=[%t %f]
    exprs=[sci2exp(RT),sci2exp(TA)]
    gr_i=['xstringb(orig(1),orig(2),[''RT_SYNC''],sz(1)*0.5,sz(2),''fill'');',
	'xstringb(orig(1)+sz(1)*0.8,orig(2),[''time'';''TA'';''Puffer''],sz(1)/10,sz(2),''fill'');']
    x=standard_define([4 2],model,exprs,gr_i)
  end
endfunction
