/////////////////////////////////////////////////////////////////////////////
// Name:        LCDWindow.cpp
// Purpose:     wxIndustrialControls Library
// Author:      Marco Cavallini <m.cavallini AT koansoftware.com>
// Modified by: Oxy <virtual_worlds(at)gmx.de>
// Copyright:   (C)2004-2006 Copyright by Koan s.a.s. - www.koansoftware.com
// Licence:     KWIC License http://www.koansoftware.com/kwic/kwic-license.htm
/////////////////////////////////////////////////////////////////////////////
//
// Portions are from Class to display numbers like a Liquid Crystal Display, 
// by Tsolakos Stavros
/////////////////////////////////////////////////////////////////////////////

#include <wx/wx.h>
#include <wx/image.h>
#include <wx/event.h>

#include "LCDWindow.h"
#include "globals.h"


#define LCD_NUMBER_SEGMENTS 8



BEGIN_EVENT_TABLE( kwxLCDDisplay, wxWindow )
	EVT_PAINT( kwxLCDDisplay::OnPaint )
	EVT_SIZE( kwxLCDDisplay::OnSize )
END_EVENT_TABLE()


kwxLCDDisplay::kwxLCDDisplay( wxWindow *parent, 
						const wxPoint& pos, 
						const wxSize& size ) 
: wxWindow( parent, -1, pos, size, wxSUNKEN_BORDER )
{
    SDBG
	mSegmentLen = 40;
	mSegmentWidth = 10;
	mSpace = 5;

	mNumberDigits = 6;

    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
	mLightColour = wxColour( 0, 255, 0 );
	mGrayColour = wxColour( 0, 64, 0 );

	SetBackgroundColour( wxColour( 0, 0, 0 ) );
}


kwxLCDDisplay::~kwxLCDDisplay()
{
    SDBG
}


void kwxLCDDisplay::OnPaint( wxPaintEvent &WXUNUSED(event))
{
	wxAutoBufferedPaintDC dc( this );

#if wxCHECK_VERSION(2,9,0)
	dc.SetBackground(*wxTheBrushList->FindOrCreateBrush(GetBackgroundColour(),wxBRUSHSTYLE_SOLID));
#else
   dc.SetBackground(*wxTheBrushList->FindOrCreateBrush(GetBackgroundColour(), wxSOLID));
#endif
	dc.Clear();
    
    int dw = GetClientSize().GetWidth();
	int dh = GetClientSize().GetHeight();

	int bw = GetBitmapWidth();
	int bh = GetBitmapHeight();

	double xs = ( double ) dw / bw;
	double ys = ( double ) dh / bh;

	m_scaleFactor= xs > ys? ys : xs;

/*	dc.SetUserScale( as, as );
	dc.SetDeviceOrigin( ( ( dw - bw * as ) / 2 ), ( ( dh - bh * as ) / 2 ) );*/
	DoDrawing( &dc );
}


void kwxLCDDisplay::OnSize( wxSizeEvent &event )
{
    SDBG
	event.Skip();
	return ;
}


void kwxLCDDisplay::DoDrawing( wxAutoBufferedPaintDC *dc )
{
	wxString buf;

#if wxCHECK_VERSION(2,9,0)
    buf.Printf(_T("%s"),mValue);
#else
    buf.Printf(_T("%s"), mValue.c_str() );
#endif
	while( buf.Replace(_T(".."),_T(". .")) );

	char current;
	char next;
	int buflen = buf.Len();
	int ac = buflen - 1;
	int c = 0 ;

	while (c < mNumberDigits)	//numero cifre display
	{
		ac >= 0? current = buf.GetChar( ac ): current = ' ';
		ac >= 0 && ac < buflen - 1? next = buf.GetChar( ac + 1 ): next = ' ';

		if ((current == '.') || (current == ','))
			ac-- ;
		else
		{

			wxDigitData *data = new wxDigitData;

			data->value = current;
			data->comma = false;

			if (( next == '.' ) || ( next == ',' ))
			{
				data->comma = true;
			}

			DrawDigit( dc, c, data  );
			ac--;
			c++ ;

			delete data;
		}
	}
}


