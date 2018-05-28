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

function varvalue = check_pos_integer(name,varname)
     tmp_string = TCL_GetVar(varname);	
     flag = and(asciimat(tmp_string)>=47&asciimat(tmp_string)<=58);
     varvalue = -1;
     messageflag = %f;
     if flag then
     	varvalue = sscanf(tmp_string,'%i');
     	if varvalue < 0 then
         messageflag = %t;
      end
     else
      messageflag = %t; 
     end
     if messageflag then
         message([name +' must be zero or a positive integer!']);
     end
endfunction
