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
*Purpose: this manages the data acquisition side of things
*/

#include "qplatformdefs.h"
#include "common.h"
#include "monitor.h"
#include "messages.h"
#include "driver.h"
#include "serialconnect.h"
#include "schedule.h"
#include "results.h"
#include "utilities.h"
#include "dispatch.h"
#include "general_defines.h"
#include "IndentedTrace.h"
#include "historicdb.h"
#include "logout.xpm"
#include "start.xpm"
#include "quit.xpm"
#include "monitor.xpm"
#include "clear_crc_eight.h"

//#define TICK_CYCLE 2000 //2 seconds
#define TICK_CYCLE 100

/////////////////////////fifo///////////////////////////////////////////
extern void iec_call_exit_handler(int line, char* file, char* reason);
#include "fifoc.h"
#define MAX_FIFO_SIZE 65535
////////////////////////////////////////////////////////////////////////

#include "iec104types.h"
#include "iec_item.h"

#ifdef USE_RIPC_MIDDLEWARE
void control_dir_consumer(void* pParam)
{
	struct subs_args* arg = (struct subs_args*)pParam;
	struct iec_item item;
	RIPCObject objDesc(&item, sizeof(struct iec_item));

	Monitor* parent = (Monitor*)arg->parent;

	while(1)
	{
		if(parent->exit_command_thread)
		{
			break;
		}

		parent->queue_control_dir->get(objDesc);

		fifo_put(parent->fifo_control_direction, (char*)&item, sizeof(struct iec_item));
	}
}
#endif

/////////////////////////////////////Middleware///////////////////////////////////////////
Boolean  quite = ORTE_FALSE;
int	regfail=0;

//event system
void onRegFail(void *param) 
{
  printf("registration to a manager failed\n");
  regfail = 1;
}

void rebuild_iec_item_message(struct iec_item *item2, iec_item_type *item1)
{
	unsigned char checksum;

	///////////////Rebuild struct iec_item//////////////////////////////////
	item2->iec_type = item1->iec_type;
	memcpy(&(item2->iec_obj), &(item1->iec_obj), sizeof(struct iec_object));
	item2->cause = item1->cause;
	item2->msg_id = item1->msg_id;
	item2->ioa_control_center = item1->ioa_control_center;
	item2->casdu = item1->casdu;
	item2->is_neg = item1->is_neg;
	item2->checksum = item1->checksum;
	///////and check the 1 byte checksum////////////////////////////////////
	checksum = clearCrc((unsigned char *)item2, sizeof(struct iec_item));

//	fprintf(stderr,"new checksum = %u\n", checksum);

	//if checksum is 0 then there are no errors
	if(checksum != 0)
	{
		//log error message
		ExitProcess(0);
	}

//	fprintf(stderr,"iec_type = %u\n", item2->iec_type);
//	fprintf(stderr,"iec_obj = %x\n", item2->iec_obj);
//	fprintf(stderr,"cause = %u\n", item2->cause);
//	fprintf(stderr,"msg_id =%u\n", item2->msg_id);
//	fprintf(stderr,"ioa_control_center = %u\n", item2->ioa_control_center);
//	fprintf(stderr,"casdu =%u\n", item2->casdu);
//	fprintf(stderr,"is_neg = %u\n", item2->is_neg);
//	fprintf(stderr,"checksum = %u\n", item2->checksum);
}

void recvCallBack(const ORTERecvInfo *info,void *vinstance, void *recvCallBackParam) 
{
	Monitor * cl = (Monitor*)recvCallBackParam;
	iec_item_type *item1 = (iec_item_type*)vinstance;

	switch (info->status) 
	{
		case NEW_DATA:
		{
		  if(!quite)
		  {
			  struct iec_item item2;
			  rebuild_iec_item_message(&item2, item1);
			  fifo_put(cl->fifo_control_direction, (char*)&item2, sizeof(struct iec_item));
		  }
		}
		break;
		case DEADLINE:
		{
			//printf("deadline occurred\n");
		}
		break;
	}
}
////////////////////////////////Middleware/////////////////////////////////////


/*
*Function: Monitor
*Inputs:none
*Outputs:none
*Returns:none
*/
Monitor * Monitor::Instance = 0;
//
Monitor::Monitor(QObject *parent,RealTimeDbDict *db_dct, Dispatcher *dsp) : QObject(parent),
fStarted(false),SequenceNumber(0),fHalt(0),translation(0),dispatcher(dsp),db_dictionary(*db_dct),
MaxRetryReconnectToDispatcher(0),MaxRetryReconnectToRealTimeDb(0),
MaxRetryReconnectToHistoricDb(0),MaxRetryReconnectToSpareDispatcher(0),
MaxRetryReconnectToSpareRealTimeDb(0)
{
	IT_IT("Monitor::Monitor");
	
	Instance = this;
	MidnightReset = 1;

	#ifdef USE_RIPC_MIDDLEWARE
	//Open global fifos
	DriverInstance::global_factory1 = RIPCClientFactory::getInstance();
	DriverInstance::global_factory2 = RIPCClientFactory::getInstance();
	DriverInstance::global_session1 = DriverInstance::global_factory1->create("localhost", 6000);
	DriverInstance::global_session2 = DriverInstance::global_factory2->create("localhost", 6000);
	DriverInstance::fifo_global_monitor_direction = DriverInstance::global_session1->createQueue("fifo_global_monitor_direction");
	DriverInstance::fifo_global_control_direction = DriverInstance::global_session2->createQueue("fifo_global_control_direction");

	queue_control_dir = DriverInstance::fifo_global_control_direction;
	#endif
	
	fifo_control_direction = fifo_open("fifo_control_dir", MAX_FIFO_SIZE, iec_call_exit_handler);

	#ifdef USE_RIPC_MIDDLEWARE
	arg.parent = this;
	unsigned long threadid;
	CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(control_dir_consumer), (void*)&arg, 0, &threadid);
	#endif

	/////////////////////Middleware/////////////////////////////////////////////////////////////////
	ORTEDomainProp          dp; 
	ORTESubscription        *s = NULL;
	int32_t                 strength = 1;
	NtpTime                 persistence,deadline,minimumSeparation,delay;
	Boolean                 havePublisher = ORTE_FALSE;
	Boolean                 haveSubscriber = ORTE_FALSE;
	IPAddress				smIPAddress = IPADDRESS_INVALID;
	ORTEDomainAppEvents     events;

	ORTEInit();
	ORTEDomainPropDefaultGet(&dp);
	NTPTIME_BUILD(minimumSeparation, 0); //0 s
	NTPTIME_BUILD(delay, 1); //1 s

	//initiate event system
	ORTEDomainInitEvents(&events);

	events.onRegFail = onRegFail;

	//Create application     
	domain = ORTEDomainAppCreate(ORTE_DEFAULT_DOMAIN,&dp,&events,ORTE_FALSE);

	iec_item_type_type_register(domain);

	//Create publisher
	NTPTIME_BUILD(persistence, 5); //5 s
	
	DriverInstance::global_publisher = ORTEPublicationCreate(
	domain,
	"fifo_global_monitor_direction",
	"iec_item_type",
	&(DriverInstance::global_instanceSend),
	&persistence,
	strength,
	NULL,
	NULL,
	NULL);

	//Create subscriber
	NTPTIME_BUILD(deadline,3);

	subscriber = ORTESubscriptionCreate(
	domain,
	IMMEDIATE,
	BEST_EFFORTS,
	"fifo_global_control_direction",
	"iec_item_type",
	&instanceRecv,
	&deadline,
	&minimumSeparation,
	recvCallBack,
	this,
	smIPAddress);
	///////////////////////////////////Middleware//////////////////////////////////////////////////


	// Connect to real time database

	#ifdef USE_STD_MAP
	RealTimeDbDict::iterator it = db_dictionary.begin();

	RealTimeDbDict::iterator j;
	
	j =  db_dictionary.find("configdb");

	if(j != db_dictionary.end())
	{
		CfgDb = (*j).second;
	}

	j =  db_dictionary.find("currentdb");

	if(j != db_dictionary.end())
	{
		CurDb = (*j).second;
	}

	j =  db_dictionary.find("resultsdb");

	if(j != db_dictionary.end())
	{
		ResDb = (*j).second;
	}

	#else

	#endif

	// Connect to realtime database
	connect (CfgDb,
	SIGNAL (TransactionDone(QObject *, const QString &, int, QObject*)), this,
	SLOT (ConfigQueryResponse(QObject *, const QString &, int, QObject*)));	// connect to the database

	connect (CurDb,
	SIGNAL (TransactionDone(QObject *, const QString &, int, QObject*)), this,
	SLOT (CurrentQueryResponse(QObject *, const QString &, int, QObject*)));	// connect to the database

	connect (ResDb,
	SIGNAL (TransactionDone(QObject *, const QString &, int, QObject*)), this,
	SLOT (ResultsQueryResponse(QObject *, const QString &, int, QObject*)));	// connect to the database

	if(GetHistoricResultDb() != NULL)
	{
		// Connect to historical database
		connect (GetHistoricResultDb(),
		SIGNAL (TransactionDone(QObject *, const QString &, int, QObject*)), this,
		SLOT (HistoricResultsQueryResponse(QObject *, const QString &, int, QObject*)));	// connect to the database
	}

	//Connect to dispatcher
	connect (dispatcher,
	SIGNAL (ReceivedNotify(int, const char *)), this,
	SLOT (ReceivedNotify(int, const char *)));
	
	//
	// scrub the alarm group tables
	//
	// We should also set the timezone of the client here - for true internationalisation
	// we have to allow for different clients accessing the computer from different time zones
	// also data may be logged from different time zones
	//
	//

	QString cmd = "select IKEY,DVAL from PROPS where SKEY='System';"; // get the system properties before anything
	CfgDb->DoExec (this, cmd, tGet);	// make the request
	
	ResetTables(); 
	
	CurDb->DoExec(0,"update PROPS set DVAL='0' where SKEY='System' and IKEY='Lock';",0); // remove sysmgr lock
	//
	//
	pSchedule =  new Schedule(this); // create the event scheduler task
	//
	QTimer *pT = new QTimer(this);
	connect(pT,SIGNAL(timeout()),this,SLOT(Tick()));
	pT->start(TICK_CYCLE); // run every 2 seconds 
	//	
};

