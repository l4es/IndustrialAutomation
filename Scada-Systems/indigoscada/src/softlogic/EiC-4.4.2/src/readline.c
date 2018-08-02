/* GNUPLOT - readline.c */
/*
 * Copyright (C) 1986 - 1993   Thomas Williams, Colin Kelley
 *
 * Permission to use, copy, and distribute this software and its
 * documentation for any purpose with or without fee is hereby granted, 
 * provided that the above copyright notice appear in all copies and 
 * that both that copyright notice and this permission notice appear 
 * in supporting documentation.
 *
 * Permission to modify the software is granted, but not the right to
 * distribute the modified code.  Modifications are to be distributed 
 * as patches to released version.
 *  
 * This software is provided "as is" without express or implied warranty.
 * 
 *
 * AUTHORS
 *
 *   Original Software:
 *     Tom Tkacik
 *
 * HISTORY:
 *   Enhancements:
 *     Gershon Elber and many others.
 *
 *   Ed Breen (Jun 10 08:11:14 EST 1996)
 *     Stripped down: EiC port.
 *     Added standalone capability -- for testing and experimenting:
 *        To make the stand alone version of readline:
 *            gcc -D_STANDALONE -Wall -o readline readline.c
 *     Added a limiter to the history recording mechanism:
 *        The default limit means that only the last 500 lines 
 *        can be recalled. This can be set via the macro _HistLimit.
 *        Note, by setting _HistLimit to 0, effectively turns  off the
 *        limiter; that is,
 *          gcc -D_STANDALONE -D_HistLimit=0 -Wall -o readline readline.c
 *     Added tab recognition -- no, not file completion.
 *     Added show_history.
 *            void EiC_show_history(FILE *fp)
 *               outputs the history list to stream fp.
 *            last_history.
 *              returns a pointer to the last entered string in the history
 *              list. Will return NULL if no entry exists.
 *              Do not attempt to free this space, it is under control
 *              of history.
 *     Added bracket balancing routine:
 *              void backupTo(char to, char from);
 *     Optimized for speedy cursor movements:     
 *     Tested under:
 *        ultrix, solaris, dec alpha, sunos, linux, irix-5.3, irix-6.x. 
 *   Ed Breen (July 17 1999)
 *       Added win95, win98, NT win32 support
 *   Ed Breen (July 16 2000)
 *       Added DJGPP support
 */

/* a small version of GNU's readline */
/* this is not the BASH or GNU EMACS version of READLINE due to Copyleft 
	restrictions */
/* do not need any terminal capabilities except , */


/* NANO-EMACS line editing facility */
/* printable characters print as themselves (insert not overwrite) */
/* ^A moves to the beginning of the line */
/* ^B moves back a single character */
/* ^E moves to the end of the line */
/* ^F moves forward a single character */
/* ^K kills from current position to the end of line */
/* ^P moves back through history */
/* ^N moves forward through history */
/* ^H and DEL delete the previous character */
/* ^D deletes the current character, or EOF if line is empty */
/* ^L/^R redraw line in case it gets trashed */
/* ^U kills the entire line */
/* ^W kills last word */
/* LF and CR return the entire line regardless of the cursor postition */
/* EOF  with an empty line returns (char *)NULL */

/* all other characters are ignored */

#ifndef NO_READLINE

/*#define _POSIX_SOURCE*/
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <ctype.h>
  #include <signal.h>
  #include <time.h>

#if defined(WIN32) 

#include <conio.h>
#include <io.h>

# define special_getc() msdos_getch()
static char msdos_getch();

#else

#include <unistd.h> 
#include <termios.h>

#define special_getc() ansi_getc()
static int ansi_getc();

/* watch out for SV4 and BSD+4.3 stuff */
#ifndef  VREPRINT
#define  VREPRINT 18
#endif
#ifndef VWERASE
#define VWERASE 23
#endif
#ifndef SIGTSTP
#define SIGTSTP 26
#endif

#endif

#ifndef CLOCKS_PER_SEC
#define CLOCKS_PER_SEC 1000000
#endif


/* stubbs added by Ed Breen */
#define ralloc(x,y,z) realloc(x,y)
#define alloc(x,y)    malloc(x)
#define int_error(text,code)  {fprintf(stderr,"Fatal error..\n");\
				   fprintf(stderr,"%s\n",text);\
				   fprintf(stderr,"...now exiting to system ...\n");\
				   exit(1);}

#if !defined(WIN32)

static struct termios orig_termio, rl_termio;

/* ULTRIX defines VRPRNT instead of VREPRINT */
#if defined(VRPRNT) && !defined(VREPRINT)
#define VREPRINT VRPRNT
#endif

