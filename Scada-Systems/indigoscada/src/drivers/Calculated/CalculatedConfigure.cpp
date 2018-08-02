/**********************************************************************
--- Qt Architect generated file ---
File: CalculatedConfigure.cpp
Last generated: Mon Sep 11 13:54:09 2000
*********************************************************************/
#include "CalculatedConfigure.h"
#define Inherited CalculatedConfigureData
#include "Calculated.h"
#include <qtextstream.h>

CalculatedConfigure::CalculatedConfigure
(
QWidget* parent,
const char* name,
const char* receipe
)
:
Inherited( parent, name ), Receipe(receipe),fNew(0)
{
	setCaption(tr("Calculated Values"));
	Name->setText(name);
	connect (GetConfigureDb (),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (QueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the database
	if(Receipe == "(default)")
	{ 
		QString pc = "select * from PROPS where SKEY='" + QString(name) + "' and IKEY='(default)';"; 
		// get the properties SKEY = unit name IKEY = receipe name
		GetConfigureDb()->DoExec(this,pc,tItem);
	}
	else
	{
		QString pc = 
		"select * from PROPS where SKEY='" + QString(name) + 
		"' and (IKEY='(default)' or IKEY='"+ Receipe + "') order by IKEY desc;"; 
		// get the properties SKEY = unit name IKEY = receipe name
		GetConfigureDb()->DoExec(this,pc,tItem);
		// not the default so disable the constant bits
	};
}
CalculatedConfigure::~CalculatedConfigure()
{
}
void CalculatedConfigure::OkClicked()
{
	QString s;
	QTextOStream os(&s);
	os << ReCalculateInterval->text();         
	//
	QString cmd = "delete from PROPS where SKEY='" + Name->text() + "' and IKEY='" + Receipe + "';";
	GetConfigureDb()->DoExec(0,cmd,0); // delete the old value
	cmd = "insert into PROPS values('" + Name->text() + "','" + Receipe + "','" + s + "');";
	GetConfigureDb()->DoExec(0,cmd,0);
	QSAuditTrail(this,caption(), tr("Edited:") + Name->text());

	Calculated::pDriver->CreateNewUnit(this, Name->text());
	//
	accept();
}

void CalculatedConfigure::QueryResponse (QObject *p, const QString &c, int id, QObject*) // handles database responses
{
	if(p != this) return;
	switch(id)
	{
		case tItem: // properties for the unit / receipe 
		{
			if(GetConfigureDb()->GetNumberResults() > 0)
			{
				QString s = GetConfigureDb()->GetString("DVAL");
				QTextIStream is(&s);
				int interval;
				is >> interval; // get the configuration
				ReCalculateInterval->setValue(interval);
			}
			else
			{
				fNew = true;
			};
		};
		break;
		default:
		break;
	};
};
void CalculatedConfigure::Help()
{
	QSHelp("Calculated");
};

