/******************************************************************************

This file is part of ControlRoom process control/HMI software.

ControlRoom is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation, either version 3 of the License, or (at your option) any
later version.

ControlRoom is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
details.

You should have received a copy of the GNU General Public License along with
ControlRoom. If not, see http://www.gnu.org/licenses/

*******************************************************************************

For different licensing and/or usage of the sources apart from GPL or any other
open source license, please contact us at https://openapc.com/contact.php

*******************************************************************************/

#include <wx/wx.h>
#include "../wx/wxFlatNotebook/renderer.h"
#include "../wx/wxFlatNotebook/wxFlatNotebook.h" // for the styles
#include <wx/image.h>


const char *down_arrow_xpm[] = {
/* width height num_colors chars_per_pixel */
"    16    16        8            1",
/* colors */
"` c #008080",
". c #000000",
"# c #000000",
"a c #000000",
"b c #000000",
"c c #000000",
"d c #000000",
"e c #000000",
/* pixels */
"````````````````",
"````````````````",
"````````````````",
"````````````````",
"````````````````",
"````````````````",
"````.........```",
"`````.......````",
"``````.....`````",
"```````...``````",
"````````.```````",
"````````````````",
"````````````````",
"````````````````",
"````````````````",
"````````````````"
};	



//-----------------------------------------------------------------------------
// Util functions
//-----------------------------------------------------------------------------

static void DrawButton(wxDC& dc, 
			const wxRect& rect, 
			const bool &focus, 
			const bool &upperTabs)
{
	// Define the middle points
	wxPoint leftPt, rightPt;
	if( focus ){
		if( upperTabs ){
			leftPt = wxPoint(rect.x, rect.y + (rect.height / 10)*8 );
			rightPt = wxPoint(rect.x + rect.width - 2, rect.y + (rect.height / 10)*8);
		} else {
			leftPt = wxPoint(rect.x, rect.y + (rect.height / 10)*5 );
			rightPt = wxPoint(rect.x + rect.width - 2, rect.y + (rect.height / 10)*5);
		}
	} else {
		leftPt = wxPoint(rect.x, rect.y + (rect.height / 2) );
		rightPt = wxPoint(rect.x + rect.width - 2, rect.y + (rect.height / 2));
	}

	// Define the top region
	wxRect top(rect.GetLeftTop(), rightPt);
	wxRect bottom(leftPt, rect.GetBottomRight());

	wxColour topStartColor(wxT("WHITE"));

	if( !focus ){
		topStartColor = wxFNBRenderer::LightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE), 50);
	}

	wxColour topEndColor( wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
	wxColour bottomStartColor(topEndColor);
	wxColour bottomEndColor(topEndColor);

	// Incase we use bottom tabs, switch the colors
	if( upperTabs ){
		if( focus ){
			wxFNBRenderer::PaintStraightGradientBox(dc, top, topStartColor, topEndColor);
			wxFNBRenderer::PaintStraightGradientBox(dc, bottom, bottomStartColor, bottomEndColor);
		} else {
			wxFNBRenderer::PaintStraightGradientBox(dc, top, topEndColor , topStartColor);
			wxFNBRenderer::PaintStraightGradientBox(dc, bottom, bottomStartColor, bottomEndColor);
		}
	} else {
		if( focus ){
			wxFNBRenderer::PaintStraightGradientBox(dc, bottom, topEndColor, bottomEndColor);
			wxFNBRenderer::PaintStraightGradientBox(dc, top,topStartColor,  topStartColor);
		} else {
			wxFNBRenderer::PaintStraightGradientBox(dc, bottom, bottomStartColor, bottomEndColor);
			wxFNBRenderer::PaintStraightGradientBox(dc, top, topEndColor, topStartColor);
		}
	}
	
	dc.SetBrush( *wxTRANSPARENT_BRUSH );
}

wxFNBRenderer::wxFNBRenderer()
: m_tabXBgBmp(16, 16)
, m_xBgBmp(16, 14)
, m_leftBgBmp(16, 14)
, m_rightBgBmp(16, 14)
{
}