/* define characters to use with our input character handler */
static char term_chars[NCCS];

static int term_set = 0;	/* =1 if rl_termio set */

#endif


#define MAXBUF	512	/* initial size and increment of input line length */
#define BACKSPACE 0x08	/* ^H */
#define SPACE	' '
#define NEWLINE	'\n'
#define BELL    '\a'
#define TABSTOPS 4       /* number of spaces per tab stop */

struct hist {
	char *line;
	struct hist *prev;
	struct hist *next;
};

static struct hist *history = NULL;      /* no history yet */
static struct hist *EndHist = NULL;      /* end of history list */
static struct hist *cur_entry = NULL;


static char *cur_line;  /* current contents of the line */
static int line_len=0;
static int cur_pos = 0;	/* current position of the cursor */
static int max_pos = 0;	/* maximum character position */
static int HistLineNo = 0;  /* Current Line Number in history list */

static void fix_line  (void)  ;
static void redraw_line  (char *prompt)  ;
static void clear_line  (char *prompt)  ;
static void clear_eoline  (void)  ;
static void copy_line  (char *line)  ;
static void set_termio  (void)  ;
static void reset_termio  (void)  ;
static int ansi_getc  (void)  ;
static void  user_putc  (char ch)  ;
static int user_putsn(char *str, int n)  ;

static void extend_cur_line  (void)  ;
static void backupTo(char to, char from);
static char _BS = BACKSPACE;

#if  defined(WIN32)
#define backspace() _putch(BACKSPACE)
#else
#define backspace()   write(STDIN_FILENO,&_BS,1)
#endif

#define user_puts(x)  user_putsn(x,strlen(x))


void delay(clock_t d)
{
    clock_t et = clock() + d;
    while(et> clock());
}

static void user_putc(char ch)
{
#if defined(WIN32)
	_putch(ch);
#else	
   write(STDIN_FILENO,&ch,1);
#endif

}

static int user_putsn(char *str, int n)
{
	int rv;
#if defined(WIN32)
	for(rv=0;rv<n;rv++)
	    _putch(*str++);
#else
	rv = write(STDIN_FILENO,str,n);
#endif
	return rv;
}

static void extend_cur_line()
{
  char *new_line;

  /* extent input line length */
  new_line=ralloc(cur_line, line_len+MAXBUF, NULL);
  if(!new_line) {
    reset_termio();
    int_error("Can't extend readline length", NO_CARET);
  }
  cur_line=new_line;
  line_len+=MAXBUF;

}

unsigned char * EiC_readline(char *prompt)
{
    
    /* start with a string of MAXBUF chars */
    char * editLine(char *);
    if(line_len!=0) {
		free(cur_line);
		line_len=0;
    }
    
    cur_line=alloc((unsigned long)MAXBUF, "readline");
    line_len=MAXBUF;
    
    /* set the termio so we can do our own input processing */
    set_termio();
    
    /* print the prompt */

    user_puts(prompt);
    cur_line[0] = '\0';
    cur_pos = 0;
    max_pos = 0;
    cur_entry = NULL;
    return editLine(prompt);
}


