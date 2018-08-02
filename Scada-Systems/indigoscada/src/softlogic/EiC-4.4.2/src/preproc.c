/* preproc.c
 *
 *	(C) Copyright Apr 15 1995, Edmond J. Breen.
 *		   ALL RIGHTS RESERVED.
 * This code may be copied for personal, non-profit use only.
 *
 *    Developed initially from
 *    p -- Small-C preprocessor by A.T. Schreiner 6/83.
 *             DDJ #93 (July) 1984.
 *
 *    However, many changes have been implemented and
 *    extensions made.
 *
 *              Grammar
 *  #define identifier token-sequence
 *  #define identifier(arg-list) token-sequence
 *  #undef  identifier
 *  #include <file-name>
 *  #include "file-name"
 *  #include token-sequence
 *  #if   constant-expr
 *  #elif constant-expr
 *  #ifdef  identifier
 *  #ifndef identifier
 *  #else
 *  #endif
 *  #error  token-sequence
 #  #pragma token-sequence
 #  #pragma 
 *
 *  The defined operator can be used in #if and #elif
 *  expressions only:
 *          #if defined(_EiC) && !defined(UNIX)
 *           ...
 *          #elif defined(MSDOS)
 *           ...
 *          #endif
 *
 *  token-sequence:
 *            [token-sequence] #token [token-sequence]
 *            [token-sequence] token##token [token-sequence]
 *            token-sequence token
 *
 *    arg-list:
 *           identifier [,identifier]*
 *
 * Predefined Macros:
 *
 *  __LINE__ The line number of the current source program, expressed
 *           as an integer.
 *  __FILE__ The name of the current source file, expressed as a
 *           string.
 *  __DATE__ resolves to a string literal containing the calender
 *           date in the form: Mmm dd yyyy.
 *  __TIME__ resolves to a string literal containing the current time
 *            in the form hh:mm:ss.
 *  __STDC__  resolves to 1.
 *
 */


#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <fcntl.h>
#include <time.h>

#include <sys/types.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <stddef.h>
#include <stdarg.h>
#include "stab.h"
#include "error.h"
#include "func.h"


#ifdef PPCLIB
int _stsptr,persist;
char *startstr;
char *ppcgets(char *str);
#endif



#ifdef _STANDALONE
#include "preproc.h"
int EiC_verboseON=1;
int showON = 1;


#define xmalloc(x)    malloc(x)
#define xcalloc(x,y)  calloc(x,y)
#define xrealloc(x,y) realloc(x,y)
#define xfree(x)      free(x)

#define xmark(x,y)

union VaL {
        char cval;              /* char value */
        int ival;               /* integer value */
	char *cpval;
	void *pval;
};
typedef union VaL val_t;

typedef struct {
        int n;
        val_t * val;
}eicstack_t;

typedef struct {
    char *id;
    int  token;
} keyword_t;

#include "preexpr.c"

#else
static int showON = 0;

#include "global.h"
#include "xalloc.h"
#include "preproc.h"
#include "symbol.h"

#endif

/* TODO: constant from eicmod.c needs header */
extern int EiC_showIncludes;

fitem_t *EiC_Infile = NULL;
int EiC_ptrSafe = 1;

static int file_cnt = 0;
static stab_t  FileNames = {NULL,0};
#define crt_fitem()   (fitem_t*)calloc(sizeof(fitem_t),1)
#define free_fitem(x)  free(x)

#define STRINGID -2


int EiC_IsIncluded(char *fname)
{
    return EiC_stab_FindString(&FileNames,fname) >= 0;
}

void EiC_showIncludedFiles(void)
{
    EiC_stab_ShowStrings(&FileNames);
}

void EiC_rmIncludeFileName(char *fname)
{
    EiC_stab_RemoveString(&FileNames,fname);
}

#if 0
/* this is the start of getting the :clear
 *  command to recursively remove the contents
 *  of an include file
 */

typedef struct nameTab {
    char * fname;
    char * inc;
    struct nameTab *nxt;
};

static struct nameTab *incTab = NULL;

static void EiC_addIncAssoc(char *fname, char *inc)
{
    struct nameTab *new = xcalloc(sizeof(new), 1);
    
    new->fname = fname;
    new->inc = inc;

    new->nxt = incTab;
    incTab = new;
}


#endif

int EiC_WithIn(char *fname)
{
    if(!EiC_Infile)
	return 0;
    else {
	fitem_t * f = EiC_Infile->next;
	while(f) {
	    if(strcmp(f->fname,fname) == 0)
		break;
	    f = f->next;
	}
	return f != NULL;
    }
    
}
    
    


static void NewInfile(int fd, char * fname, char *filep)
{
    fitem_t * f = crt_fitem();
    f->fd = fd;
    f->fname = EiC_stab_SaveString(&FileNames,fname);
    f->next = EiC_Infile;
    EiC_Infile = f;
    if(EiC_showIncludes) {
	int k = file_cnt;
	while(--k)
	  fputs("   ",stdout);
	if((filep && !*filep) || !filep)
	  filep = fname;
	fprintf(stdout,"%d:%s\n",file_cnt,filep);
    }
    file_cnt++;

}

static void NextInfile()
{
    if(EiC_Infile->next) {
	fitem_t * h = EiC_Infile->next;
	if(EiC_Infile->fd != STRINGID) {
	    close(EiC_Infile->fd);
	    if(EiC_Infile->buf)
		free(EiC_Infile->buf);
	}
	free(EiC_Infile);
	EiC_Infile = h;
	file_cnt--;
    }
}
	
static eicstack_t macros = {0, NULL};


#define Item(stack,i,item)   ((stack)->val[i].item)  


#define STDIN 	0
#define STDOUT 	1
#define STDERR  2

typedef struct {
    char * line;
    char * lp;
    unsigned len;
}Line;

static size_t NINCLUDES = 0;
static char **Include_prefixes = NULL;
static char *empty_string = " ";

typedef struct {
    char *id;          /* token name */
    char *tok_seq;     /* token replacement sequence */
    int nparms;        /* number of parameters */
    char *protect;     /* if one then no expansion before
			  replacement for a given parameter*/
    long unsigned hcode; /* `id' string hash code */
    char *fname;         /* file name pointer */
} macro_t;

static macro_t defmacro = {NULL, NULL, 0, NULL};


static int skip = 0;

char cmode;

