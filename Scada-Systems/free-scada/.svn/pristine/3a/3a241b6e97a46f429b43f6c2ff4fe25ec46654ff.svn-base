#include "common.h"
#include "VariableMenager.h"
#include "MemFile.h"
#include ".\objgraph.h"
#include "Action.h"
#include "ActionCurve.h"
#include "ActionMenager.h"
#include "ObjectMenager.h"



CObjGraph::CObjGraph()
{
	SetName(GenerateUniqueObjectName(L"Graph_"));
	SetTypeID(_GRAPH);
	SetActionFlags(AT_ALL_NOROTATE|AT_CURVE);
	//m_nBkColor  = m_pGraph->GetBackColor();
    m_Graph.EnableMemoryDraw(false);
    m_Graph.EnableLegend(false);
	m_Graph.SetYDecimal(2);
    //m_pGraph->SetBackColor(m_nBkColor);
	_GraphTime=600;
}

CObjGraph::~CObjGraph()
{

}

void CObjGraph::recalcBounds()
{
	CObjectMenagerPtr mng;
	sVector points[4];
	points[0] = GetSize() / -2;
	points[1] = sVector(GetSize().x / -2, GetSize().y / 2);
	points[2] = GetSize() / 2;
	points[3] = sVector(GetSize().x / 2, GetSize().y / -2);
	for(int i=0;i<4;i++)
	{
		//поворот и перемещение вершин
		points[i].Rotate(GetAngle());
		points[i] += GetPosition()+GetSize()/2;
		points[i] = mng->GlobalToScreen(points[i]);
	}
	
	RECT rect;
	rect.left=points[0].x;
	rect.top=points[0].y;
	rect.right=points[2].x;
	rect.bottom=points[2].y;
	m_Graph.RecalcRects(rect);
}	

void CObjGraph::Draw(HDC dc)
{
	wstring yat;
	CActionMenagerPtr amng;

	m_Graph.BeginDraw(dc);
	recalcBounds();
	
	HRGN newClip;
	newClip = CreateRectRgn(GetLTBound().x,GetLTBound().y, 
		GetRBBound().x, GetRBBound().y); 
    SelectClipRgn(dc, newClip); 


	m_Graph.SetRatio(0, 0, 1, 1);
	//m_pGraph->DrawBoundary(cr, 2);
	m_Graph.XAxisTitle(L"Time");

	CActionMenager::TActions::iterator it;
	CActionMenager::TActions ac=amng->GetAllActions(GetName(), AT_CURVE);
	time_t minTime=-1;
	double yMax=-10E6,yMin=+10E6;
	bool isScaleSet=false;
	for(it=ac.begin();it!=ac.end();++it)
	{
		
		CActionCurve::_GraphData_t data=((CActionCurve*)(*it))->GetCurveData();
		yat.append((*it)->GetName());
		yat.append(L" ");
		if(!data.empty())
		{
			isScaleSet=true;
			if(data.front().first<(ULONG)minTime)minTime=data.front().first;
			if((*it)->GetValueMin()<yMin) yMin=(*it)->GetValueMin();
			if((*it)->GetValueMax()>yMax) yMax=(*it)->GetValueMax();
		}
	}
	if(minTime==-1) time(&minTime);
	if(!isScaleSet) 
	{
		yMax=0;
		yMin=0;
	}

	m_Graph.YAxisTitle(yat.c_str());
	m_Graph.SetRange(minTime, yMin, minTime+_GraphTime, yMax);
	m_Graph.Title(GetName().c_str());
	m_Graph.Axes();
	m_Graph.Grid();
	m_Graph.EnableLegend(true);
	int j=0;
	for(it=ac.begin();it!=ac.end();++it)
	{	j++;
		CActionCurve::_GraphData_t data=((CActionCurve*)(*it))->GetCurveData();
		CActionCurve::_GraphData_t::iterator itd;
		double *y= new double[data.size()];
		double *x= new double[data.size()];
		int i=0;
		for(itd=data.begin();itd!=data.end();++itd,i++)
		{
			x[i]=(*itd).first;
			y[i]=(*itd).second;
		}
		m_Graph.Lines(x,y,(int)data.size(),((CActionCurve*)(*it))->GetColor().AsCOLORREF(),j,(*it)->GetName().c_str());		
		delete[] x;
		delete[] y;
	}
	SelectClipRgn(dc, NULL); 
	m_Graph.EndDraw(dc);
		
}

void CObjGraph::Serialize(CMemFile& file, BOOL IsLoading)
{
	CObject::Serialize(file, IsLoading);

	if(IsLoading)
	{
		file>>_GraphTime;
	}
	else
	{
		file<<_GraphTime;
		
	}


}