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
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <process.h>
#include "proc_manager.h"

const int nMaxProcCount = 127;

static PROCESS_INFORMATION pProcInfo[nMaxProcCount];

BOOL StartProcess(char* pCommandLine, char* pWorkingDir)
{ 
	STARTUPINFO startUpInfo = { sizeof(STARTUPINFO),NULL,"",NULL,0,0,0,0,0,0,0,STARTF_USESHOWWINDOW,0,0,NULL,0,0,0};  

	int nIndex = 1;
	
	if(strlen(pCommandLine) > 4)
	{
		char pUserInterface[nBufferSize+1];

		printf("%s\n", pCommandLine);
		
		strcpy(pUserInterface, "Y");

		BOOL bUserInterface = (pUserInterface[0]=='y'||pUserInterface[0]=='Y'||pUserInterface[0]=='1')?TRUE:FALSE;
		char CurrentDesktopName[512];

		// set the correct desktop for the process to be started
		if(bUserInterface)
		{
			//startUpInfo.wShowWindow = SW_SHOW;
			startUpInfo.wShowWindow = SW_SHOWMINIMIZED;
			startUpInfo.lpDesktop = NULL;
		}
		else
		{
			HDESK hCurrentDesktop = GetThreadDesktop(GetCurrentThreadId());
			DWORD len;

			GetUserObjectInformation(hCurrentDesktop,UOI_NAME,CurrentDesktopName,MAX_PATH,&len);
			startUpInfo.wShowWindow = SW_HIDE;
			startUpInfo.lpDesktop = CurrentDesktopName;
		}

		// create the process

		DWORD dwCreateFlg  = 0;
		dwCreateFlg |= CREATE_NEW_CONSOLE;

		memset(&pProcInfo[nIndex], 0, sizeof(PROCESS_INFORMATION));

		if(CreateProcess(NULL,pCommandLine,NULL,NULL,TRUE,dwCreateFlg,NULL,strlen(pWorkingDir)==0?NULL:pWorkingDir,&startUpInfo,&pProcInfo[nIndex]))
		{
			Sleep(100);
			return TRUE;
		}
		else
		{
			fprintf(stderr,"Failed to start program '%s', error code = %d\n", pCommandLine, GetLastError());
			fflush(stderr);
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}

void EndProcess(int nIndex) 
{	
	if(pProcInfo[nIndex].hProcess)
	{
		char pItem[nBufferSize+1];
		sprintf(pItem,"Process%d\0",nIndex);
				
		int nPauseEnd = 100;

		// post a WM_QUIT message first
		PostThreadMessage(pProcInfo[nIndex].dwThreadId,WM_QUIT,0,0);
		// sleep for a while so that the process has a chance to terminate itself
		::Sleep(nPauseEnd>0?nPauseEnd:50);
		// terminate the process by force
		TerminateProcess(pProcInfo[nIndex].hProcess,0);
		pProcInfo[nIndex].hProcess = 0;
	}
}

///////////////////////////////////////////////////////////////////////////
#include <winsock2.h>
#include "clear_crc_eight.h"
#include "iec104types.h"
#include "iec_item.h"

int pipe_put(char* pipe_name, char *buf, int len);

static msg_id = 0;

int send_ack_to_child(int address, int data, char* pipeName)
{
	struct iec_item item_to_send;
	memset(&item_to_send,0x00, sizeof(struct iec_item));
	item_to_send.iec_type = 37;
	item_to_send.iec_obj.ioa = address;
	item_to_send.iec_obj.o.type37.counter = data;
	item_to_send.msg_id = msg_id++;
	item_to_send.checksum = clearCrc((unsigned char *)&item_to_send, sizeof(struct iec_item));
	int rc = pipe_put(pipeName, (char *)&item_to_send, sizeof(struct iec_item)); //Send to process_manager the packet (the first packet is lost)

	return rc;
}

static HANDLE h_pipe = NULL; 

void WorkerProc(void* pParam)
{
	int nIndex = 1;
	const int nBufferSize = 500;
	char pCommandLine[nBufferSize+1];
	char pWorkingDir[nBufferSize+1];
	char pipe_name[150];
	int rc = 0;

	struct args* arg = (struct args*)pParam;

	strcpy(pCommandLine, arg->pCommandLine);
	strcpy(pWorkingDir, arg->pWorkingDir);
	strcpy(pipe_name, arg->pipe_name);

	while(1)
	{
		::Sleep(5000); //5 seconds
		
		if(pProcInfo[nIndex].hProcess)
		{
			DWORD dwCode;

			if(::GetExitCodeProcess(pProcInfo[nIndex].hProcess, &dwCode))
			{
				if(dwCode != STILL_ACTIVE)
				{
					pProcInfo[nIndex].hProcess = 0;

					rc = send_ack_to_child(4004, 5, pipe_name);

					if(rc != 0)
					{
						//wait the orphan child process to stop
						CloseHandle(h_pipe);
						h_pipe = NULL;
						Sleep(30000);
					}
					else
					{
						CloseHandle(h_pipe);
						h_pipe = NULL;
					}

					if(StartProcess(pCommandLine, pWorkingDir))
					{
						fprintf(stderr, "Restarted process %s\n", pCommandLine);
						fflush(stderr);
					}
					else
					{
						fprintf(stderr, "Failed to restart process %s\n", pCommandLine);
						fflush(stderr);
					}
				}
			}
			else
			{
				fprintf(stderr, "GetExitCodeProcess failed, error code = %d\n", GetLastError());
				fflush(stderr);
			}
		}

		rc = send_ack_to_child(4004, 33, pipe_name); //Send a packet every 5 seconds

		if(rc != 0)
		{
			;
		}
	}
}

void begin_process_checker(struct args* arg)
{
	// start a worker thread to check for dead programs (and restart if necessary)
	if(_beginthread(WorkerProc, 0, (void*)arg) == -1)
	{
		fprintf(stderr, "_beginthread failed, error code = %d\n", GetLastError());
		fflush(stderr);
	}
}

int pipe_put(char* pipe_name, char *buf, int len)
{ 
	int rc = 0;
	unsigned long written, mode; 

	if(h_pipe == NULL)
	{
		h_pipe = CreateFile(pipe_name, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

		if(h_pipe == INVALID_HANDLE_VALUE)
		{
			return rc;
		}
		
		if(!WaitNamedPipe(pipe_name, 20))
		{
			return rc;
		}

		mode = PIPE_READMODE_MESSAGE;

		rc = SetNamedPipeHandleState(h_pipe, &mode, NULL, NULL);     

		if(!rc)
		{
   			return rc;
		}
	}

	//fprintf(stderr, "Write to %s\n", pipe_name);
	//fflush(stderr);

    rc = WriteFile(h_pipe, buf, len, &written, NULL);
 
	return rc; 
}