static int linelen, olinelen, iflevel =0;
static char *line = NULL, *lp, *oline = NULL, *olp;

enum {
    DEFAULT, DEFINE,
    ELIF, ELSE, ENDIF, 
    IF, IFDEF, IFNDEF, INCLUDE,
    UNDEF, ERROR, PRAGMA
};

static keyword_t pwords[] =
{
{"define", DEFINE,},
{"elif", ELIF,},
{"else", ELSE,},
{"endif",ENDIF,},
{"error", ERROR,},
{"if", IF,},	
{"ifdef", IFDEF,},
{"ifndef",IFNDEF,},
{"include",INCLUDE,},
{"pragma", PRAGMA,},
{"undef", UNDEF,},
	
};

/** PROTOTYPES from preproc.c **/

static int Rgetc();
static void rebuff(char **buf, int *len);
static void in(char c);
static char *out(char c);
static char *outChar(char *s, char c, int mod, int i);
static int getline();
static void expr_list(eicstack_t *parms, char **p,int more);
static int findparm(eicstack_t *parms,char *name, size_t len);
static void mergeTokens(macro_t *mac);
static void parameterise(eicstack_t *parms);
static void markmacro(macro_t * mac, char mark);
static void kill_Items(eicstack_t *parms);
static void freemacro(macro_t * mac);
static void remmacroid(int k);
static void newmacro(eicstack_t *parms);
static void dodefmacro(char *s);
static int control_line(void);
static char * stringise(char * seq);
static char * EiC_expand(char *fld,char **end, int bot, int top);
static void process(void);

/* TODO: extern from eicmod.c needs header */
extern int EiC_verboseON;


static unsigned long get_hcode(unsigned char *s)
{
    unsigned long t, h,i;
    h = 0;
    while(*s) {
	for(t = i=0;*s && i<32;i+=8)
	    t |= (*s++ << i); 
	h += t;
    }
    return h;
}

               /*CUT preprocerror.cut*/
extern void EiC_pre_error(char *msg, ...)
{

    char *buff1, *buff2;
    va_list args;
    
    va_start(args,msg);
    buff1 = malloc(strlen(msg) + 256);
    buff2 = malloc(strlen(msg) + 512);
    
    EiC_errs++;
    EiC_ParseError = 1;
    sprintf(buff1,"Error in %s near line %d: %s\n",
	    CurrentFileName(),
	    CurrentLineNo(),
	    msg);
    vsprintf(buff2,buff1,args);
    EiC_messageDisplay(buff2);
    free(buff1);
    free(buff2);
    va_end(args);
}
              /*END CUT*/


static size_t ninclude = 0;
static size_t bot_stab = 0;

size_t EiC_pp_NextEntryNum(void)
{
    ninclude = NINCLUDES;
    bot_stab = EiC_stab_NextEntryNum(&FileNames);
    return macros.n;
}

size_t EiC_get_EiC_PP_NINCLUDES(void){ return NINCLUDES;}
size_t EiC_get_EiC_bot_stab(void) {return EiC_stab_NextEntryNum(&FileNames);}

void EiC_set_EiC_PPtoStart(int bot_stab, int ninclude)
{
    NINCLUDES = ninclude;
    FileNames.n = bot_stab;
}

void EiC_pp_CleanUp(size_t bot)
{
    iflevel = 0;
    skip = 0;
    /* clean up macros */
    while(macros.n > bot)
	remmacroid(macros.n-1);
    /* close opened files */
    while(EiC_Infile->next) 
	NextInfile();

    /* clean up path inclusion */
    while(NINCLUDES > ninclude)
	EiC_removepath(Include_prefixes[NINCLUDES - 1]);
    /* remove included file names */
    EiC_stab_CleanUp(&FileNames,bot_stab);
}


static unsigned putback = 0;
#define Ugetc(x)  (putback = x)
#define BUFSIZE   512


#if 1

static void doPragma(char *s)
{
  static  char * Stack = NULL;
  static  int N = 0;


  if(strstr(s,"push_safeptr")) {
    Stack = realloc(Stack,++N);
    Stack[N-1] = EiC_ptrSafe = 1;
  } else if(strstr(s,"push_unsafeptr")) {
    Stack = realloc(Stack,++N);
    Stack[N-1] = EiC_ptrSafe = 0;
  } else if(strstr(s,"pop_ptr") ) {
    if(N-2 >= 0) {
      N--;
      EiC_ptrSafe = Stack[N-1];
    } else
      EiC_ptrSafe = 1;
  } else
      EiC_formatMessage("Warning: Unrecognised pragma (%s): %s line  %d\n",
	      s,EiC_Infile->fname, EiC_Infile->lineno);
}
#endif


#if defined(_STANDALONE)

#define gchar(fd,c) (c = (read(fd,&c,1) > 0)? c : EOF)

static int rline(int fd, char *s, int limit)
{
    int c =0, i;
    limit--;
    for(i=0;i<limit && gchar(fd,c) != EOF && c != '\n';i++)
	s[i] = c;
    if(c == '\n')
	s[i++] = c;
    s[i] = '\0';
    return i;
}

static int Rgetc()
{
    int c;
    static int lastln = 0;

    if(putback) {
	c = putback;
	putback = 0;
    } else {
	if(EiC_Infile->n <= 0) {
	    if(showON && lastln && !skip && EiC_Infile->buf)
		fputs(EiC_Infile->buf,stdout);
	    EiC_Infile->n = 0;
	    if(EiC_Infile->buf == NULL)
		EiC_Infile->buf = (char *)malloc(BUFSIZE+1);
	    EiC_Infile->n = rline(EiC_Infile->fd,
				EiC_Infile->buf,
				BUFSIZE);
	    EiC_Infile->bufp = EiC_Infile->buf;
	    if(showON) {
		lastln = 0;
		if(!skip) 
		    fprintf(stdout,"%s",EiC_Infile->buf);
		else {
		    lastln  = 1;
		    putchar('\n');
		}
	    }
	}
	c = ((EiC_Infile->n-- > 0) ? *EiC_Infile->bufp++ : EOF);
    }
    return c;
}

#else

/* TODO: global from starteic.c needs header */
extern int EiC_Interact;


