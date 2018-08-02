/**********************************************************************
--- Qt Architect generated file ---
File: Rfc1006driverConfiguration.cpp
Last generated: Mon May 22 17:14:04 2000
*********************************************************************/
#include "Rfc1006driverConfiguration.h"
#include <qt.h>
#include "rfc1006driver.h"
#define Inherited Rfc1006driverConfigurationData
Rfc1006driverConfiguration::Rfc1006driverConfiguration
(
QWidget* parent,
const char* name,
const char *receipe
)
:
Inherited( parent, name ),Receipe(receipe)
{
	setCaption(tr("Rfc1006driver Configuration"));

	Name->setText(name);

	connect (GetConfigureDb (),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (QueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the database
	// 
	// get the properties for this unit and receipe
	if(Receipe == "(default)")
	{ 
		QString pc = 
		"select * from PROPS where SKEY='" + QString(name) + 
		"' and IKEY='(default)';"; 
		//
		// get the properties SKEY = unit name IKEY = receipe name
		GetConfigureDb()->DoExec(this,pc,tItem);
	}
	else
	{
		QString pc = 
		"select * from PROPS where SKEY='" + QString(name) + 
		"' and (IKEY='(default)' or IKEY='"+ Receipe + "') order by IKEY desc;"; 
		//
		// get the properties SKEY = unit name IKEY = receipe name
		GetConfigureDb()->DoExec(this,pc,tItem);
	}

	if(Receipe != "(default)")
	{
		NItems->setEnabled(false);
		RFC1006ServerIPAddressText->setEnabled(false);
		RFC1006ServerIPPortText->setEnabled(false);
		RFC1006ServerSlotText->setEnabled(false);
		PLCAddressText->setEnabled(false);
	}
}
Rfc1006driverConfiguration::~Rfc1006driverConfiguration()
{
}
void Rfc1006driverConfiguration::Help()
{
	//
	// invoke the help viewer for the Rfc1006driver
	// 
	QSHelp("Rfc1006driver");
}
void Rfc1006driverConfiguration::OkClicked()
{
	//
	QString cmd = QString("delete from PROPS where SKEY='")+QString(Name->text()) + QString("' and IKEY='") + Receipe + "';";
	GetConfigureDb()->DoExec(0,cmd,0); // delete the old value
	//
	cmd = "insert into PROPS values('"+Name->text() +"','" + Receipe + "','" + 
	NItems->text() + " " + RFC1006ServerIPAddressText->text() + " " + RFC1006ServerIPPortText->text() +  " " + RFC1006ServerSlotText->text() + " " + PLCAddressText->text() + "');";
	GetConfigureDb()->DoExec(0,cmd,0);
	QSAuditTrail(this,caption(), tr("Edited"));

	Rfc1006driver::pDriver->CreateNewUnit(this,Name->text(), (NItems->text()).toInt() );  

	accept();
		
}
void Rfc1006driverConfiguration::QueryResponse (QObject *p, const QString &c, int id, QObject*caller) // handles database responses
{
	if(p != this) return;
	switch(id)
	{
		case tItem:
		{
			if(GetConfigureDb()->GetNumberResults() > 0)
			{
				//
				// we have the properties response
				// fill out the fields
				// 
				QString s = UndoEscapeSQLText(GetConfigureDb()->GetString("DVAL")); // the top one is either the receipe or (default)
				QTextIStream is(&s); // extract the values
				//
				QString t;
				int n;
				is >> n;
				NItems->setValue(n);
				is >> t;
				RFC1006ServerIPAddressText->setText(t);
				is >> t;
				RFC1006ServerIPPortText->setText(t);
				is >> t;
				RFC1006ServerSlotText->setText(t);
				is >> t;
				PLCAddressText->setText(t);
			}
			else
			{
				// just generate the default properties
				QString cmd = "insert into PROPS values('"+Name->text()+"','" + Receipe + "','');";
				GetConfigureDb()->DoExec(0,cmd,0);
				cmd = "insert into PROPS values('"+Name->text()+"','(default)','');"; // create default
				GetConfigureDb()->DoExec(0,cmd,0);
				NItems->setValue(8);
				RFC1006ServerIPAddressText->setText("");
				RFC1006ServerIPPortText->setText("");
				RFC1006ServerSlotText->setText("");
				PLCAddressText->setText("");
			}
		} 
		break;
		default:
		break;
	};
};

