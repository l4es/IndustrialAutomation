//#include "config.h"
#include "orte-idl-c-backend.h"

#include <string.h>
#include <ctype.h>

/* ch = C header */
static void ch_output_types(IDL_tree tree, OIDL_Run_Info *rinfo, OIDL_C_Info *ci);
//static void ch_output_poa(IDL_tree tree, OIDL_Run_Info *rinfo, OIDL_C_Info *ci);
//static void ch_output_itypes (IDL_tree tree, OIDL_C_Info *ci);
static void ch_output_impls_decl(IDL_tree tree, OIDL_Run_Info *rinfo, OIDL_C_Info *ci);
//static void ch_output_skel_protos(IDL_tree tree, OIDL_Run_Info *rinfo, OIDL_C_Info *ci);

void
orte_idl_output_c_headers (IDL_tree tree, OIDL_Run_Info *rinfo, OIDL_C_Info *ci)
{
  fprintf (ci->fh, OIDL_C_WARNING);
  fprintf(ci->fh, "#ifndef %s%s_H\n", rinfo->header_guard_prefix, ci->c_base_name);
  fprintf(ci->fh, "#define %s%s_H 1\n\n", rinfo->header_guard_prefix, ci->c_base_name);

  fprintf(ci->fh, "#ifdef __cplusplus\n");
  fprintf(ci->fh, "extern \"C\" {\n");
  fprintf(ci->fh, "#endif /* __cplusplus */\n\n");

  fprintf(ci->fh, "#ifndef EXCLUDE_ORTE_H\n");
  fprintf(ci->fh, "#include <orte.h>\n");
  fprintf(ci->fh, "#endif /* EXCLUDE_ORTE_H */\n");

  /* Do all the typedefs, etc. */
  fprintf(ci->fh, "\n/** typedefs **/\n");
  ch_output_types(tree, rinfo, ci);
  
  fprintf(ci->fh, "\n/** impls declarations **/\n");
  ch_output_impls_decl(tree, rinfo, ci);

  if ( ci->ext_dcls && ci->ext_dcls->str )
    fputs( ci->ext_dcls->str, ci->fh);	/* this may be huge! */

  fprintf(ci->fh, "\n");
  fprintf(ci->fh, "#ifdef __cplusplus\n");
  fprintf(ci->fh, "}\n");
  fprintf(ci->fh, "#endif /* __cplusplus */\n\n");

  fprintf(ci->fh, "#endif\n");
}

static void
ch_output_var(IDL_tree val, IDL_tree name, OIDL_C_Info *ci)
{
  orte_cbe_write_typespec(ci->fh, val);

  fprintf(ci->fh, " ");
  switch(IDL_NODE_TYPE(name)) {
  case IDLN_IDENT:
    fprintf(ci->fh, "%s", IDL_IDENT(name).str);
    break;
  case IDLN_TYPE_ARRAY:
    {
      IDL_tree curitem;

      fprintf(ci->fh, "%s", IDL_IDENT(IDL_TYPE_ARRAY(name).ident).str);
      for(curitem = IDL_TYPE_ARRAY(name).size_list; curitem; curitem = IDL_LIST(curitem).next) {
	fprintf(ci->fh, "[%" IDL_LL "d]", IDL_INTEGER(IDL_LIST(curitem).data).value);
      }
    }
    break;
  default:
    g_error("Weird varname - %s", IDL_tree_type_names[IDL_NODE_TYPE(name)]);
    break;
  }
  fprintf(ci->fh, ";\n");
}

static void ch_output_interface(IDL_tree tree, OIDL_Run_Info *rinfo, OIDL_C_Info *ci);
static void ch_output_type_struct(IDL_tree tree, OIDL_Run_Info *rinfo, OIDL_C_Info *ci);
static void ch_output_type_enum(IDL_tree tree, OIDL_Run_Info *rinfo, OIDL_C_Info *ci);
static void ch_output_type_dcl(IDL_tree tree, OIDL_Run_Info *rinfo, OIDL_C_Info *ci);
static void ch_output_native(IDL_tree tree, OIDL_Run_Info *rinfo, OIDL_C_Info *ci);
static void ch_output_type_union(IDL_tree tree, OIDL_Run_Info *rinfo, OIDL_C_Info *ci);
static void ch_output_codefrag(IDL_tree tree, OIDL_Run_Info *rinfo, OIDL_C_Info *ci);
static void ch_output_const_dcl(IDL_tree tree, OIDL_Run_Info *rinfo, OIDL_C_Info *ci);
static void ch_prep(IDL_tree tree, OIDL_Run_Info *rinfo, OIDL_C_Info *ci);