static int Rgetc()
{
    static char prompt[20];
    unsigned char * EiC_readline(char *);
    void EiC_add_history(unsigned char *);
    int c;
    #ifdef PPCLIB
    void prs(char *str);
    static char getsbuf[120];
    #endif
#ifdef WIN32
#define SBUFSIZE	120
    static char getsbuf[SBUFSIZE];
#endif


    if(putback) {
	c = putback;
	putback = 0;
    } else {
	if(EiC_Infile->n <= 0) {
	    if(EiC_Infile->fd != STDIN || !EiC_Interact) {
		if(EiC_Infile->buf == NULL)
		    EiC_Infile->buf = malloc(BUFSIZE+1);
		if(EiC_Infile->fd != STRINGID)
		    EiC_Infile->n = read(EiC_Infile->fd,
				     EiC_Infile->buf,BUFSIZE);
	    } else {

		#ifdef NO_READLINE
		fflush(stdout);
		fflush(stderr);
                sprintf(prompt,"\nEiC %d> ",EiC_Infile->lineno+1);
		
		  #ifdef PPCLIB
                prs(prompt);
                if(_stsptr!=-1) 
                { 
                  EiC_Infile->buf=startstr; 
                  _stsptr=-1; 
                  }
                else 
		  #endif
		      EiC_Infile->buf = ppcgets(getsbuf);
                if(EiC_Infile->buf && *EiC_Infile->buf) {
                      EiC_Infile->n = strlen(EiC_Infile->buf);
                      EiC_Infile->buf[EiC_Infile->n] = '\n';
                      EiC_Infile->n++;
                  } else
                      EiC_Infile->n = 0;
 
               #else

		if(EiC_Infile->buf) {
		    free(EiC_Infile->buf);
		    EiC_Infile->buf = NULL;
		}

		sprintf(prompt,"EiC %d> ",EiC_Infile->lineno+1);
		EiC_Infile->buf = EiC_readline(prompt);
		if(EiC_Infile->buf && *EiC_Infile->buf) {
		    EiC_add_history(EiC_Infile->buf);
		    EiC_Infile->n = strlen((char*)EiC_Infile->buf);
		    EiC_Infile->buf[EiC_Infile->n] = '\n';
		    EiC_Infile->n++;
		} else
		    EiC_Infile->n = 0;
		#endif
	    }
	    EiC_Infile->bufp = EiC_Infile->buf;
	}
	c = ((EiC_Infile->n-- > 0) ? *EiC_Infile->bufp++ : EOF);
    }
    return c;
}
#endif /* _STANDALONE */

char *EiC_prolineString(char *str)
{
    NewInfile(STRINGID,"STRING","::EiC::");
    EiC_Infile->n = strlen(str);
    EiC_Infile->buf=EiC_Infile->bufp=(unsigned char*)str;
    return str;
}

static void rebuff(char **buf, int *len)
{
    *buf = (char *) realloc(*buf, (*len + REBUFF_INCREMENT) * sizeof(char));
    *len += REBUFF_INCREMENT;
}

static void in(char c)
{
    *lp++ = c;
    if (lp >= line + linelen) {
	ptrdiff_t d;
	d = lp - line;
	rebuff(&line, &linelen);
	lp = line + (size_t) d;
    }
}

static char *out(char c)
{
    *olp++ = c;
    if (olp >= oline + olinelen) {
	ptrdiff_t d;
	d = olp - oline;
	rebuff(&oline, &olinelen);
	olp = oline + (size_t) d;
    }
    *olp = '\0';
    return olp - 1;
}

static char *outChar(char *s, char c, int mod, int i)
{
    if(!(i%mod)) {
	if(!s) 
	    s = (char *)xmalloc(sizeof(char)*(i+mod + 1));
	else
	    s = (char *)xrealloc(s,sizeof(char)*(i+mod + 1));
    }
    s[i] = c;
    return s;
}

#ifdef _STANDALONE
#define EiC_saveComment() NULL
#endif

static int getline()
{
    /* automatically strips out comments and
     * performs line splicing.
     */
    char c;
    int lcom = 0;
    lp = line;

    while(1) {
	switch ((c = Rgetc())) {
	case '\\': /* look for line continuation */
	    switch ((c = Rgetc())) {
	    case EOF:
		EiC_pre_error("Unexpected end of file");
	    case '\n':
		++EiC_Infile->lineno;
		continue;
	    case '\r': /* ignore carriage returns */
	      if((c = Rgetc()) == '\n') {
		++EiC_Infile->lineno;
		continue;
	      }
	    }
	    in('\\');

	default:
	    if(!isspace(c) || lp != line) /* skip leading white */
		in(c);			       /* space */
	    continue;
	case EOF:
	    if(iflevel && file_cnt == 2)
		EiC_pre_error("unterminated `#if' conditional");
	    ++EiC_Infile->lineno;
	    if (lp != line)
		break;
	    if (EiC_Infile->next) {
		NextInfile();
		if(EiC_Infile->next)
		    continue;
	    }else if(!EiC_Interact) {
		int EiC_exit_EiC();
		EiC_exit_EiC();
	    }

	    break;
	case '/':
	    if (cmode == 0) { /* check for comment*/
		if((c = Rgetc()) == '*' || c == '/')  {
		    int i = 0;
		    void (*sC) (char *s);
		    char *comment = NULL;
		    
		    if(c == '/') /* Allow for C++ style comments */
			lcom = 1;
		    /* strip comment out */
		    c = Rgetc();
		    sC = EiC_saveComment();
		    if(lp != line)
			in(' ');  /* replace comment with space */
		    while(c != EOF) {
			if(sC) 
			    comment = outChar(comment,c,5,i++);
			if(c == '\n') {
			    if(lcom == 1) {
				Ugetc(c);
				lcom = 0;
				break;
			    } else
				++EiC_Infile->lineno;
			}
			if(c == '*' && !lcom) {
			    if((c = Rgetc()) == '/') 
				break;
			} else if (c == '\\' && lcom) {
			    /* allow for line splicing */
			  c = Rgetc(); /* waste next char */
			  if(c=='\r')
			    c=Rgetc(); 
			  c = Rgetc();
			} else
			    c = Rgetc();
		    }
		    if(sC) {
			comment = outChar(comment,'\0',5,i++);
			sC(comment);
		    }
			
		} else {
		    in('/');
		    if(c == '\n')
			++EiC_Infile->lineno;
		    if(c != EOF)
			in(c);
		}
	    } else
		in('/');
	    continue;
	case '\"': /* " */
	    if (cmode == 0)
		cmode = CMstr;
	    else if (cmode == CMstr)
		cmode = 0;
	    in('\"');  /* " */
	    continue;
	case '\'':
	    if (cmode == 0)
		cmode = CMchr;
	    else if (cmode == CMchr)
		cmode = 0;
	    in('\'');
	    continue;
	case '\r': /* ignore carriage returns */
	  continue;
	case '\n':
	    ++EiC_Infile->lineno;
	    if(lp == line && EiC_Infile->next ) {
		continue;
	    }
	    if (cmode == CMstr) {
		if (!skip)
		    EiC_pre_error("unbalanced \"");  /* " */
		else
		    in('\"');  /* " */
		cmode = 0;
	    } else if (cmode == CMchr) {
		if (!skip)
		    EiC_pre_error("unbalanced \'");
		else
		    in('\'');
		cmode = 0;
	    }
	}
	break;
    }
    *lp = '\0';

    return 1;
}

