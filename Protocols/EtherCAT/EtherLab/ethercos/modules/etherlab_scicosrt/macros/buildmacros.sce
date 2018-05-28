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
macros_dir_utils = get_absolute_file_path('buildmacros.sce');
genlib('etherlab_rt_utils',macros_dir_utils,%t,%t);

create_palette(get_absolute_file_path('buildmacros.sce'));
// clear variables on stack
clear getsave;
clear genlib;
clear cd;
clear macros_dir_utils;
// ====================================================================
clear tbx_build_macros;
clear tbx_build_palette;
// ====================================================================
