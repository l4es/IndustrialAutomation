/////////////////////////////////////////////////////////////////////////////
// Name:        AngularMeter.cpp
// Purpose:     wxIndustrialControls Library
// Author:      Marco Cavallini <m.cavallini AT koansoftware.com>
// Modified by: Oxy <virtual_worlds(at)gmx.de>
// Copyright:   (C)2004-2006 Copyright by Koan s.a.s. - www.koansoftware.com
// Licence:     KWIC License http://www.koansoftware.com/kwic/kwic-license.htm
/////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef ENV_WINDOWS
#pragma warning (disable: 4018)
#pragma warning (disable: 4663)
#pragma warning (disable: 4284)
#endif

#include <wx/wx.h>
#include <wx/image.h>
#include <math.h>
#include <wx/dcbuffer.h>

#include "AngularMeter.h"
#include "oapc_libio.h"
#include <wx/event.h>
#include "globals.h"


//IMPLEMENT_DYNAMIC_CLASS(kwxAngularMeter, wxWindow)

BEGIN_EVENT_TABLE(kwxAngularMeter,wxWindow)
	EVT_PAINT(kwxAngularMeter::OnPaint)
	EVT_ERASE_BACKGROUND(kwxAngularMeter::OnEraseBackGround)
END_EVENT_TABLE()

kwxAngularMeter::kwxAngularMeter(wxWindow* parent,
		                   const wxWindowID id,
		                   const wxString& WXUNUSED(label),
                           const wxPoint& pos,
                           const wxSize& size,
		                   const long int WXUNUSED(style))
	: wxWindow(parent, id, pos, size, 0)
{
   SDBG
   assert(parent);
   if (parent) SetBackgroundColour(parent->GetBackgroundColour());
   SetBackgroundStyle(wxBG_STYLE_CUSTOM);

    //SetSize(size);
    SetAutoLayout(TRUE); // --> ???
	Refresh();

	//valori di default

	m_nScaledVal = 0;		//gradi
	m_nRealVal = 0;
	m_nTick = 0;	//numero tacche
	m_nSec = 1;		//default numero settori
	m_nRangeStart = 0;
	m_nRangeEnd = 220;
	m_nAngleStart = -20;
	m_nAngleEnd = 200;
	m_aSectorColor[0] = *wxWHITE;
//	m_cBackColour = *wxLIGHT_GREY;
	m_cBackColour = GetBackgroundColour() ;		//default sfondo applicazione
	m_cNeedleColour = *wxBLACK;	//indicatore
	m_dPI = 4.0 * atan(1.0);
	m_Font = *wxSWISS_FONT;	//font
	m_bDrawCurrent = true ;
}

kwxAngularMeter::~kwxAngularMeter()
{
    SDBG
}

void kwxAngularMeter::SetValue(wxFloat32 val) 
{ 
    SDBG
	wxFloat32 deltarange = m_nRangeEnd - m_nRangeStart;
//	int rangezero = deltarange - m_nRangeStart;
	wxFloat32 deltaangle = m_nAngleEnd - m_nAngleStart;
	double coeff = deltaangle / deltarange;

	m_nScaledVal = (val - m_nRangeStart) * coeff; 
	m_nRealVal = val;
	Refresh();
}


