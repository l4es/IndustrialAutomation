#include "StdAfx.h"
#include ".\scriptreport.h"

CScriptReport::CScriptReport(ULONG ResID, CDocument& Doc, CEventsView* events)
	: _Document(Doc)
	, _EventsView(events)
{
	CString script;
	HRSRC hRes = FindResource(NULL, MAKEINTRESOURCE(ResID), _T("REPORT"));
	ATLASSERT(hRes != NULL);

	const DWORD Size = SizeofResource(NULL, hRes);
	HANDLE hData = LoadResource(NULL, hRes);
	if(hData && Size>0)
	{
		BYTE* ptr = (BYTE*)LockResource(hData);
		if(ptr)
			script = (LPCSTR)ptr;
	}

	_ReportData.Clear();
	if(!Parse((LPCWSTR)script, _ReportData))
	{
		ATLASSERT("Parsing error" == NULL);
		_ReportData.Clear();
	}

	for(vector<sBand>::iterator band = _ReportData.Bands.begin();band != _ReportData.Bands.end(); band++)
	{
		CString DataSource = band->DataSource.c_str();
		DataSource.MakeLower();
		map<CString, CDataSource*>::iterator it;
		it = _DataSource.find(DataSource);
		if(it != _DataSource.end())
			continue;
		_DataSource[DataSource] = _CreateDataSource(DataSource);
	}
}

CScriptReport::~CScriptReport(void)
{
	typedef map<CString, CDataSource*>::iterator it;
	for(it i=_DataSource.begin();i!=_DataSource.end();i++)
	{
		if(i->second)
			delete i->second;
	}
}

ULONG CScriptReport::GetPageCount(HDC hDC)
{
	typedef map<CString, CDataSource*>::iterator it;
	for(it i=_DataSource.begin();i!=_DataSource.end();i++)
	{
		if(i->second)
			i->second->First();
	}

	ULONG TotalPageCount=1;
	BOOL res = _PrintPage(TRUE, hDC, TRUE);
	while(res == TRUE)
	{
		res = _PrintPage(FALSE, hDC, TRUE);
		TotalPageCount++;
	}

	return TotalPageCount;
}

