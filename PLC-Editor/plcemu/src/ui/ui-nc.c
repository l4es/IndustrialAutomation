#include <fcntl.h>
#include <sys/stat.h>
#include <curses.h>
#include <time.h>

#include "wedit.h"
#include "greek.h"
#include "plcemu.h"
#include "util.h"
#include "plclib.h"
#include "ui.h"

/*************GLOBALS************************************************/

int Cur=0;
int PageLen=TERMLEN;
extern int Pagewidth;

int InWin;
int OutWin;
int AIWin;
int AQWin;
int TimWin;
int BlinkWin;
int MVarWin;
int MRealWin;
int EditWin;
int ConfWin;
int LdWin;
int FileWin;
int HelpWin;

int InWinBuf;
int OutWinBuf;
int AIWinBuf;
int AQWinBuf;
int TimWinBuf;
int BlinkWinBuf;
int MVarWinBuf;
int MRealWinBuf;
int EditWinBuf;
int ConfWinBuf;
int LdWinBuf;
int FileWinBuf;
int HelpWinBuf;

int Enable=TRUE;

int WinFlag = WIN_DI; 
int SaveFlag = FALSE; //false loads, true saves



void ui_display_message(char * msgstr)
{
    draw_info_line(PageLen + 1, msgstr);
}

void init_help()
{
	FILE * f;
	char line[MAXSTR], helpline[MAXSTR];
	buf_clear(HelpWinBuf);
    if ((f = fopen("./help", "r"))){
		while (fgets(line, MEDSTR, f)){ //read help file
			sprintf(helpline, " %s", line);
			app_line(HelpWinBuf, helpline);
		}
		fclose(f);
	}
}

void time_header()
{
	char t[TINYSTR], *p;
	char str[SMALLSTR];
	time_t now;

	time(&now);
	strcpy(t, ctime(&now));
	t[19] = '\0';
	p = t + 10;
    sprintf(str, " PLC-EMUlator v%4.2f %14s ", PRINTABLE_VERSION, p);
	draw_header(str);
}

void load_timers()
{
	int i;
	char s[SMALLBUF], color;
	buf_clear(TimWinBuf);
	for (i = 0; i < Plc.nt; i++){
		if (Plc.t[i].Q)
			color = '.'; //red
		else
			color = '#'; //green
        sprintf(s, 
                "%cT%dx%ld\t %ld %ld %s", 
                color, 
                i, 
                Plc.t[i].S, 
                Plc.t[i].V,
				Plc.t[i].P, 
				Plc.t[i].nick);
		//printf("%s\n",s);
		app_line(TimWinBuf, s);
	}
}

void load_blinkers()
{
	int i;
	char s[SMALLBUF], color;
	buf_clear(BlinkWinBuf);
	for (i = 0; i < Plc.ns; i++){
		if (Plc.s[i].Q)
			color = '.'; //red
		else
			color = '#'; //green
        sprintf(s, "%cS%dx%ld\t %s", 
                color, 
                i,
                Plc.s[i].S, 
                Plc.s[i].nick);
		//printf("%s\n",s);
		app_line(BlinkWinBuf, s);
	}
}

void load_mvars()
{
	int i;
    char s[SMALLBUF], color;
	buf_clear(MVarWinBuf);
	for (i = 0; i < Plc.nm; i++){
		if (Plc.m[i].PULSE)
			color = '#'; //green
		else if (Plc.m[i].RO) //locked
			color = '.'; //red
		else
			color = ' ';
        sprintf(s, "%cM%d.\t %ld %s", 
                color, 
                i, 
                Plc.m[i].V, 
                Plc.m[i].nick);
		//printf("%s\n",s);
		app_line(MVarWinBuf, s);
	}
}

void load_mreals()
{
	int i;
    char s[SMALLBUF], color;
	buf_clear(MRealWinBuf);
	for (i = 0; i < Plc.nmr; i++){
		if (Plc.m[i].RO) //locked
			color = '.'; //red
		else
			color = ' ';
        sprintf(s, "%cMF%d.\t %lf %s", 
                color, 
                i, 
                Plc.mr[i].V, 
                Plc.mr[i].nick);
		//printf("%s\n",s);
		app_line(MRealWinBuf, s);
	}
}


