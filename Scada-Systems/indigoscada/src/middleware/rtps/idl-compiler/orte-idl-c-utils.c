//#include "config.h"

#include "orte-idl-c-backend.h"

#include <string.h>

char *
orte_cbe_get_typecode_name (IDL_tree tree)
{
	if (!tree)
		return g_strdup ("TC_FIXME");
	else
		return g_strconcat ("TC_", orte_cbe_get_typespec_str (tree), NULL);
}

gboolean
orte_cbe_type_is_builtin(IDL_tree tree)
{ 
  return FALSE;
  switch(IDL_NODE_TYPE(tree)) {
  case IDLN_LIST:
  case IDLN_GENTREE:
  case IDLN_MEMBER:
  case IDLN_NATIVE:
  case IDLN_CASE_STMT:
  case IDLN_MODULE:
  case IDLN_BINOP:
  case IDLN_UNARYOP:
  case IDLN_CODEFRAG:
    g_error("Strange type for being a builtin");
    break;
  case IDLN_INTEGER:
  case IDLN_STRING:
  case IDLN_WIDE_STRING:
  case IDLN_CHAR:
  case IDLN_WIDE_CHAR:
  case IDLN_FIXED:
  case IDLN_FLOAT:
  case IDLN_BOOLEAN:
  case IDLN_CONST_DCL:
  case IDLN_TYPE_INTEGER:
  case IDLN_TYPE_FLOAT:
  case IDLN_TYPE_CHAR:
  case IDLN_TYPE_WIDE_CHAR:
  case IDLN_TYPE_STRING:
  case IDLN_TYPE_WIDE_STRING:
  case IDLN_TYPE_BOOLEAN:
  case IDLN_TYPE_OCTET:
  case IDLN_TYPE_ANY:
  case IDLN_TYPE_OBJECT:
  case IDLN_TYPE_TYPECODE:
  case IDLN_TYPE_ENUM:
    return TRUE;
    break;
  case IDLN_TYPE_DCL:
  case IDLN_EXCEPT_DCL:
  case IDLN_ATTR_DCL:
  case IDLN_OP_DCL:
  case IDLN_PARAM_DCL:
  case IDLN_TYPE_FIXED:
  case IDLN_TYPE_SEQUENCE:
  case IDLN_TYPE_ARRAY:
  case IDLN_TYPE_STRUCT:
  case IDLN_TYPE_UNION:
  case IDLN_IDENT:
  case IDLN_INTERFACE:
  case IDLN_FORWARD_DCL:
  default:
    return FALSE;
    break;
  }

  return FALSE;
}

