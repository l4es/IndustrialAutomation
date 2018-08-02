/*  OpenDAX - An open source data acquisition and control system 
 *  Copyright (c) 2007 Phil Birkelbach
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *  Main source code file for the OpenDAX Command Line Client  module
 */

#include <daxc.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <getopt.h>

#define HISTORY_FILE ".dax_history"

int runcmd(char *inst);
int runfile(char *filename);
char *rl_gets(const char *prompt);
void quit_signal(int sig);
static void getout(int exitstatus);

static int _quitsignal = 0;
static char *history_file = NULL;
dax_state *ds;
int quiet_mode;

/* main inits and then calls run */
int main(int argc,char *argv[]) {
    struct sigaction sa;
    int flags, result = 0;
    char *instr, *command, *filename;
    char *home_dir;
    
 /* Set up the signal handlers */
    memset (&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = &quit_signal;
    sigaction (SIGQUIT, &sa, NULL);
    sigaction (SIGINT, &sa, NULL);
    sigaction (SIGTERM, &sa, NULL);
    
    ds = dax_init("daxc");
    if(ds == NULL) {
        fprintf(stderr, "Unable to Allocate DaxState Object\n");
        return ERR_ALLOC;
    }
    
    dax_init_config(ds, "daxc");
    flags = CFG_CMDLINE | CFG_ARG_REQUIRED;
    result += dax_add_attribute(ds, "execute","execute", 'x', flags, NULL);
    result += dax_add_attribute(ds, "file","file", 'f', flags, NULL);
    flags = CFG_CMDLINE;
    result += dax_add_attribute(ds, "quiet","quiet", 'q', flags, NULL);
    result += dax_add_attribute(ds, "interactive","interactive", 'i', flags, NULL);

    dax_configure(ds, argc, argv, CFG_CMDLINE | CFG_DAXCONF);
    
    /* TODO: These have got to move to the configuration */
    dax_set_debug_topic(ds, 0);
        
 /* Check for OpenDAX and register the module */
    if( dax_connect(ds) ) {
        dax_fatal(ds, "Unable to find OpenDAX");
        getout(_quitsignal);
    }
    /* No setup work to do here.  We'll go straight to running */
    dax_mod_set(ds, MOD_CMD_RUNNING, NULL);

    if(dax_get_attr(ds, "quiet")) {
        quiet_mode = 1;
    }
    
    command = dax_get_attr(ds, "execute");
    
    if(command) {
        runcmd(command);
    }
    
    filename = dax_get_attr(ds, "file");
    if(filename) {
        runfile(filename);
    }
    if((filename || command) && !dax_get_attr(ds, "interactive")) {
        getout(0);
    }

/* At this point we are in interactive mode.  We first read the 
 * readline history file and then start an infininte loop where
 * We call the readline function repeatedly and then send the
 * input to runcmd */
    quiet_mode = 0; /* We turn quiet mode off for interactive mode */

    home_dir = getenv("HOME");
    if(home_dir != NULL) {
        history_file = malloc(strlen(home_dir) + strlen(HISTORY_FILE) + 2);
        if(history_file != NULL) {
            using_history();
            sprintf(history_file, "%s/%s", home_dir, HISTORY_FILE);
            read_history(history_file);
            /* TODO: Add to configuration */
            stifle_history(100);
        }
    }
    while(1) {
        if(_quitsignal) {
            dax_debug(ds, LOG_MAJOR, "Quitting due to signal %d", _quitsignal);
            getout(_quitsignal);
        }
        
        instr = rl_gets("dax>");
    
        if( instr == NULL ) {
            /* End of File */
            getout(0);
        }
        if( instr[0] != '\0' ) {
            runcmd(instr); /* Off we go */    
        }
    }
    
 /* This is just to make the compiler happy */
    return(0);
}


/* Takes the execution string tokenizes it and runs the appropriate function */
int
runcmd(char *instr)
{
    int tcount = 0, n, result = 0;
    char *last, *temp, *tok;
    char **tokens;
    
    /* Store the string to protect if from strtok_r() */
    temp = strdup(instr);
    if(temp == NULL) {
        fprintf(stderr, "ERROR: Unable to allocate memory\n");
        return -1;
    }
    /* First we Count the Tokens */
    tok = strtok_r(temp, " ", &last);
    if(tok) {
        tcount = 1;
        while( (tok = strtok_r(NULL, " ", &last)) ) {
            tcount++;
        }
    }
    
    /* Now that we know how many tokens we can allocate the array 
     * We get one more than needed so that we can add the NULL */
    tokens = malloc(sizeof(char *) * tcount+1);
    
    if(tokens == NULL) {
        fprintf(stderr, "ERROR: Unable to allocate memory\n");
        free(temp);
        return -1;
    }
    
    /* Since strtok_r() will mess up our string we need to copy it again */
    strcpy(temp, instr);
    tokens[0] = strtok_r(temp, " ", &last);
    /* Get all the tokens */
    for(n = 1; n < tcount; n++) {
        tokens[n] = strtok_r(NULL, " ", &last);
    }
    tokens[tcount] = NULL; /* Terminate the array with a NULL */    
    
    /* test the command in sequential if's once found call function and continue */
    if( !strcasecmp(tokens[0],"dax")) {
        printf("Haven't done 'dax' yet\n");
    } else if( !strncasecmp(tokens[0], "add", 1)) {
        result = tag_add(&tokens[1]);
    } else if( !strncasecmp(tokens[0], "list", 4)) {
        if(tokens[1] == NULL || !strncasecmp(tokens[1], "tag", 3)) {
            result = list_tags(&tokens[2]);
        } else if(!strncasecmp(tokens[1], "type", 3)) {
            result = list_types(&tokens[2]);
        } else {
            fprintf(stderr, "ERROR: Unknown list parameter %s\n", tokens[1]);
        }
    } else if( !strncasecmp(tokens[0], "read", 1)) {
        result = tag_read(&tokens[1]);
    } else if( !strncasecmp(tokens[0], "write", 1)) {
        result = tag_write(&tokens[1], tcount-1);
    } else if( !strncasecmp(tokens[0], "cdt", 3)) {
        result = cdt_add(&tokens[1], tcount -1);
//    } else if( !strncasecmp(tokens[0], "mod", 3)) {
//        printf("Haven't done 'mod' yet!\n");
    } else if( !strcasecmp(tokens[0],"db")) {
        result = db_read(&tokens[1]);
//        if(tokens[1] == NULL) fprintf(stderr, "ERROR: Missing Subcommand\n");
//        else if( !strcasecmp(tokens[1], "read")) result = db_read();
//    //  else if( !strcasecmp(tokens[1], "readbit")) result = db_read_bit();
//        else if( !strcasecmp(tokens[1], "write")) result = db_write();
//        else if( !strcasecmp(tokens[1], "format")) result = db_format();
//        else fprintf(stderr, "ERROR: Unknown Subcommand - %s\n", tokens[0]);
    } else if( !strncasecmp(tokens[0], "event", 5)) {
        if( tokens[1] == NULL) {
            fprintf(stderr, "ERROR: EVENT requires a subcommand.  Try ADD, DEL, WAIT, POLL\n");
        } else if( !strncasecmp(tokens[1], "add", 3)) {
            event_add(&tokens[2], tcount - 2);
        } else if( !strncasecmp(tokens[1], "del", 3)) {
            event_del(&tokens[2]);
        } else if( !strncasecmp(tokens[1], "wait", 4)) {
            event_wait(&tokens[2]);
        } else if( !strncasecmp(tokens[1], "poll", 4)) {
            event_poll();
        } else {
            fprintf(stderr, "ERROR: Unknown subcommand '%s'\n", tokens[1]);
        }
    } else if( !strcasecmp(tokens[0],"msg")) {
        printf("Haven't done 'msg' yet!\n");    
    /* TODO: Really should work on the help command */
    } else if( !strcasecmp(tokens[0], "help")) {
        printf("Hehehehe, Yea right!\n");
        printf(" Try READ, WRITE, LIST, ADD\n");
    
    } else if( !strcasecmp(tokens[0],"exit")) {
        getout(0);
    
    } else {
        fprintf(stderr, "Unknown Command - %s\n", tokens[0]);
    }
    
    free(tokens);
    return result;
}

int
runfile(char *filename)
{
    char string[LINE_BUFF_SIZE];
    FILE *file;
    int len;
    
    file = fopen(filename, "r");
    if(file == NULL) {
        fprintf(stderr, "ERROR: Unable to open file - %s\n", filename);
        return -1;
    }
    while(!feof(file)) {
        fgets(string, LINE_BUFF_SIZE, file);
        len = strlen(string);
        if(string[len-1] == '\n') string[len-1] = '\0';
        runcmd(string);
    }
    return 0;
}

/* TODO: Need to conditionally compile this function to
   either use libreadline or just use gets */
/* Read a string, and return a pointer to it.
   Returns NULL on EOF. */
char *
rl_gets(const char *prompt)
{
    static char *line_read = (char *)NULL;
    
    /* If the buffer has already been allocated,
       return the memory to the free pool. */
    if (line_read) {
        free(line_read);
        line_read = (char *)NULL;
    }

    /* Get a line from the user. */
    line_read = readline(prompt);

    /* If the line has any text in it,
     * save it on the history. */
    if (line_read && *line_read)
        add_history(line_read);

    return(line_read);
}

/* Signal handler - mainly jsut cleanup */
void
quit_signal(int sig)
{
    _quitsignal = sig;
    //getout(-1);
}

static void
getout(int exitstatus)
{
    if(history_file != NULL) {
        write_history(history_file);
        free(history_file);
    }
    dax_disconnect(ds);
    exit(exitstatus);
}