/*-Function:~Monitor
*Inputs:none
*Outputs:none
*Returns:none
*/
Monitor::~Monitor()
{
	IT_IT("Monitor::~Monitor");
	
	Stop();
	Instance = 0;

	#ifdef USE_RIPC_MIDDLEWARE
	exit_command_thread = 1;
	DriverInstance::fifo_global_monitor_direction->close();
	DriverInstance::fifo_global_control_direction->close();
	DriverInstance::global_session1->close();
	DriverInstance::global_session2->close();
	delete DriverInstance::global_session1;
	delete DriverInstance::global_session2;
	#endif
};
/*
*Function:UpdateCurrentValue
*Inputs:sample point to update
*Outputs:none
*Returns:none
*/
void Monitor::UpdateCurrentValue( const QString &name, SamplePoint &sp)
{
	IT_IT("Monitor::UpdateCurrentValue");
	
	//
	// update the sample point
	//	
	QString cmd;
	QString updated = (const char*)0;
	//
	// now update the current tags
	// 
	SamplePoint::TagDict::iterator i = sp.Tags.begin();
	for(;!(i == sp.Tags.end());i++)
	{
		if((*i).second.changed && (*i).second.enabled) // updated ?
		{ 
			(*i).second.changed = 0;
			cmd = "update TAGS_DB set ";
			cmd += "UPDTIME=";
				
			updated = QDATE_TIME_ISO_DATE((*i).second.updated);

			cmd += updated;

			if(IsNAN_Double(get_value_iec_value((*i).second.value)))
			{
				cmd += ",VAL="+QString::number(0);
			}
			else
			{
				cmd += ",VAL="+QString::number(get_value_iec_value((*i).second.value));
			}

			if(IsNAN_Double((*i).second.stats.Min()))
			{
				cmd += ",MINVAL="+QString::number(0);
			}
			else
			{
				cmd += ",MINVAL="+QString::number((*i).second.stats.Min());
			}

			if(IsNAN_Double((*i).second.stats.Max()))
			{
				cmd += ",MAXVAL="+QString::number(0);
			}
			else
			{
				cmd += ",MAXVAL="+QString::number((*i).second.stats.Max());
			}

			if(IsNAN_Double((*i).second.stats.sum()))
			{
				cmd += ",SUMVAL="+QString::number(0);	
			}
			else
			{
				cmd += ",SUMVAL="+QString::number((*i).second.stats.sum());
			}

			if(IsNAN_Double((*i).second.stats.sum2()))
			{
				cmd += ",SUM2VAL="+QString::number(0.0); //Forzo a 0.0
			}
			else
			{
				cmd += ",SUM2VAL="+QString::number((*i).second.stats.sum2());
			}

			cmd += ",NVAL="+QString::number((*i).second.stats.samples());
			cmd += ",STATE="+QString::number((*i).second.state);
			cmd += ",SEQNO="+QString::number(SequenceNumber); // machine independent idea of time ordering
			cmd += " where NAME='"+name+"' and TAGNAME='"+(*i).first+"';";  
			CurDb->DoExec(0,cmd,0);
		};
	};

	if(updated.isNull())
	{
		updated = DATETIME_NOW;
	}

	cmd = "update CVAL_DB set UPDTIME=" + updated + ", ALMTIME=" + QDATE_TIME_ISO_DATE(sp.alarmtime)
	+ ",NMEASURE="+QString::number(sp.nmeasures)+",NALARM="+QString::number(sp.nalarms)+
	",NWARNING="+QString::number(sp.nwarnings)+",STATE="+QString::number(sp.AlarmState)+
	",COMMENT='"+ EscapeSQLText(sp.Comment)+"'" +
	",FAILTIME="+QDATE_TIME_ISO_DATE(sp.failtime);
	//
	if(sp.fAckTriggered)
	{
		cmd += ",ACKFLAG="+ QString::number(sp.fAckTriggered);
		sp.fAckTriggered = 0;
	};
	cmd += ",SEQNO="+QString::number(SequenceNumber);
	cmd += " where NAME='"+name+"';";
	//CurDb->DoExec(this,cmd,tUpdateDone,name); //apa--- 16-06-2014
	CurDb->DoExec(this,cmd,tUpdateDone, QString::number(SequenceNumber)); //apa+++ 16-06-2014
	//
	sp.fChanged = 0; // mark as not changed
	//
	//
};
/*
*Function: Tick
*Inputs:none
*Outputs:none
*Returns:none
*/
void Monitor::Tick()
{
	IT_IT("Monitor::Tick");

	if(GetSpareDispatcher() != NULL)
	{
		if(!GetSpareDispatcher()->Ok())
		{
			QSLogEvent("Monitor", "Spare dispatcher client connection error");
			QSLogEvent("Monitor", "Attempt to restore connection with spare dispatcher server");

			if(GetSpareDispatcher()->IsConnected())
			{
				if(!GetSpareDispatcher()->Ok())
				{
					DisconnectFromSpareDispatcher();
				}
			}

			if(!GetSpareDispatcher()->IsInRetry())
			{
				ConnectToSpareDispatcher();
			}

			++MaxRetryReconnectToSpareDispatcher;
		}
		else
		{
			MaxRetryReconnectToSpareDispatcher = 0;
		}
	}

	if(!GetDispatcher()->Ok())
	{
		QSLogEvent("Monitor", "Dispatcher client connection error");
		QSLogEvent("Monitor", "Attempt to restore connection with dispatcher server");

		if(GetDispatcher()->IsConnected())
		{
			if(!GetDispatcher()->Ok())
			{
				DisconnectFromDispatcher();
			}
		}

		if(!GetDispatcher()->IsInRetry())
		{
			ConnectToDispatcher();
		}

		++MaxRetryReconnectToDispatcher;
	}
	else
	{
		MaxRetryReconnectToDispatcher = 0;
	}

	if(!GetConfigureDb()->Ok() || !GetResultDb()->Ok() || !GetCurrentDb()->Ok())
	{
		if(!GetConfigureDb()->Ok())
		{
			QString msg = QString("Real time client error: ") + GetConfigureDb()->GetErrorMessage();
			QSLogEvent("Monitor", msg);
			GetConfigureDb()->AcnoledgeError();
		}

		if(!GetResultDb()->Ok())
		{
			QString msg = QString("Real time client error: ") + GetResultDb()->GetErrorMessage();
			QSLogEvent("Monitor", msg);	
			GetResultDb()->AcnoledgeError();
		}
		
		if(!GetCurrentDb()->Ok())
		{
			QString msg = QString("Real time client error: ") + GetCurrentDb()->GetErrorMessage();
			QSLogEvent("Monitor", msg);	
			GetCurrentDb()->AcnoledgeError();
		}

		QSLogEvent("Monitor", "Attempt to restore connection with realtime database server");

		//if(GetConfigureDb()->IsConnected() || GetCurrentDb()->IsConnected() || GetResultDb()->IsConnected())
		{
			if(!GetConfigureDb()->Ok() || !GetResultDb()->Ok() || !GetCurrentDb()->Ok())
			{
				DisconnectFromRealTimeDatabases();
			}
		}
		
		ConnectToRealTimeDatabases();

		++MaxRetryReconnectToRealTimeDb;
	}
	else
	{
		MaxRetryReconnectToRealTimeDb = 0;
	}

	if((GetSpareConfigureDb() != NULL) && (GetSpareCurrentDb() != NULL)&&(GetSpareResultDb() != NULL))
	{
		if(!GetSpareConfigureDb()->Ok() || !GetSpareResultDb()->Ok() || !GetSpareCurrentDb()->Ok())
		{
			if(!GetSpareConfigureDb()->Ok())
			{
				QString msg = QString("Spare real time client error: ") + GetSpareConfigureDb()->GetErrorMessage();
				QSLogEvent("Monitor", msg);
				GetSpareConfigureDb()->AcnoledgeError();
			}

			if(!GetSpareResultDb()->Ok())
			{
				QString msg = QString("Spare real time client error: ") + GetSpareResultDb()->GetErrorMessage();
				QSLogEvent("Monitor", msg);	
				GetSpareResultDb()->AcnoledgeError();
			}
			
			if(!GetSpareCurrentDb()->Ok())
			{
				QString msg = QString("Spare real time client error: ") + GetSpareCurrentDb()->GetErrorMessage();
				QSLogEvent("Monitor", msg);	
				GetSpareCurrentDb()->AcnoledgeError();
			}

			QSLogEvent("Monitor", "Attempt to restore connection with spare realtime database server");

			if(GetSpareConfigureDb()->IsConnected())
			{
				if(!GetSpareConfigureDb()->Ok())
				{
					DisconnectFromSpareRealTimeDatabases();
				}
			}

			ConnectToSpareRealTimeDatabases();
			++MaxRetryReconnectToSpareRealTimeDb;
		}
		else
		{
			MaxRetryReconnectToSpareRealTimeDb = 0;
		}
	}

	if(GetHistoricResultDb() != NULL)
	{
		if(!GetHistoricResultDb()->Ok())
		{
			QString msg = QString("Historical client error: ") + GetHistoricResultDb()->GetErrorMessage();
			QSLogEvent("Monitor", msg);
			GetHistoricResultDb()->AcnoledgeError();
			QSLogEvent("Monitor", "Attempt to restore connection with historical database server");

			DisconnectFromHistoricDatabases();
			
			ConnectToHistoricDatabases();
			++MaxRetryReconnectToHistoricDb;
		}
		else
		{
			MaxRetryReconnectToHistoricDb = 0;
		}
	}

/*
	if((MaxRetryReconnectToHistoricDb > 50) || 
		(MaxRetryReconnectToRealTimeDb > 50) ||
		(MaxRetryReconnectToDispatcher > 50) || 
		(MaxRetryReconnectToSpareRealTimeDb > 50) ||
		(MaxRetryReconnectToSpareDispatcher > 50) 
		)
	{
		//We have no more connection with some server
		//What do we do?
	}
*/	
	//
	// update alarm groups    
	bool fUpdated = false;
	SequenceNumber++; // increment the tick count
	// 
	Results::GroupDict &g = Results::GetGroups();
	Results::GroupDict::iterator i = g.begin();
	//
	for(; !(i == g.end()); i++)
	{
		// 
		// walk the group table 
		if((*i).second.Changed)
		{
			(*i).second.Changed = 0;
			(*i).second.State = 0;
			//
			Results::StateDict::iterator j = (*i).second.begin();
			for(; !(j == (*i).second.end());j++)
			{
				//
				// now update those sample points that are member of an alarm group 
				// that have not yet been updated
				SamplePointDictWrap &d = Results::GetEnabledPoints();
				SamplePointDictWrap::iterator k = d.find((*j).first);
				//
				if(!(k == d.end()))
				{
					if((*k).second.fChanged)
					{
						QString cmd = "update ALM_GRP_STATE set STATE=" + QString::number((*k).second.AlarmState);
						cmd += ",UPDTIME=";
						cmd += DATETIME_NOW;
						if((*k).second.fAckTriggered)
						{
							cmd += ", ACKSTATE=1";
						};
						cmd += ",SEQNO="+QString::number(SequenceNumber); // machine independent idea of time ordering
						cmd +=  " where SPNAME='" + (*k).first +"';";
						CurDb->DoExec(0,cmd,0); // do it
						//
						// now write the sample point's current values out
						//
						UpdateCurrentValue((*k).first,(*k).second);
						(*k).second.fChanged = 0;
						//
						//
					}
					// track largest alarm level
					if((*k).second.AlarmState > (*i).second.State)
					{
						(*i).second.State = (*k).second.AlarmState;
					}            
				}
			}
			// write it out  
			QString cmd = "update ALM_GRP set STATE=" + QString::number((*i).second.State);
			cmd += ",UPDTIME=";
			cmd +=  DATETIME_NOW;
			if((*i).second.AckState)
			{
				cmd += ", ACKSTATE=1"; // set the ack flag if so triggered
				(*i).second.AckState = 0;
			}	      
			cmd += ",SEQNO="+QString::number(SequenceNumber); // machine independent idea of time ordering
			cmd += " where NAME='"+(*i).first+"';";
			//
			fUpdated=true;
			CurDb->DoExec(0,cmd,0); // do it
		}    
	}
	//
	// now write out the sample point's current states that do not belong to a group
	//
	{
		SamplePointDictWrap &d = Results::GetEnabledPoints();
		SamplePointDictWrap::iterator k = d.begin();
		for(;!(k == d.end());k++)
		{
			if((*k).second.fChanged)
			{
				UpdateCurrentValue((*k).first,(*k).second);
				(*k).second.fChanged = 0;
				fUpdated = true;
			}
		}
	}
	//
	//
	//
	//
	if(fUpdated)
	{
		CurDb->DoExec(this,"select * from ALM_GRP limit 1;",tAllUpdated); //from 2.51

		if(MidnightReset)
		{
			if((lastHour == 23) && (QTime::currentTime().hour() == 0))
			{
				//
				// reset the stats
				ResetStatistics();
				// 
			}
			lastHour = QTime::currentTime().hour();
		}
	}

	//broadcast
	dispatcher->DoExec(NotificationEvent::MONITOR_TICK_NOTIFY, (const char*)QString::number(SequenceNumber) );
	//
	// Handle request to halt
	//
	if(fHalt)
	{
		Stop();
		QTimer::singleShot(2000,qApp,SLOT(quit()));
	}

	/////Get global commands///////////////////////////
	get_items_from_global_fifo();
}

