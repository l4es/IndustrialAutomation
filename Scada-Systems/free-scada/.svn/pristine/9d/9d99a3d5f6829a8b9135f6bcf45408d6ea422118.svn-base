/////////////////////////////////////////////////////
//	rolloutctrl.cpp	
//	Rollout Control implementation
//
//	Written by Alexey Shirshov (qqqaqa@mail.ru)
//	Copyright (c) 2003 Alexey Shirshov
//
//
//
//

#include "stdafx.h"
#include "rolloutctrl.h"

/////////////////////////////////////////////////////
//CRolloutHolder implementation


void AWTL::CRolloutContainerT::_DoPaint(CDCHandle dc)
{
	for(int i = 0;i < m_RolloutCtrls.GetSize();i++)
	{
		CRect r;
		Rollout& ro = *m_RolloutCtrls[i];
		::GetClientRect(ro.hWnd,&r);
		
		r.right += inner_margins.cx*2;
		
		r.OffsetRect(outer_margins.cx,GetTopForButton(i)+9);

		if (IsRolloutCollapsed(i)){
			r.bottom += inner_margins.cy*2;
		}
		else{
			r.bottom = r.top + inner_margins.cy*2;
		}

		if (fTrans)
			dc.DPtoLP(&r);
		
		dc.DrawEdge(r,BDR_SUNKENOUTER,BF_RECT);
		r.DeflateRect(1,1);
		dc.DrawEdge(r,BDR_RAISEDINNER,BF_RECT);
	}
}

int AWTL::CRolloutContainerT::GetTopForButton(int idx) const
{
	int top = outer_margins.cy+yOffset;
	
	for(int i = 0;i < idx;i++)
	{
		Rollout& ro = *m_RolloutCtrls[i];
		CRect r;

		top += 9;

		if (IsRolloutCollapsed(i)){
			::GetClientRect(ro.hWnd,&r);
		}
		else{
			r.SetRect(0,0,0,0);
		}
		
		r.bottom += inner_margins.cy*2;

		top += r.Height();

		//border 4*1
		top += 4;

		top += ySpacing;
	}

	return top;
}

int AWTL::CRolloutContainerT::RecalcButtons() const
{
	int top = outer_margins.cy+yOffset;
	
	for(int i = 0;i < m_RolloutCtrls.GetSize();i++)
	{
		Rollout& ro = *m_RolloutCtrls[i];

		ro.rloButton.SetWindowPos(NULL,
			outer_margins.cx+inner_margins.cx,top,0,0,
			SWP_NOZORDER|SWP_NOSIZE);

		CRect r;

		top += 9;

		top += inner_margins.cy;
		
		::SetWindowPos(ro.hWnd,NULL,
			outer_margins.cx+inner_margins.cx,top,0,0,
			SWP_NOZORDER|SWP_NOSIZE);

		::InvalidateRect(ro.hWnd,NULL,TRUE);
		
		if (IsRolloutCollapsed(i)){
			::GetClientRect(ro.hWnd,&r);
		}
		else{
			r.SetRect(0,0,0,0);
		}
		
		r.bottom += inner_margins.cy;

		top += r.Height();

		//berder
		top += 4;

		top += ySpacing;
	}

	return top;
}

int AWTL::CRolloutContainerT::RecalcWidth(int iWidth) const
{
	int top = outer_margins.cy+yOffset;

	for(int i = 0;i < m_RolloutCtrls.GetSize();i++)
	{
		Rollout& ro = *m_RolloutCtrls[i];
		CRect r;

		ro.rloButton.GetClientRect(&r);
		
		ro.rloButton.SetWindowPos(NULL,
			outer_margins.cx+inner_margins.cx,top,iWidth,r.Height(),
			SWP_NOZORDER);

		top += 9;

		top += inner_margins.cy;
		
		::GetClientRect(ro.hWnd,&r);

		::SetWindowPos(ro.hWnd,NULL,
			outer_margins.cx+inner_margins.cx,top,iWidth,r.Height(),
			SWP_NOZORDER);

		::InvalidateRect(ro.hWnd,NULL,TRUE);
		
		if (IsRolloutCollapsed(i)){
			::GetClientRect(ro.hWnd,&r);
		}
		else{
			r.SetRect(0,0,0,0);
		}
		
		r.bottom += inner_margins.cy;

		top += r.Height();

		//border
		top += 4;

		//spacing
		top += ySpacing;
	}

	return top;
}

bool AWTL::CRolloutContainerT::Expand(int idx)
{
	_DASSERT(idx >= 0 && idx < m_RolloutCtrls.GetSize()){
		return false;
	}

	ExpandRollout(idx,true);
	_Invalidate();

	::ShowWindow(m_RolloutCtrls[idx]->hWnd,SW_SHOWNORMAL);
	
	RecalcButtons();
	return true;
}