void load_inputs()
{
	int i;
    char s[SMALLBUF], color, bit;
	buf_clear(InWinBuf);
	for(i = 0; i < BYTESIZE * Plc.ni; i++){
		if(!(((Plc.maskin[i / BYTESIZE]) >> (i % BYTESIZE)) % 2)
		&& !(((Plc.maskin_N[i / BYTESIZE]) >> (i % BYTESIZE)) % 2)){
			if(((Plc.inputs[i / BYTESIZE]) >> (i % BYTESIZE)) % 2){
				color = '#'; //green
				bit = '1';
			}
			else{
				color = ' ';
				bit = '0';
			}
		}
		else{
			color = '.'; //red
			if (((Plc.maskin[i / BYTESIZE]) >> (i % BYTESIZE)) % 2)
				bit = '1';
			else if (((Plc.maskin_N[i / BYTESIZE]) >> (i % BYTESIZE)) % 2)
				bit = '0';
		}
		sprintf(s, "%cI%d.\t %c %s", 
		        color, 
		        i, 
		        bit, 
		        Plc.di[i].nick);
		//printf("%s\n",s);
		app_line(InWinBuf, s);
	}
}

void load_ai()
{
    int i;
    char s[SMALLBUF];
    char color; 
    double val;
	buf_clear(AIWinBuf);
	for(i = 0; i < Plc.nai; i++){
		if(is_input_forced(&Plc, i) == FALSE){
				color = ' ';
				val = Plc.ai[i].V;
		}
		else{
			color = '.'; //red
			val = Plc.mask_ai[i];
		}
		sprintf(s, "%cIF%d.\t %lf %s", 
		        color, 
		        i, 
		        val, 
		        Plc.ai[i].nick);
		//printf("%s\n",s);
		app_line(AIWinBuf, s);
	}
}

void load_outputs()
{
	int i;
    char s[SMALLBUF], color, bit;
	buf_clear(OutWinBuf);
	for(i = 0; i < BYTESIZE * Plc.nq; i++){
		if(!(((Plc.maskout[i / BYTESIZE]) >> (i % BYTESIZE)) % 2)
		&& !(((Plc.maskout_N[i / BYTESIZE]) >> (i % BYTESIZE)) % 2)){
			if(((Plc.outputs[i / BYTESIZE]) >> (i % BYTESIZE)) % 2){
				bit = '1';
				color = '#'; //green
			}
			else{
				color = ' ';
				bit = '0';
			}
		}
		else{
			color = '.'; //red
			if(((Plc.maskout[i / BYTESIZE]) >> (i % BYTESIZE)) % 2)
				bit = '1';
			else if(((Plc.maskout_N[i / BYTESIZE]) >> 
			         (i % BYTESIZE)) % 2)
				bit = '0';
        }
		sprintf(s, "%cQ%d.\t %c %s", 
		        color, 
		        i, 
		        bit, 
		        Plc.dq[i].nick);
		//printf("%s\n",s);
		app_line(OutWinBuf, s);
	}
}

void load_aq()
{
    int i;
    char s[SMALLBUF];
    char color; 
    double val;
	buf_clear(AQWinBuf);
	for(i = 0; i < Plc.naq; i++){
		if(is_output_forced(&Plc, i) == FALSE){
				color = ' ';
				val = Plc.aq[i].V;
		}
		else{
			color = '.'; //red
			val = Plc.mask_aq[i];
		}
		sprintf(s, "%cQF%d.\t %lf %s", 
		        color, 
		        i, 
		        val, 
		        Plc.aq[i].nick);
		//printf("%s\n",s);
		app_line(AQWinBuf, s);
	}
}


void load_ld()
{
	int i;
	buf_clear(LdWinBuf);
	for (i = 0; i < Lineno; i++)
		app_line(LdWinBuf, Lines[i]);
}

