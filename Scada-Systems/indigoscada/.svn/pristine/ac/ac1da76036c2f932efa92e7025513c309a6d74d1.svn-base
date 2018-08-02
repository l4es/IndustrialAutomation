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

/*
*Header For: driver 
*Purpose:
*/

#include "driver.h"
#include "sptypes.h"
#include "results.h"
#include "historicdb.h"
/*
*Function:Driver
*Inputs:parent object, object name
*Outputs:none
*Returns:none
*/
Driver::Driver(QObject *parent,const QString &name) : QObject(parent,name),Name(name)
{
	IT_IT("Driver::Driver");
};
/*
*Function: ~Driver
*Inputs:none
*Outputs:none
*Returns:none
*/
Driver::~Driver() 
{
	IT_IT("Driver::~Driver");

	Stop(); // make sure all children are dead
};
// placeholders for base slots
void Driver::Start() // start everything under this driver's control
{};
void Driver::Stop() // stop everything under this driver's control
{};
void Driver::Command(const QString &, BYTE, LPVOID, DWORD, DWORD) // process a command for a named unit 
{};

void Driver::CommandDlg(QWidget *parent, const QString &name) // command dialog
{}
//
// the virtuals - just placeholders 
// 
void Driver::UnitConfigure(QWidget *parent, const QString &name, const QString &receipe) { }; // configure a unit
void Driver::SetTypeList(QComboBox *pCombo, const QString &unitname) { }; // set the type list for unit type
void Driver::GetInputList(const QString &, QStringList &,const QString &unit, const QString &name) { }; // get the permitted input IDs
QWidget * Driver::GetSpecificConfig(QWidget *,const QString &spname, const QString &sptype) { return 0;}; //specific config for sample point of type
void Driver::DropAllSpecTables(QStringList &list) { }; // drop all unit specific table
void Driver::DropOneSpecTable(const QString &name) { }; // drop one unit specific table
//
//
//
//
/*
*Function: GetTagList
* returns the list of permitted tag names
*Inputs:none
*Outputs:none
*Returns:none
*/
void Driver::GetTagList(const QString &type, QStringList &list,const QString &,const QString &)
{
	IT_IT("Driver::GetTagList");

	list << VALUE_TAG; // default behaviour is to return the VALUE_TAG (Value)
}; // returns the permitted tags for a given type for this unit
void Driver::Trace(const QString &name, const QString &msg)
{
	IT_IT("Driver::Trace");
	
	emit TraceOut(name,msg);
};
/*
*Function:CreateSamplePoint
*Inputs:sample point name, list of tags
*Outputs:none
*Returns:none
*/
void Driver::CreateSamplePoint(const QString &name, QStringList &l,const QString &initProps)
{
	IT_IT("Driver::CreateSamplePoint");
	
	QString cmd; 
	cmd = "insert into PROPS values('SAMPLEPROPS','" + name + "','" + initProps +"');"; // initialise the properties
	GetConfigureDb()->DoExec(0,cmd,0); // post it off
	//
	// add the tags

	for(unsigned i = 0; i < l.count(); i++)
	{
		
		cmd = "insert into TAGS values('" + name + "','" + l[i] + "'"DEFAULT_TAG_VALS");";
		
		GetConfigureDb()->DoExec(0,cmd,0); // add the default tag config

		cmd = "insert into TAGS_DB values('" + name +"','" + l[i] + "'," + DATETIME_EPOCH + ",0,0,0,0,0,0,0,0);";

		GetCurrentDb()->DoExec(0,cmd,0); // create the tag entry
	}


	//
	// create the table
	//
	
	cmd = "create table "+ name + " ( TIMEDATE " + DATE_TIME_COL_TYPE + ",STATE int4";
	
	for(unsigned ii = 0; ii < l.count(); ii++)
	{
		cmd += "," + l[ii] + " real8";
	}

	//cmd += ",QUALITYCODE int4";

	cmd += " );";

	GetResultDb()->DoExec(0,cmd,0); //FastDB

	if(GetHistoricResultDb() != NULL)
	{
		GetHistoricResultDb()->DoExec(0,cmd,0); //GigaBASE
	}

	cmd = "create index on "+name+".TIMEDATE;";

	GetResultDb()->DoExec(0,cmd,0);//FastDB

	if(GetHistoricResultDb() != NULL)
	{
		GetHistoricResultDb()->DoExec(0,cmd,0); //GigaBASE
	}
	//
	//
	QString scmd = "insert into CVAL_DB values('" + name + // the sample point current value
	"'," + DATETIME_EPOCH +"," + DATETIME_EPOCH + ",0,0,0,0," + DATETIME_EPOCH + ",0,'(***)'," + DATETIME_EPOCH + ",0);";

	GetCurrentDb()->DoExec(0,scmd,0);
	//
};
/*
*Function: CheckTags
*makes sure the tuple in the results table has all the correct columns
*Inputs:name of sample point, expected list of tags
*Outputs:none
*Returns:none
*/
void Driver::CheckTags(const QString &Name, const QStringList &currentList, const QStringList &list)
{
	IT_IT("Driver::CheckTags");
	
	for(unsigned i = 0; i < list.count(); i++)
	{
		if(currentList.find(list[i]) == list.end())
		{
			// create the tag entry and the database column
			GetConfigureDb()->DoExec(0,"insert into TAGS values('" + Name + "','" + list[i] + "'"DEFAULT_TAG_VALS");",0); // add the default tag config
			//
			GetCurrentDb()->DoExec(0,"insert into TAGS_DB values('" + Name +"','" + list[i] +
			"'," + DATETIME_EPOCH + ",0,0,0,0,0,0,0);" ,0); // create the tag entry into the current values
			//
			// amend the database table
			// it does not matter that there may infact already be a column existing - one can contrive a 
			// create delete sequence... but...
			//  
			#ifdef USING_POSTGRES			
			QString cmd = "ALTER TABLE " + Name + " ADD COLUMN \"" + list[i].lower() + "\"  float8;";
			#endif
			#ifdef USING_INTERBASE			
			QString cmd = "ALTER TABLE " + Name + " ADD COLUMN \"" + list[i].lower() + "\"  float;";
			#endif
			#ifdef USING_GARRET			
			//TODO Fix here
			//QString cmd = "alter table " + Name + " ADD COLUMN \"" + list[i].lower() + "\"  float;";
			#endif

			//GetResultDb()->DoExec(0,cmd,0);
			//GetHistoricResultDb()->DoExec(0,cmd,0);
			//
		};
	};
};
//
////////////////instance stuff ///////////////////////////////////////////////////////////////////////
// 
/*
*Function: DriverInstance
*Inputs:parent object, object name
*Outputs:none
*Returns:none
*/
DriverInstance::DriverInstance(Driver *parent, const QString &name) : 
QObject(parent,name),Name(name),fTrace(0),fTest(0),
pending_transactions(0), UnitName(QString::null)
{
	IT_IT("DriverInstance::DriverInstance");
	
	pResults = new Results(this); // this handles filing data to results and current value databases

	InQueue.setAutoDelete(true);
	InQueue.clear();
};
/*
*Function:~DriverInstance
*Inputs:none
*Outputs:none
*Returns:none
*/
// 
DriverInstance::~DriverInstance() 
{
	IT_IT("DriverInstance::~DriverInstance");

	Stop(); // force the object to stop
};
/*
*Function: driverEvent
*handles asynchronous notifications from a child driver thread
*Inputs:opcode , data as int, data as QString
*Outputs:none
*Returns:none
*/
//Realtime method
void DriverInstance::driverEvent(DriverEvent *p)
{
	IT_IT("DriverInstance::driverEvent");
	
	switch(p->opcode())
	{
		case DriverEvent::OpTrace: // trace message
		{
			//
			Trace(p->text());
			//
		};
		break;
		case DriverEvent::OpUnitFail: // we have a failure of a unit
		{
			//
			FailUnit(p->text());
			//
		};
		break;
		case DriverEvent::OpUnitUnFail: // failure has ended on the unit
		{
			//
			UnFailUnit(p->text());
			//
		};
		break;
		case DriverEvent::OpSampleFail: // a sample point has failed
		{
			//
			//
			FailSamplePoint(p->text(), p->text1());
			//
		};
		break;
		case DriverEvent::OpSampleUnFail: // a sample point has stopped failing
		{
			//
			//
			UnFailSamplePoint(p->text(), p->text1());
			//
		};
		break;
		case DriverEvent::OpTerminate:
		{
			threadDelete(p->caller()); // call overloaded function to handle the thread delete - zero any thing
			delete p->caller(); // delete the calling object
		};
		break;
		case DriverEvent::OpPostList:
		{
			IECValueList *sp = (IECValueList *)(p->Pointer()); 
			if(sp)
			{
				PostList(p->text(), *sp);
				delete sp; // delete the data
			};
		};
		break;
		default:
		break;
	};
};
//
// place holders for slots    
// 
void DriverInstance::Start(){}; // start everything under this driver's control
void DriverInstance::Stop(){}; // stop everything under this driver's control
void DriverInstance::Command(const QString &, BYTE, LPVOID, DWORD, DWORD)
{
	
}; // process a command for a named unit 
/*
*Function: PostValue
*send a single value to the results databases
*Inputs:unti name, tag name, value
*Outputs:none
*Returns:none
*/

