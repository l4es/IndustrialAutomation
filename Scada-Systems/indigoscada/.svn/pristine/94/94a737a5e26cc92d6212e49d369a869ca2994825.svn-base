/**********************************************************************
--- Qt Architect generated file ---
File: ReportCfg.cpp
Last generated: Thu Apr 13 17:01:39 2000
*********************************************************************/
#include "ReportCfg.h"
#define Inherited ReportCfgData
#include "main.h"
#include "dbase.h"
#include "common.h"
#include "NewDlg.h"
#include "repgen.h"
#include "helper_functions.h"

ReportCfg::ReportCfg
(QWidget * parent,
const char *name):Inherited (parent, name), pTimer (new QTimer (this)),repgen(NULL)
{
	setCaption (tr ("Report Configuration"));
	connect (GetConfigureDb (),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (QueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the databas
	//
	GetConfigureDb()->DoExec(this,"select NAME from REPORTS order by NAME asc;",tList); // get the report names 
	GetConfigureDb ()->DoExec (this, "select NAME from SAMPLE order by NAME asc;",tListSPs); // get the sample point list
	//
	//    
	ButtonState (false);
	connect (pTimer, SIGNAL (timeout ()), this, SLOT (DoSelChange ()));	// wire up the item selection timer
	//
	ToDate->setEnabled (false);
	ToTime->setEnabled (false);
	FromTime->setEnabled (false);
	FromDate->setEnabled (false);
	//
	// Get the report templates
	// these are HTML files with some simple tokens
	QDir d (QSTEMPLATE_DIR, "*.html", QDir::Name, QDir::Files);	// get the directory listing
	// 
	if (d.count ())
	{
		const QFileInfoList *pD = d.entryInfoList ();
		QFileInfoListIterator it (*pD);
		QFileInfo *fi;
		//
		while ((fi = it.current ()))
		{
			Front->insertItem (fi->baseName ());
			Back->insertItem (fi->baseName ());
			Table->insertItem (fi->baseName ());
			GraphStats->insertItem (fi->baseName ());
			++it;
		};
	};
	//
	Front->insertItem(tr(NONE_STR));
	Back->insertItem(tr(NONE_STR));
	Table->insertItem(tr(NONE_STR));
	GraphStats->insertItem(tr(NONE_STR));
	//
	SetComboItem(Front,tr(NONE_STR));
	SetComboItem(Back,tr(NONE_STR));
	SetComboItem(Table,tr(NONE_STR));
	SetComboItem(GraphStats,tr(NONE_STR));
	//
	QWidget *pL[] = {
		Name, Comment, FromMode,
		FromTime, FromDate, ToMode,
		ToTime, ToDate, AlarmFilter,
		Front,    Table,    GraphStats,
		Back,    SpList,    ReportList,
		ApplyButton,    NewButton,    DeleteButton,
		MakeButton,
		0
	};
	SetTabOrder (this, pL);
}
ReportCfg::~ReportCfg ()
{
}
void ReportCfg::Help ()
{
	QSHelp("ConfigureReports");
}
void ReportCfg::Delete ()
{
	if (!YESNO ("Delete Report", "Delete the Report - Are You Sure ?"))
	{
		QString cmd = "delete from REPORTS where NAME='"+Name->currentText()+"';";
		GetConfigureDb ()->DoExec (this,cmd,tDelete);	// lock the db and action the request
		Name->removeItem(Name->currentItem());
		Name->setCurrentItem (0);
		SelChanged (0);
		Name->setFocus ();
	};
}
void ReportCfg::ToModeChanged (int)
{
	//
	// handle the enable / disable of the fields
	// the combo box must match the enums in common/common.hpp
	// 
	switch (ToMode->currentItem ())
	{
		case Now:
		{
			ToDate->setEnabled (false);
			ToTime->setEnabled (false);
		};
		break;
		case ToYesterday:
		case ToToday:
		{
			ToDate->setEnabled (false);
			ToTime->setEnabled (true);
		};
		break;
		case ToDateTime:
		{
			ToDate->setEnabled (true);
			ToTime->setEnabled (true);
		};
		default:
		break;
	};
}
void ReportCfg::FromModeChanged (int)
{
	// handle the enable / disable of the fields
	switch (FromMode->currentItem ())
	{
		case LastHour:
		case LastFourHours:
		case LastTwelveHours:
		case LastDay:
		case LastWeek:
		{
			FromTime->setEnabled (false);
			FromDate->setEnabled (false);
		};
		break;
		case FromDateTime:
		{
			FromTime->setEnabled (true);
			FromDate->setEnabled (true);
		};
		break;
		case FromToday:
		case FromYesterday:
		{
			FromTime->setEnabled (true);
			FromDate->setEnabled (false);
		};
		break;
		default:
		break;
	};
}
void ReportCfg::SelChanged (int)
{
	pTimer->stop ();		// cancel any timer action  
	if (Name->count ())
	{
		pTimer->start (100, TRUE);	// after inactivity fetch the record - we want to avoid too much activity
	};
}
void ReportCfg::MakeReport ()
{
	// save and spawn the report generator
	Apply ();
	// invoke the report generator and then the viewer 
	QString r = Name->currentText();
	
	#ifdef UNIX
	//
	QString cmd = QSBIN_DIR + "/repgen ";
	for(int i = 1; i < 7; i++)
	{
		cmd +=  QString(" \"") + qApp->argv()[i] + "\" ";  
	};
	//
	// add the report name
	cmd += r;
	// add the report viewer startup and put the whole lot out as a detached task
	// Win32 ports will have to do this differently as non-console tasks detach immediately
	cmd += "; " + QSBIN_DIR + "/helpviewer " + QSREPORT_DIR + "/" + r + "/index.html&";
	// spawn it
	system((const char *)cmd); 
	//
	// invoke the report generator hence report display

	#endif

	#ifdef WIN32
	if(repgen) delete repgen;
	repgen = new ReportGenerator(r);
	#endif
}
void ReportCfg::Apply ()
{
	//
	QDateTime From = ReportFromTime(FromMode->currentItem(),FromDate->text(),FromTime->text()); 
	QDateTime To   = ReportToTime(ToMode->currentItem(),ToDate->text(),ToTime->text()); 
	if(From < To)
	{
		//
		// validate the time frames
		//
		Build ();
		QString cmd =
		GetConfigureDb ()->Update ("REPORTS", "NAME", Name->currentText ());	// generate the update record
		GetConfigureDb ()->DoExec (this, cmd, tApply);	// lock the db and action the request
		ButtonState (false);
		//
		DOAUDIT(tr("Apply:") + Name->currentText());
		//
	}
	else
	{
		MSG(tr("Time Frame Error"),tr("From Time is After or Equal the To Time"));
	}
}
void ReportCfg::New ()
{
	NewDlg dlg (Name, this);
	if (dlg.exec ())
	{
		Comment->setText (tr(NONE_STR));
		Name->insertItem (dlg.GetName ());	// add to the list box
		ReportList->clear ();	// say nothing selected
		ClearSPD ();
		NameDict::iterator j = SPD.begin ();
		//
		// fill out the sample points
		// 
		SpList->clear ();
		for (; !(j == SPD.end()); j++)
		{
			SpList->insertItem ((*j).first); 
		};
		//
		ToMode->setCurrentItem (0);
		FromMode->setCurrentItem (0);
		ToTime->setText ("00:00:00");
		FromTime->setText ("00:00:00");
		//
		ToDate->Default();
		FromDate->Default();
		SetComboItem(Front,tr(NONE_STR));
		SetComboItem(Back,tr(NONE_STR));
		SetComboItem(Table,tr(NONE_STR));
		SetComboItem(GraphStats,tr(NONE_STR));
		//
		SetComboItem (Name, dlg.GetName ());	// make it the selected item 
		AlarmFilter->setCurrentItem(0);
		//
		Build ();		// build the record 
		GetConfigureDb ()->AddToRecord ("NAME", Name->currentText ());
		QString cmd = GetConfigureDb ()->Insert ("REPORTS");	// generate the update record
		///
		GetConfigureDb ()->DoExec (this, cmd, tNew);	// lock the db and action the request
		ButtonState (false);
	};
}
void ReportCfg::DoSelChange ()
{
	QString cmd =
	"select * from REPORTS where NAME ='" + Name->currentText () + "';";	// retrive the record
	GetConfigureDb ()->DoExec (this, cmd ,tItem);
	ButtonState (false);
};

//                        
void ReportCfg::Build () // build the update record - the insert needs the name record addedaphpage int2, samples text);;dtod text,enddate text);
{
	// 
	GetConfigureDb ()->ClearRecord ();	// clear the record
	//
	GetConfigureDb ()->AddToRecord ("COMMENT", Comment->text ());
	GetConfigureDb ()->AddInt ("FROMMODE", FromMode->currentItem ());
	GetConfigureDb ()->AddToRecord ("FROMTIME", FromTime->text ());
	GetConfigureDb ()->AddToRecord ("FROMDATE", FromDate->text ());
	GetConfigureDb ()->AddInt ("TOMODE", ToMode->currentItem ());
	GetConfigureDb ()->AddToRecord ("TOTIME", ToTime->text ());
	GetConfigureDb ()->AddToRecord ("TODATE", ToDate->text ());
	GetConfigureDb ()->AddToRecord ("FRONT",Front->currentText());
	GetConfigureDb ()->AddToRecord ("BACK",Back->currentText());
	GetConfigureDb ()->AddToRecord ("TABLESHEET",Table->currentText());
	GetConfigureDb ()->AddToRecord ("GRAPHSTATS",GraphStats->currentText());
	GetConfigureDb ()->AddToRecord ("ALARMFILTER",QString::number(AlarmFilter->currentItem()));
	//
	// now build the selected points list
	// 
	if (ReportList->count () > 0)
	{
		QString s;
		QTextOStream os (&s);
		os << ReportList->count () << " ";
		for (unsigned i = 0; i < ReportList->count (); i++)
		{
			os << ReportList->text (i) << " ";
		};
		GetConfigureDb ()->AddToRecord ("SAMPLES", s);
	}
	else
	{
		GetConfigureDb ()->AddToRecord ("SAMPLES", "0");
	};
};
void ReportCfg::QueryResponse (QObject *p, const QString &, int State, QObject*caller) // notify transaction completerequestrt needs the name record addedaphpage int2, samples text);;dtod text,enddate text);
{

	if(p != this) return;

	IT_IT("ReportCfg::QueryResponse");

	switch (State)
	{
		case tList:
		{
			// fill the name list box
			GetConfigureDb ()->FillComboBox (Name, "NAME");
			GetConfigureDb ()->DoneExec (this);
			Name->setCurrentItem (0);
			Name->setFocus ();
			State = tListSPs;
		};
		return;			// next step int he state machine
		case tListSPs:		// list sample points
		{
			GetConfigureDb ()->FillListBox (SpList, "NAME");
			// now fill the dictionary
			SPD.clear ();
			for (unsigned i = 0; i < SpList->count (); i++)
			{
				NameDict::value_type pr (SpList->text (i), false);
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
			
			Comment->setText (UndoEscapeSQLText(GetConfigureDb()->GetString ("COMMENT")));
			FromMode->setCurrentItem (GetConfigureDb ()->GetString ("FROMMODE").
			toInt ());
			FromTime->setText (GetConfigureDb ()->GetString ("FROMTIME"));
			FromDate->setText (GetConfigureDb ()->GetString ("FROMDATE"));
			ToMode->setCurrentItem (GetConfigureDb ()->GetString ("TOMODE").
			toInt ());
			ToTime->setText (GetConfigureDb ()->GetString ("TOTIME"));
			ToDate->setText (GetConfigureDb ()->GetString ("TODATE"));
			//
			// Now we read the image field and load up the Report List
			ReportList->clear ();
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
					ReportList->insertItem (a);
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
			GetConfigureDb ()->DoneExec (this);	// all done so release
			Comment->setFocus ();
			ButtonState (true);
			ToModeChanged (0);
			FromModeChanged (0);
			//
			// set up the sample list
			// 
			NameDict::iterator j = SPD.begin ();
			SpList->clear ();
			//
			for (; !(j == SPD.end()); j++)
			{
				if (!(*j).second)
				{
					SpList->insertItem ((*j).first);
				};
			};
			//
			SetComboItem(Front,GetConfigureDb()->GetString("FRONT"));
			SetComboItem(Back,GetConfigureDb()->GetString("BACK"));
			SetComboItem(Table,GetConfigureDb()->GetString("TABLESHEET"));
			SetComboItem(GraphStats,GetConfigureDb()->GetString("GRAPHSTATS"));
			AlarmFilter->setCurrentItem(GetConfigureDb()->GetInt("ALARMFILTER"));
			//
		};
		break;
		case tNew:
		{
			GetConfigureDb ()->DoneExec (this);	// whatever it was ignore the return
			ButtonState (true);
		};
		break;
		case tDelete:
		case tApply:
		ButtonState (true);
		default:
		GetConfigureDb ()->DoneExec (this);	// whatever it was ignore the return
		break;
	};
	State = -1;
};
void ReportCfg::RemoveSP ()
{
	// take the select sample point from the report list, add to available points list
	// scan for list of selected items
	//
	QStringList l;
	for(unsigned i = 0; i < ReportList->count(); i++)
	{
		if(ReportList->isSelected(i))
		{
			SpList->insertItem (ReportList->text(i));
		}
		else
		{
			l << ReportList->text(i);
		};
	};
	ReportList->clear();
	ReportList->insertStringList(l);
};
void ReportCfg::AddSP ()
{
	// take from the available points list and add to the reports list
	// handle multiple selects
	QStringList l;
	for(unsigned i = 0; i < SpList->count();i++)
	{
		if (SpList->isSelected(i))
		{
			ReportList->insertItem (SpList->text (i));
		}
		else
		{
			l << SpList->text(i);
		};
	};
	SpList->clear();
	SpList->insertStringList(l);
};