#include "clear_crc_eight.h"

void Monitor::get_items_from_global_fifo(void)
{
	unsigned char buf[sizeof(struct iec_item)];
	int len;
	const unsigned wait_limit_ms = 1;
	struct iec_item* p_item;

	for(int i = 0; (len = fifo_get(fifo_control_direction, (char*)buf, sizeof(struct iec_item), wait_limit_ms)) >= 0; i += 1)	
	{ 
		p_item = (struct iec_item*)buf;

		//printf("Receiving %d th message \n", p_item->msg_id);
		printf("Receiving %d th global command\n", p_item->msg_id);

		//for (int j = 0; j < len; j++) 
		//{ 
			//assert((unsigned char)buf[i] == len);
			//unsigned char c = *((unsigned char*)buf + j);
			//printf("rx <--- 0x%02x-\n", c);
			//fprintf(fp,"rx <--- 0x%02x-\n", c);
			//fflush(fp);

			//IT_COMMENT1("rx <--- 0x%02x-\n", c);
		//}

		//printf("---------------\n");
		
		unsigned char rc = clearCrc((unsigned char *)buf, sizeof(struct iec_item));

		if(rc != 0)
		{
			ExitProcess(1);
		}

		QString value;

		switch(p_item->iec_type)
		{
			case C_SC_TA_1:
			{
				value.sprintf("%d", p_item->iec_obj.o.type58.scs);
			}
			break;
			case C_SC_NA_1:
			{
				value.sprintf("%d", p_item->iec_obj.o.type45.scs);
			}
			break;
			case C_DC_TA_1:
			{
				value.sprintf("%d", p_item->iec_obj.o.type59.dcs);
			}
			break;
			case C_DC_NA_1:
			{
				value.sprintf("%d", p_item->iec_obj.o.type46.dcs);
			}
			break;
			case C_SE_TA_1:
			{
				value.sprintf("%d", p_item->iec_obj.o.type61.sv);
			}
			break;
			case C_SE_NA_1:
			{
				value.sprintf("%d", p_item->iec_obj.o.type48.sv);
			}
			break;
			case C_SE_TB_1:
			{
				value.sprintf("%d", p_item->iec_obj.o.type62.sv);
			}
			break;
			case C_SE_NB_1:
			{
				value.sprintf("%d", p_item->iec_obj.o.type49.sv);
			}
			break;
			case C_SE_TC_1:
			{
				value.sprintf("%f", p_item->iec_obj.o.type63.sv);
			}
			break;
			case C_SE_NC_1:
			{
				value.sprintf("%f", p_item->iec_obj.o.type50.sv);
			}
			break;
			default:
			{
				printf("Not supported type%d \n", p_item->iec_type);
				value.sprintf("%d", 0);
			}
			break;
		}
		
		QString ioa;
		ioa.sprintf("%d", p_item->iec_obj.ioa);
		
		QString cmd = "select NAME,UNIT from TAGS where IOA="+ ioa + ";";

		GetConfigureDb()->DoExec(this, cmd, tGetUnitfromIOA, value);

		//printf("ioa %s, value %s\n", (const char*)ioa, (const char*)value);

		if(i > 50)
		{
			break;
		}
	}
}

