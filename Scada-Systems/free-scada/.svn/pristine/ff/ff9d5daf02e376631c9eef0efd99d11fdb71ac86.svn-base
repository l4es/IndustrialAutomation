
#include <string>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

using namespace std;

namespace ScriptParser
{
	struct sLabel
	{
		long x,y;
		std::wstring Text;
		int Align;
		LOGFONT Font;

		sLabel()
		{
			x = y = NULL;
			Text = L"Text";
			Align = DT_LEFT;
			ZeroMemory(&Font, sizeof(LOGFONT));
			wcscpy(Font.lfFaceName, L"Times New Roman");
			Font.lfHeight			= 40;
			Font.lfWeight			= FW_NORMAL;
			Font.lfCharSet			= RUSSIAN_CHARSET;
			Font.lfOutPrecision		= OUT_DEFAULT_PRECIS;
			Font.lfClipPrecision	= CLIP_DEFAULT_PRECIS;
			Font.lfQuality			= DEFAULT_QUALITY;
			Font.lfPitchAndFamily	= DEFAULT_PITCH;
		}
	};

	struct sField
	{
		long x,y;
		long Width;
		std::wstring Data;
		LOGFONT Font;

		sField()
		{
			x = y = NULL;
			Width = 0;
			Data = L"Text";
			ZeroMemory(&Font, sizeof(LOGFONT));
			wcscpy(Font.lfFaceName, L"Times New Roman");
			Font.lfHeight			= 12;
			Font.lfWeight			= FW_NORMAL;
			Font.lfCharSet			= RUSSIAN_CHARSET;
			Font.lfOutPrecision		= OUT_DEFAULT_PRECIS;
			Font.lfClipPrecision	= CLIP_DEFAULT_PRECIS;
			Font.lfQuality			= DEFAULT_QUALITY;
			Font.lfPitchAndFamily	= DEFAULT_PITCH;
		}
	};

	struct sLine
	{
		long x1,y1,x2,y2;
		long Width;
		int Style;
		COLORREF Color;

		sLine()
		{
			x1 = y1 = x2 = y2 = 0;
			Width = 1;
			Style = PS_SOLID;
			Color = RGB(0,0,0);
		};
	};
	struct sBand
	{
		enum enBandType{BT_HEADER,BT_DATA,BT_SUMMARY};

		enBandType		Type;
		std::wstring			DataSource;

		vector<sLabel>	Labels;
		vector<sField>	Fields;
		vector<sLine>	Lines;
	};

	struct sReport
	{
		struct {
			int Left,Right,Top,Bottom;
		}Marigin;
		std::wstring			Name;
		std::wstring			Author;
		vector<sBand>	Bands;

		sReport()
		{
			Clear();
		};

		void Clear()
		{
			Bands.clear();
			Name.clear();
			Author.clear();
			Marigin.Left	= 30;
			Marigin.Top		= 15;
			Marigin.Bottom	= 15;
			Marigin.Right	= 15;
		};
	};

	extern bool Parse(std::wstring script, sReport& report);
};
