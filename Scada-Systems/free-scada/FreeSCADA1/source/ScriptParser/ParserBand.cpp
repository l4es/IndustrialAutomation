#include "stdafx.h"
#include ".\parserband.h"

namespace ScriptParser
{
	sBand* __CurrentBand;

	void fAssignBandType(LPCTSTR first, LPCTSTR last)
	{ 
		std::wstring str(first,last);
		to_lower(str);

		assert(__CurrentBand != NULL);
		if(str == L"theader")	__CurrentBand->Type = sBand::BT_HEADER;
		if(str == L"tdata")		__CurrentBand->Type = sBand::BT_DATA;
		if(str == L"tsummary")	__CurrentBand->Type = sBand::BT_SUMMARY;
	}

	namespace LabelFunc
	{
		inline void fAddNewLabel(LPCTSTR /*first*/, LPCTSTR /*last*/)
		{
			assert(__CurrentBand != NULL);
			__CurrentBand->Labels.push_back(sLabel());
		}

		inline void fSetLabelText(LPCTSTR first, LPCTSTR last)
		{
			std::wstring str = std::wstring(first,last);

			assert(__CurrentBand != NULL);
			if(!__CurrentBand->Labels.empty())
				__CurrentBand->Labels.rbegin()->Text = str;
		}

		inline void fSetLabelPosX(const int val)
		{
			assert(__CurrentBand != NULL);
			if(!__CurrentBand->Labels.empty())
				__CurrentBand->Labels.rbegin()->x = val;
		}

		inline void fSetLabelPosY(const int  val)
		{
			assert(__CurrentBand != NULL);
			if(!__CurrentBand->Labels.empty())
				__CurrentBand->Labels.rbegin()->y = val;
		}

		inline void fSetLabelFontName(LPCTSTR first, LPCTSTR last)
		{
			std::wstring str = std::wstring(first, last);

			assert(__CurrentBand != NULL);
			if(!__CurrentBand->Labels.empty())
				wcscpy(__CurrentBand->Labels.rbegin()->Font.lfFaceName, str.c_str());
		}
		
		inline void fSetLabelFontHeight(const int  val)
		{
			assert(__CurrentBand != NULL);
			if(!__CurrentBand->Labels.empty())
				__CurrentBand->Labels.rbegin()->Font.lfHeight = val;
		}

		inline void fSetLabelFontWeight(const int  val)
		{
			assert(__CurrentBand != NULL);
			if(!__CurrentBand->Labels.empty())
				__CurrentBand->Labels.rbegin()->Font.lfWeight = val;
		}

		inline void fSetLabelFontItalic(LPCTSTR first, LPCTSTR last)
		{
			std::wstring str = std::wstring(first,last);
			to_lower(str);
			assert(__CurrentBand != NULL);
			if(!__CurrentBand->Labels.empty())
			{
				if(str == L"true" || str == L"y" || str == L"1")
					__CurrentBand->Labels.rbegin()->Font.lfItalic = TRUE;
				else
					__CurrentBand->Labels.rbegin()->Font.lfItalic = FALSE;
			}
		}

		inline void fSetLabelFontUnderline(LPCTSTR first, LPCTSTR last)
		{
			std::wstring str = std::wstring(first,last);
			to_lower(str);
			assert(__CurrentBand != NULL);
			if(!__CurrentBand->Labels.empty())
			{
				if(str == L"true" || str == L"y" || str == L"1")
					__CurrentBand->Labels.rbegin()->Font.lfUnderline = TRUE;
				else
					__CurrentBand->Labels.rbegin()->Font.lfUnderline = FALSE;
			}
		}

		inline void fSetLabelAlign(LPCTSTR first, LPCTSTR last)
		{
			std::wstring str = std::wstring(first,last);
			to_lower(str);
			assert(__CurrentBand != NULL);
			if(!__CurrentBand->Labels.empty())
			{
				if(str == L"center")
					__CurrentBand->Labels.rbegin()->Align = DT_CENTER;
				else
					__CurrentBand->Labels.rbegin()->Align = DT_LEFT;
			}
		}
	}