/*
*Function: Start
*tell all drivers to start
*Inputs:none
*Outputs:none
*Returns:none
*/
void Monitor::Start()
{
	IT_IT("Monitor::Start");
	
	//
	// start everything
	//
	if(!fStarted)
	{ 
		DDict::iterator i = drivers.begin();
		for(; !(i == drivers.end()); i++)
		{
			if((*i).second)
			{
				IT_COMMENT2("Monitor Starting", "%s",(const char *)((*i).first));

				QSMessage(tr("Starting") + " " + (*i).first); 
				QSLogEvent("Monitor",tr("Starting") + " "+ (*i).first);  
				(*i).second->Start(); // signal the driver to start 
			};
		};

		//
		// log we have started monitoring
		//
		QSLogEvent("Monitor",tr("Monitor started"));

		//
		fStarted = true;
		//
		#ifdef UNIX
		// When we start we do a version control snap shot on the configuration database
		// this should go most of the way towards keeping CFR 21 Part 11 users happy
		// This catches configuration changes that we made but failed to run version control
		// (eg user interface crashed)
		// 
		// CFR 21 Part 11 is an FDA document 2 pages long with about 30 pages of explination
		// 
		QString cmd = QSBIN_DIR + "/version.sh"; // this is the script for doing this
		//
		// under Linux we can invoke cvs
		// We assume the postgres structures have been imported into CVS at this point
		// this assumes (oh dear !) that the system has been configured ready for use 
		// and then put under version control prior to validation (we are talking Pharamceutical parinoia here).
		// 
		// The number of tables (files) should not change but it might be necessary 
		// following upgrades or bug fixes to add new tables to CVS.
		// 
		QFile f(cmd);
		if(f.exists())
		{
			cmd += " \"Monitor Startup\""; // this is the script for doing this
			cmd += "&";
			system((const char *)cmd); 
		};
		#endif
		//
		//
		//
		//broadcast
		dispatcher->DoExec(NotificationEvent::MONITOR_STARTED_NOTIFY); // notify started monitoring
	};
};
/*
*Function: Stop
*Stop all drivers
*Inputs:none
*Outputs:none
*Returns:none
*/
void Monitor::Stop()
{
	IT_IT("Monitor::Stop");
		
	if(fStarted)
	{
		DDict::iterator i = drivers.begin();
		for(; !(i == drivers.end()); i++)
		{
			if((*i).second)
			{
				IT_COMMENT2("Monitor Stopping", "%s",(const char *)((*i).first));

				QSMessage(tr("Stopping") + " " + (*i).first);   
				(*i).second->Stop(); // signal the driver to stop
			};
		};
		drivers.clear();
		fStarted = false;
		//broadcast
		dispatcher->DoExec(NotificationEvent::MONITOR_STOPPED_NOTIFY);
		QSLogEvent("Monitor",tr("Stopped Monitoring"));
		//
		DriverInstance::Props.clear(); // clear and properties or semaphores
	}
};

