/**********************************************************************
--- Qt Architect generated file ---
File: ReportSelect.cpp
Last generated: Wed May 17 15:58:58 2000
*********************************************************************/
#include <qt.h>
#include "ReportSelect.h"
#include "dbase.h"
#include "main.h"

#define Inherited ReportSelectData
ReportSelect::ReportSelect
(
QWidget* parent,
const char* name
)
:
Inherited( parent, name )
{
	setCaption(tr("Select Report"));
	connect(GetConfigureDb(),SIGNAL(TransactionDone(QObject *,const QString &, int, QObject*)),
	this,SLOT(QueryResponse(QObject *,const QString &,int, QObject*)));
	GetConfigureDb()->DoExec(this,"select NAME from REPORTS;",tList);
	OkButton->setEnabled(false);
}
ReportSelect::~ReportSelect()
{
}
// handle database responses
void ReportSelect::QueryResponse(QObject *p,const QString &,int id, QObject*caller) // response to database search
{
	if(p != this) return;
	switch(id)
	{
		case tList:
		{
			GetConfigureDb()->FillListBox(List,"NAME");
			OkButton->setEnabled(true);
		};
		break;
		default:
		break;
	}; 
};

