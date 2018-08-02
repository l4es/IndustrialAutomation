/**********************************************************************
--- Qt Architect generated file ---
File: ActionConfigure.cpp
Last generated: Tue May 16 10:23:37 2000
*********************************************************************/
#include "main.h"
#include "dbase.h"
#include "common.h"
#include "ActionConfigure.h"
#include "helper_functions.h"

#define Inherited ActionConfigureData
ActionConfigure::ActionConfigure
(
QWidget* parent,
const char* name
)
:
Inherited( parent, name ) , SpName(name)
{
	setCaption(tr("Action Configuration"));
	Name->setText(SpName);
	//
	connect (GetConfigureDb (),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (QueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the database
	//
	QString cmd = "select * from ACTIONS where NAME='"+SpName+"';";
	GetConfigureDb()->DoExec(this,cmd,tItem);
	//
	MeasureAction->setMaxLength(MAX_LENGHT_OF_STRING);
	AlarmAction->setMaxLength(MAX_LENGHT_OF_STRING);
	//
}
ActionConfigure::~ActionConfigure()
{
}
void ActionConfigure::Help()
{
	QSHelp("ActionConfigure");
}
void ActionConfigure::OkClicked()
{
	QString cmd = "delete from ACTIONS where NAME='"+SpName+"';";
	GetConfigureDb()->DoExec(0,cmd,0);
	//
	GetConfigureDb ()->ClearRecord ();	// clear the record
	GetConfigureDb ()->AddToRecord ("MEASURE", MeasureAction->text ()); //add the actions
	GetConfigureDb ()->AddToRecord ("ALARM", AlarmAction->text ());
	GetConfigureDb ()->AddToRecord ("NAME", SpName);
	//
	cmd = GetConfigureDb ()->Insert ("ACTIONS");	// generate the update record
	GetConfigureDb()->DoExec(0,cmd,0);
	accept();
	//
	DOAUDIT(tr("Updated Actions For ") + SpName);
}


void ActionConfigure::QueryResponse (QObject *p, const QString &, int State, QObject*caller) // notify transaction completerequestrt needs the name record addedaphpage int2, samples text);;dtod text,enddate text);
{
	if(p != this) return;
	switch (State)
	{
		case tItem:
		{
			MeasureAction->setText(GetConfigureDb()->GetString("MEASURE"));
			AlarmAction->setText(GetConfigureDb()->GetString("ALARM"));                      
		};
		break;
		default:
		break;
	};
};