/*
*Function:ConfigQueryResponse
*Inputs:client , command, transactionid
*Outputs:none
*Returns:none
*/
void Monitor::ConfigQueryResponse (QObject *p,const QString &c, int id, QObject* caller)  // handles configuration responses
{
	if(p != this) return;

	IT_IT("Monitor::ConfigQueryResponse");

	switch(id)
	{
		case tGet: // we have the system properties
		{
			for(unsigned i = 0; i < CfgDb->GetNumberResults(); i++,CfgDb->FetchNext())
			{
				if(CfgDb->GetString("IKEY") == "Language")
				{
					if(UndoEscapeSQLText(CfgDb->GetString("DVAL")) != "English")
					{
						//
						// we have the language - default is English (sorry) 
						// 
						if(translation.load(UndoEscapeSQLText(CfgDb->GetString("DVAL")),QSTRANS_DIR))
						{
							qApp->installTranslator(&translation); // install the translator
						};
					};
				}
			}
		};
		break;
		case tUnitTypes:
		{
			Stop();
			// we now have a list of drivers to load - load them 
			// set a one shot to go off in a second or so
			if(CfgDb->GetNumberResults() > 0)
			{
				int n = CfgDb->GetNumberResults();
				//
				// Load the drivers
				//
				for(int i = 0 ; i < n ; i++, CfgDb->FetchNext())
				{
					QString s = CfgDb->GetString("UNITTYPE");

                    DDict::iterator iter = drivers.find(s);

                    if(iter == drivers.end()) //apa+++ 20-12-2012
                    {
					    Driver *p = FindDriver(s);
					    
					    IT_COMMENT1("Loading UNIT %s", (const char*)s);

					    if(p)
					    {
						    QSLogEvent("Monitor",tr("Loading Module") + " " +s);
						    DDict::value_type pr(s,p);
						    drivers.insert(pr); // put in the dictionary
						    //
						    QObject::connect(this,SIGNAL(DoCommand(const QString &, BYTE, LPVOID , DWORD, DWORD)),
						    p,SLOT(Command(const QString &, BYTE , LPVOID, DWORD, DWORD ))); // connect the command slot
						    //
						    // connect the trace line to the top level application
						    // 
						    QObject::connect(p,SIGNAL(TraceOut(const QString &,const QString &)),
						    this,SLOT(Trace(const QString &, const QString &)));
					    }
                    }
				}
				//
				// now we get the receipe if it is not (default)
				//
				//Results::GetEnabledPoints().clear(); //<------crash on exit of Monitor applivation 17-05-2005 APA. With STLPort no more crash;
				//DriverInstance::EnabledUnits.clear();//<------crash on exit of Monitor stop start
				//
				if(GetReceipeName() != "(default)")
				{
					QString cmd = "select * from RECEIPE where NAME='"+ GetReceipeName() +"';";
					CfgDb->DoExec(this,cmd,tReceipeRecord); // request the record
				}
				else
				{ 
					//
					// the default enabled units and sample points
					// 
					CfgDb->DoExec(this,"select * from UNITS where ENABLED=1;",tUnits);
					CfgDb->DoExec(this,"select * from SAMPLE where ENABLED=1;",tSamples);
				};
			};
		};
		break;
		case tReceipeRecord:
		{
			if(CfgDb->GetNumberResults() > 0)
			{
				// we now have the list of enabled units and stuff
				QString s = CfgDb->GetString("UNITS"); // enabled units
				QTextIStream is(&s);
				is >> ws;
				while (!is.atEnd ())
				{
					//
					QString a;
					is >> a;
					DriverInstance::EnabledUnits << a.stripWhiteSpace ();
				};
				//
				s = CfgDb->GetString("SAMPLES"); // enabled sample points
				QTextIStream ss(&s);
				ss >> ws;
				//
				SamplePointDictWrap &d = Results::GetEnabledPoints();
				d.clear();
				//
				while (!ss.atEnd ())
				{
					//
					QString a;
					ss >> a;
					//
					//SamplePoint smp(CfgDb->GetString("NAME"));
					//SamplePointDictWrap::value_type pr(CfgDb->GetString("NAME"),smp);
					SamplePoint smp(a.stripWhiteSpace());
					SamplePointDictWrap::value_type pr(a.stripWhiteSpace(), smp);

					d.insert(pr);
				};
				//
				QString cmd = "select * from SAMPLE where NAME in (" + DriverInstance::FormSamplePointList() + ");";
				CfgDb->DoExec(this,cmd,tSamples); // get the sample point configuration
				//
			};
		};
		break;
		case tUnits:
		{
			if(CfgDb->GetNumberResults() > 0)
			{
				int n = CfgDb->GetNumberResults();
				for(int i = 0; i < n; i++, CfgDb->FetchNext())
				{
					DriverInstance::EnabledUnits << CfgDb->GetString("NAME");
				};
			};
		};
		break;
		case tSamples:
		{
			//loading the enabled sample points
			
			//Results::GetEnabledPoints().clear();  //<------crash on exit of Monitor applivation 17-05-2005 APA With STLPort no more crash; 13-10-09 commentato

			if(CfgDb->GetNumberResults() > 0)
			{
				int n = CfgDb->GetNumberResults();

				for(int i = 0; i < n; i++, CfgDb->FetchNext())
				{
					SamplePoint s(CfgDb->GetString("NAME"));
					
					SamplePointDictWrap &d = Results::GetEnabledPoints();
					
					SamplePointDictWrap::value_type pr(CfgDb->GetString("NAME"), s);
					
					d.insert(pr); //questa insert va in crash su Vista

	 				SamplePointDictWrap::iterator j = d.find(CfgDb->GetString("NAME"));
					
					//
					(*j).second.Unit = CfgDb->GetString("UNIT"); // which unit is assicated with this SP
					(*j).second.Fileable = CfgDb->GetBool("FILEABLE"); // do we store the results
					(*j).second.Retriggerable = CfgDb->GetBool("RETRIGGER"); // do we have retriggerable alarms
					(*j).second.AlarmThreshold = CfgDb->GetInt("ALARMTHRESHOLD"); // what is the alarm threshold count
					(*j).second.Type = CfgDb->GetString("QTYPE"); // the type
					(*j).second.InputIndex = CfgDb->GetString("IPINDEX"); // input index
					//
				}
				//
				if(GetReceipeName() != "(default)")
				{
					QString cmd = "select * from TAGS where ((RECEIPE='" + GetReceipeName() + 
					"') or (RECEIPE='(default)')) order by RECEIPE asc;";
					
					CfgDb->DoExec(this,cmd,tTags); // get all the tags used 
				}
				else
				{
					CfgDb->DoExec(this,
					"select * from TAGS where RECEIPE='(default)';",tTags); // get all the tags used 
				}
				// the (default) receipe will come first then the current receipe
				//
			}
		}
		break;
		case tTags:
		{
			//loading the enabled tags
						
			if(CfgDb->GetNumberResults() > 0)
			{
				int n = CfgDb->GetNumberResults();

				for(int i = 0; i < n; i++, CfgDb->FetchNext())
				{
					SamplePointDictWrap &d = Results::GetEnabledPoints();
					SamplePointDictWrap::iterator j = d.find(CfgDb->GetString("NAME"));

					IT_COMMENT1("Name =%s", (const char *)CfgDb->GetString("NAME"));

					if(j != d.end())
					{
						SamplePoint::TagDict::iterator k = (*j).second.Tags.find(CfgDb->GetString("TAG"));

						if(k == (*j).second.Tags.end())
						{
							SamplePoint::TagDict::value_type pr(CfgDb->GetString("TAG"),TagItem());

							IT_COMMENT1("Tag = %s", (const char *)CfgDb->GetString("TAG"));

							//printf("Tag = %s", (const char *)CfgDb->GetString("TAG"));

							(*j).second.Tags.insert(pr);

							k = (*j).second.Tags.find(CfgDb->GetString("TAG"));
						}
						//
						// set the alarm limits
						//
						//   
						(*k).second.UpperAlarm.Enabled = CfgDb->GetBool("UAENABLE");
						(*k).second.UpperAlarm.Limit= CfgDb->GetDouble("UPPERALARM");
						//
						(*k).second.UpperWarning.Enabled = CfgDb->GetBool("UWENABLE");
						(*k).second.UpperWarning.Limit= CfgDb->GetDouble("UPPERWARN");
						//
						(*k).second.LowerWarning.Enabled = CfgDb->GetBool("LWENABLE");
						(*k).second.LowerWarning.Limit= CfgDb->GetDouble("LOWERWARN");
						//
						(*k).second.LowerAlarm.Enabled = CfgDb->GetBool("LAENABLE");
						(*k).second.LowerAlarm.Limit= CfgDb->GetDouble("LOWERALARM");  
						//
						//
						(*k).second.enabled = CfgDb->GetInt("ENABLED") ? true : false; 
						//
						//
					}
				}
				//
				QString cmd = "select * from CVAL_DB;";
				CurDb->DoExec(this,cmd,tSamplesCurrent); // get the sample point current values
				//
				// now get the tags afterwards
				CurDb->DoExec(this,"select * from TAGS_DB;",tTagsCurrent); // after this has completed we start
				//
			}
		}
		break;
		case tAlarmGroups:
		{
			IT_COMMENT("tAlarmGroups");
			
			if(CfgDb->GetNumberResults() > 0)
			{
				int n = CfgDb->GetNumberResults();
				//
				// Load the drivers
				//
				for(int i = 0 ; i < n ; i++, CfgDb->FetchNext())
				{
					QString name = CfgDb->GetString("NAME");
					//
					QString cmd = "insert into ALM_GRP values ('" + 
					name + "',0,0," + DATETIME_NOW + ",0);";	

					//
					CurDb->DoExec(0,cmd,0); // insert
					//
					QString s = CfgDb->GetString("SAMPLES");
					//
					Results::GroupDict &g = Results::GetGroups();
					Results::GroupDict::value_type pr(name,Results::StateDict());
					g.insert(pr); 

					Results::GroupDict::iterator igp = g.find(name); 
					//
					if(!(igp == g.end()))
					{
						QTextIStream is (&s);
						int ns = 0;		// how many names
						is >> ns;
						//
						if (ns > 0)
						{
							for (int j = 0; j < ns; j++)
							{
								QString a;
								is >> a;
								a = a.stripWhiteSpace ();

								cmd = 
								"insert into ALM_GRP_STATE values('" + name + 
								"','" + a + "',0,0," + DATETIME_NOW + ",0);";	


								CurDb->DoExec(0,cmd,0); // insert
								//
								Results::StateDict::value_type spr(a,0); // add to internal version of group
								(*igp).second.insert(spr); //
								//
							}
						}
					}
					//
				}
			}
			//broadcast
			dispatcher->DoExec(NotificationEvent::CURRENT_NOTIFY);
			dispatcher->DoExec(NotificationEvent::ALARMGROUP_NOTIFY);
		}
		break;
		case tReceipe:
		{
			IT_COMMENT("tReceipe");
			
			// set the receipe name
			if(CfgDb->GetNumberResults() > 0)
			{
				for(unsigned i = 0; i < CfgDb->GetNumberResults(); i++,CfgDb->FetchNext())
				{
					if(CfgDb->GetString("IKEY") == "Receipe")
					{
						QSLogEvent("Monitor",tr("Setting Receipe to") + " " + UndoEscapeSQLText(CfgDb->GetString("DVAL")));  
						SetReceipeName (UndoEscapeSQLText(CfgDb->GetString("DVAL"))); 
					}
					else if(CfgDb->GetString("IKEY") == "MidnightReset")
					{
						MidnightReset = CfgDb->GetInt("DVAL");
					};

				};
			};
		};
		break;
		case tGetUnitfromIOA:
		{
			QSTransaction &t = GetConfigureDb()->CurrentTransaction();
			
			if(CfgDb->GetNumberResults() > 0)
			{
				for(unsigned i = 0; i < CfgDb->GetNumberResults(); i++,CfgDb->FetchNext())
				{
					unsigned char data[sizeof(dispatcher_extra_params)];
					dispatcher_extra_params* params = (dispatcher_extra_params *) data;

					memset(data, 0, sizeof(dispatcher_extra_params));
								
					params->value = atof((const char*)t.Data1);
					
					QString unit_name = CfgDb->GetString("UNIT");
					QString samplePointName = CfgDb->GetString("NAME");

					strcpy(params->string1, (const char *)unit_name); //driver instance
					strcpy(params->string2, (const char *)samplePointName);
					strcpy(params->string3, (const char *)t.Data1);

					struct cp56time2a actual_time;
					get_utc_host_time(&actual_time);
					params->time_stamp = actual_time;
	
					Command(unit_name, 0, (void*) data, sizeof(dispatcher_extra_params), 0);

					//printf("Command for unit %s and sample point %s, data = %s\n", (const char*)unit_name, (const char*)samplePointName, (const char *)t.Data1);
				}
			}
		}
		default:
		break;
	};
};