wxFNBRenderer::~wxFNBRenderer()
{
}

int wxFNBRenderer::GetRightButtonPos(wxWindow* pageContainer)
{
	wxPageContainer *pc = static_cast<wxPageContainer*>( pageContainer );
	wxRect rect = pc->GetClientRect();
	int clientWidth = rect.width;
	return clientWidth - 22;
}

int wxFNBRenderer::GetDropArrowButtonPos(wxWindow *pageContainer)
{
	return GetRightButtonPos(pageContainer);
}

int wxFNBRenderer::GetButtonsAreaLength(wxWindow* pageContainer)
{
	wxPageContainer *pc = static_cast<wxPageContainer*>( pageContainer );
	long style = pc->GetParent()->GetWindowStyleFlag();

   if (style & wxFNB_TABS_DISABLED) return 0;
	// 'v'
	if (style & wxFNB_DROPDOWN_TABS_LIST) return 22;

	// '<>x'
	return 53;
}

void wxFNBRenderer::DrawDropDownArrow(wxWindow* pageContainer, wxDC& dc)
{
	wxPageContainer *pc = static_cast<wxPageContainer*>( pageContainer );
	// Check if this style is enabled
	long style = pc->GetParent()->GetWindowStyleFlag();
	if(! (style & wxFNB_DROPDOWN_TABS_LIST) )
		return;

	// Make sure that there are pages in the container
	if(pc->GetPageInfoVector().empty())
		return;

	// Set the bitmap according to the button status
	wxBitmap xbmp;
	
	xbmp = wxBitmap(down_arrow_xpm);

	xbmp.SetMask(new wxMask(xbmp, MASK_COLOR));
	// erase old bitmap
	int posx = GetDropArrowButtonPos( pc );
	dc.DrawBitmap(m_rightBgBmp, posx, 6);

	// Draw the new bitmap
	dc.DrawBitmap(xbmp, posx, 6, true);
}



void wxFNBRenderer::PaintStraightGradientBox(wxDC& dc, const wxRect& rect, const wxColour& startColor, const wxColour& endColor, bool  vertical)
{
	int rd, gd, bd, high = 0;
	rd = endColor.Red() - startColor.Red();
	gd = endColor.Green() - startColor.Green();
	bd = endColor.Blue() - startColor.Blue();

	/// Save the current pen and brush
	wxPen savedPen = dc.GetPen();
	wxBrush savedBrush = dc.GetBrush();

	if ( vertical )
		high = rect.GetHeight()-1;
	else
		high = rect.GetWidth()-1;

	if( high < 1 )
		return;

	for (int i = 0; i <= high; ++i)
	{
		int r = startColor.Red() +  ((i*rd*100)/high)/100;
		int g = startColor.Green() + ((i*gd*100)/high)/100;
		int b = startColor.Blue() + ((i*bd*100)/high)/100;

		wxPen p(wxColor(r, g, b));
		dc.SetPen(p);

		if ( vertical )
			dc.DrawLine(rect.x, rect.y+i, rect.x+rect.width, rect.y+i);
		else
			dc.DrawLine(rect.x+i, rect.y, rect.x+i, rect.y+rect.height);
	}

	/// Restore the pen and brush
	dc.SetPen( savedPen );
	dc.SetBrush( savedBrush );
}


void wxFNBRenderer::GetBitmap(wxDC &dc, const wxRect &rect, wxBitmap &bmp)
{
	wxMemoryDC mem_dc;
	mem_dc.SelectObject(bmp);
	mem_dc.Blit(0, 0, rect.width, rect.height, &dc, rect.x, rect.y);
	mem_dc.SelectObject(wxNullBitmap);
}



wxColor wxFNBRenderer::LightColour(const wxColour& color, int percent)
{
	int rd, gd, bd, high = 0;
	wxColor end_color = wxT("WHITE");
	rd = end_color.Red() - color.Red();
	gd = end_color.Green() - color.Green();
	bd = end_color.Blue() - color.Blue();
	high = 100;

	// We take the percent way of the color from color --> white
	int i = percent;
	int r = color.Red() +  ((i*rd*100)/high)/100;
	int g = color.Green() + ((i*gd*100)/high)/100;
	int b = color.Blue() + ((i*bd*100)/high)/100;
	return wxColor(r, g, b);
}

