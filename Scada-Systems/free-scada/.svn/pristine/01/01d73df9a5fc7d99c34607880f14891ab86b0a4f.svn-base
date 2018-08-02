#include "StdAfx.h"
#include ".\graphicview.h"

//#import "libid:{8996B0A4-D7BE-101B-8650-00AA003A5593}"

wstring CGraphicView::cLegendX::GetString(double val)
{
	COleDateTime dt;
	dt.m_dt = val;

	wstring str;
	str = dt.Format(L"%d.%m %H:%M:%S");
	return str;
}
wstring CGraphicView::cLegendY::GetString(double val)
{
	wchar_t str[0xf];
	wsprintf(str, L"%.3f", val);
	return str;
}

BOOL CGraphicView::PreTranslateMessage(MSG* /*pMsg*/)
{
	return FALSE;
}

LRESULT CGraphicView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	_graph.Create(m_hWnd, CWindow::rcDefault, L"Graph", WS_VISIBLE|WS_CHILD|WS_BORDER, NULL);
	
	_graph.SetLegendCallback(&fLegendX, &fLegendY);
	_graph.SetIntervalX(120);
	return 0;
}

LRESULT CGraphicView::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	WORD Width	= LOWORD(lParam);
	WORD Height	= HIWORD(lParam);

	_graph.MoveWindow(0,0,Width,Height);
	return 0;
}

LRESULT CGraphicView::OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return TRUE;
}

void CGraphicView::OnUpdate()
{
	CDocument* pDoc = (CDocument*)GetDocument();

	if(pDoc->m_ActiveView != CDocument::AV_GRAPH)
		return;

	if(pDoc->m_GraphicType == CDocument::GT_STATIC)
	{
		BuildStaticGraph(pDoc);
	}
}

void CGraphicView::BuildStaticGraph(CDocument* pDoc)
{
	CWaitCursor wait;
	_graph.SetInterMethod(pDoc->m_ApproxMethod);
	_graph.DeleteAllTrends();
	_graph.SetPointsOnScreen(pDoc->m_GraphicPoints);
	CString str = pDoc->m_ToDateTime.Format(_T("%m/%d/%Y %H:%M:%S"));
	str = pDoc->m_FromDateTime.Format(_T("%m/%d/%Y %H:%M:%S"));

	double RightSpace = 0;
	CDCHandle dc = _graph.GetDC();
	for(CTagDB::TVChannel::iterator j=pDoc->m_GraphChannels.begin();j!=pDoc->m_GraphChannels.end();j++)
	{
		CTagDB::TVChannelData chnls;
		chnls = pDoc->m_DB.GetData(j->id, pDoc->m_FromDateTime, pDoc->m_ToDateTime);
		CGraph::TVPoints vec;
		vec.reserve(chnls.size());
		for(CTagDB::TVChannelData::iterator i=chnls.begin();i!=chnls.end();i++)
		{
			CGraph::sPoint d;
			d.y	= i->val;
			d.x = i->DateTime.m_dt;
			vec.push_back(d);
		}
		
		_graph.AddTrend(wstring(j->Name), vec, (BYTE)pDoc->m_ApproxDegree);
		
		CSize sz;
		dc.GetTextExtent(j->Name, j->Name.GetLength(), &sz);
		RightSpace = max(RightSpace, sz.cx + 10);
	}
	_graph.ReleaseDC(dc);

	_graph.SetRightSpaces(RightSpace);
	_graph.RedrawWindow();
}