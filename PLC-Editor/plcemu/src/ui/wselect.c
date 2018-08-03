/************************************************************************
 *	wselect.c
 *	Special wedit version
 *
 ************************************************************************/

/************************************************************************
 *
 *       Warning ! Modified version
 *
 * 	Titles: 	Not centered
 * 	wselect:
 *	wreadme:	No blocking done!
 *			win_start() sets terminal in haldelay mode.
 *		 	Get character pressed with lastchar()
 *			Update will be done only if:
 *
 *			lastchar() != ERR
 *			Global variable UPDATE has been set to TRUE
 *			by calling the update() function
 *
 *
 * 	Includes:
 *			buf_empty
 *			buf_search_forw
 *			buf_search_back
 *
 *			new: wedit supported
 AK 2008
 *************************************************************************/

#include <stdio.h>
#include <curses.h>
#include <string.h>
#include <dirent.h>
#include "greek.h"
#include "args.h"
#include "wedit.h"

struct WIN win[WMAX];
struct BUF buf[WMAX];
static string *line[WMAX][LMAX];
static string tmp[BMAX];
static string linebuf[BMAX];
int curwin = -1; /* current window */
int win_started = FALSE;

void init()
{
	int i, b;

	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	init_pair(2, COLOR_BLACK, COLOR_WHITE);
	init_pair(3, COLOR_RED, COLOR_BLACK);
	init_pair(4, COLOR_RED, COLOR_WHITE);
	init_pair(5, COLOR_GREEN, COLOR_BLACK);
	init_pair(6, COLOR_GREEN, COLOR_WHITE);
	init_pair(7, COLOR_WHITE, COLOR_BLUE);
	init_pair(8, COLOR_BLUE, COLOR_BLACK);
	init_pair(9, COLOR_BLACK, COLOR_GREEN);
	init_pair(10, COLOR_WHITE, COLOR_RED);

	for (b = 0; b < WMAX; ++b)
		for (i = 0; i < LMAX; ++i)
			line[b][i] = NULL;
	for (b = 0; b < WMAX; ++b)
		win[b].status = 0;
	for (b = 0; b < WMAX; ++b){
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

	attron(COLOR_PAIR(8));
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

void draw_vline(int y, int x, int h)
{
	int i;

	for (i = 0; i < h; ++i)
		mvaddch(y, x+i, ACS_VLINE);
}

void draw_title(int y, int x, int w, char *title)
{
	int len, xs;

	move(y, x);
	attron(COLOR_PAIR(7));
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

void draw_info_line(int y, char *str)
{
	attroff(A_DIM);
	attron(COLOR_PAIR(9));
	qcopy(tmp, str, 0, COLS - 1);
	mvaddstr(y, 0, tmp);
	refresh();
}

void draw_header(char *str)
{

	attroff(A_DIM);
	attron(COLOR_PAIR(7));
	qcopy(tmp, str, 0, COLS - 1);
	mvaddstr(0, 0, tmp);
	refresh();
}

void draw_footer(char *str)
{
	attroff(A_DIM);
	attron(COLOR_PAIR(7));
	qcopy(tmp, str, 0, COLS - 1);
	mvaddstr(LINES-1, 0, tmp);
	refresh();
}

/******************************** WIN ****************************/

int win_start()
{
	initscr();
	start_color();
	//halfdelay(TRUE);
	cbreak();
	noecho();
	nonl();
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE);
	init();
	win_started = TRUE;
	return (TRUE);
}
int win_end()
{
	erase();
	refresh();
	endwin();
	win_started = FALSE;
	return (FALSE);
}

int win_mode()
{
	return (win_started);
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
	win[b].first = 1;
	win[b].col = 0;
	win[b].buf = NONE;
	win[b].redraw = TRUE;
	return (b);
}

int win_close(int win_no)
{
	if (win_no < 0 || win_no > WMAX)
		return (ERR);
	if (win[win_no].status & ST_DEFINED == 0)
		return (ERR);
	win[win_no].status = 0;
	return (OK);
}

int win_resize(int win_no, int h, int w)
{
	if (win_no < 0 || win_no > WMAX)
		return (ERR);
	if (win[win_no].status & ST_DEFINED == 0)
		return (ERR);
	win[win_no].h = h;
	win[win_no].w = w;
	win[win_no].redraw = TRUE;
	return (OK);
}

int win_move(int win_no, int y, int x)
{
	if (win_no < 0 || win_no > WMAX)
		return (ERR);
	if (win[win_no].status & ST_DEFINED == 0)
		return (ERR);
	win[win_no].y = y;
	win[win_no].x = x;
	win[win_no].redraw = TRUE;
	return (OK);
}

int win_puts(int win_no, int y, int x, char *str)
{
	int wx, wy;

	if (win_no < 0 || win_no > WMAX)
		return (ERR);
	if (win[win_no].status & ST_DEFINED == 0)
		return (ERR);
	wy = win[win_no].y;
	wx = win[win_no].x;
	attron(COLOR_PAIR(1));
	mvaddstr(y+wy, x+wx, str);
	win[win_no].redraw = TRUE;
	return (OK);
}

int win_infoline(int win_no, char *str)
{
	int wx, wy;
	char buf[116];

	if (win_no < 0 || win_no > WMAX)
		return (ERR);
	if (win[win_no].status & ST_DEFINED == 0)
		return (ERR);
	wy = win[win_no].y + win[win_no].h - 1;
	wx = win[win_no].x + 1;
	attron(COLOR_PAIR(4));
	strncpy(buf, str, 79);
	buf[win[win_no].w] = '\0';
	mvaddstr(wy, wx, buf);
	attron(COLOR_PAIR(1));
	win[win_no].redraw = TRUE;
	return (OK);
}

int win_infoclear(int win_no, char *str)
{
	int wx, wy, i;
	char buf[116];

	if (win_no < 0 || win_no > WMAX)
		return (ERR);
	if (win[win_no].status & ST_DEFINED == 0)
		return (ERR);
	wy = win[win_no].y + win[win_no].h - 1;
	wx = win[win_no].x + 1;
	attron(COLOR_PAIR(1));
	strncpy(buf, str, 79);
	buf[win[win_no].w] = '\0';
	for (i = 0; buf[i] != '\0'; ++i)
		buf[i] = 32;
	mvaddstr(wy, wx, buf);
	win[win_no].redraw = TRUE;
	return (OK);
}

int win_gets(int win_no, int y, int x, int maxlen, char *str)
{
	int c;
	int l;
	int wx, wy;

	if (win_no < 0 || win_no > WMAX)
		return (ERR);
	if (win[win_no].status & ST_DEFINED == 0)
		return (ERR);
	wy = win[win_no].y;
	wx = win[win_no].x;
	l = strlen(str);
	win_puts(win_no, y, x, str);
	move(y+wy, x+wx+l);
	grgetch();
	c = lastchar();
	if ((c > 31 && c < 128) && l < maxlen - 1){
		str[l] = c;
		str[l + 1] = '\0';
		win_puts(win_no, y, x, str);
	}
	else if (c == KEY_BACKSPACE && l > 0){
		--l;
		str[l] = ' ';
		win_puts(win_no, y, x, str);
		str[l] = '\0';
	}
	return (0);
}

int win_reset(int win_no)
{
	if (win_no < 0 || win_no > WMAX)
		return (ERR);
	if (win[win_no].status & ST_DEFINED == 0)
		return (ERR);
	win[win_no].top = 0;
	win[win_no].cur = 0;
	/*	win[win_no].first=1; */
	win[win_no].col = 0;
	win[win_no].redraw = TRUE;
	return (OK);
}

int win_set_top(int win_no, int top)
{
	if (win_no < 0 || win_no > WMAX)
		return (ERR);
	if (win[win_no].status & ST_DEFINED == 0)
		return (ERR);
	if (top < 0)
		return (ERR);

	win[win_no].top = top;
	win[win_no].redraw = TRUE;
	return (OK);
}

int win_set(int win_no, int cur)
{
	if (win_no < 0 || win_no > WMAX)
		return (ERR);
	if (win[win_no].status & ST_DEFINED == 0)
		return (ERR);
	if (cur < 0)
		return (ERR);

	win[win_no].cur = cur;
	if (cur < win[win_no].top)
		win[win_no].top = cur;
	if (cur > win[win_no].top + win[win_no].h - 2)
		win[win_no].top = cur - win[win_no].h + 2;
	win[win_no].redraw = TRUE;
	return (OK);
}

int win_title(int win_no, char *t)
{
	if (win_no < 0 || win_no > WMAX)
		return (ERR);
	if (win[win_no].status & ST_DEFINED == 0)
		return (ERR);
	strcpy(win[win_no].t, t);
	return (OK);
}

int win_update(int win_no)
{
	if (win_no < 0 || win_no > WMAX)
		return (ERR);
	if (win[win_no].status & ST_DEFINED == 0)
		return (ERR);
	win[win_no].redraw = TRUE;
	return (OK);
}

int win_set_first(int win_no, int first)
{
	if (win_no < 0 || win_no > WMAX)
		return (ERR);
	if (win[win_no].status & ST_DEFINED == 0)
		return (ERR);
	win[win_no].first = first;
	return (OK);
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

/*************************** FILES **************************************/

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

int buf_load(int buf_no, char *file)
{
	FILE *source;
	int curline;

	if (buf_no < 0 || buf_no > WMAX)
		return (ERR);
	if ((buf[buf_no].status & ST_DEFINED) == 0)
		return (ERR);
	if ((buf[buf_no].status & ST_NOWRITE) == ST_NOWRITE)
		return (ERR);

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

	if (buf_no < 0 || buf_no > WMAX)
		return (ERR);
	if ((buf[buf_no].status & ST_DEFINED) == 0)
		return (ERR);

	if ((target = fopen(file, "w")) == NULL )
		return (CREATEFAILED);

	curline = 0;
	while (line[buf_no][curline] != NULL ){
		fprintf(target, "%s\n", line[buf_no][curline]);
		++curline;
	}
	fclose(target);
	return (OK);
}
int buf_search_forw(int buf_no, char *str, int curline)
{
	/* search for the first occurance of 'str' after curline 
	 returns -1 if not found */

	int len;

	if (buf_no < 0 || buf_no > WMAX)
		return (ERR);
	if ((buf[buf_no].status & ST_DEFINED) == 0)
		return (ERR);

	len = strlen(str);
	if (len == 0)
		return (curline);
	while (curline < BMAX && line[buf_no][curline] != NULL )
		if (strncmp(line[buf_no][curline], str, len) == 0)
			return (curline);
		else
			++curline;
	return (-1);
}

int buf_search_back(int buf_no, char *str, int curline)
{
	/* search for the first occurance of 'str' before curline 
	 returns -1 if not found */

	int len;

	if (buf_no < 0 || buf_no > WMAX)
		return (ERR);
	if ((buf[buf_no].status & ST_DEFINED) == 0)
		return (ERR);

	len = strlen(str);
	if (len == 0)
		return (curline);
	while (curline >= 0 && line[buf_no][curline] != NULL )
		if (strncmp(line[buf_no][curline], str, len) == 0)
			return (curline);
		else
			--curline;
	return (-1);
}

/*************************** wreadme, wselect ****************************/

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

int w_vline(int win_no, int col)
{
	/* draw a vertical line in window <win_no> at col <col> */

	if (win_no < 0 || win_no > WMAX)
		return (ERR);
	if (win[win_no].status & ST_DEFINED == 0)
		return (ERR);

	if (col == 0)
		return (OK);
	draw_vline(win[win_no].y, win[win_no].x, win[win_no].h);
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

void wshowline_c(int win_no, int buf_no, int line_no)
{
	/* Show a line in color */

	char ch0;

	if (line[buf_no][line_no] == NULL )
		attron(COLOR_PAIR(1));
	else{
		ch0 = *line[buf_no][line_no];
		if (ch0 == '.')
			attron(COLOR_PAIR(3));
		else if (ch0 == '#')
			attron(COLOR_PAIR(5));
		else if (ch0 == '@')
			attron(COLOR_PAIR(8));
		else
			attron(COLOR_PAIR(1));
		if (ch0 == ';')
			attron(A_DIM);
		else
			attroff(A_DIM);
	}
	wshowline(win_no, buf_no, line_no);
}

void wshowsel_c(int win_no, int buf_no)
{
	char ch0;
	int line_no;

	line_no = win[win_no].cur;

	if (line[buf_no][line_no] == NULL )
		attron(COLOR_PAIR(2));
	else{
		ch0 = *line[buf_no][line_no];
		if (ch0 == '.')
			attron(COLOR_PAIR(4));
		else if (ch0 == '#')
			attron(COLOR_PAIR(6));
		else
			attron(COLOR_PAIR(2));
		if (ch0 == ';')
			attron(A_DIM);
		else
			attroff(A_DIM);
	}
	wshowline(win_no, buf_no, line_no);
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
	for (i = 0; i < h - 1; ++i)
	{
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
	int x, y, w, h, top, cur, first;
	char ch0;

	x = win[win_no].x;
	y = win[win_no].y;
	w = win[win_no].w;
	h = win[win_no].h;

	top = win[win_no].top;
	cur = win[win_no].cur;
	first = win[win_no].first;

	for (i = 0; i < h - 1; ++i){
		if (line[buf_no][i + top] == NULL )
			qclear(tmp, w - 1);
		else{
			qcopy(tmp, line[buf_no][i + top], first, w - 1);
			ch0 = *line[buf_no][i + top];
			if (ch0 == '.')
				attron(COLOR_PAIR(3));
			else if (ch0 == '#')
				attron(COLOR_PAIR(5));
			else if (ch0 == '~')
				attron(COLOR_PAIR(2));
			else
				attron(COLOR_PAIR(1));

			if (ch0 == ';')
				attron(A_DIM);
			else
				attroff(A_DIM);

		}
		mvaddstr(y+i+1, x+1, tmp);
	}
	refresh();
}

int wreadme(int buf_no, int win_no)
{
	int ch;
	int redraw;

	if (win_no < 0 || win_no > WMAX)
		return (ERR);
	if (win[win_no].status & ST_DEFINED == 0)
		return (ERR);

	if (buf_no < 0 || buf_no > WMAX)
		return (ERR);
	if (buf[buf_no].status & ST_DEFINED == 0)
		return (ERR);

	redraw = win[win_no].redraw || curwin != win_no;
	curwin = win_no;

	/* In case lines have disappered but win[].update has not been set */

	if (win[win_no].buf != buf_no || win[win_no].top > buf[buf_no].lines){
		win_reset(win_no);
		win[win_no].buf = buf_no;
		redraw = TRUE;
	}

	if (redraw){
		attroff(A_DIM);
		wdraw(win_no);
		wshowall_c(win_no, buf_no);
		win[win_no].redraw = FALSE;
	}

	ch = grgetch();
	switch (ch){
	case KEY_UP:
		if (win[win_no].top > 0){
			--win[win_no].top;
			win[win_no].redraw = TRUE;
		}
		break;

	case KEY_DOWN:
		if (buf[buf_no].lines - 1 > win[win_no].top + win[win_no].h - 2){
			++win[win_no].top;
			win[win_no].redraw = TRUE;
		}
		break;

	case KEY_HOME:
		win[win_no].top = 0;
		win[win_no].redraw = TRUE;
		break;

	case KEY_END:
		win[win_no].top = buf[buf_no].lines - win[win_no].h + 2;
		if (win[win_no].top < 0)
			win[win_no].top = 0;
		win[win_no].redraw = TRUE;
		break;

	}
	return (OK);
}

int wselect_show(int buf_no, int win_no)
{
	int ch;
	int redraw;

	if (win_no < 0 || win_no > WMAX)
		return (ERR);
	if (win[win_no].status & ST_DEFINED == 0)
		return (ERR);

	if (buf_no < 0 || buf_no > WMAX)
		return (ERR);
	if (buf[buf_no].status & ST_DEFINED == 0)
		return (ERR);

	redraw = win[win_no].redraw || curwin != win_no;
	curwin = win_no;

	/* In case lines have disappered but win[].update has not been set */
	if (win[win_no].buf != buf_no || win[win_no].top > buf[buf_no].lines){
		win_reset(win_no);
		win[win_no].buf = buf_no;
		redraw = TRUE;
	}

	if (redraw){
		attroff(A_DIM);
		wdraw(win_no);
		wshowall_c(win_no, buf_no);
		wshowsel_c(win_no, buf_no);
		win[win_no].redraw = FALSE;
	}
	return (OK);
}

int wselect(int buf_no, int win_no)
{
	int ch;

	if (wselect_show(buf_no, win_no) == ERR)
		return (ERR);

	ch = grgetch();
	switch (ch){
	case KEY_UP:
		if (win[win_no].cur > 0){
			--win[win_no].cur;
			if (win[win_no].cur < win[win_no].top)
				win[win_no].top = win[win_no].cur;
			win[win_no].redraw = TRUE;
		}
		return (win[win_no].cur);
		break;
	case KEY_DOWN:
		if (buf[buf_no].lines - 1 > win[win_no].cur){
			++win[win_no].cur;
			if (win[win_no].cur > win[win_no].top + win[win_no].h - 2)
				++win[win_no].top;
			win[win_no].redraw = TRUE;
		}
		return (win[win_no].cur);
		break;
	case 10:
	case 13:
	case KEY_ENTER:

		win[win_no].redraw = TRUE;
		break;

	}
	return (win[win_no].cur);
}

int wselect_old(int buf_no, int win_no)
{
	int ch;
	int redraw;

	if (win_no < 0 || win_no > WMAX)
		return (ERR);
	if (win[win_no].status & ST_DEFINED == 0)
		return (ERR);

	if (buf_no < 0 || buf_no > WMAX)
		return (ERR);
	if (buf[buf_no].status & ST_DEFINED == 0)
		return (ERR);

	redraw = win[win_no].redraw || curwin != win_no;
	;
	curwin = win_no;

	/* In case lines have disappered but win[].update has not been set */
	if (win[win_no].buf != buf_no || win[win_no].top > buf[buf_no].lines){
		win_reset(win_no);
		win[win_no].buf = buf_no;
		redraw = TRUE;
	}

	if (redraw){
		attroff(A_DIM);
		wdraw(win_no);
		wshowall_c(win_no, buf_no);
		wshowsel_c(win_no, buf_no);
		win[win_no].redraw = FALSE;
	}

	ch = grgetch();
	switch (ch){
	case KEY_UP:
		if (win[win_no].cur > 0){
			--win[win_no].cur;
			if (win[win_no].cur < win[win_no].top)
				win[win_no].top = win[win_no].cur;
			win[win_no].redraw = TRUE;
		}
		return (win[win_no].cur);
		break;
	case KEY_DOWN:
		if (buf[buf_no].lines - 1 > win[win_no].cur){
			++win[win_no].cur;
			if (win[win_no].cur > win[win_no].top + win[win_no].h - 2)
				++win[win_no].top;
			win[win_no].redraw = TRUE;
		}
		return (win[win_no].cur);
		break;
	case 10:
	case 13:
	case KEY_ENTER:

		win[win_no].redraw = TRUE;
		break;

	}
	return (win[win_no].cur);
}

int buf_cpline(int buf_no, int line_no, char *s)
{
	if (buf_no < 0 || buf_no > WMAX)
		return (ERR);
	if (buf[buf_no].status & ST_DEFINED == 0)
		return (ERR);
	if (buf[buf_no].lines <= line_no)
		return (ERR);
	strcpy(s, line[buf_no][line_no]);
	return (line_no);
}

/* This is a special function. Do not use in other applications */

int buf_special_paint(int buf_no, int l)
{

	int curline;
	int c;
	char *p;

	if (buf_no < 0 || buf_no > WMAX)
		return (ERR);
	if ((buf[buf_no].status & ST_DEFINED) == 0)
		return (ERR);
	if ((buf[buf_no].status & ST_NOWRITE) == ST_NOWRITE)
		return (ERR);

	curline = 0;
	while ((p = line[buf_no][curline]) != NULL ){
		c = atoi(p + 1);
		if (c == 0)
			*p = ';';
		else if (c == l)
			*p = '~';
		else if (c < l)
			*p = '.';
		else
			*p = ' ';
		++curline;
	}
}

int convert(int win_no, int buf_no)
{
	char str[116];
	int i, c, cur;

	if (buf_no < 0 || buf_no > WMAX)
		return (ERR);
	if ((buf[buf_no].status & ST_DEFINED) == 0)
		return (ERR);
	if ((buf[buf_no].status & ST_NOWRITE) == ST_NOWRITE)
		return (ERR);

	cur = win[win_no].cur;
	for (i = 0; i < cur && line[buf_no][i] != NULL ; ++i){
		c = line[buf_no][i][0];
		if (c != '#' && c != ';' && c != '.'){
			sprintf(str, ";%s", line[buf_no][i]);
			free(line[buf_no][i]);
			line[buf_no][i] = strdup(str);
			win[win_no].redraw = TRUE;
		}
	}
	return (0);
}

int uconvert(int win_no, int buf_no)
{
	char str[116], *p;
	int i, c;

	if (buf_no < 0 || buf_no > WMAX)
		return (ERR);
	if ((buf[buf_no].status & ST_DEFINED) == 0)
		return (ERR);
	if ((buf[buf_no].status & ST_NOWRITE) == ST_NOWRITE)
		return (ERR);

	for (i = 0; i < BMAX && line[buf_no][i] != NULL ; ++i){
		p = line[buf_no][i];
		c = *p;
		if (c == ';')
		{
			sprintf(str, "%s", p + 1);
			free(line[buf_no][i]);
			line[buf_no][i] = strdup(str);
			win[win_no].redraw = TRUE;
		}
	}
	return (0);
}
//***New: editing supported*/
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
//	x=win[win_no].col;
	x = 1;
	buflines = buf[buf_no].lines;
	if (line[buf_no][0] == NULL )
		line[buf_no][cur] = strdup(" ");
	strcpy(linebuf, line[buf_no][0]);
	curchanged = FALSE;

	redraw = REDRAW_ALL;
	ch = 0;
	while (ch != CTRLX){
		if (mode == INSMODE)
			sprintf(footer,
					" Line:%4d/%-4d Col: %3d Mode: INS                      Save & Exit: Ctrl-X",
					cur + 1, buflines, x + 1);

		sprintf(footer,
				" Line:%4d/%-4d Col: %3d Mode: OVE                      Save & Exit: Ctrl-X",
				cur + 1, buflines, x + 1);

//		footpage=footpage % 3;
		draw_footer(footer);
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
		ch = getch();
		switch (ch)
		{
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
			if (x <= 1)
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
				while (buflines > top + h - 1 && i < h - 1)
				{
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
		case KEY_SLEFT: /* go to start of line */
			x = 1;
			if (first > 1){
				first = 1;
				redraw = REDRAW_ALL;
			}
			break;
		case KEY_END:
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
		case KEY_F0 + 6:
		case KEY_F0 + 7:
		case KEY_F0 + 8:
		case KEY_F0 + 9:
		case KEY_F0 + 10:
		case KEY_F0 + 12:
		case KEY_F0 + 11:
			break;
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
				if (first > x + 1)
					first = x + 1;
				if (top > cur)
					--top;
				redraw = REDRAW_ALL;
			}
			break;

		case KEY_ENTER:
		case 10:
		case 13:
			buflines = ins_line(buf_no, cur + 1, linebuf + x - 1);
			linebuf[x] = '\0';
			free(line[buf_no][cur]);
			line[buf_no][cur] = strdup(linebuf);
			x = 1;
			first = 1;
			++cur;
			strcpy(linebuf, line[buf_no][cur]);
			if (cur > top + h - 2)
				++top;
			redraw = REDRAW_ALL;
			break;

		case KEY_BACKSPACE:
			if (x <= 1)
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
		case ERR:
			break;
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