/**
    Gets the "type" of {tree} as known in C.
    The return value was alloc'd via g_malloc, and must be g_free'd.
**/
char *
orte_cbe_get_typespec_str(IDL_tree tree)
{
  char *retval = NULL;
  GString *tmpstr = NULL;

  if(!tree) {
    return g_strdup("void");
  }

  switch(IDL_NODE_TYPE(tree)) {
  case IDLN_MEMBER:
    return orte_cbe_get_typespec_str(IDL_MEMBER(tree).type_spec);
    break;
  case IDLN_TYPE_ANY:
    retval = "CORBA_any";
    break;
  case IDLN_TYPE_FLOAT:
    switch(IDL_TYPE_FLOAT(tree).f_type) {
    case IDL_FLOAT_TYPE_FLOAT:
      retval = "CORBA_float";
      break;
    case IDL_FLOAT_TYPE_DOUBLE:
      retval = "CORBA_double";
      break;
    case IDL_FLOAT_TYPE_LONGDOUBLE:
      retval = "CORBA_long_double";
      break;
    }
    break;
  case IDLN_TYPE_FIXED:
    return g_strdup_printf( "CORBA_fixed_%" IDL_LL "d_%" IDL_LL "d",
		     IDL_INTEGER(IDL_TYPE_FIXED(tree).positive_int_const).value,
		     IDL_INTEGER(IDL_TYPE_FIXED(tree).integer_lit).value);
    break;
  case IDLN_TYPE_INTEGER:
    tmpstr = g_string_new(NULL);
    g_string_append(tmpstr, "CORBA_");
    if(!IDL_TYPE_INTEGER(tree).f_signed)
	g_string_append(tmpstr, "unsigned_");

    switch(IDL_TYPE_INTEGER(tree).f_type) {
    case IDL_INTEGER_TYPE_SHORT:
	g_string_append(tmpstr, "short");
	break;
    case IDL_INTEGER_TYPE_LONGLONG:
	g_string_append(tmpstr, "long_");
    	/* FALLTHROUGH */
    case IDL_INTEGER_TYPE_LONG:
	g_string_append(tmpstr, "long");
	break;
    }
    break;
  case IDLN_TYPE_STRING:
    retval = "CORBA_string";	/* this is non-standard! */
    break;
  case IDLN_TYPE_OCTET:
    retval = "CORBA_octet";
    break;
  case IDLN_TYPE_WIDE_STRING:
    retval = "CORBA_wstring";	/* this is non-standard! */
    break;
  case IDLN_TYPE_CHAR:
    retval = "CORBA_char";
    break;
  case IDLN_TYPE_WIDE_CHAR:
    retval = "CORBA_wchar";
    break;
  case IDLN_TYPE_BOOLEAN:
    retval = "CORBA_boolean";
    break;
  case IDLN_TYPE_STRUCT:
    return orte_cbe_get_typespec_str(IDL_TYPE_STRUCT(tree).ident);
  case IDLN_EXCEPT_DCL:
    return orte_cbe_get_typespec_str(IDL_EXCEPT_DCL(tree).ident);
  case IDLN_TYPE_ARRAY:
    return orte_cbe_get_typespec_str(IDL_TYPE_ARRAY(tree).ident);
  case IDLN_TYPE_UNION:
    return orte_cbe_get_typespec_str(IDL_TYPE_UNION(tree).ident);
  case IDLN_TYPE_ENUM:
    return orte_cbe_get_typespec_str(IDL_TYPE_ENUM(tree).ident);
  case IDLN_IDENT:
    return IDL_ns_ident_to_qstring(IDL_IDENT_TO_NS(tree), "_", 0);
  case IDLN_PARAM_DCL:
    return orte_cbe_get_typespec_str(IDL_PARAM_DCL(tree).param_type_spec);
  case IDLN_TYPE_SEQUENCE:
    {
	IDL_tree subtype = IDL_TYPE_SEQUENCE(tree).simple_type_spec;
        char *ctmp, *base;
        ctmp = orte_cbe_get_typespec_str(subtype);
	/* We should have built-in alias to make this next line not needed */
	base = orte_cbe_type_is_builtin(subtype)
	  ? ctmp + strlen("CORBA_") : ctmp;
	retval = g_strdup_printf( "CORBA_sequence_%s", base);
        g_free(ctmp);
        return retval;
    }
    break;
  case IDLN_NATIVE:
    retval = "gpointer";
    break;
  case IDLN_FORWARD_DCL:
  case IDLN_INTERFACE:
    return orte_cbe_get_typespec_str(IDL_INTERFACE(tree).ident);
  case IDLN_TYPE_OBJECT:
    retval = "CORBA_Object";
    break;
  case IDLN_TYPE_TYPECODE:
    retval = "CORBA_TypeCode";
    break;
  default:
    g_error("We were asked to get a typename for a %s",
	    IDL_tree_type_names[IDL_NODE_TYPE(tree)]);
    break;
  }

  if (retval)
    return g_strdup (retval);
  else
    return g_string_free (tmpstr, FALSE);
}

void
orte_cbe_write_typespec(FILE *of, IDL_tree tree)
{
    char *name = orte_cbe_get_typespec_str(tree);
    fprintf( of, name);
    g_free(name);
}

