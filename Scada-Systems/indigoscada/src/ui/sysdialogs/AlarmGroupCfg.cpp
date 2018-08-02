/**********************************************************************
--- Qt Architect generated file ---
File: AlarmGroupCfg.cpp
Last generated: Fri Apr 14 08:57:03 2000
*********************************************************************/
#include <qt.h>
#include "AlarmGroupCfg.h"
#define Inherited AlarmGroupCfgData
#include "main.h"
#include "dbase.h"
#include "common.h"
#include "NewDlg.h"
#include "IndentedTrace.h"
#include "helper_functions.h"

AlarmGroupCfg::AlarmGroupCfg
(QWidget * parent, const char *name):Inherited (parent, name),pTimer(new QTimer(this))
{
	setCaption (tr("Alarm Group Configuration"));
	connect (GetConfigureDb (),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (QueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the databas
	//
	GetConfigureDb()->DoExec(this,"select NAME from ALARMGROUP order by NAME asc;",tList); // get the report namesd 
	GetConfigureDb ()->DoExec (this, "select NAME from SAMPLE;",tListSPs); // get the sample point list
	//
	connect (pTimer, SIGNAL (timeout ()), this, SLOT (DoSelChange ()));	// wire up the item selection timer
	ButtonState(false);
	//
	Comment->setMaxLength(MAX_LENGHT_OF_STRING);
	
}
AlarmGroupCfg::~AlarmGroupCfg ()
{
}
void AlarmGroupCfg::Help ()
{
	QSHelp("ConfigureAlarmGroup");
}
void AlarmGroupCfg::Delete ()
{
	if (!YESNO ("Delete Alarm Group", "Delete the Alarm Group - Are You Sure ?"))
	{
		QString cmd = "delete from ALARMGROUP where NAME='"+Name->currentText()+"';";
		GetConfigureDb ()->DoExec (this,cmd,tDelete);	// lock the db and action the request
		//
		cmd = "delete from ALM_GRP_STATE where NAME ='" + Name->currentText() + "';"; 
		GetCurrentDb()->DoExec(0,cmd,0);
		//
		DOAUDIT(tr("Delete:") + Name->currentText());
		Name->removeItem(Name->currentItem());
		Name->setCurrentItem (0);
		SelChanged (0);
		//
		Name->setFocus ();
	};
}
void AlarmGroupCfg::SelChanged (int)
{
	pTimer->stop ();		// cancel any timer action  
	if (Name->count ())
	{
		pTimer->start (100, TRUE);	// after inactivity fetch the record - we wan to avoid too much activity
	};
}
void AlarmGroupCfg::Apply ()
{
	Build ();
	//
	QString cmd =
	GetConfigureDb ()->Update ("ALARMGROUP", "NAME", Name->currentText ());	// generate the update record
	GetConfigureDb ()->DoExec (this, cmd, tApply);	// lock the db and action the request
	//
	ButtonState (false);
	//
	cmd = "delete from ALM_GRP_STATE where NAME ='" + Name->currentText() + "';"; 
	GetCurrentDb()->DoExec(0,cmd,0);
	if (GroupList->count () > 0)
	{
		for(unsigned i = 0; i < GroupList->count(); i++)
		{
			cmd = "insert into ALM_GRP_STATE values ('"+ Name->currentText() + "','" + GroupList->text (i) + "',0,0," + DATETIME_NOW + ",0);";

			GetCurrentDb()->DoExec(0,cmd,0);
		}
	}
	DOAUDIT(tr("Apply:") + Name->currentText());
}
void AlarmGroupCfg::New ()
{
	NewDlg dlg (Name, this);
	if (dlg.exec ())
	{
		//
		DOAUDIT(tr("New:") + dlg.GetName());
		//
		Comment->setText (tr(NONE_STR));
		Name->insertItem (dlg.GetName ());	// add to the list box
		GroupList->clear ();	// say nothing selected
		ClearSPD ();
		NameDict::iterator j = SPD.begin ();
		//
		// fill out the sample points
		// 
		SamplePoints->clear ();
		for (; !(j == SPD.end()); j++)
		{
			SamplePoints->insertItem ((*j).first); 
		};
		//
		//
		SetComboItem (Name, dlg.GetName ());	// make it the selected item 
		//
		Build ();		// build the record 
		GetConfigureDb ()->AddToRecord ("NAME", Name->currentText ());
		QString cmd = GetConfigureDb ()->Insert ("ALARMGROUP");	// generate the update record
		///
		GetConfigureDb ()->DoExec (this, cmd, tNew);	// lock the db and action the request
		//
		ButtonState (false);
	};
}
void AlarmGroupCfg::DoSelChange ()
{
	QString cmd =
	"select * from ALARMGROUP where NAME ='" + Name->currentText () + "';";	// retrive the record
	GetConfigureDb ()->DoExec (this, cmd ,tItem);
	ButtonState (false);
};
void AlarmGroupCfg::Build () // build the update record - the insert needs the name record addedaphpage int2, samples text);;dtod text,enddate text);
{
	// 
	GetConfigureDb ()->ClearRecord ();	// clear the record
	GetConfigureDb ()->AddToRecord ("COMMENT", Comment->text ());
	//
	// now build the selected points list
	// 
	if (GroupList->count () > 0)
	{
		QString s;
		QTextOStream os (&s);
		os << GroupList->count () << " ";
		for (unsigned i = 0; i < GroupList->count (); i++)
		{
			os << GroupList->text (i) << " ";
		};

		GetConfigureDb ()->AddToRecord ("SAMPLES", s);
	}
	else
	{
		GetConfigureDb ()->AddToRecord ("SAMPLES", "0");
	};
};
void AlarmGroupCfg::QueryResponse (QObject *p, const QString &, int State, QObject*caller) // notify transaction completerequestrt needs the name record addedaphpage int2, samples text);;dtod text,enddate text);
{
	if(p != this) return;

	IT_IT("AlarmGroupCfg::QueryResponse");

	switch (State)
	{
		case tList:
		{
			// fill the name list box
			GetConfigureDb ()->FillComboBox (Name, "NAME");
			Name->setCurrentItem (0);
			Name->setFocus ();
		};
		return;			// next step int he state machine
		case tListSPs:		// list sample points
		{
			GetConfigureDb ()->FillListBox (SamplePoints, "NAME");
			// now fill the dictionary
			SPD.clear ();
			for (unsigned i = 0; i < SamplePoints->count (); i++)
			{
				NameDict::value_type pr (SamplePoints->text (i), false);
				SPD.insert (pr);
			};
			//
			//
			ButtonState (true);
			SelChanged (0);
		};
		break;
		case tItem:
		{
			IT_COMMENT("Received User Data");

			if(GetConfigureDb()->GetNumberResults() > 0)
			{
				Comment->setText (UndoEscapeSQLText(GetConfigureDb()->GetString ("COMMENT")));
				// Now we read the image field and load up the Report List
				GroupList->clear ();
				QString s = GetConfigureDb ()->GetString ("SAMPLES");
				//
				// now extract the strings
				// 
				QTextIStream is (&s);
				int n = 0;		// how many names
				is >> n;
				//
				ClearSPD ();
				if (n > 0)
				{
					for (int i = 0; i < n; i++)
					{
						QString a;
						is >> a;
						a = a.stripWhiteSpace ();
						GroupList->insertItem (a);
						//
						// mark as in report list
						NameDict::iterator k = SPD.find (a);
						if (!(k == SPD.end()))
						{
							(*k).second = true;
						};
					};
				};
				//
				//
				Comment->setFocus ();
				ButtonState (true);
				//
				// set up the sample list
				// 
				NameDict::iterator j = SPD.begin ();
				SamplePoints->clear ();
				//
				for (; !(j == SPD.end()); j++)
				{
					if (!(*j).second)
					{
						SamplePoints->insertItem ((*j).first);
					};
				};
			};
			//
		};
		break;
		case tNew:
		case tDelete:
		case tApply:
		ButtonState (true);
		default:
		break;
	};
};
void AlarmGroupCfg::Remove ()
{
	// take the select sample point from the report list, add to available points list
	// 
	// 
	// 
	QStringList l;
	for(unsigned i = 0; i < GroupList->count(); i++)
	{
		if(GroupList->isSelected(i))
		{
			SamplePoints->insertItem (GroupList->text(i));
		}
		else
		{
			l << GroupList->text(i);
		};
	};
	GroupList->clear();
	GroupList->insertStringList(l);
	//int n = GroupList->currentItem ();
	//if (n >= 0)
	//	{
	//		SamplePoints->insertItem (GroupList->text (n));
	//		GroupList->removeItem (n);
	//	};
};
void AlarmGroupCfg::Add ()
{
	// take from the available points list and add to the reports list
	//int n = SamplePoints->currentItem ();
	//if (n >= 0)
	//{
	//	GroupList->insertItem (SamplePoints->text (n));
	//	SamplePoints->removeItem (n);
	//};
	QStringList l;
	for(unsigned i = 0; i < SamplePoints->count();i++)
	{
		if (SamplePoints->isSelected(i))
		{
			GroupList->insertItem (SamplePoints->text (i));
		}
		else
		{
			l << SamplePoints->text(i);
		};
	};
	SamplePoints->clear();
	SamplePoints->insertStringList(l);
};

