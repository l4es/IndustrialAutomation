#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <ctype.h>
#include <math.h>

#include "plcemu.h"
#include "util.h"
#include "plclib.h"
#include "config.h"
#include "parser-tree.h"
#include "parser-il.h"
#include "parser-ld.h"
#include "ui.h"
#include "hardware.h"
#include "init.h"


/*************GLOBALS************************************************/
struct PLC_regs Plc;
//TODO: this will go away
char com_nick[MEDSTR][COMMLEN];///comments for up to 256 serial commands

char Lines[MAXBUF][MAXSTR];///program lines
int Lineno;	///actual no of active lines
int UiReady=FALSE;
BYTE Update=FALSE;

int Language = LANG_LD;

typedef enum
{
    MSG_PLCERR,
    MSG_BADCHAR,
    MSG_BADFILE,
    MSG_BADOPERAND,
    MSG_BADINDEX,
    MSG_BADCOIL,
    MSG_BADOPERATOR,
    MSG_TIMEOUT,
    MSG_OVFLOW,
    N_ERRMSG
}ERRORMESSAGES;

const char ErrMsg[N_ERRMSG][MEDSTR] =
{ 		"Something went terribly wrong!",
		"Invalid Symbol!",
		"Wrong File!",
		"Invalid Operand!",
		"Invalid Numeric!",
		"Invalid Output!",
		"Invalid Command!",
		"Timeout!",
		"Stack overflow!"
};

const char LangStr[3][TINYSTR] =
{ 	"Ladder Diagram",
	"Instruction List",
	"Structured Text"
};


void sigenable()
{
    ui_toggle_enabled();
}

/**
* @brief graceful shutdown
*/
void sigkill()
{
    plc_log("program interrupted");
    double mean, var = 0;
    unsigned long loop = get_loop();
    get_variance(&mean, &var);
    plc_log("Total loops: %d", loop);
    plc_log("Average loop time: %f us", mean);
    plc_log("Standard deviation: +-%f us", sqrt(var));
    ui_end();
    exit(0);
}

void init_emu(const config_t conf, plc_t plc)
{
    memset(plc, 0, sizeof(struct PLC_regs));
    configure(conf, plc);
    hw_config(conf);
    Update = TRUE;
    
	signal(conf->sigenable, sigenable);
	signal(SIGINT, sigkill);
	signal(SIGTERM, sigkill);
	
	project_init();
	open_pipe(conf->pipe, plc);
}

int plc_load_file(const char * path)
{ 
	FILE * f;
    char * tab = 0;
    int idx = 0;
    int r = 0;
    //int lineno=0;
    int i=0;
    int j=0;
    int found_start = FALSE;
    int ld = FALSE;
    int il = FALSE;
    char line[MAXSTR], name[SMALLSTR], val[SMALLSTR];
    
    if ((f = fopen(path, "r"))){
        memset(line, 0, MAXSTR);
        memset(name, 0, SMALLSTR);
        memset(val, 0, NICKLEN);
		disable_bus();
        while (fgets(line, MAXSTR, f)){    //read initialization values
            j = extract_name(line, name, 0);
            if(ld || il)
                found_start = TRUE;
            if (!found_start){
			    ld = !strcmp(name, "LD");
                il = !strcmp(name, "IL");
               
                if (j < 0)
                    break;

                j = extract_index(line, &idx, j);
                if (j < 0)
                    break;
                
                if (idx < 0){
                    r = ERR_BADINDEX;
                    break;
                }
                extract_value( line, val, j);
                //lineno++;
               // plc_log("Found: %s, %d, %s\n", name, idx, val); 
                if((r = declare_input(name, idx, val))<0)
                    break;
                if((r = declare_output(name, idx, val))<0)
                    break;
                
                if((r = declare_analog_input(name, idx, val))<0)
                    break;
                if((r = declare_analog_output(name, idx, val))<0)
                    break;
                if((r = configure_input_min(name, idx, val))<0)
                    break;
                if((r = configure_output_min(name, idx, val))<0)
                    break;
                if((r = configure_input_max(name, idx, val))<0)
                    break;
                if((r = configure_output_max(name, idx, val))<0)
                    break;
                                
                if((r = declare_register(name, idx, val))<0)
                    break;
                if((r = declare_register_r(name, idx, val))<0)
                    break;    
                if((r = declare_timer(name, idx, val))<0)
                    break;
                if((r = declare_blinker(name, idx, val))<0)
                    break;
                if((r = declare_serial(name, idx, val))<0)
                    break;
                if((r = init_register(name, idx, val))<0)
                    break;
                if((r = init_register_r(name, idx, val))<0)
                    break;    
                if((r = define_reg_direction(name, idx, val))<0)
                    break;
                if((r = define_reg_readonly(name, idx, val))<0)
                    break;
                if((r = init_timer_set(name, idx, val))<0)
                    break;
                if((r = init_timer_preset(name, idx, val))<0)
                    break;
                if((r = init_timer_delay(name, idx, val))<0)
                    break;
                if((r = init_blinker_set(name, idx, val))<0)
                    break;
                if (name[0] != ';'
                && !isalnum(name[0])
                && !isspace(name[0])
                && !isblank(name[0])
                && name[0] != 0){
					r = ERR_BADOPERAND;
					break;
				}
			}
			else{//copy line
			    while (strchr(line, '\t') != NULL ){
			    //tabs are not supported
					tab = strchr(line, '\t');
					*tab = '.';
				}
				memset(Lines[i], 0, MAXSTR);
				sprintf(Lines[i++], "%s", line);
			}
            r = PLC_OK;
			memset(line, 0, MAXSTR);
			memset(name, 0, SMALLSTR);
		}
		enable_bus();
		fclose(f);
		Lineno = i;
		plc_log("Successfully loaded %d lines of %s code from %s", 
		i, ld?"LD":"IL", path);
		
		if(ld)
		    r = parse_ld_program(Lines, &Plc);
		else
		    r = parse_il_program(Lines, &Plc);
		
	}
	else
		r = ERR_BADFILE;
//	printf(msg,"");
	if (r < 0){
		switch (r){
		case ERR_BADFILE:
            plc_log( "Invalid filename:!");
			break;
	
		default:
			break;
		}
		return PLC_ERR;
	}
	else{
	    char dump[MAXSTR * MAXBUF];
        memset(dump, 0, MAXBUF * MAXSTR);
        dump_rung(Plc.rungs[0], dump);
        printf("%s", dump); 
        return PLC_OK;
    }
}