//static void ch_type_alloc_and_tc(IDL_tree tree, OIDL_Run_Info *rinfo, OIDL_C_Info *ci, gboolean do_alloc);

static void
ch_output_types (IDL_tree       tree,
		 OIDL_Run_Info *rinfo,
		 OIDL_C_Info   *ci)
{
	if (!tree)
		return;

	switch (IDL_NODE_TYPE (tree)) {
	case IDLN_EXCEPT_DCL: {
		char *id;

		id = IDL_ns_ident_to_qstring (
			IDL_IDENT_TO_NS (IDL_EXCEPT_DCL (tree).ident), "_", 0);

		fprintf (ci->fh, "#undef ex_%s\n", id);
		fprintf (ci->fh, "#define ex_%s \"%s\"\n",
				id, IDL_IDENT (IDL_EXCEPT_DCL (tree).ident).repo_id);

		g_free (id);

		ch_output_type_struct (tree, rinfo, ci);
		}
		break;
	case IDLN_FORWARD_DCL:
	case IDLN_INTERFACE:
		ch_output_interface (tree, rinfo, ci);
		break;
	case IDLN_TYPE_STRUCT:
		ch_output_type_struct (tree, rinfo, ci);
		break;
	case IDLN_TYPE_ENUM:
		ch_output_type_enum (tree, rinfo, ci);
		break;
	case IDLN_TYPE_DCL:
		ch_output_type_dcl (tree, rinfo, ci);
		break;
	case IDLN_TYPE_UNION:
		ch_output_type_union (tree, rinfo, ci);
		break;
	case IDLN_CODEFRAG:
		ch_output_codefrag (tree, rinfo, ci);
		break;
	case IDLN_SRCFILE: {
		if (rinfo->onlytop) {
			char *idlfn = IDL_SRCFILE (tree).filename;

			if (!IDL_SRCFILE (tree).seenCnt &&
			    !IDL_SRCFILE(tree).isTop    &&
			    !IDL_SRCFILE(tree).wasInhibit) {
				gchar *hfn, *htail;

				hfn   = g_path_get_basename (idlfn);
				htail = strrchr (hfn,'.');

				g_assert (htail && strlen (htail) >= 2);

				htail [1] = 'h';
				htail [2] = 0;

				fprintf (ci->fh, "#include \"%s\"\n", hfn);

				g_free (hfn);
			}

		fprintf (ci->fh, "/* from IDL source file \"%s\" "
				 "(seen %d, isTop %d, wasInhibit %d) */ \n", 
					idlfn,
					IDL_SRCFILE (tree).seenCnt,
					IDL_SRCFILE (tree).isTop,
					IDL_SRCFILE (tree).wasInhibit);
		}
		}
		break;
	case IDLN_CONST_DCL:
		ch_output_const_dcl (tree, rinfo, ci);
		break;
	case IDLN_NATIVE:
		ch_output_native (tree, rinfo, ci);
		break;
	default:
		break;
	}

	switch (IDL_NODE_TYPE (tree)) {
	case IDLN_MODULE:
		ch_output_types (IDL_MODULE (tree).definition_list, rinfo, ci);
		break;
	case IDLN_LIST: {
		IDL_tree sub;

		for (sub = tree; sub; sub = IDL_LIST (sub).next) {
			ch_output_types (IDL_LIST (sub).data, rinfo, ci);
		}
		}
		break;
	case IDLN_INTERFACE:
		ch_output_types (IDL_INTERFACE (tree).body, rinfo, ci);
		break;
	default:
		break;
	}
}

