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
// along with Etherlab; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA// 
// ====================================================================

names =   [ 'etl_scicos']; // ; Seperator defined in loader.sce

files     = ["etl_scicos.o"]; // ; Seperator

libs      = [];
flag      = "c";
makename  = "Makefile";
loadername= "loader.sce";
libname   = "etherlab_basis";
cflags    = " -O2 -g -fpic -fno-stack-protector   -I/opt/etherlab/include  -I"+get_absolute_file_path('builder_c.sce')+"../../../../includes ";
ldflags   = " -Wl,--rpath -Wl,"+get_absolute_file_path('builder_c.sce')+"../../../../src/c -L"+get_absolute_file_path('builder_c.sce')+"../../../../src/c -lpthread -lstdc++ -lrt -lm -letherlab";
fflags    = [];
cc        = "gcc";


tbx_build_src(names, files, flag, get_absolute_file_path('builder_c.sce'),libs,ldflags,cflags,fflags,cc,libname,loadername,makename);


// ====================================================================
clear tbx_build_src;

clear names;
clear files;
clear libs;
// ====================================================================
