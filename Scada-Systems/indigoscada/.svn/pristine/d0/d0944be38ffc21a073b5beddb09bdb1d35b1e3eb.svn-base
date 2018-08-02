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
*Header For: Graph Pane
*Purpose:to display a graph as part of an inspector or trace
*/

#include "graphpane.h"
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
void GraphPane::paintEvent(QPaintEvent *p)
{
	IT_IT("GraphPane::paintEvent");
	
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
void GraphPane::Add(const QString &name,const QDateTime &d,double v) // add a value
{
	//IT_IT("GraphPane::Add");
	
	TheGraph.Update(name,d,v);   
};
/*
*Function: Clear
*clear a graph
*Inputs:none
*Outputs:none
*Returns:none
*/
void GraphPane::Clear() // clear the graph
{
	IT_IT("GraphPane::Clear");
	
	TheGraph.Clear();
	TheGraph.ScaleGraph();
	repaint();       
};
/*
*Function: SetLogMode
*Inputs:true if log mode on
*Outputs:none
*Returns:none
*/
void GraphPane::SetLogMode(bool f) // set log/lin mode
{
	IT_IT("GraphPane::SetLogMode");

	TheGraph.SetLogMode(f);
};
/*
*Function:Update
*triggers an update
*Inputs:none
*Outputs:none
*Returns:none
*/
void GraphPane::Update()
{
	IT_IT("GraphPane::Update");

	TheGraph.ScaleGraph();
	repaint();
};
/*
*Function: FitRect
*Work Out What fits
*Inputs:tip string , area to fit into
*Outputs:none
*Returns:none
*/
QRect GraphTips::FitRect(const QString &t, const QRect &r) 
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
void GraphTips::maybeTip(const QPoint &pt) 
{
	if(pP->timeRange > 0)
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
					int dt = (int)(dx * (double)pP->timeRange /(double)pP->plotRect.width());
					QDateTime d = pP->minTime.addSecs(dt); 
					double v = dy /(double)pP->plotRect.height() * (pP->scaleMax - pP->scaleMin) + pP->scaleMin;
					s.sprintf("%8.6g",v);
					s = s + " " + d.toString();
					//
					QRect r = FitRect(s,pP->plotRect);
					//
					if(!r.isEmpty())
					{
						tip(r,s);
					};
				};
			};
		};
	};
};
/*
*Function:mousePressEvent
*Inputs:none
*Outputs:none
*Returns:none
*/
void GraphPane::mousePressEvent ( QMouseEvent *pEvent)
{
	IT_IT("GraphPane::mousePressEvent");
	
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
void GraphPane::Print()
{
	IT_IT("GraphPane::Print");
	
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
void GraphPane::resizeEvent(QResizeEvent *P)
{
	IT_IT("GraphPane::resizeEvent");
	
	QWidget::resizeEvent(P);  
};

