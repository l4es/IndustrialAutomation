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

function slave_config= getslavedesc_getconfig(slave_desc,slave_typeid)
 slave_config.vendor = getslavedesc_vendor(slave_desc);
 slave_config.productcode = getslavedesc_productcode(slave_desc,slave_typeid);
 slave_numSm = getslavedesc_numSm(slave_desc,slave_typeid);
 slave_config = [];
 index_sm = 1;
 for i=1:slave_numSm
  //disp(['Syncmgr: '+string(i)])
  CBytetype =getslavedesc_getsmtype(slave_desc,slave_typeid,i);
  //disp(['Controltype: '+string(CBytetype)])
  if CBytetype >= 0 then 
    slave_numPdo = getslavedesc_numPdo(slave_desc,slave_typeid,CBytetype);
    //disp(['Pdo Count: '+string(slave_numPdo)])
    index_default = 1;
    index_alternative = 1;
    Sm=[];
    Sm.index = i;
    Sm.direction = CBytetype; //1= TxPdo Slave send to Master, 2=RxPdo Master send to Slave
    for j=1:slave_numPdo
      //disp(['Pdo: '+string(j)])
      Pdo=[];
     [smno,pdoindex,smexclude,smmandatory] = getslavedesc_SmPdoInfo(slave_desc,slave_typeid,CBytetype,j);
     Pdo.index = pdoindex;
     slave_numentrys = getslavedesc_SmPdonEntry(slave_desc,slave_typeid,CBytetype,j); 
     //disp(['Entrys :'+string(slave_numentrys)]);
     Pdo.Entrys = cell2mat(cell(slave_numentrys,4));
     for k=1:slave_numentrys
       [eindex,esubindex,ebitlen,etype] = getslavedesc_SmPdoEntry(slave_desc,slave_typeid,CBytetype,j,k);
       if eindex == 0 then
          //disp('GAP-Entry found')
          esubindex = 0;
          etype = 0;
       end
       //disp(['Pdo Entry: '+string(eindex)+' '+string(esubindex)+' '+string(ebitlen)+' '+string(etype)])
       Pdo.Entrys(k,:)=[eindex esubindex ebitlen etype]
     end
     //disp(['Sync Index '+string(smno)+' available '+string(~isempty(smno))])
     if and([~isempty(smno) isequal(smno,i-1)]) then //Passender Eintrag für Syncmanager
       Sm.default.Pdo(index_default) = Pdo;
       //disp('Added Default Sync Manager Configuration')
       index_default = index_default+1;
     else
       Sm.alternativ.Pdo(index_alternative) = Pdo; 
       //disp('Added Alternativ Sync Manager Configuration')
       index_alternative = index_alternative +1;
     end
    end 
    slave_config.Sm(index_sm)=Sm;
    index_sm = index_sm +1;
  end
 end
endfunction