int plc_save_file(const char *path)
{
    FILE * f;
    int i;
    //open file for writing
    if ((f = fopen(path, "w")) == NULL ){
        return PLC_ERR;
    }
    else{
        for (i = 0; i < Plc.ni * BYTESIZE; i++){
            if (Plc.di[i].nick[0] != 0)
                fprintf(f, "I\t%d\t%s\t\n", i, Plc.di[i].nick);
        }
        for (i = 0; i < Plc.nq * BYTESIZE; i++){
            if (Plc.dq[i].nick[0] != 0)
                fprintf(f, "Q\t%d\t%s\t\n", i, Plc.dq[i].nick);
        }
        for (i = 0; i < Plc.nai ; i++){
            if (Plc.ai[i].nick[0] != 0)
                fprintf(f, "IF\t%d\t%s\t\n", i, Plc.ai[i].nick);
            fprintf(f, "I_MIN\t%d\t%f\t\n", i, Plc.ai[i].min);
            fprintf(f, "I_MAX\t%d\t%f\t\n", i, Plc.ai[i].max);    
        }
        for (i = 0; i < Plc.naq; i++){
            if (Plc.aq[i].nick[0] != 0)
                fprintf(f, "QF\t%d\t%s\t\n", i, Plc.aq[i].nick);
            fprintf(f, "Q_MIN\t%d\t%f\t\n", i, Plc.aq[i].min);
            fprintf(f, "Q_MAX\t%d\t%f\t\n", i, Plc.aq[i].max);    
        }
        

        for (i = 0; i < Plc.nm; i++){
            if (Plc.m[i].nick[0] != 0)
                fprintf(f, "M\t%d\t%s\t\n", i, Plc.m[i].nick);
            if (Plc.m[i].V > 0)
                fprintf(f, "MEMORY\t%d\t%ld\t\n", i, Plc.m[i].V);
            if (Plc.m[i].DOWN > 0)
                fprintf(f, "COUNT\t%d\tDOWN\t\n", i);
            if (Plc.m[i].RO > 0)
                fprintf(f, "COUNTER\t%d\tOFF\t\n", i);
        }
        for (i = 0; i < Plc.nmr; i++){
            if (Plc.mr[i].nick[0] != 0)
                fprintf(f, "MF\t%d\t%s\t\n", i, Plc.mr[i].nick);
            if (Plc.mr[i].V > FLOAT_PRECISION)
                fprintf(f, "REAL\t%d\t%lf\t\n", i, Plc.mr[i].V);
        }

        for (i = 0; i < Plc.nt; i++){
            if (Plc.t[i].nick[0] != 0)
                fprintf(f, "T\t%d\t%s\t\n", i, Plc.t[i].nick);
            if (Plc.t[i].S > 0)
                fprintf(f, "TIME\t%d\t%ld\t\n", i, Plc.t[i].S);
            if (Plc.t[i].P > 0)
                fprintf(f, "PRESET\t%d\t%ld\t\n", i, Plc.t[i].P);
            if (Plc.t[i].ONDELAY > 0)
                fprintf(f, "DELAY\t%d\tON\t\n", i);
        }
        for (i = 0; i < Plc.ns; i++){
            if (Plc.s[i].nick[0] != 0)
                fprintf(f, "B\t%d\t%s\t\n", i, Plc.s[i].nick);
            if (Plc.s[i].S > 0)
                fprintf(f, "BLINK\t%d\t%ld\t\n", i, Plc.s[i].S);
        }
        for (i = 0; i < MEDSTR; i++){
            if (com_nick[i][0] != 0)
                fprintf(f, "COM\t%d\t%s\t\n", i, com_nick[i]);
        }
        fprintf(f, "\n%s\n", "LD");
        for (i = 0; i < Lineno; i++)
            fprintf(f, "%s\n", Lines[i]);
        fclose(f);
    }
    return 0;
}

