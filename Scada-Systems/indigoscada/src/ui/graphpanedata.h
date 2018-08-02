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
*graph window
*Purpose: to plot a graph on the screen
*/

#ifndef include_graphpanedata_hpp 
#define include_graphpanedata_hpp 
#include "common.h"
#include "graphdata.h"

class GraphTipsData : public QToolTip // the tool tips
{
	GraphData *pP;
	public:
	GraphTipsData(QWidget *parent, GraphData *pG) : QToolTip(parent),pP(pG) {};
	QRect FitRect(const QString &t, const QRect &r); 
	void maybeTip(const QPoint &pt); 
};

class GraphPaneData : public QWidget
{
	Q_OBJECT
	GraphData TheGraph; // the graph object
	QString Title;
	protected:
	//
	void paintEvent(QPaintEvent *);
	void mousePressEvent ( QMouseEvent *pEvent);
	void resizeEvent(QResizeEvent *);
	//
	public:
	GraphPaneData(QWidget *parent, const char *name = 0) : QWidget(parent,name),Title(tr("Graph Plot"))
	{
		(void)(new GraphTipsData(this,&TheGraph)); // create the tool tip
	};
	void SetMaxPoints(int n) { TheGraph.SetMaxPoints(n);};
	bool HasLine(const QString &s) { return TheGraph.HasLine(s);};
	void GetMinMaxY(double &miny, double &maxy) 
	{
		TheGraph.GetMinMaxY(miny,maxy);
	};
	void SetTitle(const QString &s) { Title = s; };
	public slots:
	void Add(const QString &,const double &,double); // add a value
	void Clear(); // clear the graph
	void EraseDataMap(QString& data_map_name); //erase a datamap
	void SetLogMode(bool f); // set log/lin mode
	void SetAbscissaPrecision(int p) {TheGraph.SetAbscissaPrecision(p);};
	void Update(); // action the update
	void Print();
	signals:
	void RightClick();
};

#endif