bool AWTL::CRolloutContainerT::Collapse(int idx)
{
	_DASSERT(idx >= 0 && idx < m_RolloutCtrls.GetSize()){
		return false;
	}

	ExpandRollout(idx,false);
	_Invalidate();

	::ShowWindow(m_RolloutCtrls[idx]->hWnd,SW_HIDE);
	RecalcButtons();
	return true;
}

void AWTL::C3DStudioRolloutContainer::DoPaint(CDCHandle dc)
{
	CRect rc(GetSliderRect());
	
	dc.DrawEdge(&rc, BDR_SUNKENOUTER, BF_RECT);

	rc.DeflateRect(2,2);

	CRect r;
	GetClientRect(&r);

	double k = double(rc.Height())/_height;
	if (_height > r.Height()){
		rc.bottom = long(r.Height()*k);
		rc.OffsetRect(0,_ptOffset.y);
	}
	
//	if (rc.bottom <= 0)
//		rc.bottom = rc.top+2;

	dc.FillRect(&rc, clrActiveCaption);

	dc.SetViewportOrg(-_ptOffset.x, -long(_ptOffset.y/k));

	_DoPaint(dc);
}

LRESULT AWTL::C3DStudioRolloutContainer::OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
	CPoint p(lParam);
	if (GetCapture() == m_hWnd){
		Scroll(p.y-_ptStartDrag.y);
	}
	else{
		CRect r(GetSliderRect());
		r.InflateRect(4,0);
		if (r.PtInRect(p)){
			SetCursor(_hCursorHand);
		}
	}
	return 1;
}

void AWTL::C3DStudioRolloutContainer::Scroll(int newY)
{
	CRect r;
	GetClientRect(&r);

	CRect rc(GetSliderRect());
	rc.DeflateRect(2,2);
	double k = double(rc.Height())/_height;
	rc.bottom = long(r.Height()*k);

	rc.OffsetRect(0,prev+newY);

	bool b = false;
	r.right = GetWidth()+outer_margins.cx*2+
		inner_margins.cx*2+4;

	static bool in_t = false;
	static bool in_b = false;

	if (rc.top >= 4){
		in_t = false;
		if (rc.bottom <= r.Height()-4){			
			in_b = false;
			yOffset = -long((prev+newY)/k);

			//CSize p(0,long((cur-newY)/k));
			//CWindowDC dc(m_hWnd);
			//dc.DPtoLP(&p);
			//ScrollWindowEx(p.cx,p.cy,
			//	SW_SCROLLCHILDREN|SW_INVALIDATE|SW_ERASE);
			
			//for(int i = 0;i < m_RolloutCtrls.GetSize();i++)
			//	m_RolloutCtrls[i]->Move(long((cur-newY)/k));

			_ptOffset.y = prev+newY;
			cur = newY;
			
			RecalcButtons();
			InvalidateRect(&r);
			InvalidateSlider();
		}
		else if (!in_b){
			const int d = rc.bottom-r.Height()+4;
			const int y = rc.top-d-4;
	
			yOffset = -long(y/k);
			_ptOffset.y = y;
			cur = y;
			
			RecalcButtons();
			InvalidateRect(&r);
			InvalidateSlider();

			in_b = true;
		}
	}
	else if (!in_t){
		yOffset = 0;

		_ptOffset.y = 0;
		cur = 0;
		
		RecalcButtons();
		InvalidateRect(&r);
		InvalidateSlider();
		in_t = true;
	}
	
	//if (b){
	//}
}

LRESULT AWTL::C3DStudioRolloutContainer::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
	const int y = HIWORD(lParam);
	
	CRect rc(GetSliderRect());	
	rc.DeflateRect(2,2);

	double k = double(y)/_height;
	if (_height > y){
		rc.bottom = long(y*k);
		rc.OffsetRect(0,_ptOffset.y);
	}

	if (rc.bottom > y){
		const int dy = rc.bottom-y;
		
		CRect r(0,0,GetWidth()+outer_margins.cx*2+
				inner_margins.cx*2+4,_height);
		
		//ScrollWindowEx(0, long(dy/k),
		//	SW_SCROLLCHILDREN|SW_INVALIDATE|SW_ERASE);
		
		_ptOffset.y -= dy;
		yOffset = -long(_ptOffset.y/k);
		
		RecalcButtons();
		InvalidateRect(&r);
		//Invalidate();
	}

	InvalidateSlider();
	return 1;
}
