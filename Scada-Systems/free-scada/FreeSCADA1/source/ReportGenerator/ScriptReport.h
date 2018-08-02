#include "Document.h"
#include "DataSource.h"
#include "EventsView.h"

#include <map>
using namespace std;

#pragma once

using namespace ScriptParser;

class CScriptReport
{
public:
	CScriptReport(ULONG ResID, CDocument& Doc,CEventsView* events = NULL);
	~CScriptReport(void);

	ULONG GetPageCount(HDC hDC);
	void PrintPage(HDC hDC, UINT Page);
	CString GetReportName(){return _ReportData.Name.c_str();};

protected:
	sReport _ReportData;
	CRect	_PageRect;
	CDocument& _Document;
	map<CString, CDataSource*> _DataSource;
	CEventsView* _EventsView;

	BOOL _PrintPage(BOOL IsFirst, HDC hDC, BOOL Calc=FALSE);
	void _PrintBand(HDC hDC, sBand& band, LONG StartY, LONG EndY, CDataSource* ds);
	LONG _GetBandHeight(HDC hDC, sBand& band, CDataSource *ds);
	CDataSource* _CreateDataSource(CString SourceName);	
};
