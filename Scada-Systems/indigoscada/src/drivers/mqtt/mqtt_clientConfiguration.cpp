/**********************************************************************
--- Qt Architect generated file ---
File: MQTT_clientConfiguration.cpp
Last generated: Mon May 22 17:14:04 2000
*********************************************************************/
#include "MQTT_clientConfiguration.h"
#include <qt.h>
#include "mqtt_client.h"
#define Inherited MQTT_clientConfigurationData
MQTT_clientConfiguration::MQTT_clientConfiguration
(
QWidget* parent,
const char* name,
const char *receipe
)
:
Inherited( parent, name ),Receipe(receipe)
{
	setCaption(tr("MQTT_client Configuration"));

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
		MqttBrokerHostNameText->setEnabled(false);
		SubscribeTopicNameText->setEnabled(false);
		UserNameText->setEnabled(false);
		PasswordText->setEnabled(false);
		PortText->setEnabled(false);
	};
}
MQTT_clientConfiguration::~MQTT_clientConfiguration()
{
}
void MQTT_clientConfiguration::Help()
{
	//
	// invoke the help viewer for the MQTT_client
	// 
	QSHelp("MQTT_client");
}
void MQTT_clientConfiguration::OkClicked()
{
	//
	QString cmd = QString("delete from PROPS where SKEY='")+QString(Name->text()) + QString("' and IKEY='") + Receipe + "';";
	GetConfigureDb()->DoExec(0,cmd,0); // delete the old value
	//
	cmd = "insert into PROPS values('"+Name->text() +"','" + Receipe + "','" + 
	NItems->text() + 
	" " + MqttBrokerHostNameText->text() + 
	" " + SubscribeTopicNameText->text() +
	" " + UserNameText->text() +
	" " + PasswordText->text() +
	" " + PortText->text() +
	"');";

	GetConfigureDb()->DoExec(0,cmd,0);
	QSAuditTrail(this,caption(), tr("Edited"));

	MQTT_client::pDriver->CreateNewUnit(this,Name->text(), (NItems->text()).toInt() );  

	accept();
		
}
void MQTT_clientConfiguration::QueryResponse (QObject *p, const QString &c, int id, QObject*caller) // handles database responses
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
				MqttBrokerHostNameText->setText(t);
				is >> t;
				SubscribeTopicNameText->setText(t);
				is >> t;
				UserNameText->setText(t);
				is >> t;
				PasswordText->setText(t);
				is >> t;
				PortText->setText(t);
			}
			else
			{
				// just generate the default properties
				QString cmd = "insert into PROPS values('"+Name->text()+"','" + Receipe + "','');";
				GetConfigureDb()->DoExec(0,cmd,0);
				cmd = "insert into PROPS values('"+Name->text()+"','(default)','');"; // create default
				GetConfigureDb()->DoExec(0,cmd,0);
				NItems->setValue(8);
				MqttBrokerHostNameText->setText("");
				SubscribeTopicNameText->setText("");
				UserNameText->setText("");
				PasswordText->setText("");
				PortText->setText("");
			}
		} 
		break;
		default:
		break;
	};
};