int main_page()
{
	static int redraw = TRUE;
    int c, i, ret;
    //, ch;
	i = 0;
	static int win_sticky;
    if(redraw)
        ui_draw(&Plc, Lines, Lineno);
    ret = PAGE_MAIN;
	if (Enable){
        if (WinFlag == WIN_DI){
			draw_info_line(1,
					"F1/2:Force 1/0|F3:Unforce|F4:Run/Stop|F5:Edit|F7:Load|F8:Save|F9:Help|F10:Quit");
			i = wselect(InWin, InWinBuf);
			win_set(InWin, i);
		}
        else if (WinFlag == WIN_DQ){
			draw_info_line(1,
					"F1/2:Force 1/0|F3:Unforce|F4:Run/Stop|F5:Edit|F7:Load|F8:Save|F9:Help|F10:Quit");
			i = wselect(OutWin, OutWinBuf);
			win_set(OutWin, i);
		}
		if (WinFlag == WIN_AI){
			draw_info_line(1,
					"F1/2:Force 1/0|F3:Unforce|F4:Run/Stop|F5:Edit|F7:Load|F8:Save|F9:Help|F10:Quit");
			i = wselect(AIWin, AIWinBuf);
			win_set(AIWin, i);
		}
        else if (WinFlag == WIN_AQ){
			draw_info_line(1,
					"F1/2:Force 1/0|F3:Unforce|F4:Run/Stop|F5:Edit|F7:Load|F8:Save|F9:Help|F10:Quit");
			i = wselect(AQWin, AQWinBuf);
			win_set(AQWin, i);
		}
        else if (WinFlag == WIN_MEMORY){
			draw_info_line(1,
					"F1/2:Toggle Pulse|F4:Run/Stop|F5:Edit|F6:Lock|F7:Load|F8:Save|F9:Help|F10:Quit");
			i = wselect(MVarWin, MVarWinBuf);
			win_set(MVarWin, i);
		}
		else if (WinFlag == WIN_REAL){
			draw_info_line(1,
					"F1/2:Toggle Pulse|F4:Run/Stop|F5:Edit|F6:Lock|F7:Load|F8:Save|F9:Help|F10:Quit");
			i = wselect(MRealWin, MRealWinBuf);
			win_set(MRealWin, i);
		}
        else if (WinFlag == WIN_TIMER){
			draw_info_line(1,
					"F1:Start|F2:Pause|F3:Reset|F4:Run/Stop|F5:Edit|F7:Load|F8:Save|F9:Help|F10:Quit");
			i = wselect(TimWin, TimWinBuf);
			win_set(TimWin, i);
		}
        else if (WinFlag == WIN_BLINKER){
			draw_info_line(1,
					"F4:Run/Stop|F5:Edit|F7:Load|F8:Save|F9:Help|F10:Quit");
			i = wselect(BlinkWin, BlinkWinBuf);
			win_set(BlinkWin, i);
		}
		else if (WinFlag == WIN_PROGRAM){
			if (Plc.status % 2 == ST_STOPPED){	
				draw_info_line(1,
						"F4:Run and execute |F7:Load|F8:Save|F9:Help|F10:Quit");
                return PAGE_EDITMODE;
			}
			else{
				draw_info_line(1,
						"F4:Stop and edit |F7:Load|F8:Save|F9:Help|F10:Quit");
				i = wselect(LdWin, LdWinBuf);
				win_set(LdWin, i);
			}
		}
		c = lastchar();
        redraw = TRUE;
		switch (c){
		//arrows change window
		case KEY_RIGHT:
			if (WinFlag < WIN_PROGRAM)
				WinFlag++;
			break;
		case KEY_LEFT:
			if (WinFlag > WIN_DI)
				WinFlag--;
			break;
		case KEY_TAB:	//hotkey for edit window
			if (WinFlag < N_WIN){
				win_sticky = WinFlag;
				WinFlag = WIN_PROGRAM;
			}
			else
				WinFlag = win_sticky;
			break;

		case KEY_F(1):    //F1 forces 1
            if (WinFlag == WIN_DI){
				Plc.maskin[i / BYTESIZE] |= (1 << i % BYTESIZE);
				Plc.maskin_N[i / BYTESIZE] &= ~(1 << i % BYTESIZE);
			}
            else if (WinFlag == WIN_DQ){
				Plc.maskout[i / BYTESIZE] |= (1 << i % BYTESIZE);
				Plc.maskout_N[i / BYTESIZE] &= ~(1 << i % BYTESIZE);
			}
			else if (WinFlag == WIN_AI){
			; //TODO: a mechanism for forcing
			}
            else if (WinFlag == WIN_AQ){
			;
			}
            else if (WinFlag == WIN_MEMORY)
                set(&Plc, WIN_MEMORY, i);
            else if (WinFlag == WIN_TIMER)
                set(&Plc, WIN_TIMER, i);
			redraw = TRUE;

			break;
		case KEY_F(2):    //F2 forces 0
            if (WinFlag == WIN_DI){
				Plc.maskin[i / BYTESIZE] &= ~(1 << i % BYTESIZE);
				Plc.maskin_N[i / BYTESIZE] |= (1 << i % BYTESIZE);
			}
            else if (WinFlag == WIN_DQ){
				Plc.maskout[i / BYTESIZE] &= ~(1 << i % BYTESIZE);
				Plc.maskout_N[i / BYTESIZE] |= (1 << i % BYTESIZE);
			}
            else if (WinFlag == WIN_AI){
			; //TODO: a mechanism for forcing
			}
            else if (WinFlag == WIN_AQ){
			;
			}
            
            else if (WinFlag == WIN_MEMORY)
                reset(&Plc, WIN_MEMORY, i);
            else if (WinFlag == WIN_TIMER)
                reset(&Plc, WIN_TIMER, i);

			redraw = TRUE;
			break;
		case KEY_F(3):    //F3 unforces
            if (WinFlag == WIN_DI){
				Plc.maskin[i / BYTESIZE] &= ~(1 << i % BYTESIZE);
				Plc.maskin_N[i / BYTESIZE] &= ~(1 << i % BYTESIZE);
			}
            else if (WinFlag == WIN_DQ){
				Plc.maskout[i / BYTESIZE] &= ~(1 << i % BYTESIZE);
				Plc.maskout_N[i / BYTESIZE] &= ~(1 << i % BYTESIZE);
			}
            else if (WinFlag == WIN_AI){
			; //TODO: a mechanism for forcing
			}
            else if (WinFlag == WIN_AQ){
			;
			}
            
            else if (WinFlag == WIN_TIMER)
				down_timer(&Plc, i);
			redraw = TRUE;
			break;
		case KEY_F(4):    //F4 runs/stops
			if (Plc.status % 2 == ST_STOPPED){//stopped
                plc_log("Start!");
                Plc.status = ST_RUNNING;
            }
			else{
                plc_log("Stop!");
				--Plc.status;    //running
            }
			redraw = TRUE;
			break;
		case KEY_F(5):    //edit
			Cur = i;
            ret = PAGE_EDIT;
			break;
		case KEY_F(6):    //toggle lock
			if (WinFlag == WIN_MEMORY)
				Plc.m[i].RO = (Plc.m[i].RO) ? FALSE : TRUE;
			redraw = TRUE;
			break;

		case KEY_F(7):
            ret = PAGE_FILE;
			SaveFlag = FALSE;
			break;
		case KEY_F(8):
            ret = PAGE_FILE;
			SaveFlag = TRUE;
			break;

		case KEY_F(9):
            ret = PAGE_HELP;
			break;

		case KEY_F(10):
            ret = PAGE_EXIT;
			break;
		default:
			break;
		}
	}
	return ret;
}

