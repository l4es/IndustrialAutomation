//#include "config.h"

#include "orte-idl-c-backend.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

static FILE *out_for_pass(const char *input_filename, int pass, 
			  OIDL_Run_Info *rinfo);

gboolean
orte_idl_output_c (IDL_tree       tree,
		    OIDL_Run_Info *rinfo)
{
  int i;
  char *ctmp;
  OIDL_C_Info ci;

  ci.base_name = g_path_get_basename(rinfo->input_filename);
  ctmp = strrchr(ci.base_name, '.');
  g_assert(ctmp);
  *ctmp = '\0';

  ci.c_base_name = g_strdup(ci.base_name);
  if(!isalpha((guchar)ci.c_base_name[0]))
    ci.c_base_name[0] = '_';
  for(i = 0; ci.c_base_name[i]; i++) {
    if(!isalnum((guchar)ci.c_base_name[i])) ci.c_base_name[i] = '_';
  }

  ci.ext_dcls = g_string_new(NULL);

  ci.do_impl_hack = 1;
  for(i = 0; i < OUTPUT_NUM_PASSES; i++) {
    if( (1 << i) & rinfo->enabled_passes) {
      ci.fh = out_for_pass(rinfo->input_filename, 1 << i, rinfo);
      
      switch(1 << i) {
      case OUTPUT_HEADERS:
	orte_idl_output_c_headers(tree, rinfo, &ci);
	break;
      case OUTPUT_IMPLS:
	orte_idl_output_c_impls(tree, rinfo, &ci);
	break;
      }
//      if (1 << i == OUTPUT_DEPS)
//	fclose(ci.fh);
//      else 
	pclose(ci.fh);
    }
  }
  g_string_free(ci.ext_dcls,TRUE);

  return TRUE;
}

char *
orte_idl_c_filename_for_pass (const char *input_filename, 
                               int pass)
{
	char *filename;
	char *basename;
	char *dot;
	const char *tack_on = NULL;
  
	basename = g_path_get_basename (input_filename);
	dot = strrchr (basename, '.');
	if (dot != NULL)
		*dot = '\0';

	switch (pass) {
	case OUTPUT_HEADERS:
		tack_on = ".h";
		break;
	case OUTPUT_IMPLS:
		tack_on = ".c";
		break;
	default:
		g_error("Unknown output pass");
		break;
	}

	filename = g_strconcat (basename, tack_on, NULL);
	g_free (basename);

	return filename;
}

static FILE *
out_for_pass (const char    *input_filename,
	      int            pass,
	      OIDL_Run_Info *rinfo)
{
	FILE *fp;
	char *output_filename;
	char *cmdline;

/*	if (pass == OUTPUT_DEPS) {
		if (!g_file_test (".deps", G_FILE_TEST_IS_DIR)) {
			if (mkdir (".deps", 0775) < 0) {
				g_warning ("failed to create '.deps' directory '%s'",
					   strerror (errno));
				return NULL;
			}
		}
		
		if (rinfo->deps_file)
			fp =  fopen (rinfo->deps_file, "w");
		else
			fp = NULL;

		if (fp == NULL) 
			g_warning ("failed to open '%s': %s\n",
				   rinfo->deps_file, strerror (errno));
		
	} else {*/
		output_filename = orte_idl_c_filename_for_pass (input_filename, pass);

		cmdline = g_alloca (strlen (rinfo->output_formatter) +
				    strlen (output_filename) +
				    sizeof(" > "));
		sprintf (cmdline, "%s > %s", rinfo->output_formatter, output_filename);

		g_free (output_filename);

		/* Many versions of cpp do evil translating internal
		* strings, producing bogus output, so clobber LC_ALL */
		putenv ("LC_ALL=C");
		fp = popen (cmdline, "w");

		if (fp == NULL)
			g_error ("failed to popen '%s': %s\n", cmdline, strerror(errno));
//	}

	return fp;
}
