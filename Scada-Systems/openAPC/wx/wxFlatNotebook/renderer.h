#ifndef RENDERE_H
#define RENDERE_H

#include <wx/dc.h>
#include <wx/string.h>
#include "../../wx/wxFlatNotebook/fnb_singleton.h"
#include "../../wx/wxFlatNotebook/fnb_smart_ptr.h"

#ifdef ENV_WINDOWS
#pragma warning (disable: 4018)
#pragma warning (disable: 4663)
#endif

#include <map>
#include <vector>
#include <wx/event.h>

class wxFNBRenderer
{
protected:
	// A bitmap that holds the background of the
	// x button which is drawn on a tab
	wxBitmap m_tabXBgBmp, m_xBgBmp, m_leftBgBmp, m_rightBgBmp;
	wxBitmap m_arrowDown, m_arrowUp;

public:
	wxFNBRenderer();
	virtual ~wxFNBRenderer();

	/**
	 * Generic function that draws the tabs and updates values in the page container
	 * MAC requires that the event will be skipped, so we must pass it
	 * \param pageContainer window that contains the tabs drawing
	 * \param dc device context
	 */
	virtual void DrawTabs(wxWindow* pageContainer, wxDC &dc, wxEvent &event);


	/**
	 * Draw tab 
	 * \param pageContainer parent window on which to draw
	 * \param dc device context to use
	 * \param posx tab x coordinate
	 * \param tabIdx tab index
	 * \param tabWidth tab width 
	 * \param tabHeight tab height 
	 * \param btnStatus btnStatus the little 'x' button (on top of the active tab) status, can be one of
	 *   - wxFNB_BTN_PRESSED
	 *   - wxFNB_BTN_HOVER
	 *   - wxFNB_BTN_NONE
	 */
	virtual void DrawTab(wxWindow* pageContainer, wxDC &dc, const int &posx, const int &tabIdx, const int &tabWidth, const int &tabHeight) = 0;

	/**
	 * Calculate tab width , based on its index (for image, x button on tab) 
	 * \param pageContainer pageContainer parent window on which to draw
	 * \param tabIdx tab index
	 * \param tabHeight the tab height (used for tan() function calculations)
	 * \return tab bouding rectangle size
	 */
	virtual int CalcTabWidth (wxWindow* pageContainer, int tabIdx, int tabHeight);

	/**
	 * Calculate tab height
	 * \param pageContainer pageContainer parent window on which to draw
	 * \return tab bouding rectangle size
	 */
	virtual int CalcTabHeight(wxWindow* pageContainer);

	/**
	 * Get a bitmap from device context, with rect size
	 * \param dc device context
	 * \param rect bitmap rectangle
	 * \param bmp [output] bitmap
	 */
	virtual void GetBitmap(wxDC& dc, const wxRect &rect, wxBitmap &bmp);

	/**
	 * Draw a bottom line for the tabs area
	 * \param pageContainer the owner of this tabs 
	 * \param dc device context to use
	 * \param selTabX1 the selection tab X1 coord
	 * \param selTabX2 the selection tab X2 coord
	 */
	void DrawTabsLine(wxWindow *pageContainer, wxDC& dc, wxCoord selTabX1 = -1, wxCoord selTabX2 = -1);

	/**
	 * Brighten a given colour with amount
	 * \param color starting colour
	 * \param percent percent, 0 - no change, 100 - white
	 * \return brighten colour
	 */
	static wxColor LightColour(const wxColour& color, int percent);

	int GetRightButtonPos(wxWindow *pageContainer);
	int GetButtonsAreaLength(wxWindow *pageContainer);
	int GetDropArrowButtonPos(wxWindow *pageContainer);

	/**
	 * Paint rectangle with gradient colouring
	 * \param dc device context
	 * \param rect rectangle 
	 * \param startColor gradient colour 1
	 * \param endColor gradient colour 2
	 * \param vertical use vertical gradient or horizontal
	 */
	static void PaintStraightGradientBox(wxDC& dc, const wxRect& rect, const wxColour& startColor, const wxColour& endColor, bool  vertical = true);

	/**
	 * Draw drop down arrow on the right corner
	 * \param pageContainer window tabs container
	 * \param dc device context
	 */
	void DrawDropDownArrow(wxWindow* pageContainer, wxDC& dc);

	/**
	 * Return an array of tabs info that can fit to screen starting from 'from'
	 * \param pageContainer 
	 * \param [output] vTabInfo 
	 * \param from 
	 */
	virtual void NumberTabsCanFit(wxWindow *pageContainer, std::vector<wxRect> &vTabInfo, int from = -1);

protected:
	/**
	 * Generate random colour
	 * \return random colour
	 */
	wxColour RandomColor();


};

typedef wxFNBSmartPtr<wxFNBRenderer> wxFNBRendererPtr;

class wxFNBRendererFirefox2 : public wxFNBRenderer
{
public:
	wxFNBRendererFirefox2(){}
	virtual ~wxFNBRendererFirefox2(){}
	virtual void DrawTab(wxWindow* pageContainer, wxDC &dc, const int &posx, const int &tabIdx, const int &tabWidth, const int &tabHeight);
};



//-----------------------------------
// Renderer manager class
//-----------------------------------

class wxFNBRendererMgr
{
	friend class wxFNBSingleton<wxFNBRendererMgr>;
	wxFNBRendererPtr  m_renderers;
public:
	/**
	 * Return the renderer according to the style flag, the returned pointer should not be
	 * deleted by caller, it is owned by this class
	 * \param style window style flag
	 * \return wxFNBRenderer 
	 */
	wxFNBRendererPtr GetRenderer();

private:
	wxFNBRendererMgr();
	virtual ~wxFNBRendererMgr();
};
typedef wxFNBSingleton<wxFNBRendererMgr> wxFNBRendererMgrST;
#endif // RENDERE_H
