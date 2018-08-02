#define TYPEQUAL	case constsym: case volatilesym

#define STORECLASS 	case autosym:\
			case externsym:\
			case registersym:\
			case staticsym:\
			case typedefsym

#define TYPESPEC 	case charsym:\
			case doublesym:\
			case enumsym:\
			case floatsym:\
			case intsym:\
			case longsym:\
			case shortsym:\
			case signedsym:\
			case structsym:\
			case voidsym:\
			case unionsym:\
			case unsignedsym:\
			case TYPENAME


#define DECL  case '*':case ID: case '('



/* cdecl.c
-----------*/
int EiC_ext_decl(token_t *e1);
int EiC_type_name(token_t * e1);
void EiC_clearTempories(void);
token_t *EiC_genTemporay(type_expr *type, int level);
void EiC_free_un_mem(type_expr * e);

extern int EiC_INFUNC;