static void
ch_output_interface(IDL_tree tree, OIDL_Run_Info *rinfo, OIDL_C_Info *ci)
{
    char *fullname;
    fullname = orte_cbe_get_typespec_str(tree);
    fprintf(ci->fh, "#if !defined(ORTE_DECL_%s) && !defined(_%s_defined)\n#define ORTE_DECL_%s 1\n#define _%s_defined 1\n", fullname, fullname, fullname, fullname);

    if ( tree->declspec & IDLF_DECLSPEC_PIDL ) {
        /* PIDL interfaces are not normal CORBA Objects */
    	fprintf(ci->fh, "typedef struct %s_type *%s;\n", fullname, fullname);
	fprintf(ci->fh, "#ifndef TC_%s\n", fullname);
	fprintf(ci->fh, "#  define TC_%s TC_CORBA_Object\n", fullname);
	fprintf(ci->fh, "#endif\n");
    } else {
    	fprintf(ci->fh, "#define %s__freekids CORBA_Object__freekids\n", fullname);
    	fprintf(ci->fh, "typedef CORBA_Object %s;\n", fullname);
    	fprintf(ci->fh, "extern CORBA_unsigned_long %s__classid;\n", fullname);
//	ch_type_alloc_and_tc(tree, rinfo, ci, FALSE);
    }

    fprintf(ci->fh, "#endif\n");
    g_free(fullname);
}

static void
ch_output_type_enum (IDL_tree       tree,
		     OIDL_Run_Info *rinfo,
		     OIDL_C_Info   *ci)
{
	IDL_tree  l;
	char     *enumid;

	/* CORBA spec says to do
	 * typedef unsigned int enum_name;
	 * and then #defines for each enumerator.
	 * This works just as well and seems cleaner.
	 */

	enumid = IDL_ns_ident_to_qstring (
			IDL_IDENT_TO_NS (IDL_TYPE_ENUM (tree).ident), "_", 0);
	fprintf (ci->fh, "#if !defined(_%s_defined)\n#define _%s_defined 1\n", enumid, enumid);
	fprintf (ci->fh, "typedef enum {\n");

	for (l = IDL_TYPE_ENUM (tree).enumerator_list; l; l = IDL_LIST (l).next) {
		char *id;

		id = IDL_ns_ident_to_qstring (
			IDL_IDENT_TO_NS (IDL_LIST (l).data), "_", 0);

		fprintf (ci->fh, "  %s%s\n", id, IDL_LIST (l).next ? "," : "");

		g_free (id);
	}

	fprintf (ci->fh, "} %s;\n", enumid);

//	ch_type_alloc_and_tc (tree, rinfo, ci, FALSE);

	fprintf (ci->fh, "#endif\n");

	g_free (enumid);
}

