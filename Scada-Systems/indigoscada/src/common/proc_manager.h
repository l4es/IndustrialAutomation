/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2011 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */

#ifndef PROC_MANAGER_H
#define PROC_MANAGER_H 

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*p_send_ack_to_child)(int address, int data, char* pipeName);

typedef bool (*p_StartProcess)(char* pCommandLine, char* pWorkingDir, PROCESS_INFORMATION* pProcInfo);

extern bool StartProcess(char* pCommandLine, char* pWorkingDir, PROCESS_INFORMATION* pProcInfo);
//extern void EndProcess(int nIndex);
extern void begin_process_checker(struct args* arg);

const int nBufferSize = 500;

struct args{
	char pCommandLine[nBufferSize+1];
	char pWorkingDir[nBufferSize+1];
	char pipe_name[150];
    PROCESS_INFORMATION* pProcInfo;
    p_send_ack_to_child send_ack_to_child;
    p_StartProcess StartProcess;
};

#ifdef __cplusplus
}
#endif

#endif