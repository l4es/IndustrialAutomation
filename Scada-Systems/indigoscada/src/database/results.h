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

#ifndef include_results_hpp 
#define include_results_hpp 
#include "common.h"
#include "smplstat.h"
#include "realtimedb.h"
#include "historicdb.h"
#include "helper_functions.h"
//#include <sptypes.h>
#include "mythread.h"

extern "C"
{ 

typedef struct scada_point SC_PT;
//begin variabili e funzioni condivise con l'interprete EiC

#include "scada_point.h"

#define MAX_SCADA_POINTS 5000

QSEXPORT extern struct scada_point scada_db[MAX_SCADA_POINTS];
	
/** mutex for thread to stop the threads hitting data at the same time. */
QSEXPORT extern ins_mutex_t * mut;

};

#include "iec104types.h"

////////////////////////////////////////////////////////////////////////////////////
//measurement_type
/*
enum m_type
{
	M_SP_NA_1 =		1,
	M_DP_NA_1 =		3,
	M_BO_NA_1 =		7,
	M_ME_NA_1 =		9,
	M_ME_NB_1 =		11,
	M_ME_NC_1 =		13,
	M_SP_TB_1 =		30,
	M_DP_TB_1 =		31,
	M_BO_TB_1 =		33,
	M_ME_TD_1 =		34,
	M_ME_TE_1 =		35,
	M_ME_TF_1 =		36,
	M_ME_TN_1 =		150, //custom type
	M_ME_TO_1 =		151,
	M_ME_TP_1 =		152,
	M_ME_TQ_1 =		153,
	M_ME_TR_1 =		154,
	M_ME_TS_1 =		155,
	M_ME_TT_1 =		156
};
*/

struct IECValue // a measured result
{
	QString tag; // the tag name eg value, 0.5
	unsigned char type; //the type of measurement
	//from single bit with quality to value at 64 bits
	union {
		iec_type1 v1_q;
		iec_type3 v3_q;
		iec_type7 v7_q;
		iec_type9 v9_q;
		iec_type11 v11_q;
		iec_type13 v13_q;
		iec_type30 v30_q;
		iec_type33 v33_q;
		iec_type34 v34_q;
		iec_type35 v35_q;
		iec_type36 v36_q;
		iec_type37 v37_q;
		is_type150 v150_q;
		is_type151 v151_q;
		is_type152 v152_q;
		is_type153 v153_q;
		is_type154 v154_q;
		is_type155 v155_q;
		is_type156 v156_q;
		/*
		unsigned char uval8;						//0 to           255 
		char val8;									//-128 to           127
		short int val16; //measurand with sign         -32,768 to        32,767 
		unsigned short int uval16; //measurand unsigned 0 to        65,535
		unsigned int uval32;// measurand unsigned 0 to 4,294,967,295
		int    val32; //32 bit signed -2,147,483,648 to 2,147,483,647
		float  fval32; //32 bit floating point
		__int64 val64;
		unsigned __int64 uval64;
		double fval64; //64 bit floating point 
		double value; // the value 
		*/
	};

	IECValue(const QString &s= "", void* v = NULL, unsigned char tp = 0) : tag(s), type(tp)
	{
		/*
		memset(&v1_q, 0x00, sizeof(v1_q));
		memset(&v3_q, 0x00, sizeof(v3_q));
		memset(&v7_q, 0x00, sizeof(v7_q));
		memset(&v9_q, 0x00, sizeof(v9_q));
		memset(&v11_q, 0x00, sizeof(v11_q));
		memset(&v13_q, 0x00, sizeof(v13_q));
		memset(&v30_q, 0x00, sizeof(v30_q));
		memset(&v33_q, 0x00, sizeof(v33_q));
		memset(&v34_q, 0x00, sizeof(v34_q));
		memset(&v35_q, 0x00, sizeof(v35_q));
		memset(&v36_q, 0x00, sizeof(v36_q));
		memset(&v150_q, 0x00, sizeof(v150_q));
		memset(&v151_q, 0x00, sizeof(v151_q));
		memset(&v152_q, 0x00, sizeof(v152_q));
		memset(&v153_q, 0x00, sizeof(v153_q));
		memset(&v154_q, 0x00, sizeof(v154_q));
		memset(&v155_q, 0x00, sizeof(v155_q));
		*/
		memset(&v150_q, 0x00, sizeof(v150_q));

		if(v == NULL)
		{
			v150_q.mv = 0.0;
			v150_q.iv = 0;
			v = (void *)&v150_q;
		}

		switch(tp)
		{
			case M_SP_NA_1:
			v1_q = *(iec_type1*)v;
			break;
			case M_DP_NA_1:
			v3_q = *(iec_type3*)v;
			break;
			case M_BO_NA_1:
			v7_q = *(iec_type7*)v;
			break;
			case M_ME_NA_1:
			v9_q = *(iec_type9*)v;
			break;
			case M_ME_NB_1:
			v11_q = *(iec_type11*)v;
			break;
			case M_ME_NC_1:
			v13_q = *(iec_type13*)v;
			break;
			case M_SP_TB_1:
			v30_q = *(iec_type30*)v;
			break;
			case M_BO_TB_1:
			v33_q = *(iec_type33*)v;
			break;
			case M_ME_TD_1:
			v34_q =  *(iec_type34*)v;
			break;
			case M_ME_TE_1:
			v35_q =  *(iec_type35*)v;
			break;
			case M_ME_TF_1:
			v36_q =  *(iec_type36*)v;
			break;
			case M_IT_TB_1:
			v37_q =  *(iec_type37*)v;
			break;
			case M_ME_TN_1:
			v150_q =  *(is_type150*)v;
			break;
            case M_ME_TQ_1:
            v153_q =  *(is_type153*)v;
            break;
			default:
			v150_q.mv = *((double*)v);
			type = 150;
			break;
		}
	};

