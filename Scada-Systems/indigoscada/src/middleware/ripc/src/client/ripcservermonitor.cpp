#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ripc.h"

#if !defined(_WIN32)
#define NO_STRICMP 1
#endif

/**
 * Server monitor class is used to send commands to the JIPC server spawned in daemon mode
 * (without user dialog). 
 * <PRE>
 *    Usage: 
 *        RIPCServerMonitor HOSTNAME PORT [command]
 *    Commands:
 *        SHUTDOWN            shutdown server
 *        INFO                dump information about server status
 * </PRE>
 * If command is not specified in command line, server monitor starts interactive dialog.
 * In this dialog, except SHTDOWN and INFO commands it also supports EXIT and HELP commands.
 * EXIT command is used to close dialog, HELP shows list of available commands.<BR>
 * Server monitor main function. It should be called with 2 or 3 command line parameters.
 * Mandatory parameters are HOSTNAME and PORT. Optional third parameter specifies
 * command to be executed. If third parameter is absent, interactive dialog is started.
 * Otherwise monitor will exit after execution of the command.
 * @param args command line 
 */

void input(char const* prompt, char* buf, size_t buf_size)
{
    char* p;
    do {
	printf(prompt);
	*buf = '\0';
	fgets(buf, buf_size, stdin);
	p = buf + strlen(buf);
    } while (p <= buf+1);

    if (*(p-1) == '\n') {
	*--p = '\0';
    }
}

#if defined(NO_STRICMP) 
#include <ctype.h>
inline int stricmp(const char* p, const char* q)
{
    while (toupper(*(unsigned char*)p) == toupper(*(unsigned char*)q)) { 
	if (*p == '\0') { 
	    return 0;
	}
	p += 1;
	q += 1;
    }
    return toupper(*(unsigned char*)p) - toupper(*(unsigned char*)q);
}
#endif

bool executeCommand(RIPCSession* session, char* cmd)
{
    char buf[32*1024];
    if (stricmp(cmd, "info") == 0) { 
	session->showServerInfo(buf, sizeof(buf));
	fputs(buf, stdout);
    } else if (stricmp(cmd, "shutdown") == 0) { 
	session->shutdownServer();
    } else { 
	fprintf(stderr, "Unknown command: '%s'\n", cmd);
	fprintf(stderr, "Available commands: exit, shutdown, info, help\n");
	return false;
    }
    return true;
}

void dialog(RIPCSession* session) 
{
    char cmd[1024];
    while (true) {
	input("> ", cmd, sizeof(cmd));
	if (stricmp(cmd, "exit") == 0) { 
	    break;
	} else if (stricmp(cmd, "help") == 0 || stricmp(cmd, "?") == 0) { 
	    printf("Commands: exit, shutdown, info, help\n");
	} else { 
	    executeCommand(session, cmd); 
	}
    }
}


int main(int argc, char* argv[]) 
{ 
    if (argc < 3) { 
	fprintf(stderr, "Usage: RIPCServerMonitor HOSTNAME PORT [info|exit]");
	return 1;
    }
    char* hostname = argv[1];
    int   port = atoi(argv[2]);
    RIPCFactory* factory = RIPCClientFactory::getInstance();
    RIPCSession* session = factory->create(hostname, port);
    if (argc > 3) { 
	return executeCommand(session, argv[3]);
    } else { 
	dialog(session);
	return 0;
    }
}

