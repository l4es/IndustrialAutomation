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
#include "sockio.h"
#include "cli_dispatcher.h"

///////////////////////////////////Dispatcher_Thread/////////////////////////////////////////////////////

Dispatcher_Thread::Dispatcher_Thread(Dispatcher *parent,const QString &host, const QString &user, const QString &password) : 
Done(true),	Trigger(false),dispatcher_handle(0),fFail(false),Parent(parent),fExit(0),
server_is_connected(false),Host(host),User(user),Password(password),
retry_to_connect_to_server_pending(false),is_started_dispatcher_thread_wait(false)
{
	IT_IT("Dispatcher_Thread::Dispatcher_Thread");
	
	m_hevtSinc.open();
	m_hevtEnd.open();

	// start a worker thread to connect to server
	connThread.create((dbThread::thread_proc_t)ConnectorToDispatcherServer, this);
}

Dispatcher_Thread::~Dispatcher_Thread()
{
	IT_IT("Dispatcher_Thread::~Dispatcher_Thread");
	
	//Parent = 0; 
	usleep(1000); // task switch 
	fExit = true;
	m_hevtSinc.signal();
	
	if(server_is_connected && is_started_dispatcher_thread_wait)
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
		
	if(dispatcher_handle >= 0 && server_is_connected && (!fFail))
	{
		if ((rc = cli_close_dispatcher(dispatcher_handle)) != cli_ok) 
		{
			IT_COMMENT1("cli_close_dispatcher failed with code %d", rc);
		}
	}

	m_hevtSinc.close();
	m_hevtEnd.close();

}

void Dispatcher_Thread::ConnectToServer()
{
	IT_IT("Dispatcher_Thread::ConnectToServer");

	if (Host.isEmpty()) //29-11-09
	{
		fFail = true;
		return;
	}

	retry_to_connect_to_server_pending = true;

	dispatcher_handle = cli_open_dispatcher((const char *)Host, 10, 1, (const char *)User, (const char *)Password, 0);

	if (dispatcher_handle < 0) {

		IT_COMMENT1("cli_open_dispatcher failed with code %d", dispatcher_handle);
		fFail = true;
	}
	else
	{
		server_is_connected = true;
		IT_COMMENT1("cli_open_dispatcher SUCCESS with handle %d", dispatcher_handle);
	}

	retry_to_connect_to_server_pending = false;
}


bool Dispatcher_Thread::SetNotif(NotificationData* n)
{	
	IT_IT("Dispatcher_Thread::SetNotif");
	
	if(Done) 
	{
		notif = n;
		Trigger = true; // go 
		Done = false; // mark as not complete
	
		m_hevtSinc.signal();		

		return true;
	}
	return false;
}	

/*
*Function:run
*runs the thread
*Inputs:none
*Outputs:none
*Returns:none
*/

void Dispatcher_Thread::run()
{	
	IT_IT("Dispatcher_Thread::run");
	
	IT_COMMENT("Thread Running");
	
	while(true)
	{
		
		IT_COMMENT("Waiting....");
		
		{
			dbCriticalSection cs(mutex); //enter critical section
			m_hevtSinc.wait(mutex);
			if(!fExit)
			{
				m_hevtSinc.reset();
			}
		}//exit critical section

		if(fExit)
		{
			IT_COMMENT("Dispatcher_Thread::run is exiting ....");
			m_hevtEnd.signal();
			return; //terminate the thread
		}
		
		IT_COMMENT("Dispatcher_Thread Past Wait Flag");
		if(Trigger)
		{
			Trigger = false;
								
			IT_COMMENT2("Notification: %d, data %s", notif->NotificationCode, notif->Data);
						
			if((rc = cli_write_notification(dispatcher_handle, notif->NotificationCode, notif->Data, notif->Datalength)) != cli_ok)
			{
				IT_COMMENT1("cli_write_notification failed with code %d", rc);
				fFail = true;
			}
			///////////////////////////////////////////////////////////////
			//
			IT_COMMENT("Write notification Done");

			// tell the parent that the transaction has completed
			Done = true;
			
			NotificationEvent * p = new NotificationEvent(this, END_WRITE_NOTIF);
			p->SetPointer((void*)notif); 
			if(Parent) QThread::postEvent(Parent, p);  // parent must delete
			//
		}
		#ifdef WIN32
		else
		{
			QThread::msleep(10);		
		}	
		#endif
	}
}