static void expr_list(eicstack_t *parms, char **p,int more)
{
    extern char *EiC_strsave(char *);
    int c = 0, b = 0;

    char *s = *p;
    static unsigned sz =BUFSIZE;
    static char *str = NULL;
    
    val_t v;
    if(!str)
	str = (char*)malloc(BUFSIZE);
    while (1) {
	for (; *s; ++s) {
	    if(isspace(*s) && !cmode) {
		while(*s && isspace(*s)) s++;
		if(!*s)
		    break;
		s--;
	    }
	    if(c == sz) {
		sz += BUFSIZE;
		str = realloc(str,sz);
	    }
	    str[c++] = *s;
	    switch (*s) {
	      case '\\':
		str[c++] = *++s; /*get next char */
		continue;
	      case '{': case '(':
		if (cmode == 0) ++b;
		continue;
	      case ',':
		if (cmode || b) continue;
		--c;
		break;
	      case '}': case ')':
		if(b) {b--;continue;}
		if (cmode) continue;
		--c;
		break;
	      case '\'':
		switch (cmode) {
		  case 0: cmode = CMchr;
		  case CMstr: continue;
		}
		cmode = 0;
		continue;
	      case '\"':  /* " */
		switch (cmode) {
		  case 0: cmode = CMstr;
		  case CMchr: continue;
		}
		cmode = 0;
		continue;
	      default: continue;
	    } /* end switch */
	    break;
	} /* end for loop */

	if(!b && *s) {
	    str[c] = '\0';
	    /*strip leading white space */
	    c = 0;
	    while(str[c] && isspace(str[c]))
		c++;
	    v.p.p =  EiC_strsave(&str[c]);
	    EiC_eicpush(parms, v);
	}
	if(!*s && more) { /*need more input*/
	    if(EiC_Infile->fd == STDIN) {
		EiC_pre_error("Illegal line continuation during macro expansion");
		break;
	    }
	    getline();
	    if(c && !isspace(str[c-1]) && !isspace(*line))
		str[c++] = ' ';  /* need white space */
	    s = line;
	    continue;
	}
	
	if (*s == ')')
	    break;
	if (*s != ',') {
	    EiC_pre_error("Illegal macro definition");
	    break;
	}
	c = 0;
	s++;
    } /* end while */
    *p = ++s;
    if(sz > BUFSIZE << 2) {
	sz = BUFSIZE;
	str = realloc(str,sz);
    }
	
}

static int findparm(eicstack_t *parms,char *name, size_t len)
{
    int i;
    val_t *v;
    for (v = parms->val, i = 1; i <= parms->n; i++, v++)
	if (strncmp(v->p.p, name, len) == 0)
	    return i;
    return 0;
}

static void mergeTokens(macro_t *mac)
{
    char * s, *seq;
    int left, right;

    if(mac->nparms) {
	mac->protect = xcalloc(mac->nparms + 1,sizeof(char));
	/*printf("%s id %ld\n",mac->id,tot_seen);*/
    }

    s = seq  = mac->tok_seq;
    while(*s) {
	if(!cmode && *s == '#' && *(s+1) == '#') {
	    int d = (int)(s - seq) - 1;
	    while(d >= 0 && seq[d] > 0 && isspace(seq[d]))
		d--;
	    if(d < 0)
		EiC_pre_error("macro definition begins with ##");
	    left = d;
	    d = (int)(s - seq) + 2;
	    while(seq[d] > 0 && isspace(seq[d]))
		d++;	
	    if(!seq[d])
		EiC_pre_error("macro definition ends with ##");
	    right = d;
	    s = seq + left + 1;
	    
	    
	    if(seq[left] < 0)
		mac->protect[-seq[left]] = 1;
	    if(seq[right] < 0)
		mac->protect[-seq[right]] = 1;
	    
	    while(seq[left] != 0)
		seq[++left] = seq[right++];
	} else if (*s == '"') { 
	    if (!cmode)
		cmode = CMstr;
	    else if (cmode == CMstr)
		cmode = 0;
	} else if (*s == '\'') {
	    if (!cmode)
		cmode = CMchr;
	    else if (cmode == CMchr)
		cmode = 0;
	}
	s++;
    }
}

static void parameterise(eicstack_t *parms)
{
    char *s, *id, *op;
    int i;
    
    cmode = 0;

    op = s = defmacro.tok_seq;
    
    while (*s) {
	if (!cmode && (isalpha(*s) || *s == '_')) {
	    id = s++;
	    while (isalnum(*s) || *s == '_')
		++s;
	    if ((i = findparm(parms, id, (size_t) (s - id))) != 0)
		*op++ = -i;
	    else
		while (id < s)
		    *op++ = *id++;
	} else {
	    if (*s == '"') { 
		if (!cmode)
		    cmode = CMstr;
		else if (cmode == CMstr)
		    cmode = 0;
	    } else if (*s == '\'') {
		if (!cmode)
		    cmode = CMchr;
		else if (cmode == CMchr)
		    cmode = 0;
	    } else if (isspace(*s) && !cmode) {
		do
		    s++;
		while(*s && isspace(*s));
		*--s = ' '; 
	    }
	    *op++ = *s++;
	}
    }
    *op = '\0';
    if (cmode) {
	if (cmode == CMstr)
	    EiC_pre_error("Missing end \" in macro token sequence");
	else
	    EiC_pre_error("Missing end ' in macro token sequence");
    } else
	mergeTokens(&defmacro);
}

