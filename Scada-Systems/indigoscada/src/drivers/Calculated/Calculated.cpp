/*
* Calculated monitoring driver
*/
#include "Calculated.h"
#include <qtextstream.h>
#include "CalculatedInput.h"
#include "CalculatedConfigure.h"

/*
*Function:Calculated
*Inputs:parent object, object name
*Outputs:none
*Returns:none
*/
Calculated::Calculated(QObject *parent,const QString &name) : Driver(parent,name)
{
	connect (GetConfigureDb (),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (QueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the database
};
/*
*Function:~Calculated
*Inputs:none
*Outputs:none
*Returns:none
*/
Calculated::~Calculated()
{
	Stop(); // clean up
};
/*
*Function:UnitConfigure
*Inputs:parent widget, obejct name, receipe
*Outputs:none
*Returns:none
*/
void Calculated::UnitConfigure(QWidget *parent, const QString &name, const QString &receipe) // configure a unit
{
	// update the properties now
	CalculatedConfigure dlg(parent,name,receipe);
	dlg.exec();   
};
/*
*Function:SetTypeList
*Inputs:combo box, unit name
*Outputs:none
*Returns:none
*/
void Calculated::SetTypeList(QComboBox *pCombo, const QString &unitname) // set the type list for unit type
{
	pCombo->insertItem(TYPE_M_SP_NA_1);
	pCombo->insertItem(TYPE_M_DP_NA_1);
	pCombo->insertItem(TYPE_M_ME_NA_1);
	pCombo->insertItem(TYPE_M_ME_NB_1);
	pCombo->insertItem(TYPE_M_ME_NC_1);
	pCombo->insertItem(TYPE_M_IT_NA_1);
	pCombo->insertItem(TYPE_M_SP_TB_1);
	pCombo->insertItem(TYPE_M_DP_TB_1);
	pCombo->insertItem(TYPE_M_ME_TD_1);
	pCombo->insertItem(TYPE_M_ME_TE_1);
	pCombo->insertItem(TYPE_M_ME_TF_1);
	pCombo->insertItem(TYPE_M_IT_TB_1);
};
/*
*Function:GetInputList
*Inputs:sample type
*Outputs:list of types
*Returns:none
*/
void Calculated::GetInputList(const QString &type, QStringList &list,const QString &unit,const QString &name) // set the permitted input IDs
{
	list.clear();
};
/*
*Function:GetSpecificConfig
*Inputs:parent widget, spname, sp type
*Outputs:none
*Returns:specific config widget for sample point
*/
QWidget * Calculated::GetSpecificConfig(QWidget *parent, const QString &spname, const QString &sptype) //specific config for sample point of type
{
	return new CalculatedInput(parent,spname);
};
/*
*Function:GetTagList
*Inputs:sample point type
*Outputs:list of tags
*Returns:none
*/
void Calculated::GetTagList(const QString &type, QStringList &list,const QString &unit, const QString &) // returns the permitted tags for a given type for this unit
{
	list.clear();
	list << VALUE_TAG;
};
/*
*Function:CreateNewUnit
*Inputs:parent widget, unit name
*Outputs:none
*Returns:none
*/
void Calculated::CreateNewUnit(QWidget *parent, const QString &name) // create a new unit - quick configure
{
	int i = 1;

	QString n;
	n.sprintf("%02d",i);
	//
	QString spname = name+"point"+n;
	QString cmd = "insert into SAMPLE values('"+spname+"','calculated point "+ n + 
	"','"+name+"','"TYPE_M_ME_TF_1"','V',1,1,'"+n+"',0,0,0);" ;
	GetConfigureDb()->DoExec(0,cmd,0); // post it off
	//
	QStringList l;
	GetTagList(TYPE_M_ME_TF_1,l,"",""); 
	CreateSamplePoint(spname, l,"");

};
//
// 
Calculated *Calculated::pDriver; // only one instance should be created
/*
*Function:Start
*Inputs:none
*Outputs:none
*Returns:none
*/
void Calculated::Start() // start everything under this driver's control
{
	//
	// create instances for each unit - get the receipe qualifier
	// request each to start
	// form the database request transaction
	// post it to the database - the magic is in the response handler
	// get all enabled units
	// 
	// form up the list of enabled units
	//
	QString cmd = "select * from UNITS where UNITTYPE='Calculated' and NAME in(" + DriverInstance::FormUnitList()+ ");";
	GetConfigureDb()->DoExec(this,cmd,tListUnits);
	//
};
/*
*Function:Stop
*Inputs:none
*Outputs:none
*Returns:none
*/
void Calculated::Stop() // stop everything under this driver's control
{
	// ask each instance to stop 
	IDict::iterator i = Instances.begin();
	for(; i != Instances.end();i++)
	{
		(*i).second->Stop();
	};
	//
	// now delete them
	//     
	i = Instances.begin();
	for(; i != Instances.end();i++)
	{
		delete (*i).second;
	};
	//  
	Instances.clear();
};
/*
*Function:Command
*Inputs:target name, command
*Outputs:none
*Returns:none
*/
void Calculated::Command(const QString &name ,const QString &command)
{
	IT_IT("Calculated::Command");
	
	IDict::iterator i = Instances.find(name);
	if(!(i == Instances.end()))
	{
		(*i).second->Command(name, command); // pass on the command
	};
}; // process a command for a named unit 
/*
*Function:QueryResponse
*Inputs:client object, command, transaction id
*Outputs:none
*Returns:none
*/
void Calculated::QueryResponse (QObject *p, const QString &c, int id, QObject*caller)
{
	if(p != this) return; // is this for us
	switch(id)
	{
		case tListUnits:
		{
			int n = GetConfigureDb()->GetNumberResults();
			if(n > 0)
			{
				for(int i = 0; i < n; i++,GetConfigureDb()->FetchNext())
				{
					CalculatedInstance *p = new CalculatedInstance(this,GetConfigureDb()->GetString("NAME"));
					IDict::value_type pr(GetConfigureDb()->GetString("NAME"),p);
					Instances.insert(pr);
					p->Start(); // kick it off 
				};
			};
		};
		break;
		default:
		break;
	};
};
//
//
//
// *********************************************************************************
// Driver instance stuff
//
//
/*
*Function:
*Inputs:none
*Outputs:none
*Returns:none
*/
void CalculatedInstance::Start() // start everything under this driver's control
{
	//
	// Get the sample points and their expressions
	QString cmd =
	"select * from PROPS where SKEY='SAMPLEPROPS' and IKEY in (select NAME from SAMPLE where UNIT='" 
	+ Name + "' and ENABLED=1);";
	GetConfigureDb()->DoExec(this,cmd,tProperties);
	//
	// get the properties for this receipe  - fall back to default
	//
	QString pc = "select * from PROPS where SKEY='" + Name + 
	"' and (IKEY = '(default)' or IKEY='"+ GetReceipeName() + "') order by IKEY desc;"; 
	// get the properties SKEY = unit name IKEY = receipe name
	GetConfigureDb()->DoExec(this,pc,tUnitProperties);
	Countdown = 0;
	StartWait = 0;
};

extern "C"
{ 
	int numSamplePoints;
	void post_value(void);
	//char processed_name[35];
	//char processed_tag[20];
	//double processed_value;
	//unsigned char write_processed_value_to_driver;
	int processed_id;
	int exit_loop;

	double Calc(char *, int *);
};

/*
*Function: Stop
*Inputs:none
*Outputs:none
*Returns:none
*/
void CalculatedInstance::Stop() // stop everything under this driver's control
{
	exit_loop = 1;
	pTimer->stop();
}
/*
*Function:Command
*Inputs:none
*Outputs:none
*Returns:none
*/
void CalculatedInstance::Command(const QString &,const QString &) // process a command for a named unit 
{

}

/*
extern "C"
{ 
	DWORD this_;

	void post_value(void)
	{
		printf("processed_name = %s\n", processed_name);
		printf("processed_tag = %s\n", processed_tag);
		printf("processed_value = %lf\n", processed_value);

		((CalculatedInstance*)this_)->PostValue(processed_name, processed_tag, processed_value);
	}
};
*/

/////fine variabili e funzioni condivise con l'interprete EiC

//
// clock tick interval in milliseconds
// 
#define LN_TICK (100)

HANDLE script_thread;

//void WINAPI WorkerProc(void* pParam)  //se usi la CreateThread
void WorkerProc(void* pParam)
{
	//CalculatedInstance* p = (CalculatedInstance*)pParam;
	int err;
	DWORD exitcode = 0;

	//script.c si trova nella directory C:\scada\scripts
	
	Calc("script.c", &err); //start loop of interpreter

	//_endthread();

	//ExitThread(exitcode);

	//CloseHandle(script_thread);

	//Usa la exit() cosi' quando si ferma il loop dell'interpreter
	//va giu' il processo che usa la .dll, in questo caso monitor.exe
	ins_mutex_free(mut);
	//exit(exitcode); //this exits not only the thread but also the monitor.exe main thread
}

extern "C"
{ 
	DWORD this_;

	//This fuction is called inside interpreted thread
	void post_value(void)
	{
		char buf[sizeof(struct scada_point)];
		struct scada_point item_to_send;

		memset(&item_to_send,0x00, sizeof(struct scada_point));

		//printf("processed_name = %s\n", scada_db[processed_id].name);
		//printf("processed_tag = %s\n", scada_db[processed_id].tag);
		//printf("processed_value = %lf\n", scada_db[processed_id].current_value);

		strcpy(item_to_send.name, scada_db[processed_id].name);
		strcpy(item_to_send.tag, scada_db[processed_id].tag);
		item_to_send.current_value = scada_db[processed_id].current_value;
		item_to_send.write_to_driver = scada_db[processed_id].write_to_driver;
		
		//Send message///////////////////////////////////////////////////////////////////
		memcpy(buf, &item_to_send, sizeof(struct scada_point));

		struct scada_point* p_item;
		p_item = (struct scada_point*)buf;
		p_item->checksum = clearCrc((unsigned char *)buf, sizeof(struct scada_point));

		if(((CalculatedInstance*)this_)->p_fifo_script)
		{
			fifo_put(((CalculatedInstance*)this_)->p_fifo_script, buf, sizeof(struct scada_point));
		}
		//////////////////////////////////////////////////////////////////////////////
	}
};

#include <signal.h>

char* get_date_time()
{
	static char sz[128];
	time_t t = time(NULL);
	struct tm *ptm = localtime(&t);
	
	strftime(sz, sizeof(sz)-2, "%m/%d/%y %H:%M:%S", ptm);

	strcat(sz, "|");
	return sz;
}

void iec_call_exit_handler(int line, char* file, char* reason)
{
	FILE* fp;
	char program_path[_MAX_PATH];
	char log_file[_MAX_FNAME+_MAX_PATH];
	IT_IT("iec_call_exit_handler");

	program_path[0] = '\0';
#ifdef WIN32
	if(GetModuleFileName(NULL, program_path, _MAX_PATH))
	{
		*(strrchr(program_path, '\\')) = '\0';        // Strip \\filename.exe off path
		*(strrchr(program_path, '\\')) = '\0';        // Strip \\bin off path
    }
#elif __unix__
	if(getcwd(program_path, _MAX_PATH))
	{
		*(strrchr(program_path, '/')) = '\0';        // Strip \\filename.exe off path
		*(strrchr(program_path, '/')) = '\0';        // Strip \\bin off path
    }
#endif

	strcpy(log_file, program_path);

#ifdef WIN32
	strcat(log_file, "\\logs\\calculated.log");
#elif __unix__
	strcat(log_file, "/logs/calculated.log");	
#endif

	fp = fopen(log_file, "a");

	if(fp)
	{
		if(line && file && reason)
		{
			fprintf(fp, "PID:%d time:%s exit process at line: %d, file %s, reason:%s\n", GetCurrentProcessId, get_date_time(), line, file, reason);
		}
		else if(line && file)
		{
			fprintf(fp, "PID:%d time:%s exit process at line: %d, file %s\n", GetCurrentProcessId, get_date_time(), line, file);
		}
		else if(reason)
		{
			fprintf(fp, "PID:%d time:%s exit process for reason %s\n", GetCurrentProcessId, get_date_time(), reason);
		}

		fflush(fp);
		fclose(fp);
	}

	//raise(SIGABRT);   //raise abort signal which in turn starts automatically a separete thread and call exit SignalHandler
	ExitProcess(0);

	IT_EXIT;
}


/*
*Function: QueryResponse
*Inputs:none
*Outputs:none
*Returns:none
*/
void CalculatedInstance::QueryResponse (QObject *p, const QString &c, int id, QObject*caller) // handles database responses
{
	if(p != this) return;
	switch(id)
	{
		case tProperties:
		{
			//
			// build a table of sample point names and expressions
			ExprTable.clear();
			int n = GetConfigureDb()->GetNumberResults();
			if(n > 0)
			{
				for(int i = 0; i < n; i++,GetConfigureDb()->FetchNext())
				{
					ExpItem e;
					e.Sample = GetConfigureDb()->GetString("IKEY");
					e.Expr = 	GetConfigureDb()->GetString("DVAL");
					ExprTable.insert(ExprTable.end(),e);
				};
			};
		};
		break;
		case tUnitProperties: // properties for the unit / receipe 
		{
			if(GetConfigureDb()->GetNumberResults() > 0)
			{
				QString s = GetConfigureDb()->GetString("DVAL");
				QTextIStream is(&s);
				is >> Interval;
				Countdown = Interval;
				pTimer->start(LN_TICK); // start the timer
			}
			else
			{
				Interval = 60;
			};
			
			GetCurrentDb()->DoExec(this,"select * from TAGS_DB;",tTagsCurrent);
		};
		break;
		case tTagsCurrent:
		{	
			// get the current tag name and sample point name
			
			if(GetCurrentDb()->GetNumberResults() > 0)
			{
				nSamplePoints = GetCurrentDb()->GetNumberResults();

				numSamplePoints = nSamplePoints;

				for(int i = 0; i < nSamplePoints; i++, GetCurrentDb()->FetchNext())
				{
					// 
					// update the current values with the values from the last time we started
					//
					SamplePointDictWrap &d = Results::GetEnabledPoints();
					SamplePointDictWrap::iterator k =  d.find(GetCurrentDb()->GetString("NAME"));

					if(!(k == d.end()))
					{
						SamplePoint::TagDict::iterator j = (*k).second.Tags.find(GetCurrentDb()->GetString("TAGNAME"));
						if(!(j == (*k).second.Tags.end()))
						{
							if((*j).second.enabled)
							{
								strcpy(scada_db[i].name, (const char*)GetCurrentDb()->GetString("NAME"));
								strcpy(scada_db[i].tag, (const char*)GetCurrentDb()->GetString("TAGNAME"));
								scada_db[i].current_value = GetCurrentDb()->GetDouble("VAL");
							}
						}  
					}
				}
			}
			
			//mut = ins_mutex_new();

			this_ = (DWORD)this;
			
			DWORD threadid;
			script_thread = CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(WorkerProc), WorkerProc, 0, &threadid);

			// start a worker thread to run the script loop
			
			/*
			if(_beginthread(WorkerProc, 0, 0)==-1)
			{
				long nError = GetLastError();
				
				fprintf(stderr, "_beginthread failed, error code = %d", nError);
				fflush(stderr);
			}
			*/
						
			//const size_t max_fifo_queue_size = 4*1024;
			const size_t max_fifo_queue_size = 4*65536;
			
			//Init thread shared fifos
			p_fifo_script = fifo_open("fifo_script", max_fifo_queue_size, iec_call_exit_handler);
		}
		break;
		case tUnit:
		{
			if(GetConfigureDb()->GetNumberResults() > 0)
			{
				/*
				//Generazione del COMANDO OPC/////////////////////////////////////////////
				unsigned char parametri[sizeof(dispatcher_extra_params)];
				dispatcher_extra_params* params = (dispatcher_extra_params *) parametri;

				memset(parametri, 0, sizeof(dispatcher_extra_params));
							
				params->value = p_item->current_value;

				QString unit_name = GetConfigureDb()->GetString("UNIT");
				
				strcpy(params->string1, (const char*)unit_name); //driver instance

				if(strlen(p_item->name) < 30)
				{
					strcpy(params->string2, p_item->name);
				}
				else
				{
					printf("reduce OPC point name to 30 characters\n");
				}

				//NEXT instruction NOT used HERE
				//strcpy(params->string3, (const char *)Value->text()); //For writing the string
								
				GetDispatcher()->DoExec(NotificationEvent::CMD_SEND_COMMAND_TO_UNIT, (char *)parametri, sizeof(dispatcher_extra_params));  //broadcast to all tcp clients

				*/
			}
		} 
		break;
		default:
		break;
	};
};
/*
*Function: Tick
*Inputs:none
*Outputs:none
*Returns:none
*/

void CalculatedInstance::Tick()
{
/*
	--Countdown;

	if(Countdown <1)
	{
		Countdown = Interval; // reset the count down

		if(++StartWait > 2) // we wait two sample intervals to allow all dependent sample points to get values
		{
			for(unsigned i = 0; i < ExprTable.size(); i++)
			{
				int err = 0;
				double res;
				// evaluate the expression ExprTable[i]

				res = Calc((char *)((const char *)ExprTable[i].Expr),&err);

				// if OK then post it
				if(!err)
				{
					PostValue(ExprTable[i].Sample,VALUE_TAG, res);
				}

				if(fTrace)
				{
					Trace(tr("Evaluated:") + ExprTable[i].Expr + ":" +
					tr("Result:") + QString::number(res) + ":" 
					+tr("Error:") + QString::number(err));
				}
			};
		};
	};
*/
	//This code runs inside main monitor.exe thread

	unsigned char buf[sizeof(struct scada_point)];
    int len;
	const unsigned wait_limit_ms = 1;
	struct scada_point* p_item;

	if(p_fifo_script)
	{
		for(int i = 0; (len = fifo_get(p_fifo_script, (char*)buf, sizeof(struct scada_point), wait_limit_ms)) >= 0; i += 1)	
		{ 
			p_item = (struct scada_point*)buf;
				
			//printf("Receiving %d th message \n", p_item->msg_id);

			for (int j = 0; j < len; j++) 
			{ 
				//assert((unsigned char)buf[i] == len);
				unsigned char c = *((unsigned char*)buf + j);
				//printf("rx <--- 0x%02x-\n", c);
				//fprintf(fp,"rx <--- 0x%02x-\n", c);
				//fflush(fp);

				//IT_COMMENT1("rx <--- 0x%02x-\n", c);
			}

			unsigned char rc = clearCrc((unsigned char *)buf, sizeof(struct scada_point));
			if(rc != 0)
			{
				//ExitProcess(1);
			}
			
			if(p_item->write_to_driver)
			{
				//QString cmd = "select UNIT from SAMPLE where NAME='"+ QString(p_item->name) +"';";
				//GetConfigureDb()->DoExec(this,cmd,tUnit); // kick it off

				//Generazione del COMANDO OPC/////////////////////////////////////////////
				unsigned char parametri[sizeof(dispatcher_extra_params)];
				dispatcher_extra_params* params = (dispatcher_extra_params *) parametri;

				memset(parametri, 0, sizeof(dispatcher_extra_params));
							
				params->value = p_item->current_value;
				
				strcpy(params->string1, "OPC"); //driver instance

				if(strlen(p_item->name) < 30)
				{
					strcpy(params->string2, p_item->name);
				}
				else
				{
					printf("reduce OPC point name to 30 characters\n");
				}

				//NEXT instruction NOT used HERE
				//strcpy(params->string3, (const char *)Value->text()); //For writing the string

				struct cp56time2a actual_time;
				get_utc_host_time(&actual_time);
				params->time_stamp = actual_time;
								
				GetDispatcher()->DoExec(NotificationEvent::CMD_SEND_COMMAND_TO_UNIT, (char *)parametri, sizeof(dispatcher_extra_params));  //broadcast to all tcp clients
			}
			else
			{
				//printf("p_item->name = %s\n", p_item->name);
				//printf("p_item->tag = %s\n", p_item->tag);
				//printf("p_item->current_value = %lf\n", p_item->current_value);
				
				PostValue(p_item->name, p_item->tag, p_item->current_value); //Post the value directly in memory database
			}
		}
	}
};
// ********************************************************************************************************************************
/*
*Function:GetDriverEntry
*Inputs:parent object
*Outputs:none
*Returns:driver interface 
*/
extern "C"
{ 
	#ifdef WIN32
	CALCULATED_API Driver *  _cdecl _GetDriverEntry(QObject *parent); 
	CALCULATED_API void _cdecl _Unload();
	#endif

	Driver * _GetDriverEntry(QObject *parent) 
	{
		if(!Calculated::pDriver )
		{
			Calculated::pDriver = new Calculated(parent,"Calculated");
		};
		return Calculated::pDriver;
	};
	/*
	*Function: _Unload
	*clean up before DLL unload. and QObjects must be deleted or we get a prang
	*Inputs:none
	*Outputs:none
	*Returns:none
	*/
	void _Unload()
	{
		if(Calculated::pDriver) delete Calculated::pDriver;
		Calculated::pDriver = 0;
	};
};
			
#include <time.h>
#include <sys/timeb.h>

void CalculatedInstance::get_utc_host_time(struct cp56time2a* time)
{
	struct timeb tb;
	struct tm	*ptm;
		
	IT_IT("get_utc_host_time");

    ftime (&tb);
	ptm = gmtime(&tb.time);
		
	time->hour = ptm->tm_hour;					//<0..23>
	time->min = ptm->tm_min;					//<0..59>
	time->msec = ptm->tm_sec*1000 + tb.millitm; //<0..59999>
	time->mday = ptm->tm_mday; //<1..31>
	time->wday = (ptm->tm_wday == 0) ? ptm->tm_wday + 7 : ptm->tm_wday; //<1..7>
	time->month = ptm->tm_mon + 1; //<1..12>
	time->year = ptm->tm_year - 100; //<0..99>
	time->iv = 0; //<0..1> Invalid: <0> is valid, <1> is invalid
	time->su = (u_char)tb.dstflag; //<0..1> SUmmer time: <0> is standard time, <1> is summer time

	IT_EXIT;
    return;
}