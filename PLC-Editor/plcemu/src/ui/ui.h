#ifndef _UI_H_
#define _UI_H_

/**
 *@file ui.h
 *@brief main header
*/

enum
{
    PAGE_MAIN = 1,
    PAGE_EXIT,
    PAGE_EDIT,
    PAGE_FILE,
    PAGE_EDITMODE,
    PAGE_HELP,
    N_PAGES
}PAGES;

typedef enum
{///winows
    WIN_DI,        ///digital input
    WIN_AI,        ///analog input
    WIN_DQ,        ///digital output
    WIN_AQ,        ///analog output
    WIN_MEMORY,   ///pulse of counter
    WIN_REAL,     ///floating point variable
    WIN_TIMER,     ///output of timer
    WIN_BLINKER,   ///output of blinker
    WIN_PROGRAM,
    N_WIN
}WINDOWS;

extern struct PLC_regs Plc;
extern char Lines[MAXBUF][MAXSTR];///ladder lines
extern int Lineno;	///actual no of active lines

void ui_display_message(char *msgstr);
void ui_draw();
int ui_update(int page);
int ui_init(int h, int w);
void ui_end();
void ui_toggle_enabled();

#endif //UI_H