#ifndef _STANDALONE
static void markmacro(macro_t * mac, char mark)
{
    xmark(mac, mark);
    xmark(mac->id, mark);
    if(mac->protect)
	xmark(mac->protect,mark);
    if(mac->tok_seq != empty_string)
	xmark(mac->tok_seq, mark);
    EiC_stab_Mark(&FileNames,mark);
}

void EiC_markmacros(char mark)
{
    macro_t *mac;
    val_t *v;
    int i;

    if (macros.n) {
	xmark(macros.val, mark);
	v = macros.val;
	for (i = 0; i < macros.n; ++i, ++v) {
	    mac = v->p.p;
	    markmacro(mac, mark);
	}
    }

    for(i=0;i<NINCLUDES;++i)
	xmark(Include_prefixes[i],mark);
}
#endif

static void Check4Res(macro_t * mac)
{
    char str[20];
    char * itoa(int,char *s, int);
    int c = 0, q = 1;
    char * s = NULL;
    if(mac->id[0] == '_' && mac->id[1] == '_') {
	switch(mac->id[2]) {
	  case 'F':
	    if(strcmp(mac->id,"__FILE__") == 0) {
		c = 1; s = EiC_Infile->fname;
		if(strcmp(s,mac->tok_seq) == 0)
		    return;
	    }
	    break;
	case 'L':    
	    if(strcmp(mac->id,"__LINE__")== 0) {
		sprintf(str,"%d",EiC_Infile->lineno);
		s = str;
		c = 1;
		q = 0;
	    }
	    break;
	  case 'D':
	  case 'T':
	    if(strcmp(mac->id,"__DATE__") == 0 ||
	       strcmp(mac->id,"__TIME__") == 0) {
		time_t t = time(NULL);
		char * ct = ctime(&t);
		c = 1; s = str;
		if(mac->id[2] == 'D') { 
		    strncpy(str, ct+4, 7);
		    strncpy(&str[7], ct+20, 4);
		    str[11] = 0;
		} else {
		    strncpy(str, ct+11, 8);
		    str[8] = 0;
		}
	    }
	    break;
/*	  case 'S':
	    if(strcmp(mac->id,"__STDC__")== 0) {
		str[0] = '1'; str[1] = 0;
		s = str;
		c = 1;
	    }
	    break;
*/
	}
	if(c && s) {
	    char * p;
	    xfree(mac->tok_seq);
	    p = mac->tok_seq = (char*)xmalloc(strlen(s) + 3);
	    xmark(mac->tok_seq,eicstay);
	    if(q)
		*p++ = '\"';
	    while(*s)
		*p++ = *s++;
	    if(q)
		*p++ ='\"';
	    *p='\0';
	}
	
    }
}

static void displayMacro(int k, char *id)
{
    macro_t *mac  = macros.val[k].p.p;
    char *s = mac->tok_seq;
    int c,p;
    printf("%s -> #define %s",id,id);
    if(mac->nparms) {
	putchar('(');
	for(p = 0;*s != 0;++s)
	    if(*s < p)
		p = *s;
	s = mac->tok_seq;
	p = abs(p);
	for(k=0; k<p;++k) {
	    c = 'a'+k;
	    putchar(c);
	    if(k+1 < mac->nparms)
		putchar(',');
	}
	putchar(')');
    }
    putchar('\t');
    for(k = 0;*s != 0;++s,++k) 
	if(*s < 0) {
	    if(k) 
		if(*(s-1) > EOF && isalnum(*(s-1)))
		    printf(" ## ");
	    c = 'a' - *s - 1;
	    putchar(c);
	    if(*(s+1) && (*(s+1) < 0 || isalnum(*(s+1))))
		printf(" ## ");
	} else
	    putchar(*s);

    putchar('\n');
}    

int EiC_showMacro(char * id)
{
    int k;
    if((k = EiC_ismacroid(id)) >= 0) {
	displayMacro(k,id);
	return 1;
    }
    
    return 0;
}

void EiC_showFileMacros(char *fname)
{
    macro_t *mac;
    val_t *v;
    int i;
    v = macros.val;
    for (i = 0; i < macros.n; ++i, ++v) {
	mac = v->p.p;
	if (strcmp(mac->fname, fname) == 0) 
	    displayMacro(i,mac->id);
    }
}


void EiC_ClearFileMacros(char *fname)
{
    /* clear all the macros from the macro lut
     * that were entered via file `fname'
     */
    macro_t *mac;
    val_t *v;
    int i, *ind = NULL, cnt = 0;
    
    v = macros.val;
    for (i = 0; i < macros.n; ++i, ++v) {
	mac = v->p.p;
	if (strcmp(mac->fname, fname) == 0) {
	    if(cnt)
		ind = xrealloc(ind,sizeof(int) * (cnt + 1));
	    else
		ind = xmalloc(sizeof(int));
	    ind[cnt++] = i;
	}
    }
    for(i = cnt; i>0;i--)
	remmacroid(ind[i-1]);
    if(ind)
	xfree(ind);
}

int EiC_ismacroid(char *id)
{
    unsigned long hc;
    macro_t *mac;
    val_t *v;
    int i;
    v = macros.val;
    hc = get_hcode((unsigned char *)id);
    for (i = 0; i < macros.n; ++i, ++v) {
	mac = v->p.p;
	if (hc == mac->hcode && strcmp(mac->id, id) == 0) {
	    Check4Res(mac);
	    return i;
	}
    }
    return -1;
}

static void kill_Items(eicstack_t *parms)
{
    if (parms->n) {
	int i;
	for (i = 0; i < parms->n; i++)
	    xfree(parms->val[i].p.p);
	xfree(parms->val);
	parms->n = 0;
    }
    parms->val = NULL;
}

static void freemacro(macro_t * mac)
{
    if (mac->id) {
	xfree(mac->id);
	mac->id = NULL;
    }
    if (mac->tok_seq) {
	if(mac->tok_seq != empty_string)
	    xfree(mac->tok_seq);
	mac->tok_seq = NULL;
    }
    if(mac->protect)
	xfree(mac->protect);
}

static void remmacroid(int k)
{
    val_t v;
    macro_t *mac;

    mac = macros.val[k].p.p;
    freemacro(mac);
    if(macros.n) {
	if(k < macros.n-1) {
	    memmove(&macros.val[k],
		   &macros.val[k + 1],
		   ((macros.n-1) - k) * sizeof(val_t));
	}
	/* Throw away last item on stack*/
	EiC_eicpop(&macros,&v);
    } 
    xfree(mac);
}


