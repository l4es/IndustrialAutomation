/************************************************************************
 *
 *	wedit2 - Special mini wedit version for wed editor
 *
 ************************************************************************/

/************************************************************************
 *
 *               Warning ! Modified version
 *
 *
 * 		Colors for lines starting with # ? .
 *               Diming after ;
 *
 *               F6 copy, F7 cut, F8 paste, F9 paste all, F10 save & exit
 *               F11 exit without saving, F12 toggle gr/eng
 *************************************************************************/

#include <stdio.h>
#include <curses.h>
#include <string.h>
#include <dirent.h>
#include "greek.h"
#include "args.h"

#define string	unsigned char
#define EVER	(;;)

#define SMAX		80
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
#define CTRLU		21
#define CTRLC		3
#define CTRLD		4
#define CTRLX		24

#define INSMODE		0
#define OVEMODE		1

struct WIN
{
	int x, y, w, h;
	int buf;
	int top, cur;
	int first, col;
	int status;
	string t[SMAX];
};

struct BUF
{
	int status;
	int lines;
	int winlinks;
	string f[SMAX];
};

struct WIN win[WMAX];
struct BUF buf[WMAX];
static string *line[WMAX][LMAX];
static string tmp[BMAX];
static string linebuf[BMAX];
static string cpbuf[BMAX];

void init()
{
	int i, b;

	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	init_pair(2, COLOR_BLACK, COLOR_WHITE);
	init_pair(3, COLOR_WHITE, COLOR_BLUE);
	init_pair(4, COLOR_BLUE, COLOR_BLACK);
	init_pair(5, COLOR_GREEN, COLOR_BLACK);
	init_pair(6, COLOR_RED, COLOR_BLACK);

	for (b = 0; b < WMAX; ++b)
		for (i = 0; i < LMAX; ++i)
			line[b][i] = NULL;
	for (b = 0; b < WMAX; ++b)
		win[b].status = 0;
	for (b = 0; b < WMAX; ++b)
	{
		buf[b].status = 0;
		buf[b].winlinks = 0;
		buf[b].lines = 0;
	}
}

void convstr(string *str)
{
	int i, j, k;
	static string s[LMAX];

	j = 0;
	strcpy(s, str);
	for (i = 0; s[i] != '\0' && s[i] != 10; ++i){
		if (s[i] == 9)
			for (k = 0; k < TABLEN - (i % TABLEN); ++k)
				str[j++] = 32;
		else
			str[j++] = s[i];
	}
	str[j] = '\0';
}

int istab(string *str, int i)
{
	++i;
	while (i % TABLEN != 0)
		if (str[i++] != 32)
			return (FALSE);
	return (TRUE);
}

void uconvstr(string *str)
{
	int i, j, k, tab;
	static string s[LMAX];

	j = 0;
	tab = FALSE;
	strcpy(s, str);
	for (i = 0; s[i] != '\0' && s[i] != 10; ++i){
		if (s[i] == 32 && (i % TABLEN) == 0){
			if (istab(s, i) == TRUE){
				str[j++] = 9;
				tab = TRUE;
			}
			else{
				str[j++] = 32;
				tab = FALSE;
			}
		}
		else if (s[i] == 32 && tab == FALSE)
			str[j++] = 32;
		if (s[i] != 32){
			str[j++] = s[i];
			tab = FALSE;
		}
	}
	str[j] = '\0';
}

void qclear(char *target, int n)
{
	int i;

	for (i = 0; i < n; ++i)
		target[i] = 32;
	target[n] = '\0';
}

void qcopy(char *target, char *source, int first, int n)
{
	int i, l;

	qclear(target, n);
	l = strlen(source) - first;
	for (i = 0; i < l && i < n; ++i)
		target[i] = source[i + first];
}

/******************************** DRAW ***************************/

void draw_box(int y, int x, int h, int w)
{
	int cx, cy;

	attron(COLOR_PAIR(4));
	mvaddch(y, x, ACS_ULCORNER);
	for (cx = x + 1; cx < x + w; ++cx)
		mvaddch(y, cx, ACS_HLINE);
	mvaddch(y, x+w, ACS_URCORNER);
	for (cy = y + 1; cy < y + h; ++cy){
		mvaddch(cy, x, ACS_VLINE);
		mvaddch(cy, x+w, ACS_VLINE);
	}

	mvaddch(y+h, x, ACS_LLCORNER);
	for (cx = x + 1; cx < x + w; ++cx)
		mvaddch(y+h, cx, ACS_HLINE);
	mvaddch(y+h, x+w, ACS_LRCORNER);
	refresh();
}

