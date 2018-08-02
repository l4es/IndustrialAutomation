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
*Header For: Results processor object
*most of the magic happens here
*Process and file results
*/   

#include "results.h"
#include "IndentedTrace.h"
#include <sptypes.h>
//
Results::GroupDict Results::Groups; // alarm group dictionary 
SamplePointDictWrap Results::EnabledPoints; //<------crash on exit of Monitor applivation 17-05-2005 APA With STLPort no more crash
QMutex Results::ResultLock; // the results mutex
//

double get_value_iec_value(IECValue &v)
{
	IT_IT("get_value_iec_value");

	switch(v.type)
	{
		case M_SP_NA_1:
		return (double)v.v1_q.sp;
		case M_DP_NA_1:
		return (double)v.v3_q.dp;
		case M_BO_NA_1:
		return (double)(*(char*)&(v.v7_q.stcd));
		case M_ME_NA_1:
		return (double)(v.v9_q.mv);
		case M_ME_NB_1:
		return (double)(v.v11_q.mv);
		case M_ME_NC_1:
		return (double)(v.v13_q.mv);
		case M_SP_TB_1:
		return (double)v.v30_q.sp;
		case M_BO_TB_1:
		return (double)(*(char*)&(v.v33_q.stcd));
		case M_ME_TD_1:
		return (double)(v.v34_q.mv);
		case M_ME_TE_1:
		return (double)(v.v35_q.mv);
		case M_ME_TF_1:
		return (double)(v.v36_q.mv);
		case M_IT_TB_1:
		return (double)(v.v37_q.counter);
		case M_ME_TN_1:
		return (double)(v.v150_q.mv);
		case M_ME_TO_1:
		return (double)(v.v151_q.mv);
		case M_ME_TP_1:
		return (double)(v.v152_q.mv);
		case M_ME_TQ_1:
		return (double)(v.v153_q.mv);
		case M_ME_TR_1:
		return (double)((__int64)v.v154_q.mv);
		case M_ME_TS_1:
		return (double)(v.v155_q.mv);
		default:
		return (double)(v.v150_q.mv);
	}
}

struct cp56time2a get_time_of_iec_value(IECValue &v)
{
	IT_IT("get_time_of_iec_value");

	struct cp56time2a t;

	memset(&t, 0x00, sizeof(struct cp56time2a));

	switch(v.type)
	{
		case M_SP_NA_1:
		return t;
		case M_DP_NA_1:
		return t;
		case M_BO_NA_1:
		return t;
		case M_ME_NA_1:
		return t;
		case M_ME_NB_1:
		return t;
		case M_ME_NC_1:
		return t;
		case M_SP_TB_1:
		return v.v30_q.time;
		case M_BO_TB_1:
		return v.v33_q.time;
		case M_ME_TD_1:
		return v.v34_q.time;
		case M_ME_TE_1:
		return v.v35_q.time;
		case M_ME_TF_1:
		return v.v36_q.time;
		case M_IT_TB_1:
		return v.v37_q.time;
		case M_ME_TN_1:
		return v.v150_q.time;
		case M_ME_TO_1:
		return v.v151_q.time;
		case M_ME_TP_1:
		return v.v152_q.time;
		case M_ME_TQ_1:
		return v.v153_q.time;
		case M_ME_TR_1:
		return v.v154_q.time;
		case M_ME_TS_1:
		return v.v155_q.time;
		case M_ME_TT_1:
		return v.v156_q.time;
		default:
		return t;
	}
}