static void newmacro(eicstack_t *parms)
{

    macro_t *new;
    int k;
    k = EiC_ismacroid(defmacro.id);
    if (k > -1) {
	macro_t *old;
	old = macros.val[k].p.p;
	if ((old->nparms != parms->n) ||
	    !((old->tok_seq == empty_string && !*defmacro.tok_seq) ||
	    strcmp(old->tok_seq, defmacro.tok_seq) == 0)) {
	    EiC_pre_error("Re-declaration of macro %s",defmacro.id);
	}
	if(defmacro.protect)
	    xfree(defmacro.protect);
    } else {
	val_t v;
	new = (macro_t *) xcalloc(1, sizeof(macro_t));
	defmacro.id = EiC_strsave(defmacro.id);
	defmacro.hcode = get_hcode((unsigned char *)defmacro.id);
	defmacro.fname = CurrentFileName();
	if(*defmacro.tok_seq)
	    defmacro.tok_seq = EiC_strsave(defmacro.tok_seq);
	else /* allow for empty macro */
	    defmacro.tok_seq = empty_string;
	defmacro.nparms = parms->n;
	*new = defmacro;
	v.p.p = new;
	EiC_eicpush(&macros, v);
    }
    defmacro.protect = defmacro.id = defmacro.tok_seq = NULL;
}

extern void dodefine(char *def)
{
    /*
     * for processing -Dname[=value] switch
     * def = name[=value]
     */
    if(def) {
	char * p;
	int i = strlen(def);
	char * str = xmalloc(i+3);
	memcpy(str,def,i+1);
	for(p = str;*p && *p != '=';++p)
	    ;
	if(*p) {
	    *p = ' ';
	    str[i] = 0;
	}else {
	    str[i] = ' ';
	    str[i+1] = '1';
	    str[i+2] = 0;
	}
	dodefmacro(str);
	xfree(str);
    }
}	
    
static void dodefmacro(char *s)
{
    eicstack_t parms = {0, NULL};
    skipfws(s);
    defmacro.id = s;
    while (*s && !isspace(*s) && *s != '(')
	++s;
    if (*s == '(') {
	*s++ = '\0';
	expr_list(&parms,&s,0);
    } else
	if(*s)
	    *s++ = '\0';
    skipfws(s);
    defmacro.tok_seq = s;
    defmacro.nparms = parms.n;
    skipall(s);
    --s;
    skipbws(s);
    *++s = '\0';
    if (parms.n != 0)
	parameterise(&parms);
    newmacro(&parms);
    kill_Items(&parms);
}

int EiC_Include_file(char *e,   /* name of file to Include for */
		 int mode)  /* if 1, look locally first */
{   /* returns 1 on success else it returns 0 */
    
    int i,fd;
    char fname[512] = {0};
    if(mode == 1) /* look in current directory first */
	fd = open(e,O_RDONLY);
    else
	fd = -1;
    for(i=0;i<NINCLUDES && fd < 0;i++) {
	strcpy(fname,Include_prefixes[i]);
	strcat(fname,"/");
	strcat(fname,e);
	if(EiC_verboseON)
	    printf("looking for %s:%d\n",fname,file_cnt);
	fd = open(fname, O_RDONLY);
    }
    if (fd >= 0)
	NewInfile(fd,e,fname);
    else
	return 0;
    return fd;
}

static int control_line(void)
{
    char key[25];
    int k;   
    char *s, *e;
    if (*line == '#') {
	s = line + 1;

	skipfws(s);

	if(!*s) /* test for null directive */
	    return 1;
	
	for(k=0;isalpha(*s);k++)
	    key[k] = *s++;

	key[k] = '\0';
	k = EiC_iskeyword(pwords, key, sizeof(pwords)
		      / sizeof(keyword_t));
	skipfws(s);
	switch (k) {
	  case DEFINE:
	    if (!skip) {
		if(*s)
		    dodefmacro(s);
		else
		    EiC_pre_error("empty '#define' directive"); 
		}
	    break;
	  case ELIF:
	    if(skip && skip == iflevel) {
		if(EiC_cpp_parse(s)) 
		    skip = 0;
	    } else if(!skip && iflevel)
		skip = iflevel + 1;
	    break;
	  case ELSE:
	    if (iflevel == 0)
		EiC_pre_error("Unmatched #else");
	    else if (skip == iflevel)
		skip = 0;
	    else if (skip == 0)
		skip = iflevel;
	    break;
	  case ENDIF:
	    if (iflevel == 0)
		EiC_pre_error("Unmatched #endif");
	    else {
		if (skip >= iflevel)
		    skip = 0;
		--iflevel;
	    }
	    break;
	  case IF:
	    ++iflevel;
	    if(!skip) {
		if(*s) {
		    if(!EiC_cpp_parse(s))
			skip = iflevel;
		} else
		    EiC_pre_error("empty '#if' directive");
		}
	    break;
	  case IFDEF:
	  case IFNDEF:
	    ++iflevel;
	    if (!skip) {
		if (isalpha(*s) || *s == '_') {
		    e = s;
		    skipnws(s);
		    *s = '\0';
		    if (EiC_ismacroid(e) > -1) {
			if (k == IFNDEF)

			    skip = iflevel;
		    } else if (k == IFDEF)
			skip = iflevel;
		} else
		    EiC_pre_error("Illegal macro identifier");
	    }
	    break;
	  case INCLUDE:
	{
	    if(skip) break;
	    if(!*s) {
		EiC_pre_error("empty '#include' directive");
		break;
	    }
	    if (*s == '\"') /* " */
		s++, cmode = CMstr;
	    else if (*s == '<')
		s++, cmode = CMang;
	    else
		cmode = 0;
	    e = s;
	    skipnws(s);
	    if (cmode) {
		if (cmode == CMstr && *(s - 1) != '\"')
		    EiC_pre_error("Missing \"");
		else if (cmode == CMang && *(s - 1) != '>')
		    EiC_pre_error("Missing >");
		*--s = '\0';
	    } else {		/* token_sequence */
		lp = line;
		while (e != s)
		    *lp++ = *e++;
		*lp = '\0';
		process();
		e = oline;
	    }

	    if(!EiC_Include_file(e,(cmode != CMang)))
		EiC_pre_error("failed to open include file %s",e);
	    cmode = 0;
	   }
	  case UNDEF:
	    if (!skip) {
		e = s;
		skipnws(s);
		*s = '\0';
		k = EiC_ismacroid(e);
		if (k > -1)
		    remmacroid(k);
	    }
	    break;
	  case ERROR:
	    if(!skip) {
		char *S = EiC_process2(s,0,0);
		EiC_pre_error(S);
		if(S)
		    xfree(S);
	    }
	    break;
	  case PRAGMA:  
	    if(!skip) {
	      char *S = EiC_process2(s,0,0);
	      if(S) {
		doPragma(S);
		xfree(S);
	      }
	    }

	      break;
	default: {
		  extern int ScriptMode;
		  if(!ScriptMode) 
		      EiC_pre_error("undefined or invalid # directive");
	      }
	      break;
	}
	return 1;
    }
    return skip;
}

