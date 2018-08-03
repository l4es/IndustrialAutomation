#ifndef _WEDIT_H_
#define _WEDIT_H_
/** @file wedit.h*/

#define VER	1.01
#define string	unsigned char
#define EVER	(;;)

#define SMAX		116
#define MENUS		10
#define LMAX		1024
#define BMAX		1024
#define WMAX		16
#define ST_DEFINED	1
#define ST_CHANGED	2
#define ST_NOREAD	4
#define ST_NOWRITE	8
#define ST_DEFUNCT	16
#define ST_NOSCROLL	32

#define NONE		-1
#define FILENOTFOUND	-2
#define BUFFERFULL	-3
#define CREATEFAILED	-4
#define OUTOFMEM	-5
#define TABLEN		8

#define REDRAW_NONE	0
#define REDRAW_LINE	1
#define REDRAW_ALL	2

#define KEY_ESC		27
#define CTRLX		24
#define CTRLU		21
#define CTRLD		4

#define INSMODE		0
#define OVEMODE		1

#define NORMMODE	0
#define MENUMODE	1


struct WIN	{
			int x,y,w,h;
			string t[SMAX];
			int buf;
			int top, cur;
			int first, col;
			int status;
			int redraw;
		};

struct BUF	{
			int status;
			int lines;
			int winlinks;
			string f[SMAX];
		};

void init();
void convstr(string *);
int istab(string *, int);
void uconvstr(string *);
void qclear(char *, int);
void qcopy(char *, char *, int , int);
void draw_box(int ,int ,int ,int );
void clear_box(int , int , int , int);
void draw_vline(int , int , int);
void draw_title(int , int , int , char *);
void clear_title(int , int , int );
void draw_info_line(int , char *);
void draw_header(char *);
void draw_footer(char *str);
int wedit(int, int, int );
int win_start();
int win_end();
int win_mode();
int win_open(int , int , int , int , char *);
int win_close(int);
int win_resize(int, int, int);
int win_move(int, int, int);
int win_puts(int, int, int, char *);
int win_gets(int, int, int, int, char *);
int win_reset(int);
int win_title(int, char *);
int win_update(int);
int win_set(int, int);
int win_set_first(int, int);
int buf_open();
int buf_clear(int);
int buf_close(int);
int app_line(int, char *);
int buf_empty(int );
int buf_load(int , char *);
int buf_save(int , char *);
int buf_search_forw(int, char *, int);
int buf_search_back(int, char *, int);
int wdraw(int);
int win_clear(int);
int w_vline(int, int);
void wshowline(int, int, int);
void wshowline_c(int, int, int);
void wshowsel_c(int, int);
void wshowall(int, int);
void wshowall_c(int, int);
int wreadme(int, int);
int wselect_show(int, int);
int wselect(int, int);
int wselect_old(int, int);
int buf_cpline(int, int, char *);
/* This is a special function. Do not use in other applications */
int buf_special_paint(int, int);
int convert(int, int);
int uconvert(int, int);

#endif //_WEDIT_H_