/*
*Function:ResultsQueryResponse
*for talking to the real time results database
*Inputs:client, command, transaction id
*Outputs:none
*Returns:none
*/
void Monitor::ResultsQueryResponse (QObject *p,const QString &c, int id, QObject* caller) // results responses
{
	if(p != this) return;

	IT_IT("Monitor::ResultsQueryResponse");
};

/*
*Function:HistoricResultsQueryResponse
*for talking to the historic results database
*Inputs:client, command, transaction id
*Outputs:none
*Returns:none
*/
void Monitor::HistoricResultsQueryResponse (QObject *p,const QString &c, int id, QObject* caller) // results responses
{
	if(p != this) return;

	IT_IT("Monitor::HistoricResultsQueryResponse");
};

/*
*Function: CurrentQueryResponse
*talking to the current values database
*Inputs:client, command, transaction id
*Outputs:none
*Returns:none
*/
void Monitor::CurrentQueryResponse (QObject *p,const QString &c, int id, QObject* caller) // current value responses
{
	if(p != this) return;

	IT_IT("Monitor::CurrentQueryResponse");

	switch(id)
	{
		case tUpdateDone:
		{
			#define USE_UPDATE_NOTIFY 1
			#ifdef USE_UPDATE_NOTIFY
			QSTransaction &t = CurDb->CurrentTransaction();
			//broadcast
			dispatcher->DoExec(NotificationEvent::UPDATE_NOTIFY,(const char*) t.Data1);
			#endif
		}
		break;
		case tAllUpdated: // post a notification to update alarm and current values
		{
			//broadcast
			dispatcher->DoExec(NotificationEvent::ALARMGROUP_NOTIFY);
			dispatcher->DoExec(NotificationEvent::CURRENT_NOTIFY);
		}
		break;
		case tSamplesCurrent: 
		{	
			// get the current values for samples
			if(CurDb->GetNumberResults() > 0)
			{
				int n = CurDb->GetNumberResults();
				for(int i = 0; i < n; i++, CurDb->FetchNext())
				{
					// update the current values with the values from the last time we started
					SamplePointDictWrap &d = Results::GetEnabledPoints();
					SamplePointDictWrap::iterator j = d.find(CurDb->GetString("NAME"));
					if(!(j == d.end()))
					{
						(*j).second.fChanged = 0;        // has the sample point changed
						(*j).second.fAckTriggered = 0;   // has an ack been triggered
						(*j).second.AlarmState = CurDb->GetInt("STATE");      // the alarm state
						(*j).second.AlarmEvents = 0;     // alarm events
						//
						//
						(*j).second.Comment = UndoEscapeSQLText(CurDb->GetString("COMMENT"));     // the last comment string (for current value)
						(*j).second.nalarms = CurDb->GetInt("NALARM");         // number of alarms
						(*j).second.nwarnings = CurDb->GetInt("NWARNING");     // number of warnings
						(*j).second.nmeasures = CurDb->GetInt("NMEASURE");     // number of measures
						(*j).second.alarmtime = CurDb->GetDateTime("ALMTIME"); // last alarm
						(*j).second.failtime = CurDb->GetDateTime("FAILTIME"); // last failure
					}

					//16-08-2003: Reset of CVAL_DB
					CurDb->DoExec(0,"update CVAL_DB set STATE=0, UPDTIME=" + DATETIME_NOW + "where NAME='" + CurDb->GetString("NAME") + "';",0); // mark as not measured
					CurDb->DoExec(0,"update CVAL_DB set SEQNO=0 where NAME='" + CurDb->GetString("NAME") + "';",0); // mark as not measured
					CurDb->DoExec(0,"update CVAL_DB set NMEASURE=0,NALARM=0,NWARNING=0 where NAME='" + CurDb->GetString("NAME") + "';",0);
				}
			}
		}
		break;
		case tTagsCurrent:
		{	
			// get the current tag values
			
			if(CurDb->GetNumberResults() > 0)
			{
				int n = CurDb->GetNumberResults();
				for(int i = 0; i < n; i++, CurDb->FetchNext())
				{
					// 
					// update the current values with the values from the last time we started
					//
					SamplePointDictWrap &d = Results::GetEnabledPoints();
					SamplePointDictWrap::iterator k =  d.find(CurDb->GetString("NAME"));
					if(!(k == d.end()))
					{
						(*k).second.db_idx = i;

						SamplePoint::TagDict::iterator j = (*k).second.Tags.find(CurDb->GetString("TAGNAME"));
						if(!(j == (*k).second.Tags.end()))
						{
							if((*j).second.enabled)
							{
								set_value_iec_value((*j).second.value, CurDb->GetDouble("VAL")); // current value
								(*j).second.state = CurDb->GetInt("STATE"); // current state
								(*j).second.updated =  CurDb->GetDateTime("UPDTIME"); // when it was updated
								(*j).second.stats.reset();
								//(*j).second.stats.set
								//(
								//CurDb->GetInt("NVAL"),
								//CurDb->GetDouble("SUMVAL"),
								//CurDb->GetDouble("SUMVAL2"),
								//CurDb->GetDouble("MINVAL"),
								//CurDb->GetDouble("MAXVAL")
								//); // the stats
							}
						}  
					}
					
					//16-08-2003: Reset of TAGS_DB
					CurDb->DoExec(0, "update TAGS_DB set SEQNO=0,STATE=0 where NAME='"+ CurDb->GetString("NAME") +"' and TAGNAME='"+ CurDb->GetString("TAGNAME") +"';",0);
				}
			}
			//  
			Start(); // we can go now
			//
		}
		break;
		default:
		break;
	};
};

