#pragma once


namespace ScriptParser
{
	struct fParseReport
	{
		fParseReport(sReport& report, bool& result): _report(report), _result(result){};
        
		void operator()(LPCWSTR first, LPCWSTR last) const
		{
			std::wstring str(first,last);
			trim_left_if(str,is_any_of(L" {}"));
			const_cast<fParseReport*>(this)->Parse(str);
		}
		
		void Parse(std::wstring ReportBlock);

        sReport& _report;
		bool& _result;
	};
}