/**********************************************************************
--- Qt Architect generated file ---
File: BatchCfg.cpp
Last generated: Thu Apr 13 17:34:58 2000
*********************************************************************/
#include "BatchCfg.h"
#define Inherited BatchCfgData
#include "main.h"
#include "dbase.h"
#include "common.h"
#include "NewDlg.h"
#include "ReportSelect.h"
#include "helper_functions.h"
//******************repgen support********************************
#include "repgen.h"
//****************************************************************

BatchCfg::BatchCfg
(QWidget * parent,
const char *name):
Inherited (parent, name),
pTimer (new QTimer (this)),repgen(NULL)
{
	setCaption (tr ("Batch Editor"));
	//
	connect (GetConfigureDb (),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (QueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the databas
	GetConfigureDb()->DoExec(this,"select NAME from BATCH order by NAME asc;",tList);
	ButtonState (false);
	connect (pTimer, SIGNAL (timeout ()), this, SLOT (DoSelChange ()));	// wire up the item selection timer
	//set the tab order
	QWidget *pL[] = {
		Name, Comment, StartTime, StartDate, EndTime, EndDate, 0
	};
	SetTabOrder (this, pL);
	
	Comment->setMaxLength(MAX_LENGHT_OF_STRING);
	
}
BatchCfg::~BatchCfg ()
{
}
void BatchCfg::Help ()
{
	MSG ("Help", "Help");
}
void BatchCfg::Delete ()
{
	if (!YESNO ("Delete Batch", "Delete the Batch - Are You Sure ?"))
	{
		QString cmd = "delete from BATCH where NAME='"+Name->currentText()+"';";
		GetConfigureDb ()->DoExec(this,cmd,tDelete);
		//
		DOAUDIT(tr("Delete:") + Name->currentText());
		//
		Name->removeItem(Name->currentItem());
		Name->setCurrentItem (0);
		SelChanged (0);
		Name->setFocus ();
	};
}
void BatchCfg::SelChanged (int)
{
	pTimer->stop ();		// cancel any timer action  
	if (Name->count () > 0)
	{
		pTimer->start (100, TRUE);	// after inactivity fetch the record - we wan to avoid too much activity
	};
}
void BatchCfg::DoSelChange ()
{
	QString cmd =
	"select * from BATCH where NAME ='" + Name->currentText () + "';";	// retrive the record
	GetConfigureDb ()->DoExec (this, cmd, tItem);
	ButtonState (false);
};
void BatchCfg::Apply ()
{
	Build ();
	QString cmd = GetConfigureDb ()->Update ("BATCH", "NAME", Name->currentText ());	// generate the update record
	GetConfigureDb ()->DoExec (this, cmd, tApply);	// lock the db and action the request
	ButtonState (false);
	//
	DOAUDIT(tr("Apply:") + Name->currentText());
	//
}
void BatchCfg::Report ()
{
	//
	// run the report
	// get the report to template
	// generate the report thing
	//
	ReportSelect dlg(this);
	if(dlg.exec())
	{
		if(!dlg.List->currentText().isEmpty())
		{
			// invoke the report generator and then the viewer 
			QString r = dlg.List->currentText();
			//
			#ifdef UNIX
			QString cmd = QSBIN_DIR + "/repgen ";
			for(int i = 1; i < 7; i++)
			{
				cmd +=  QString(" \"") + qApp->argv()[i] + "\" ";  
			};
			//
			// add the report name
			cmd += r;
			// 
			// add the time frame
			cmd += QString(" \"") + StartDate->text() + " " + StartTime->text() + "\" ";
			cmd += QString(" \"") + EndDate->text() + " "   + EndTime->text() + "\" ";
			cmd += " " + Name->currentText(); // add the batch name
			//
			// add the report viewer startup and put the whole lot out as a detached task
			// Win32 ports will have to do this differently as non-console tasks detach immediately
			// 
			cmd += "; " + QSBIN_DIR + "/helpviewer " + QSREPORT_DIR + "/" + r + "/index.html&";
			//
			// spawn it
			system((const char *)cmd); 
			//
			#endif

			#ifdef WIN32
			QString From = QString(" ") + StartDate->text() + " " + StartTime->text() + " ";
			QString To = QString(" ") + EndDate->text() + " "   + EndTime->text() + " ";
			QString BatchName =  Name->currentText(); // add the batch name
			if(repgen) delete repgen;
			repgen = new ReportGenerator(r, From, To, BatchName);
			#endif //WIN32
		};
	};
	// 
}
void BatchCfg::New ()
{
	NewDlg dlg (Name, this);
	if (dlg.exec ())
	{
		Comment->setText (tr(NONE_STR));
		StartTime->setText ("00:00:00");
		EndTime->setText ("00:00:00");
		StartDate->Default ();
		EndDate->Default ();
		// leave the language - so it defaults to current
		// 
		Name->insertItem (dlg.GetName ());	// add to the list box
		SetComboItem (Name, dlg.GetName ());	// make it the selected item 
		//
		Build ();		// build the record 
		GetConfigureDb ()->AddToRecord ("NAME", Name->currentText ());
		QString cmd = GetConfigureDb ()->Insert ("BATCH");	// generate the update record
		GetConfigureDb ()->DoExec (this, cmd,tNew);	// lock the db and action the request
		//
		ButtonState (false);
		//
		DOAUDIT(tr("New:") + dlg.GetName());
		//
	};
}

void BatchCfg::Build () // build the update record - the insert needs the name record addedarttod text,startdate text,endtod text,enddate text);
{
	GetConfigureDb ()->ClearRecord ();	// clear the record
	GetConfigureDb ()->AddToRecord ("COMMENT", Comment->text ());
	GetConfigureDb ()->AddToRecord ("STARTTOD", StartTime->text ());
	GetConfigureDb ()->AddToRecord ("STARTDATE", StartDate->text ());
	GetConfigureDb ()->AddToRecord ("ENDTOD", EndTime->text ());
	GetConfigureDb ()->AddToRecord ("ENDDATE", EndDate->text ());
};
void BatchCfg::QueryResponse (QObject *p, const QString &c, int State, QObject*caller) // notify transaction completerequestrt needs the name record addedarttod text,startdate text,endtod text,enddate text);
{
	IT_IT("BatchCfg::QueryResponse");

	if(this == p)
	{
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
				StartTime->setText (GetConfigureDb ()->GetString ("STARTTOD"));
				StartDate->setText (GetConfigureDb ()->GetString ("STARTDATE"));
				EndTime->setText (GetConfigureDb ()->GetString ("ENDTOD"));
				EndDate->setText (GetConfigureDb ()->GetString ("ENDDATE"));
				// 
				Comment->setFocus ();
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
};