void kwxAngularMeter::OnPaint(wxPaintEvent &WXUNUSED(event))
{
	int w,h ;
	
	GetClientSize(&w,&h);
    w--;
    h--;

	/////////////////


	// Create a memory DC
    wxAutoBufferedPaintDC dc( this );

#if wxCHECK_VERSION(2,9,0)
	dc.SetBackground(*wxTheBrushList->FindOrCreateBrush(m_cBackColour,wxBRUSHSTYLE_SOLID));
	dc.SetBrush(*wxTheBrushList->FindOrCreateBrush(m_cBackColour,wxBRUSHSTYLE_SOLID));
#else
	dc.SetBackground(*wxTheBrushList->FindOrCreateBrush(m_cBackColour,wxSOLID));
	dc.SetBrush(*wxTheBrushList->FindOrCreateBrush(m_cBackColour,wxSOLID));
#endif
	dc.Clear();


/*
    if (m_pPreviewBmp && m_pPreviewBmp->Ok())
		dc.DrawBitmap( *m_pPreviewBmp, 1, 1 );
*/


	///////////////////

	//settori
	DrawSectors(dc,w,h) ;

	//tacche
	if (m_nTick > 0)
		DrawTicks(dc,w,h);

	//indicatore lancetta

	DrawNeedle(dc,w,h);

	
	//testo valore
	if (m_bDrawCurrent) 
	{
		wxString valuetext;
        valuetext.Printf(_T("%d"),(wxInt32)OAPC_ROUND(m_nRealVal,0));
		dc.SetFont(m_Font);
		dc.DrawText(valuetext, (w / 2) - 10, (h / 2) + 10);
	}

}


void kwxAngularMeter::DrawNeedle(wxAutoBufferedPaintDC &dc,int w, int h) 
{
	//indicatore triangolare
	double dxi,dyi, val;
	wxPoint ppoint[6];

#if wxCHECK_VERSION(2,9,0)
   dc.SetPen(*wxThePenList->FindOrCreatePen(m_cNeedleColour, 1,wxPENSTYLE_SOLID));
#else
   dc.SetPen(*wxThePenList->FindOrCreatePen(m_cNeedleColour, 1,wxSOLID));
#endif

	val = (m_nScaledVal + m_nAngleStart) * m_dPI / 180; //radianti parametro angolo

	dyi = sin(val - 90) * 2; //coordinate base sinistra
	dxi = cos(val - 90) * 2;

	ppoint[0].x = (w / 2) - dxi;	//base sinistra
	ppoint[0].y = (h / 2) - dyi;

	dxi = cos(val) * ((h / 2) - 4); //coordinate punta indicatore
	dyi = sin(val) * ((h / 2) - 4);

	ppoint[2].x = (w / 2) - dxi;	//punta
	ppoint[2].y = (h / 2) - dyi;

	dxi = cos(val + 90) * 2; //coordinate base destra
	dyi = sin(val + 90) * 2;

	ppoint[4].x = (w / 2) - dxi;	//base destra
	ppoint[4].y = (h / 2) - dyi;

	ppoint[5].x = ppoint[0].x;	//ritorno base sinistra
	ppoint[5].y = ppoint[0].y;

//////////////////////////
	val = (m_nScaledVal + m_nAngleStart + 1) * m_dPI / 180;

	dxi = cos(val) * ((h / 2) - 10); //coordinate medio destra
	dyi = sin(val) * ((h / 2) - 10);

	ppoint[3].x = (w / 2) - dxi;	//base media destra
	ppoint[3].y = (h / 2) - dyi;

	val = (m_nScaledVal + m_nAngleStart - 1) * m_dPI / 180;

	dxi = cos(val) * ((h / 2) - 10); //coordinate medio sinistra
	dyi = sin(val) * ((h / 2) - 10);

	ppoint[1].x = (w / 2) - dxi;	//base media sinistra
	ppoint[1].y = (h / 2) - dyi;

/////////////////////////


#if wxCHECK_VERSION(2,9,0)
   dc.SetBrush(*wxTheBrushList->FindOrCreateBrush(m_cNeedleColour,wxBRUSHSTYLE_SOLID));
#else
   dc.SetBrush(*wxTheBrushList->FindOrCreateBrush(m_cNeedleColour,wxSOLID));
#endif

	dc.DrawPolygon(6, ppoint, 0, 0, wxODDEVEN_RULE);

	//cerchio indicatore
#if wxCHECK_VERSION(2,9,0)
   dc.SetBrush(*wxTheBrushList->FindOrCreateBrush(*wxWHITE,wxBRUSHSTYLE_SOLID));
#else
	dc.SetBrush(*wxTheBrushList->FindOrCreateBrush(*wxWHITE,wxSOLID));
#endif
	dc.DrawCircle(w / 2, h / 2, 4);
}



