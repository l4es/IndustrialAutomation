mode(-1)
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

help_dir = get_absolute_file_path('build_help.sce');

xmltohtml(help_dir,'Etherlab Codegeneration');

// ====================================================================
clear xmltohtml;
clear need_to_be_build;
clear dl_df_lang_xml_files;
clear complete_with_df_lang;
clear gener_hh;
clear gener_contents;
clear gener_links;
clear gener_index;
clear gener_whatis;
   
clear help_dir;

// ====================================================================