char * editLine(char *prompt)
{
    /* The line to be edited is stored in cur_line.*/
    /* get characters */
    int cur_char;
    
    for(;;) {
	cur_char = special_getc();
	
	if(isprint(cur_char) || (((unsigned char)cur_char > 0x7f) &&
				 cur_char != EOF) || cur_char == '\t') {
	    int i,inc = 1;
	    if(cur_char == '\t') {
		inc = TABSTOPS;
		cur_char = ' ';
	    }
	    

	    if(max_pos+inc>=line_len) 
		extend_cur_line();

	    for(i=max_pos+inc-1; i-inc>=cur_pos; i--) {
		    cur_line[i] = cur_line[i-inc];
		}
	    max_pos += inc;
	    while(inc--) {
		user_putc(cur_char);
		cur_line[cur_pos++] = cur_char;
	    }
	    if (cur_pos < max_pos)
		fix_line();
	    cur_line[max_pos] = '\0';
	    switch(cur_char) {
	      case ')':backupTo('(',')');break;
	      case ']':backupTo('[',']');break;
	    }
#if defined(VERASE) 
	} else if(cur_char == term_chars[VERASE] ){ /* DEL? */
	    if(cur_pos > 0) {
		int i;
		cur_pos -= 1;
		backspace();
		for(i=cur_pos; i<max_pos; i++)
		    cur_line[i] = cur_line[i+1];
		max_pos -= 1;
		fix_line();
	    }
	} else if(cur_char == term_chars[VEOF] ){ /* ^D? */
	    if(max_pos == 0) {
		copy_line("to exit EiC, enter  :exit\n");
		user_putc(BELL);

		reset_termio();		
		return((char*)NULL);
	    }
	    if((cur_pos < max_pos)&&(cur_char == 004)) { /* ^D */
		int i;
		for(i=cur_pos; i<max_pos; i++)
		    cur_line[i] = cur_line[i+1];
		max_pos -= 1;
		fix_line();
	    }

	} else if(cur_char == term_chars[VKILL] ){ /* ^U? */
	    clear_line(prompt);

	} else if(cur_char == term_chars[VWERASE] ){ /* ^W? */
	    while((cur_pos > 0) &&
		  (cur_line[cur_pos-1] == SPACE)) {
		cur_pos -= 1;
		backspace();
	    }
	    while((cur_pos > 0) &&
		  (cur_line[cur_pos-1] != SPACE)) {
		cur_pos -= 1;
		backspace();
	    }
	    clear_eoline();
	    max_pos = cur_pos;


	} else if(cur_char == term_chars[VREPRINT] ){ /* ^R? */
	    user_putc(NEWLINE); /* go to a fresh line */
	    redraw_line(prompt);


	} else if(cur_char == term_chars[VSUSP]) {
	    reset_termio();
	    kill(0, SIGTSTP);

	    /* process stops here */

	    set_termio();
	    /* print the prompt */
	    redraw_line(prompt);
#endif
	} else {
	    /* do normal editing commands */
	    /* some of these are also done above */
	    int i;
	    switch(cur_char) {
	      case EOF:
		reset_termio();
		return((char *)NULL);
	      case 001:		/* ^A */
		while(cur_pos > 0) {
		    cur_pos -= 1;
		    backspace();
		}
		break;
	      case 002:		/* ^B */
		if(cur_pos > 0) {
		    cur_pos -= 1;
		    backspace();
		}
		break;
	      case 005:		/* ^E */
		while(cur_pos < max_pos) {
		    user_putc(cur_line[cur_pos]);
		    cur_pos += 1;
		}
		break;
	      case 006:		/* ^F */
		if(cur_pos < max_pos) {
		    user_putc(cur_line[cur_pos]);
		    cur_pos += 1;
		}
		break;
	      case 013:		/* ^K */
		clear_eoline();
		max_pos = cur_pos;
		break;
		
	      case 020:		/* ^P */
		if(history != NULL) {
		    if(cur_entry == NULL) {
			cur_entry = history;
			clear_line(prompt);
			copy_line(cur_entry->line);
		    } else if(cur_entry->prev != NULL) {
			cur_entry = cur_entry->prev;
			clear_line(prompt);
			copy_line(cur_entry->line);
		    }else
			user_putc(BELL);
		}else
		    user_putc(BELL);
		break;

	    case 016:		/* ^N */
		if(cur_entry != NULL) {
		    cur_entry = cur_entry->next;
		    clear_line(prompt);
		    if(cur_entry != NULL) 
			copy_line(cur_entry->line);
		    else
			cur_pos = max_pos = 0;
		}else
		    user_putc(BELL);
		break;
	      case 014:		/* ^L */
	      case 022:		/* ^R */
		user_putc(NEWLINE); /* go to a fresh line */
		redraw_line(prompt);
		break;
	      case 0177:	/* DEL */
	      case 010:		/* ^H */
		if(cur_pos > 0) {
		    cur_pos -= 1;
		    backspace();
		    for(i=cur_pos; i<max_pos; i++)
			cur_line[i] = cur_line[i+1];
		    max_pos -= 1;
		    fix_line();
		}
		break;
	      case 004:		/* ^D */
		if(max_pos == 0) {
		    reset_termio();
		    return((char *)NULL);
		}
		if(cur_pos < max_pos) {
		    for(i=cur_pos; i<max_pos; i++)
			cur_line[i] = cur_line[i+1];
		    max_pos -= 1;
		    fix_line();
		}
		break;
	      case 025:		/* ^U */
		clear_line(prompt);
		break;
	      case 027:		/* ^W */
		while((cur_pos > 0) &&
		      (cur_line[cur_pos-1] == SPACE)) {
		    cur_pos -= 1;
		    backspace();
		}
		while((cur_pos > 0) &&
		      (cur_line[cur_pos-1] != SPACE)) {
		    cur_pos -= 1;
		    backspace();
		}
		clear_eoline();
		max_pos = cur_pos;
		break;
	    case '\n':	/* ^J */
	    case '\r':	/* ^M */
		user_putc(NEWLINE);
		cur_line[max_pos+1] = '\0';
		cur_line = (char *)ralloc(cur_line,
					  (unsigned
					   long)(strlen(cur_line)+2),
					  "line resize");
		line_len=0;
		
		reset_termio();
		return cur_line;
	      default:
		break;
	    }
	}
    }
}


