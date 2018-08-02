#pragma once

#include "TagDB.h"


class CDocument :
	public DVF::CBaseDocument
{
public:
	enum enGraphType
	{
		GT_STATIC	= 0,
		GT_DYNAMIC	= 1
	};
	enum enActiveView
	{
		AV_GRAPH,
		AV_TABLE,
		AV_EVENTS
	};

	enGraphType		m_GraphicType;
	CTagDB			m_DB;
	COleDateTime	m_FromDateTime;
	COleDateTime	m_ToDateTime;
	DWORD			m_IntervalDays;
	DWORD			m_IntervalHours;
	DWORD			m_IntervalMinutes;
	CTagDB::TVChannel	m_GraphChannels;
	UINT			m_GraphicPoints;
	UINT			m_ApproxDegree;
	UINT			m_ApproxMethod;

	enActiveView	m_ActiveView;	

	CDocument(void);
	~CDocument(void);
};