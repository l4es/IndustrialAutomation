/**********************************************************************
--- Qt Architect generated file ---
File: ScheduleCfg.cpp
Last generated: Thu Apr 20 09:35:35 2000
*********************************************************************/
#include "ScheduleCfg.h"
#define Inherited ScheduleCfgData
#include "main.h"
#include "dbase.h"
#include "common.h"
#include "NewDlg.h"
#include "IndentedTrace.h"
#include "helper_functions.h"

ScheduleCfg::ScheduleCfg
(QWidget * parent,
const char *name):Inherited (parent, name), pTimer (new QTimer (this))
{
	setCaption (tr("Scheduled Event Configuration"));
	connect (GetConfigureDb (),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (QueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the database
	GetConfigureDb()->DoExec(this,"select NAME from SCHEDULE order by NAME asc;",tList);
	QString cmd = "select NAME from REPORTS order by NAME asc;";
	GetConfigureDb ()->DoExec (this, cmd,tReportList);
	cmd = "select NAME from SAMPLE order by NAME asc;";
	GetConfigureDb ()->DoExec (this,  cmd, tSamplePointList);
	cmd = "select NAME from RECEIPE order by NAME asc;";
	GetConfigureDb ()->DoExec (this, cmd, tReceipeList);
	ButtonState (false);
	connect (pTimer, SIGNAL (timeout ()), this, SLOT (DoSelChange ()));	// wire up the item selection timer
	//
	Comment->setMaxLength(MAX_LENGHT_OF_STRING);

	//set the tab order
	// 
	QWidget *pL[] = 
	{
		Name, Comment, FrequencyMode, Time,
		SamplePointList, TagList, RelOp, TriggerValue,
		ReportList, PrintReport, EditReportButton,
		ReceipeList, EditReceipeButton,	Action,
		ApplyButton, NewButton, DeleteButton,
		0
	};
	SetTabOrder (this, pL);
}
ScheduleCfg::~ScheduleCfg ()
{
}
void ScheduleCfg::Help ()
{
	QSHelp("ConfigureSchedule");
}
void ScheduleCfg::SpChanged (int)
{
	// sample point has changed update the tag list
	TagList->clear ();
	// list the tags for this sample point
	if (SamplePointList->currentText () != tr(NONE_STR))
	{
		QString cmd =
		"select TAG from TAGS where NAME='" + SamplePointList->currentText()+"';";
		GetConfigureDb ()->DoExec (this, cmd,tTagList);
	}
	else
	{
		TagList->insertItem (tr(NONE_STR));
	};
}
void ScheduleCfg::Delete ()
{
	if(Name->count() > 0)
	{
		if (!YESNO
		("Delete Schedules Event",
		"Delete the Scheduled Event - Are You Sure ?"))
		{
			//
			DOAUDIT(tr("Delete:") + Name->currentText());
			//
			QString cmd =
			GetConfigureDb ()->Delete ("SCHEDULE", "NAME",
			Name->currentText ());
			GetConfigureDb ()->DoExec (this,  cmd,tDelete);
			Name->removeItem(Name->currentItem());
			Name->setCurrentItem (0);
			SelChanged (0);
			Name->setFocus ();
		};
	};
}
void ScheduleCfg::SelChanged (int)
{
	pTimer->stop ();		// cancel any timer action  
	if (Name->count ())
	{
		pTimer->start (100, TRUE);	// after inactivity fetch the record - we wan to avoid too much activity
	};
}
void ScheduleCfg::Apply ()
{
	if(Name->count() > 0)
	{
		Build ();
		QString cmd =
		GetConfigureDb ()->Update ("SCHEDULE", "NAME", Name->currentText ());	// generate the update record
		GetConfigureDb ()->DoExec (this,  cmd, tApply);	// lock the db and action the request
		ButtonState (false);
		DOAUDIT(tr("Apply:") + Name->currentText());
	};
}
void ScheduleCfg::New ()
{
	NewDlg dlg (Name, this);
	if (dlg.exec ())
	{
		Comment->setText (tr(NONE_STR));
		Name->insertItem (dlg.GetName ());	// add to the list box
		SetComboItem (Name, dlg.GetName ());	// make it the selected item 
		Time->setText ("00:00:00");
		//
		SamplePointList->setCurrentItem (0);
		TagList->clear ();
		TriggerValue->setText ("0");
		Action->setText("");
		//
		SetComboItem(ReportList, tr(NONE_STR));
		SetComboItem(ReceipeList, tr(NONE_STR));
		SetComboItem(SamplePointList,tr(NONE_STR));
		//
		Build ();		// build the record 
		GetConfigureDb ()->AddToRecord ("NAME", Name->currentText ());
		QString cmd = GetConfigureDb ()->Insert ("SCHEDULE");	// generate the update record
		GetConfigureDb ()->DoExec (this,  cmd, tNew);	// lock the db and action the request
		//
		ButtonState (false);
		//
		DOAUDIT(tr("New:") + dlg.GetName());
		//
	};
}
void ScheduleCfg::ActionSetup ()
{
	// go to the action editor
}
void ScheduleCfg::FreqChange (int)
{
	// frequency has changed
}
void ScheduleCfg::ActionToggle (bool f)
{
}
void ScheduleCfg::DoSelChange () // action the fetchangedorord record - we wan to avoid too much activityy from the properties table) text,enddate text);
{
	QString cmd = "select * from SCHEDULE where NAME='" + Name->currentText()+ "';";
	GetConfigureDb ()->DoExec (this,  cmd, tItem);	// lock the db and action the request
	ButtonState (false);
};
//

//
void ScheduleCfg::Build () // build the update record - the insert needs the name record added8,repname text, action text,repprint int2);ate text);
{
	GetConfigureDb ()->ClearRecord ();	// clear the record
	GetConfigureDb ()->AddToRecord ("COMMENT", Comment->text ());
	GetConfigureDb ()->AddInt ("FREQMODE", FrequencyMode->currentItem ());
	GetConfigureDb ()->AddInt ("TOD", QStringToInterval(Time->text ()));
	GetConfigureDb ()->AddToRecord ("TRIGGERPOINT",
	SamplePointList->currentText ());
	GetConfigureDb ()->AddInt ("RELOP", RelOp->currentItem ());
	GetConfigureDb ()->AddToRecord ("TRIGGERTAG", TagList->currentText ());
	GetConfigureDb ()->AddToRecord ("TRIGGERVAL", TriggerValue->text (),
	false);
	GetConfigureDb ()->AddToRecord ("REPNAME", ReportList->currentText ());
	GetConfigureDb ()->AddBool ("REPPRINT", PrintReport->isChecked ());
	GetConfigureDb ()->AddToRecord ("RECEIPE", ReceipeList->currentText ());
	GetConfigureDb ()->AddToRecord ("ACTIONS",Action->text());
	//
	// store the actions
	//
};
void ScheduleCfg::QueryResponse (QObject *p, const QString &c, int State, QObject*caller) // notify transaction completerequestrt needs the name record added8,repname text, action text,repprint int2);ate text);
{
	if(p != this) return;

	IT_IT("ScheduleCfg::QueryResponse");

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
			//
		};
		break;
		case tItem:
		{
			
			IT_COMMENT("Received User Data");
						//
			// fill the fields 
			// 
			Comment->setText (UndoEscapeSQLText(GetConfigureDb()->GetString ("COMMENT")));
			FrequencyMode->setCurrentItem (GetConfigureDb ()->GetInt ("FREQMODE"));
			Time->setText (IntervalToString(GetConfigureDb ()->GetInt ("TOD")));
			SetComboItem (SamplePointList,
			GetConfigureDb ()->GetString ("TRIGGERPOINT"));
			RelOp->setCurrentItem (GetConfigureDb ()->GetInt ("RELOP"));
			//
			TagList->clear ();
			TagList->insertItem (GetConfigureDb ()->GetString ("TRIGGERTAG"));
			//
			// we should now fetch the tag list for the selected trigger point
			// 
			TriggerValue->setText (GetConfigureDb ()->GetString ("TRIGGERVAL"));
			//
			SetComboItem (ReportList, GetConfigureDb ()->GetString ("REPNAME"));
			//
			PrintReport->setChecked (GetConfigureDb ()->GetBool ("REPPRINT"));
			//
			// Get the action string
			//
			Action->setText(GetConfigureDb ()->GetString("ACTIONS"));
			SetComboItem(ReceipeList,GetConfigureDb()->GetString("RECEIPE"));
			//
			Comment->setFocus ();
			ButtonState (true);
			//
			SpChanged(0);
			//
		};
		return;
		case tTagList:		// 
		{
			QString s = TagList->currentText ();	// should have been set
			TagList->clear ();
			GetConfigureDb ()->FillComboBox (TagList, "TAG");
			//
			if (GetConfigureDb ()->GetNumberResults () > 0)
			{
				SetComboItem (TagList, s);	// put it back as it was
			}
			else
			{
				TagList->insertItem (tr(NONE_STR));
			};
			//
			GetConfigureDb ()->DoneExec (this);	// all done so release
		};
		break;
		case tReportList:		// get the list of reports
		{
			GetConfigureDb ()->FillComboBox (ReportList, "NAME");
			ReportList->insertItem(tr(NONE_STR));
			SetComboItem(ReportList,tr(NONE_STR));
		};
		break;
		case tSamplePointList:	//
		{
			GetConfigureDb ()->FillComboBox (SamplePointList, "NAME");
			SamplePointList->insertItem (tr(NONE_STR));	// the null choice
			SetComboItem(SamplePointList,tr(NONE_STR));
			ButtonState (true);	// the basic field are all set up now
			SelChanged (0);		// get the top of the list - load up first record
		};
		break;
		case tReceipeList:
		{
			GetConfigureDb ()->FillComboBox (ReceipeList, "NAME");
			ReceipeList->insertItem (tr(NONE_STR));	// the null choice
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
#include "ReportCfg.h"
void ScheduleCfg::EditReport ()
{
	if (ReportList->currentText () != tr(NONE_STR))
	{
		ReportCfg dlg (this, ReportList->currentText ());
		dlg.exec ();
	};
};
#include "ReceipeCfg.h"
void ScheduleCfg::EditReceipe ()
{
	if (ReceipeList->currentText () != tr(NONE_STR))
	{
		ReceipeCfg dlg (this, ReceipeList->currentText ());
		dlg.exec ();
	};
};

