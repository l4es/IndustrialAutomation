/**********************************************************************
--- Qt Architect generated file ---
File: SystemCfg.cpp
Last generated: Thu Apr 13 15:02:26 2000
*********************************************************************/
#include "SystemCfg.h"
#define Inherited SystemCfgData
#include "main.h"
#include "dbase.h"
#include "common.h"
#include "helper_functions.h"

SystemCfg::SystemCfg(QWidget * parent, const char *name):Inherited (parent, name)
{
	setCaption (tr ("System Configuration"));
	QWidget *pL[] = {
		Name, Comment, Language,AuditLevel,AutoLogOut,AutoLogOutTime,MidnightReset,0
	};
	SetTabOrder (this, pL);
	//
	connect (GetConfigureDb (),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (QueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the database
	//
	QString cmd = "select IKEY,DVAL from PROPS where SKEY='System';";
	GetConfigureDb ()->DoExec (this, cmd, tGet);	// make the request
	//
	//
	Name->setText(tr(SYSTEM_NAME));
	QString comment = QString(tr(SYSTEM_NAME)) + QString(" tr(Client)");
	Comment->setText (comment);
	Language->setCurrentItem (0);
	//
	Name->setMaxLength(MAX_LENGHT_OF_STRING);
	Comment->setMaxLength(MAX_LENGHT_OF_STRING);
}

SystemCfg::~SystemCfg ()
{
}

void SystemCfg::OkClicked ()
{
	QString cmd = "delete from PROPS where SKEY='System';\n";
	GetConfigureDb ()->DoExec (0, cmd, 0);	// not interested in reply
	cmd = QSInsertIntoProps("System","Name",Name->text ());
	GetConfigureDb ()->DoExec (0, cmd, 0);	// not interested in reply
	cmd = QSInsertIntoProps("System","Comment",Comment->text ());
	GetConfigureDb ()->DoExec (0, cmd, 0);	// not interested in reply
	cmd = QSInsertIntoProps("System","Language",Language->currentText ());
	GetConfigureDb ()->DoExec (0, cmd, 0);	// not interested in reply
	cmd = QSInsertIntoProps("System","AuditLevel",QString::number(AuditLevel->currentItem()));
	GetConfigureDb ()->DoExec (0, cmd, 0);	// not interested in reply
	cmd = QSInsertIntoProps("System","Receipe",GetReceipeName());
	GetConfigureDb ()->DoExec (0, cmd, 0);	// not interested in reply

	//
	if(AutoLogOut->isChecked()) // auto log out setting
	{
		cmd = QSInsertIntoProps("System","AutoLogOut",AutoLogOutTime->text());
		GetConfigureDb ()->DoExec (0, cmd, 0);	// not interested in reply
	}
	else
	{
		cmd = QSInsertIntoProps("System","AutoLogOut","0");
		GetConfigureDb()->DoExec (0, cmd, 0);	// not interested in reply
	};
	//
	cmd = QSInsertIntoProps("System","MidnightReset",QString::number(MidnightReset->isChecked()));
	GetConfigureDb ()->DoExec (0, cmd, 0);	// not interested in reply
	//
	accept ();
}

void SystemCfg::Help()
{
	QSHelp("SystemConfigure");
};

void SystemCfg::QueryResponse (QObject * p, const QString &, int State, QObject*caller)	// notify transaction complete
{
	switch (State)
	{
		case tGet:
		{
			//
			// Filter the properties
			AutoLogOut->setChecked(false);
			AutoLogOutTime->setEnabled(false);
			//
			if (GetConfigureDb ()->GetNumberResults () > 0)
			{
				for (unsigned i = 0; i < GetConfigureDb ()->GetNumberResults (); i++,GetConfigureDb()->FetchNext())
				{
					if (GetConfigureDb()->GetString ("IKEY") == "Name")
					{
						//Name->setText (GetConfigureDb()->GetString ("DVAL"));
					}
					else if (GetConfigureDb ()->GetString ("IKEY") == "Comment")
					{
						//Comment->setText (GetConfigureDb()->GetString ("DVAL"));
					}
					else if (GetConfigureDb ()->GetString ("IKEY") == "Language")
					{
						SetComboItem (Language,	GetConfigureDb ()->GetString ("DVAL"));
					}
					else if (GetConfigureDb ()->GetString ("IKEY") == "AuditLevel")
					{
						AuditLevel->setCurrentItem(GetConfigureDb ()->GetInt ("DVAL"));
					}
					else if (GetConfigureDb ()->GetString ("IKEY") == "AutoLogOut")
					{
						int n = GetConfigureDb ()->GetInt ("DVAL");
						AutoLogOutTime->setValue(n);
						if(n > 0)
						{
							AutoLogOut->setChecked(true);
							AutoLogOutTime->setEnabled(true);
						}
					}
					else if (GetConfigureDb ()->GetString ("IKEY") == "MidnightReset") // midnight reset flag
					{
						MidnightReset->setChecked(GetConfigureDb()->GetInt("DVAL"));	
					};
				};
			};
			GetConfigureDb ()->DoneExec (this);	// 
			Name->setFocus ();	// put focus on the name
		};
		break;
		default:
		GetConfigureDb ()->DoneExec (this);	// whatever it was ignore the return
		break;
	};
};

void SystemCfg::AutoLogoutToggled(bool f) // handle auto log off settings
{
	AutoLogOutTime->setEnabled(f);
	if(!f)
	{
		AutoLogOutTime->setValue(0);
	};
};