static void
ch_output_type_dcl(IDL_tree tree, OIDL_Run_Info *rinfo, OIDL_C_Info *ci)
{
	IDL_tree  l;

	ch_prep (IDL_TYPE_DCL (tree).type_spec, rinfo, ci);

	for (l = IDL_TYPE_DCL (tree).dcls; l; l = IDL_LIST (l).next) {
		char *ctmp = NULL;

		IDL_tree ent = IDL_LIST (l).data;

		switch (IDL_NODE_TYPE(ent)) {
		case IDLN_IDENT:
			ctmp = IDL_ns_ident_to_qstring (
					IDL_IDENT_TO_NS (ent), "_", 0);
			break;
		case IDLN_TYPE_ARRAY:
			ctmp = IDL_ns_ident_to_qstring (
					IDL_IDENT_TO_NS (IDL_TYPE_ARRAY (ent).ident), "_", 0);
			break;
		default:
			g_assert_not_reached ();
			break;
		}

		fprintf (ci->fh, "#if !defined(_%s_defined)\n#define _%s_defined 1\n", ctmp, ctmp);
		fprintf (ci->fh, "typedef ");
		orte_cbe_write_typespec (ci->fh, IDL_TYPE_DCL (tree).type_spec);

		switch (IDL_NODE_TYPE (ent)) {
		case IDLN_IDENT:
			fprintf (ci->fh, " %s;\n", ctmp);
			fprintf (ci->fh, "#define %s_serialize(x) ", ctmp);
			orte_cbe_write_typespec (ci->fh, IDL_TYPE_DCL (tree).type_spec);
			fprintf (ci->fh, "_serialize(x)\n");

			fprintf (ci->fh, "#define %s_deserialize(x) ", ctmp);
			orte_cbe_write_typespec (ci->fh, IDL_TYPE_DCL (tree).type_spec);
			fprintf (ci->fh, "_deserialize(x)\n");

                        fprintf(ci->fh, "#define %s_get_max_size(x) ", ctmp);
			orte_cbe_write_typespec (ci->fh, IDL_TYPE_DCL (tree).type_spec);
			fprintf (ci->fh, "_get_max_size(x)\n");
			break;
		case IDLN_TYPE_ARRAY: {
			IDL_tree sub;

			fprintf (ci->fh, " %s", ctmp);
			for (sub = IDL_TYPE_ARRAY (ent).size_list; sub; sub = IDL_LIST (sub).next)
				fprintf (ci->fh, "[%" IDL_LL "d]",
					 IDL_INTEGER (IDL_LIST (sub).data).value);

			fprintf (ci->fh, ";\n");
			fprintf (ci->fh, "typedef ");
			orte_cbe_write_typespec (ci->fh, IDL_TYPE_DCL (tree).type_spec);
			fprintf (ci->fh, " %s_slice", ctmp);
			for (sub = IDL_LIST (IDL_TYPE_ARRAY (ent).size_list).next;
			     sub; sub = IDL_LIST (sub).next)
				fprintf (ci->fh, "[%" IDL_LL "d]", IDL_INTEGER (IDL_LIST (sub).data).value);
			fprintf(ci->fh, ";\n");
			}
			break;
		default:
			break;
		}

//		ch_type_alloc_and_tc (ent, rinfo, ci, TRUE);
		fprintf (ci->fh, "#endif\n");
		g_free (ctmp);
	}
}

static void
ch_output_native(IDL_tree tree, OIDL_Run_Info *rinfo, OIDL_C_Info *ci)
{
    char *ctmp;
    IDL_tree id = IDL_NATIVE(tree).ident;
    ctmp = IDL_ns_ident_to_qstring(IDL_IDENT_TO_NS(id), "_", 0);
    fprintf(ci->fh, "#if !defined(_%s_defined)\n#define _%s_defined 1\n", ctmp, ctmp);
    fprintf(ci->fh, "typedef struct %s_type *%s;\n", ctmp, ctmp);
    /* Dont even think about emitting a typecode. */
    fprintf(ci->fh, "#endif\n");
    g_free(ctmp);
}

static void
ch_output_type_struct(IDL_tree tree, OIDL_Run_Info *rinfo, OIDL_C_Info *ci)
{
  char *id;
  IDL_tree cur, curmem;

  id = IDL_ns_ident_to_qstring(IDL_IDENT_TO_NS(IDL_TYPE_STRUCT(tree).ident), 
    "_", 0);
  fprintf(ci->fh, "#if !defined(_%s_defined)\n#define _%s_defined 1\n", id, id);
  /* put typedef out first for recursive seq */
  fprintf(ci->fh, "typedef struct %s_type %s;\n", id, id);

  /* Scan for any nested decls */
  for(cur = IDL_TYPE_STRUCT(tree).member_list; cur; cur = IDL_LIST(cur).next) {
    IDL_tree ts;
    ts = IDL_MEMBER(IDL_LIST(cur).data).type_spec;
    ch_prep(ts, rinfo, ci);
  }

  fprintf(ci->fh, "struct %s_type {\n", id);

  for(cur = IDL_TYPE_STRUCT(tree).member_list; cur; cur = IDL_LIST(cur).next) {
    for(curmem = IDL_MEMBER(IDL_LIST(cur).data).dcls; curmem; curmem = IDL_LIST(curmem).next) {
      ch_output_var(IDL_MEMBER(IDL_LIST(cur).data).type_spec, IDL_LIST(curmem).data, ci);
    }
  }
  if(!IDL_TYPE_STRUCT(tree).member_list)
    fprintf(ci->fh, "int dummy;\n");
  fprintf(ci->fh, "};\n\n");

//  ch_type_alloc_and_tc(tree, rinfo, ci, TRUE);

  fprintf(ci->fh, "#endif\n");

  g_free(id);
}

