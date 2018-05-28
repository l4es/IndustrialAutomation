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

function [sdo_config,valid] = build_sdostruct(sdoinfo)
  sdocounter = max(size(sdoinfo))
  valid = -1;
  sdo_config = int32(zeros(4,sdocounter));

  for i=1: sdocounter
    try
      valuetype = -1;
      select sdoinfo(i).datatype
       case 'si_double_T' then 
          valuetype = 1;
       case 'si_single_T' then
          valuetype = 2;
       case 'si_uint8_T' then
          valuetype = 3;
       case 'si_sint8_T' then
          valuetype = 4;
       case 'si_uint16_T' then
          valuetype = 5;
       case 'si_sint16_T' then
          valuetype = 6;
       case 'si_uint32_T' then
          valuetype = 7;
       case 'si_sint32_T' then
          valuetype = 8;
       case 'si_boolean_T' then
          valuetype = 9;
       else
          disp('Unknow Valuetype for SDO No '+string(i));
          return;
     end;
    
     sdo_config(:,i) = int32([sdoinfo(i).index sdoinfo(i).subindex valuetype sdoinfo(i).value]')
    catch 
      disp('Failure in SDO-Struct. SDO No '+string(i)+' Exiting!')
      return;
    end
  end
  //disp(sdo_config);
  valid = 1;
endfunction