	namespace FieldFunc
	{
		inline void fAddNewField(LPCTSTR /*first*/, LPCTSTR /*last*/)
		{
			assert(__CurrentBand != NULL);
			__CurrentBand->Fields.push_back(sField());
		}

		inline void fSetFieldData(LPCTSTR first, LPCTSTR last)
		{
			std::wstring str = std::wstring(first,last);

			assert(__CurrentBand != NULL);
			assert(!__CurrentBand->Fields.empty());
			if(!__CurrentBand->Fields.empty())
				__CurrentBand->Fields.rbegin()->Data = str;
		}

		inline void fSetFieldPosX(const int val)
		{
			assert(__CurrentBand != NULL);
			assert(!__CurrentBand->Fields.empty());
			if(!__CurrentBand->Fields.empty())
				__CurrentBand->Fields.rbegin()->x = val;
		}

		inline void fSetFieldPosY(const int  val)
		{
			assert(__CurrentBand != NULL);
			assert(!__CurrentBand->Fields.empty());
			if(!__CurrentBand->Fields.empty())
				__CurrentBand->Fields.rbegin()->y = val;
		}

		inline void fSetFieldWidth(const int val)
		{
			assert(__CurrentBand != NULL);
			assert(!__CurrentBand->Fields.empty());
			if(!__CurrentBand->Fields.empty())
				__CurrentBand->Fields.rbegin()->Width = val;
		}

		inline void fSetFieldFontName(LPCTSTR first, LPCTSTR last)
		{
			std::wstring str = std::wstring(first,last);

			assert(__CurrentBand != NULL);
			if(!__CurrentBand->Fields.empty())
				wcscpy(__CurrentBand->Fields.rbegin()->Font.lfFaceName, str.c_str());
		}

		inline void fSetFieldFontHeight(const int  val)
		{
			assert(__CurrentBand != NULL);
			if(!__CurrentBand->Fields.empty())
				__CurrentBand->Fields.rbegin()->Font.lfHeight = val;
		}

		inline void fSetFieldFontWeight(const int  val)
		{
			assert(__CurrentBand != NULL);
			if(!__CurrentBand->Fields.empty())
				__CurrentBand->Fields.rbegin()->Font.lfWeight = val;
		}

		inline void fSetFieldFontItalic(LPCTSTR first, LPCTSTR last)
		{
			std::wstring str = std::wstring(first,last);
			to_lower(str);
			assert(__CurrentBand != NULL);
			if(!__CurrentBand->Fields.empty())
			{
				if(str == L"true" || str == L"y" || str == L"1")
					__CurrentBand->Fields.rbegin()->Font.lfItalic = TRUE;
				else
					__CurrentBand->Fields.rbegin()->Font.lfItalic = FALSE;
			}
		}

		inline void fSetFieldFontUnderline(LPCTSTR first, LPCTSTR last)
		{
			std::wstring str = std::wstring(first,last);
			to_lower(str);
			assert(__CurrentBand != NULL);
			if(!__CurrentBand->Fields.empty())
			{
				if(str == L"true" || str == L"y" || str == L"1")
					__CurrentBand->Fields.rbegin()->Font.lfUnderline = TRUE;
				else
					__CurrentBand->Fields.rbegin()->Font.lfUnderline = FALSE;
			}
		}
	}

	namespace LineFunc
	{
		inline void fAddNewLine(LPCTSTR /*first*/, LPCTSTR /*last*/)
		{
			assert(__CurrentBand != NULL);
			__CurrentBand->Lines.push_back(sLine());
		}

