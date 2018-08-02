/*
 * Copyright (c) 2004-2010
 */

#ifndef PLATFORMS_H
#define PLATFORMS_H


#ifdef ENV_WINDOWS
 #include <windows.h>

 #define MSG_NOSIGNAL 0
#endif

#ifdef ENV_LINUX
typedef void* HINSTANCE;

extern bool reloadSignal;
#endif

#ifdef ENV_QNX
typedef void* HINSTANCE;
#endif

void showLog(const char *format,...);
bool LeaveServer(void);

#ifndef ENV_WINDOWS
void setupSignalTraps();
#else
#ifdef ENV_WINDOWSCE
int splitCmdLine(unsigned short *lpCmdLine,char *argv[]);
#else
int splitCmdLine(char *lpCmdLine,char *argv[]);
#endif
#endif

#endif // PLATFORMS_H