BOOL CScriptReport::_PrintPage(BOOL IsFirst, HDC hDC, BOOL Calc)
{
	CDCHandle dc(hDC);

	const int oldMapMode = dc.SetMapMode(MM_LOMETRIC);
	CSize Size(dc.GetDeviceCaps(PHYSICALWIDTH), dc.GetDeviceCaps(PHYSICALHEIGHT));
	dc.DPtoLP(&Size);
	CSize Offset(dc.GetDeviceCaps(PHYSICALOFFSETX), dc.GetDeviceCaps(PHYSICALOFFSETY));
	dc.DPtoLP(&Offset);

	_PageRect.left		= _ReportData.Marigin.Left*10 - Offset.cx;
	_PageRect.top		= -_ReportData.Marigin.Top*10 + Offset.cy;
	_PageRect.right		= Size.cx - _ReportData.Marigin.Right*10;
	_PageRect.bottom	= -(Size.cy - _ReportData.Marigin.Bottom*10);


	BOOL EndPage = FALSE;
	BOOL EndData = FALSE;
	if(IsFirst)
	{
		LONG StartY=_PageRect.top;
		for(vector<sBand>::iterator band = _ReportData.Bands.begin();band != _ReportData.Bands.end(); band++)
		{
			if(band->Type != sBand::BT_HEADER)
				continue;

			CDataSource *ds = _DataSource[band->DataSource.c_str()];
			LONG EndY = StartY-_GetBandHeight(hDC, *band, ds);
			if(EndY < _PageRect.bottom)
				EndY = _PageRect.bottom;
			if(!Calc)
				_PrintBand(hDC,*band,StartY,EndY, ds);
			StartY = EndY;
		}
		for(vector<sBand>::iterator band = _ReportData.Bands.begin();band != _ReportData.Bands.end(); band++)
		{
			if(band->Type != sBand::BT_DATA)
				continue;
			CDataSource *ds = _DataSource[band->DataSource.c_str()];
			if(ds == NULL)
				continue;
			const LONG Height = _GetBandHeight(hDC, *band, ds);
			do
			{
				LONG EndY = StartY-Height;
				if(EndY < _PageRect.bottom)
				{
					EndPage = TRUE;
					break;
				}
				if(!Calc)
					_PrintBand(hDC,*band,StartY,EndY,ds);
				StartY = EndY;
				ds->Next();
			}while(!ds->IsEOF());
			if(EndPage)
				break;
		}
		if(!EndPage)
		{
			for(vector<sBand>::iterator band = _ReportData.Bands.begin();band != _ReportData.Bands.end(); band++)
			{
				if(band->Type != sBand::BT_SUMMARY)
					continue;
				CDataSource *ds = _DataSource[band->DataSource.c_str()];
				const LONG Height = _GetBandHeight(hDC, *band, ds);

				LONG EndY = StartY-Height;
				if(EndY < _PageRect.bottom)
				{
					EndPage = TRUE;
					break;
				}

				if(!Calc)
					_PrintBand(hDC,*band,StartY,EndY,ds);				
				
				if(EndPage)
					break;
			}
		}
	}
	else
	{
		LONG StartY=_PageRect.top;
		for(vector<sBand>::iterator band = _ReportData.Bands.begin();band != _ReportData.Bands.end(); band++)
		{
			if(band->Type != sBand::BT_DATA)
				continue;
			CDataSource *ds = _DataSource[band->DataSource.c_str()];
			if(ds == NULL)
				continue;
			const LONG Height = _GetBandHeight(hDC, *band, ds);
			do
			{
				LONG EndY = StartY-Height;
				if(EndY < _PageRect.bottom)
				{
					EndPage = TRUE;
					break;
				}
				if(!Calc)
					_PrintBand(hDC,*band,StartY,EndY,ds);
				StartY = EndY;
				ds->Next();
			}while(!ds->IsEOF());
			EndData = ds->IsEOF();
			if(EndPage)
				break;
		}
		if(EndData)
		{
			for(vector<sBand>::iterator band = _ReportData.Bands.begin();band != _ReportData.Bands.end(); band++)
			{
				if(band->Type != sBand::BT_SUMMARY)
					continue;
				CDataSource *ds = _DataSource[band->DataSource.c_str()];
				const LONG Height = _GetBandHeight(hDC, *band, ds);

				LONG EndY = StartY-Height;
				if(EndY < _PageRect.bottom)
				{
					EndPage = TRUE;
					break;
				}
				if(!Calc)
					_PrintBand(hDC,*band,StartY,EndY,ds);				

				if(EndPage)
					break;
			}
		}
	}

	dc.SetMapMode(oldMapMode);

	return EndPage;
}

void CScriptReport::PrintPage(HDC hDC, UINT Page)
{
	typedef map<CString, CDataSource*>::iterator it;
	for(it i=_DataSource.begin();i!=_DataSource.end();i++)
	{
		if(i->second)
			i->second->First();
	}

	UINT tmp = 0;
	BOOL res;
	do 
	{
		res = _PrintPage(tmp==0, hDC, Page!=tmp);
	}while(res != FALSE && tmp++ != Page);
	
}