void clear_box(int y, int x, int h, int w)
{
	int xs, j;

	attron(COLOR_PAIR(1));

	for (j = 0; j < h + 1; ++j){
		move(y+j, x);
		for (xs = 0; xs < w + 1; ++xs)
			addch(32);
	}
	refresh();
}

void draw_title(int y, int x, int w, char *title)
{
	int len, xs;

	move(y, x);
	attron(COLOR_PAIR(3));
	for (xs = 0; xs < w + 1; ++xs)
		addch(32);

	len = strlen(title);
	if (len > w){
		title[w] = '\0';
		xs = 0;
	}
	else
		xs = (w - len) / 2;
	/* Do not center title */
	xs = 0;
	mvaddstr(y, x+xs, title);
}

void clear_title(int y, int x, int w)
{
	int xs;

	move(y, x);
	attron(COLOR_PAIR(1));
	for (xs = 0; xs < w + 1; ++xs)
		addch(32);
}

void draw_header(char *str)
{

	attroff(A_DIM);
	attron(COLOR_PAIR(3));
	qcopy(tmp, str, 0, COLS - 1);
	mvaddstr(0, 0, tmp);
	refresh();
}

void draw_footer(char *str)
{
	attroff(A_DIM);
	attron(COLOR_PAIR(3));
	qcopy(tmp, str, 0, COLS - 1);
	mvaddstr(LINES-1, 0, tmp);
	refresh();
}

/******************************** WIN ****************************/

void win_start()
{
	initscr();
	start_color();
	cbreak();
	noecho();
	nonl();
	keypad(stdscr, TRUE);

	init();
}

void win_end()
{
	erase();
	refresh();
	endwin();
}

int win_open(int y, int x, int h, int w, char *title)
{
	int b;

	/* Search for empty window */

	b = 0;
	while ((win[b].status & ST_DEFINED == ST_DEFINED) && (b < WMAX))
		++b;
	if (win[b].status & ST_DEFINED == ST_DEFINED)
		return (NONE);

	/* Assign values to new window */

	win[b].status = ST_DEFINED;
	win[b].x = x;
	win[b].y = y;
	win[b].w = w;
	win[b].h = h;
	strcpy(win[b].t, title);
	win[b].top = 0;
	win[b].cur = 0;
	win[b].first = 0;
	win[b].col = 0;
	win[b].buf = NONE;
	return (b);
}

int win_close(int win_no)
{
	win[win_no].status = 0;
	return (OK);
}

void win_resize(int win_no, int h, int w)
{
	win[win_no].h = h;
	win[win_no].w = w;
}

void win_move(int win_no, int y, int x)
{
	win[win_no].y = y;
	win[win_no].x = x;
}

void win_reset(int win_no)
{
	win[win_no].top = 0;
	win[win_no].cur = 0;
	win[win_no].first = 0;
	win[win_no].col = 0;
}

void win_title(int w, char *t)
{
	strcpy(win[w].t, t);
}
/*************************** BUFFERS ************************************/

int buf_open()
{
	int b;

	/* Search for empty buffer */

	b = 0;
	while ((buf[b].status & ST_DEFINED == ST_DEFINED) && (b < WMAX))
		++b;
	if (buf[b].status & ST_DEFINED == ST_DEFINED)
		return (NONE);

	/* Occupy buffer */

	buf[b].status = ST_DEFINED;
	return (b);
}

int buf_clear(int buf_no)
{
	/* Clear buffer lines */

	int i;

	if (buf_no < 0 || buf_no > WMAX)
		return (ERR);
	if ((buf[buf_no].status & ST_DEFINED) == 0)
		return (ERR);
	if ((buf[buf_no].status & ST_NOWRITE) != 0)
		return (ERR);

	for (i = 0; i < BMAX; ++i)
		if (line[buf_no][i] != NULL ){
			free(line[buf_no][i]);
			line[buf_no][i] = NULL;
		}
	buf[buf_no].lines = 0;
	return (OK);
}

