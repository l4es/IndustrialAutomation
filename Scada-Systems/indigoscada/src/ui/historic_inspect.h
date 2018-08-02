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
* Header For: inspector classes
* Purpose: these provide a sample point level 
* 
*/
#ifndef include_historic_inspect_hpp 
#define include_historic_inspect_hpp
// database interface
#include "dbase.h" 
#include "driver.h"
#include "graphpane.h"
#include "display.h"
#include "realtimedb.h"
#include "InspectFrameDlg.h"
#include "statuspane.h"

class HistoricInspect : public QMainWindow
{
	//
	Q_OBJECT
	//
	enum { tItem = 1, tResults,tTags,tSamplePoint,tTagLimits};
	QString Name; // sample point name
	QDateTime LastTime; // last time
	int maxItems; // number of items in a list
	StateListItem *pItem; // current item being added to 
	//
	QListView *pHistory;
	GraphPane *pChart;
	StatusPane Status;

	QSplitter *pSm;
	QSplitter *pS;

	QLineEdit *lined2;

	QToolBar       *pToolBarBrowse;
	__int64 MoreOldRecord;
	__int64 MoreRecentRecord;
	__int64 timestamp;

	//
	public:
	// pass in the obect name and the time to go back (in secs) - default 2 hours
	HistoricInspect(const QString &name,int dt = (3600*2));
	~HistoricInspect();
	static HistoricInspect* Create(const QString &name, int dt)
	{			
		HistoricInspect * p = new HistoricInspect(name,dt);    
		// connect the inspector to the top level widget which provides the current value update logic
		//connect(qApp->mainWidget(),SIGNAL(UpdateTags()),p,SLOT(UpdateTags()));
		p->show();
		//p->setGeometry(QApplication::desktop()->geometry());// maximise to full screen 
		p->setGeometry(3, 20, QApplication::desktop()->width() - 3, QApplication::desktop()->height()-50);

		return p;
	};
	//
	public slots:
	//
	void ConfigQueryResponse (QObject *,const QString &, int, QObject*);  // handles configuration responses
	void ResultsQueryResponse (QObject *,const QString &, int, QObject*); // results responses from historic database
	void CurrentQueryResponse (QObject *,const QString &, int, QObject*); // current value responses
	void PointMenu(QListViewItem *, const QPoint &, int); // handle right click on history pain
	void GraphMenu(); // handle right click on graph menu
	void UpdateTags(); // tags have been updated - update graph and table
	void Close();
	//
	//Toolbar slots
	void first();
	void before();
	void next();
	void last();

	void Changed(const QString &);
};

#endif