void kwxLCDDisplay::DrawDigit( wxAutoBufferedPaintDC *dc, int digit, wxDigitData *data )
{
	unsigned char dec = Decode( data->value );

	if( data->value == ':' )	//scrive :
		DrawTwoDots( dc, digit );
	else
	{
		for( int c = 0; c < LCD_NUMBER_SEGMENTS - 1; c++ )
		{
			DrawSegment( dc, digit, c, ( dec >> c ) & 1 );
		}

		DrawSegment( dc, digit, 7, data->comma );	//scrive comma
	}
}


void kwxLCDDisplay::DrawTwoDots( wxAutoBufferedPaintDC *dc, int digit )
{
	wxFloat32 sl = mSegmentLen;
	wxFloat32 sw = mSegmentWidth;
//	int sp = mSpace;

	wxFloat32 x = DigitX( digit );
	wxFloat32 y = DigitY( digit );

#if wxCHECK_VERSION(2,9,0)
   wxBrush brushOn(mLightColour, wxBRUSHSTYLE_SOLID);
#else
	wxBrush brushOn( mLightColour, wxSOLID );
#endif

	x += (( sl / 2 ) - sw)*m_scaleFactor;
	y += (( sl / 2 ) - sw)*m_scaleFactor;

	dc->SetBrush( brushOn );
#if wxCHECK_VERSION(2,9,0)
   dc->SetPen(wxPen(GetBackgroundColour(), 1, wxPENSTYLE_SOLID));
#else
   dc->SetPen( wxPen( GetBackgroundColour(), 1, wxSOLID ) );
#endif
	dc->DrawEllipse( x, y, (2 * sw)*m_scaleFactor, (2 * sw)*m_scaleFactor );
	y += sl;

	dc->DrawEllipse( x, y, (2 * sw)*m_scaleFactor, (2 * sw)*m_scaleFactor );
}



void kwxLCDDisplay::SetSize(int width, int height)
{
   SDBG
   wxWindow::SetSize(width,height);
}



