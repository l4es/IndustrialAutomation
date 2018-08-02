/**********************************************************************
--- Qt Architect generated file ---
File: UnitCfg.cpp
Last generated: Thu Apr 13 16:01:56 2000
*********************************************************************/
#include <qt.h>
#include "UnitCfg.h"
#define Inherited UnitCfgData
#include "main.h"
#include "dbase.h"
#include "common.h"
#include "NewDlg.h"
#include "driver.h"
#include "simulator.h"
#include "IndentedTrace.h"


UnitCfg::UnitCfg  (QWidget * parent,
const char *name):Inherited (parent, name), pTimer (new QTimer (this)),fNew(0)
{
	setCaption (tr ("Unit Configuration"));
	connect (GetConfigureDb (),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (QueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the databas
	GetConfigureDb()->DoExec(this,"select NAME from UNITS order by NAME asc;",tList);
	ButtonState (false);
	connect (pTimer, SIGNAL (timeout ()), this, SLOT (DoSelChange ()));	// wire up the item selection timer
	//
	// Fill the unit type list
	QDir d (QSDRIVER_DIR, "*" DLL_EXT, QDir::Name, QDir::Files);	// get the directory listing
	// 
	if (d.count ())
	{
		const QFileInfoList *pD = d.entryInfoList ();
		QFileInfoListIterator it (*pD);
		QFileInfo *fi;
		//
		while ((fi = it.current ()))
		{
			UnitType->insertItem (fi->baseName ());
			++it;
		};
	};
	//
	Comment->setMaxLength(MAX_LENGHT_OF_STRING);
	//set the tab order
	// 
	QWidget *pL[] = 
	{
		Name, Comment, UnitType, Enabled, ApplyButton, NewButton,
		DeleteButton,0
	};
	SetTabOrder (this, pL);
}
UnitCfg::~UnitCfg ()
{
}
void UnitCfg::Help ()
{
	QSHelp("ConfigureUnits");
}
void UnitCfg::Configure ()
{
	// configure the unit by invoking the specific config from the driver DLL
	fNew = false;
	Driver * p = FindDriver(UnitType->currentText());
	if(p)
	{
		DOAUDIT(tr("Configure Unit:") + Name->currentText());
		p->UnitConfigure(this,Name->currentText(),"(default)"); // invoked the unit level configuration 
	}
}
void UnitCfg::Delete ()
{
	if (!YESNO ("Delete Unit", "Delete the Unit - Are You Sure ?"))
	{
		QString cmd =
		GetConfigureDb ()->Delete ("UNITS", "NAME", Name->currentText ());
		GetConfigureDb ()->DoExec (this, cmd, tDelete);
		//
		DOAUDIT(tr("Delete:") + Name->currentText());
		//
		// Delete all associated sample points
		// 
		if (!YESNO ("Delete Unit Sample Points", "Delete the Sample Points As Well - Are You Sure ?"))
		{
			DOAUDIT(tr("Delete Sample Points For:") + Name->currentText());

			GetConfigureDb ()->DoExec (this, "select NAME from SAMPLE where UNIT='" + Name->currentText() + "';", tDrop);

			QString USelect = "(select NAME from SAMPLE where UNIT='" + Name->currentText() + "');";
			
			// delete the sample point tags
			cmd = "delete from TAGS where NAME in " + USelect;
			GetConfigureDb()->DoExec(0,cmd,0);
			// delete the sample point properties
			cmd = "delete from PROPS where IKEY in " + USelect;
			GetConfigureDb()->DoExec(0,cmd,0);
			//
			cmd = "delete from SAMPLE where UNIT='" + Name->currentText() + "';";
			GetConfigureDb()->DoExec(0,cmd,0);
		};
		//
		// delete the properties for specific configurations
		GetConfigureDb()->DoExec(0,"delete from PROPS where SKEY='"+Name->currentText()+"';",0);
		//
		Name->removeItem(Name->currentItem());
		Name->setCurrentItem (0);
		SelChanged (0);
		Name->setFocus ();
		//
	};
}
void UnitCfg::SelChanged (int)
{
	pTimer->stop ();		// cancel any timer action  
	if (Name->count ())
	{
		pTimer->start (100, TRUE);	// after inactivity fetch the record - we wan to avoid too much activity
	};
}
void UnitCfg::Apply ()
{
	Build ();
	QString cmd =
	GetConfigureDb ()->Update ("UNITS", "NAME", Name->currentText ());	// generate the update record
	GetConfigureDb ()->DoExec (this,  cmd, tApply);	// 
	ButtonState (false);
	//
	if(fNew)
	{
		Configure();
	};
	//
	DOAUDIT(tr("Apply:") + Name->currentText());
}
void UnitCfg::New ()
{
	NewDlg dlg (Name, this);
	if (dlg.exec ())
	{
		//
		Comment->setText (tr(NONE_STR));
		Name->insertItem (dlg.GetName ());	// add to the list box
		Enabled->setChecked (0);
		SetComboItem (Name, dlg.GetName ());	// make it the selected item 
		//
		Build ();		// build the record 
		GetConfigureDb ()->AddToRecord ("NAME", Name->currentText ());
		QString cmd = GetConfigureDb ()->Insert ("UNITS");	// generate the update record
		GetConfigureDb ()->DoExec (this, cmd,tNew);	// lock the db and action the request
		//
		ButtonState (false);
		fNew = true;
		DOAUDIT(tr("New:") + dlg.GetName());
	};
}
void UnitCfg::DoSelChange ()
{
	IT_IT("UnitCfg::DoSelChange");	

	QString cmd =
	"select * from UNITS where NAME ='" + Name->currentText () + "';";	// retrive the record
	GetConfigureDb ()->DoExec (this,cmd,tItem);
	ButtonState (false);
	fNew = false;
};
void UnitCfg::Build () // build the update record - the insert needs the name record addedivityLled int2);, tion text,repprint int2);ate text);
{
	GetConfigureDb ()->ClearRecord ();	// clear the record
	GetConfigureDb ()->AddToRecord ("COMMENT", Comment->text ());
	GetConfigureDb ()->AddToRecord ("UNITTYPE", UnitType->currentText ());
	GetConfigureDb ()->AddToRecord ("ENABLED",
	(Enabled->isChecked ()? "1" : "0"),
	false);
};
void UnitCfg::QueryResponse (QObject *p, const QString &c, int State, QObject*caller) // notify transaction completerequestrt needs the name record addedivityLled int2);, tion text,repprint int2);ate text);
{
	if(p != this) return;

	IT_IT("UnitCfg::QueryResponse");

	switch (State)
	{
		case tList:
		{
			// fill the name list box
			GetConfigureDb ()->FillComboBox (Name, "NAME");
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
			SetComboItem (UnitType, GetConfigureDb ()->GetString ("UNITTYPE"));
			Enabled->setChecked (GetConfigureDb ()->GetString ("ENABLED").toInt ());
			Comment->setFocus ();
			ButtonState (true);
		};
		break;
		case tDrop:
		{
			int n = GetConfigureDb()->GetNumberResults();
			
			QStringList list;
			QString point_list = "(";
			if (n)
			{
				for(int i = 0; i < n; i++,GetConfigureDb()->FetchNext())
				{
					if(i)
					{
						point_list += ",";
					};
					point_list += "'" + GetConfigureDb()->GetString("NAME") + "'";
					//
					list << GetConfigureDb()->GetString ("NAME");
				};

				point_list += ");";
			}

			QString cmd ="delete from TAGS_DB where NAME in " + point_list;
			GetCurrentDb ()->DoExec (0,cmd,0);
			//
			cmd ="delete from CVAL_DB where NAME in " + point_list;
			GetCurrentDb ()->DoExec (0,cmd,0);

			
			if(!list.isEmpty())
			{
				QStringList::Iterator it = list.begin();
				for(;it != list.end();++it)
				{
					QString cmd = "drop table "+ (*it) + ";";
					GetResultDb()->DoExec(0,cmd,0);
					GetHistoricResultDb()->DoExec (0,cmd,0);
				}
			}

			//TO DO APA caricare tutti i Driver *p in un dizionario come fa il monitor
			//e poi usarli quando e' necessario
			Driver * p = FindDriver(UnitType->currentText());
			if(p)
			{
				DOAUDIT(tr("Drop Unit Tables:") + Name->currentText());
				p->DropAllSpecTables(list); // invoked the unit level drop tables 
			}

		}
		break;
		case tNew:
		case tDelete:
		case tApply:
		ButtonState (true);
		default:
		break;
	};
};

