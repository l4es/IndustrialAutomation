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

function [configs] = etl_slave_param_radio(configs,vname,label,value,varargin)
  [lhs,rhs]=argn(0);
  if rhs>=1 then
    disp(varargin)
  else //Failure
    return;
  end
  index = max(size(configs))+1;
  
  //Set Label of Radiogroup
  configs(index).valuetype = 'label';
  configs(index).text = label;
  configs(index).vname = ' '; //Dummy Name
  //now the following Radiobuttons
  index = index+1;
  //Set TCL Variablename
  varname = 'radio'+string(index);
  for i = 1:length(varargin)
    entry = varargin(i);
    configs(index).text = entry(1);
    configs(index).varname = varname;
    configs(index).valuetype = 'radio';
    configs(index).value = entry(2); //Setvalue for this Button
    configs(index).vname = ' '; //Dummy Name
    index = index +1;
  end
  //now the Variable of the Radiobutton
  configs(index).valuetype = 'radiodefault';
  configs(index).varname = varname;
  configs(index).value = value; //Set actual Value
  configs(index).vname = vname; //User defined Name , to access the value
endfunction
