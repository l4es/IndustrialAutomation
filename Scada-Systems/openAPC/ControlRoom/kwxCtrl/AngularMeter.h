#ifndef KWXANGULARMETER_H
#define KWXANGULARMETER_H

#define MAX_NUM_ANGULARMETER_SECTORS 20

class kwxAngularMeter : public wxWindow
{
public:
    kwxAngularMeter(wxWindow *parent,
				const wxWindowID id         = -1,
				const wxString&  label      = wxEmptyString,
				const wxPoint&   pos        = wxDefaultPosition,
				const wxSize&    size       = wxDefaultSize,
				const long int   style      = 0);

	virtual ~kwxAngularMeter();
	void SetSectorColor(int nSector, wxColour colour) ;
	void SetNumSectors(int nSector) { m_nSec = nSector ; };
	void SetNumTick(int nTick) { m_nTick = nTick ; };
	void SetRange(int min, int max) { m_nRangeStart = min ; m_nRangeEnd = max ; } ;
	void SetAngle(wxFloat32 min, wxFloat32 max) { m_nAngleStart = min ; m_nAngleEnd = max ; } ;
	void SetValue(wxFloat32 val);
	virtual bool SetForegroundColour(const wxColour &colour) { m_cNeedleColour = colour ; return true;} ;
	virtual bool SetBackgroundColour(const wxColour &colour) { m_cBackColour = colour ; return true;} ;
    virtual bool SetFont(const wxFont& font) { m_Font = font ; return true;} ;
	void DrawCurrent(bool state) { m_bDrawCurrent = state ; } ;


private:
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()

	void    OnPaint(wxPaintEvent& event);
	void	OnEraseBackGround(wxEraseEvent& WXUNUSED(event)) {};
	void	DrawTicks(wxAutoBufferedPaintDC &dc,int w,int h) ;
	void	DrawNeedle(wxAutoBufferedPaintDC &dc,int w,int h) ;
	void	DrawSectors(wxAutoBufferedPaintDC &dc,int w,int h) ;

private:
	int		m_nRangeStart ;
	int		m_nRangeEnd ;
	wxFloat32		m_nAngleStart ;
	wxFloat32		m_nAngleEnd ;
	double	m_nScaledVal ;
	int		m_nTick ;
	int		m_nSec ;
	double	m_dPI ;
	wxFloat32 m_nRealVal ;
	bool	m_bDrawCurrent ;
	wxColour m_aSectorColor[MAX_NUM_ANGULARMETER_SECTORS] ;
//	wxBitmap *membitmap ;
	wxFont m_Font ;
	wxColour m_cNeedleColour ;
	wxColour m_cBackColour ;

    wxBitmap  *m_pPreviewBmp ;


};


#endif
