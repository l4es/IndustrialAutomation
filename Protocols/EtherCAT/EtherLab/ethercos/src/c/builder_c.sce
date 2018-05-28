// Copyright (C) 2008-2009  Andreas Stewering, IgH Essen
//
// This file is part of EtherCOS.
//
// EtherCOS is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// EtherCOS is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with EtherCOS; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA// 
// ====================================================================
names =''; //defined in loader.sce

files     = ["getstr.o","ecrt_scicos_support.o","ecrt_support_simply.o","list.o"];
libs      = [];
flag      = "c";
makename  = "Makefile";
loadername= "loader.sce";
libname   = "etherlab";
cflags    = " -O2 -g -fpic -fno-stack-protector  -I/opt/etherlab/include   -I"+get_absolute_file_path('builder_c.sce')+"../../includes ";
ldflags   = " -Wl,--rpath -Wl,/opt/etherlab/lib -L/opt/etherlab/lib -lpthread -lstdc++ -lm -lethercat";
fflags    = [];
cc        = "gcc";
tbx_build_src(names, files, flag, get_absolute_file_path('builder_c.sce'),libs,ldflags,cflags,fflags,cc,libname,loadername,makename);


// ====================================================================
clear tbx_build_src;

clear names;
clear files;
clear libs;
// ====================================================================