int buf_close(int buf_no)
{

	if (buf_no < 0 || buf_no > WMAX)
		return (ERR);
	if ((buf[buf_no].status & ST_DEFINED) == 0)
		return (ERR);
	if ((buf[buf_no].status & ST_NOWRITE) != 0)
		return (ERR);

	/* Clear buffer lines */

	buf_clear(buf_no);

	/* Declare unused */

	buf[buf_no].status = 0;
	return (OK);
}

int app_line(int buf_no, char *str)
{
	int lastline;

	if (buf_no < 0 || buf_no > WMAX)
		return (ERR);
	if ((buf[buf_no].status & ST_DEFINED) == 0)
		return (ERR);
	if ((buf[buf_no].status & ST_NOWRITE) != 0)
		return (ERR);
	lastline = buf[buf_no].lines;
	if (lastline >= BMAX)
		return (ERR);
	convstr(str);
	line[buf_no][lastline] = strdup(str);
	if (line[buf_no][lastline] == NULL )
		return (ERR);
	++lastline;
	buf[buf_no].lines = lastline;
	return (lastline);
}

int ins_line(int buf_no, int line_no, char *str)
{
	int lastline;
	int i;

	if (buf_no < 0 || buf_no > WMAX)
		return (ERR);
	if ((buf[buf_no].status & ST_DEFINED) == 0)
		return (ERR);
	if ((buf[buf_no].status & ST_NOWRITE) != 0)
		return (ERR);

	lastline = buf[buf_no].lines;
	if (lastline >= BMAX)
		return (ERR);

	if (line_no == lastline)
		return (app_line(buf_no, str));

	for (i = lastline; i >= line_no; --i)
		line[buf_no][i + 1] = line[buf_no][i];
	convstr(str);
	line[buf_no][line_no] = strdup(str);
	if (line[buf_no][line_no] == NULL )
		return (ERR);
	++lastline;
	buf[buf_no].lines = lastline;
	return (lastline);
}

int del_line(int buf_no, int line_no)
{
	int i;

	if (buf_no < 0 || buf_no > WMAX)
		return (ERR);
	if ((buf[buf_no].status & ST_DEFINED) == 0)
		return (ERR);
	if ((buf[buf_no].status & ST_NOWRITE) != 0)
		return (ERR);

	if (line_no >= buf[buf_no].lines)
		return (FALSE);
	if (line[buf_no][line_no] != NULL )
		free(line[buf_no][line_no]);
	for (i = line_no; i < buf[buf_no].lines; ++i)
		line[buf_no][i] = line[buf_no][i + 1];
	--buf[buf_no].lines;
	return (buf[buf_no].lines);
}

int buf_empty(int buf_no)
{

	if (buf_no < 0 || buf_no > WMAX)
		return (-10);
	if ((buf[buf_no].status & ST_DEFINED) == 0)
		return (-11);
	if ((buf[buf_no].status & ST_NOWRITE) == ST_NOWRITE)
		return (-12);

	strcpy(tmp, "");
	if (app_line(buf_no, tmp) == ERR)
		return (OUTOFMEM);
	return (1);
}

/*************************** LINE STACK *************************************/

int push_line(int buf_no, char *str)
{
	return (app_line(buf_no, str));
}

int pop_line(int buf_no, char *str)
{
	int l;

	if (buf_no < 0 || buf_no > WMAX)
		return (-10);
	if ((buf[buf_no].status & ST_DEFINED) == 0)
		return (-11);
	if ((buf[buf_no].status & ST_NOWRITE) == ST_NOWRITE)
		return (-12);

	if (buf[buf_no].lines == 0){
		str[0] = '\0';
		return (0);
	}
	l = buf[buf_no].lines - 1;
	strcpy(str, line[buf_no][l]);
	del_line(buf_no, l);
	return (1);
}

/*************************** FILES **************************************/

int buf_load(int buf_no, char *file)
{
	FILE *source;
	int curline;

	if (buf_no < 0 || buf_no > WMAX)
		return (-10);
	if ((buf[buf_no].status & ST_DEFINED) == 0)
		return (-11);
	if ((buf[buf_no].status & ST_NOWRITE) == ST_NOWRITE)
		return (-12);

	if ((source = fopen(file, "r")) == NULL )
		return (FILENOTFOUND);

	curline = 0;
	while (fgets(tmp, LMAX, source) != NULL && curline < BMAX){
		if ((curline = app_line(buf_no, tmp)) == ERR){
			fclose(source);
			return (OUTOFMEM);
		}
	}
	fclose(source);
	if (curline == BMAX)
		return (BUFFERFULL);
	return (curline);
}

