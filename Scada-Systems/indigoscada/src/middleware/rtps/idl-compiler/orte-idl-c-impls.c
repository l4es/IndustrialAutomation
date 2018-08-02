//#include "config.h"
#include "orte-idl-c-backend.h"

#include <string.h>
#include <ctype.h>

/* ch = C header */
static void ci_output_impls (IDL_tree tree,OIDL_Run_Info *rinfo,OIDL_C_Info *ci);


void
orte_idl_output_c_impls (IDL_tree tree, OIDL_Run_Info *rinfo, OIDL_C_Info *ci)
{
  fprintf (ci->fh, OIDL_C_WARNING);

  fprintf(ci->fh, "#include \"%s.h\"\n\n",ci->base_name);

  /* Do all the serialization, deserialization etc. */
  ci_output_impls(tree, rinfo, ci);

}

static void
ci_output_var(IDL_tree val, IDL_tree name, OIDL_C_Info *ci, int type)
{

  fprintf(ci->fh, "  ");
  orte_cbe_write_typespec(ci->fh, val);

  switch (type) {
  case 0:
    fprintf(ci->fh, "_serialize(cdrCodec,&(object->");
    break;	
  case 1:
    fprintf(ci->fh, "_deserialize(cdrCodec,&(object->");
    break;	
  default:
    break;
  }

  switch(IDL_NODE_TYPE(name)) {
  case IDLN_IDENT:
    fprintf(ci->fh, "%s", IDL_IDENT(name).str);
    break;
/*  case IDLN_TYPE_ARRAY:
    {
      IDL_tree curitem;

      fprintf(ci->fh, "%s", IDL_IDENT(IDL_TYPE_ARRAY(name).ident).str);
      for(curitem = IDL_TYPE_ARRAY(name).size_list; curitem; curitem = IDL_LIST(curitem).next) {
	fprintf(ci->fh, "[%" IDL_LL "d]", IDL_INTEGER(IDL_LIST(curitem).data).value);
      }
    }
    break;*/
  default:
    g_error("Weird varname - %s", IDL_tree_type_names[IDL_NODE_TYPE(name)]);
    break;
  }

//  if (type==1)
    fprintf(ci->fh, ")");

  fprintf(ci->fh, ");\n");
}

static void
ci_output_impls_struct(IDL_tree tree, OIDL_Run_Info *rinfo, OIDL_C_Info *ci)
{
  char *id;
  IDL_tree cur,curmem,cur_tspec;

  id = IDL_ns_ident_to_qstring(IDL_IDENT_TO_NS(IDL_TYPE_STRUCT(tree).ident), 
    "_", 0);

  /* serialize */
  fprintf(ci->fh, "/****************************************************************/\n");
  fprintf(ci->fh, "/* struct - %-50s  */\n", id);
  fprintf(ci->fh, "/****************************************************************/\n\n");
  
  fprintf(ci->fh, "void %s_serialize(CDR_Codec *cdrCodec,%s *object) {\n", id, id);
//  fprintf(ci->fh, "  %s *object=(%s*)instance;\n\n", id, id);

  for(cur = IDL_TYPE_STRUCT(tree).member_list; cur; cur = IDL_LIST(cur).next) {
    for(curmem = IDL_MEMBER(IDL_LIST(cur).data).dcls; curmem; curmem = IDL_LIST(curmem).next) {
      ci_output_var(IDL_MEMBER(IDL_LIST(cur).data).type_spec, IDL_LIST(curmem).data, ci, 0);
    }
  }
  fprintf(ci->fh, "}\n\n");

  /* deserialize */
  fprintf(ci->fh, "void\n%s_deserialize(CDR_Codec *cdrCodec,%s *object) {\n", id, id);
//  fprintf(ci->fh, "  %s *object=(%s*)instance;\n\n", id, id);

  for(cur = IDL_TYPE_STRUCT(tree).member_list; cur; cur = IDL_LIST(cur).next) {
    for(curmem = IDL_MEMBER(IDL_LIST(cur).data).dcls; curmem; curmem = IDL_LIST(curmem).next) {
      ci_output_var(IDL_MEMBER(IDL_LIST(cur).data).type_spec, IDL_LIST(curmem).data, ci, 1);
    }
  }
  fprintf(ci->fh, "}\n\n");

  /* get_max_size */
  fprintf(ci->fh, "int\n%s_get_max_size(ORTEGetMaxSizeParam *gms) {\n", id);

  for(cur = IDL_TYPE_STRUCT(tree).member_list; cur; cur = IDL_LIST(cur).next) {
    for(curmem = IDL_MEMBER(IDL_LIST(cur).data).dcls; curmem; curmem = IDL_LIST(curmem).next) {
       fprintf(ci->fh, "  ");
       orte_cbe_write_typespec(ci->fh, IDL_MEMBER(IDL_LIST(cur).data).type_spec);
       fprintf(ci->fh, "_get_max_size(gms");

       cur_tspec=IDL_MEMBER(IDL_LIST(cur).data).type_spec;
       switch (IDL_NODE_TYPE (cur_tspec)) {
       case IDLN_TYPE_STRING:
	    if (IDL_TYPE_STRING (cur_tspec).positive_int_const) {
		int length = IDL_INTEGER (IDL_TYPE_STRING (cur_tspec).positive_int_const).value;
                fprintf(ci->fh, ",%d",length);                 
            }
	    break;
       default:
	    break;
       }
       fprintf(ci->fh, ");\n");
    }
  }
  fprintf(ci->fh, "  return gms->csize;\n");
  fprintf(ci->fh, "}\n\n");

  /* get_max_size */
  fprintf(ci->fh, "Boolean\n%s_type_register(ORTEDomain *d) {\n", id);
  fprintf(ci->fh, "  Boolean ret;\n\n");
  fprintf(ci->fh, "  ret=ORTETypeRegisterAdd(d,\n");
  fprintf(ci->fh, "                          \"%s\",\n",id);
  fprintf(ci->fh, "                          (ORTETypeSerialize)%s_serialize,\n",id);
  fprintf(ci->fh, "                          (ORTETypeDeserialize)%s_deserialize,\n",id);
  fprintf(ci->fh, "                          %s_get_max_size,\n",id);
  fprintf(ci->fh, "                          0);\n");
  fprintf(ci->fh, "  return ret;\n");
  fprintf(ci->fh, "}\n\n");

  /* Scan for any nested decls */
/*  for(cur = IDL_TYPE_STRUCT(tree).member_list; cur; cur = IDL_LIST(cur).next) {
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

  g_free(id);*/
}


static void
ci_output_impls (IDL_tree       tree,
		 OIDL_Run_Info *rinfo,
		 OIDL_C_Info   *ci)
{
	if (!tree)
		return;

	switch (IDL_NODE_TYPE (tree)) {
	case IDLN_TYPE_STRUCT:
		ci_output_impls_struct (tree, rinfo, ci);
		break;
	default:
		break;
	}

	switch (IDL_NODE_TYPE (tree)) {
	case IDLN_MODULE:
		ci_output_impls (IDL_MODULE (tree).definition_list, rinfo, ci);
		break;
	case IDLN_LIST: {
		IDL_tree sub;

		for (sub = tree; sub; sub = IDL_LIST (sub).next) {
			ci_output_impls (IDL_LIST (sub).data, rinfo, ci);
		}
		}
		break;
	case IDLN_INTERFACE:
		ci_output_impls (IDL_INTERFACE (tree).body, rinfo, ci);
		break;
	default:
		break;
	}
}
