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

function smtype = getslavedesc_getsmtype(slave_desc,slave_typeid,smindex)
  CByte = getslavedesc_SmCByte(slave_desc,slave_typeid,smindex);
  value = modulo(CByte,16);
  select value
     case 0 then
       smtype = 1; //Input Type z.B. EL1xxx, EL3xxx, TxPdo
       break
     case 4 then 
       smtype = 2; //Output Type z.B. El2xxx, El4xxx, RxPdo
       break
     else
     smtype = -1; //Unknown Error
  end
endfunction