int buf_save(int buf_no, char *file)
{
	FILE *target;
	int curline;

	if ((target = fopen(file, "w")) == NULL )
		return (CREATEFAILED);

	curline = 0;
	while (line[buf_no][curline] != NULL ){
		strcpy(tmp, line[buf_no][curline]);
		/* uconvstr(tmp);*/
		fprintf(target, "%s\n", tmp);
		++curline;
	}
	fclose(target);
	return (OK);
}

/********************************** wedit ********************************/

int wdraw(int win_no)
{

	char *title;
	int x, y, w, h;

	if (win_no < 0 || win_no > WMAX)
		return (ERR);
	if (win[win_no].status & ST_DEFINED == 0)
		return (ERR);

	x = win[win_no].x;
	y = win[win_no].y;
	w = win[win_no].w;
	h = win[win_no].h;

	title = win[win_no].t;

	draw_title(y - 1, x, w, title);
	draw_box(y, x, h, w);
	return (OK);
}

int win_clear(int win_no)
{

	int x, y, w, h;

	if (win_no < 0 || win_no > WMAX)
		return (ERR);
	if (win[win_no].status & ST_DEFINED == 0)
		return (ERR);

	x = win[win_no].x;
	y = win[win_no].y;
	w = win[win_no].w;
	h = win[win_no].h;

	clear_title(y - 1, x, w);
	clear_box(y, x, h, w);
	return (OK);
}

void wshowline(int win_no, int buf_no, int line_no)
{

	int x, y, w, h, top, cur, first;
	int i;
	char ch0;

	x = win[win_no].x;
	y = win[win_no].y;
	w = win[win_no].w;
	h = win[win_no].h;

	top = win[win_no].top;
	cur = win[win_no].cur;
	first = win[win_no].first;

	if (line_no < top || line_no >= top + h)
		return;
	i = line_no - top;
	if (line[buf_no][line_no] == NULL )
		qclear(tmp, w - 1);
	else
		qcopy(tmp, line[buf_no][line_no], first, w - 1);
	mvaddstr(y+i+1, x+1, tmp);
	refresh();
}

char FirstNonBlank(char *s)
{
	while (*s == 9 || *s == 32)
		++s;
	return (*s);
}

int CommentStart(char *s)
{
	int i;

	for (i = 0; s[i] != '\0'; ++i)
		if (s[i] == ';')
			return (i);
	return (-1);
}

void wshowline_c(int win_no, int buf_no, int line_no)
{

	int x, y, w, h, top, cur, first, x_comment;
	int i;
	char ch0;
	char str[255];

	x = win[win_no].x;
	y = win[win_no].y;
	w = win[win_no].w;
	h = win[win_no].h;

	top = win[win_no].top;
	cur = win[win_no].cur;
	first = win[win_no].first;

	if (line_no < top || line_no >= top + h)
		return;
	i = line_no - top;
	if (line[buf_no][line_no] == NULL )
		qclear(tmp, w - 1);
	else
		qcopy(tmp, line[buf_no][line_no], first, w - 1);

	ch0 = FirstNonBlank(tmp);
	if (ch0 == '.')
		attron(COLOR_PAIR(6));
	else if (ch0 == '#')
		attron(COLOR_PAIR(5));
	else
		attron(COLOR_PAIR(1));

	x_comment = CommentStart(tmp);
	if (x_comment == -1){
		attroff(A_DIM);
		mvaddstr(y+i+1, x+1, tmp);
	}
	else if (x_comment == 0){
		attron(A_DIM);
		mvaddstr(y+i+1, x+1, tmp);
		attroff(A_DIM);
	}
	else{
		qcopy(str, tmp, 0, x_comment);
		attroff(A_DIM);
		mvaddstr(y+i+1, x+1, str);
		qcopy(str, tmp, x_comment, strlen(tmp) - x_comment);
		attron(A_DIM);
		mvaddstr(y+i+1, x+x_comment+1, str);
	}
	refresh();
}

void wshowall(int win_no, int buf_no)
{
	int i;
	int x, y, w, h, top, cur, first;

	x = win[win_no].x;
	y = win[win_no].y;
	w = win[win_no].w;
	h = win[win_no].h;

	top = win[win_no].top;
	cur = win[win_no].cur;
	first = win[win_no].first;

	attron(COLOR_PAIR(1));
	for (i = 0; i < h - 1; ++i){
		if (line[buf_no][i + top] == NULL )
			qclear(tmp, w - 1);
		else
			qcopy(tmp, line[buf_no][i + top], first, w - 1);
		mvaddstr(y+i+1, x+1, tmp);
	}
	refresh();
}