void wxFNBRenderer::DrawTabsLine(wxWindow* pageContainer, wxDC& dc, wxCoord selTabX1, wxCoord selTabX2)
{
	wxPageContainer *pc = static_cast<wxPageContainer*>( pageContainer );
	wxRect clntRect = pc->GetClientRect();
	wxRect clientRect, clientRect2, clientRect3;
	clientRect3 = wxRect(0, 0, clntRect.width, clntRect.height);

		wxColour fillColor;
		if( !pc->HasFlag(wxFNB_BOTTOM) ){
			fillColor = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
		} else {
			fillColor = wxColor(wxT("WHITE"));
		}

		dc.SetPen( wxPen(fillColor) );
		if(pc->HasFlag(wxFNB_BOTTOM)){

			dc.DrawLine(1, 0, clntRect.width-1, 0);
			dc.DrawLine(1, 1, clntRect.width-1, 1);

			dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW)));
			dc.DrawLine(1, 2, clntRect.width-1, 2);

			dc.SetPen(wxPen(fillColor));
			dc.DrawLine(selTabX1 + 2, 2, selTabX2 - 1, 2);
		} else {

			dc.DrawLine(1, clntRect.height, clntRect.width-1, clntRect.height);
			dc.DrawLine(1, clntRect.height-1, clntRect.width-1, clntRect.height-1);

			dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW)));
			dc.DrawLine(1, clntRect.height-2, clntRect.width-1, clntRect.height-2);

			dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE)));
			dc.DrawLine(selTabX1 + 2, clntRect.height-2, selTabX2-1, clntRect.height-2);
		}
}

int wxFNBRenderer::CalcTabWidth(wxWindow *pageContainer, int tabIdx, int tabHeight)
{
	wxPageContainer *pc = static_cast<wxPageContainer*>( pageContainer );
	int tabWidth, shapePoints(0), width, pom;
	wxMemoryDC dc;

	if (pc->HasFlag(wxFNB_TABS_DISABLED)) return 0;

	// bitmap must be set before it can be used for anything
	wxBitmap bmp(10, 10);
	dc.SelectObject(bmp);

	wxFont normalFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont boldFont(normalFont);
	boldFont.SetWeight(wxFONTWEIGHT_BOLD);

	shapePoints = (int)(tabHeight*tan((double)pc->GetPageInfoVector()[tabIdx].GetTabAngle()/180.0*M_PI));

	if(pc->GetSelection() == tabIdx)
		dc.SetFont(boldFont);
	else
		dc.SetFont(normalFont);
	dc.GetTextExtent(pc->GetPageText(tabIdx), &width, &pom);

	// Set a minimum size to a tab
	if(width < 20)
		width = 20;

	tabWidth = ((wxFlatNotebook *)pc->m_pParent)->GetPadding() * 2 + width;

	/// Style to add a small 'x' button on the top right
	/// of the tab
	tabWidth += 2 * shapePoints;

	bool hasImage = (pc->m_ImageList != NULL && pc->GetPageInfoVector()[tabIdx].GetImageIndex() != -1);

	// For VC71 style, we only add the icon size (16 pixels)
	if(hasImage)
	{
		tabWidth += (16 + ((wxFlatNotebook*)pc->m_pParent)->GetPadding()) + shapePoints / 2;
	}
	return tabWidth;
}

