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
*Header For: satuspane implementation
*Purpose: provides single sample point status update
*/

#include "statuspane.h"
#include "smplstat.h"
#include "IndentedTrace.h"
/*
*Function:UpdateTagsPane()
*updates tag values. Given that the current tuples are a set of current tag values for one sample point
*Inputs:none
*Outputs:none
*Returns:none
*/
void  StatusPane::UpdateTagsPane()
{
//	IT_IT("StatusPane::UpdateTagsPane");
	
	bool fHaveCleared = false;  
	int n = GetCurrentDb()->GetNumberResults();
	GetCurrentDb()->GotoBegin();
//	if(!n)  //APA commented
//	{
//		pTagStatus->clear();
//	}
	for(int i = 0; i < n ; i++,GetCurrentDb()->FetchNext())
	{
		if(Name == GetCurrentDb()->GetString("NAME"))
		{
			if(!fHaveCleared)
			{
				pTagStatus->clear();
				fHaveCleared = true;
			};
			// build the report
			//
			SampleStatistic sm;
			sm. set( GetCurrentDb()->GetInt("NVAL"), GetCurrentDb()->GetDouble("SUMVAL"),  GetCurrentDb()->GetDouble("SUM2VAL"),
			GetCurrentDb()->GetDouble("MINVAL"),  GetCurrentDb()->GetDouble("MAXVAL"));
			//
			double v = atof((const char*)(GetCurrentDb()->GetString("VAL")));
			QString val = QString::number(v,'f',2); //two decimal points of precision
			//
			StateListItem * p = new StateListItem(pTagStatus,
			GetCurrentDb()->GetString("TAGNAME"),
			val,
			GetCurrentDb()->GetInt("STATE"),"");
			//
			// set all the other bits
			//
			p->setText(2,GetCurrentDb()->GetString("MINVAL"));
			p->setText(3,GetCurrentDb()->GetString("MAXVAL"));
			p->setText(4,QString::number(sm.mean()));
			p->setText(5,QString::number(sm.stdDev()));
			p->setText(6,GetCurrentDb()->GetIsoDateString("UPDTIME"));
			//  
		};
	};
	pTagStatus->triggerUpdate();
};
/*
*Function:UpdateSamplePoint()
*update the sample point - it is given that the current record is a current value sample status
*Inputs:none
*Outputs:none
*Returns:none
*/
void StatusPane::UpdateSamplePoint()
{  
	IT_IT("StatusPane::UpdateSamplePoint");
	
	int n = GetCurrentDb()->GetNumberResults();
	//
	// status of seleted item
	QDateTime et;
	et.setTime_t(0);
	// form up the point stats
	//
	GetCurrentDb()->GotoBegin();
	for(int i = 0; i < n ; i++, GetCurrentDb()->FetchNext())
	{
		if(Name == GetCurrentDb()->GetString("NAME"))
		{
			pPointStatus->clear();
			int state = GetCurrentDb()->GetInt("STATE");
			//
			// we want to trap out times set to 1/1/70 
			QDateTime t = GetCurrentDb()->GetDateTime("UPDTIME");
			if(t > et)
			{ 
				(void ) new StateListItem(pPointStatus,tr("Last Update"), GetCurrentDb()->GetIsoDateString("UPDTIME"),state);
			};
			t = GetCurrentDb()->GetDateTime("ALMTIME");
			if(t > et)
			{ 
				(void ) new StateListItem(pPointStatus, tr("Last Alarm"), GetCurrentDb()->GetIsoDateString("ALMTIME"),state);
			};
			t = GetCurrentDb()->GetDateTime("FAILTIME");
			if(t > et)
			{ 
				(void ) new StateListItem(pPointStatus,tr("Failure Time"), GetCurrentDb()->GetIsoDateString("FAILTIME"),state);
			};
			t = GetCurrentDb()->GetDateTime("ACKTIME");
			if(t > et)
			{ 
				(void ) new StateListItem(pPointStatus,tr("Last Acknowledged"), GetCurrentDb()->GetIsoDateString("ACKTIME"),state);
			};
			(void ) new StateListItem(pPointStatus,tr("No. Measures"), GetCurrentDb()->GetString("NMEASURE"),state);
			(void ) new StateListItem(pPointStatus,tr("No. Alarms" ),GetCurrentDb()->GetString("NALARM"),state);
			(void ) new StateListItem(pPointStatus,tr("No. Warnings" ), GetCurrentDb()->GetString("NWARNING"),state);
			(void ) new StateListItem(pPointStatus,tr("State"), GetAlarmStateName(GetCurrentDb()->GetInt("STATE")),state);
			(void ) new StateListItem(pPointStatus,tr("Comment"), UndoEscapeSQLText(GetCurrentDb()->GetString("COMMENT")),state);
			//
			if(GetCurrentDb()->GetBool("ACKFLAG"))
			{ 
				(void ) new StateListItem(pPointStatus,"", tr(" Ack. Needed"),state);
			};
			break;
		};
	};
};
/*
*Function:UpdateTagsLimits
*sets the tag limits based on the current config database tuples
*Inputs:none
*Outputs:none
*Returns:none
*/
void StatusPane::UpdateTagsLimits()
{
	IT_IT("StatusPane::UpdateTagsLimits");
	
	pTagAlarms->clear();
	//
	int n = GetConfigureDb()->GetNumberResults();
	GetConfigureDb()->GotoBegin();
	//
	for(int i = 0; i < n ; i++,GetConfigureDb()->FetchNext())
	{
		StateListItem *p =  new StateListItem(pTagAlarms,GetConfigureDb()->GetString("TAG"),QString(""),0);
		//
		if(GetConfigureDb()->GetInt("LAENABLE"))
		{
			p->setText(1,GetConfigureDb()->GetString("LOWERALARM"));
		}
		else
		{
			p->setText(1,tr(NONE_STR));
		};
		//
		if(GetConfigureDb()->GetInt("LWENABLE"))
		{
			p->setText(2,GetConfigureDb()->GetString("LOWERWARN"));
		}
		else
		{
			p->setText(2,tr(NONE_STR));
		};
		//
		if(GetConfigureDb()->GetInt("UWENABLE"))
		{
			p->setText(3,GetConfigureDb()->GetString("UPPERWARN"));
		}
		else
		{
			p->setText(3,tr(NONE_STR));
		};
		//
		if(GetConfigureDb()->GetInt("UAENABLE"))
		{
			p->setText(4,GetConfigureDb()->GetString("UPPERALARM"));
		}
		else
		{
			p->setText(4,tr(NONE_STR));
		};
	};
};