void wshowall_c(int win_no, int buf_no) /* Special colored */
{
	int i;
	int x, y, w, h, top, cur, first, x_comment;
	char ch0;
	char str[255];

	x = win[win_no].x;
	y = win[win_no].y;
	w = win[win_no].w;
	h = win[win_no].h;

	top = win[win_no].top;
	cur = win[win_no].cur;
	first = win[win_no].first;

	for (i = 0; i < h - 1; ++i){
		if (line[buf_no][i + top] == NULL ){
			qclear(tmp, w - 1);
			mvaddstr(y+i+1, x+1, tmp);
		}
		else{
			qcopy(tmp, line[buf_no][i + top], first, w - 1);
			ch0 = FirstNonBlank(tmp);
			if (ch0 == '.')
				attron(COLOR_PAIR(6));
			else if (ch0 == '#')
				attron(COLOR_PAIR(5));
			else
				attron(COLOR_PAIR(1));

			x_comment = CommentStart(tmp);
			if (x_comment == -1){
				attroff(A_DIM);
				mvaddstr(y+i+1, x+1, tmp);
			}
			else if (x_comment == 0){
				attron(A_DIM);
				mvaddstr(y+i+1, x+1, tmp);
				attroff(A_DIM);
			}
			else{
				qcopy(str, tmp, 0, x_comment);
				attroff(A_DIM);
				mvaddstr(y+i+1, x+1, str);
				qcopy(str, tmp, x_comment, strlen(tmp) - x_comment);
				attron(A_DIM);
				mvaddstr(y+i+1, x+x_comment+1, str);
			}
		}
	}
	refresh();
}

int overwr_ch(char *buf, int x, char ch, int m)
{
	if (x < 0 || x >= m)
		return (FALSE);
	if (buf[x] == '\0')
		buf[x + 1] = '\0';
	buf[x] = ch;
	return (TRUE);
}

int ins_ch(char *buf, int x, char ch, int m)
{
	int l;

	if (x < 0 || x >= m)
		return (FALSE);
	l = strlen(buf);
	if (l > m - 1)
		return (FALSE);
	if (x == l)
		return (overwr_ch(buf, x, ch, m));
	for (; l >= x; --l)
		buf[l + 1] = buf[l];
	buf[x] = ch;
	return (TRUE);
}

int del_ch(char *buf, int x, char ch, int m)
{
	int l;

	if (x < 0 || x >= m)
		return (FALSE);
	l = strlen(buf);
	if (x >= l)
		return (FALSE);
	for (; x < l; ++x)
		buf[x] = buf[x + 1];
	return (TRUE);
}

