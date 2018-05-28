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

mode(-1)
//Convert MAT V6 in Scilab Binary Format
disp('Convert Slave Descriptions')
//scilab_basename =  SCI;
slavedesc_path = get_absolute_file_path('convert_sd_mat2dat.sce')
//slavedesc_path = [scilab_basename+'/contrib/etherlab/modules/etherlab_base/slave_descriptions/'];
filestoconvert = dir([slavedesc_path+'/*_v6.mat']);

for i=1:max(size(filestoconvert.name))
  [tmppath, filename, tmpext] = fileparts(filestoconvert.name(i));
  disp(['Convert '+filename])
  loadmatfile(filestoconvert.name(i));
  tmpfilename = strsubst(filename,'_v6','');
  execstr(['slavedesc = '+tmpfilename+';']);
  execstr(['clear '+tmpfilename+';']);
  clear tmpfilename;
  save([slavedesc_path+filename+'.dat'],slavedesc);
end

clear slavedesc;
clear scilab_basename;
clear slavedesc_path;
clear filestoconvert;
clear i;
clear tmppath;
clear filename;
clear tmpext;