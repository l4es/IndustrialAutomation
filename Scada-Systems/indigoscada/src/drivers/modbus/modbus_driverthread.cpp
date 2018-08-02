/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2009 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */
#include "modbus_driver_instance.h"
#include "modbus_driverthread.h"
/*
*Function:run
*runs the thread
*Inputs:none
*Outputs:none
*Returns:none
*/

void Modbus_DriverThread::run()
{
	IT_IT("Modbus_DriverThread::run");

	IT_COMMENT("Modbus_DriverThread Running");

    ///////////////////start child process modbus_master.exe////////////////////
	char line_number[50];
	char polling_time[50];

	strcpy(pipe_name, "\\\\.\\pipe\\modbus_master_namedpipe");

	itoa(instanceID + 1, line_number, 10);

    strcat(pipe_name, line_number);

	itoa(((Modbus_driver_Instance*)Parent)->Cfg.SampleTime, polling_time, 10);

	if(((Modbus_driver_Instance*)Parent)->Cfg.context == InstanceCfg::TCP)
	{
		//MODBUS TCP
		strcpy(pCommandLine, GetScadaHomeDirectory());
		strcat(pCommandLine, "\\bin\\modbus_master.exe -a ");
		strcat(pCommandLine, ((Modbus_driver_Instance*)Parent)->Cfg.MODBUSServerIPAddress);
		strcat(pCommandLine, " -p ");
		strcat(pCommandLine, ((Modbus_driver_Instance*)Parent)->Cfg.MODBUSServerTCPPort);
		strcat(pCommandLine, " -l ");
		strcat(pCommandLine, line_number);
		strcat(pCommandLine, " -t ");
		strcat(pCommandLine, polling_time);
	}
	else if(((Modbus_driver_Instance*)Parent)->Cfg.context == InstanceCfg::RTU)
	{
		//MODBUS RTU
		strcpy(pCommandLine, GetScadaHomeDirectory());
		strcat(pCommandLine, "\\bin\\modbus_master.exe -d ");
		strcat(pCommandLine, ((Modbus_driver_Instance*)Parent)->Cfg.SerialDevice);
		strcat(pCommandLine, " -b ");
		strcat(pCommandLine, ((Modbus_driver_Instance*)Parent)->Cfg.Baud);
		strcat(pCommandLine, " -c ");
		strcat(pCommandLine, ((Modbus_driver_Instance*)Parent)->Cfg.DataBits);
		strcat(pCommandLine, " -e ");
		strcat(pCommandLine, ((Modbus_driver_Instance*)Parent)->Cfg.StopBit);
		strcat(pCommandLine, " -f ");
		strcat(pCommandLine, ((Modbus_driver_Instance*)Parent)->Cfg.Parity);
		strcat(pCommandLine, " -l ");
		strcat(pCommandLine, line_number);
		strcat(pCommandLine, " -t ");
		strcat(pCommandLine, polling_time);
	}
		
	strcpy(pWorkingDir, GetScadaHomeDirectory());
	strcat(pWorkingDir,"\\bin");
		
	if(StartProcess(pCommandLine, pWorkingDir))
	{
        fprintf(stderr, "Started process %s\n", pCommandLine);
	    fflush(stderr);
	}
	
    int rc = 0;

    fRequestHalt = false;

	while(1)
	{
        Sleep(1000);
        ///////////////Child process support/////////////////////////////////////////////////////
        if(fRequestHalt)
        {
            break;
        }

		restart_count++;

		if(restart_count%10 == 0) //Check every 10 seconds
		{
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
		}

        pipe_sends_cont++;

        if(pipe_sends_cont%2 == 0)
        {
	        rc = send_ack_to_child(4004, 33, pipe_name); //Send a packet every 2 seconds
        }
        /////////////////////////////////////////////////////////////////////////////
	}
	
	UnitFail("MODBUS driver stopped");

    Done = true;
}


bool Modbus_DriverThread::StartProcess(char* pCommandLine, char* pWorkingDir)
{ 
	STARTUPINFO startUpInfo = { sizeof(STARTUPINFO),NULL,"",NULL,0,0,0,0,0,0,0,STARTF_USESHOWWINDOW,0,0,NULL,0,0,0};  

	nIndex = 1;
	
	if(strlen(pCommandLine) > 4)
	{
		char pUserInterface[nBufferSize+1];

		//printf("%s\n", pCommandLine);
		
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

		memset(pProcInfo + nIndex, 0, sizeof(PROCESS_INFORMATION));

		if(CreateProcess(NULL,pCommandLine,NULL,NULL,TRUE,dwCreateFlg,NULL,strlen(pWorkingDir)==0?NULL:pWorkingDir,&startUpInfo,pProcInfo + nIndex))
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

/*
PROCESS_INFORMATION* Modbus_DriverThread::getProcInfo(void)
{
    return(pProcInfo);
}
*/

void Modbus_DriverThread::EndProcess(int nIndex)
{	
	if(pProcInfo[nIndex].hProcess)
	{
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

int Modbus_DriverThread::send_ack_to_child(int address, int data, char* pipeName)
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

int Modbus_DriverThread::pipe_put(char* pipe_name, char *buf, int len)
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

void Modbus_DriverThread::TerminateProtocol() // parent requests the thread close
{
	IT_IT("Modbus_DriverThread::TerminateProtocol");

    EndProcess(nIndex); //stop the child process

    fRequestHalt = true;
};