int exit_page()
{
	static int redraw = TRUE;
	int c;
	if(redraw){
		wdraw(ConfWin);
		wshowall(ConfWin, ConfWinBuf);
		redraw = FALSE;
	}
	grgetch();
	c = lastchar();
	if (c == 'n' || c == 'N')
		return (0);
	if (c == 'y' || c == 'Y'){
		redraw = TRUE;
		win_clear(ConfWin);
        return (PAGE_MAIN);
	}
    return (PAGE_EXIT);
}

int help_page()
{
	int c, i = 0;
	static int redraw_help = TRUE;
	if (redraw_help){
		wdraw(HelpWin);
		wshowall_c(HelpWin, HelpWinBuf);
		redraw_help = FALSE;
//	win_set(help,i);
	}
	i = wselect(HelpWin, HelpWinBuf);
	win_set(HelpWin, i);

	c = lastchar();

	if (c == KEY_ESC){
		redraw_help = TRUE;
        return (PAGE_MAIN);
	}
    return (PAGE_HELP);
}

int edit_page(int i)
{ //edit comments, value if memory/timer/blinker, scale if timer/blinker, preset & up/down if timer
	static int redraw = TRUE;
	int c;
	static int maxrow;
    static char buf[NICKLEN] = "";
	static int row = 0;
	static int col = 0;
    int y = 0;
            //x, y;
    //n;

	if (Plc.status % 2)    //if running
		--Plc.status;    //running

	if (redraw)
	{    //init window
		redraw = FALSE;
		wdraw(EditWin);
		draw_footer("Esc:Cancel  Enter:Enter");
		wshowall(EditWin, EditWinBuf);
		redraw = FALSE;

		switch (WinFlag){
		case WIN_DI:    //inputs
			maxrow = 1;
			win_puts(EditWin, 1, 1, "Comment        :");
			sprintf(buf, "%s", Plc.di[i].nick);
			win_puts(EditWin, 1, 20, buf);
			break;
		case WIN_DQ:    //outputs
			maxrow = 1;
			win_puts(EditWin, 1, 1, "Comment        :");
			sprintf(buf, "%s", Plc.dq[i].nick);
			win_puts(EditWin, 1, 20, buf);
			break;
		case WIN_AI:    //inputs
			maxrow = 1;
			win_puts(EditWin, 1, 1, "Comment        :");
			sprintf(buf, "%s", Plc.ai[i].nick);
			win_puts(EditWin, 1, 20, buf);
			break;
		case WIN_AQ:    //outputs
			maxrow = 1;
			win_puts(EditWin, 1, 1, "Comment        :");
			sprintf(buf, "%s", Plc.aq[i].nick);
			win_puts(EditWin, 1, 20, buf);
			break;
		
		case WIN_MEMORY:    //memory
			maxrow = 3;
			win_puts(EditWin, 1, 1, "Comment        :");
			sprintf(buf, "%s", Plc.m[i].nick);
			win_puts(EditWin, 1, 20, buf);
			win_puts(EditWin, 2, 1, "Value          :");
            sprintf(buf, "%ld", Plc.m[i].V);
			win_puts(EditWin, 2, 20, buf);
			win_puts(EditWin, 3, 1, "Downcounting     :");
			sprintf(buf, "%d", Plc.m[i].DOWN);
			win_puts(EditWin, 3, 20, buf);
			break;
			
		case WIN_REAL:    //real
			maxrow = 2;
			win_puts(EditWin, 1, 1, "Comment        :");
			sprintf(buf, "%s", Plc.mr[i].nick);
			win_puts(EditWin, 1, 20, buf);
			win_puts(EditWin, 2, 1, "Value          :");
            sprintf(buf, "%lf", Plc.mr[i].V);
			win_puts(EditWin, 2, 20, buf);
			break;
				
		case WIN_TIMER:    //timers
			maxrow = 5;
			win_puts(EditWin, 1, 1, "Comment        :");
			sprintf(buf, "%s", Plc.t[i].nick);
			win_puts(EditWin, 1, 20, buf);
			win_puts(EditWin, 2, 1, "Value          :");
            sprintf(buf, "%ld", Plc.t[i].V);
			win_puts(EditWin, 2, 20, buf);
			win_puts(EditWin, 3, 1, "Preset         :");
            sprintf(buf, "%ld", Plc.t[i].P);
			win_puts(EditWin, 3, 20, buf);
			win_puts(EditWin, 4, 1, "Cycles/count   :");
            sprintf(buf, "%ld", Plc.t[i].S);
			win_puts(EditWin, 4, 20, buf);
			win_puts(EditWin, 5, 1, "ON/OFF delay   :");
			sprintf(buf, "%d", Plc.t[i].ONDELAY);
			win_puts(EditWin, 5, 20, buf);
			break;

		case WIN_BLINKER:    //blinkers
			maxrow = 2;
			win_puts(EditWin, 1, 1, "Comment        :");
			sprintf(buf, "%s", Plc.s[i].nick);
			win_puts(EditWin, 1, 20, buf);
			win_puts(EditWin, 2, 1, "Cycles/count   :");
            sprintf(buf, "%ld", Plc.s[i].S);
			win_puts(EditWin, 2, 20, buf);
			break;
		case WIN_PROGRAM: //last window is for programming
			maxrow = 2;
			win_puts(EditWin, 1, 1, "Press Enter to switch");
			win_puts(EditWin, 2, 1, "languages (PLC will");
			win_puts(EditWin, 3, 1, "stop and the current");
			win_puts(EditWin, 4, 1, "program will be lost!");
		default:
			break;
		}
	}

	col = 0;
	y = row + 1;

	switch (WinFlag){    //update correct row every time
	case WIN_DI:    //inputs
		if (row == 0)
			sprintf(buf, "%s", Plc.di[i].nick);
		break;
	case WIN_DQ:    //outputs
		if (row == 0)
			sprintf(buf, "%s", Plc.dq[i].nick);
		break;
	case WIN_AI:    //inputs
		if (row == 0)
			sprintf(buf, "%s", Plc.ai[i].nick);
		break;
	case WIN_AQ:    //outputs
		if (row == 0)
			sprintf(buf, "%s", Plc.aq[i].nick);
		break;
	
	case WIN_MEMORY:    //memory
		switch (row){
		case 0:
			sprintf(buf, "%s", Plc.m[i].nick);
			break;
		case 1:
            sprintf(buf, "%ld", Plc.m[i].V);
			break;
		case 2:
			sprintf(buf, "%d", Plc.m[i].DOWN);
			break;
		default:
			break;
		}
		break;
	case WIN_REAL:    //reals
		switch (row){
		case 0:
			sprintf(buf, "%s", Plc.mr[i].nick);
			break;
		case 1:
            sprintf(buf, "%lf", Plc.mr[i].V);
			break;
		default:
			break;
		}
		break;	
	case WIN_TIMER:    //timers
		switch (row){
		case 0:
			sprintf(buf, "%s", Plc.t[i].nick);
			break;
		case 1:
            sprintf(buf, "%ld", Plc.t[i].V);
			break;
		case 2:
            sprintf(buf, "%ld", Plc.t[i].P);
			break;
		case 3:
            sprintf(buf, "%ld", Plc.t[i].S);
			break;
		case 4:
			sprintf(buf, "%d", Plc.t[i].ONDELAY);
			break;
		default:

			break;
		}
		break;
	case WIN_BLINKER:    //blinkers
		if (row == 0)
			sprintf(buf, "%s", Plc.s[i].nick);
		if (row == 1)
            sprintf(buf, "%ld", Plc.s[i].S);
		break;
	default:
		break;
	}
	win_gets(EditWinBuf, y, 20, 16, buf);
	switch (WinFlag){    //update correct value with input
	case WIN_DI:    //inputs
		if (row == 0)
			sprintf(Plc.di[i].nick, "%s", buf);
		break;
	case WIN_DQ:    //outputs
		if (row == 0)
			sprintf(Plc.dq[i].nick, "%s", buf);
		break;
	case WIN_AI:    //inputs
		if (row == 0)
			sprintf(Plc.ai[i].nick, "%s", buf);
		break;
	case WIN_AQ:    //outputs
		if (row == 0)
			sprintf(Plc.aq[i].nick, "%s", buf);
		break;
	
	
	case WIN_MEMORY:    //memory
		switch (row){
		case 0:
			sprintf(Plc.m[i].nick, "%s", buf);
			break;
		case 1:
			Plc.m[i].V = (uint64_t)atoi(buf);
			break;
		case 2:
			Plc.m[i].DOWN = atoi(buf) ? 1 : 0;
		default:
			break;
		}
		break;
    case WIN_REAL:    //reals
		switch (row){
		case 0:
			sprintf(Plc.mr[i].nick, "%s", buf);
			break;
		case 1:
			Plc.mr[i].V = atof(buf);
			break;
		default:
			break;
		}
		break;

	case WIN_TIMER:    //timers
		switch (row){
		case 0:
			sprintf(Plc.t[i].nick, "%s", buf);
			break;
		case 1:
			Plc.t[i].V = atoi(buf);
			break;
		case 2:
			Plc.t[i].P = atoi(buf);
			break;
		case 3:
			Plc.t[i].S = atoi(buf);
			break;
		case 4:
			Plc.t[i].ONDELAY = atoi(buf) ? 1 : 0;
		default:
			break;
		}
		break;
	case WIN_BLINKER:    //blinkers
		if (row == 0)
			sprintf(Plc.s[i].nick, "%s", buf);
		if (row == 1)
			Plc.s[i].S = atoi(buf);
		break;

	default:
		break;
	}
	c = lastchar();
	switch (c){
	case KEY_UP:
		if (row > 0)
			--row;
		break;
	case KEY_DOWN:
		if (row < maxrow - 1)
			++row;
		break;
	case KEY_LEFT:
		if (col > 0)
			--col;
		break;
	case KEY_RIGHT:
        if (col < NICKLEN)
			++col;
		break;
	case 10:
	case 13:
	case KEY_END:
		redraw = TRUE;
		win_clear(EditWin);
		Plc.status |= ST_RUNNING;    //RUN mode
		if (WinFlag == WIN_PROGRAM){
			memset(Lines[i], 0, MAXSTR);
			Plc.status |= ST_STOPPED;  //STOP mode
		}
        return (PAGE_MAIN);

	case KEY_ESC:
		redraw = TRUE;
		win_clear(EditWin);
		Plc.status |= ST_RUNNING;    //RUN mode
        return (PAGE_MAIN);
	}
    return (PAGE_EDIT);
    
    //TODO: ranges
}

