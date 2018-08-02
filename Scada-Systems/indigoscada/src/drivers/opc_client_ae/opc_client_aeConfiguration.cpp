/**********************************************************************
--- Qt Architect generated file ---
File: Opc_client_aeConfiguration.cpp
Last generated: Mon May 22 17:14:04 2000
*********************************************************************/
#include "Opc_client_aeConfiguration.h"
#include <qt.h>
#include "opc_client_ae.h"
#define Inherited Opc_client_aeConfigurationData
Opc_client_aeConfiguration::Opc_client_aeConfiguration
(
QWidget* parent,
const char* name,
const char *receipe
)
:
Inherited( parent, name ),Receipe(receipe)
{
	setCaption(tr("Opc_client_ae Configuration"));

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
		OpcServerProgIDText->setEnabled(false);
		OpcServerIPAddressText->setEnabled(false);
		OpcServerClassIDText->setEnabled(false);
	};
}
Opc_client_aeConfiguration::~Opc_client_aeConfiguration()
{
}
void Opc_client_aeConfiguration::Help()
{
	//
	// invoke the help viewer for the Opc_client_ae
	// 
	QSHelp("Opc_client_ae");
}
void Opc_client_aeConfiguration::OkClicked()
{
	//
	QString cmd = QString("delete from PROPS where SKEY='")+QString(Name->text()) + QString("' and IKEY='") + Receipe + "';";
	GetConfigureDb()->DoExec(0,cmd,0); // delete the old value
	//
	cmd = "insert into PROPS values('"+Name->text() +"','" + Receipe + "','" + 
	NItems->text() + 
	" " + PollInterval->text() + 
	" " + OpcServerProgIDText->text() + 
	" " + OpcServerIPAddressText->text() +
	" " + OpcServerClassIDText->text() +
	"');";

	GetConfigureDb()->DoExec(0,cmd,0);
	QSAuditTrail(this,caption(), tr("Edited"));

	Opc_client_ae::pDriver->CreateNewUnit(this,Name->text(), (NItems->text()).toInt() );  

	accept();
		
}
void Opc_client_aeConfiguration::QueryResponse (QObject *p, const QString &c, int id, QObject*caller) // handles database responses
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
				//SetComboItem(OpcServerProgID,t);
				OpcServerProgIDText->setText(t);
				is >> t;
				OpcServerIPAddressText->setText(t);
				is >> t;
				OpcServerClassIDText->setText(t);
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
				//SetComboItem(OpcServerProgID,"COM2");
				OpcServerProgIDText->setText("");
				OpcServerIPAddressText->setText("");
				OpcServerClassIDText->setText("");
			}
		} 
		break;
		default:
		break;
	};
};

