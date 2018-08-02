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
*Header For: graph plotting code
*Purpose: to plot time related data
*/

#ifndef include_graph_hpp 
#define include_graph_hpp 
#include "common.h"
#include <cmath>

//
// min width of legend
#define N_Y_CHAR 10
// rows for legends
#define N_X_CHAR 3
//
class QSEXPORT Graph
{
	QDateTime LastDateTime; 
	double    last_v; 
	int MaxPointsPerLine; 
	public:  
	class DataMap :public std::map<QDateTime,double, std::less<QDateTime> >
	{
		public:
		QColor colour;  
		DataMap(QColor &c) : colour(c) {};
		DataMap(const DataMap &p) : colour(p.colour){}; 
	};
	typedef std::map<QString,DataMap, std::less<QString> > DATAMAP;
	void SeekMaxMinYLine(DataMap &l);  
	void SeekMaxMinTimeLine(DataMap &l); 
	void PlotTimeLegend(QPainter &,QDateTime &d,int x,int y,int w,int h);
	//
	DATAMAP plotMap; // data
	bool logMode;  // are we in log mode ?
	//
	QDateTime minTime,  maxTime, halfTime; // scale bits
	double minY, maxY;
	//
	double scaleMin, scaleMax, scaleSave, scaleStep, timeRange; 
	QRect plotRect; // drawing area for the graphs
	//
	Graph();
	//
	void Plot(QPainter &p,const QRect &r,QColor bkg = Qt::gray); // draw to DC
	void ScaleGraph();
	void Update(const QString &tag,const QDateTime &dt, double v);
	void AddLine(const QString &tag,QColor &c); 
	void GetMinMaxY(double &miny, double &maxy) const {miny = minY;maxy = maxY;};  
	void SetMinMaxY(double miny, double maxy) {minY = miny;maxY = maxy;};  
	//
	bool HasLine(const QString &name)
	{
		return (!(plotMap.find(name) == plotMap.end()) );
	};
	void SetMaxPoints(int n) { MaxPointsPerLine = n;};
	bool GetLogMode() const { return logMode;};
	void SetLogMode(bool f) { logMode = f;};
	void Clear() { plotMap.clear();};
};
#endif

