/**********************************************************************
--- Qt Architect generated file ---
File: ReceipeSelect.h
Last generated: Fri May 19 09:40:23 2000
*********************************************************************/
#ifndef ReceipeSelect_included
#define ReceipeSelect_included
#include "ReceipeSelectData.h"
class ReceipeSelect : public ReceipeSelectData
{
	Q_OBJECT
	enum{tList = 1};
	public:
	ReceipeSelect(
	QWidget *parent = NULL,
	const char *name = NULL );
	virtual ~ReceipeSelect();
	public slots:
	void QueryResponse(QObject *,const QString &,int, QObject*);
};
#endif // ReceipeSelect_included