static void
ch_output_type_union(IDL_tree tree, OIDL_Run_Info *rinfo, OIDL_C_Info *ci)
{
  char *id;
  IDL_tree curitem;

  if (IDL_NODE_TYPE (IDL_TYPE_UNION (tree).switch_type_spec) == IDLN_TYPE_ENUM)
    ch_output_type_enum (IDL_TYPE_UNION (tree).switch_type_spec, rinfo, ci);

  id = IDL_ns_ident_to_qstring(IDL_IDENT_TO_NS(IDL_TYPE_UNION(tree).ident), "_", 0);
  fprintf(ci->fh, "#if !defined(_%s_defined)\n#define _%s_defined 1\n", id, id);
  fprintf(ci->fh, "typedef struct %s_type %s;\n", id, id);

  /* Scan for any nested decls */
  for(curitem = IDL_TYPE_UNION(tree).switch_body; curitem; curitem = IDL_LIST(curitem).next) {
    IDL_tree member = IDL_CASE_STMT(IDL_LIST(curitem).data).element_spec;
    ch_prep(IDL_MEMBER(member).type_spec, rinfo, ci);
  }

  fprintf(ci->fh, "struct %s_type {\n", id);
  orte_cbe_write_typespec(ci->fh, IDL_TYPE_UNION(tree).switch_type_spec);
  fprintf(ci->fh, " _d;\nunion {\n");

  for(curitem = IDL_TYPE_UNION(tree).switch_body; curitem; curitem = IDL_LIST(curitem).next) {
    IDL_tree member;

    member = IDL_CASE_STMT(IDL_LIST(curitem).data).element_spec;
    ch_output_var(IDL_MEMBER(member).type_spec,
		  IDL_LIST(IDL_MEMBER(member).dcls).data,
		  ci);
  }

  fprintf(ci->fh, "} _u;\n};\n");

//  ch_type_alloc_and_tc(tree, rinfo, ci, TRUE);

  fprintf(ci->fh, "#endif\n");

  g_free(id);
}

static void
ch_output_codefrag(IDL_tree tree, OIDL_Run_Info *rinfo, OIDL_C_Info *ci)
{
  GSList *list;

  for(list = IDL_CODEFRAG(tree).lines; list;
      list = g_slist_next(list)) {
    if(!strncmp(list->data,
		"#pragma include_defs",
		sizeof("#pragma include_defs")-1)) {
	char *ctmp, *cte;
	ctmp = ((char *)list->data) + sizeof("#pragma include_defs");
	while(*ctmp && (isspace((int)*ctmp) || *ctmp == '"')) ctmp++;
	cte = ctmp;
	while(*cte && !isspace((int)*cte) && *cte != '"') cte++;
	*cte = '\0';
      fprintf(ci->fh, "#include <%s>\n", ctmp);
    } else
      fprintf(ci->fh, "%s\n", (char *)list->data);
  }
}

static void
ch_output_const_dcl(IDL_tree tree, OIDL_Run_Info *rinfo, OIDL_C_Info *ci)
{
	char    *id;
	IDL_tree ident;
	IDL_tree typespec;

	ident = IDL_CONST_DCL (tree).ident;
	id = IDL_ns_ident_to_qstring(IDL_IDENT_TO_NS (ident), "_", 0);

	fprintf(ci->fh, "#ifndef %s\n", id);
	fprintf(ci->fh, "#define %s ", id);

	orte_cbe_write_const(ci->fh,
			      IDL_CONST_DCL(tree).const_exp);

	typespec = orte_cbe_get_typespec (IDL_CONST_DCL(tree).const_type);
	if (IDL_NODE_TYPE (typespec) == IDLN_TYPE_INTEGER &&
	    !IDL_TYPE_INTEGER (typespec).f_signed)
		fprintf(ci->fh, "U");

	fprintf(ci->fh, "\n");
	fprintf(ci->fh, "#endif /* !%s */\n\n", id);

	g_free(id);
}