//Realtime method
void DriverInstance::PostValue(const QString &name, const QString &tag, double value)
{
	IT_IT("DriverInstance::PostValue");
	
	IECValue v(tag,&value);

	IECValueList l;

	l.insert(l.end(),v);

	PostList(name,l);
};
/*
*Function: PostList
*Inputs:object name, new value list
*Outputs:none
*Returns:none
*/

//Realtime method
void DriverInstance::PostList(const QString &name, IECValueList &list)
{
	IT_IT("DriverInstance::PostList");
	
	if(fTrace)
	{
		for(unsigned i = 0; i < list.size(); i++)
		{
			Trace(tr("Post Value:") + name +  " [" + list[i].tag + "] Value " + get_value_iec_value(list[i]));
		};
	};

	pResults->QueueResult(name, list); // ca tout !!

};
/*
*Function: FormSampleUnitList
*Inputs:none
*Outputs:none
*Returns:none
*/
QStringList DriverInstance::EnabledUnits;          // this is the list of enabled units
DriverInstance::DriverProps DriverInstance::Props; // properties dictionary
QMutex DriverInstance::Lock; // mutex lock

#ifdef USE_RIPC_MIDDLEWARE
RIPCFactory* DriverInstance::global_factory1;
RIPCFactory* DriverInstance::global_factory2;
RIPCSession* DriverInstance::global_session1;
RIPCSession* DriverInstance::global_session2;
RIPCQueue* DriverInstance::fifo_global_monitor_direction;
RIPCQueue* DriverInstance::fifo_global_control_direction;
#endif

