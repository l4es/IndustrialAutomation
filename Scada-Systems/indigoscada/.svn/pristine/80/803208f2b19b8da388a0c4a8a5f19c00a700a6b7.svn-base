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
*Purpose: to plot NOT time related data
*/

#ifndef include_graphdata_hpp 
#define include_graphdata_hpp 
#include "common.h"
#include <cmath>

//
// min width of legend
#define N_Y_CHAR 10
// rows for legends
#define N_X_CHAR 3
//
class QSEXPORT GraphData
{
	double LastDateX; 
	double    last_v; 
	int MaxPointsPerLine; 
	public:  
	class DataMap :public std::map<double,double, std::less<double> >
	{
		public:
		QColor colour;  
		DataMap(QColor &c) : colour(c) {};
		DataMap(const DataMap &p) : colour(p.colour){}; 
	};
	typedef std::map<QString,DataMap, std::less<QString> > DATAMAP;
	void SeekMaxMinYLine(DataMap &l);  
	void SeekMaxMinXLine(DataMap &l); 
	void PlotXLegend(QPainter &,double &d,int x,int y,int w,int h);
	//
	DATAMAP plotMap; // data
	bool logMode;  // are we in log mode ?
	//
	double minX,  maxX, halfX; // scale bits
	double minY, maxY;
	int abscissaDecimalPrecision;
	//
	double scaleMin, scaleMax, scaleSave, scaleStep, XRange; 
	QRect plotRect; // drawing area for the graphs
	//
	GraphData();
	//
	void Plot(QPainter &p,const QRect &r,QColor bkg = Qt::gray); // draw to DC
	void ScaleGraph();
	void Update(const QString &tag,const double &dt, double v);
	void AddLine(const QString &tag,QColor &c); 
	void GetMinMaxY(double &miny, double &maxy) const {miny = minY;maxy = maxY;};  
	//
	bool HasLine(const QString &name)
	{
		return (!(plotMap.find(name) == plotMap.end()) );
	};
	void SetMaxPoints(int n) { MaxPointsPerLine = n;};
	bool GetLogMode() const { return logMode;};
	void SetLogMode(bool f) { logMode = f;};
	void SetAbscissaPrecision(int p);
	void Clear() { plotMap.clear();};
	void EraseDataMap(QString& data_map_name);
};
#endif

