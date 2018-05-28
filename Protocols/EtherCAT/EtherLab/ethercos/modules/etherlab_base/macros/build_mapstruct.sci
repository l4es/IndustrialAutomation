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

function [pdo_map,pdo_map_scale,valid] = build_mapstruct(channelinfo)
  channelcounter = max(size(channelinfo))
  valid = -1;
  pdo_map = int32(zeros(9,channelcounter));
  pdo_map_scale = real(zeros(2,channelcounter));
  for i=1: channelcounter
    try
      valuetype = -1;
      bitlength = 0;
      select channelinfo(i).valuetype
       case 'si_double_T' then 
          valuetype = 1;
          bitlength = 32;
       case 'si_single_T' then
          valuetype = 2;
          bitlength = 16;
       case 'si_uint8_T' then
          valuetype = 3;
          bitlength = 8;
       case 'si_sint8_T' then
          valuetype = 4;
          bitlength = 8;
       case 'si_uint16_T' then
          valuetype = 5;
          bitlength = 16;
       case 'si_sint16_T' then
          valuetype = 6;
          bitlength = 16;
       case 'si_uint32_T' then
          valuetype = 7;
          bitlength = 32;
       case 'si_sint32_T' then
          valuetype = 8;
          bitlength = 32;
       case 'si_boolean_T' then
          valuetype = 9;
          bitlength = 1;
       else
          disp('Unknow Valuetype for Channel No '+string(i));
          return;
     end;
     
     typecode = -1;
     select channelinfo(i).typecode
      case 'analog' then
          typecode = 0;
      case 'raw' then
          typecode = 1;
      case 'digital' then
          typecode = 2;
      else
        disp('Unkown Typecode for Channel No '+string(i));
        return;
     end;
   
     direction = -1;
     select channelinfo(1).direction
     case 'getvalues' then
        direction = 1;
     case 'setvalues' then
        direction = 2;
     else
        disp('Unkown Direction for Channel No '+string(i));
        return;
     end; 
    
     pdo_map(:,i) = int32([channelinfo(i).index channelinfo(i).subindex channelinfo(i).vectorlength valuetype bitlength (channelinfo(i).channelno-1) direction typecode  channelinfo(i).fullrange]')
     pdo_map_scale(:,i) = real([channelinfo(i).scale channelinfo(i).offset]');
    catch 
      disp('Failure in Channelstruct.Channel No '+string(i)+' Exiting!')
      return;
    
    end
   
    
  end
  //disp(pdo_map);
  //disp(pdo_map_scale);
  valid = 1;
endfunction