	IECValue(const IECValue &s) : tag(s.tag), type(s.type) 
	{
		switch(s.type)
		{
			case M_SP_NA_1:
			v1_q = s.v1_q;
			break;
			case M_DP_NA_1:
			v3_q = s.v3_q;
			break;
			case M_BO_NA_1:
			v7_q = s.v7_q;
			break;
			case M_ME_NA_1:
			v9_q = s.v9_q;
			break;
			case M_ME_NB_1:
			v11_q = s.v11_q;
			break;
			case M_ME_NC_1:
			v13_q = s.v13_q;
			break;
			case M_SP_TB_1:
			v30_q = s.v30_q;
			break;
			case M_BO_TB_1:
			v33_q = s.v33_q;
			break;
			case M_ME_TD_1:
			v34_q = s.v34_q;
			break;
			case M_ME_TE_1:
			v35_q = s.v35_q;
			break;
			case M_ME_TF_1:
			v36_q = s.v36_q;
			break;
			case M_IT_TB_1:
			v37_q = s.v37_q;
			break;
			case M_ME_TN_1:
			v150_q = s.v150_q;
			break;
            case M_ME_TQ_1:
            v153_q = s.v153_q;
			break;
			default:
			v150_q = s.v150_q;
			break;
		}
	}; 
};


extern "C"
{
	QSEXPORT double get_value_iec_value(IECValue &v);
	QSEXPORT struct cp56time2a 	get_time_of_iec_value(IECValue &v);
	QSEXPORT void set_value_iec_value(IECValue &v, double val);
	QSEXPORT __int64 Epoch_in_millisec_from_cp56time2a(const struct cp56time2a* time);
};

QSEXPORT QString GetIsoDateString_from_epoch_in_millisec(__int64 epoch_in_millisec);

typedef std::vector<IECValue> IECValueList; 

struct AlarmLimit // a tag's alarm limits
{
	bool Enabled;

	double Limit;

	AlarmLimit() : Enabled(0),Limit(0) {};

	bool Check(IECValue v, bool dir = true) // true if upper, false if lower
	{
		if(Enabled)
		{
			if(dir)
			{
				if(get_value_iec_value(v) > Limit) return true; 
			}
			else
			{
				if(get_value_iec_value(v) < Limit) return true;
			};
		};
		return false;
	};
};

struct TagItem // current tag value
{
	AlarmLimit UpperAlarm;
	AlarmLimit UpperWarning;
	AlarmLimit LowerWarning;
	AlarmLimit LowerAlarm;
	bool changed;  // set on a new value
	//double value; // current value
	IECValue value; //prova del 07-12-09
	int state; // current state
	QDateTime updated; // when it was updated
	SampleStatistic stats; // the stats
	bool enabled;	//APA added on 29-12-01

	TagItem() : changed(0), value(0),state(0),enabled(false) 
	{
	};
};

