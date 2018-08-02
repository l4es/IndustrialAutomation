/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2009 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */

/*
*Header For: Sample Status Display
*Purpose: this window is a vertically split window. On the left are the sample points (colour coded status) 
*on the right is the status of the currently selected sample point
*
*/
#include "main.h"
#include "statusdisplay.h"
#include "inspect.h"
#include "AckAlarmDlg.h"
#include "IndentedTrace.h"
#define N_STATUS_COLS 4
StatusDisplay::StatusDisplay(QWidget *parent) : 
QSplitter(parent)
{
	IT_IT("StatusDisplay::StatusDisplay");
	
	// 
	pStatus   = new Table(1,N_STATUS_COLS,this); // we now have the list views
	//
	QFontMetrics m = pStatus->fontMetrics();
	//
	//
	Status.Create(this); // create the status panes
	// 
	connect(pStatus,SIGNAL(selectionChanged()),SLOT(SelChanged())); 
	//
	// handle menu request (right click)
	// 
	connect(pStatus,SIGNAL(rightClicked(TableItem *, const QPoint &)),
	SLOT(PointMenu(TableItem *, const QPoint &))); 
	// 
	//
	// connect to the current database
	// 
	connect (GetCurrentDb (),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (QueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the database
	//  
	// connect to the configuration database 
	// 
	connect (GetConfigureDb (),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (QueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the database
	//

	if(GetReceipeName() != "(default)")
	{
		QString cmd = "select * from RECEIPE where NAME='"+ GetReceipeName() +"';";
		GetConfigureDb()->DoExec(this,cmd,tReceipeRecord); // request the record
	}
	else
	{ 
		//
		// the default enabled units and sample points
		// 
		GetConfigureDb()->DoExec(this,QString("select NAME,UNITS from SAMPLE order by NAME asc;"),tList);
	};
	//
	connect(qApp->mainWidget(),SIGNAL(UpdateSamplePoint()),this,SLOT(UpdateSamplePoint()));
	connect(qApp->mainWidget(),SIGNAL(Restart()),this,SLOT(Restart()));
	connect(qApp->mainWidget(),SIGNAL(UpdateTags()),this,SLOT(UpdateTags()));
	
	//////////////////////////////////////////////////////////////////////////////////////

	QString ini_file = GetScadaHomeDirectory() + "\\project\\scada.ini";
	Inifile iniFile((const char*)ini_file);

	QValueList<int> l;
	const char* str;
	if((str = iniFile.find("Size1","StatusDisplay")) != NULL)
	{
		l.append(atoi(str));
	}
	if((str = iniFile.find("Size2","StatusDisplay")) != NULL)
	{
		l.append(atoi(str));
	}
		
	this->setSizes(l);


};
/*-Function: Status point changed 
*Inputs:none
*Outputs:none
*Returns:none
*/
void StatusDisplay::SelChanged() // handle the selection
{
	IT_IT("StatusDisplay::SelChanged");
	
	CurrentSP = (pStatus->currentItem())->text();
	if(!CurrentSP.isEmpty()) // non empty cell ?
	{
		Status.SetName(CurrentSP);
		QString cmd = "select * from TAGS_DB where NAME='"+ CurrentSP +"';"; // update the tag values for the selected SP
		GetCurrentDb()->DoExec(this,cmd,tTags);
		cmd = "select * from CVAL_DB where NAME='"+ CurrentSP +"';"; // update the tag values for the selected SP
		GetCurrentDb()->DoExec(this,cmd,tSamplePoint);
		cmd = "select * from SAMPLE where NAME='"+ CurrentSP +"';"; // update the tag values for the selected SP
		GetConfigureDb()->DoExec(this,cmd,tItem);
		cmd = "select * from TAGS where NAME='"+CurrentSP+"' and RECEIPE='"+GetReceipeName()+"';";
		GetConfigureDb()->DoExec(this,cmd,tTagLimits);
	};
};
/*-Function: Sample point menu
*Inputs:Selected sample point
*Outputs:none
*Returns:none
*/
void StatusDisplay::PointMenu(TableItem *pI, const QPoint &pt) // an item has been right clicked - open the point menu
{
	IT_IT("StatusDisplay::PointMenu");
	
	//
	if(!pI) return;
	QString name = pI->text();
	if(name.isEmpty()) return;
	InspectMenu(pStatus,name,pI->GetFlash());
};
/*
*Function: Query Response - handle transactions with the current values database
*Inputs:object,command, transaction code
*Outputs:none
*Returns:none
*/
void StatusDisplay::UpdateSamplePoint() // handle updated sample points
{
	IT_IT("StatusDisplay::UpdateSamplePoint");
	
	int n = GetCurrentDb()->GetNumberResults();
	GetCurrentDb()->GotoBegin();
	for(int i = 0; i < n ; i++,GetCurrentDb()->FetchNext())
	{
		//
		//get the record
		//
		QString ns = GetCurrentDb()->GetString("NAME");
		//
		// Find the entry in the table
		//
		for(unsigned j = 0; j < pStatus->count(); j++)
		{
			TableItem *st = pStatus->getCell(j);
			if(st->text() == ns)
			{
				int s = GetCurrentDb()->GetInt("STATE");
				st->setBackColour(GetAlarmStateBkColour(s));    
				st->setTextColour(GetAlarmStateFgColour(s));
				st->SetState(s);
				st->SetFlash(GetCurrentDb()->GetInt("ACKFLAG"));
				break;
			};
		};
	};
	pStatus->updateAll();
};
/*
*Function:QueryResponse
*Configuration DB callback
*Inputs:object, command, id
*Outputs:none
*Returns:none
*/
void StatusDisplay::QueryResponse (QObject *p, const QString &c, int id, QObject*caller)
{
	if(p != this) return;

	IT_IT("StatusDisplay::QueryResponse");

	switch(id)
	{
		case tItem:
		{
			Status.UpdateHeader();
		};
		break;
		case tTags:
		{
			Status.UpdateTagsPane();
		};
		break;
		case tSamplePoint:
		{
			Status.UpdateSamplePoint();
		};
		break;
		case tTagLimits:
		{
			Status.UpdateTagsLimits();
		};
		break;
		case tReceipeRecord:
		{
			IT_COMMENT("tReceipeRecord");
			
			if(GetConfigureDb()->GetNumberResults() > 0)
			{
				QString smp = GetConfigureDb()->GetString("SAMPLES"); // enabled sample points
				//
				QString cmd = "select NAME,UNITS from SAMPLE where NAME in ('" + smp + "');";
				GetConfigureDb()->DoExec(this,cmd,tList); // get the sample point configuration
				//
			};
		};
		break;
		case tList: // list received - process and update the display
		{
			pStatus->clear();  
			int n = GetConfigureDb()->GetNumberResults();
			if(n > 0)
			{
				//  
				// set up the table
				int nr = n /N_STATUS_COLS + 1;
				if(n > 0 && ((n % N_STATUS_COLS) == 0))
				{
					nr--; // exact multiple of N_STATUS_COLS
				};
				pStatus->resizeTable(nr, N_STATUS_COLS);
				// 
				for(int i = 0; i < n ; i++,GetConfigureDb()->FetchNext())
				{
					// we have the name 
					QString name = GetConfigureDb()->GetString("NAME");
					//
					int r = i / N_STATUS_COLS;
					int c = i % N_STATUS_COLS;
					//
					pStatus->setCellText(r,c,name);
					pStatus->setCellTextColour(r,c,Qt::black);
					pStatus->setCellBackColour(r,c,Qt::white);
					//
					SampleMap::value_type pr(GetConfigureDb()->GetString("NAME"),GetConfigureDb()->GetString("UNITS"));
					umap.insert(pr);// add to dict
					//   
				};
			};
			SelChanged();
			pStatus->repaint();
		};
		break;
		default:
		break;
	};
};
/*
*Function: Restart
*handle the restart
*Inputs:none
*Outputs:none
*Returns:none
*/
void StatusDisplay::Restart()
{
	IT_IT("StatusDisplay::Restart");
	
	Status.Clear();
	
	if(GetReceipeName() != "(default)")
	{
		QString cmd = "select * from RECEIPE where NAME='"+ GetReceipeName() +"';";
		GetConfigureDb()->DoExec(this,cmd,tReceipeRecord);
	}
	else
	{ 
		//
		// the default enabled units and sample points
		// 
		GetConfigureDb()->DoExec(this,QString("select NAME,UNITS from SAMPLE order by NAME asc;"),tList); // start by getting everything
	};
}; 
/*
*Function: UpdateTags
*update the tag values displayed
*Inputs:none
*Outputs:none
*Returns:none
*/
void StatusDisplay::UpdateTags()
{
	IT_IT("StatusDisplay::UpdateTags");

	//Here we have set of record from TAGS_DB
	//
	int n = GetCurrentDb()->GetNumberResults();
	GetCurrentDb()->GotoBegin();
	//
	QString lastName = "";
	for(int i = 0; i < n ; i++,GetCurrentDb()->FetchNext())
	{
		QString s = GetCurrentDb()->GetString("NAME");
		if(s != lastName)
		{
			//
			for(unsigned j = 0; j < pStatus->count(); j++)
			{
				TableItem *st = pStatus->getCell(j);
				if(st->text() == s) // do we want this cell
				{
					SampleMap::iterator k = umap.find(s);
					if(!(k == umap.end()))
					{
						double v = atof((const char*)(GetCurrentDb()->GetString("VAL")));
						QString val = QString::number(v,'f',2); //two decimal points of precision
												
						QString t =  val + " " + (*k).second;
						//cerr << (const char *)s << " Setting Text To" << (const char *)t << endl;
						st->setText1(t); //add the value 
					};
					break; // handle the next record
				};
			};
			lastName = s;
		};
		//
	};
	pStatus->repaint();
};