int wedit(int buf_no, int win_no, int sbuf_no)
{
	int ch;
	int redraw;
	int top, first, cur;
	int buflines;
	int h, w;
	int i;
	int x;
	int curchanged, mode, footpage;
	char footer[127];

	mode = INSMODE;
	footpage = 0;
	if (win_no < 0 || win_no > WMAX)
		return (ERR);
	if (win[win_no].status & ST_DEFINED == 0)
		return (ERR);

	if (buf_no < 0 || buf_no > WMAX)
		return (ERR);
	if ((buf[buf_no].status & ST_DEFINED) == 0)
		return (ERR);
	if ((buf[buf_no].status & ST_NOWRITE) == ST_NOWRITE)
		return (ERR);

	if (sbuf_no < 0 || sbuf_no > WMAX)
		return (ERR);
	if ((buf[sbuf_no].status & ST_DEFINED) == 0)
		return (ERR);
	if ((buf[sbuf_no].status & ST_NOWRITE) == ST_NOWRITE)
		return (ERR);

	if (win[win_no].buf != buf_no){
		win_reset(win_no);
		win[win_no].buf = buf_no;
	}

	if (win[win_no].top > buf[buf_no].lines)
		win_reset(win_no);
	wdraw(win_no);

	h = win[win_no].h;
	w = win[win_no].w;
	top = win[win_no].top;
	cur = win[win_no].cur;
	first = win[win_no].first;
	x = win[win_no].col;

	buflines = buf[buf_no].lines;

	strcpy(linebuf, line[buf_no][cur]);
	curchanged = FALSE;

	redraw = REDRAW_ALL;
	ch = 0;

	while (ch != KEY_F0 + 10 && ch != CTRLX){
		if (mode == INSMODE)
			sprintf(footer,
					" Line:%4d/%-4d Col: %3d Mode: INS                         Save & Exit: F10",
					cur + 1, buflines, x + 1);
		else
			sprintf(footer,
					" Line:%4d/%-4d Col: %3d Mode: OVE                         Save & Exit: F10",
					cur + 1, buflines, x + 1);

		footpage = footpage % 3;
		if (footpage == 0)
			draw_footer(footer);
		else if (footpage == 1)
			draw_footer(" F6:Copy     F7:Cut     F8:Paste     F9:Paste all");
		else if (footpage == 2)
			draw_footer(" F10:Save&Exit    F11:Help     F12:Eng/Gr");
		if (curchanged){
			free(line[buf_no][cur]);
			line[buf_no][cur] = strdup(linebuf);
			curchanged = FALSE;
		}
		if (redraw == REDRAW_ALL){
			win[win_no].h = h;
			win[win_no].w = w;
			win[win_no].top = top;
			win[win_no].first = first;
			win[win_no].cur = cur;
			win[win_no].col = x;
			wshowall_c(win_no, buf_no);
			redraw = REDRAW_NONE;
		}
		if (redraw == REDRAW_LINE){
			wshowline_c(win_no, buf_no, cur);
			redraw = REDRAW_NONE;
		}

		move(win[win_no].y+cur-top+1, win[win_no].x+x-first+1);
		refresh();
		ch = grgetch();
		switch (ch){
		case KEY_UP:
			if (cur == 0)
				break;
			--cur;
			strcpy(linebuf, line[buf_no][cur]);
			if (x > strlen(linebuf))
				x = strlen(linebuf);
			if (first > x){
				first = x;
				redraw = REDRAW_ALL;
			}
			if (top > cur){
				--top;
				redraw = REDRAW_ALL;
			}
			break;

		case KEY_F0 + 6:
			push_line(sbuf_no, linebuf);
		case KEY_DOWN:
			if (cur >= buflines - 1)
				break;
			++cur;
			strcpy(linebuf, line[buf_no][cur]);
			if (x > strlen(linebuf))
				x = strlen(linebuf);
			if (first > x){
				first = x;
				redraw = REDRAW_ALL;
			}
			if (cur > top + h - 2){
				++top;
				redraw = REDRAW_ALL;
			}
			break;

		case KEY_LEFT:
			if (x <= 0)
				break;
			--x;
			if (first > x){
				--first;
				redraw = REDRAW_ALL;
			}
			break;

		case KEY_RIGHT:
			if (x >= strlen(linebuf))
				break;
			++x;
			if (x > first + w - 2 && first < LMAX - w){
				++first;
				redraw = REDRAW_ALL;
			}
			break;

		case KEY_NPAGE:
			if (cur >= buflines - 1)
				break;
			if (cur == top + h - 2){
				i = 0;
				while (buflines > top + h - 1 && i < h - 1){
					++top;
					++i;
				}
			}
			cur = top + h - 2;
			if (cur >= buflines)
				cur = buflines - 1;
			strcpy(linebuf, line[buf_no][cur]);
			if (x > strlen(linebuf))
				x = strlen(linebuf);
			if (first > x)
				first = x;
			redraw = REDRAW_ALL;
			break;

		case KEY_PPAGE:
			if (cur == 0)
				break;
			if (cur == top){
				i = 0;
				while (top > 0 && i < h - 1)
				{
					--top;
					++i;
				}
			}
			cur = top;
			strcpy(linebuf, line[buf_no][cur]);
			if (x > strlen(linebuf))
				x = strlen(linebuf);
			if (first > x)
				first = x;
			redraw = REDRAW_ALL;
			break;

		case KEY_HOME:
			x = 0;
			first = 0;
			redraw = REDRAW_LINE;
			if (cur == 0)
				break;
			top = 0;
			cur = 0;
			strcpy(linebuf, line[buf_no][cur]);
			redraw = REDRAW_ALL;
			break;

		case KEY_END:
			x = 0;
			first = 0;
			redraw = REDRAW_LINE;
			if (cur >= buflines - 1)
				break;
			if (buflines < h)
				top = 0;
			else
				top = buflines - h + 1;
			cur = buflines - 1;
			strcpy(linebuf, line[buf_no][cur]);
			redraw = REDRAW_ALL;
			break;

		case KEY_SLEFT: /* go to start of line */
			x = 0;
			if (first > 0){
				first = 0;
				redraw = REDRAW_ALL;
			}
			break;
		case KEY_SRIGHT:
			/* go to end of line */
			x = strlen(linebuf);
			if (x > first + w - 2){
				first = x - w + 2;
				redraw = REDRAW_ALL;
			}
			break;
		case KEY_F0 + 1:
		case KEY_F0 + 2:
		case KEY_F0 + 3:
		case KEY_F0 + 4:
		case KEY_F0 + 5:
		case KEY_F0 + 12:
			break;
		case KEY_F0 + 11:
			++footpage;
			break;

		case KEY_F0 + 8:
			if (pop_line(sbuf_no, cpbuf) == 1){
				buflines = ins_line(buf_no, cur + 1, cpbuf);
				redraw = REDRAW_ALL;
			}
			break;

		case KEY_F0 + 9:
			while (pop_line(sbuf_no, cpbuf) == 1)
				buflines = ins_line(buf_no, cur + 1, cpbuf);
			redraw = REDRAW_ALL;
			break;

		case KEY_F0 + 7:
			push_line(sbuf_no, linebuf);
		case CTRLD:
			x = 0;
			first = 0;
			redraw = REDRAW_ALL;
			curchanged = TRUE;
			if (cur == buflines - 1){
				strcpy(linebuf, "");
				break;
			}
			strcpy(linebuf, line[buf_no][cur + 1]);
			buflines = del_line(buf_no, cur + 1);
			break;

		case KEY_IC:
			if (mode == INSMODE)
				mode = OVEMODE;
			else
				mode = INSMODE;
			break;
		case KEY_DC:
			if (x == strlen(linebuf) && cur < buflines - 1){
				/* Check for overflow! */
				if (strlen(linebuf) + strlen(line[buf_no][cur + 1]) >= LMAX)
					break;
				strcat(linebuf, line[buf_no][cur + 1]);
				curchanged = TRUE;
				buflines = del_line(buf_no, cur + 1);
				redraw = REDRAW_ALL;
				break;
			}
			if (del_ch(linebuf, x, ch, BMAX)){
				curchanged = TRUE;
				redraw = REDRAW_LINE;
			}
			if (cur == 0 && buflines == 0)
				break;
			if (strlen(linebuf) == 0){
				if (buflines <= 1)
					break;
				buflines = del_line(buf_no, cur);
				if (cur == buflines)
					--cur;
				strcpy(linebuf, line[buf_no][cur]);
				if (x > strlen(linebuf))
					x = strlen(linebuf);
				if (first > x)
					first = x;
				if (top > cur)
					--top;
				redraw = REDRAW_ALL;
			}
			break;

		case KEY_ENTER:
		case 10:
		case 13:
			buflines = ins_line(buf_no, cur + 1, linebuf + x);
			linebuf[x] = '\0';
			free(line[buf_no][cur]);
			line[buf_no][cur] = strdup(linebuf);
			x = 0;
			first = 0;
			++cur;
			strcpy(linebuf, line[buf_no][cur]);
			if (cur > top + h - 2)
				++top;
			redraw = REDRAW_ALL;
			break;

		case KEY_BACKSPACE:
			if (x <= 0)
				break;
			--x;
			if (mode == INSMODE){
				del_ch(linebuf, x, ch, BMAX);
				curchanged = TRUE;
				redraw = REDRAW_LINE;
			}
			if (first > x){
				--first;
				redraw = REDRAW_ALL;
			}
			break;

		case 9:
			ch = 32;
		default:
			if (x > strlen(linebuf))
				break;
			if (ch >= 0 && ch <= 31){
				beep();
				break;
			}
			if (mode == OVEMODE)
				overwr_ch(linebuf, x, ch, BMAX);
			else
				ins_ch(linebuf, x, ch, BMAX);
			curchanged = TRUE;
			redraw = REDRAW_LINE;
			++x;
			if (x > first + w - 2 && first < LMAX - w){
				++first;
				redraw = REDRAW_ALL;
			}
			break;
		}
	}
	return (ch);
}