int file_page()
{
	static int redraw = TRUE;
	static char path[MEDSTR];
	int c;
	static char buf[MEDSTR] = "";

	if (redraw){
		if (Plc.status % 2)    //if running
			--Plc.status;    //running
		wdraw(FileWin);
		draw_footer("Esc:Cancel  Enter:Enter");
		wshowall(FileWin, FileWinBuf);
		redraw = FALSE;
	}
	win_gets(FileWinBuf, 1, 1, MEDSTR, buf);
	c = lastchar();
	if (c == 10 || c == 13){    //enter
		redraw = TRUE;
		win_clear(ConfWin);
		sprintf(path, "%s", buf);
		if (SaveFlag){    //save to file
            if (plc_save_file(path) < 0){
				draw_info_line(PageLen + 1, "Invalid filename!");
                return PAGE_FILE;
			}
		}
		else{    //init from file
            if (plc_load_file(path) < 0)
                return PAGE_FILE;
		}
		Plc.status |= ST_RUNNING;    //RUN mode
        return (PAGE_MAIN);
	}
	if (c == KEY_ESC){
		redraw = TRUE;
		win_clear(FileWin);
		Plc.status |= ST_RUNNING;    //RUN mode
        return (PAGE_MAIN);
	}
    return (PAGE_FILE);
}

