/**********************************************************************
--- Qt Architect generated file ---
File: Dnp3driverConfiguration.cpp
Last generated: Mon May 22 17:14:04 2000
*********************************************************************/
#include "Dnp3driverConfiguration.h"
#include <qt.h>
#include "dnp3driver.h"
#define Inherited Dnp3driverConfigurationData
Dnp3driverConfiguration::Dnp3driverConfiguration
(
QWidget* parent,
const char* name,
const char *receipe
)
:
Inherited( parent, name ),Receipe(receipe)
{
	setCaption(tr("Dnp3driver Configuration"));

	Name->setText(name);

	connect (GetConfigureDb (),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (QueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the database

	//GetConfigureDb()->DoExec(this,"select * from SERIAL where ENABLED=1 order by NAME;",tSerial); // get the list of enabled serial ports 
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
	};
	if(Receipe != "(default)")
	{
		NItems->setEnabled(false);
		DNP3ServerIPAddressText->setEnabled(false);
		DNP3ServerIPPortText->setEnabled(false);
		IOA_AO->setEnabled(false);
		IOA_BO->setEnabled(false);
		IOA_CI->setEnabled(false);
		IOA_BI->setEnabled(false);
		IOA_AI->setEnabled(false);
		ServerID->setEnabled(false);
	};
}
Dnp3driverConfiguration::~Dnp3driverConfiguration()
{
}
void Dnp3driverConfiguration::Help()
{
	//
	// invoke the help viewer for the Dnp3driver
	// 
	QSHelp("Dnp3driver");
}
void Dnp3driverConfiguration::OkClicked()
{
	//
	QString cmd = QString("delete from PROPS where SKEY='")+QString(Name->text()) + QString("' and IKEY='") + Receipe + "';";
	GetConfigureDb()->DoExec(0,cmd,0); // delete the old value
	//
	cmd = "insert into PROPS values('"+Name->text() +"','" + Receipe + "','" + 
	NItems->text() + " " + PollInterval->text() + " " + DNP3ServerIPAddressText->text() + " " + DNP3ServerIPPortText->text() +  " " +
	
	IOA_AO->text() +  " " + IOA_BO->text() +  " " + IOA_CI->text() + " " + IOA_BI->text() + " " + IOA_AI->text() + " " + ServerID->text() + "');";

	GetConfigureDb()->DoExec(0,cmd,0);
	QSAuditTrail(this,caption(), tr("Edited"));

	Dnp3driver::pDriver->CreateNewUnit(this,Name->text(), (NItems->text()).toInt() );  

	accept();
		
}
void Dnp3driverConfiguration::QueryResponse (QObject *p, const QString &c, int id, QObject*caller) // handles database responses
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
				is >> n;
				PollInterval->setValue(n);
				is >> t;
				DNP3ServerIPAddressText->setText(t);
				is >> t;
				DNP3ServerIPPortText->setText(t);
				is >> n;
				IOA_AO->setValue(n);
				is >> n;
				IOA_BO->setValue(n);
				is >> n;
				IOA_CI->setValue(n);
				is >> n;
				IOA_BI->setValue(n);
				is >> n;
				IOA_AI->setValue(n);
				is >> n;
				ServerID->setValue(n);
			}
			else
			{
				// just generate the default properties
				QString cmd = "insert into PROPS values('"+Name->text()+"','" + Receipe + "','');";
				GetConfigureDb()->DoExec(0,cmd,0);
				cmd = "insert into PROPS values('"+Name->text()+"','(default)','');"; // create default
				GetConfigureDb()->DoExec(0,cmd,0);
				NItems->setValue(8);
				PollInterval->setValue(1000);
				DNP3ServerIPAddressText->setText("");
				DNP3ServerIPPortText->setText("");
				IOA_AO->setValue(200);
				IOA_BO->setValue(150);
				IOA_CI->setValue(100);
				IOA_BI->setValue(50);
				IOA_AI->setValue(1);
				ServerID->setValue(1);
			}
		} 
		break;
		default:
		break;
	};
};