const char * Usage = "Usage: plcemu [-i program file] [-c config file] [-d] \n \
        Options:\n \
        -i loads initially a text file with initialization values and LD/IL program, \n \
        -c uses a configuration file other than plc.config \n \
        -d runs PLC-EMU as daemon \n";

void print_error(int errcode)
{
    const char * errmsg;
    switch(errcode){
        case ERR_BADCHAR:
            errmsg = ErrMsg[MSG_BADCHAR];
            break;
        case ERR_BADFILE:
            errmsg = ErrMsg[MSG_BADFILE];
            break;
        case ERR_BADOPERAND:
            errmsg = ErrMsg[MSG_BADOPERAND];
            break;
        case ERR_BADINDEX:
            errmsg = ErrMsg[MSG_BADINDEX];
            break; 
        case ERR_BADCOIL:
            errmsg = ErrMsg[MSG_BADCOIL];
            break;
        case ERR_BADOPERATOR:
            errmsg = ErrMsg[MSG_BADOPERATOR];
            break;
        case ERR_TIMEOUT:
            errmsg = ErrMsg[MSG_TIMEOUT];
            break;   
        case ERR_OVFLOW:
            errmsg = ErrMsg[MSG_OVFLOW];
            break;                 
        default://PLC_ERR
            errmsg = ErrMsg[MSG_PLCERR];
    }
    plc_log("error %d: %s", -errcode, errmsg);
}


int main(int argc, char **argv)
{
    int i, errcode, daemon_flag = FALSE;
    int more = 0;
    char confstr[SMALLSTR], inistr[SMALLSTR];
    config_t conf = init_config();
	strcpy(confstr, "plc.config");
    sprintf(inistr, " ");
  
	for (i = 1; i < argc; i++){
		if (!strcmp(argv[i], "-d"))
			daemon_flag = TRUE;
		else{	//check if previous arg was -i or -c
			if (!strcmp(argv[i - 1], "-i"))
				strcpy(inistr, argv[i]);
			else if (!strcmp(argv[i - 1], "-c"))
				strcpy(confstr, argv[i]);
			else
			{
				if ((strcmp(argv[i], "-i") && strcmp(argv[i], "-c"))
						|| argc == i + 1)
				{
                    printf("%s", Usage);
                    return PLC_ERR;
				}
			}
		}
	}
	
	if (load_config(confstr, conf) < PLC_OK){
		printf("Invalid configuration file\n");
        return PLC_ERR;
	}

    init_emu(conf, &Plc);
    
    errcode = plc_load_file(inistr);
    if (inistr[0] && errcode < PLC_OK)
		printf("Invalid program file\n");

	if (daemon_flag == FALSE){
        more = ui_init(conf->page_len, conf->page_width);
        UiReady=more;
    }
    else
        more = 1;
    while (more > 0 ){
        if(daemon_flag == FALSE){
            if(Update == TRUE)
                ui_draw(&Plc, Lines, Lineno);
            more = ui_update(more);
        }
        else
            more = Plc.status;
        if(errcode >= PLC_OK && Plc.status > 0){  
            errcode = plc_func(&Update, &Plc);
            if(errcode < 0){
                print_error(errcode);
                Plc.status = 0;
            }
        }
	}
    
	disable_bus();
	clear_config(&conf);
	close_log();
	
    if (daemon_flag)
        ui_end();
    return 0;
}
