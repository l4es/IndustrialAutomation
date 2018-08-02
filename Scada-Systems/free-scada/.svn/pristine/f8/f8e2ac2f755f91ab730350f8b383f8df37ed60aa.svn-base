#include "StdAfx.h"
#include ".\document.h"

CDocument::CDocument(void)
	: m_GraphicType(GT_STATIC)
	, m_IntervalDays(0)
	, m_IntervalHours(0)
	, m_IntervalMinutes(15)
	, m_ActiveView(AV_GRAPH)
{
	m_FromDateTime	= COleDateTime::GetCurrentTime();
	m_FromDateTime -= COleDateTimeSpan(1, 0, 0, 0);
	m_ToDateTime	= COleDateTime::GetCurrentTime();
	m_GraphChannels.clear();

	HelpersLib::CCfgReg reg;
	reg.Open(cProduct);
	m_ApproxDegree	= reg.ReadDWORD(L"ApproxDegree", 3);
	m_GraphicPoints	= reg.ReadDWORD(L"GraphicPoints", 100);
	m_ApproxMethod	= reg.ReadDWORD(L"ApproxMethod", 0);
}

CDocument::~CDocument(void)
{
	HelpersLib::CCfgReg reg;
	reg.Open(cProduct);
	reg.WriteDWORD(L"ApproxDegree",	m_ApproxDegree);
	reg.WriteDWORD(L"GraphicPoints",	m_GraphicPoints);
	reg.WriteDWORD(L"ApproxMethod",	m_ApproxMethod);
}