int get_quality_of_iec_value(IECValue &v)
{
	IT_IT("get_quality_of_iec_value");

//	printf("v.type = %d\n", v.type);
	int quality = 0;

	switch(v.type)
	{
		case M_SP_NA_1:
		quality =v.v1_q.iv;
		break;
		case M_DP_NA_1:
		quality =v.v3_q.iv;
		break;
		case M_BO_NA_1:
		quality =v.v7_q.iv;
		break;
		case M_ME_NA_1:
		quality =v.v9_q.iv;
		break;
		case M_ME_NB_1:
		quality =v.v11_q.iv;
		break;
		case M_ME_NC_1:
		quality =v.v13_q.iv;
		break;
		case M_SP_TB_1:
		quality =v.v30_q.iv;
		break;
		case M_BO_TB_1:
		quality =v.v33_q.iv;
		break;
		case M_ME_TD_1:
		quality =v.v34_q.iv;
		break;
		case M_ME_TE_1:
		quality =v.v35_q.iv;
		break;
		case M_ME_TF_1:
		quality =v.v36_q.iv;
		break;
		case M_IT_TB_1:
		quality =v.v37_q.iv;
		break;
		case M_ME_TN_1:
		quality =v.v150_q.iv;
		break;
		case M_ME_TO_1:
		quality =v.v151_q.iv;
		break;
		case M_ME_TP_1:
		quality =v.v152_q.iv;
		break;
		case M_ME_TQ_1:
		quality =v.v153_q.iv;
		break;
		case M_ME_TR_1:
		quality =v.v154_q.iv;
		break;
		case M_ME_TS_1:
		quality =v.v155_q.iv;
		break;
		case M_ME_TT_1:
		quality =v.v156_q.iv;
		break;
		default:
		//quality =v.v150_q.iv;
		quality = 0; //good quality
	}

	//printf("quality = %d\n", quality);

	return quality;
}

void set_value_iec_value(IECValue &v, double val)
{
	IT_IT("set_value_iec_value");	

	switch(v.type)
	{
		case M_SP_NA_1:
		v.v1_q.sp = val;
		break;
		case M_DP_NA_1:
		v.v3_q.dp = val;
		break;
		case M_BO_NA_1:
		memcpy(&v.v7_q.stcd, &val, sizeof(struct iec_stcd));
		break;
		case M_ME_NA_1:
		v.v9_q.mv = val;
		break;
		case M_ME_NB_1:
		v.v11_q.mv = val;
		break;
		case M_ME_NC_1:
		v.v13_q.mv = val;
		break;
		case M_SP_TB_1:
		v.v30_q.sp = val;
		break;
		case M_BO_TB_1:
		memcpy(&v.v33_q.stcd, &val, sizeof(struct iec_stcd));
		break;
		case M_ME_TD_1:
		v.v34_q.mv = val;
		break;
		case M_ME_TE_1:
		v.v35_q.mv = val;
		break;
		case M_ME_TF_1:
		v.v36_q.mv = val;
		break;
		case M_IT_TB_1:
		v.v37_q.counter = val;
		break;
		case M_ME_TN_1:
		v.v150_q.mv = val;
		break;
		case M_ME_TO_1:
		v.v151_q.mv = val;
		break;
		case M_ME_TP_1:
		v.v152_q.mv = val;
		break;
		case M_ME_TQ_1:
		v.v153_q.mv = val;
		break;
		case M_ME_TR_1:
		v.v154_q.mv = val;
		break;
		case M_ME_TS_1:
		v.v155_q.mv = val;
		break;
		default:
		v.v150_q.mv = val;
		break;
	}
}