static void
ch_prep_fixed(IDL_tree tree, OIDL_Run_Info *rinfo, OIDL_C_Info *ci)
{
  char *ctmp;

  ctmp = orte_cbe_get_typespec_str(tree);
  fprintf(ci->fh,
	  "typedef struct { CORBA_unsigned_short _digits; CORBA_short _scale; CORBA_char _value[%d]; } %s;\n",
	  (int) (IDL_INTEGER(IDL_TYPE_FIXED(tree).positive_int_const).value + 2)/2,
	  ctmp);
  g_free(ctmp);

//  ch_type_alloc_and_tc(tree, rinfo, ci, TRUE);
}

static void
ch_prep_sequence(IDL_tree tree, OIDL_Run_Info *rinfo, OIDL_C_Info *ci)
{
  char *ctmp, *fullname, *fullname_def, *ctmp2;
  IDL_tree tts;
  gboolean separate_defs, fake_if;
  IDL_tree fake_seq = NULL;

  tts = orte_cbe_get_typespec(IDL_TYPE_SEQUENCE(tree).simple_type_spec);
  ctmp = orte_cbe_get_typespec_str(IDL_TYPE_SEQUENCE(tree).simple_type_spec);
  ctmp2 = orte_cbe_get_typespec_str(tts);
  fake_if = (IDL_NODE_TYPE(tts) == IDLN_INTERFACE);
  if(fake_if)
    {
      g_free(ctmp2);
      ctmp2 = g_strdup("CORBA_Object");
    }
  separate_defs = strcmp((const char *)ctmp, (const char*)ctmp2);
  fullname = orte_cbe_get_typespec_str(tree);

  if(separate_defs)
    {
      if(fake_if)
	tts = IDL_type_object_new();
      fake_seq = IDL_type_sequence_new(tts, NULL);
      IDL_NODE_UP(fake_seq) = IDL_NODE_UP(tree);
      ch_prep_sequence(fake_seq, rinfo, ci);
      fullname_def = g_strdup_printf("CORBA_sequence_%s", ctmp2);
      if(!fake_if)
	IDL_TYPE_SEQUENCE(fake_seq).simple_type_spec = NULL;
    }
  else
    fullname_def = g_strdup(fullname);

  if(IDL_NODE_TYPE(IDL_TYPE_SEQUENCE(tree).simple_type_spec)
     == IDLN_TYPE_SEQUENCE)
    ch_prep_sequence(IDL_TYPE_SEQUENCE(tree).simple_type_spec, rinfo, ci);

  /* NOTE: ORTE_DECL_%s protects redef of everything (struct,TC,externs)
   * while _%s_defined protects only the struct */

  fprintf(ci->fh, "#if !defined(ORTE_DECL_%s)\n#define ORTE_DECL_%s 1\n",
	  fullname, fullname);
  if ( ci->do_impl_hack )
      orte_cbe_id_define_hack(ci->fh, "ORTE_IMPL", fullname, ci->c_base_name);

  if(separate_defs)
    {
      fprintf(ci->fh, "#if !defined(_%s_defined)\n#define _%s_defined 1\n",
	      fullname, fullname);
      if(!strcmp((const char *)ctmp, (const char*)"CORBA_RepositoryId"))
	fprintf(ci->fh, "/* CRACKHEADS */\n");
      fprintf(ci->fh, "typedef %s %s;\n", fullname_def, fullname);
      fprintf(ci->fh, "#endif\n");
//      ch_type_alloc_and_tc(tree, rinfo, ci, FALSE);
      fprintf(ci->fh, "#define %s__alloc %s__alloc\n",
	      fullname, fullname_def);
      fprintf(ci->fh, "#define %s__freekids %s__freekids\n",
	      fullname, fullname_def);
      fprintf(ci->fh, "#define CORBA_sequence_%s_allocbuf CORBA_sequence_%s_allocbuf\n",
	      ctmp, ctmp2);
      fprintf(ci->fh, "#define %s_serialize(x) %s_serialize(x)\n", fullname, fullname_def);

      fprintf(ci->fh, "#define %s_deserialize(x) %s_deserialize(x)\n", fullname, fullname_def);
      IDL_tree_free(fake_seq);
    }
  else
    {
      char *tc, *member_type;

      fprintf(ci->fh, "#if !defined(_%s_defined)\n#define _%s_defined 1\n",
	      fullname, fullname);
      fprintf(ci->fh, "typedef struct { CORBA_unsigned_long _maximum, _length; ");
      orte_cbe_write_typespec(ci->fh, IDL_TYPE_SEQUENCE(tree).simple_type_spec);
      fprintf(ci->fh, "* _buffer; CORBA_boolean _release; } ");
      orte_cbe_write_typespec(ci->fh, tree);
      fprintf(ci->fh, ";\n#endif\n");
//      ch_type_alloc_and_tc(tree, rinfo, ci, TRUE);

      tc = orte_cbe_get_typecode_name (orte_cbe_get_typespec (tree));
      member_type = orte_cbe_type_is_builtin (IDL_TYPE_SEQUENCE (tree).simple_type_spec) ?
				ctmp + strlen ("CORBA_") : ctmp;

      fprintf (ci->fh, "#define CORBA_sequence_%s_allocbuf(l) "
		       "((%s*)ORTE_small_allocbuf (%s, (l)))\n",
		       member_type, member_type, tc);

      g_free (tc);
    }

  fprintf(ci->fh, "#endif\n");

  g_free(ctmp2);
  g_free(ctmp);
  g_free(fullname);
  g_free(fullname_def);
}