/* Writes the value of the constant in 'tree' to file handle 'of' */
static char *
orte_cbe_get_const(IDL_tree tree)
{
  char *opc = NULL, *retval, *ctmp;
  GString *tmpstr = g_string_new(NULL);

  switch(IDL_NODE_TYPE(tree)) {
  case IDLN_BOOLEAN:
    g_string_printf(tmpstr, "%s", IDL_BOOLEAN(tree).value?"CORBA_TRUE":"CORBA_FALSE");
    break;
  case IDLN_CHAR:
    g_string_printf(tmpstr, "'%s'", IDL_CHAR(tree).value);
    break;
  case IDLN_FLOAT:
    g_string_printf(tmpstr, "%f", IDL_FLOAT(tree).value);
    break;
  case IDLN_INTEGER:
    g_string_printf(tmpstr, "%" IDL_LL "d", IDL_INTEGER(tree).value);
    break;
  case IDLN_STRING:
    g_string_printf(tmpstr, "\"%s\"", IDL_STRING(tree).value);
    break;
  case IDLN_WIDE_CHAR:
    g_string_printf(tmpstr, "L'%ls'", IDL_WIDE_CHAR(tree).value);
    break;
  case IDLN_WIDE_STRING:
    g_string_printf(tmpstr, "L\"%ls\"", IDL_WIDE_STRING(tree).value);
    break;
  case IDLN_BINOP:
    g_string_printf(tmpstr, "(");
    ctmp = orte_cbe_get_const(IDL_BINOP(tree).left);
    g_string_append(tmpstr, ctmp);
    g_free(ctmp);
    switch(IDL_BINOP(tree).op) {
    case IDL_BINOP_OR:
      opc = "|";
      break;
    case IDL_BINOP_XOR:
      opc = "^";
      break;
    case IDL_BINOP_AND:
      opc = "&";
      break;
    case IDL_BINOP_SHR:
      opc = ">>";
      break;
    case IDL_BINOP_SHL:
      opc = "<<";
      break;
    case IDL_BINOP_ADD:
      opc = "+";
      break;
    case IDL_BINOP_SUB:
      opc = "-";
      break;
    case IDL_BINOP_MULT:
      opc = "*";
      break;
    case IDL_BINOP_DIV:
      opc = "/";
      break;
    case IDL_BINOP_MOD:
      opc = "%";
      break;
    }
    g_string_append_printf(tmpstr, " %s ", opc);
    ctmp = orte_cbe_get_const(IDL_BINOP(tree).right);
    g_string_append_printf(tmpstr, "%s)", ctmp);
    g_free(ctmp);
    break;
  case IDLN_UNARYOP:
    switch(IDL_UNARYOP(tree).op) {
    case IDL_UNARYOP_PLUS: opc = "+"; break;
    case IDL_UNARYOP_MINUS: opc = "-"; break;
    case IDL_UNARYOP_COMPLEMENT: opc = "~"; break;
    }
    ctmp = orte_cbe_get_const(IDL_UNARYOP(tree).operand);
    g_string_printf(tmpstr, "%s%s", opc, ctmp);
    g_free(ctmp);
    break;
  case IDLN_IDENT:
    {
      char *id;
      id = IDL_ns_ident_to_qstring(IDL_IDENT_TO_NS(tree), "_", 0);
      g_string_printf(tmpstr, "%s", id);
      g_free(id);
    }
    break;
  default:
    g_error("We were asked to print a constant for %s", IDL_tree_type_names[tree->_type]);
    break;
  }

  retval = tmpstr->str;

  g_string_free(tmpstr, FALSE);

  return retval;
}

void
orte_cbe_write_const(FILE *of, IDL_tree tree)
{
  char *ctmp;

  ctmp = orte_cbe_get_const(tree);
  fprintf(of, "%s", ctmp);
  g_free(ctmp);
}

/* This is the WORST HACK in the WORLD, really truly, but the C preprocessor doesn't allow us to use
   strings, so we have to work around it by using individual characters. */
void
orte_cbe_id_define_hack(FILE *fh, const char *def_prefix, const char *def_name, const char *def_value)
{
  int i, n;
  n = strlen(def_value);
  for(i = 0; i < n; i++)
    fprintf(fh, "#define %s_%s_%d '%c'\n", def_prefix, def_name, i, def_value[i]);
}
