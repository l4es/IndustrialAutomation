#include "stdafx.h"
#include "parserreport.h"
#include "ParserBand.h"

namespace ScriptParser
{
	//Grammar
	struct grammar_report : grammar<grammar_report>
	{
		grammar_report(sReport& report, bool& result_): _report(report), _result(result_){};
		sReport& _report;
		bool& _result;

		template <typename ScannerT>
		struct definition
		{
			rule<ScannerT>	band,attribute,block;
			rule<ScannerT> const& start() const {return block;};
			definition(grammar_report const& self)
			{
				attribute = 
					(
						as_lower_d[L"name"] >> L'=' >> confix_p(L'"', (*c_escape_ch_p)[fAssignString(self._report.Name)], L'"') |
						as_lower_d[L"author"] >> L'=' >> confix_p(L'"', (*c_escape_ch_p)[fAssignString(self._report.Author)], L'"') |
						as_lower_d[L"left"] >> L'=' >> int_p[assign(self._report.Marigin.Left)] |
						as_lower_d[L"right"] >> L'=' >> int_p[assign(self._report.Marigin.Right)] |
						as_lower_d[L"top"] >> L'=' >> int_p[assign(self._report.Marigin.Top)] |
						as_lower_d[L"bottom"] >> L'=' >> int_p[assign(self._report.Marigin.Bottom)]
					)>>L';';

				band = (as_lower_d[L"band"] >> comment_nest_p(L'{',L'}')[fParseBand(self._report,self._result)]>>L';');

				block = *(attribute | band);
			};
		};
	};


	void fParseReport::Parse(std::wstring ReportBlock)
	{
		grammar_report grammar(_report,_result);
		parse_info<const wchar_t*> pi = parse(ReportBlock.c_str(), grammar, space_p);
		_result = pi.full;
	}
}