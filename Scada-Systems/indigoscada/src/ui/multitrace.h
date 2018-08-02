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
*Header For: Multiple sample point trace window
*Purpose: to plot several sample points on one graph
*/
#ifndef include_multitrace_hpp 
#define include_multitrace_hpp
//
// the multitrace window
//
#include <qt.h> 
#include "dbase.h" 
#include "driver.h"
#ifdef QWT_TRACE
#include "graphpaneqwt.h"
#else
#include "graphpane.h"
#endif

#include "display.h"
#include "realtimedb.h"
#include "statuspane.h"
class MultiTraceDlg;
class Multitrace : public QMainWindow
{
	Q_OBJECT
	enum {tList=1,tData};
	//
	typedef std::map <QString, QCheckListItem *, std::less<QString> > ItemDict; // index to tags (to check) items
	typedef std::map <QString, ItemDict, std::less<QString> > SpDict; // sample point to tags
	SpDict Items;
	int maxItems; // number of items in a list
	long back_in_time;
	#ifdef QWT_TRACE
	GraphPaneQwt *pChart; // the graph
	#else
	GraphPane *pChart; // the graph
	#endif
	QListView *List; // the setup bits

	QSplitter *pSm;
	//
	public:
	Multitrace(); // top level window - so no parent

	~Multitrace() 
	{
		if(List)
		{
			delete List;
			List = NULL;
		}
		if(pChart)
		{
			delete pChart;
			pChart = NULL;
		}
		if(pSm)
		{
			delete pSm;
			pSm = NULL;
		}
	};

	public slots:
	//
	void Load();    // load a plot
	void Save();    // save a plot
	void Print();   // print a plot
	void Close();   // close window
	void UpdateTags(); 
	//
	void ConfigQueryResponse (QObject *,const QString &, int, QObject*);  // handles configuration responses
	void ResultsQueryResponse (QObject *,const QString &, int, QObject*); // results responses
	void CurrentQueryResponse (QObject *,const QString &, int, QObject*); // current value responses
	void LogScalesOn();
	void LinScalesOn();
	void Check();
	//
};
#endif