/* fix up the line from cur_pos to max_pos */
/* do not need any terminal capabilities except backspace, */
/* and space overwrites a character */
static void fix_line()
{
    int i;

    /* write tail of string */
    user_putsn(&cur_line[cur_pos],max_pos - cur_pos);
    
    
    /* write a space at the end of the line in case we deleted one */
    user_putc(SPACE);

    /* backup to original position */
    for(i=max_pos+1; i>cur_pos; i--)
	backspace();

}

/* redraw the entire line, putting the cursor where it belongs */
static void redraw_line(char *prompt)
{
    int i;

    user_puts(prompt);
    user_puts(cur_line);

    /* put the cursor where it belongs */
    for(i=max_pos; i>cur_pos; i--)
	backspace();
}

/* clear cur_line and the screen line */
static void clear_line(char *prompt)
{
    int i;

    memset(cur_line,0,max_pos);

    for(i=cur_pos; i>0; i--)
	backspace();

    for(i=0; i<max_pos; i++)
	user_putc(SPACE);

    user_putc('\r');
    user_puts(prompt);

    cur_pos = 0;
    max_pos = 0;
}

static void backupTo(char to, char from)
{
    int cmode = 0;
    int k = 1,i = cur_pos-1;

    backspace();
    while(i-- > 0) {
	backspace();
	if(cur_line[i] == '\'') {
	    if(cmode & 1)
		cmode &= ~1;
	    else
		cmode |= 1;
	    continue;
	}else if(cur_line[i] == '\"') {
	    if(cmode & 2)
		cmode &= ~2;
	    else
		cmode |= 2;
	    continue;
	}
	    
	if(cur_line[i] == to && !cmode) {
	    if(!--k)
		break;
	}else if(cur_line[i] == from && !cmode)
	    k++;
    }
    if(k) {
	user_putc(BELL);
	i = 0;
    } else
	delay(CLOCKS_PER_SEC / 2);

    user_putsn(&cur_line[i],cur_pos - i);

}
    
/* clear to end of line and the screen end of line */
static void clear_eoline()
{
    int i;
    for(i=cur_pos; i<max_pos; i++)
	cur_line[i] = '\0';

    for(i=cur_pos; i<max_pos; i++)
	user_putc(SPACE);
    for(i=cur_pos; i<max_pos; i++)
	backspace();
}

/* copy line to cur_line, draw it and set cur_pos and max_pos */
static void copy_line(char *line)
{
    while(strlen(line)+1>line_len) {
	extend_cur_line();
    }
    strcpy(cur_line, line);
    user_puts(cur_line);
    cur_pos = max_pos = strlen(cur_line);
}

/* add line to the history */
#ifndef _HistLimit       /* history limiter */
#define _HistLimit 500
#endif

void EiC_add_history(unsigned char *line)
{
    static unsigned int limit = 0;
    struct hist *entry;

    if(limit == _HistLimit && EndHist) {
	free(EndHist->line);
	entry = EndHist;
	EndHist = EndHist->next;
	EndHist->prev = NULL;
    } else {
	entry = (struct hist *)alloc((unsigned long)sizeof(struct hist),"history");
	limit++;
    }
    entry->line = alloc((unsigned long)(strlen(line)+1),"history");
    strcpy(entry->line, line);
    
    entry->prev = history;
    entry->next = NULL;
    if(history != NULL) {
	history->next = entry;
    } else /* get first entry */
	EndHist = entry;
    
    history = entry;
    HistLineNo++;
}

int  EiC_getHistLineNo()
{
    return HistLineNo;
}
	    
void EiC_save_history(FILE *to, int from)
{
    int cl = HistLineNo - 1;
    struct hist *p;
    p = history;
    while(cl-- > from)
	p = p->prev;
    while(p) {
	fprintf(to,"%s\n",p->line);
	p = p->next;
    }
}

/* show all history lines */
void EiC_show_history(FILE *fp)
{
    struct hist *p;
    p = EndHist;
    while(p) {
	fputs(p->line,fp);
	putc(NEWLINE,fp);
	p = p->next;
    }
    fflush(fp);
}