void CScriptReport::_PrintBand(HDC hDC, sBand& band, LONG StartY, LONG EndY, CDataSource* ds)
{
	CPoint BlockOffset(_PageRect.left, StartY);

	CDCHandle dc(hDC);
	const int oldBkMode = dc.SetBkMode(TRANSPARENT);

	for(vector<sLabel>::iterator label=band.Labels.begin();label!=band.Labels.end();label++)
	{
		CFont font;
		CFontHandle oldFont;
		if(font.CreateFontIndirect(&label->Font))
		{
			oldFont = dc.SelectFont(font);
			CSize sz;
			dc.GetTextExtent(label->Text.c_str(), -1, &sz);
			CRect rc(	BlockOffset.x+label->x*10,
						BlockOffset.y-label->y*10,
						_PageRect.right,
						BlockOffset.y-label->y*10-sz.cy);
			if(rc.top > EndY)
			{
				if(rc.bottom < EndY)
					rc.bottom = EndY;
                dc.DrawText(label->Text.c_str(), -1, rc, label->Align);
			}
			dc.SelectFont(oldFont);
		}
	}

	if(ds != NULL)
	{
		for(vector<sField>::iterator field=band.Fields.begin();field!=band.Fields.end();field++)
		{
			CFont font;
			CFontHandle oldFont;
			if(font.CreateFontIndirect(&field->Font))
			{
				oldFont = dc.SelectFont(font);

				CString text = ds->GetField(field->Data.c_str());
				CSize sz;
				dc.GetTextExtent(text, -1, &sz);
				CRect rc(	BlockOffset.x+field->x*10,
							BlockOffset.y-field->y*10,
							_PageRect.right,
							BlockOffset.y-field->y*10-sz.cy);
				if(field->Width > 0)
					rc.right = rc.left + field->Width*10;
				if(rc.top > EndY)
				{
					if(rc.bottom < EndY)
						rc.bottom = EndY;
					dc.DrawText(text, -1, rc, DT_LEFT);
				}
				dc.SelectFont(oldFont);
			}
		}
	}
	for(vector<sLine>::iterator line=band.Lines.begin();line!=band.Lines.end();line++)
	{
		CPen Pen;
		if(Pen.CreatePen(line->Style, line->Width, line->Color))
		{
			int y1 = BlockOffset.y - line->y1*10,
				y2 = BlockOffset.y - line->y2*10;
			if(y1 < EndY)
				y1 = EndY;
			if(y2 < EndY)
				y2 = EndY;
			CPenHandle oldPen = dc.SelectPen(Pen);
			dc.MoveTo(BlockOffset.x + line->x1*10, y1);
			dc.LineTo(BlockOffset.x + line->x2*10, y2);
			dc.SelectPen(oldPen);
		}
	}
	dc.SetBkMode(oldBkMode);
}

CDataSource* CScriptReport::_CreateDataSource(CString SourceName)
{
	SourceName.MakeLower();
	if(SourceName == "channels")
		return new CDSChannels(_Document);

	if(SourceName == "events")
	{
		if(_EventsView == NULL)
			return NULL;
		
		CTagDB::TVChannelValues Values;
		Values = _EventsView->GetEvents();
		return new CDSEvents(_Document, Values);
	}
	return NULL;
}

LONG CScriptReport::_GetBandHeight(HDC hDC, sBand& band, CDataSource *ds)
{
	LONG Height=0;

	CDCHandle dc(hDC);

	for(vector<sLabel>::iterator label=band.Labels.begin();label!=band.Labels.end();label++)
	{
		CFont font;
		CFontHandle oldFont;
		if(font.CreateFontIndirect(&label->Font))
		{
			oldFont = dc.SelectFont(font);
			CSize sz;
			dc.GetTextExtent(label->Text.c_str(), -1, &sz);
			dc.SelectFont(oldFont);

			Height = max(Height, abs(label->y*10+sz.cy));
		}
	}

	if(ds != NULL)
	{
		for(vector<sField>::iterator field=band.Fields.begin();field!=band.Fields.end();field++)
		{
			CFont font;
			CFontHandle oldFont;
			if(font.CreateFontIndirect(&field->Font))
			{
				oldFont = dc.SelectFont(font);

				CString text = ds->GetField(field->Data.c_str());
				CSize sz;
				dc.GetTextExtent(text, -1, &sz);
				dc.SelectFont(oldFont);

				Height = max(Height, abs(field->y*10+sz.cy));
			}
		}
	}
	for(vector<sLine>::iterator line=band.Lines.begin();line!=band.Lines.end();line++)
	{
		Height = max(Height, line->y1*10);
		Height = max(Height, line->y2*10);
	}
	return Height;
}