void kwxLCDDisplay::DrawSegment( wxAutoBufferedPaintDC *dc, int digit, int segment, bool state )
{
	wxFloat32 sl = mSegmentLen;
	wxFloat32 sw = mSegmentWidth;
//	int sp = mSpace;

	wxFloat32 x = DigitX( digit );
	wxFloat32 y = DigitY( digit );

#if wxCHECK_VERSION(2,9,0)
   wxBrush brushOn(mLightColour, wxBRUSHSTYLE_SOLID);
   wxBrush brushOff(mGrayColour, wxBRUSHSTYLE_SOLID);
#else
	wxBrush brushOn( mLightColour, wxSOLID );
	wxBrush brushOff( mGrayColour, wxSOLID );
#endif

	if( state )
		dc->SetBrush( brushOn );
	else
		dc->SetBrush( brushOff );

#if wxCHECK_VERSION(2,9,0)
   dc->SetPen( wxPen( GetBackgroundColour(), 1, wxPENSTYLE_SOLID ) );
#else
   dc->SetPen( wxPen( GetBackgroundColour(), 1, wxSOLID ) );
#endif
	wxPoint points[4];
	wxPoint p6[6];

	switch( segment )
	{
		case 0:
			{
				points[0].x = x*m_scaleFactor;
				points[0].y = y*m_scaleFactor;
				points[1].x = (x + sl)*m_scaleFactor;
				points[1].y = y*m_scaleFactor;
				points[2].x = (x + sl - sw)*m_scaleFactor;
				points[2].y = (y + sw)*m_scaleFactor;
				points[3].x = (x + sw)*m_scaleFactor;
				points[3].y = (y + sw)*m_scaleFactor;
				break;
			}
		case 1:
			{
				points[0].x = x*m_scaleFactor;
				points[0].y = y*m_scaleFactor;
				points[1].x = x*m_scaleFactor;
				points[1].y = (y + sl)*m_scaleFactor;
				points[2].x = (x + sw)*m_scaleFactor;
				points[2].y = (y + sl - sw / 2)*m_scaleFactor;
				points[3].x = (x + sw)*m_scaleFactor;
				points[3].y = (y + sw)*m_scaleFactor;
				break;
			}
		case 2:
			{
				x += sl - sw;
				points[0].x = x*m_scaleFactor;
				points[0].y = (y + sw)*m_scaleFactor;
				points[1].x = (x + sw)*m_scaleFactor;
				points[1].y = y*m_scaleFactor;
				points[2].x = (x + sw)*m_scaleFactor;
				points[2].y = (y + sl)*m_scaleFactor;
				points[3].x = x*m_scaleFactor;
				points[3].y = (y + sl - sw / 2.0)*m_scaleFactor;
				break;
			}
		case 3:
			{
				y += sl;
				points[0].x = x*m_scaleFactor;
				points[0].y = y*m_scaleFactor;
				points[1].x = x*m_scaleFactor;
				points[1].y = (y + sl)*m_scaleFactor;
				points[2].x = (x + sw)*m_scaleFactor;
				points[2].y = (y + sl - sw)*m_scaleFactor;
				points[3].x = (x + sw)*m_scaleFactor;
				points[3].y = (y + sw - sw / 2.0)*m_scaleFactor;
				break;
			}
		case 4:
			{
				y += sl;
				x += sl - sw;
				points[0].x = x*m_scaleFactor;
				points[0].y = (y + sw / 2.0)*m_scaleFactor;
				points[1].x = (x + sw)*m_scaleFactor;
				points[1].y = y*m_scaleFactor;
				points[2].x = (x + sw)*m_scaleFactor;
				points[2].y = (y + sl)*m_scaleFactor;
				points[3].x = x*m_scaleFactor;
				points[3].y = (y + sl - sw)*m_scaleFactor;
				break;
			}
		case 5:
			{
				y += 2 * sl - sw;
				points[0].x = (x + sw)*m_scaleFactor;
				points[0].y = y*m_scaleFactor;
				points[1].x = (x + sl - sw)*m_scaleFactor;
				points[1].y = y*m_scaleFactor;
				points[2].x = (x + sl)*m_scaleFactor;
				points[2].y = (y + sw)*m_scaleFactor;
				points[3].x = x*m_scaleFactor;
				points[3].y = (y + sw)*m_scaleFactor;
				break;
			}
		case 6:
			{
				y += sl - sw / 2;

				p6[0].x = x*m_scaleFactor;
				p6[0].y = (y + sw / 2)*m_scaleFactor;
				p6[1].x = (x + sw)*m_scaleFactor;
				p6[1].y = y*m_scaleFactor;
				p6[2].x = (x + sl - sw)*m_scaleFactor;
				p6[2].y = y*m_scaleFactor;
				p6[3].x = (x + sl)*m_scaleFactor;
				p6[3].y = (y + sw / 2)*m_scaleFactor;
				p6[4].x = (x + sl - sw)*m_scaleFactor;
				p6[4].y = (y + sw)*m_scaleFactor;
				p6[5].x = (x + sw)*m_scaleFactor;
				p6[5].y = (y + sw)*m_scaleFactor;

				break ;
			}
		default:
			break;
	}

	if( segment < 6 )
    {
       //segmento normale
	   dc->DrawPolygon( 4, points );
    }
	else if ( segment == 6 )		//segmento centrale a 6 lati
    {
	   dc->DrawPolygon( 6, p6 );
    }
	else							//comma
	{
		y += 2 * sl;
		x += sl;

        sw*=m_scaleFactor;
        if (sw<3) sw=3;
		dc->DrawEllipse( (x*m_scaleFactor + 1), (y*m_scaleFactor - sw),sw,sw);
	}

}


// Protected functions that calculate sizes.
// Needed by OnPaint

int kwxLCDDisplay::GetDigitWidth( void )
{
   SDBG
   return mSegmentLen + mSegmentWidth + mSpace;
}


int kwxLCDDisplay::GetDigitHeight( void )
{
   SDBG
   return ( 2 * mSegmentLen ) + ( 2 * mSpace );
}


int kwxLCDDisplay::GetBitmapWidth( void )
{
    SDBG
	return ( mNumberDigits * GetDigitWidth() ) + mSpace;
}


int kwxLCDDisplay::GetBitmapHeight( void )
{
    SDBG
	return GetDigitHeight();
}


int kwxLCDDisplay::DigitX( int digit )
{
    SDBG
	return GetBitmapWidth() - ( ( digit + 1 ) * GetDigitWidth() );
}


