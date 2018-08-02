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
*Header For: Alarm Display Window
*Purpose: this displays current alarm groups and alarm states
The alarm display shows on the left the alarm groups. On the right the alarm status of points
belonging to the selected alarm group (or All)
*/

#include "alarmdisplay.h"
#include "main.h"
#include "messages.h"
#include "display.h"
#include "inspect.h"
#include "AckAlarmDlg.h"
#include "dispatch.h"
#include "IndentedTrace.h"

/*
*Function:AlarmDisplay
*Inputs:none
*Outputs:none
*Returns:none
*/
AlarmDisplay::AlarmDisplay(QWidget *parent) : QSplitter(parent) ,pTimer(new QTimer(this))
{
	
	IT_IT("AlarmDisplay::AlarmDisplay");

	pAlarmGroups = new  StatusListView(this); // we now have the list views
	pAlarmPoints = new  StatusListView(this); //
	Status.Create(this); // status pane
	//
	pAlarmGroups->setItemMargin(5);
	pAlarmPoints->setItemMargin(5);
	// 
	pAlarmGroups->addColumn(tr("Name"));
	pAlarmGroups->addColumn(tr("Comment"));
	pAlarmGroups->addColumn(tr("Update"));
	//
	pAlarmPoints->addColumn(tr("Name"));
	pAlarmPoints->addColumn(tr("Update"));
	//
	//pAlarmGroups->setSorting(-1);
	connect(pAlarmGroups,SIGNAL(selectionChanged()),SLOT(SelChanged())); 
	connect(pAlarmPoints,SIGNAL(selectionChanged()),SLOT(PointSelChanged())); 
	//
	// handle menu request (right click)
	connect(pAlarmGroups,SIGNAL(rightButtonClicked(QListViewItem *, const QPoint &, int)),
	SLOT(GroupMenu(QListViewItem *, const QPoint &, int))); 
	//
	connect(pAlarmPoints,SIGNAL(rightButtonClicked(QListViewItem *, const QPoint &, int)),
	SLOT(PointMenu(QListViewItem *, const QPoint &, int))); 
	//
	connect (GetCurrentDb (),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (CurrentQueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the database
	//  
	// connect to the configuration database 
	// 
	connect (GetConfigureDb (),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (QueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the database
	//  
	// connect to the dispatcher 
	//
	connect (GetDispatcher(),
	SIGNAL (ReceivedNotify(int, const char *)), this,
	SLOT (ReceivedNotify(int, const char *)));	
	
	//
	// get the alarm groups
	// we could join thestate table with the config table, but ....
	// they are in different databases and the current state db does not necessarily exist
	GetConfigureDb()->DoExec(this,QString("select NAME,COMMENT from ALARMGROUP order by NAME asc;"),tList); // start by getting everything
	// 
	connect (pTimer, SIGNAL (timeout ()), this, SLOT (DoSelChange ()));	// wire up the item selection timer
	connect(qApp->mainWidget(),SIGNAL(Restart()),this,SLOT(Restart()));   // we want know about restarts
	//
	//QTimer *pTicks = new QTimer(this);
	//connect (pTicks, SIGNAL (timeout ()), this, SLOT (Flash()));	// wire up the item selection timer
	//pTicks->start(2000);
	//
};
/*
*Function:Flash
*Inputs:none
*Outputs:none
*Returns:none
*/
void AlarmDisplay::Flash()
{
	IT_IT("AlarmDisplay::Flash");
	
	QListViewItemIterator st (pAlarmGroups);
	for (; st.current (); st++)
	{
		StateListItem *p = (StateListItem *)(st.current ()); 
		p->Toggle();
		if(p->GetAckState())
		{
			pAlarmGroups->repaintItem(p);
		};
	};
	QListViewItemIterator pt (pAlarmPoints);
	for (; pt.current (); pt++)
	{
		StateListItem *p = (StateListItem *)(pt.current ()); 
		p->Toggle();
		if(p->GetAckState())
		{
			pAlarmPoints->repaintItem(p);
		};
	};
	pAlarmPoints->triggerUpdate();        
	pAlarmGroups->triggerUpdate();        
};
/*-Function: SelChanged
*Inputs:
*Outputs:none
*Returns:none
*/
void AlarmDisplay::SelChanged() // handle the selection
{
	IT_IT("AlarmDisplay::SelChanged");
	
	pTimer->stop(); 
	pTimer->start (100, TRUE);	// after inactivity fetch the record - we wan to avoid too much activity
};
/*
*Function: DoSelChange
*action the change - delayed by 100ms to avoid to many dud fetches
*Inputs:none
*Outputs:none
*Returns:none
*/
void AlarmDisplay::DoSelChange()
{
	IT_IT("AlarmDisplay::DoSelChange");
	
	if(pAlarmGroups->currentItem())
	{
		//
		// issue the transaction for the current alarm group status
		// 
		QListViewItem *p = pAlarmGroups->currentItem();
		CurrentGroup = p->text(0);
		pAlarmPoints->setColumnText(0,CurrentGroup);
		//
		// get the alarm group
		// 
		QString cmd = "select * from ALM_GRP_STATE where NAME='" + CurrentGroup + "' order by STATE desc;"; // order by alarm state
		//
		GetCurrentDb()->DoExec(this,cmd,tGroup);        
	};
};
/*-Function:ReceivedNotify
*Inputs:current value db notification
*Outputs:none
*Returns:none
*/
void AlarmDisplay::ReceivedNotify(int ntf, const char * data) 
{
	IT_IT("AlarmDisplay::ReceivedNotify");
	
	if(ntf == NotificationEvent::ALARMGROUP_NOTIFY)
	{
		IT_COMMENT("ALARMGROUP_NOTIFY");
		
		// request from the current values database the alarm status of the group 
		GetCurrentDb()->DoExec(this,"select * from ALM_GRP order by STATE desc;",tList); // order by alarm state  
		if(!CurrentGroup.isEmpty())
		{
			QString cmd = "select * from ALM_GRP_STATE where NAME='" + CurrentGroup + "' order by STATE desc;"; // order by alarm state
			GetCurrentDb()->DoExec(this,cmd,tGroup);
		}
	}
};
/*-Function:ConfigDbComplete
*notification that the operation has completed
*Inputs:none
*Outputs:none
*Returns:none
*/
void AlarmDisplay::QueryResponse (QObject *p , const QString &c, int id, QObject*caller) // notify transaction done on configuration
{
	if(p != this) return;

	IT_IT("AlarmDisplay::QueryResponse");

	switch(id)
	{
		case tList:
		{
			// fill the names of the alarm groups
			pAlarmGroups->clear();
			int n = GetConfigureDb()->GetNumberResults();
			if(n)
			{
				for(int i = 0; i < n; i++, GetConfigureDb()->FetchNext())
				{
					// we have the name and comment
					(void)(new StateListItem(pAlarmGroups,GetConfigureDb()->GetString("NAME"),
					UndoEscapeSQLText(GetConfigureDb()->GetString("COMMENT")) ));
				};
			};
			// request from the current values database the alarm status of the group 
			GetCurrentDb()->DoExec(this,"select * from ALM_GRP order by NAME asc;",tList); 
			// 
		};
		break;
		case tItem:
		{
			Status.UpdateHeader();
		};
		break;
		case tTagLimits:
		{
			Status.UpdateTagsLimits();
		};
		break;
		default:
		break;
	};
};
/*-Function:CurrentDbComplete
*Inputs:op on current values db has completed
*Outputs:none
*Returns:none
*/
void AlarmDisplay::CurrentQueryResponse (QObject *p, const QString &, int id, QObject* caller) // notify transaction done on current values
{
	if(p != this) return;

	IT_IT("AlarmDisplay::CurrentQueryResponse");

	switch(id)
	{
		case tTags:
		{
			Status.UpdateTagsPane();
		};
		break;
		case tSamplePoint:
		{
			Status.UpdateSamplePoint();
		};
		break;
		case tList: // we have the list 
		{
			int n = GetCurrentDb()->GetNumberResults();
			if(n)
			{
				for(int i = 0; i < n; i++,GetCurrentDb()->FetchNext())
				{
					//
					QString s = GetCurrentDb()->GetString("NAME");
					//
					// look for the entry
					// 
					QListViewItemIterator st (pAlarmGroups);
					for (; st.current (); st++)
					{
						StateListItem *p = (StateListItem *)(st.current ()); 
						if(p->text(0) == s)
						{
							// set the alarm state
							p->SetState(GetCurrentDb()->GetInt("STATE")); // set the alarm state
							p->setText(2,GetCurrentDb()->GetIsoDateString("UPDTIME")); // set the update time
							//p->SetAckState(GetCurrentDb()->GetInt("ACKSTATE"));
							break;
						};
					}; 
				};
				//
				if(CurrentGroup.isEmpty())
				{
					Status.SetName(QString::null);
					pAlarmGroups->setCurrentItem(pAlarmGroups->firstChild()); // select the first item
				}
				else
				{
					// set the selection to the current group
					SetListViewItem(pAlarmGroups,CurrentGroup); // select it
				};
				DoSelChange();
			};
			pAlarmGroups->resize(pAlarmGroups->size());
			pAlarmGroups->triggerUpdate();
		};
		break;
		case tGroup:
		{
			pAlarmPoints->clear(); // clear the list box
			//
			int n = GetCurrentDb()->GetNumberResults();
			if(n)
			{
				int ackState = 0;
				for(int i = 0; i < n; i++,GetCurrentDb()->FetchNext())
				{
					// we have the name and state
					StateListItem *p = new StateListItem(pAlarmPoints,GetCurrentDb()->GetString("SPNAME"),
					GetCurrentDb()->GetIsoDateString("UPDTIME"));
					p->SetState(GetCurrentDb()->GetInt("STATE")); // set the colour
					p->SetAckState(GetCurrentDb()->GetInt("ACKSTATE"));
					ackState |= GetCurrentDb()->GetInt("ACKSTATE");
				};
				//
				{
					StateListItem *p = (StateListItem *) pAlarmGroups->currentItem();				
					if(p)
					{
						p->SetAckState(ackState);
					};
				};
				//
				if(Status.GetName().isEmpty())
				{ 
					pAlarmPoints->setCurrentItem(pAlarmPoints->firstChild()); // select the first item
				}
				else
				{
					SetListViewItem(pAlarmPoints,Status.GetName()); // this all works out because if the sample point is missing we default to the first point
				};
				PointSelChanged();
			};
			pAlarmPoints->resize(pAlarmPoints->size());
			pAlarmPoints->triggerUpdate();        
		};
		break;
		default:
		break;
	};
};
/*-Function: PointMenu
*Inputs:selected sample point item
*Outputs:none
*Returns:none
*/
void AlarmDisplay::PointMenu(QListViewItem *p, const QPoint &pt, int) // an item has been right clicked - open the point menu
{
	IT_IT("AlarmDisplay::PointMenu");
	
	if(!p) return;
	StateListItem *pI = ( StateListItem *)p;
	InspectMenu(pAlarmPoints,pI->text(0),pI->GetAckState());
};
/*-Function: GroupMenu
*Inputs:alarm group item 
*Outputs:none
*Returns:none
*/
void AlarmDisplay::GroupMenu(QListViewItem *p, const QPoint &pt, int) // a group alarm has been right clicked - open its menu
{
	IT_IT("AlarmDisplay::GroupMenu");
	
	if(!p) return;
	if(GetUserDetails ().privs &  PRIVS_ACK_ALARMS )
	{
		StateListItem *pI = ( StateListItem *)p;
		if(pI && pI->GetAckState())
		{
			//
			// Build the popup menu
			QPopupMenu m(pAlarmGroups);
			m.insertItem(tr("Acknowledge Alarms in Group..."),1); // acknowledge alarm
			m.insertItem(tr("Cancel"),-1);
			//
			switch(m.exec(QCursor::pos()))
			{
				case 1: // acknowledge the alarms for all points in alarm in this group
				{
					// update the acknowledge state for all unacknowledged alarm points
					// in the selected group
					AckAlarmDlg dlg(qApp->mainWidget());
					dlg.SetHeading(tr("Acknowledge All Alarms In Group ") + pI->text(0));
					//
					if(dlg.exec())
					{
						QString cmd = 
						"update CVAL_DB set ACKFLAG=0,ACKTIME=" +  DATETIME_NOW + " where NAME in (select SPNAME from ALM_GRP_STATE where ACKSTATE=1 and NAME='"+pI->text(0)+"');";
						GetCurrentDb()->DoExec(0,cmd,0);
						//
						// now generate the acknowledge handling for alarm groups
						cmd = "update ALM_GRP_STATE set ACKSTATE=0 where NAME='"+pI->text(0)+"';" ; // update all members of the alarm groups
						GetCurrentDb()->DoExec(0,cmd,0); // do the transaction
						//
						// recalculate the ackstate of the group
						cmd = "update ALM_GRP set ACKSTATE=0 where NAME ='" + pI->text(0) + "';";
						GetCurrentDb()->DoExec(0,cmd,0); // do the transaction
						//    
						//broadcast
						//GetDispatcher()->DoExec(NotificationEvent::ACK_NOTIFY);
						GetDispatcher()->DoExec(NotificationEvent::ACK_NOTIFY, ACK_ALL_TOKEN);
						GetDispatcher()->DoExec(NotificationEvent::ALARMGROUP_NOTIFY);
						GetDispatcher()->DoExec(NotificationEvent::CURRENT_NOTIFY);
						//
						QSLogAlarm("Client",  GetUserDetails().Name + tr(" has acknowledged all alarms in group ") 
						+ pI->text(0) + ":" + dlg.GetComment());
						//
						pI->SetAckState(0); // clear the alrm from the selected item
						//
					}
					//
				}
				break;
				default:
				break;
			}
		}
	}
}
/*
*Function: Restart
*the system has restarted reload everything
*Inputs:none
*Outputs:none
*Returns:none
*/
void AlarmDisplay::Restart()
{
	IT_IT("AlarmDisplay::Restart");
	
	pAlarmPoints->clear(); // clear the list box
	pAlarmGroups->clear();
	CurrentGroup = QString::null;
	Status.Clear();
	GetConfigureDb()->DoExec(this,QString("select NAME,COMMENT from ALARMGROUP order by NAME asc;"),tList); // start by getting everything
};
/*
*Function:PointSelChanged
*Inputs:none
*Outputs:none
*Returns:none
*/
void AlarmDisplay::PointSelChanged() // handle the selection
{
	IT_IT("AlarmDisplay::PointSelChanged");
	
	QListViewItem *p = pAlarmPoints->currentItem();
	Status.SetName(p->text(0));
	//
	QString cmd = "select * from SAMPLE where NAME='"+ p->text(0) +"';"; // update the tag values for the selected SP
	GetConfigureDb()->DoExec(this,cmd,tItem);
	cmd = "select * from TAGS_DB where NAME='"+  p->text(0) +"';"; // update the tag values for the selected SP
	GetCurrentDb()->DoExec(this,cmd,tTags);
	cmd = "select * from CVAL_DB where NAME='"+  p->text(0) +"';"; // update the tag values for the selected SP
	GetCurrentDb()->DoExec(this,cmd,tSamplePoint);
	// get the alarm limits
	cmd = "select * from TAGS where NAME='"+p->text(0)+"' and RECEIPE='"+GetReceipeName()+"';";
	GetConfigureDb()->DoExec(this,cmd,tTagLimits);
};