/*
*Function: ReceivedNotify
*notifications to control the montior task
*Inputs:notification code
*Outputs:none
*Returns:none
*/
void Monitor::ReceivedNotify(int ntf, const char * data)
{
	IT_IT("Monitor::ReceivedNotify");

	switch(ntf)
	{
		case NotificationEvent::CMD_MONITOR_START:
		{
			IT_COMMENT("CMD_MONITOR_START - monitoring start command is received");
			// start by getting the unit types -> drivers
			ResetTables(); 
			pSchedule->Restart();
			//
		}
		break;
		case NotificationEvent::CMD_MONITOR_STOP:
		{
			IT_COMMENT("CMD_MONITOR_STOP - monitoring stop command is received");
			Stop();
		}
		break;
		case  NotificationEvent::CMD_SHUTDOWN_MONITOR:
		{
			QTimer::singleShot(500,qApp,SLOT(quit())); // quit in 1 seconds
		}
		break;
		case NotificationEvent::CMD_SEND_COMMAND_TO_UNIT:
		{
			dispatcher_extra_params* params = (dispatcher_extra_params *)data;
			QString driver_instance = QString(params->string1);
			//QString sample_point_name = QString(params->string2);

			Command(driver_instance, 0, (void*) data, sizeof(dispatcher_extra_params), 0);

			//IT_COMMENT1("Command for sample point %s", (const char*)sample_point_name);
		}
		break;
		default:
		break;
	}
};