int kwxLCDDisplay::DigitY( int WXUNUSED(digit))
{
    SDBG
	return mSpace;
}


// Public functions accessible by the user.

void kwxLCDDisplay::SetNumberDigits( int ndigits )	//numero cifre
{
   SDBG
   if (ndigits!=mNumberDigits)
   {
	  mNumberDigits = ndigits;
	  Refresh( false );
   }
}


void kwxLCDDisplay::SetValue( wxString value )
{
    SDBG
	mValue = value;
	Refresh( false );
}


wxString kwxLCDDisplay::GetValue( void )
{
    SDBG
	return mValue;
}


int kwxLCDDisplay::GetNumberDigits( void )
{
    SDBG
	return mNumberDigits;
}


void kwxLCDDisplay::SetLightColour( wxColour c )
{
    SDBG
	mLightColour = c;
}


void kwxLCDDisplay::SetGrayColour( wxColour c )
{
    SDBG
	mGrayColour = c;
}


wxColour kwxLCDDisplay::GetLightColour( void )
{
    SDBG
	return mLightColour;
}


wxColour kwxLCDDisplay::GetGrayColour( void )
{
    SDBG
	return mGrayColour;
}


int kwxLCDDisplay::GetDigitsNeeded( wxString value )
{
    SDBG
	wxString tst = value;

	while( tst.Replace(_T("."),_T("")) );
	while( tst.Replace(_T(","),_T("")) );

	return tst.Len();
}


// The decoder function. The heart of the kwxLCDDisplay.


//      ***0***
//     *       *
//     1       2
//     *       *
//      ***6***
//     *       *  
//     3       4
//     *       *
//      ***5***

// A 10
// B 11
// C 12
// D 13
// E 14
// F 15

//     8421 8421
//     -654 3210
//---------------------
// 0 : 0011.1111 = 0x3F
// 1 : 0001.0100 = 0x14
// 2 : 0110.1101 = 0x6D
// 3 : 0111.0101 = 0x75
// 4 : 0101.0110 = 0x56
// 5 : 0111.0011 = 0x73
// 6 : 0111.1011 = 0x7B
// 7 : 0001,0101 = 0x15
// 8 : 0111.1111 = 0x7F
// 9 : 0111.0111 = 0x77
//   : 0000.0000 = 0x00
// - : 0100.0000 = 0x40

// r : 0100.1000 = 0x48
// o : 0111.1000 = 0x78 
// ^ : 0100.0111 = 0x47 //simbolo gradi

// A : 0101.1111 = 0x2F
// B : 0111.1010 = 0x7A
// C : 0010.1011 = 0x2B
// D : 0111.1100 = 0x7C
// E : 0110.1011 = 0x6B
// F : 0100.1011 = 0x4B

// H : 0101.1110 = 0x5E
// L : 0010.1010 = 0x2A
// P : 0100.1111 = 0x4F
// U : 0011.1110 = 0x3E


unsigned char kwxLCDDisplay::Decode( char c )
{
    SDBG
	unsigned char ret = 0;
	
	struct DecodedDisplay
	{
		char ch;
		unsigned char value;
	};

	DecodedDisplay dec[] = 
	{
		{ '0', 0x3F }, 
		{ '1', 0x14 },
		{ '2', 0x6D },
		{ '3', 0x75 },
		{ '4', 0x56 },
		{ '5', 0x73 },
		{ '6', 0x7B },
		{ '7', 0x15 },
		{ '8', 0x7F },
		{ '9', 0x77 },
//		{ ' ', 0x00 },
		{ '-', 0x40 },
/*		{ 'r', 0x48 },
		{ 'o', 0x78 },
		{ '^', 0x47 },
		{ 'A', 0x5F },
		{ 'B', 0x7A },
		{ 'C', 0x2B },
		{ 'D', 0x7C },
		{ 'E', 0x6B },
		{ 'F', 0x4B },
		{ 'H', 0x5E },*/
		{ 0, 0 }
	};

	for( int d = 0; dec[d].ch != 0; d++ )
	{
		if( dec[d].ch == c )
		{
			ret = dec[d].value;
			break;
		}
	}

	return ret;
}