		inline void fSetLineStyle(LPCTSTR first, LPCTSTR last)
		{
			std::wstring str = std::wstring(first,last);
			to_lower(str);

			assert(__CurrentBand != NULL);
			assert(!__CurrentBand->Lines.empty());
			if(!__CurrentBand->Lines.empty())
			{
				if(str == L"solid")		__CurrentBand->Lines.rbegin()->Style = PS_SOLID;
				if(str == L"dot")		__CurrentBand->Lines.rbegin()->Style = PS_DOT;
				if(str == L"dash")		__CurrentBand->Lines.rbegin()->Style = PS_DASH;
				if(str == L"dashdot")	__CurrentBand->Lines.rbegin()->Style = PS_DASHDOT;
				if(str == L"dashdotdot")	__CurrentBand->Lines.rbegin()->Style = PS_DASHDOTDOT;
			}
		}

		inline void fSetLineWidth(const int val)
		{
			assert(__CurrentBand != NULL);
			assert(!__CurrentBand->Lines.empty());
			if(!__CurrentBand->Lines.empty())
				__CurrentBand->Lines.rbegin()->Width = val;
		}

		inline void fSetLinePosX1(const int val)
		{
			assert(__CurrentBand != NULL);
			assert(!__CurrentBand->Lines.empty());
			if(!__CurrentBand->Lines.empty())
				__CurrentBand->Lines.rbegin()->x1 = val;
		}

		inline void fSetLinePosY1(const int  val)
		{
			assert(__CurrentBand != NULL);
			assert(!__CurrentBand->Lines.empty());
			if(!__CurrentBand->Lines.empty())
				__CurrentBand->Lines.rbegin()->y1 = val;
		}

		inline void fSetLinePosX2(const int val)
		{
			assert(__CurrentBand != NULL);
			assert(!__CurrentBand->Lines.empty());
			if(!__CurrentBand->Lines.empty())
				__CurrentBand->Lines.rbegin()->x2 = val;
		}

		inline void fSetLinePosY2(const int  val)
		{
			assert(__CurrentBand != NULL);
			if(!__CurrentBand->Lines.empty())
				__CurrentBand->Lines.rbegin()->y2 = val;
		}
		inline void fSetLineColor(const unsigned long  val)
		{
			assert(__CurrentBand != NULL);
			if(!__CurrentBand->Lines.empty())
				__CurrentBand->Lines.rbegin()->Color = val;
		}
	}

