/**********************************************************************
--- Qt Architect generated file ---
File: ReportSelect.h
Last generated: Wed May 17 15:58:58 2000
*********************************************************************/
#ifndef ReportSelect_included
#define ReportSelect_included
#include "ReportSelectData.h"
class ReportSelect : public ReportSelectData
{
	Q_OBJECT
	enum { tList = 1};
	public:
	ReportSelect(
	QWidget *parent = NULL,
	const char *name = NULL );
	virtual ~ReportSelect();
	public slots:
	void QueryResponse(QObject *p,const QString &,int id, QObject*);
};
#endif // ReportSelect_included