///////////////////////////////////Dispatcher_Thread_Wait/////////////////////////////////////////////////////

Dispatcher_Thread_Wait::Dispatcher_Thread_Wait(Dispatcher *parent) : 
Done(true),	Trigger(false),fFail(false),Parent(parent),fExit(0)
{
	IT_IT("Dispatcher_Thread_Wait::Dispatcher_Thread_Wait");
	
	m_hevtSinc.open();
	m_hevtEnd.open();
	m_hevtWait.open();
}

Dispatcher_Thread_Wait::~Dispatcher_Thread_Wait()
{
	IT_IT("Dispatcher_Thread_Wait::~Dispatcher_Thread_Wait");
	
	//Parent = 0; 
	usleep(1000); // task switch 
	fExit = true;
	m_hevtSinc.signal();
	m_hevtWait.signal();
	
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

	m_hevtSinc.close();
	m_hevtEnd.close();
	m_hevtWait.close();
}



bool Dispatcher_Thread_Wait::SetNotif(NotificationData* n)
{	
	IT_IT("Dispatcher_Thread_Wait::SetNotif");
	
	if(Done) 
	{
		notif = n;
		Trigger = true; // go 
		Done = false; // mark as not complete
	
		m_hevtWait.reset();
		m_hevtSinc.signal();		

		return true;
	}
	return false;
}	

/*
*Function:run
*runs the thread
*Inputs:none
*Outputs:none
*Returns:none
*/

void Dispatcher_Thread_Wait::run()
{	
	IT_IT("Dispatcher_Thread_Wait::run");
	
	IT_COMMENT("Thread Running");
	
	while(true)
	{
		
		IT_COMMENT("Waiting....");
		
		{
			dbCriticalSection cs(mutex); //enter critical section
			m_hevtSinc.wait(mutex);
			if(!fExit)
			{
				m_hevtSinc.reset();
			}
		}//exit critical section

		if(fExit)
		{
			IT_COMMENT("Dispatcher_Thread_Wait::run is exiting ....");
			m_hevtEnd.signal();
			return; //terminate the thread
		}
		
		IT_COMMENT("Dispatcher_Thread_Wait Past Wait Flag");
		if(Trigger)
		{
			Trigger = false;

			{
				dbCriticalSection cs(mutex); //enter critical section
				m_hevtWait.wait(mutex, notif->Delay);
			
			}//exit critical section

			///////////////////////////////////////////////////////////////
			//
			IT_COMMENT("Wait Done");

			// tell the parent that the transaction has completed
			Done = true;
			
			NotificationEvent * p = new NotificationEvent(this, END_WAIT_NOTIF);
			p->SetPointer((void*)notif); 
			if(Parent) QThread::postEvent(Parent, p);  // parent must delete
			//
		}
		#ifdef WIN32
		else
		{
			QThread::msleep(10);		
		}	
		#endif
	}
}



/////////////////////Dispatcher implementation/////////////////////////////////////////////

/*
*Function: notificationEvent
*handles asynchronous events from a child notification thread
*Inputs:
*Outputs:none
*Returns:none
*/
void Dispatcher::notificationEvent(NotificationEvent *p)
{
	IT_IT("Dispatcher::notificationEvent");
	
	switch(p->opcode())
	{
		case END_READ_NOTIF:
		{
			emit ReceivedNotify(p->data(), (const char*)p->text()); 
		}
		break;
		case END_WRITE_NOTIF:
		{
			if(WriteQueue.count() > 0 && pConnect)
			{
				NotificationData *ntf = (NotificationData *)(p->Pointer()); 

				assert(WriteQueue.head() == ntf);

				WriteQueue.remove(); // delete the write notification: fine ciclo di vita istanza NotificationData
				pending_writes--;

				IT_COMMENT1("PENDING WRITES %d", pending_writes);

				if((WriteQueue.count() > 0) && (pConnect->Ok()))
				{
					pConnect->SetNotif(WriteQueue.head()); // send the next notification
				}
			}
		}
		break;
		case END_WAIT_NOTIF:
		{
			if(WaitQueue.count() > 0 && pConnectWait && pConnect)
			{
				NotificationData *ntf = (NotificationData *)(p->Pointer()); 

				assert(WaitQueue.head() == ntf);

				NotificationData* ntfd = new NotificationData(ntf->NotificationCode, ntf->Data, ntf->Datalength); //inizio ciclo di vita di istanza NotificationData

				WaitQueue.remove(); // delete the write notification: fine ciclo di vita istanza NotificationData
				pending_waits--;

				IT_COMMENT1("PENDING WAITS %d", pending_waits);

				if((WaitQueue.count() > 0) && (pConnectWait->Ok()))
				{
					pConnectWait->SetNotif(WaitQueue.head()); // send the next notification
				}

				if((!WriteQueue.count()) && (pConnect->Ok())) 
				{
					pConnect->SetNotif(ntfd);
				}
				
				pending_writes++;

				IT_COMMENT1("PENDING WRITES %d", pending_writes);

				WriteQueue.enqueue(ntfd); 
			}
		}
		break;
		default:
		break;
	}
}


