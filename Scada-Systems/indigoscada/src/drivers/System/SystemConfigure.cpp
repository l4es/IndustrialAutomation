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
/**********************************************************************
--- Qt Architect generated file ---
File: SystemConfigure.cpp
Last generated: Fri Sep 8 16:30:38 2000
*********************************************************************/
#include "SystemConfigure.h"
#include "System.h"
#include <qtextstream.h>
#define Inherited SystemConfigureData
SystemConfigure::SystemConfigure
(
QWidget* parent,
const char* name
)
:
Inherited( parent, name ),Name(name)
{
	//
	setCaption(tr("System Monitor Configure"));
	//
	connect (GetConfigureDb (),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (QueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the database
	//
	// This driver does not have a receipe dependenance at the moment - may change though
	//
	QString pc = "select * from PROPS where SKEY='SystemMonitor' and IKEY='(default)';"; 
	// get the properties SKEY = unit name IKEY = receipe name
	GetConfigureDb()->DoExec(this,pc,tItem);
	QWidget *pL[] =
	{
		MinDiskSpace,
		Drive,
		0
	};
	SetTabOrder (this, pL);
}
SystemConfigure::~SystemConfigure()
{
}
void SystemConfigure::OkClicked()
{
	// Save Properties
	QString s;
	QTextOStream os(&s);
	os << MinDiskSpace->text() << " " << Drive->currentText();           // the node id
	//
	QString cmd = "delete from PROPS where SKEY='SystemMonitor' and IKEY='(default)';";
	GetConfigureDb()->DoExec(0,cmd,0); // delete the old value
	cmd = "insert into PROPS values('SystemMonitor','(default)','" + s + "');";
	GetConfigureDb()->DoExec(0,cmd,0);

	System::pDriver->CreateNewUnit(this,Name);  
	
	QSAuditTrail(this,caption(), tr("Edited"));
	accept();
}
void SystemConfigure::QueryResponse (QObject *p, const QString &c, int id, QObject*caller) // handles database responses
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
				int minDisk;
				QString drive;
				is >> minDisk >> drive; // get the configuration
				MinDiskSpace->setValue(minDisk);
				SetComboItem(Drive,drive);
			};
		};
		break;
		default:
		break;
	};
};
void SystemConfigure::Help()
{
	QSHelp("SystemConfigure");
};

