#include "StdAfx.h"
#include ".\datasource.h"


CDSChannels::CDSChannels(CDocument& Doc)
	:_Document(Doc)
{
	CTagDB::TVChannel Channels;

	vector<DWORD> IDs;
	for(CTagDB::TVChannel::iterator j=_Document.m_GraphChannels.begin();j!=_Document.m_GraphChannels.end();j++)
		IDs.push_back(j->id);
	Channels = _Document.m_DB.GetHumanReadableChannelsList();
	_Values = _Document.m_DB.GetValues(IDs, _Document.m_FromDateTime, _Document.m_ToDateTime);
	First();
}

void CDSChannels::First()
{
	_Iter = _Values.begin();
}

void CDSChannels::Next()
{
	if(_Iter != _Values.end())
		_Iter++;
}

BOOL CDSChannels::IsEOF()
{
	return _Iter == _Values.end();
}

CString CDSChannels::GetField(CString FieldName)
{
	FieldName.MakeLower();
	if(FieldName == "startdate")
		return _Document.m_FromDateTime.Format(_T("%Y.%m.%d %H:%M:%S"));
	if(FieldName == "enddate")
		return _Document.m_ToDateTime.Format(_T("%Y.%m.%d %H:%M:%S"));
	if(FieldName == "datenow")
	{
		COleDateTime dt = COleDateTime::GetCurrentTime();
		return dt.Format(_T("%Y.%m.%d %H:%M:%S"));
	}

	if(FieldName == "name")
		if(!IsEOF())
			return _Iter->Name;

	if(FieldName == "datetime")
		if(!IsEOF())
			return _Iter->DateTime.Format(_T("%m.%d %H:%M:%S"));
		
	if(FieldName == "value")
		if(!IsEOF())
		{
			CComVariant v = _Iter->Value;
			CString tmp;
			v.ChangeType(VT_BSTR);
			tmp = CString(v.bstrVal);
			return tmp.Mid(0, 6);
		}
	
	return "";
}

CDSEvents::CDSEvents(CDocument& Doc, CTagDB::TVChannelValues& Values)
:_Values(Values)
,_Document(Doc)
{
	First();
}

void CDSEvents::First()
{
	_Iter = _Values.begin();
}

void CDSEvents::Next()
{
	if(_Iter != _Values.end())
		_Iter++;
}

BOOL CDSEvents::IsEOF()
{
	return _Iter == _Values.end();
}

CString CDSEvents::GetField(CString FieldName)
{
	FieldName.MakeLower();
	if(FieldName == "startdate")
		return _Document.m_FromDateTime.Format(_T("%Y.%m.%d %H:%M:%S"));
	if(FieldName == "enddate")
		return _Document.m_ToDateTime.Format(_T("%Y.%m.%d %H:%M:%S"));
	if(FieldName == "datenow")
	{
		COleDateTime dt = COleDateTime::GetCurrentTime();
		return dt.Format(_T("%Y.%m.%d %H:%M:%S"));
	}

	if(FieldName == "name")
		if(!IsEOF())
			return _Iter->Name;

	if(FieldName == "datetime")
		if(!IsEOF())
			return _Iter->DateTime.Format(_T("%m.%d %H:%M:%S"));

	if(FieldName == "value")
		if(!IsEOF())
		{
			CComVariant v = _Iter->Value;
			CString tmp;
			v.ChangeType(VT_BSTR);
			tmp = CString(v.bstrVal);
			return tmp;
		}

		return "";
}