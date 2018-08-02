/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2014 Enscada 
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
#ifndef include_inspect_hpp 
#define include_inspect_hpp
// database interface
#include "dbase.h" 
#include "driver.h"
#include "graphpane.h"
#include "display.h"
#include "realtimedb.h"
#include "InspectFrameDlg.h"
#include "statuspane.h"

typedef std::map<QString, QObject*, std::less<QString> > ScadaWindowsDict; //the inspect windows dictionary

class Inspect : public QMainWindow
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
	//
	public:
	// pass in the obect name and the time to go back (in secs) - default 2 hours
	Inspect(const QString &name,int dt = (3600*2));
	~Inspect();
	static Inspect* Create(const QString &name, int dt)
	{			
		Inspect * p = new Inspect(name,dt);    
		// connect the inspector to the top level widget which provides the current value update logic
		connect(qApp->mainWidget(),SIGNAL(UpdateTags()),p,SLOT(UpdateTags()));
		p->show();
		//p->setGeometry(QApplication::desktop()->geometry());// maximise to full screen 
		p->setGeometry(3, 20, QApplication::desktop()->width() - 3, QApplication::desktop()->height()-50);

		return p;
	};
	//
	public slots:
	//
	void ConfigQueryResponse (QObject *,const QString &, int, QObject*);  // handles configuration responses
	void ResultsQueryResponse (QObject *,const QString &, int, QObject*); // results responses
	void CurrentQueryResponse (QObject *,const QString &, int, QObject*); // current value responses
	void PointMenu(QListViewItem *, const QPoint &, int); // handle right click on history pain
	void GraphMenu(); // handle right click on graph menu
	void UpdateTags(); // tags have been updated - update graph and table
	void Close();
	//
};

class inspectPopupMenu: public QPopupMenu
{
	Q_OBJECT
	enum { tTagUnit = 1, tUnitType};

	QString command_parent;
	QString command_samplePointName;
	QString command_unit_type;

	public:

	inspectPopupMenu( class QWidget *parent = 0, const char *name = 0);
	//~inspectPopupMenu();
	void get_unit(QWidget* parent, QString name);
	public slots:
	//
	void ConfigQueryResponse (QObject *,const QString &, int, QObject*);  // handles configuration responses
	void DoCommandDialog();
};

void InspectMenu(QWidget *parent, const QString &name, bool AckState);
ScadaWindowsDict* GetInpectWindowsDict();

#endif

