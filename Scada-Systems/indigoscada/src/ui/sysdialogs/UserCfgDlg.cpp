/**********************************************************************
--- Qt Architect generated file ---
File: UserCfgDlg.cpp
Last generated: Thu Apr 13 14:56:12 2000
*********************************************************************/
#include "UserCfgDlg.h"
#include "main.h"
#define Inherited UserCfgDlgData
#include "dbase.h"
#include "common.h"
#include "NewDlg.h"
#include "IndentedTrace.h"
#include "helper_functions.h"

UserCfgDlg::UserCfgDlg
(QWidget * parent,
const char *name):Inherited (parent, name), pTimer (new QTimer (this))
{
	setCaption (tr ("User Configuration"));
	//
	// retrive a table of user names
	//
	connect (GetConfigureDb (), SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (QueryResponse (QObject *, const QString &, int, QObject*)));	
	GetConfigureDb()->DoExec(this,"select NAME from USERS order by NAME asc;",tList);
	ButtonState (false);
	connect (pTimer, SIGNAL (timeout ()), this, SLOT (DoSelChange ()));	// wire up the item selection timer
	//
	Comment->setMaxLength(MAX_LENGHT_OF_STRING);
	//set the tab order
	// 
	QWidget *pL[] = {
		Name,
		Password,
		Comment,
		Language,
		AgePassword,
		AgeDays,
		AllPriv,
		AlarmAckPriv,
		ReportsPriviledge,
		ReportsPriv,
		ExitApp,
		FactoryAdministrator,
		0
	};
	SetTabOrder (this, pL);
}
UserCfgDlg::~UserCfgDlg ()
{
}
void UserCfgDlg::Help ()
{
	QSHelp("ConfigureUsers");
}
void UserCfgDlg::Delete ()
{
	if (!YESNO ("Delete User", "Delete the User - Are You Sure ?"))
	{
		QString cmd = GetConfigureDb ()->Delete ("USERS", "NAME", Name->currentText ());
		GetConfigureDb ()->DoExec (this,  cmd, tDelete);
		DOAUDIT(tr("Delete:") + Name->currentText());
		Name->removeItem(Name->currentItem());
		Name->setCurrentItem (0);
		SelChanged (0);
		Name->setFocus ();
	};
}
void UserCfgDlg::SelChanged (int)
{
	pTimer->stop ();		// cancel any timer action  
	if (Name->count () > 0)
	{
		pTimer->start (100, TRUE);	// after inactivity fetch the record - we wan to avoid too much activity
	};
}
void UserCfgDlg::DoSelChange ()
{
	IT_IT("UUserCfgDlg::DoSelChange");	
	
	QString cmd =
	"select * from USERS where NAME ='" + Name->currentText () + "';";	// retrive the record
	GetConfigureDb ()->DoExec (this,cmd,tItem);
	ButtonState (false);
};
void UserCfgDlg::Apply ()
{
	Build ();
	GetConfigureDb()->AddToRecord("UPDTIME",DATETIME_NOW,false); // set the update time
	QString cmd =
	GetConfigureDb ()->Update ("USERS", "NAME", Name->currentText ());	// generate the update record
	GetConfigureDb ()->DoExec (this, cmd, tApply);	// lock the db and action the request
	ButtonState (false);
	DOAUDIT(tr("Apply:") + Name->currentText());
}
void UserCfgDlg::New ()
{
	// Get the new name
	NewDlg dlg (Name, this);
	if (dlg.exec ())
	{
		// 
		ReportsPriviledge->setChecked (0);
		AlarmAckPriv->setChecked (0);
		ReportsPriv->setChecked (0);
		ReportsPriviledge->setEnabled (true);
		AlarmAckPriv->setEnabled (true);
		ReportsPriv->setEnabled (true);
		ExitApp->setEnabled(0); // need this priviledge to close the user interface
		AllPriv->setChecked (0);
		AgePassword->setChecked(true); // default to needing to change passwords
		AgeDays->setValue(0); // require password to change on next login
		FactoryAdministrator->setChecked(false); // default no Factory Administrator
		//
		Comment->setText (tr(NONE_STR));
		Password->setText (tr(NONE_STR));
		//
		// leave the language - so it defaults to current
		// 
		Name->insertItem (dlg.GetName ());	// add to the list box
		SetComboItem (Name, dlg.GetName ());	// make it the selected item 
		//
		Build ();		// build the record 
		GetConfigureDb ()->AddToRecord ("PSSWORD", tr(NONE_STR)); // force user toe choose password on startup
		//
		GetConfigureDb ()->AddToRecord ("NAME", Name->currentText ());
		QString cmd = GetConfigureDb ()->Insert ("USERS");	// generate the update record
		GetConfigureDb ()->DoExec (this, cmd,tNew);	// lock the db and action the request
		//
		ButtonState (false);
		//
		DOAUDIT(tr("New:") + dlg.GetName());
		//
	};
}
void UserCfgDlg::AllChanged (bool f)
{
	if (f)
	{
		ReportsPriviledge->setChecked (false);
		AlarmAckPriv->setChecked (false);
		ReportsPriv->setChecked (false);
		AgePassword->setChecked(true);
		ExitApp->setChecked(false);
		FactoryAdministrator->setChecked(false);
		//
		ReportsPriviledge->setEnabled (false);
		AlarmAckPriv->setEnabled (false);
		ReportsPriv->setEnabled (false);
		AgePassword->setEnabled(false);
		ExitApp->setEnabled(false);
		FactoryAdministrator->setEnabled(false);
	}
	else
	{
		ReportsPriviledge->setEnabled (true);
		AlarmAckPriv->setEnabled (true);
		ReportsPriv->setEnabled (true);
		AgePassword->setEnabled(true);
		FactoryAdministrator->setEnabled(true);
		ExitApp->setEnabled(true);
	};
}
void UserCfgDlg::Build () // build the update record - the insert needs the name record addedivityLled int2);, tion text,repprint int2);ate text);
{
	GetConfigureDb ()->ClearRecord ();	// clear the record
	if(Password->text() != tr(NONE_STR))
	{
		GetConfigureDb ()->AddToRecord ("PSSWORD", EncodePassword(Password->text ()));
	};
	GetConfigureDb ()->AddToRecord ("COMMENT", Comment->text ());
	GetConfigureDb ()->AddToRecord ("LANGUAGE", Language->currentText ());
	GetConfigureDb ()->AddToRecord("EXPIREDAYS",AgeDays->text());
	//
	// get the privs mask
	if (AllPriv->isChecked ())
	{
		GetConfigureDb ()->AddToRecord ("PRIVS", "-1", false);
	}
	else
	{
		int p = 0;
		//
		p =
		(ReportsPriviledge->isChecked ()? PRIVS_REPORTS : 0) |
		(AlarmAckPriv->isChecked ()? PRIVS_ACK_ALARMS : 0) |
		(ReportsPriv->isChecked ()? PRIVS_REPORT_MAKE : 0) | 
		(AgePassword->isChecked()?PRIVS_AGE_PASSWORD:0) |
		(FactoryAdministrator->isChecked()?PRIVS_FACTORY_ADMIN:0) |
		(ExitApp->isChecked()? PRIVS_CAN_EXIT:0) ;
		//
		QString s;
		s.setNum (p);
		GetConfigureDb ()->AddToRecord ("PRIVS", s, false);
		//
	};
};
void UserCfgDlg::QueryResponse (QObject *p, const QString &c, int State, QObject*caller) // notify transaction completerequestrt needs the name record addedivityLled int2);, tion text,repprint int2);ate text);
{
	if(p != this) return;

	IT_IT("UserCfgDlg::QueryResponse");

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
			Password->setText (tr(NONE_STR)); // we set the password to tr(NONE_STR). If it changes we save it otherwise leave it alone
			Comment->setText (UndoEscapeSQLText(GetConfigureDb()->GetString ("COMMENT")));
			SetComboItem (Language, GetConfigureDb ()->GetString ("LANGUAGE"));
			//
			// get the privs 
			// 
			int p = GetConfigureDb ()->GetString ("PRIVS").toInt ();
			//
			AllPriv->setChecked (0);
			ReportsPriviledge->setChecked (0);
			AlarmAckPriv->setChecked (0);
			ReportsPriv->setChecked (0);
			AgePassword->setChecked(0);
			FactoryAdministrator->setChecked(0);
			ExitApp->setChecked(0);
			AgeDays->setValue(GetConfigureDb ()->GetInt("EXPIREDAYS"));
			if (p == (int) PRIVS_ALL)
			{
				AllPriv->setChecked (true);
				AllChanged (true);
			}
			else
			{
				AllChanged (false);	// mark as not all privs
				ReportsPriviledge->setChecked (p & PRIVS_REPORTS);
				AlarmAckPriv->setChecked (p & PRIVS_ACK_ALARMS);
				ReportsPriv->setChecked (p & PRIVS_REPORT_MAKE);
				AgePassword->setChecked (p & PRIVS_AGE_PASSWORD);
				FactoryAdministrator->setChecked (p & PRIVS_FACTORY_ADMIN);
				ExitApp->setChecked(p & PRIVS_CAN_EXIT);
			};
			Password->setFocus ();
			ButtonState (true);
		};
		break;
		case tDelete:
		case tNew:
		case tApply:
		ButtonState (true);
		default:
		break;
	};
};

