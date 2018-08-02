/**********************************************************************
--- Qt Architect generated file ---
File: Modbus_driverConfiguration.cpp
Last generated: Mon May 22 17:14:04 2000
*********************************************************************/
#include "Modbus_driverConfiguration.h"
#include <qt.h>
#include "modbus_driver.h"
#define Inherited Modbus_driverConfigurationData
Modbus_driverConfiguration::Modbus_driverConfiguration
(
QWidget* parent,
const char* name,
const char *receipe
)
:
Inherited( parent, name ),Receipe(receipe)
{
	setCaption(tr("Modbus_driver Configuration"));

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
		MODBUSServerIPAddressText->setEnabled(false);
		MODBUSServerIPPortText->setEnabled(false);
	};

    static const char* items[] = { "N", "E", "O", 0 };
    Parity->insertStrList( items );
}
Modbus_driverConfiguration::~Modbus_driverConfiguration()
{
}
void Modbus_driverConfiguration::Help()
{
	//
	// invoke the help viewer for the Modbus_driver
	// 
	QSHelp("Modbus_driver");
}
void Modbus_driverConfiguration::OkClicked()
{
	//
	QString cmd = QString("delete from PROPS where SKEY='")+QString(Name->text()) + QString("' and IKEY='") + Receipe + "';";
	GetConfigureDb()->DoExec(0,cmd,0); // delete the old value
	//
	if(context == RTU)
	{
		MODBUSServerIPAddressText->setText("xxx.xxx.xxx.xxx");
		MODBUSServerIPPortText->setText("502");

		cmd = "insert into PROPS values('"+Name->text() +"','" + Receipe + "','" + 
		NItems->text() + " " + PollInterval->text() + " " + MODBUSServerIPAddressText->text() + " " + MODBUSServerIPPortText->text() +
		" " + SerialDevice->text() + " " + Baud->text() + " " + DataBits->text() +" "+ StopBit->text() +" "+ Parity->currentText() +	"');";
	}
	else if(context == TCP)
	{
		cmd = "insert into PROPS values('"+Name->text() +"','" + Receipe + "','" + 
		NItems->text() + " " + PollInterval->text() + " " + MODBUSServerIPAddressText->text() + " " + MODBUSServerIPPortText->text() +"');";
	}

	GetConfigureDb()->DoExec(0,cmd,0);
	QSAuditTrail(this,caption(), tr("Edited"));

	Modbus_driver::pDriver->CreateNewUnit(this,Name->text(), (NItems->text()).toInt() );  

	accept();
		
}
void Modbus_driverConfiguration::QueryResponse (QObject *p, const QString &c, int id, QObject*caller) // handles database responses
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
				QString MODBUSServerIPAddress;
				QString MODBUSServerIPPort;
				int n;
				is >> n;
				NItems->setValue(n);
				is >> n;
				PollInterval->setValue(n);
				is >> MODBUSServerIPAddress;
				is >> MODBUSServerIPPort;
				is >> t;
				SerialDevice->setText(t);
				is >> n;
				Baud->setValue(n);
				is >> n;
				DataBits->setValue(n);
				is >> n;
				StopBit->setValue(n);
				is >> t;
				Parity->setCurrentText(t);

				if(strlen((const char*)(SerialDevice->text())) == 0)
				{
					TCPButton->toggle();
	                context = TCP;

					MODBUSServerIPAddressText->setText(MODBUSServerIPAddress);
					MODBUSServerIPPortText->setText(MODBUSServerIPPort);

				}
				else
				{
					RTUButton->toggle();
	                context = RTU;

					MODBUSServerIPAddressText->setText("");
					MODBUSServerIPPortText->setText("");
				}
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
				MODBUSServerIPAddressText->setText("");
				MODBUSServerIPPortText->setText("");
				SerialDevice->setText("COM1");
				Baud->setValue(9600);
				DataBits->setValue(8);
				StopBit->setValue(1);
				Parity->setCurrentText("N");
				RTUButton->toggle();
			}
		} 
		break;
		default:
		break;
	};
};

void Modbus_driverConfiguration::RTUContextActive(bool)
{
	context = RTU;
}

void Modbus_driverConfiguration::TCPContextActive(bool)
{
    context = TCP;
}