/*
*Function: ResetStatistics()
*Inputs:none
*Outputs:none
*Returns:none
*/
void Monitor::ResetStatistics()
{
	IT_IT("Monitor::ResetStatistics");
	
	SamplePointDictWrap &d = Results::GetEnabledPoints();
	SamplePointDictWrap::iterator k =  d.begin();
	//
	for(;!(k == d.end());k++)
	{
		SamplePoint::TagDict::iterator j = (*k).second.Tags.begin();
		for(;!(j == (*k).second.Tags.end());j++)
		{
			(*j).second.stats.reset();
		};  
	};
		
	QString cmd = "update CVAL_DB set NMEASURE=0,NALARM=0,NWARNING=0;";
	CurDb->DoExec(0,cmd,0);

	QSLogEvent("Monitor",tr("Reset Statistics"));
	//
};
/*
*Function:ResetTables
*Inputs:none
*Outputs:none
*Returns:none
*/
void Monitor::ResetTables()
{
	IT_IT("Monitor::ResetTables");
	
	CurDb->DoExec(0,"delete from ALM_GRP;",0);
	CurDb->DoExec(0,"delete from ALM_GRP_STATE;",0);

	QString cmd = "update CVAL_DB set ACKFLAG=0,ACKTIME=" +  DATETIME_NOW + " where ACKFLAG=1;";	
	CurDb->DoExec(0,cmd,0);	
	//	
	cmd = "update ALM_GRP_STATE set ACKSTATE=0;";	
	CurDb->DoExec(0,cmd,0); // do the transaction	

	cmd = "update ALM_GRP set ACKSTATE=0;";	
	CurDb->DoExec(0,cmd,0); // do the transaction

	CfgDb->DoExec(this,"select * from ALARMGROUP;",tAlarmGroups); // build the alarm group
	CfgDb->DoExec(this,"select * from PROPS where SKEY='System';",tReceipe);
	CfgDb->DoExec(this,"select UNITTYPE from UNITS;",tUnitTypes);
	
	//broadcast
	GetDispatcher()->DoExec(NotificationEvent::CURRENT_NOTIFY);
	GetDispatcher()->DoExec(NotificationEvent::ALARMGROUP_NOTIFY);
	GetDispatcher()->DoExec(NotificationEvent::ACK_NOTIFY);
};
//
/*
*Function:Trace
*Inputs:message source, message text
*Outputs:none
*Returns:none
*/
void Monitor::Trace(const QString &src,const QString &msg)
{   
	IT_IT("Monitor::Trace");

	// now forward to attached interfaces
	QSTrace( src + ": " + msg);

	printf("%s %s\n", (const char*)src, (const char*)msg);
	
	emit TraceOut(src,msg); // forward it
};

#include <time.h>
#include <sys/timeb.h>

void Monitor::get_utc_host_time(struct cp56time2a* time)
{
	struct timeb tb;
	struct tm	*ptm;

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

    return;
}

//
//
//
//
// ********************** STARTUP **********************************************************
//
#ifdef UNIX
void SigTermHandler(int)
{
	IT_IT("SigTermHandler");

	Monitor::Instance->fHalt = true;
};
#endif
//
//
//
int main(int argc, char **argv)
{
	IT_IT("main - MONITOR");

	int stat = -1;

	//version control///////////////////////////////////////////////////////////////
	char version[100];
    sprintf(version, "monitor.exe - Build: %s %s at enscada.com",__DATE__,__TIME__);
    fprintf(stdout, "%s\n", version);
	SYSTEMTIME oT;
	::GetLocalTime(&oT);
	fprintf(stdout,"%02d/%02d/%04d, %02d:%02d:%02d Starting ... %s\n",oT.wMonth,oT.wDay,oT.wYear,oT.wHour,oT.wMinute,oT.wSecond,SYSTEM_NAME_MONITOR); 
	fflush(stdout);
	////////////////////////////////////////////////////////////////////////////////
	
	if(!IsSingleInstance(""SYSTEM_NAME_MONITOR""))
	{
		IT_COMMENT("Another instance of the monitor is already running!");//error message
		return stat;
	}
		
	SetScadaHomeDirectory(argv[0]);
	
	// 
	// if the DISPLAY variable is empty or not set then we go into none-GUI mode
	// this application can run GUI mode or non-GUI mode
	// it is expected that the GUI mode is started when we want to do debugging
	//   
	#ifdef UNIX
	bool useGUI = (getenv( "DISPLAY" ) != 0) && (strlen(getenv( "DISPLAY" )) > 0);
	//
	//
	if(!useGUI)
	{
		setenv("DISPLAY","localhost:0",1); 
	};
	//
	QApplication a(argc, argv,useGUI);
	#else
	QApplication a(argc, argv);
	#endif
	//
	//
	//
	if(!QT_CHDIR(QSFilename(""))) // change directory   
	{
	#ifndef MONITOR_CAN_RUN_AS_ROOT
		#ifdef UNIX
		// uid = 0 for root 
		if(getuid() > QS_MIN_UID)
		#else
		if(!RunningAsAdministrator())
		#endif
	#endif
		{
			if(OpenRealTimeConnections() && 
			   OpenDispatcherConnection() )
			{
				RealTimeDbDict realtime_databases = GetRealTimeDbDict();
				RealTimeDbDict spare_realtime_databases;
				
				Monitor *pMonitor = NULL;
				Monitor *pSpareMonitor = NULL;

				pMonitor = new Monitor(NULL, &realtime_databases, GetDispatcher());  // create the monitoring engine interface

				if(OpenSpareDispatcherConnection() && OpenSpareRealTimeConnections())
				{
					//Se esiste un server Spare, allora si crea un nuovo oggetto monitor
					//con lo spare dispatcher e con lo spare database realtime
					
					spare_realtime_databases = GetSpareRealTimeDbDict();
					pSpareMonitor = new Monitor(NULL, &spare_realtime_databases, GetSpareDispatcher());  // create the monitoring engine interface
				}

				OpenHistoricConnections();

				#ifdef UNIX
				signal(SIGTERM,SigTermHandler);						
				#endif
				
				stat = a.exec();

				if(pMonitor)
					delete pMonitor;

				if(pSpareMonitor)
					delete pSpareMonitor;

				//
				//

				#ifdef STL_BUG_FIXED
				CloseRealTimeConnections();
				#endif

				#ifdef STL_BUG_FIXED
				CloseDispatcherConnection();
				#endif

				if(GetHistoricResultDb() != NULL)
				{
					#ifdef STL_BUG_FIXED
					CloseHistoricConnections();
					#endif
				}

				if(GetSpareDispatcher() != NULL)
				{
					#ifdef STL_BUG_FIXED
					CloseSpareDispatcherConnection();
					#endif
				}

				if((GetSpareConfigureDb() != NULL) && (GetSpareCurrentDb() != NULL)&&(GetSpareResultDb() != NULL))
				{
					#ifdef STL_BUG_FIXED
					CloseSpareRealTimeConnections();
					#endif
				}

				UnloadAllDlls();
				//
				return stat;
				//
			}
			else
			{
				//cerr << "Failed to connect to database(s) and (or) dispatcher" << endl;
				IT_COMMENT("Failed to connect to database(s) and (or) dispatcher");//error messag
				MessageBox(NULL,"Failed to connect to database(s) and (or) dispatcher","Monitor", MB_OK|MB_ICONSTOP);
			}
		}
	#ifndef MONITOR_CAN_RUN_AS_ROOT
		else
		{
			//cerr << "Must Not Run As Root" << endl;
			IT_COMMENT("Must Not Run As Root");//error messag
			MessageBox(NULL,"Must Not Run As Root","Monitor", MB_OK|MB_ICONSTOP);
		}
	#endif
		
	}
	else
	{
		//cerr << "User Directory Not Accessible:" << (const char *) QSFilename("") << endl;
		
		QString err_msg;
		err_msg = "User Directory Not Accessible:" + QSFilename("")+ "\n";
		IT_COMMENT((const char *)err_msg);
	}

	return stat;
}

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
	strcat(log_file, "\\logs\\fifo.log");
#elif __unix__
	strcat(log_file, "/logs/fifo.log");	
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
