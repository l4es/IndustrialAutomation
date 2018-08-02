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
*Header For: graph window
*Purpose: to plot a graph on the screen
*/

#ifndef include_graphpane_hpp 
#define include_graphpane_hpp 
#include "common.h"
#include "graph.h"

class GraphTips : public QToolTip // the tool tips
{
	Graph *pP;
	public:
	GraphTips(QWidget *parent, Graph *pG) : QToolTip(parent),pP(pG) {};
	QRect FitRect(const QString &t, const QRect &r); 
	void maybeTip(const QPoint &pt); 
};
class GraphPane : public QWidget
{
	Q_OBJECT
	Graph TheGraph; // the graph object
	QString Title;
	protected:
	//
	void paintEvent(QPaintEvent *);
	void mousePressEvent ( QMouseEvent *pEvent);
	void resizeEvent(QResizeEvent *);
	//
	public:
	GraphPane(QWidget *parent, const char *name = 0) : QWidget(parent,name),Title(tr("Graph Plot"))
	{
		(void)(new GraphTips(this,&TheGraph)); // create the tool tip
	};
	void SetMaxPoints(int n) { TheGraph.SetMaxPoints(n);};
	bool HasLine(const QString &s) { return TheGraph.HasLine(s);};
	void GetMinMaxY(double &miny, double &maxy) 
	{
		TheGraph.GetMinMaxY(miny,maxy);
	};

	void SetMinMaxY(double miny, double maxy) 
	{
		TheGraph.SetMinMaxY(miny,maxy);
	};

	void SetTitle(const QString &s) { Title = s; };
	public slots:
	void Add(const QString &,const QDateTime &,double); // add a value
	void Clear(); // clear the graph
	void SetLogMode(bool f); // set log/lin mode
	void Update(); // action the update
	void Print();
	signals:
	void RightClick();
};

#endif

