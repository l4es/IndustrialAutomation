/**********************************************************************
--- Qt Architect generated file ---
File: Iec101driverConfiguration.cpp
Last generated: Mon May 22 17:14:04 2000
*********************************************************************/
#include "Iec101driverConfiguration.h"
#include <qt.h>
#include "iec101driver.h"
#define Inherited Iec101driverConfigurationData
Iec101driverConfiguration::Iec101driverConfiguration
(
QWidget* parent,
const char* name,
const char *receipe
)
:
Inherited( parent, name ),Receipe(receipe)
{
	setCaption(tr("Iec101driver Configuration"));

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
		IEC101LinkAddressText->setEnabled(false);
		IEC101CASDUText->setEnabled(false);
		COMPortNameText->setEnabled(false);
		baud_rateText->setEnabled(false);
		read_timeout_ms->setEnabled(false);
	};
}
Iec101driverConfiguration::~Iec101driverConfiguration()
{
}
void Iec101driverConfiguration::Help()
{
	//
	// invoke the help viewer for the Iec101driver
	// 
	QSHelp("Iec101driver");
}
void Iec101driverConfiguration::OkClicked()
{
	//
	QString cmd = QString("delete from PROPS where SKEY='")+QString(Name->text()) + QString("' and IKEY='") + Receipe + "';";
	GetConfigureDb()->DoExec(0,cmd,0); // delete the old value
	//
	cmd = "insert into PROPS values('"+Name->text() +"','" + Receipe + "','" + 
	NItems->text() + " " + PollInterval->text() + " " + IEC101LinkAddressText->text() + " " + IEC101CASDUText->text() + " " + COMPortNameText->text() + " " + baud_rateText->text() + " " + read_timeout_ms->text() +"');";

	GetConfigureDb()->DoExec(0,cmd,0);
	QSAuditTrail(this,caption(), tr("Edited"));

	Iec101driver::pDriver->CreateNewUnit(this,Name->text(), (NItems->text()).toInt() );  

	accept();
		
}
void Iec101driverConfiguration::QueryResponse (QObject *p, const QString &c, int id, QObject*caller) // handles database responses
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
				IEC101LinkAddressText->setText(t);
				is >> t;
				IEC101CASDUText->setText(t);
				is >> t;
				COMPortNameText->setText(t);
				is >> t;
				baud_rateText->setText(t);
				is >> n;
				read_timeout_ms->setValue(n);
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
				IEC101LinkAddressText->setText("");
				IEC101CASDUText->setText("");
				COMPortNameText->setText("");
				baud_rateText->setText("9600");
				read_timeout_ms->setValue(1000);
			}
		} 
		break;
		default:
		break;
	};
};