/*
*Function:event
*event handler
*Inputs:none
*Outputs:none
*Returns:none
*/
bool Dispatcher::event(QEvent *e)
{
	IT_IT("Dispatcher::event");
		
	if(e->type() == QEvent::User)
	{
		notificationEvent((NotificationEvent *)e);
		return true;
	}
	return QObject::event(e);
};

bool Dispatcher::DoExec(int not, const char *data, int datalength, time_t delay)
{
	IT_IT("Dispatcher::DoExec");
	
	bool res = false;
	if(pConnect && pConnectWait)
	{
		NotificationData* ntfd = new NotificationData(not, data, datalength, delay); //inizio ciclo di vita di istanza NotificationData

		IT_COMMENT2("New notification: %d, data %s", not, data);

		if(!delay)
		{
			if(!WriteQueue.count()) 
			{
				pConnect->SetNotif(ntfd);
			}
			
			pending_writes++;

			IT_COMMENT1("PENDING WRITES %d", pending_writes);

			WriteQueue.enqueue(ntfd); 
		}
		else
		{
			if(!WaitQueue.count()) 
			{
				pConnectWait->SetNotif(ntfd);
			}
			
			pending_waits++;

			IT_COMMENT1("PENDING WAITS %d", pending_waits);

			WaitQueue.enqueue(ntfd); 
		}
	}
	return res;
};

bool Dispatcher::Connect(const QString &host, const QString &user, const QString &password)
{
	IT_IT("Dispatcher::Connect");
	
	if(pConnect) delete pConnect;
	if(pConnectWait) delete pConnectWait;
	if(pConnectReader) delete pConnectReader;
		
	pConnect = new Dispatcher_Thread(this,host, user, password); 

	if(pConnect->Ok()) 
	{
		//Go if pConnect->server_is_connected or wait 200 ms
		unsigned int count = 0;
		//for(;;)
		{
			count++;	
			Sleep(100); //era 500
			if(pConnect->server_is_connected)
			{
				IT_COMMENT("Connesso al dispatcher");

				pConnect->start();

				pConnectWait = new Dispatcher_Thread_Wait(this); 

				//pConnect->SetFlagThreadWaitStarted();

				//the Dispatcher_Reader_Thread must be created after the Dispatcher_Thread

				pConnectReader = new Dispatcher_Reader_Thread(this); 

				if(pConnectWait->Ok()) 
				{
					pConnectWait->start();
					pConnect->SetFlagThreadWaitStarted();
				}

				if(pConnectReader->Ok()) 
				{
					pConnectReader->start();
				}

				return (pConnect->Ok() && pConnectReader->Ok() && pConnectWait->Ok());
			}
			else
			{
			   IT_COMMENT("Non connesso al dispatcher");

			   //if(count > 20)
			   //{
			//	   break;
			   //}
			}
		}
	}

	return pConnect->Ok();
};

void Dispatcher::close()
{
	IT_IT("Dispatcher::close");

	Disconnect();
}

