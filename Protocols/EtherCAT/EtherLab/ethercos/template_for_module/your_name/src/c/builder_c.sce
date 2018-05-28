// Copyright (C) 2008  Holger Nahrstaedt
//
// This file is part of HART, the Hardware Access in Real Time Toolbox for Scilab/Scicos.
//
// HART is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// HART is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with HART; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA// 
// ====================================================================

names =   [ 'rt_tempfunc';]; //defined in loader.sce

if ~MSDOS then
 files     = ["template_function.o"];
else
  files     = ["template_function.o"];
end
libs      = ["../../../../src/c/libhart"];
flag      = "c";
makename  = "Makefile";
loadername= "loader.sce";
libname   = "_your_name";
if ~MSDOS then
  cflags    = "-D_TTY_POSIX_ -fno-stack-protector     -I"+get_absolute_file_path('builder_c.sce')+"../../../../includes ";
else
     cflags    = "  $(CC_PICFLAGS) -I"+SCI+"/routines   -I../includes";
end
if ~MSDOS
	ldflags   = "-lpthread -lstdc++ -lrt -lm";
else
ldflags   = "Winmm.lib";
end
fflags    = [];
if ~MSDOS then
  cc        = "gcc";
end
if ~MSDOS then
//  ilib_for_link(names,files, libs,flag, makename,loadername,libname, ldflags, cflags, fflags,cc);
tbx_build_src(names, files, flag, get_absolute_file_path('builder_c.sce'),libs,ldflags,cflags,fflags,cc,libname,loadername,makename);


else
  ilib_for_link(names,files, libs,flag, makename,loadername,libname, ldflags, cflags, fflags);
end
// ====================================================================
clear tbx_build_src;

clear names;
clear files;
clear libs;
// ====================================================================