int EiC_load_history(char * fname, int prompt)
{
#define BufSz  512

    int i;
    char buff[BufSz];
    char *line;
    
    FILE *fp = fopen(fname,"r");
    if(prompt)
	set_termio();    
    if(fp) {
	while(fgets(buff,BufSz-2,fp)) {
	    for(i=0;buff[i] && buff[i] != '\n';++i)
		;
	    if(!buff[i])
		buff[i++] = '\\';
	    buff[i] = 0;
	    if(prompt) {
		printf("Re-enter [%s] (Y/N/E)?",buff);
		switch(special_getc()) {
		case 'y':
		case 'Y':
		    user_puts(" Y\n");
		    break;
		case 'e':
		case 'E':
		    user_puts(" E\n");
		    copy_line(buff);
		    line = editLine("edit: ");
		    if(*line)
			EiC_add_history(line);
		    free(line);
		    set_termio();
		    continue;
		default:
		    user_puts(" N\n");
		    continue;
		    
		}
	    }
	    EiC_add_history(buff);
	}
	fclose(fp);
	i = 1;
    } else
	i = 0;
    if(prompt)
	reset_termio();
    printf("added %d lines\n",HistLineNo);
    return i;

#undef BufSz
}
    

#if defined(WIN32)

/* Convert Arrow keystrokes to Control characters: */
static char msdos_getch()
{
    int c = _getch();
    if (c == 224 || c== 0) {
		c = _getch();		/* Get the extended code. */
		switch (c) {
		case 75:		/* Left Arrow. */
			c = 002;
		 break;
		case 77:		/* Right Arrow. */
			c = 006;
			break;
		case 72:		/* Up Arrow. */
			c = 020;
			break;
		case 80:		/* Down Arrow. */
			c = 016;
			break;
		case 115:		/* Ctl Left Arrow. */
		case 71:		/* Home */
			c = 001;
			break;
		case 116:		/* Ctl Right Arrow. */
		case 79:		/* End */
			c = 005;
			break;
		case 83:		/* Delete */
			c = 004;
			break;
		default:
			c = 0;
	    break;
		}
	} else if (c == 033) {	/* ESC */
		c = 025;
    }
    return c;
}

static void set_termio() {} 
static void reset_termio() {}

#else

/* Convert ANSI arrow keys to control characters */
static int ansi_getc()
{
  int c = getc(stdin);
  if (c == 033) {
    c = getc(stdin); /* check for CSI */
    if (c == '[') {
      c = getc(stdin); /* get command character */
      switch (c) {
      case 'D': /* left arrow key */
	c = 002;
	break;
      case 'C': /* right arrow key */
	c = 006;
	break;
      case 'A': /* up arrow key */
	c = 020;
	break;
	
      case 'B': /* down arrow key */
	c = 016;
	break;
      }
    }
  }
  return c;
}

/* set termio so we can do our own input processing */
static void set_termio()
{
    if(term_set == 0) {
	tcgetattr(0, &orig_termio);
	rl_termio = orig_termio;
	rl_termio.c_iflag &= ~(BRKINT|PARMRK|INPCK/*|IUCLC*/|IXON|IXOFF);
	rl_termio.c_iflag |=  (IGNBRK|IGNPAR);
	/* rl_termio.c_oflag &= ~(ONOCR); Costas Sphocleous Irvine,CA */
	rl_termio.c_lflag &= ~(ICANON|ECHO|ECHOE|ECHOK|ECHONL|NOFLSH);
	rl_termio.c_lflag |=  (ISIG);
	rl_termio.c_cc[VMIN] = 1;
	rl_termio.c_cc[VTIME] = 0;
	term_chars[VERASE]   = orig_termio.c_cc[VERASE];
	term_chars[VEOF]     = orig_termio.c_cc[VEOF];
	term_chars[VKILL]    = orig_termio.c_cc[VKILL];
	term_chars[VWERASE]  = orig_termio.c_cc[VWERASE];
	term_chars[VREPRINT] = orig_termio.c_cc[VREPRINT];
	term_chars[VSUSP]    = orig_termio.c_cc[VSUSP];
	/* disable suspending process on ^Z */
	rl_termio.c_cc[VSUSP] = 0;
	tcsetattr(0, TCSADRAIN, &rl_termio);
	term_set = 1;
    }
    
}
  
static void reset_termio()
{
    if(term_set == 1) {
	tcsetattr(0, TCSADRAIN, &orig_termio);
	term_set = 0;
    }
}

#endif

#ifdef _STANDALONE
int main()
{
    char * line = NULL;
    printf("**Press ^D in empty line to exit**\n");
    do {
	line = EiC_readline("$$> ");
	if (line) {
	    if(*line)
		EiC_add_history(line);
	    free(line);
	}
    } while(line);
    printf("\n-----------------\n");
    EiC_show_history(stdout);
    return 0;
}
#endif

#endif      