void wxFNBRenderer::NumberTabsCanFit(wxWindow *pageContainer, std::vector<wxRect> &vTabInfo, int from)
{
	wxPageContainer *pc = static_cast<wxPageContainer*>( pageContainer );
	int tabHeight, clientWidth;
	
	wxRect rect = pc->GetClientRect();
	clientWidth = rect.width;

	/// Empty results
	vTabInfo.clear();

	tabHeight = CalcTabHeight( pageContainer );

	// The drawing starts from posx
	int posx = ((wxFlatNotebook *)pc->m_pParent)->GetPadding();
	
	if( from < 0 )
		from = pc->m_nFrom;

	for(int i = from; i<(int)pc->GetPageInfoVector().GetCount(); i++)
	{
		int tabWidth = CalcTabWidth( pageContainer, i, tabHeight );
		if(posx + tabWidth + GetButtonsAreaLength( pc ) >= clientWidth)
			break;

		/// Add a result to the returned vector
		wxRect tabRect(posx, VERTICAL_BORDER_PADDING, tabWidth , tabHeight);
		vTabInfo.push_back(tabRect);

		/// Advance posx
		posx += tabWidth + wxFNB_HEIGHT_SPACER;
	}
}



int wxFNBRenderer::CalcTabHeight(wxWindow *pageContainer)
{
	int              tabHeight;
	wxMemoryDC       dc;
	wxPageContainer *pc = static_cast<wxPageContainer*>( pageContainer );

	if (pc->HasFlag(wxFNB_TABS_DISABLED)) return 0;

	wxBitmap bmp(10, 10);
	dc.SelectObject(bmp);

	// For GTK it seems that we must do this steps in order
	// for the tabs will get the proper height on initialization
	// on MSW, preforming these steps yields wierd results
	wxFont normalFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont boldFont = normalFont;
	boldFont.SetWeight(wxFONTWEIGHT_BOLD);
#ifdef __WXGTK__
	dc.SetFont( boldFont );
#endif

	static int height(-1);
	static int width(-1);

	if( height == -1 && width == -1 )
	{
		wxString stam = wxT("Tp");	// Temp data to get the text height;
		dc.GetTextExtent(stam, &width, &height);
	}

	tabHeight = height + wxFNB_HEIGHT_SPACER; // We use 8 pixels as padding
#ifdef __WXGTK__
	// On GTK the tabs are should be larger
	tabHeight += 6;
#endif
	return tabHeight;
}

void wxFNBRenderer::DrawTabs(wxWindow *pageContainer, wxDC &dc, wxEvent &event)
{
	wxPageContainer *pc = static_cast<wxPageContainer*>( pageContainer );
#ifndef __WXMAC__
	// Works well on MSW & GTK, however this lines should be skipped on MAC
	if(pc->GetPageInfoVector().empty() || pc->m_nFrom >= (int)pc->GetPageInfoVector().GetCount())
	{
		pc->Hide();
		event.Skip();
		return;
	}
#endif

	// Get the text hight
	int tabHeight = CalcTabHeight(pageContainer);

	// Calculate the number of rows required for drawing the tabs
	wxRect rect = pc->GetClientRect();
	int clientWidth = rect.width;

	// Set the maximum client size
#ifdef __WXMAC__
	pc->SetSizeHints(wxSize(GetButtonsAreaLength( pc ), tabHeight));
#endif
	wxPen borderPen = wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW));

	wxBrush backBrush;
    backBrush = wxBrush(pc->m_tabAreaColor);

	wxBrush noselBrush = wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
	wxBrush selBrush = wxBrush(pc->m_activeTabColor);

	wxSize size = pc->GetSize();

	// Background
    dc.SetTextBackground(pc->GetBackgroundColour());
	dc.SetTextForeground(pc->m_activeTextColor);
	dc.SetBrush(backBrush);

	// If border style is set, set the pen to be border pen
	if(pc->HasFlag(wxFNB_TABS_BORDER_SIMPLE))
		dc.SetPen(borderPen);
	else
	{
		wxColor colr = pc->GetBackgroundColour();
		dc.SetPen( wxPen(colr) );
	}

    int lightFactor = pc->HasFlag(wxFNB_BACKGROUND_GRADIENT) ? 70 : 0;
	PaintStraightGradientBox(dc, pc->GetClientRect(), pc->m_tabAreaColor, LightColour(pc->m_tabAreaColor, lightFactor));
	dc.SetBrush( *wxTRANSPARENT_BRUSH );
	dc.DrawRectangle(0, 0, size.x, size.y);

	// We always draw the bottom/upper line of the tabs
	// regradless the style
	dc.SetPen(borderPen);

	// Restore the pen
	dc.SetPen(borderPen);