	//Grammar
	struct grammar_band : grammar<grammar_band>
	{
		template <typename ScannerT>
		struct definition
		{
			rule<ScannerT>	attribute,block,type_val;
			rule<ScannerT>	label,label_attribute,label_attr_font;
			rule<ScannerT>	field,field_attribute,field_attr_font;
			rule<ScannerT>	line,line_style,line_attribute;
			rule<ScannerT> const& start() const {return block;};
			definition(grammar_band const& /*self*/)
			{
				//Label
				using namespace LabelFunc;

				label_attr_font =
					as_lower_d[L"font"]>>L'='>>L'{'>>
						*((
							as_lower_d[L"name"]>>L'='>>confix_p(L'"', (*c_escape_ch_p)[&fSetLabelFontName], L'"') |
							as_lower_d[L"height"]>>L'='>>int_p[&fSetLabelFontHeight] |
							as_lower_d[L"weight"]>>L'='>>int_p[&fSetLabelFontWeight] |
							as_lower_d[L"italic"]>>L'='>>(*(anychar_p-L';'))[&fSetLabelFontItalic] |
							as_lower_d[L"underline"]>>L'='>>(*(anychar_p-L';'))[&fSetLabelFontUnderline]
						)>>L';')>>L'}';

				label_attribute =
					as_lower_d[L"align"]>>L'='>>(*(anychar_p-L';'))[&fSetLabelAlign] |
					as_lower_d[L"position.x"]>>L'='>>int_p[&fSetLabelPosX]	|
					as_lower_d[L"position.y"]>>L'='>>int_p[&fSetLabelPosY]	|
					as_lower_d[L"position"]>>L'='>>L'{'>>int_p[&fSetLabelPosX]>>L','>>int_p[&fSetLabelPosY]>>L'}' |
					as_lower_d[L"text"]>>L'='>>confix_p(L'"', (*c_escape_ch_p)[&fSetLabelText], L'"') |
					label_attr_font;

				label = 
					as_lower_d[L"label"][&fAddNewLabel]>>L'{'>>*(label_attribute>>L';')>>L'}';

				//Field
				using namespace FieldFunc;

				field_attr_font =
					as_lower_d[L"font"]>>L'='>>L'{'>>
					*((
						as_lower_d[L"name"]>>L'='>>confix_p('"', (*c_escape_ch_p)[&fSetFieldFontName], '"') |
						as_lower_d[L"height"]>>L'='>>int_p[&fSetFieldFontHeight] |
						as_lower_d[L"weight"]>>L'='>>int_p[&fSetFieldFontWeight] |
						as_lower_d[L"italic"]>>L'='>>(*(anychar_p-L';'))[&fSetFieldFontItalic] |
						as_lower_d[L"underline"]>>L'='>>(*(anychar_p-L';'))[&fSetFieldFontUnderline]
					)>>L';')>>L'}';

				field_attribute =
					as_lower_d[L"position.x"]>>L'='>>int_p[&fSetFieldPosX]	|
					as_lower_d[L"position.y"]>>L'='>>int_p[&fSetFieldPosY]	|
					as_lower_d[L"position"]>>L'='>>L'{'>>int_p[&fSetFieldPosX]>>','>>int_p[&fSetFieldPosY]>>L'}' |
					as_lower_d[L"data"]>>L'='>>confix_p('"', (*c_escape_ch_p)[&fSetFieldData], '"') |
					as_lower_d[L"width"]>>L'='>>int_p[&fSetFieldWidth]	|
					field_attr_font;

				field = 
					as_lower_d[L"field"][&fAddNewField]>>L'{'>>*(field_attribute>>L';')>>L'}';

				//Line
				using namespace LineFunc;

				line_style =
					as_lower_d[L"solid"] |
					as_lower_d[L"dot"] |
					as_lower_d[L"dash"] |
					as_lower_d[L"dashdot"] |
					as_lower_d[L"dashdotdot"];

				line_attribute =
					as_lower_d[L"x1"]>>L'='>>int_p[&fSetLinePosX1]	|
					as_lower_d[L"y1"]>>L'='>>int_p[&fSetLinePosY1]	|
					as_lower_d[L"x2"]>>L'='>>int_p[&fSetLinePosX2]	|
					as_lower_d[L"y2"]>>L'='>>int_p[&fSetLinePosY2]	|
					as_lower_d[L"position"]>>L'='>>L'{'>>
											int_p[&fSetLinePosX1]>>','>>
											int_p[&fSetLinePosY1]>>','>>
											int_p[&fSetLinePosX2]>>','>>
											int_p[&fSetLinePosY2]>>L'}' |
					as_lower_d[L"width"]>>L'='>>int_p[&fSetLineWidth]	|
					as_lower_d[L"style"]>>L'='>>line_style[&fSetLineStyle] |
					as_lower_d[L"color"]>>L'='>>hex_p[&fSetLineColor];

				line = 
					as_lower_d[L"line"][&fAddNewLine]>>L'{'>>*(line_attribute>>L';')>>L'}';

				//Атрибуты Band
				type_val =	
					as_lower_d[L"theader"] |
					as_lower_d[L"tdata"] |
					as_lower_d[L"tsummary"];

				attribute = 
					as_lower_d[L"datasource"]>>L'='>>confix_p(L'"', (*c_escape_ch_p)[fAssignString(__CurrentBand->DataSource,true)], L'"')	|
					as_lower_d[L"type"]>>L'='>>type_val[&fAssignBandType];

				block = *((attribute | label | field | line)>>L';');
			};
		};
	};

	void fParseBand::Parse(std::wstring BandBlock)
	{
		sBand band;

		__CurrentBand = &band;

		grammar_band grammar;
		parse_info<const wchar_t*> pi = parse(BandBlock.c_str(), grammar, space_p);
		_result = pi.full;
		_report.Bands.push_back(band);
		__CurrentBand = NULL;
	}
}