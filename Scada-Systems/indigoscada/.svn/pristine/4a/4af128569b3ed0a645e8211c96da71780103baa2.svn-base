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
*Header For: Graph Window 
*Purpose: graph object - not a widget
*/

#include "graph.h"
#include "inifile.h"
#include "IndentedTrace.h"
/*
*Function: Graph
*construct graph object
*Inputs:none
*Outputs:none
*Returns:none
*/
Graph::Graph() :
LastDateTime(QDateTime::currentDateTime()),
last_v(0),MaxPointsPerLine(1000),logMode(0), 
minTime( QDateTime::currentDateTime()), maxTime(QDateTime::currentDateTime()), 
halfTime(QDateTime::currentDateTime()),minY(0.0), maxY(0.0), scaleMin(0), 
scaleMax(1),scaleStep(2), timeRange(0)
{
	IT_IT("Graph::Graph");

	//QString ini_file = GetScadaHomeDirectory() + "\\project\\scada.ini";
	//Inifile iniFile((const char*)ini_file);

	//const char* str;
	//if((str = iniFile.find("MaxPointsPerLine","Graph")) != NULL)
	//{
	//	MaxPointsPerLine = atoi(str);
	//}
};
/*
*Function:SeekMaxMinYLine
*Inputs:none
*Outputs:none
*Returns:none
*/
void Graph::SeekMaxMinYLine(DataMap &l)
{
	IT_IT("Graph::SeekMaxMinYLine");
	
	for(DataMap::iterator j = l.begin(); !(j == l.end());j++)
	{
		if(minY > (*j).second) 
		{
			minY = (*j).second;
		}
		else if(maxY < (*j).second)
		{
			maxY = (*j).second;
		};
	};
};
/*
*Function:
*Inputs:noneSeekMaxMinTimeLine
*Outputs:none
*Returns:none
*/
void Graph::SeekMaxMinTimeLine(DataMap &l)  // seek max and min in a line
{
	IT_IT("Graph::SeekMaxMinTimeLine");
	
	if(l.size() > 0)
	{  
		if(minTime > (*l.begin()).first)
		{
			minTime = (*l.begin()).first;
		}; 
		if(maxTime < (*l.rbegin()).first)
		{
			maxTime = (*l.rbegin()).first;
		};
	}; 
};
/*
*Function:ScaleGraph()
*Inputs:none
*Outputs:none
*Returns:none
*/
void Graph::ScaleGraph()
{
	IT_IT("Graph::ScaleGraph");
	
	//minY = 0; //commented out on 27-10-04
	//maxY = 0; //commented out on 27-10-04
	int npts = 0;
	//  
	if(plotMap.size() <  1) return; // avoid work !
	//  
	minTime = LastDateTime;
	maxTime = LastDateTime;
	//minY = maxY = last_v; //commented out on 27-10-04
	//   
	DATAMAP::iterator i = plotMap.begin();
	for(; !(i == plotMap.end()); i++,npts++)
	{
		if((*i).second.size() > 0)
		{
			// is the first time 
			SeekMaxMinTimeLine((*i).second);
			SeekMaxMinYLine((*i).second);
			npts += (*i).second.size();
		};		
	};
	//
	// we have the time range
	timeRange = minTime.secsTo(maxTime);
	halfTime = minTime.addSecs(timeRange/2);
	scaleMax = scaleSave = scaleMin = 0;
	//
	// Try and get pleasant Y - axis scales
	//
	//
	if((minY >= 0) && (maxY > 0))
	{
		//
		scaleMax = maxY;
		//
		if(minY > 0)
		{
			if((maxY / minY) < 1.2)
			{
				scaleMin = minY;
			}; 				
		}
	}
	else if((minY < 0) && (maxY <= 0))
	{
		scaleMin = minY;
		if(maxY < 0)
		{
			if((minY / maxY) < 1.2)
			{
				scaleMax = maxY;
			}; 				
		}
		logMode = false; 
	}
	else 
	{
		scaleMin = -maxY;
		scaleMax =  maxY;
		if(minY != maxY)
		{
			logMode = false; 
		};
	};
	//
	if(scaleMin == scaleMax)
	{
		scaleMax += 1.0; // they are the same - increment max 
	};
	//
	if(logMode)
	{
		scaleMin = 0;
		if(scaleMax > 1)
		{
			scaleMax = ceil(log10(scaleMax)); // add one on
		};
	};
	scaleSave = scaleMax;
};
/*
*Function: Update
*Inputs:line name , time stamp, value
*Outputs:none
*Returns:none
*/
void  Graph::Update(const QString &tag,const QDateTime &dt, double v)
{
	//IT_IT("Graph::Update");
	
	if(!HasLine(tag))
	{
		static QColor colour_list[] = 
		{
			Qt::black,Qt::darkRed,Qt::darkGreen,Qt::darkBlue,Qt::red,Qt::green,
			Qt::yellow,Qt::cyan
		};
		AddLine(tag,colour_list[plotMap.size() % 8]);
	};
	//
	DATAMAP::iterator j = plotMap.find(tag);    
	DataMap::value_type pv(dt,v); 
	//
	(*j).second.insert(pv); 
	//
	if((int)(*j).second.size() > MaxPointsPerLine)
	{
		(*j).second.erase((*j).second.begin());
	};
	LastDateTime = dt;
	last_v = v;
};
/*
*Function:AddLine
*Inputs:line name, colour
*Outputs:none
*Returns:none
*/
void Graph::AddLine(const QString &tag,QColor &c) // add a line to 
{
	IT_IT("Graph::AddLine");
	
	DataMap P(c); 
	DATAMAP::value_type pr(tag,P); 
	plotMap.insert(pr);
};
/*
*Function:void PlotTimeLegend(QDateTime &d,int x,int y,int w,int h)
*plots a time legend on the x-axis
*Inputs:date/time, x mid point , y top, width , line height
*Outputs:none
*Returns:none
*/
void Graph::PlotTimeLegend(QPainter &DC,QDateTime &d,int x,int y,int w,int h)
{
	IT_IT("Graph::PlotTimeLegend");
	
	DC.drawText(x - w / 2, y + (h * 3)/2,d.time().toString());
	DC.drawText(x - w , y + (h * 5)/2,d.date().toString());
};
/*
*Function:Plot
*draws the graph to the DC
*Inputs:none
*Outputs:none
*Returns:none
*/
void Graph::Plot(QPainter &DC, const QRect &rect,  QColor bkg) 
{
	IT_IT("Graph::Plot");
	
	QFont fnt("times",10); 
	DC.setFont(fnt);
	//
	int lr_border, tb_border, dtwidth; 
	QFontMetrics m(fnt);
	//
	// Workout the legend space needed on the right
	lr_border = N_Y_CHAR * m.width('0'); // left border size
	int rborder = lr_border; // right border size
	tb_border = N_X_CHAR * m.height(); 
	for(DATAMAP::iterator i = plotMap.begin(); !(i == plotMap.end()); i++)
	{
		int w = m.width((*i).first);
		if(w > rborder) rborder = w;
	};
	//
	rborder += m.width('0') * 2;
	dtwidth = m.width("WWW 01 WWW 2000  "); // width of the date string
	//
	//
	bool fTimeLegend = false;
	bool fYLegend = false;
	//
	if(rect.width() > (lr_border * 2 + rborder))
	{
		fYLegend = true;
	}
	//
	if(rect.height() > tb_border * 3)
	{
		if(rect.width() > dtwidth)
		{
			fTimeLegend = true;
		}
	};
	//
	int dx = 0;
	if(fYLegend)
	{	
		dx = lr_border;
	};
	//
	int dy = 0;
	if(fTimeLegend)
	{
		dy = tb_border;
	};
	//
	plotRect = QRect(rect.left() + dx,rect.top() + dy, rect.right() - (dx + rborder), rect.bottom() - 2*dy); // get space
	DC.fillRect(plotRect,QBrush(bkg));
	DC.setPen(Qt::red);
	DC.drawRect(plotRect);
	if(fTimeLegend)
	{
		if(plotRect.width() > lr_border) 
		{
			DC.setPen(Qt::black);
			if(rect.width() > dtwidth *3)
			{
				PlotTimeLegend(DC, minTime,plotRect.left(),plotRect.bottom(),dtwidth/2,m.height());
			};
			PlotTimeLegend(DC, halfTime,(plotRect.left() + plotRect.right())/2,plotRect.bottom(),dtwidth/2,m.height());
			if(rect.width() > dtwidth * 3)
			{
				PlotTimeLegend(DC, maxTime,plotRect.right(),plotRect.bottom(),dtwidth/2,m.height());
			};
			DC.setPen(Qt::lightGray);
			for(int i = 1; i < 4; i++)
			{
				int x = plotRect.left() + (plotRect.width() * i) / 4;
				DC.moveTo(x ,plotRect.bottom());
				DC.lineTo(x ,plotRect.top());
			};
		};
	};
	if(fYLegend)
	{
		scaleMax = scaleSave; 
		if(logMode)
		{
			DC.setPen(Qt::black);
			int nsteps = (int)scaleMax;                        // this is required number of steps
			int maxsteps = plotRect.height() / m.height() / 2; // this is the max number of steps we can have
			//
			if(nsteps > maxsteps)
			{
				// not enough room so put only the top and bottom values
				QString s;
				DC.drawText(	2, plotRect.bottom(),		"1");
				s.sprintf("%8.5g",pow(scaleMax,10));
				DC.drawText(	2, plotRect.top(),		s);
			}
			else if(nsteps > 0)
			{
				int dy = plotRect.height() / nsteps;
				//
				double sc = 1.0;
				//
				QString s;
				for(int kk = 0; kk <  nsteps; kk++)
				{
					s.sprintf("%8.5g",sc);
					DC.drawText(	2, plotRect.bottom() - kk * dy,		s);
					sc *=10;
				};
				//
				s.sprintf("%8.5g",sc);
				DC.drawText(	2, plotRect.top(),		s);
				//
				for(int k = 0 ; k < nsteps; k++)
				{
					int y = plotRect.bottom() - k * dy;
					DC.setPen(Qt::lightGray);
					DC.moveTo(plotRect.left(), y);
					DC.lineTo(plotRect.right(), y);
					//
					// we are in log mode - maybe we should add some log graph lines too
					// is there enough space for some sub interval lines ?
					//
					if(dy > 40)
					{				
						static int ltab[] = 
						{
							//2,3,4,5,6,7,8,9
							3010,4771,6021,6990,7782,8451,9031,9542
						};
						QPen dpen(Qt::lightGray,0,Qt::DotLine);
						DC.setPen(dpen);
						for(int j = 0 ; j < 8; j++)
						{
							int z = y - dy * ltab[j] / 10000; // get the sub division
							DC.moveTo(plotRect.left(), z);
							DC.lineTo(plotRect.right(),z);
						};
					};
				};
			};
		}
		else
		{
			DC.setPen(Qt::black);
			int nsteps = (plotRect.height() / m.height() / 3) + 1; 
			if((nsteps > 1) && (timeRange > 0))
			{
				//
				if((scaleMin == 0) && (scaleMax > 2))
				{
					scaleMax = ceil(scaleMax); 
					//
					if(nsteps > (int)scaleMax) 
					{
						nsteps = (int)scaleMax;			
					}
					else
					{
						double pt = floor(log10(scaleMax)); 
						//
						// Power of ten
						// 
						pt = pow(10.0,pt); 
						int ns = (int)(scaleMax / pt) + 1;
						if(ns <= nsteps)
						{
							nsteps = ns;
						}
						else
						{
							double r = scaleMax; 
							do
							{
								pt *= 2;
								r = ((double)nsteps) * pt;
							} while(r < scaleMax);
						};
						scaleMax = ((double)nsteps) * pt;
					};
				};
				//
				double sy = 0.0;
				int dy = 0;
				if(nsteps != 0)
					sy = (scaleMax - scaleMin) / (double)nsteps;
				if(nsteps != 0)
					dy = plotRect.height() / nsteps;
				QString s;
				// handle the problem of small steps
				const char *Format = (sy < 1.0)?"%8.5g":"%6.1f";
				for(int kk = 0; kk <  nsteps; kk++)
				{
					s.sprintf(Format,scaleMin + sy * (double)kk);
					DC.drawText(	2, plotRect.bottom() - kk * dy,		s);
				};
				//
				s.sprintf(Format,scaleMax);
				//
				DC.drawText(	2, plotRect.top(),		s);
				//
				//
				for(int k = 0 ; k < nsteps; k++)
				{
					int y =  plotRect.bottom() - k * dy;
					DC.setPen(Qt::lightGray);
					DC.moveTo(plotRect.left(), y);
					DC.lineTo(plotRect.right(), y);
					//
					// How many sub lines to we add in 
					// 
					// we want 2, 4 or 10
					int nl = dy / 5;
					if(nl > 1)
					{
						if(nl > 9)
						{
							nl = 9;
						}
						else if(nl < 4)
						{
							nl = 1;
						}
						else
						{
							nl = 3;
						};
						for(int j = 1; j <= nl; j++)
						{
							int z = y - dy * j / (nl + 1);
							QPen dpen(Qt::lightGray,0,Qt::DotLine);
							DC.setPen(dpen);
							DC.moveTo(plotRect.left(), z);
							DC.lineTo(plotRect.right(), z);
						};
					};
				};
			};
		};
	};
	//  
	if(timeRange > 0)
	{
		int nc = 0; 
		for(DATAMAP::iterator i = plotMap.begin(); !(i == plotMap.end()); i++, nc++)
		{
			if((*i).second.size() > 0)
			{
				double dx = plotRect.width();
				double dy = scaleMax - scaleMin;
				if(dy)
				{
					DC.setPen((*i).second.colour);
					dy = (double)plotRect.height() / dy;
					for(DataMap::iterator j = (*i).second.begin(); !(j == (*i).second.end());j++)
					{
						double x = (double) minTime.secsTo((*j).first) / timeRange * dx;
						double y = 0.0;
						//
						if(logMode)
						{
							if((*j).second > 1)
							{
								y = log10((*j).second) * dy; 
							};
						}
						else
						{
							y = ((*j).second - scaleMin) * dy;
						};
						//
						if(j ==  (*i).second.begin())
						{
							DC.moveTo((int)x + plotRect.left(),plotRect.bottom() - (int)y); 
						}
						else
						{
							DC.lineTo((int)x + plotRect.left(),plotRect.bottom() - (int)y); 
						};
					};
				}; 
				DC.drawText(plotRect.right() + m.width('0'),plotRect.top() + nc * (m.height() + 2),(*i).first);
			};
		};

		DC.drawText(plotRect.right() + m.width('0'),plotRect.bottom(), GetNames());		
	};	
};