int edit_mode()
{
	char line[MAXSTR];
	int i = 0;
	wedit(LdWinBuf, LdWin, 0);	//del_buf);
	WinFlag = 0;
	memset(line, 0, MAXSTR);
	for (i = 0; i <= Lineno; i++){
		memset(Lines[i], 0, MAXSTR);
//		memset(Labels[i], 0, MAXSTR);
	}
	i = 0;
	while (buf_cpline(LdWinBuf, i, line) >= 0){
		line[0] = '-';
		sprintf(Lines[i], "%s", line);
		i++;
		memset(line, 0, MAXSTR);
	}
	Lineno = i;
    return PAGE_MAIN;
}

void ui_draw()
{
    char str[SMALLSTR];
    if (Plc.status % 2) //running
        sprintf(str, "Hardware:%s  RUNNING", Plc.hw);
    else
        sprintf(str, "Hardware:%s  STOPPED", Plc.hw);
    draw_footer(str);
//	draw_info_line(1,"F1/2:Force 1/0|F3:Unforce|F4:Run|F5:Edit|F6:Lock|F7:Load|FBYTESIZE:Save|F9:Help|F10:Quit");

    wdraw(InWin);
    wdraw(OutWin);
    wdraw(AIWin);
    wdraw(AQWin);
 
    wdraw(TimWin);
    wdraw(MVarWin);
    wdraw(MRealWin);
    wdraw(BlinkWin);
    wdraw(LdWin);

    load_inputs();
    load_outputs();
    load_ai();
    load_aq();
    load_mvars();
    load_mreals();
    load_timers();
    load_blinkers();
    load_ld();

    wshowall_c(InWin, InWinBuf);
    wshowall_c(OutWin, OutWinBuf);
    wshowall_c(AIWin, AIWinBuf);
    wshowall_c(AQWin, AQWinBuf);
    
    wshowall_c(MVarWin, MVarWinBuf);
    wshowall_c(MRealWin, MRealWinBuf);
    wshowall_c(TimWin, TimWinBuf);
    wshowall_c(BlinkWin, BlinkWinBuf);
    wshowall_c(LdWin, LdWinBuf);
}