static
void ch_prep (IDL_tree       tree,
	      OIDL_Run_Info *rinfo,
	      OIDL_C_Info   *ci)
{
	switch (IDL_NODE_TYPE (tree)) {
	case IDLN_TYPE_SEQUENCE:
		ch_prep_sequence (tree, rinfo, ci);
		break;
	case IDLN_TYPE_FIXED:
		ch_prep_fixed (tree, rinfo, ci);
		break;
	case IDLN_TYPE_STRUCT:
		ch_output_type_struct (tree, rinfo, ci);
		break;
	case IDLN_TYPE_ENUM:
		ch_output_type_enum (tree, rinfo, ci);
		break;
	default:
		break;
	}
}

static void
ch_output_decl(IDL_tree tree, OIDL_Run_Info *rinfo, OIDL_C_Info *ci)
{
  char *id;
  IDL_tree cur;

  id = IDL_ns_ident_to_qstring(IDL_IDENT_TO_NS(IDL_TYPE_STRUCT(tree).ident), 
    "_", 0);
  fprintf(ci->fh, "void %s_serialize(CDR_Codec *cdrCodec,%s *object);\n", id, id);
  fprintf(ci->fh, "void %s_deserialize(CDR_Codec *cdrCodec,%s *object);\n", id, id);
  fprintf(ci->fh, "int %s_get_max_size(ORTEGetMaxSizeParam *gms);\n", id);
  fprintf(ci->fh, "Boolean %s_type_register(ORTEDomain *d);\n", id);
  fprintf(ci->fh, "\n");

  /* Scan for any nested decls */
  for(cur = IDL_TYPE_STRUCT(tree).member_list; cur; cur = IDL_LIST(cur).next) {
    IDL_tree ts;
    ts = IDL_MEMBER(IDL_LIST(cur).data).type_spec;
    switch (IDL_NODE_TYPE (ts)) {
	case IDLN_TYPE_STRUCT:
		ch_output_decl(ts, rinfo, ci);
		break;
	default:
		break;
	}
  }

  g_free(id);
}


static void
ch_output_impls_decl (IDL_tree       tree,
		 OIDL_Run_Info *rinfo,
		 OIDL_C_Info   *ci)
{
	if (!tree)
		return;

	switch (IDL_NODE_TYPE (tree)) {
	case IDLN_TYPE_STRUCT:
		ch_output_decl (tree, rinfo, ci);
		break;
	default:
		break;
	}

	switch (IDL_NODE_TYPE (tree)) {
	case IDLN_MODULE:
		ch_output_impls_decl (IDL_MODULE (tree).definition_list, rinfo, ci);
		break;
	case IDLN_LIST: {
		IDL_tree sub;

		for (sub = tree; sub; sub = IDL_LIST (sub).next) {
			ch_output_impls_decl (IDL_LIST (sub).data, rinfo, ci);
		}
		}
		break;
	case IDLN_INTERFACE:
		ch_output_impls_decl (IDL_INTERFACE (tree).body, rinfo, ci);
		break;
	default:
		break;
	}
}
