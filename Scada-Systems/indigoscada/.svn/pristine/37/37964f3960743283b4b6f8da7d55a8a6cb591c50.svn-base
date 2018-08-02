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
*Header For: status pane set manager
*Purpose:
*/

#ifndef include_statuspane_hpp 
#define include_statuspane_hpp
#include "dbase.h"
#include "realtimedb.h"
#include "display.h"
#include "inifile.h"

class StatusListView : public QListView
{
	public:

	StatusListView(QWidget *parent, const QString &name = QString::null) : QListView(parent) 
	{
	};
	void resizeEvent(QResizeEvent *e)
	{
		QListView::resizeEvent(e); // action the resize
		
		#ifdef SCOMMENTARE	//APA 05-12-01: Questo blocco di codice e' rimosso poiche'
					//l'aggiustamento della colonna piu' a dx crea problemi
			// adjust the size of the right most column if necessary
			int s = 0;
			int n = columns() - 1;
			//
			for(int i = 0; i < n; i++)
			{
				s += columnWidth(i); // where is the max width
			};
			//
			if(e->size().width() > s)
			{
				int w = columnWidth(n) + (e->size().width() - s);
				setColumnWidth(n,w); 
			};
		#endif

	}; // the resize event
};
// locus for managing status pane
class StatusPane : public QObject
{
	Q_OBJECT
	QLabel *pHeader;         // header detials 
	QListView *pPointStatus; // the sample point level status
	QListView *pTagStatus;   // tag status 
	QListView *pTagAlarms;   // the tag by tag alarm limits
	QString Name;
	public:  
	StatusPane()
	{  
	};

	void Create(QWidget *parent,Orientation mode = Qt::Vertical)
	{
		QString ini_file = GetScadaHomeDirectory() + "\\project\\scada.ini";
		Inifile iniFile((const char*)ini_file);


		QSplitter * pSs = new QSplitter (mode,parent); //=> vertical sub bar 1
		pSs->setFrameStyle(QFrame::Sunken | QFrame::WinPanel);

		pPointStatus = new  StatusListView(pSs); 

		pSs->setResizeMode (pPointStatus, QSplitter::Stretch);

		#ifdef UNIX
		(pPointStatus->header())->setFont(QFont("helvetica", 14));
		#endif
		#ifdef WIN32
		(pPointStatus->header())->setFont(QFont("Arial", 14));
		#endif
		//pPointStatus->setSorting(-1);
		pPointStatus->addColumn("");  // 0 
		pPointStatus->addColumn("");  // 1

		disconnect(pPointStatus->header(), SIGNAL(sectionClicked(int)),pPointStatus, SLOT(changeSortColumn(int)));
		//
		// Tags Panes
		QSplitter * pSt = new QSplitter (Qt::Vertical,pSs); //=> horizontal sub bar

		pSs->setResizeMode (pSt, QSplitter::Stretch);

		QValueList<int> l;

		const char* str;
		if((str = iniFile.find("pSsSize1","inspect_window")) != NULL)
		{
			l.append(atoi(str));
		}
		if((str = iniFile.find("pSsSize2","inspect_window")) != NULL)
		{
			l.append(atoi(str));
		}

		pSs->setSizes(l);

		pTagStatus     = new StatusListView(pSt); // selected value tag list
		pTagAlarms     = new StatusListView(pSt); // the limits 
		//
		//pTagStatus->setSorting(-1);
		pTagStatus->addColumn(tr("Name      "));     // 0 
		pTagStatus->addColumn(tr("Value     "));    // 1
		pTagStatus->addColumn(tr("Minimum   "));  // 2
		pTagStatus->addColumn(tr("Maximum   "));  // 3
		pTagStatus->addColumn(tr("Average   "));  // 4
		pTagStatus->addColumn(tr("Std. Dev. "));  // 5
		pTagStatus->addColumn(tr("Updated   "));  // 6
		//
		disconnect(pTagStatus->header(), SIGNAL(sectionClicked(int)),pTagStatus, SLOT(changeSortColumn(int)));
		//
		pTagAlarms->addColumn(tr("Name"));
		pTagAlarms->addColumn(tr("Lower Alarm"));
		pTagAlarms->addColumn(tr("Lower Warning"));
		pTagAlarms->addColumn(tr("Upper Warning"));
		pTagAlarms->addColumn(tr("Upper Alarm"));
		//
		disconnect(pTagAlarms->header(), SIGNAL(sectionClicked(int)),pTagAlarms, SLOT(changeSortColumn(int)));
		//	
		//l.clear();
		//l.append(pSs->width() / 2);
		//l.append(pSs->width() / 2);
		//pSs->setSizes(l);
		//
		pTagAlarms->setItemMargin(5);
		pTagStatus->setItemMargin(5);
		pPointStatus->setItemMargin(5);
		//
		// It is a given that the top level widget handles current value updates
		connect(qApp->mainWidget(),SIGNAL(UpdateTags()),this,SLOT(UpdateTagsPane()));
		connect(qApp->mainWidget(),SIGNAL(UpdateSamplePoint()),this,SLOT(UpdateSamplePoint()));
		//
	};
	void UpdateHeader() // update the header pane - has the sample point details
	{
		pPointStatus->setColumnWidth(0,(pPointStatus->header())->fontMetrics().width(Name) + 10);
		pPointStatus->setColumnText(0,Name);
		QString s = UndoEscapeSQLText(GetConfigureDb()->GetString("COMMENT")) + "  ";// add two spaces
		pPointStatus->setColumnWidth(1,(pPointStatus->header())->fontMetrics().width(s));
		pPointStatus->setColumnText(1,s);
		pTagStatus->setColumnText(1, GetConfigureDb()->GetString("UNITS"));
		//
		pTagStatus->resize(pTagStatus->size()); // trigger widths
		pPointStatus->resize(pPointStatus->size());
		//
	};
	void SetName(const QString &s)  { Name = s;}; // set the object name
	const QString & GetName() const { return Name;}; // return the name of the point
	//
	void Clear()
	{
		pTagStatus->clear();
		pPointStatus->clear();
		pTagAlarms->clear();
		Name = QString::null;
	};
	public slots: 
	void UpdateTagsPane(); 
	void UpdateSamplePoint();
	void UpdateTagsLimits();
};
#endif

