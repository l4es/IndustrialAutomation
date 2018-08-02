#pragma once
#include "Document.h"
#include "TagDB.h"

class CDataSource
{
public:
	CDataSource(void){};
	virtual ~CDataSource(void){};

	virtual void First()=0;
	virtual void Next()=0;
	virtual BOOL IsEOF()=0;
	virtual CString GetField(CString FieldName)=0;
};

class CDSChannels
	:public CDataSource
{
public:
	CDSChannels(CDocument& Doc);
	void First();
	void Next();
	BOOL IsEOF();
	CString GetField(CString FieldName);

protected:
	CDocument& _Document;
	CTagDB::TVChannelValues _Values;
	CTagDB::TVChannelValues::iterator _Iter; 
};

class CDSEvents
	:public CDataSource
{
public:
	CDSEvents(CDocument& Doc, CTagDB::TVChannelValues& Values);
	void First();
	void Next();
	BOOL IsEOF();
	CString GetField(CString FieldName);

protected:
	CDocument& _Document;
	CTagDB::TVChannelValues _Values;
	CTagDB::TVChannelValues::iterator _Iter; 
};