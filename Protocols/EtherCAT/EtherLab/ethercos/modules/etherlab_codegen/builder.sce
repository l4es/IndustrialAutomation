// Copyright (C) 2008  Andreas Stewering
// Copyright (C) 2008  Holger Nahrstaedt (HART-Project)
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
// along with HART; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA//
// ====================================================================

TOOLBOX_NAME = 'etherlab_codegen';
TOOLBOX_TITLE = 'Codegeneration module for Etherlab Toolbox';

// ====================================================================

mode(-1);
lines(0);
status='';
if (exists('buildDetails')==0) then
	buildDetails='';
end
module_dir = get_absolute_file_path('builder.sce');


//=====================================================================
//Set Release dependend Codegeneration Files
try
  [version,modules]=getversion();
  disp('')
  select version
	case 'scilab-4.1.2' then
		shellstr = 'ln -s '+module_dir+'libs/4.1.2 '+module_dir+'libs_codegen'; 
	    	rcode = unix(shellstr);
		if rcode != 0 then
		  disp('Library Initialisation not successfull!')
		  return;
		end
	case 'Scilab-Gtk-2007-12-07' then
		shellstr = 'ln -s '+module_dir+'libs/4.1.2 '+module_dir+'libs_codegen'; 
	    	rcode = unix(shellstr);
		if rcode != 0 then
		  disp('Macro Initialisation not successfull!')
		  return;
		end
	case '4.3' then
		shellstr = 'ln -s '+module_dir+'libs/4.3 '+module_dir+'libs_codegen'; 
	    	rcode = unix(shellstr);
		if rcode != 0 then
		  disp('Macro Initialisation not successfull!')
		  return;
		end
	else
	  disp('Scilab Version not supported');
	  return;	
 end;
catch 
 disp('Failure Handling')
end 




try
 getversion('scilab');
 if ~with_module('development_tools') then
  error(msprintf(gettext('%s module not installed."),'development_tools'));
  end
catch
  try
    test=gettext('test');
    clear test;
  catch
    try
      baselib=lib(module_dir+"..\..\macros\");
    catch
      error(msprintf(('%s Toolbox not installed."),'ETHERLAB'));
    end
  end

end;

// ====================================================================

try


tbx_build_loader(TOOLBOX_NAME, module_dir);


tbx_builder_macros(module_dir);


//tbx_builder_src(module_dir);
//tbx_builder_gateway(module_dir);
tbx_builder_help(module_dir);


listSuccessful($+1)=TOOLBOX_NAME;
status=sprintf(' |   %s                         ',TOOLBOX_NAME);

catch
   status=sprintf(' !                         %s ',TOOLBOX_NAME);
    printf('etherlab %s module could not be build!',TOOLBOX_NAME);
    listFailed($+1)=TOOLBOX_NAME;
end
buildDetails=[buildDetails; status];


clear tbx_builder_macros tbx_builder_src tbx_builder_gateway tbx_builder_help tbx_build_loader;
clear module_dir;
clear TOOLBOX_NAME TOOLBOX_TITLE;
clear status;
// ====================================================================
