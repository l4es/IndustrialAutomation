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

#include "dispatch.h"
#include "stdtp.h"
#include "sockio.h"
#include "sync.h"
#include "cli_dispatcher.h"
#include "cliproto_dispatcher.h"


///////////////////////////////////Dispatcher_Reader_Thread/////////////////////////////////////////////////////

Dispatcher_Reader_Thread::Dispatcher_Reader_Thread(Dispatcher *parent) : 
Done(true),	Trigger(false),dispatcher_handle(0),fFail(false),Parent(parent),fExit(0)
{
	IT_IT("Dispatcher_Reader_Thread::Dispatcher_Reader_Thread");
	
	m_hevtEnd.open();
	
	dispatcher_handle = Parent->pConnect->GetHandle();

	if (dispatcher_handle < 0) 
	{
		fFail = true;
	}
	else
	{
		dsp_connection = true;
	}
}

Dispatcher_Reader_Thread::~Dispatcher_Reader_Thread()
{
	IT_IT("Dispatcher_Reader_Thread::~Dispatcher_Reader_Thread");
	
	usleep(1000); // task switch 

	if(dispatcher_handle >= 0)
	{
		session_desc_dispatcher* s = cli_get_session_desc(dispatcher_handle);
	
		fExit = true;

		if (s != NULL) 
		{
			s->sock->shutdown();
		}

		if(dsp_connection)
		{
			dbCriticalSection cs(mutex); //enter critical section
			m_hevtEnd.wait(mutex, 5);
		}//exit critical section
		//
		if(running())
		{
			while(!finished());
			Sleep(50);
		}
	}
	else
	{
		fExit = true;
	}
		
	m_hevtEnd.close();
}


/*
*Function:run
*runs the thread
*Inputs:none
*Outputs:none
*Returns:none
*/

void Dispatcher_Reader_Thread::run()
{	
	IT_IT("Dispatcher_Reader_Thread::run");
	
	IT_COMMENT("Thread Running");

	cli_request_dispatcher not;
	int4 response = cli_ok;

	session_desc_dispatcher* s = cli_get_session_desc(dispatcher_handle);
	if (s == NULL) 
	{
		IT_COMMENT("Dispatcher_Reader_Thread::run is exiting ....");
		m_hevtEnd.signal();
		return; //finish the thread
    }
	
	while (!fExit && s->sock->read(&not, sizeof not)) 
	{
		if(fExit) break;

		IT_COMMENT("Pass read.....");

		not.unpack();
		int length = not.length - sizeof(not);
		dbSmallBuffer<char> msg(length);
		if (length > 0) 
		{
			if (!s->sock->read(msg, length)) 
			{
				response = cli_network_error;
				fFail = true;
				break;
			}
		}
		
		if(Parent) QThread::postEvent(Parent, new NotificationEvent(this, END_READ_NOTIF, not.cmd, (const char*)msg, length));  // parent must delete

		IT_COMMENT("Waiting next read.....");
	}

	
	IT_COMMENT("Dispatcher_Reader_Thread::run is exiting ....");
	m_hevtEnd.signal();
	return; //terminate the thread
}