#define TopMax 256
#define MoD  10
static int forbid[TopMax];

static char * stringise(char * seq)
{
    int i = 0;
    char *S=NULL;

    S = outChar(S,'\"',MoD,i++); /* " */
    while(*seq) {
	if(*seq == '\"' || *seq == '\\')  /* " */
	    S = outChar(S,'\\',MoD,i++);
	S = outChar(S,*seq++,MoD,i++);
    }
    S = outChar(S,'\"',MoD,i++); /* " */
    S[i]='\0';
    return S;
}

static char * EiC_expand(char *fld,char **end, int bot, int top)
{
    char word[128];
    int i = 0, k;
    char *p, *p2;
    macro_t *mac;
    char *S =NULL;
    eicstack_t parms = {0, NULL};
    
    for(i=0;*fld && (isalnum(*fld) || *fld == '_');++i)
	word[i] = *fld++;
    word[i]=0;

    if(end)
	*end = fld;
    if ((k = EiC_ismacroid(word)) < 0) 
	return NULL;
    for(i=bot;i<top;i++) 
	if(k == forbid[i])
	    return NULL;
    forbid[top++] = k;
    skipfws(fld);
    mac = macros.val[k].p.p;
    if (mac->nparms > 0) {
	if (*fld != '(')
	   return NULL; /**/
	else { /* collect arguments */
	    ++fld;
	    expr_list(&parms,&fld,1);
	}
	if(end)
	    *end = fld;
    }
    if (parms.n != mac->nparms)
	EiC_pre_error("Macro syntax error");
    else {
	char * t, *s;
	p = mac->tok_seq;
	/* Now substitute in arguments and
	 * expand as necessary
	 */
	i = 0; S = NULL;
	while (*p) {
	    if (*p < 0) {
		if(mac->protect && mac->protect[-*p] == 1)
		    p2 = NULL;
		else  if((i && S[i-1] == '#') || (i >= 2 && S[i-2] == '#')) {
		    p2 = stringise(Item(&parms,-*p - 1,p.p));
		    if(S[i-1] == '#')
			i--;
		    else
			i -= 2;
		} else 
		    p2 = EiC_process2(Item(&parms,-*p-1,p.p),top,top);
		if(p2) {
		    char *p = p2;
		    while (*p2)
			S = outChar(S,*p2++,MoD,i++);
		    xfree(p);
		} else {
		    p2 = Item(&parms,-*p - 1,p.p);
		    while (*p2)
			S = outChar(S,*p2++,MoD,i++);
		}
		p++;
	    } else
		S = outChar(S,*p++,MoD,i++);
	    
	}
	if(S) { /* Now rescan macro definition */
	    char *S2=NULL;
	    int k = 0;
	    if(mac->nparms > 0)
	    do {
		/* catch possible new macro funcs */
		/* bit of hack, but seems to work */
		/* should really check also for */
		/* brackets in strings and char literals */
		while(*fld && isspace(*fld)) 
		    fld++;
		while(*fld == '(') {
		    int parens = 0;
		    do {
			S = outChar(S,*fld,MoD,i++);
			if(*fld == '(')
			    parens++;
			else if(*fld == ')')
			    parens--;
			if(! *++fld && parens) {
			    /* need more input */
			    if(EiC_Infile->fd == STDIN) {
				EiC_pre_error("Illegal line continuation "
					  "during macro expansion");
				break;
			    }
			    getline();
			    fld = line;
			}
		    } while(*fld && parens);
		    if(parens)
			EiC_pre_error("Missing `)'");
		    *end = fld;
		}
	    } while(isspace(*fld));

	    s = S;
	    S[i] = 0;
	    while(*s) {
		if (*s == '"') {
		    if (!cmode)
			cmode = CMstr;
		    else if (cmode == CMstr)
			cmode = 0;
		} else if (*s == '\'') {
		    if (!cmode)
			cmode = CMchr;
		    else if (cmode == CMchr)
			cmode = 0;
		} else if(*s == '\\') 
		    S2 = outChar(S2,*s++,MoD,k++);
		else if((isalpha(*s) || *s == '_') && !cmode) {
		    t = EiC_expand(s,&p,bot,top);
		    if(t) {
			char * h = t;
			while(*t)
			    S2 = outChar(S2,*t++,MoD,k++);
			xfree(h);
			s = p;
		    } else 
			while(s < p)
			    S2 = outChar(S2,*s++,MoD,k++);
		    continue;
		}
		S2 = outChar(S2,*s++,MoD,k++);
	    }
	    S2[k] = 0;
	    xfree(S);
	    S = S2;
	}
    }	
    kill_Items(&parms);
    return S;
}


char * EiC_process2(char * line,int bot,int top)
{
    int k = 0;
    char *p, *s, *S = NULL;
    char * lp = line;

    while (*lp)
	if (!cmode && (isalpha(*lp) || *lp == '_')) {
	    s = lp;
	    p = EiC_expand(lp, &lp,bot,top);
	    if(p) {
		s = p;
		while(*p)
		    S = outChar(S,*p++,MoD,k++);
		xfree(s);
	    } else
		while(s != lp)
		    S = outChar(S,*s++,MoD,k++);
	} else {
	    if(*lp == '\'') {
		if (cmode == 0)
		    cmode = CMchr;
		else if (cmode == CMchr)
		    cmode = 0;
	    }
	    if (*lp == '\"') {	/* " */
		if (cmode == 0)
		    cmode = CMstr;
		else if(cmode == CMstr)
		    cmode = 0;
	    } else if (*lp == '\\'  && (cmode == CMstr || cmode == CMchr) ) {
		S = outChar(S,*lp++,MoD,k++);
		if (!*lp)	/* get second char */
		    break;
	    }
	    S = outChar(S,*lp++,MoD,k++);
	}
    if(S)
	S[k] = '\0';
    return S;
}