bool  Dispatcher::Disconnect()              // disconnect from dispatcher
{
	IT_IT("Dispatcher::Disconnect");

	bool res = true;

	//Warning: do not reverst STEP 1 with STEP 2
	//STEP 1
	if(pConnect) delete pConnect; //Prima devi chiudere la connessione col server
	pConnect = NULL;

	//STEP 1.1
	if(pConnectWait) delete pConnectWait;
	pConnectWait = NULL;

	
	//STEP 2
	if(pConnectReader) delete pConnectReader; // poi fai una s->sock->shutdown();
	pConnectReader = NULL;

	WriteQueue.clear();

	WaitQueue.clear();

	return res;
};

bool Dispatcher::Ok()
{
	IT_IT("Dispatcher::Ok");

	return (pConnect && pConnectWait && pConnectReader) ? (pConnect->Ok() && pConnectReader->Ok() && pConnectWait->Ok()) : false;
}

bool Dispatcher::IsInRetry()
{
	IT_IT("Dispatcher::IsInRetry");

	return pConnect ? (pConnect->IsInRetry()) : false;
}

bool Dispatcher::IsConnected()
{
	IT_IT("Dispatcher::IsConnected");

	return pConnect ? (pConnect->IsConnected()) : false;
}

/////////////////////////////////////TheDispatcher on primary host///////////////////////////////////////

static Dispatcher *TheDispatcher = NULL;

Dispatcher * GetDispatcher()
{
	return TheDispatcher;
};

void SetDispatcher(Dispatcher *d)
{
	TheDispatcher = d;
}

//
// return true on success else false
//
bool OpenDispatcherConnection()	// open the connections with the dispatcher server
{
	IT_IT("OpenDispatcherConnection");

	bool res = true;  
	QString DispatcherUserName;
	QString DispatcherPassword;
		
	Dispatcher* d = new Dispatcher(); 
	SetDispatcher(d);

	QString dsp_ini_file = GetScadaHomeDirectory() + "\\project\\dispatcher.ini";
	Inifile dsp_iniFile((const char*)dsp_ini_file);

	if( dsp_iniFile.find("user","dispatcherserver") &&
		dsp_iniFile.find("password","dispatcherserver") )
	{
		DispatcherUserName = dsp_iniFile.find("user","dispatcherserver");
		DispatcherPassword = dsp_iniFile.find("password","dispatcherserver");
	}
	else
	{
		res = false;
	}

	//open connection with dispatcher
	if( dsp_iniFile.find("address","dispatcherserver") &&
		dsp_iniFile.find("port","dispatcherserver") )
	{
		QString host = QString(dsp_iniFile.find("address","dispatcherserver")) + ":" + QString(dsp_iniFile.find("port","dispatcherserver"));
		res &= GetDispatcher()->Connect(host, DispatcherUserName, DispatcherPassword);
	}
	else
	{
		res = false;	
	}

	if(res == false)
	{
		if(d)
		{
			d->Disconnect();
			delete d;
		}

		SetDispatcher(NULL);
	}

	return res;
};

//
// return true on success else false
//
bool ConnectToDispatcher()
{
	IT_IT("ConnectToDispatcher");

	bool res = true;  
	QString DispatcherUserName;
	QString DispatcherPassword;

	QString dsp_ini_file = GetScadaHomeDirectory() + "\\project\\dispatcher.ini";
	Inifile dsp_iniFile((const char*)dsp_ini_file);

	if( dsp_iniFile.find("user","dispatcherserver") &&
		dsp_iniFile.find("password","dispatcherserver") )
	{
		DispatcherUserName = dsp_iniFile.find("user","dispatcherserver");
		DispatcherPassword = dsp_iniFile.find("password","dispatcherserver");
	}
	else
	{
		res = false;
	}

	//open connection with dispatcher
	if( dsp_iniFile.find("address","dispatcherserver") &&
		dsp_iniFile.find("port","dispatcherserver") )
	{
		QString host = QString(dsp_iniFile.find("address","dispatcherserver")) + ":" + QString(dsp_iniFile.find("port","dispatcherserver"));
		res &= GetDispatcher()->Connect(host, DispatcherUserName, DispatcherPassword);
	}
	else
	{
		res = false;	
	}

	return res;
};
//
//
//
void CloseDispatcherConnection()
{
	IT_IT("CloseDispatcherConnection");
	
	GetDispatcher()->close();
	Dispatcher* d = GetDispatcher();
	delete d;
	TheDispatcher = NULL;
};
//
void DisconnectFromDispatcher()
{
	IT_IT("DisconnectFromDispatcher");
	
	GetDispatcher()->close();
};
/////////////////////////////TheSpareDispatcher on Spare host////////////////////////////////////////////////////////////

