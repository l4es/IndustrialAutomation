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
#ifndef RFC1006DRIVERTHREAD
#define RFC1006DRIVERTHREAD

#include "general_defines.h"
#include "IndentedTrace.h"
#include "rfc1006driver_instance.h"

const int nMaxProcCount = 127;
const int nBufferSize = 500;

class RFC1006DRIVERDRV Rfc1006DriverThread : public DriverThread 
{
	public:

    ///////////////Child process support/////////////
    PROCESS_INFORMATION pProcInfo[nMaxProcCount];
    char pCommandLine[nBufferSize+1];
    char pWorkingDir[nBufferSize+1];
    int nIndex;
	char pipe_name[150];
    HANDLE h_pipe;
    int pipe_sends_cont;
	int restart_count; //apa++ 07-05-2012
    int msg_id;
    int instanceID;
    bool Done;
    /////////////////////////////////////////////////

	Rfc1006DriverThread(DriverInstance *parent) : 
		DriverThread(parent),h_pipe(NULL), nIndex(1), pipe_sends_cont(0),
        msg_id(0), Done(false), restart_count(0)
	{ 
		IT_IT("Rfc1006DriverThread::Rfc1006DriverThread");

		instanceID = ((Rfc1006driver_Instance*)Parent)->instanceID;

        ///////////////Child process support/////////////
        for(int i = 0; i < nMaxProcCount; i++)
        {
            memset(pProcInfo + i, 0x00, sizeof(PROCESS_INFORMATION));
        }
        /////////////////////////////////////////////////
	};
	
	~Rfc1006DriverThread()
	{
		IT_IT("Rfc1006DriverThread::~Rfc1006DriverThread");
	}

    void TerminateProtocol(); // parent requests the thread close
    //PROCESS_INFORMATION* getProcInfo(void);

	protected:
	void run(); // thread main routine
    ///////////////Child process support/////////////////////////////////////////////////////
    bool StartProcess(char* pCommandLine, char* pWorkingDir);
    void EndProcess(int nIndex);
    int send_ack_to_child(int address, int data, char* pipeName);
    int pipe_put(char* pipe_name, char *buf, int len);
	/////////////////////////////////////////////////////////////////////////////////////////
};

#endif //RFC1006DRIVERTHREAD