#ifdef __WXMAC__
	// On MAC, Add these lines so the tab background gets painted
	if(pc->GetPageInfoVector().empty() || pc->m_nFrom >= (int)pc->GetPageInfoVector().GetCount())
	{
		pc->Hide();
		return;
	}
#endif

	// Draw labels
	wxFont normalFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont boldFont = normalFont;
	boldFont.SetWeight(wxFONTWEIGHT_BOLD);
	dc.SetFont(boldFont);

	int posx = ((wxFlatNotebook *)pc->m_pParent)->GetPadding();
	int i = 0;

	// Update all the tabs from 0 to 'pc->m_nFrom' to be non visible
	for(i=0; i<pc->m_nFrom; i++)
	{
		pc->GetPageInfoVector()[i].SetPosition(wxPoint(-1, -1));
		pc->GetPageInfoVector()[i].GetRegion().Clear();
	}

	//----------------------------------------------------------
	// Go over and draw the visible tabs
	//----------------------------------------------------------
	wxCoord x1(-1), x2(-1);
	for(i=pc->m_nFrom; i<(int)pc->GetPageInfoVector().GetCount(); i++)
	{
		dc.SetPen(borderPen);

		// Now set the font to the correct font
		dc.SetFont((i==pc->GetSelection()) ? boldFont : normalFont);

		// Add the padding to the tab width
		// Tab width:
		// +-----------------------------------------------------------+
		// | PADDING | IMG | IMG_PADDING | TEXT | PADDING | x |PADDING |
		// +-----------------------------------------------------------+
		int tabWidth = CalcTabWidth(pageContainer, i, tabHeight);

		// Check if we can draw more
		if(posx + tabWidth +0>= clientWidth)
			break;

		// By default we clean the tab region
		pc->GetPageInfoVector()[i].GetRegion().Clear();

		// Clean the 'x' buttn on the tab.
		// A 'Clean' rectanlge, is a rectangle with width or height
		// with values lower than or equal to 0
		pc->GetPageInfoVector()[i].GetXRect().SetSize(wxSize(-1, -1));

		// Draw the tab (border, text, image & 'x' on tab)
		DrawTab(pc, dc, posx, i, tabWidth, tabHeight);
		
		if(pc->GetSelection() == i){
			x1 = posx;
			x2 = posx + tabWidth + 2;
		}

		// Restore the text forground
		dc.SetTextForeground(pc->m_activeTextColor);

		// Update the tab position & size
		int posy = pc->HasFlag(wxFNB_BOTTOM) ? 0 : VERTICAL_BORDER_PADDING;

		pc->GetPageInfoVector()[i].SetPosition(wxPoint(posx, posy));
		pc->GetPageInfoVector()[i].SetSize(wxSize(tabWidth, tabHeight));
		posx += tabWidth;
	}

	// Update all tabs that can not fit into the screen as non-visible
	for(; i<(int)pc->GetPageInfoVector().GetCount(); i++)
	{
		pc->GetPageInfoVector()[i].SetPosition(wxPoint(-1, -1));
		pc->GetPageInfoVector()[i].GetRegion().Clear();
	}

	// Draw the left/right/close buttons
	// Left arrow
	DrawDropDownArrow(pc, dc);

	DrawTabsLine(pc, dc, x1, x2);
}

//------------------------------------------
// Renderer manager
//------------------------------------------
wxFNBRendererMgr::wxFNBRendererMgr()
{
	// register renderers
	m_renderers=wxFNBRendererPtr(new wxFNBRendererFirefox2());
}

wxFNBRendererMgr::~wxFNBRendererMgr()
{
}

wxFNBRendererPtr wxFNBRendererMgr::GetRenderer()
{
	return m_renderers;
}

//------------------------------------------
// Default renderer 
//------------------------------------------


