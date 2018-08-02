/**********************************************************************
--- Qt Architect generated file ---
File: SerialCfg.cpp
Last generated: Fri Apr 14 12:49:20 2000
*********************************************************************/
#include "SerialCfg.h"
#define Inherited SerialCfgData
#include "main.h"
#include "dbase.h"
#include "common.h"
#include "NewDlg.h"
#include "IndentedTrace.h"
#include "helper_functions.h"

SerialCfg::SerialCfg
(QWidget * parent,
const char *name):Inherited (parent, name), pTimer (new QTimer (this))
{
	setCaption (tr ("Serial Port Configuration"));
	connect (GetConfigureDb (),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (QueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the databas
	GetConfigureDb()->DoExec(this,"select NAME from SERIAL order by NAME asc;",tList);
	ButtonState (false);
	connect (pTimer, SIGNAL (timeout ()), this, SLOT (DoSelChange ()));	// wire up the item selection timer
	//
	Comment->setMaxLength(MAX_LENGHT_OF_STRING);
	//set the tab order
	// 
	QWidget *pL[] = {
		Name, Enabled, Comment, DataBits, StopBits, Parity, BaudRate, 0
	};
	SetTabOrder (this, pL);
}
SerialCfg::~SerialCfg ()
{
}
void SerialCfg::Help ()
{
	QSHelp("ConfigureSerialPorts");
}
void SerialCfg::SelChanged (int)
{
	pTimer->stop ();		// cancel any timer action  
	if (Name->count () > 0)
	{
		pTimer->start (100, TRUE);	// after inactivity fetch the record - we wan to avoid too much activity
	};
}
void SerialCfg::Apply ()
{
	Build ();
	QString cmd =
	GetConfigureDb ()->Update ("SERIAL", "NAME", Name->currentText ());	// generate the update record
	GetConfigureDb ()->DoExec (this, cmd,tApply);	// lock the db and action the request
	ButtonState (false);
	DOAUDIT(tr("Apply:") + Name->currentText());
}


void SerialCfg::Build () // build the update record - the insert needs the name record added, enabled int2);, tion text,repprint int2);ate text);
{
	GetConfigureDb ()->ClearRecord ();	// clear the record
	GetConfigureDb ()->AddToRecord ("COMMENT", Comment->text ());
	GetConfigureDb ()->AddToRecord ("BAUDRATE", BaudRate->currentText (),
	false);
	GetConfigureDb ()->AddToRecord ("DATASIZE", DataBits->currentText (),
	false);
	GetConfigureDb ()->AddToRecord ("STOPBITS", StopBits->currentText (),
	false);
	GetConfigureDb ()->AddInt ("PARITY", Parity->currentItem ());
	GetConfigureDb ()->AddBool ("ENABLED", Enabled->isChecked ());
};
void SerialCfg::QueryResponse (QObject *p, const QString &c, int State, QObject*caller) // notify transaction completerequestrt needs the name record added, enabled int2);, tion text,repprint int2);ate text);
{
	if(p != this) return;

	IT_IT("SerialCfg::QueryResponse");

	switch (State)
	{
		case tList:
		{
			// fill the name list box
			GetConfigureDb ()->FillComboBox (Name, "NAME");
			GetConfigureDb ()->DoneExec (this);
			Name->setCurrentItem (0);
			SelChanged (0);
			Name->setFocus ();
			ButtonState (true);
		};
		break;
		case tItem:
		{
			IT_COMMENT("Received User Data");
			
			// fill the fields     
			Comment->setText (UndoEscapeSQLText(GetConfigureDb()->GetString ("COMMENT")));
			SetComboItem (BaudRate, GetConfigureDb ()->GetString ("BAUDRATE"));
			SetComboItem (DataBits, GetConfigureDb ()->GetString ("DATASIZE"));
			SetComboItem (StopBits, GetConfigureDb ()->GetString ("STOPBITS"));
	
			char *pa[] = {"No Parity","Even Parity","Odd Parity"};
			
			SetComboItem (Parity, pa[GetConfigureDb ()->GetInt ("PARITY")]);

			Enabled->setChecked (GetConfigureDb ()->GetBool ("ENABLED"));
			GetConfigureDb ()->DoneExec (this);	// all done so release
			Comment->setFocus ();
			ButtonState (true);
		};
		break;
		case tApply:
		ButtonState (true);
		default:
		GetConfigureDb ()->DoneExec (this);	// whatever it was ignore the return
		break;
	};
};
void SerialCfg::DoSelChange ()
{
	QString cmd =
	"select * from SERIAL where NAME ='" + Name->currentText () + "';";	// retrive the record
	GetConfigureDb ()->DoExec (this, cmd, tItem);
	ButtonState (false);
};

