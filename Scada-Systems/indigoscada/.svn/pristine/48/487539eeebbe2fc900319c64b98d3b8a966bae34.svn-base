/**********************************************************************
--- Qt Architect generated file ---
File: ReceipeSelect.cpp
Last generated: Fri May 19 09:40:23 2000
*********************************************************************/
#include "ReceipeSelect.h"
#define Inherited ReceipeSelectData
#include <qt.h>
#include "dbase.h"
#include "main.h"
ReceipeSelect::ReceipeSelect
(
QWidget* parent,
const char* name
)
:
Inherited( parent, name )
{
	setCaption(tr("Select Receipe"));
	connect(GetConfigureDb(),SIGNAL(TransactionDone(QObject *,const QString &, int, QObject*)),
	this,SLOT(QueryResponse(QObject *,const QString &,int, QObject*)));
	GetConfigureDb()->DoExec(this,"select NAME from RECEIPE;",tList);
	OkButton->setEnabled(false);
}
ReceipeSelect::~ReceipeSelect()
{
}
void ReceipeSelect::QueryResponse(QObject *p,const QString &,int id, QObject*caller) // response to database search
{
	if(p != this) return;
	switch(id)
	{
		case tList:
		{
			GetConfigureDb()->FillListBox(List,"NAME");
			OkButton->setEnabled(true);
			List->insertItem("(default)");
		};
		break;
		default:
		break;
	}; 
};

