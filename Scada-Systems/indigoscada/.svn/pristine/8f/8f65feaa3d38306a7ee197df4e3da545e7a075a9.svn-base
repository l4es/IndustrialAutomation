/**************************************************************************

    orte-idl-main.c (Driver program for the IDL parser & backend)

    Copyright (C) 1999 Elliot Lee

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    $Id: orte-idl-main.c,v 1.2 2005/03/11 16:46:22 smolik Exp $

***************************************************************************/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <glib.h>
#include <libIDL/IDL.h>
#include <popt.h>

#include "orte-idl2.h"

/* Settings made from the command line (prefaced with cl_) */
static gboolean cl_disable_headers = FALSE,
  cl_disable_impls = FALSE;

static int cl_idlwarnlevel = 2;
static int cl_debuglevel = 0;
static int cl_is_pidl = 0;
static char *cl_output_lang = "c";
static char *cl_header_guard_prefix = "";
static char *cl_deps_file = NULL;
static gboolean cl_onlytop = FALSE;

#define BASE_CPP_ARGS "-D__ORTE_IDL__ "
static GString *cl_cpp_args;

#define INDENT_COMMAND "cat"

static char *c_output_formatter = INDENT_COMMAND;

/* Callbacks for popt */
static void
cl_libIDL_version_callback(poptContext con, enum poptCallbackReason reason,
			   const struct poptOption *opt, char *arg,
			   void *data)
{
  g_print("libIDL %s (CORBA %s)",
	  IDL_get_libver_string(),
	  IDL_get_IDLver_string());

  exit(0);
}

static void
cl_cpp_callback(poptContext con, enum poptCallbackReason reason,
		const struct poptOption *opt, char *arg,
		void *data)
{
  g_assert(opt!=NULL);

  if(opt->shortName=='D')
    g_string_append(cl_cpp_args, "-D");
  else if(opt->shortName=='I')
    g_string_append(cl_cpp_args, "-I");

  g_string_append(cl_cpp_args, arg);
  g_string_append_c(cl_cpp_args, ' ');
}

static const
struct poptOption cl_libIDL_callback_options[] = {
  {NULL, '\0', POPT_ARG_CALLBACK, (void *)cl_libIDL_version_callback,
   0, NULL, NULL},
  {"libIDL-version", '\0', POPT_ARG_NONE, NULL, 0,
   "Show version of libIDL used.", NULL},
  {NULL, '\0', 0, NULL, 0, NULL, NULL}
};

static const
struct poptOption cl_cpp_callback_options[] = {
  {NULL, '\0', POPT_ARG_CALLBACK, (void *)cl_cpp_callback, 0, NULL, NULL},
  {"define", 'D', POPT_ARGFLAG_ONEDASH | POPT_ARG_STRING, NULL, 0,
   "Define value in preprocessor", NULL},
  {"include", 'I', POPT_ARGFLAG_ONEDASH | POPT_ARG_STRING, NULL, 0,
   "Add search path for include files", NULL},
  {NULL, '\0', 0, NULL, 0, NULL, NULL}
};

static const
struct poptOption options[] = {
  {NULL, '\0', POPT_ARG_INCLUDE_TABLE, &cl_cpp_callback_options, 0, NULL, NULL},
  {NULL, '\0', POPT_ARG_INCLUDE_TABLE, &cl_libIDL_callback_options, 0, NULL, NULL},
  {"lang", 'l', POPT_ARG_STRING, &cl_output_lang, 0, "Output language (default is C)", NULL},
  {"debug", 'd', POPT_ARG_INT, &cl_debuglevel, 0, "Debug level 0 to 4", NULL},
  {"idlwarnlevel", '\0', POPT_ARG_INT, &cl_idlwarnlevel, 0, "IDL warning level 0 to 4, default is 2", NULL},
  {"noheaders", '\0', POPT_ARG_NONE, &cl_disable_headers, 0, "Don't output headers", NULL},
  {"noimpls", '\0', POPT_ARG_NONE, &cl_disable_impls, 0, "Don't output implementations", NULL},
  {"c-output-formatter", '\0', POPT_ARG_STRING, &c_output_formatter, 0, "Program to use to format output (normally, indent)", "PROGRAM"},
  {"onlytop", '\0', POPT_ARG_NONE, &cl_onlytop, 0, "Inhibit includes", NULL},
  {"pidl", '\0', POPT_ARG_NONE, &cl_is_pidl, 0, "Treat as Pseudo IDL", NULL},
  {"deps", '\0', POPT_ARG_STRING, &cl_deps_file, 0, "Generate dependency info suitable for inclusion in Makefile", "FILENAME"},
  {"headerguardprefix", '\0', POPT_ARG_STRING, &cl_header_guard_prefix, 0, "Prefix for #ifdef header guards. Sometimes useful to avoid conflicts.", NULL},
  POPT_AUTOHELP
  {NULL, '\0', 0, NULL, 0, NULL, NULL}
};


/********** main routines **********/
int main(int argc, const char *argv[])
{
  poptContext pcon;
  int rc, retval = 0;
  const char *arg;
  OIDL_Run_Info rinfo;

  /* Argument parsing, etc. */
  cl_cpp_args = g_string_new("-D__ORTE_IDL__ ");

  if(getenv("C_OUTPUT_FORMATTER"))
    c_output_formatter = getenv("C_OUTPUT_FORMATTER");

  pcon = poptGetContext ("orte-idl", argc, argv, options, 0);
  poptSetOtherOptionHelp (pcon, "<IDL files>");

  if(argc < 2) {
  	poptPrintUsage(pcon, stdout, 0);
	return 0;
  }

  if((rc=poptGetNextOpt(pcon)) < -1) {
    g_print ("orte-idl: bad argument %s: %s\n", 
    		poptBadOption(pcon, POPT_BADOPTION_NOALIAS),
		poptStrerror(rc));
    exit(0);
  }

  /* Prep our run info for the backend */
  rinfo.cpp_args = cl_cpp_args->str;
  rinfo.debug_level = cl_debuglevel;
  rinfo.idl_warn_level = cl_idlwarnlevel;
  rinfo.is_pidl = cl_is_pidl;
  rinfo.enabled_passes =
     (cl_disable_headers?0:OUTPUT_HEADERS)
    |(cl_disable_impls?0:OUTPUT_IMPLS);

  rinfo.deps_file = cl_deps_file;

  rinfo.output_formatter = c_output_formatter;
  rinfo.output_language = cl_output_lang;
  rinfo.header_guard_prefix = cl_header_guard_prefix;
  rinfo.onlytop = cl_onlytop;
  
  printf (" %s mode, passes: %s%s\n\n",
	  rinfo.is_pidl ? "pidl" : "",
	  cl_disable_headers ? "" : "headers ",
	  cl_disable_impls ? "" : "impls ");
	   
  /* Do it */
  while((arg=poptGetArg(pcon))!=NULL) {
    rinfo.input_filename = g_strdup (arg); /* g_path_get_basename(arg); - what !? */
    if (!orte_idl_to_backend(arg, &rinfo)) {
      g_warning("%s compilation failed", arg);
      retval = 1;
    }
    g_free(rinfo.input_filename);
  }

  return retval;
}