static Dispatcher *TheSpareDispatcher = NULL;

Dispatcher * GetSpareDispatcher()
{
	return TheSpareDispatcher;
};

void SetSpareDispatcher(Dispatcher *d)
{
	TheSpareDispatcher = d;
}

//
// return true on success else false
//
bool OpenSpareDispatcherConnection()	// open the connections with the dispatcher server
{
	IT_IT("OpenSpareDispatcherConnection");

	bool res = true;
	QString spare_val;

	QString scada_ini_file = GetScadaHomeDirectory() + "\\project\\scada.ini";

	Inifile scada_iniFile((const char*)scada_ini_file);

	if(scada_iniFile.find("spare","redundancy"))
	{
		spare_val = scada_iniFile.find("spare","redundancy");
	}

	if(atoi((const char*)spare_val) == 1)
	{
 		QString DispatcherUserName;
		QString DispatcherPassword;
			
		Dispatcher* d = new Dispatcher(); 
		SetSpareDispatcher(d);

		QString dsp_ini_file = GetScadaHomeDirectory() + "\\project\\spare_dispatcher.ini";
		Inifile dsp_iniFile((const char*)dsp_ini_file);

		if( dsp_iniFile.find("user","dispatcherserver") &&
			dsp_iniFile.find("password","dispatcherserver") )
		{
			DispatcherUserName = dsp_iniFile.find("user","dispatcherserver");
			DispatcherPassword = dsp_iniFile.find("password","dispatcherserver");
		}
		else
		{
			res = false;
		}

		//open connection with dispatcher
		if( dsp_iniFile.find("address","dispatcherserver") &&
			dsp_iniFile.find("port","dispatcherserver") )
		{
			QString host = QString(dsp_iniFile.find("address","dispatcherserver")) + ":" + QString(dsp_iniFile.find("port","dispatcherserver"));
			res &= GetSpareDispatcher()->Connect(host, DispatcherUserName, DispatcherPassword);
		}
		else
		{
			res = false;	
		}

		if(res == false)
		{
			if(d)
			{
				d->Disconnect();
				delete d;
			}

			SetSpareDispatcher(NULL);
		}
	}
	else
	{
		res = false;
	}

	return res;
};

//
// return true on success else false
//
bool ConnectToSpareDispatcher()
{
	IT_IT("ConnectToSpareDispatcher");

	bool res = true;  
	QString DispatcherUserName;
	QString DispatcherPassword;

	QString dsp_ini_file = GetScadaHomeDirectory() + "\\project\\spare_dispatcher.ini";
	Inifile dsp_iniFile((const char*)dsp_ini_file);

	if( dsp_iniFile.find("user","dispatcherserver") &&
		dsp_iniFile.find("password","dispatcherserver") )
	{
		DispatcherUserName = dsp_iniFile.find("user","dispatcherserver");
		DispatcherPassword = dsp_iniFile.find("password","dispatcherserver");
	}
	else
	{
		res = false;
	}

	//open connection with dispatcher
	if( dsp_iniFile.find("address","dispatcherserver") &&
		dsp_iniFile.find("port","dispatcherserver") )
	{
		QString host = QString(dsp_iniFile.find("address","dispatcherserver")) + ":" + QString(dsp_iniFile.find("port","dispatcherserver"));
		res &= GetSpareDispatcher()->Connect(host, DispatcherUserName, DispatcherPassword);
	}
	else
	{
		res = false;	
	}

	return res;
};
//
//
//
void CloseSpareDispatcherConnection()
{
	IT_IT("CloseSpareDispatcherConnection");
	
	GetSpareDispatcher()->close();
	Dispatcher* d = GetSpareDispatcher();
	delete d;
	TheSpareDispatcher = NULL;
};
//
void DisconnectFromSpareDispatcher()
{
	IT_IT("DisconnectFromSpareDispatcher");
	
	GetSpareDispatcher()->close();
};
