// ====================================================================
// Copyright INRIA 2008
// ====================================================================

help_dir = get_absolute_file_path('builder_help.sce');

try
 	getversion('scilab');
	tbx_builder_help_lang("en_US", help_dir);
catch
	tbx_builder_help_lang("en_US_scilab4", help_dir);
end;

//tbx_builder_help_lang("en_US", help_dir);
//tbx_builder_help_lang("fr_FR", help_dir);

clear help_dir;