////////////////////////////////Middleware/////////////////
iec_item_type DriverInstance::global_instanceSend;
ORTEPublication* DriverInstance::global_publisher = NULL;
////////////////////////////////Middleware/////////////////
//
QString DriverInstance::FormUnitList() // build the lists for SQL transaction filtering
{
	IT_IT("DriverInstance::FormUnitList");
	
	QString lu;
	for(unsigned i = 0; i < EnabledUnits.count(); i++)
	{
		if(i)
		{
			lu += ",";
		}
		lu += "'" + DriverInstance::EnabledUnits[i] + "'";
	}

	if(!lu.isEmpty())
	{
		return lu;
	}
	else
	{
		return QString("''");
	}
}
/*
*Function:FormSamplePointList
*Inputs:none
*Outputs:none
*Returns:none
*/
QString DriverInstance::FormSamplePointList()
{
	IT_IT("DriverInstance::FormSamplePointList");
	
	QString lu;
	SamplePointDictWrap &d = Results::GetEnabledPoints(); // get the dictionary and lock it
	SamplePointDictWrap::iterator i = d.begin();
	for(; !(i == d.end()); i++)
	{
		if(!(i == d.begin()))
		{
			lu += ",";
		};
		lu += "'" + (*i).first + "'";
	};
	Results::Unlock(); // unlock it
	return lu;
};
/*
*Function:SetAllInUnit
*Inputs:comment for current value, state to set the sample point to
*Outputs:none
*Returns:none
*/
void DriverInstance::SetAllInUnit(const QString &comment, int state) // put all sample points associated with a unit into a state
{
	IT_IT("DriverInstance::SetAllInUnit");
	
	SamplePointDictWrap &d = 	Results::GetEnabledPoints();
	if(d.size() > 0)
	{
		//
		// get all the sample points owned by the unit and enabled
		SamplePointDictWrap::iterator i = d.begin();
		for(; !(i == d.end());i++)
		{
			//printf("Name = %s\n", (const char*)Name);
			if((*i).second.Unit == Name)
			{
				SetSamplePointState((*i).first, comment,state); 
			};
		};
	};   
};
/*
*Function:FailSamplePoint
*Inputs:sample point name, comment
*Outputs:none
*Returns:none
*/
void DriverInstance::FailSamplePoint(const QString &name, const QString &comment) // fail the sample point with comment
{
	IT_IT("DriverInstance::FailSamplePoint");
	
	SetSamplePointState(name, comment,FailureLevel);
	QSLogAlarm("Monitor",name + tr(" has failed : ") + comment );
	
	if(fTrace)
	{
		Trace(name + tr(" has failed : ") + comment);
	}
};
/*
*Function:UnFailSamplePoint
*Inputs:sample point name, comment
*Outputs:none
*Returns:none
*/
void DriverInstance::UnFailSamplePoint(const QString &name, const QString &comment) // fail the sample point with comment
{
	IT_IT("DriverInstance::UnFailSamplePoint");
	
	SetSamplePointState(name, comment,NoLevel);
	QSLogAlarm("Monitor",name + tr(" has stopped failing : ") + comment );
}; 
/*
*Function:SetSamplePointState
*Inputs:sample point name, comment to set, state to set
*Outputs:none
*Returns:none
*/
void DriverInstance::SetSamplePointState(const QString &name, const QString &comment,int state) // set the state of a sample point
{
	IT_IT("DriverInstance::SetSamplePointState");
	//
	SamplePointDictWrap &d = Results::GetEnabledPoints(); // get the dictionary and lock it
	SamplePointDictWrap::iterator i = d.find(name);
	if(!(i == d.end()))
	{
		(*i).second.Comment = comment;
		(*i).second.AlarmState = state; // this is usually NoLevel or FailureLevel
		(*i).second.fChanged = true; // mark as changed
		if(state == FailureLevel)
		{
			(*i).second.failtime = QDateTime::currentDateTime();
		};
	};
	pResults->UpdateSamplePointAlarmGroup(name,state,false); // update the alarm groups to which this sample point belongs
	Results::Unlock();
};

