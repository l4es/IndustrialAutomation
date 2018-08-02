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
*Graph Pane
*Purpose:to display a graph as NOT a time related graph
*/

#include "graphpanedata.h"
#include "general_defines.h"
#include "IndentedTrace.h"
#include <qt.h>
/*
*Function:paintEvent
*handle the paint event
*Inputs:paint event
*Outputs:none
*Returns:none
*/

/*
*Function: FitRect
*Work Out What fits
*Inputs:tip string , area to fit into
*Outputs:none
*Returns:none
*/
QRect GraphTipsData::FitRect(const QString &t, const QRect &r) 
{
	QRect res;
	QFontMetrics fmt(font());
	int w = fmt.width(t); 
	int h = fmt.height();
	QWidget *p = parentWidget();
	if((p->width() > w) && (p->height() > h)) 
	{
		int l = r.left();
		if(r.left() + w > p->width())
		{
			l = (p->width() - w);
		};		
		int y = r.top();
		if(r.top() + h > p->height())
		{
			y = p->height() - h;
		};	
		res = QRect(l,y,w,h);
	};
	return res;
}
/*
*Function: maybeTip
*Inputs:where to plot it
*Outputs:none
*Returns:none
*/
void GraphTipsData::maybeTip(const QPoint &pt) 
{
	if(pP->XRange > 0)
	{
		if(pP->plotRect.contains(pt,TRUE))
		{
			QString s;
			double dx = pt.x() - pP->plotRect.left(); 
			if(dx > 0)
			{
				double dy = pP->plotRect.bottom() - pt.y();
				if(dy > 0)
				{
					double dt = (dx * (double)pP->XRange /(double)pP->plotRect.width());
					double d = pP->minX + dt; 
					double v = dy /(double)pP->plotRect.height() * (pP->scaleMax - pP->scaleMin) + pP->scaleMin;
					s.sprintf("%8.6g",v);
					s = s + " " + QString::number(d);
					//
					QRect r = FitRect(s,pP->plotRect);
					//
					if(!r.isEmpty())
					{
						clear();
						tip(r,s);
					};
				};
			};
		};
	};
};

void GraphPaneData::paintEvent(QPaintEvent *p)
{
	IT_IT("GraphPaneData::paintEvent");
	
	//
	QWidget::paintEvent(p);
	//
	QPainter DC;
	DC.begin(this);
	//
	// Draw the graph
	//   
	TheGraph.Plot(DC,rect(),Qt::white);
	//
	DC.end();
};
/*
*Function:Add
*add a point to a named line
*Inputs:line name, time stamp, value
*Outputs:none
*Returns:none
*/
void GraphPaneData::Add(const QString &name,const double &d,double v) // add a value
{
	//IT_IT("GraphPaneData::Add");
	
	TheGraph.Update(name,d,v);   
};
/*
*Function: Clear
*clear a graph
*Inputs:none
*Outputs:none
*Returns:none
*/
void GraphPaneData::Clear() // clear the graph
{
	IT_IT("GraphPaneData::Clear");
	
	TheGraph.Clear();
	TheGraph.ScaleGraph();
	repaint();       
};
/*
*Function: EraseDataMap
*erase a datamap
*Inputs:name of the data map
*Outputs:none
*Returns:none
*/
void GraphPaneData::EraseDataMap(QString& data_map_name) 
{
	IT_IT("GraphPaneData::EraseDataMap");
	
	TheGraph.EraseDataMap(data_map_name);
	TheGraph.ScaleGraph();
	repaint();       
};

/*
*Function: SetLogMode
*Inputs:true if log mode on
*Outputs:none
*Returns:none
*/
void GraphPaneData::SetLogMode(bool f) // set log/lin mode
{
	IT_IT("GraphPaneData::SetLogMode");

	TheGraph.SetLogMode(f);
};
/*
*Function:Update
*triggers an update
*Inputs:none
*Outputs:none
*Returns:none
*/
void GraphPaneData::Update()
{
	IT_IT("GraphPaneData::Update");

	TheGraph.ScaleGraph();
	repaint();
};
/*
*Function:mousePressEvent
*Inputs:none
*Outputs:none
*Returns:none
*/
void GraphPaneData::mousePressEvent ( QMouseEvent *pEvent)
{
	IT_IT("GraphPaneData::mousePressEvent");
	
	switch(pEvent->button())
	{
		case RightButton:
		{
			emit RightClick();
		};
		break;
		default:
		break;  
	};
};
/*
*Function: Print
*Inputs:none
*Outputs:none
*Returns:none
*/
void GraphPaneData::Print()
{
	IT_IT("GraphPaneData::Print");
	
	QPrinter prt; 
	prt.setDocName(tr("Pen Trace"));
	prt.setCreator(tr(SYSTEM_NAME));
	prt.setOrientation(QPrinter::Landscape);
	prt.setOutputFileName("~out.ps");
	prt.setOutputToFile(false);
	//
	if(prt.setup(this))
	{
		//
		// Handle the case of no printer being selected
		//
		if(!prt.printerName().isEmpty())
		{
			QPainter p;
			p.begin(&prt);
			QPaintDeviceMetrics metrics(p.device());
			//
			int dpix = metrics.logicalDpiX() ; //  inch border
			int dpiy = metrics.logicalDpiY() ;
			//
			QRect body(dpix, dpiy,	metrics.width()  -  dpix*6,	metrics.height() -  dpiy*2);
			TheGraph.Plot(p,body,Qt::white);
			//
			QFont font("times", 8); 
			p.setFont(font);
			//
			p.drawText( body.left() ,body.top(), Title);
			//
			p.end();
			//
		}
		else
		{
			QMessageBox::information(this,tr("Print Graph Error"),tr("No Printer Selected!"));
		};
	};
};
/*
*Function:resizeEvent
*Inputs:none
*Outputs:none
*Returns:none
*/
void GraphPaneData::resizeEvent(QResizeEvent *P)
{
	IT_IT("GraphPaneData::resizeEvent");
	
	QWidget::resizeEvent(P);  
};

