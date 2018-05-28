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

function [smconfig, pdoconfig, entryconfig, rvalue] = getslavedesc_buildopar(slave_config,txid,rxid)
      //Fixation of opar struct
     //opar(1) Type integer matrix, Value number of pdos in read/write Sync manager
     // [ count_pdo_sm1 count_pdo_sm2  count_pdo_sm3;...;
     //   sm1_index     sm2_index      sm3_index; ....;
     //   sm1_direction sm2_direction  sm3_direction;...]
     // opar(2) Pdo Configuration integer matrix follows sm1 
     // [ pdoindex num_pdoentrys; //Smread 1 PDO 1
     //   pdoindex num_pdoentrys; //smread 1 PDO 2
     //   pdoindex num_pdoentrys; //smread 2 PDO 1
     //   pdoindex num_pdoentrys;...] //smwrite 1 PDO 1
     // opar(3) Pdo Entrys integer matrix 
     // [ entryindex, subindex, bitlen, datatype; //Smread 1 PDO 1 Entry 1
     //   entryindex, subindex, bitlen, datatype; //Smread 1 PDO 1 Entry 2
     //   entryindex, subindex, bitlen, datatype; //Smread 1 PDO 2 Entry 1
     //   entryindex, subindex, bitlen, datatype;...] //Smread 1 PDO 2 Entry 2
     // opar(4) Pdo Mapping integer matrix
     // [ aliaspos, offsetpos, entryindex, subindex, vendorid, productcode, channelno , direction, typecode, fullrange, scale,offset; ...
     // Typecode 0= Analog, 1= Bit, 2=Digital
     // fullrange = x, 2^x=> max bitvalue from slave
     // scale, offset : Parameter for analog values
     // opar(5) Sdo Configuration integer matrix
     // [ Sdoindex, Sdosubindex, bitlen, value;....]
 
     rvalue = %t
     smcount = max(size(slave_config.Sm));
     pdoconfig = [];
     entryconfig = [];
     smconfig = zeros(3,smcount);
     for i=1:smcount
      for j=1:2
        if isequal(j,1) then
          id = txid; //Slave send to Master
        end
        if isequal(j,2) then
          id = rxid; //Master send to Slave
        end
        valid_config = %f
        if isequal(slave_config.Sm(i).direction ,j) then //TxPdo
            if isequal(id,0) then //default config
              if ~isempty(slave_config.Sm(i).default) then
                pdocount = max(size(slave_config.Sm(i).default.Pdo))
                Pdo = slave_config.Sm(i).default.Pdo; 
                //disp('Select Default Configuration')
                valid_config = %t;
              end
            end
           if id > 0 then
              if ~isempty(slave_config.Sm(i).alternativ(id)) then
                pdocount = max(size(slave_config.Sm(i).alternativ(id).Pdo))
                Pdo = slave_config.Sm(i).alternativ(id).Pdo; 
                //disp('Select Alternativ Configuration')
                valid_config = %t;
              end
           end
     end
     if valid_config then
        //disp(['PdoCount = '+string(pdocount)])
        for j=1:pdocount
           entrycount = size(Pdo(j).Entrys);
           pdoconfig = cat(1,pdoconfig,[Pdo(j).index entrycount(1)]);
           for k=1:entrycount(1)
              entryconfig = cat(1,entryconfig,Pdo(j).Entrys(k,:));
           end
        end
        smconfig(:,i)=[pdocount; slave_config.Sm(i).index; slave_config.Sm(i).direction]
     end  
    end
   end
  
 
     smconfig = int32(smconfig);
     pdoconfig = int32(pdoconfig');
     entryconfig = int32(entryconfig');
endfunction