/*
*Function:OverWriteSamplePointState APA added 06-03-04
*Inputs:sample point name, comment to set, state to set
*Outputs:none
*Returns:none
*/
void DriverInstance::OverWriteSamplePointState(const QString &name, const QString &comment,int state) // set the state of a sample point
{
	IT_IT("DriverInstance::OverWriteSamplePointState");
	//
	SamplePointDictWrap &d = Results::GetEnabledPoints(); // get the dictionary and lock it
	SamplePointDictWrap::iterator i = d.find(name);
	if(!(i == d.end()))
	{
		(*i).second.Comment = comment;
		(*i).second.AlarmState = state;
		(*i).second.fOvState = true; // mark as overwrite
		if(state == FailureLevel)
		{
			(*i).second.failtime = QDateTime::currentDateTime();
		};
	};
	//TODO: fix the group part
	//pResults->UpdateSamplePointAlarmGroup(name,state,false); // update the alarm groups to which this sample point belongs
	//Results::Unlock();
};

/*
*Function:FindSamplePoint
*Inputs:unit name, sp type, input index
*Outputs:none
*Returns:sample pointer or null on failure
*/
const QString  DriverInstance::FindSamplePoint(const QString &unit, const QString &type, const QString &ip)
{
	IT_IT("DriverInstance::FindSamplePoint");
	
	SamplePointDictWrap &d = Results::GetEnabledPoints(); // get the dictionary and lock it
	SamplePointDictWrap::iterator i = d.begin();
	for(; !(i == d.end()); i++)
	{
		//
		if((*i).second.Unit == unit)
		{
			if((*i).second.Type == type)
			{
				if((*i).second.InputIndex == ip)
				{
					return (*i).first;
				};
			};
		};
	};
	return QString::null;
};


/*
*Function:CurrentTransaction
*Inputs:none
*Outputs:none
*Returns:none
*/
SendRecePacket &  DriverInstance::CurrentTransaction()
{
	IT_IT("DriverInstance::CurrentTransaction");

	return *(InQueue.head());
};
/*
*Function:GetTransQueueSize
*Inputs:none
*Outputs:none
*Returns:none
*/
unsigned  DriverInstance::GetTransQueueSize()
{
	IT_IT("DriverInstance::GetTransQueueSize");

	return  InQueue.count();
};