class QSEXPORT SamplePoint            // the sample point configuration - we need to cache these
{
	public:	
	QString  Name;            // Sample Point name
	bool     fChanged;        // has the sample point changed
	bool	 fOvState;		  // APA added 06-03-04, has the state been setted
	bool     fAckTriggered;   // has an ack been triggered
	int      AlarmState;      // the alarm state
	bool     Fileable;        // do we file this point's data
	int      OldState;        // the previous larm state
	//
	unsigned AlarmThreshold;  // do we need more than one alarm event to trigger an alarm
	unsigned AlarmEvents;     // alarm events
	QString  Unit;            // the unit associated with this sample point
	QString  Type;            // the type of the sample point
	QString InputIndex;       // the input index
	bool Retriggerable;       // are the alarms retriggerable - if in alarm do we trigger ack and alarm events on each alarm
	// or do we wait for it to exit alarm
	QString Comment;          // the last comment string (for current value)
	QDateTime updated;        // last update time
	//
	// statistics
	int nalarms;         // number of alarms
	int nwarnings;       // number of warnings
	int nmeasures;       // number of measures
	QDateTime alarmtime; // last alarm
	QDateTime failtime;  // last failure
	//
	typedef std::map<QString, TagItem, std::less<QString> > TagDict;
	TagDict Tags; // the tag values
	//
	//
	SamplePoint(const QString &name) : Name(name),
	fChanged(0),fOvState(0),fAckTriggered(0),AlarmState(0),
	Fileable(0),OldState(0),AlarmThreshold(0),AlarmEvents(0),
	Retriggerable(0),nalarms(0),nwarnings(0),nmeasures(0),db_idx(0)
	{
		alarmtime.setTime_t(0);
		failtime.setTime_t(0);
		mut = ins_mutex_new();
	};

	~SamplePoint()
	{
	}

	//Il vettore scada_db e' ordinato come il risultato della query:
	//GetCurrentDb()->DoExec(this,"select * from TAGS_DB;",tTagsCurrent);
	int db_idx; //indice nel vettore scada_db

	//
	// copy constructor
	// 
	SamplePoint(const SamplePoint &s) : 
	Name(s.Name),
	fChanged(s.fChanged),
	fOvState(s.fOvState),
	fAckTriggered(s.fAckTriggered),
	AlarmState(s.AlarmState),
	Fileable(s.Fileable),
	OldState(s.OldState),
	AlarmThreshold(s.AlarmThreshold),
	AlarmEvents(s.AlarmEvents),
	Retriggerable(s.Retriggerable),
	nalarms(s.nalarms),
	nwarnings(s.nwarnings),
	nmeasures(s.nmeasures)
	{
		alarmtime.setTime_t(0);
		failtime.setTime_t(0);
	};
	//
	//
	//
	void Update(int state, bool ack = false, const QString &c=""); // mark a sample point as updated and ack triggered
	
	int UpdateTag(const QString &name, IECValue value, QString Type); 
};


typedef std::map<QString,SamplePoint, std::less<QString> > SamplePointDictWrap; // dictionary of them


class QSEXPORT Results : public QObject
{
	//
	// Alarm Groups cause a lot of database traffic - so we have to cache them 
	// and then write them out from time to time
	//
	Q_OBJECT
	//
	enum { tCurrentSample = 1 , tCurrentTags, tTags, tTable, tAlarmGroup, tActions,tAlarmActions}; // various transactions
	static QMutex ResultLock;
	//
	//
	void UpdateStart(const QString &name); //  start an update 
	void Update(const QString &name, const QString &tag, IECValue &value); // update a sample point by tag
	void UpdateEnd(const QString &name); // end an update on a sample point
	//
	public:
	//
	class StateDict : public std::map<QString, bool, std::less<QString> >  // state dictionary - alarm group item
	{
		public:
		int AckState; // the acknowledge state of the group
		int State;    // the state of the group
		bool Changed; // has it changed
		StateDict() : AckState(0),State(0),Changed(0){};
	};
	//
	typedef std::map<QString, StateDict, std::less<QString> > GroupDict; // dictionary of dictionaries
//	private:
public:

	static GroupDict Groups; // the alarm groups
	static SamplePointDictWrap EnabledPoints;// this is the list of enabled sample points and current values
	
	public:
	static GroupDict &GetGroups(); // the alarm groups - NB Also Locks 
	static SamplePointDictWrap &GetEnabledPoints();// this is the list of enabled sample points and current values - NB Also Locks
	//
	// responsibility to action the update
	//
	Results(QObject *parent); 
	//     
	~Results() {};
	// queue a new result packet for filing
	void QueueResult(const QString &name, IECValueList &pL); //queue a result
	static void Lock()
	{
		ResultLock.lock(); // lock it		
	}; // wait for mutex to lock
	static void Unlock() // unlock the mutex (assumes we locked it)
	{
		ResultLock.unlock();
	};
	//
	void UpdateSamplePointAlarmGroup(const QString &spname, int state,bool fAck); // update a single sample point in an alarm group
	//
	public slots:
	//
	void ConfigQueryResponse (QObject *,const QString &, int, QObject*);  // handles configuration responses
	//    
};

#endif