static void cp2out(char *S)
{
    if(S) {
	while(*S)
	    out(*S++);
    }
}

static void process(void)
{
    char *S;
    *(olp = oline) = '\0';
    S = EiC_process2(line,0,0);
    if(S) {
	cp2out(S);
	xfree(S);
    }
    cmode = 0;
}    

int EiC_setinfile(char * fname)
{
    /* look in current directory */
    char name[100] = {0,};
    int fd = open(fname,O_RDONLY);
    
    if(fd < 0) {
	sprintf(name,"%s/%s",getenv("HOME"),fname);
	fd = open(name,O_RDONLY);
	if(fd < 0) {
	    /* look in search path include directories */
	    fd = EiC_Include_file(fname,0);
	    return fd;
	}
    }
    if(fd > 0) 
	NewInfile(fd,fname,name);
    return fd;
}

extern char *EiC_nextproline(void)
{
    extern int EiC_SHOWLINE;
   
    while (1) {
	getline();
	if(line[0] != ':') {
	    if (!control_line()) {
		process();
		break;
	    } else if (EiC_Infile->fd == STDIN) {
		olp = oline;
		break;
	    }
	} else if(!skip) { /* else got an EiC command line */
	    *(olp = oline) = '\0';
	    cp2out(line);
	    break;
	}
    }
    if (EiC_Infile->fd == STDIN)
	out(EOF);
    if (EiC_SHOWLINE && !showON) {
	out('\0');
	fputs(oline,stdout);
	fputc('\n',stdout);
    }
    return oline;
}

int EiC_insertpath(char *path)
{
    /* Adds path to include path list */

    int i;
    /*
     * don't append the same path more than once
     */
    for(i=0;i<NINCLUDES;++i)
	if(strcmp(path,Include_prefixes[i]) == 0)
	    return 1;
    Include_prefixes = realloc(Include_prefixes,
			       sizeof(char*)*(NINCLUDES+1));
    Include_prefixes[NINCLUDES] = EiC_strsave(path);
    if(!Include_prefixes[NINCLUDES])
	return 0;
    NINCLUDES++;
    return 1;

}

int EiC_removepath(char *path)
{
    int i,k;
    for(i=0;i<NINCLUDES;++i) 
	if(Include_prefixes[i] &&
	             strcmp(path,Include_prefixes[i])==0) {
	    xfree(Include_prefixes[i]);
	    for(k=i;k<NINCLUDES-1;k++)
		Include_prefixes[k] = Include_prefixes[k+1];
	    NINCLUDES--;
	    return 1;
	}
    return 0;
}

void EiC_listpath(void)
{
    int i;
    for(i=0;i<NINCLUDES;++i)
	printf("%s\n",Include_prefixes[i]);
}

int EiC_initpp(void)
{
    static int ftime = 1;
    NewInfile(STDIN,"::EiC::","::EiC::");
    if (line)
	free(line);
    if (oline)
	free(oline);
    line = (char *)  calloc(REBUFF_INCREMENT, sizeof(char));
    oline = (char *) calloc(REBUFF_INCREMENT, sizeof(char));
    linelen = olinelen = REBUFF_INCREMENT;
    if (!line || !oline)
	return 0;
    if(ftime) {
	dodefine("__FILE__=emp");
	dodefine("__LINE__=emp");
	dodefine("__DATE__=emp");
	dodefine("__TIME__=emp");
	dodefine("__STDC__=1");
	ftime = 0;
    }
    	return 1;
}

#ifdef _STANDALONE

/* TODO: eicmod.c global needs header */
extern int EiC_showIncludes = 0;

static int EiC_iskeyword(keyword_t *keywords,char*id,int n)
{
    int i;
    for(i=0;i<n;i++)
	if(strcmp(keywords[i].id,id) == 0)
	    return keywords[i].token;
    return 0;
}

static void EiC_eicpush(eicstack_t *s, val_t v)
{
    if(!(s->n%2)) {	
	if(!s->n)
	    s->val = (val_t*)xcalloc(sizeof(val_t),2);
	else
	    s->val = (val_t*)xrealloc(s->val,(s->n+2)*sizeof(val_t));
    }
    s->val[s->n] = v;
    s->n++;
}

static int EiC_eicpop(eicstack_t *s, val_t *pop)
{
    if(s->n == 0)
	return 0;
    s->n--;
    *pop = s->val[s->n];
    if(!(s->n%2)) {		
	if(!s->n)
	    xfree(s->val);
	else
	    s->val = (val_t*)xrealloc(s->val,s->n*sizeof(val_t));
    }
    return 1;
}

static extern char * EiC_strsave(char *s)
{
    char *p;
    if((p = (char*)xcalloc(strlen(s)+1,sizeof(char))) != NULL)
	strcpy(p,s);
    return(p);
}

/* TODO: eicmod.c global needs header */
int EiC_SHOWLINE = 1;

static int do_sw_commands(char *cp)
{
    switch(*cp++) {
      case 'D': dodefine(cp);  return 1;
      case 'I': EiC_insertpath(cp);return 1;
      case '\?':
      case 'r':
      case 'R': EiC_setinfile(cp);  return 1;
      case 's':
      case 'S': EiC_verboseON = 0; return 1;
      default:
	return 0;
    }
}

void main(int argc, char ** argv)
{
    char *buf;
    EiC_initpp();
    if(argc > 1)
	while(argv[1]) {
	    if(argv[1][0] == '-') {
		if(!do_sw_commands(&argv[1][1])) {
		    fprintf(stderr,"Unknown switch command [%s]\n",argv[1]);
		    exit(0);
		}
	    } else {
		fprintf(stderr,"Unknown switch command [%s]\n",argv[1]);
		exit(0);
	    }
	    argv++;
	}
    if(EiC_Infile->fd == STDIN) {
	fputs("testpp> ",stdout);
	fflush(stdout);
    }
    do {
	buf = EiC_nextproline();
	while(*buf && *buf != EOF)
	    buf++;
    }while(*buf != EOF); 
}

#endif    