/*
*Function: Tick
*checks for triggered events - run every second or so
*Inputs:none
*Outputs:none
*Returns:none
*/
void DriverInstance::Tick()
{
	IT_IT("DriverInstance::Tick");
	
	// Check the event counts
};

//
// ************************* DRIVER THREAD ROUTINES
// 
/*
*Function: Terminate
*hint the thread should stop
*Inputs:none
*Outputs:none
*Returns:none
*/
void DriverThread::Terminate() // parent requests the thread close
{
	IT_IT("DriverThread::Terminate");

	fRequestHalt = true;
};
/*
*Function:Trace
*trace out a message
*Inputs:trace text
*Outputs:none
*Returns:none
*/
void  DriverThread::Trace(const QString &s)
{
	IT_IT("DriverThread::Trace");

	postEvent(Parent,new DriverEvent(this,DriverEvent::OpTrace,0,s));	
};
/*
*Function:UnitFail
*fail the unit
*Inputs:message
*Outputs:none
*Returns:none
*/
void  DriverThread::UnitFail(const QString &s) 
{
	IT_IT("DriverThread::UnitFail");

	postEvent(Parent,new DriverEvent(this,DriverEvent::OpUnitFail,0,s));	
}; // we have a failure of a unit
/*
*Function:UnitUnFail
*Unfail theunit, say it is working
*Inputs:message text
*Outputs:none
*Returns:none
*/
void  DriverThread::UnitUnFail(const QString &s) // failure has ended on the unit
{
	IT_IT("DriverThread::UnitUnFail");
	
	postEvent(Parent,new DriverEvent(this,DriverEvent::OpUnitUnFail,0,s));	
};
/*
*Function:SampleFail
*fail a sample point
*Inputs:name, message
*Outputs:none
*Returns:none
*/
void  DriverThread::SampleFail(const QString &name, const QString &s) // a sample point has failed
{
	IT_IT("DriverThread::SampleFail");
	
	postEvent(Parent,new DriverEvent(this,DriverEvent::OpSampleFail,0,name,s));	
};
/*
*Function:SampleUnFail
*mark a sample point as working
*Inputs:name,text
*Outputs:none
*Returns:none
*/
void  DriverThread::SampleUnFail(const QString &name, const QString &s) // a sample point has stopped failing
{
	IT_IT("DriverThread::SampleUnFail");
	
	postEvent(Parent,new DriverEvent(this,DriverEvent::OpSampleUnFail,0,name,s));	
};
/*
*Function:Terminating
*notify parent thread wants to terminate
*Inputs:none
*Outputs:none
*Returns:none
*/
void  DriverThread::Terminating() // the thread wants to terminate - fatal type of error - by the time the parent receives this the thread has exited
{
	IT_IT("DriverThread::Terminating");
	
	postEvent(Parent,new DriverEvent(this,DriverEvent::OpTerminate,0)); // parent must delete
};
/*
*Function:PostList
*Inputs:Sample Point Name, list of values
*Outputs:none
*Returns:none
*/
void DriverThread::PostList(const QString &name, IECValueList &list) // send a list of values to the results database
{
	IT_IT("DriverThread::PostList");
	
	IECValueList * l = new IECValueList;

	*l = list;
	//
	DriverEvent * d = new DriverEvent(this,DriverEvent::OpPostList,0,name);

	d->SetPointer((void *)l); // set the data

	postEvent(Parent,d); // parent must delete IECValueList
	//
};
/*
*Function:PostValue
*Inputs:sample point name, tag name, value
*Outputs:none
*Returns:none
*/
void DriverThread::PostValue(const QString &name, const QString &tag, double value) // post a single value to the results database
{
	IT_IT("DriverThread::PostValue");
	
	IECValue v(tag,&value);
	IECValueList * l = new IECValueList;
	l->insert(l->end(),v);
	//
	DriverEvent * d = new DriverEvent(this,DriverEvent::OpPostList,0,name);
	d->SetPointer((void *)l); // set the data
	postEvent(Parent,d); // parent must delete data IECValueList
	//
};



bool DriverThread::SetCommand(SendRecePacket * packet)
{	
	IT_IT("DriverThread::SetCommand");
	
	if(Done) 
	{
		pinternal_packet = packet;
		Trigger = true; // go 
		Done = false; // mark as not complete
		fFail = false; //reset the failures state
		
		m_hevtSinc.signal();

		return true;
	}
	return false;
}
	
