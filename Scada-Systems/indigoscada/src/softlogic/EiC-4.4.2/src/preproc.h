#ifndef PREPROC_H_
#define PREPROC_H_

/* cmode states */
#define CMcmt 1			/* in comment */
#define CMstr 2			/* in string */
#define CMchr 3			/* in character constant */
#define CMang 4			/* in < > */

#define REBUFF_INCREMENT   80

#define skipall(a)      while(*(a)) ++a;
#define skipfws(a)	while(*(a) && isspace(*(a))) ++a;
#define skipbws(a)	while(*(a) && isspace(*(a))) --a;
#define skip2char(a,b)  while(*(a) && *(a) != b) ++a;
#define skipnws(a)      while(*(a) && !isspace(*(a))) ++a;




typedef struct fitem_t{
    int fd;                  /* file descriptor */
    char * fname;            /* file name */
    unsigned int lineno;     /* file lineno */
    unsigned char * buf;              /* file buffer */
    unsigned char * bufp;             /* file buffer pointer  */
    int n;                   /* number of chars left in buffer */
    struct fitem_t * next;
} fitem_t;

extern fitem_t *EiC_Infile;
#define CurrentFileName()   EiC_Infile->fname
#define CurrentLineNo()     EiC_Infile->lineno


/* preproc.c
-------------*/
int EiC_initpp(void);
char * EiC_nextproline(void);
void dodefine(char *);
int EiC_insertpath(char *);
char * EiC_strsave(char *s);
char *EiC_prolineString(char *str);

void EiC_ClearFileMacros(char *fname);
void EiC_showFileMacros(char *fname);
int EiC_IsIncluded(char *fname);
void EiC_showIncludedFiles(void);
void EiC_rmIncludeFileName(char *fname);
int EiC_Include_file(char *e, int mode);
int EiC_setinfile(char * fname);
void EiC_listpath(void);
int EiC_removepath(char *path);

char * EiC_process2(char * line,int bot,int top);
void EiC_pre_error(char *msg, ...);
int EiC_ismacroid(char *id);
int EiC_showMacro(char *id);
void EiC_markmacros(char mark);
void EiC_pp_CleanUp(size_t bot);
size_t EiC_pp_NextEntryNum(void);

extern int EiC_ptrSafe;
extern char cmode;

/* preexpr.c
-------------*/
int EiC_cpp_parse(char *s);

#endif