void kwxAngularMeter::DrawSectors(wxAutoBufferedPaintDC &dc, int w, int h)
{
	double starc,endarc;
	int secount,dx,dy;
	double val;
	
	//arco -> settori
#if wxCHECK_VERSION(2,9,0)
   dc.SetPen(*wxThePenList->FindOrCreatePen(*wxBLACK, 1, wxPENSTYLE_SOLID));
#else
	dc.SetPen(*wxThePenList->FindOrCreatePen(*wxBLACK, 1, wxSOLID));
#endif
	starc = m_nAngleStart;
	endarc = starc + ((m_nAngleEnd - m_nAngleStart) / (double)m_nSec);
	//dc.SetBrush(*wxTheBrushList->FindOrCreateBrush(*wxRED,wxSOLID));
	for(secount=0;secount<m_nSec;secount++)
	{
#if wxCHECK_VERSION(2,9,0)
      dc.SetBrush(*wxTheBrushList->FindOrCreateBrush(m_aSectorColor[secount],wxBRUSHSTYLE_SOLID));
#else
		dc.SetBrush(*wxTheBrushList->FindOrCreateBrush(m_aSectorColor[secount],wxSOLID));
#endif
		dc.DrawEllipticArc(0,0,w,h,180 - endarc,180 - starc);
		//dc.DrawEllipticArc(0,0,w,h,0,180);
		starc = endarc;
		endarc += ((m_nAngleEnd - m_nAngleStart) / (double)m_nSec);
	}

	val = (m_nAngleStart * m_dPI) / 180.0;
	dx = cos(val) * h / 2.0;
	dy = sin(val) * h / 2.0;

	dc.DrawLine(w / 2, h / 2, (w / 2) - dx, (h / 2) - dy);	//linea sinistra

	val = (m_nAngleEnd * m_dPI) / 180.0;
	dx = cos(val) * h / 2.0;
	dy = sin(val) * h / 2.0;
		
	dc.DrawLine(w / 2, h / 2, (w / 2) - dx, (h / 2) - dy);	//linea destra

}

void kwxAngularMeter::DrawTicks(wxAutoBufferedPaintDC &dc,int w, int h)
{
	double intervallo = (m_nAngleEnd - m_nAngleStart) / (m_nTick + 1.0);
	double valint = intervallo + m_nAngleStart;
	double tx, ty;
	double val;
	double dx, dy;
	int n;
	int tw, th;
	wxString s;
	
	for(n = 0;n < m_nTick;n++)
	{
		val=(valint * m_dPI) / 180;
		//wxLogTrace("v: %f",valint);
		dx = cos(val) * (h/2);	//punto sul cerchio
		dy = sin(val) * (h/2);

		tx = cos(val) * ((h / 2) - 10);	//punto nel cerchio
		ty = sin(val) * ((h / 2) - 10);

		dc.DrawLine((w / 2) - tx, (h / 2) - ty, (w / 2) - dx, (h / 2) - dy);
		
		wxFloat32 deltarange = m_nRangeEnd - m_nRangeStart;
   		wxFloat32 rightval=(deltarange/(m_nTick+1.0))*(n+1.0);
		
		s.Printf(_T("%d"), (wxInt32)rightval);

		dc.GetTextExtent(s, &tw, &th);

		val = ((valint - 4) * m_dPI) / 180;	//angolo spostato
		tx = cos(val) * ((h / 2) - 12);	//punto testo
		ty = sin(val) * ((h / 2) - 12);

		dc.SetFont(m_Font);

        wxInt32 angle=90-valint;
        if (angle <0) angle+=360;
        else if (angle>=360) angle-=360;

		dc.DrawRotatedText(s,(w / 2) - tx, (h / 2) - ty, angle);

		valint = valint + intervallo;
	}	

	
}

void kwxAngularMeter::SetSectorColor(int nSector, wxColour colour) 
{ 
    SDBG
	m_aSectorColor[nSector] = colour; 
}