#define TOP 3
#define BOTTOM 1
#define LEFT 1
#define RIGHT 1
#define PAD_X 1
#define PAD_Y 1
#define MIDDLE (TOP + PAD_Y + PageLen) / 2
#define BOX_H PageLen / 4 - 2
#define LONG_BOX_H PageLen / 2 -3
#define BOX_W screen_w / 4


int ui_init(int screen_h, int screen_w)
{//TODO: analog windows optional
    char str[SMALLSTR];
    WinFlag = WIN_DI;
    Enable = 1;
    Cur = 0;
    win_start();
    PageLen = screen_h - 3;
    InWin = win_open(TOP,
                     LEFT, 
                     BOX_H, 
                     BOX_W - PAD_X, 
                     "DIGITAL INPUTS");
                     
    OutWin = win_open(TOP,
                      LEFT + BOX_W, 
                      BOX_H, 
                      BOX_W - PAD_X, 
                      "DIGITAL OUTPUTS");
                      
    AIWin = win_open(TOP + PAD_Y + BOX_H,
                     LEFT, 
                     BOX_H, 
                     BOX_W - PAD_X, 
                     "ANALOG INPUTS");
                     
    AQWin = win_open(TOP + PAD_Y + BOX_H,
                      LEFT + BOX_W, 
                      BOX_H, 
                      BOX_W - PAD_X, 
                      "ANALOG OUTPUTS");                  
                      
    MVarWin = win_open(TOP,
                       LEFT + 2 * BOX_W,
                       BOX_H, 
                       BOX_W - PAD_X, 
                       "MEMORY COUNTERS");
    
    MRealWin = win_open(TOP + PAD_Y + BOX_H,
                       LEFT + 2 * BOX_W,
                       BOX_H, 
                       BOX_W - PAD_X, 
                       "MEMORY VARIABLES");                   
                       
    TimWin = win_open(TOP,
                      LEFT + 3 * BOX_W,
                      BOX_H, 
                      BOX_W - PAD_X - RIGHT, 
                      "TIMERS");
                      
    BlinkWin = win_open(TOP + PAD_Y + BOX_H, 
                        LEFT + 3 * BOX_W, 
                        BOX_H, 
                        BOX_W - PAD_X - RIGHT,
                        "BLINKERS");
                        
    LdWin = win_open(MIDDLE, 
                     LEFT, 
                     LONG_BOX_H - BOTTOM, 
                     screen_w - LEFT - RIGHT,
                     "PLC TASK");
                     
    EditWin = win_open(10, 15, 8, 40, " Configuration ");
    ConfWin = win_open(10, 15, 4, 50, " Exit PLC-emu ? ");
    FileWin = win_open(10, 15, 8, 40, " Write a UNIX path");
    HelpWin = win_open(6, 1, PageLen - 10, 80, "HELP");

    InWinBuf = buf_open();
    OutWinBuf = buf_open();
    AIWinBuf = buf_open();
    AQWinBuf = buf_open();
    MVarWinBuf = buf_open();
    MRealWinBuf = buf_open();
    TimWinBuf = buf_open();
    BlinkWinBuf = buf_open();
    LdWinBuf = buf_open();
    EditWinBuf = buf_open();
    ConfWinBuf = buf_open();
    strcpy(str, " Quit?  wh(Y)? Why (N)ot? ");
    app_line(ConfWinBuf, str);

    FileWinBuf = buf_open();
    HelpWinBuf = buf_open();
    init_help();
    return 1;
}

int ui_update(int page)
{
    int nextpage=0;
    time_header();
    switch(page){
        case PAGE_MAIN:
            nextpage = main_page();
            break;
        case PAGE_EXIT:
            nextpage = exit_page();
            break;
        case PAGE_EDIT:
            nextpage = edit_page(Cur);
            break;
        case PAGE_FILE:
            nextpage = file_page();
            break;
        case PAGE_EDITMODE:
            nextpage = edit_mode();
            break;
        case PAGE_HELP:
            nextpage = help_page();
            break;
        default:
            break;
    }
    return nextpage;
}

void ui_end()
{
    win_end();
}

void ui_toggle_enabled()
{
    Enable = Enable ? 0 : 1;
}