//------------------------------------------
// Firefox2 renderer 
//------------------------------------------
void wxFNBRendererFirefox2::DrawTab(wxWindow* pageContainer, wxDC &dc, const int &posx, const int &tabIdx, const int &tabWidth, const int &tabHeight)
{
		// Default style
	wxPen borderPen = wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW));
	wxPageContainer *pc = static_cast<wxPageContainer*>( pageContainer );

	wxPoint tabPoints[7];
	tabPoints[0].x = posx + 2;
	tabPoints[0].y = pc->HasFlag(wxFNB_BOTTOM) ? 2 : tabHeight - 2;

	tabPoints[1].x = tabPoints[0].x;
	tabPoints[1].y = pc->HasFlag(wxFNB_BOTTOM) ? tabHeight - (VERTICAL_BORDER_PADDING+2) : (VERTICAL_BORDER_PADDING+2);

	tabPoints[2].x = tabPoints[1].x+2;
	tabPoints[2].y = pc->HasFlag(wxFNB_BOTTOM) ? tabHeight - VERTICAL_BORDER_PADDING : VERTICAL_BORDER_PADDING;

	tabPoints[3].x = posx +tabWidth - 2;
	tabPoints[3].y = pc->HasFlag(wxFNB_BOTTOM) ? tabHeight - VERTICAL_BORDER_PADDING : VERTICAL_BORDER_PADDING;

	tabPoints[4].x = tabPoints[3].x+2;
	tabPoints[4].y = pc->HasFlag(wxFNB_BOTTOM) ? tabHeight - (VERTICAL_BORDER_PADDING+2) : (VERTICAL_BORDER_PADDING+2);

	tabPoints[5].x = tabPoints[4].x;
	tabPoints[5].y = pc->HasFlag(wxFNB_BOTTOM) ? 2 : tabHeight - 2;

	tabPoints[6].x = tabPoints[0].x;
	tabPoints[6].y = tabPoints[0].y;

	//------------------------------------
	// Paint the tab with gradient
	//------------------------------------
	wxRect rr(tabPoints[2], tabPoints[5]);
	DrawButton( dc, 
				rr, 
				pc->GetSelection() == tabIdx , 
				!pc->HasFlag(wxFNB_BOTTOM));

	dc.SetBrush( *wxTRANSPARENT_BRUSH );
	dc.SetPen( borderPen );

	// Draw the tab as rounded rectangle
	dc.DrawPolygon(7, tabPoints);

	// -----------------------------------
	// Text and image drawing
	// -----------------------------------

	// Text drawing offset from the left border of the
	// rectangle
	int textOffset;

	// The width of the images are 16 pixels
	int padding = static_cast<wxFlatNotebook*>( pc->GetParent() )->GetPadding();
	int shapePoints = (int)(tabHeight * tan((double)pc->GetPageInfoVector()[tabIdx].GetTabAngle()/180.0*M_PI));
	bool hasImage = pc->GetPageInfoVector()[tabIdx].GetImageIndex() != -1;

#ifdef __WXMSW__
	int imageYCoord = pc->HasFlag(wxFNB_BOTTOM) ? 6 : 8;
#else 
	int imageYCoord = pc->HasFlag(wxFNB_BOTTOM) ? 6 : 10;
#endif

	hasImage ? textOffset = padding * 2 + 16 + shapePoints / 2 : textOffset = padding + shapePoints / 2 ;
	textOffset += 2;

	if(tabIdx != pc->GetSelection())
	{
		// Set the text background to be like the vertical lines
		dc.SetTextForeground( pc->GetNonoActiveTextColor() );
	}

	if(hasImage)
	{
		int imageXOffset = textOffset - 16 - padding;
		dc.DrawBitmap((*pc->GetImageList())[pc->GetPageInfoVector()[tabIdx].GetImageIndex()],
			posx + imageXOffset, imageYCoord, true);
	}

    if (pc->GetEnabled(tabIdx)) dc.SetTextForeground(*wxBLACK);
    else dc.SetTextForeground(*wxLIGHT_GREY);

	dc.DrawText(pc->GetPageText(tabIdx), posx + textOffset, imageYCoord);

	// draw 'x' on tab (if enabled)
}
