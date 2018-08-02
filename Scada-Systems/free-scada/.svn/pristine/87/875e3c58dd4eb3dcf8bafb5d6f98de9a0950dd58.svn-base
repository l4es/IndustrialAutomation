#include "stdafx.h"
#include "ParserReport.h"

namespace ScriptParser
{
	bool Parse(std::wstring script, sReport& report)
	{
		/*
		Очистить скрипт от комментариев
		*/
		bool result = false;
		rule<wide_phrase_scanner_t> r = (as_lower_d[L"report"] >> comment_nest_p(L'{',L'}')[fParseReport(report,result)])>>*ch_p(L';');	
		return parse(script.c_str(), r, space_p).full && result;
	}
}