/*
*Function:Results::GetGroups()
*Inputs:none
*Outputs:none
*Returns:none
*/
Results::GroupDict & Results::GetGroups() // the alarm groups
{
	IT_IT("Results::GetGroups");
	
	return Groups;
};
/*
*Function:Results::GetEnabledPoints()
*Inputs:none
*Outputs:none
*Returns:none
*/
SamplePointDictWrap & Results::GetEnabledPoints()// this is the list of enabled sample points and current values
{
//	IT_IT("Results::GetEnabledPoints");
	
	return EnabledPoints;
};
/*
*Function:Results
*constructs the results handler
*Inputs:none
*Outputs:none
*Returns:none
*/
Results::Results(QObject *parent) : QObject(parent,"Results")
{
	
	IT_IT("Results::Results");
	
	// connect to real time config the databases
	connect (GetConfigureDb (),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (ConfigQueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the databas
};
/*
*Function:QueueResult
*Inputs:sample point name, results data
*Outputs:none
*Returns:none
*/

//Realtime method
void Results::QueueResult(const QString &name, IECValueList &List)
{
	IT_IT("Results::QueueResult");
	
	//Lock(); // lock for results update //was commented
	UpdateStart(name); // begin the update

	IECValueList::iterator i = List.begin();

	for(; i != List.end();i++)
	{
		Update(name,(*i).tag,(*i)); // update the value 
	}; 

	UpdateEnd(name); // end the update
	//Unlock(); // unlock the results objects //was commented
};
/*
*Function:ConfigQueryResponse
*responses from requests to the configuration database
*Inputs:none
*Outputs:none
*Returns:none
*/
void Results::ConfigQueryResponse (QObject *p,const QString &c, int id, QObject* caller)  // handles configuration responses
{
	if(p != this) return;

	IT_IT("Results::ConfigQueryResponse");

	switch(id)
	{
		case tAlarmActions:
		{
			//
			// ***************  we do the actions here *********************
			// 
			if(GetConfigureDb()->GetNumberResults())
			{
				QString s = GetConfigureDb()->GetString("ALARM");
				if(!s.isEmpty())
				{
					//
					// if the first character is ! then this is a shell script
					if(s[0] == '!')
					{    
						// one good reason NEVER EVER to run the monitoring system as root
						#ifdef UNIX
						QString cmd = s.mid(1) + "&";
						system((const char *)cmd); // runs in the background
						#endif
					}   
					else
					{
						// it is an SQL transaction on the current values database
						// one would expect things like calculated values and so forth, changing the state of alarm outputs
						// to be done here
						GetCurrentDb()->DoExec(0,s,0); // we do the SQL transaction 
					};
				};            
			};
		};
		break;
		case tActions:
		{
			//
			// ***************  we do the actions here *********************
			// 
			if(GetConfigureDb()->GetNumberResults() > 0)
			{
				QString s = GetConfigureDb()->GetString("MEASURE");
				if(!s.isEmpty())
				{
					//
					// if the first character is ! then this is a shell script
					if(s[0] == '!')
					{    
						// one good reason NEVER EVER to run the monitoring system as root
						#ifdef UNIX
						QString cmd = s.mid(1) + "&";
						system((const char *)cmd);
						#endif
					}   
					else
					{
						// it is an SQL transaction on the current values database
						// one would expect things like calculated values and so forth, changing the state of alarm outputs
						// to be done here
						// pontentially very dangerous but then again this allows the system to be extensible
						GetCurrentDb()->DoExec(0,s,0); // we do the SQL transaction 
					};
				};
			};            
		};
		break;
		default:
		break;
	};
};
/*
*Function:UpdateSamplePoint
*Inputs:sample point name, alarm state
*Outputs:none
*Returns:none
*
* We have to cache the alarm group stuff to reduce database updates
*
*/
void Results::UpdateSamplePointAlarmGroup(const QString &spname, int state, bool ack)
{
	IT_IT("Results::UpdateSamplePointAlarmGroup");
	
	GroupDict::iterator i = Groups.begin();
	for(; !(i == Groups.end()); i++)
	{
		// look for the sample point in the group
		StateDict::iterator j = (*i).second.find(spname);
		//
		if(!(j == (*i).second.end()))
		{
			(*j).second = true; // mark as changed
			if(ack) 
			{
				(*i).second.AckState = true;
			};
			//
			if((*i).second.State < state)
			{
				(*i).second.State = state;
			};
			//
			(*i).second.Changed = true; // mark as changed
			//
		};
		//
	};    
};
/*
*Function: UpdateStart
*prepare for updating a sample point - clear state...
*Inputs:name of sample point
*Outputs:none
*Returns:none
*/

//Realtime method
void Results::UpdateStart(const QString &name) //  start an update 
{
	IT_IT("Results::UpdateStart");
	
	SamplePointDictWrap::iterator i =  EnabledPoints.find(name);

	if(!(i == EnabledPoints.end()))
	{
		(*i).second.OldState = (*i).second.AlarmState; // save the old state      
		(*i).second.AlarmState = 0; // set to lowest level
		(*i).second.fAckTriggered = false;
	}
}
/*
*Function: Update
*update a tag value
*Inputs:sample point name, tag name,new value, new state (for tag)
*Outputs:none
*Returns:none
*/

//Realtime method
void Results::Update(const QString &name, const QString &tag, IECValue &value) // update a sample point by tag
{
	IT_IT("Results::Update");
	
	SamplePointDictWrap::iterator i =  EnabledPoints.find(name);
	if(!(i == EnabledPoints.end()))
	{
		IT_COMMENT3("update Sample %s tag %s : %lf",(const char *) name,(const char *)tag, get_value_iec_value(value));

		if(mut)
		{
			ins_mutex_acquire(mut);
			//scada_db[(*i).second.db_idx].previous_value = scada_db[(*i).second.db_idx].current_value;
			scada_db[(*i).second.db_idx].current_value = get_value_iec_value(value);
			ins_mutex_release(mut);
		}
		//
		int state = (*i).second.UpdateTag(tag,value, (*i).second.Type); // update the tags
		int laststate = state; // track exit from alarm condition
		//
		if(state > (*i).second.AlarmState) // track the alarm state
		{
			(*i).second.AlarmState = state;
		}
		//
		//
		if((laststate == AlarmLevel) && (state < AlarmLevel))
		{
			QSLogAlarm(name, tr("Stopped Alarming"));			
		}
	}
};

/*
*Function:UpdateEnd
*Inputs:name of sample point
*Outputs:none
*Returns:none
*/

//Realtime method
void Results::UpdateEnd(const QString &name)
{
	IT_IT("Results::UpdateEnd");
	
	SamplePointDictWrap::iterator i =  EnabledPoints.find(name);
	if(!(i == EnabledPoints.end()))
	{
		//  
		//  
		if((*i).second.AlarmState == AlarmLevel) // track alarm levels and thresholds
		{
			(*i).second.AlarmEvents++; // do alarm threshold handling here
			if((*i).second.AlarmEvents < (*i).second.AlarmThreshold)
			{
				(*i).second.AlarmState  = WarningLevel; // mark as warning until over the alarm threshold count
			};
		}
		else
		{
			(*i).second.AlarmEvents = 0; // reset the track
		};

		/*
		//
		// trigger a measure action
		// 
		GetConfigureDb()->DoExec(this,
		QString("select NAME,MEASURE from ACTIONS where NAME='") + name + QString("';") ,tActions);
		//   
		if((*i).second.AlarmState == AlarmLevel)
		{
			//
			// do we have to set the need acknowledge flag if so we also generate an alarm log message
			// the alarm notifcation gets generated later
			// 
			if((*i).second.Retriggerable) // is this retriggerable ?
			{
				(*i).second.fAckTriggered = true; // mark that an acknowledge has been triggered
			};
			//
			//
			if( (*i).second.AlarmState  > (*i).second.OldState)  // is this a new alarm
			{
				(*i).second.fAckTriggered = true; // trigger alarm ack always
			};
			//
			if((*i).second.fAckTriggered)
			{
				// do we trigger the alarm
				//QSLogAlarm(name, tr("Threshold Alarm"));
				// trigger the alarm action
				GetConfigureDb()->DoExec(this,
				QString("select NAME,ALARM from ACTIONS where NAME='") + name + QString("';") ,tAlarmActions);
				(*i).second.alarmtime = QDateTime::currentDateTime();
			};
		};
		*/
		//
		//
		//
		//
		(*i).second.Update((*i).second.AlarmState, (*i).second.fAckTriggered);
		if((*i).second.Fileable)  // do we file this data or not - if the state is not Ok,Warning,Alarm ignore it 
		{
			if(((*i).second.AlarmState > NoLevel) && ((*i).second.AlarmState < FailureLevel))
			{
				QString cmd = "insert into " + name + " (TIMEDATE,STATE";

				SamplePoint::TagDict::iterator j = (*i).second.Tags.begin();
				
				//list the tags
				for(int kk = 0; !(j == (*i).second.Tags.end());j++,kk++)
				{
					cmd += "," + (*j).first; 
				};

				QString datatime_result = QDATE_TIME_ISO_DATE((*i).second.updated);
				
				cmd += ") values( " + datatime_result; // enter the actual update time
				// the alarm state
				cmd += "," + QString::number((*i).second.AlarmState); 
				//
				j = (*i).second.Tags.begin();

				// and the values
				for(kk = 0; !(j == (*i).second.Tags.end());j++, kk++)
				{
					if((*j).second.enabled)
					{
						cmd += "," + QString::number(get_value_iec_value((*j).second.value));
					}
					else
					{
						cmd += "," + QString::number(0.0);
					}
				};
				//
				cmd += ");";	

				
				// now do it
				GetResultDb()->DoExec(0,cmd,0); //in real time database FastDB

				if(GetHistoricResultDb() != NULL)
				{
					GetHistoricResultDb()->DoExec(0,cmd,0); //in historical database GigaBASE
				}

			};
		};
		// 
		UpdateSamplePointAlarmGroup(name, (*i).second.AlarmState, (*i).second.fAckTriggered); 
		//
	}
};

__int64 Epoch_in_millisec_from_cp56time2a(const struct cp56time2a* time)
{
	struct tm	t;
	long epoch = 0;
	int ms;
	__int64 epoch_in_milliseconds = 0;
	
	memset(&t, 0x00, sizeof(struct tm));
	
	t.tm_hour = time->hour;
	t.tm_min = time->min;
	t.tm_sec = time->msec/1000;
	ms = time->msec%1000;

	t.tm_mday = time->mday;
	t.tm_mon = time->month - 1;	  //from <1..12> to	<0..11>				
	t.tm_year = time->year + 100; //from <0..99> to <years from 1900>
	t.tm_isdst = time->su;
	
	epoch = mktime(&t);

	if((epoch == -1) || (time->iv == 1))
	{
		epoch = 0;
	}

	epoch_in_milliseconds = (__int64)epoch;

	epoch_in_milliseconds = epoch_in_milliseconds*1000 + (__int64)ms; 

	return epoch_in_milliseconds;
}

QString GetIsoDateString_from_epoch_in_millisec(__int64 epoch_in_millisec)
{
	struct tm *ts;
	
	int ms = epoch_in_millisec%1000;

	time_t seconds = epoch_in_millisec/1000;

	if((ts = localtime((time_t*)(&seconds))) == NULL)
	{
		IT_IT("GetIsoDateString_from_epoch_in_millisec");
		IT_COMMENT("Error in localtime function");
		return QString::null;
	}

	//if(_isindst(ts))
	//{
	//	ts->tm_hour + 1; // Se siamo in ora legale aggiungo un'ora
	//}

	//
	// format the tm_struct into ISO time with milliseconds
	// 
	char b[50];
	sprintf(b,"%04d-%02d-%02d %02d:%02d:%02d.%03d",ts->tm_year + 1900,ts->tm_mon + 1,ts->tm_mday,
	ts->tm_hour,ts->tm_min,ts->tm_sec,ms);

	return QString(b);
};

void SamplePoint::Update(int state, bool ack, const QString &c) // mark a sample point as updated and ack triggered
{
		
		//IT_IT("SamplePoint::Update");
		
		updated = QDateTime::currentDateTime();
		AlarmState = state;
		fChanged = true;  // mark changed
		fAckTriggered = ack; 
		Comment=c; // set the comment
		switch(state)
		{
			case OkLevel:
			{
				nmeasures++;
			};
			break;
			case AlarmLevel:
			{
				nmeasures++;
				nalarms++;
				alarmtime =updated;
			};
			break;
			case WarningLevel:
			{
				nmeasures++;
				nwarnings++;
			};
			break;
			case FailureLevel:
			{
				failtime =updated;
			};
			break;
			default:
			break;                      
		};
};

int SamplePoint::UpdateTag(const QString &name, IECValue value, QString Type) 
{
		//IT_IT("SamplePoint::UpdateTag");
		
		int state = OkLevel;
		//
		TagDict::iterator i = Tags.find(name);
		if(i == Tags.end())
		{
			assert(0);
			
			//TagDict::value_type pr(name,TagItem());
			//Tags.insert(pr);
			//i = Tags.find(name); // look it up

			//QString err = QString(name) + QString(" not found in Tags");

			//QSLogEvent("Monitor", name);
		}

		//set alarm state

		if(Type == QString(TYPE_M_ME_NA_1) ||
			Type == QString(TYPE_M_ME_NB_1) ||
			Type == QString(TYPE_M_ME_NC_1) ||
			Type == QString(TYPE_M_ME_TD_1) ||
			Type == QString(TYPE_M_ME_TE_1) ||
			Type == QString(TYPE_M_ME_TF_1) ||
			Type == QString(TYPE_M_IT_NA_1) ||
			Type == QString(TYPE_M_IT_TB_1)	) //Analog or Counter
		{
			if((*i).second.UpperAlarm.Check(value,true))
			{
				if(Retriggerable || (AlarmState != AlarmLevel))
				{
					// log the alarm
					QSLogAlarm(Name, QObject::tr("Upper Threshold Alarm ") + name + " " + 
					QString::number(get_value_iec_value(value)) + 

					QObject::tr(":Limit:") + QString::number((*i).second.UpperAlarm.Limit));
				}
				
				state = AlarmLevel;
			} 
			else if((*i).second.UpperWarning.Check(value,true))
			{
				state = WarningLevel;
			}
			//
			if((*i).second.LowerAlarm.Check(value,false))
			{
				if(Retriggerable || (AlarmState != AlarmLevel))
				{
					// log the alarm
					QSLogAlarm(Name, QObject::tr("Lower Threshold Alarm ") + name + " " +
					QString::number(get_value_iec_value(value)) + 
					QObject::tr(":Limit:") + QString::number((*i).second.LowerAlarm.Limit));
				} 
				
				state = AlarmLevel;
			}
			else if((*i).second.LowerWarning.Check(value,false))
			{
				state = WarningLevel;
			}
		}
		else if(Type == TYPE_M_SP_TB_1 || Type == TYPE_M_SP_NA_1 || 
			    Type == TYPE_M_DP_TB_1 || Type == TYPE_M_DP_NA_1) //Digital single point or Double point
		{
			if((*i).second.UpperAlarm.Enabled)
			{
				if(get_value_iec_value(value) == (*i).second.UpperAlarm.Limit)
				{
					state = AlarmLevel;

					// log the alarm
					QSLogAlarm(Name, QObject::tr("Alarm ") + name + " " + 
					QString::number(get_value_iec_value(value)) + 
					QObject::tr(":Alarm for:") + QString::number((*i).second.UpperAlarm.Limit));
				}
				else
				{
					state = OkLevel;
				}
			}
		}
		//
		//
		(*i).second.value   = value;   // update the entry
		(*i).second.stats  += get_value_iec_value(value);  // update the stats
		
		if(fOvState)
		{
			fOvState = false;
			(*i).second.state = OldState;
			AlarmState = OldState;
			state = OldState;
		}
		else
		{
			//////////////31-10-2010///quality code management added//////////
			if(get_quality_of_iec_value(value))
			{
				state = FailureLevel;
			}
			//////////////31-10-2010///quality code management added//////////
						
			(*i).second.state   = state; // set the state
			
			if(AlarmState < state) AlarmState = state;
		}

		//////////////31-10-2010///time stamp management added//////////////////
		struct cp56time2a t1,t2;

		memset(&t1, 0x00, sizeof(struct cp56time2a));

		t2 = get_time_of_iec_value(value);

		int rc;
		rc = memcmp(&t2, &t1, sizeof(struct cp56time2a));

		if(rc == 0)
		{
			//if time stamp IS NOT available from driver, get current time
//
			(*i).second.updated = QDateTime::currentDateTime(); // and update time
		}
		else
		{
			//if time stamp IS available from driver

			__int64 epoch_in_ms = Epoch_in_millisec_from_cp56time2a(&t2);
			QString isodate = GetIsoDateString_from_epoch_in_millisec(epoch_in_ms);

			//printf("%s\n", (const char*)isodate);
			(*i).second.updated = IsoDateMsToQDateTime(isodate);
		}
		//////////////////////end 31-10-2010//time stamp management added/////

		(*i).second.changed = true;  // mark as changed
		//
		//
		return state;
};


//globals
QSEXPORT struct scada_point scada_db[MAX_SCADA_POINTS];
