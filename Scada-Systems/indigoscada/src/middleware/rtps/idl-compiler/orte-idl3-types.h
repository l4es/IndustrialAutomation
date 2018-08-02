#ifndef ORTE_IDL3_TYPES_H
#define ORTE_IDL3_TYPES_H 1

#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <libIDL/IDL.h>

typedef struct _OIDL_Marshal_Context OIDL_Marshal_Context;

#define OUTPUT_NUM_PASSES 7

typedef struct {
  char *cpp_args;
  int debug_level;
  int idl_warn_level;
  int show_cpp_errors;
  int is_pidl;

  enum { OUTPUT_HEADERS=1<<0,
	 OUTPUT_IMPLS=1<<1
  } enabled_passes;

  char *output_formatter;

  char *output_language;
  char *input_filename;
  char *backend_directory;
  char *deps_file;
  char *header_guard_prefix;
  gboolean onlytop;

  IDL_ns ns; /* Use ns instead of namespace because that's a C++ reserved keyword */
} OIDL_Run_Info;

typedef struct {
  IDL_tree op1;
  IDL_tree op2;
} OIDL_Attr_Info;

#endif
