/**********************************************************************
--- Qt Architect generated file ---
File: CalculatedInput.cpp
Last generated: Fri Sep 8 16:18:50 2000
*********************************************************************/
#include "Calculated.h"
#include "CalculatedInput.h"
#include "CalcEntryDlg.h"
#define Inherited CalculatedInputData
#include <qtextstream.h>
CalculatedInput::CalculatedInput
(
QWidget* parent,
const char* name
)
:
Inherited( parent, name )
{
}
CalculatedInput::~CalculatedInput()
{
}
void CalculatedInput::Expression()
{
	CalcEntryDlg dlg(this); // get the entry - this will have been validated and checked
	dlg.SetExpression(Expr->text());
	if(dlg.exec())
	{
		Expr->setText(dlg.GetExpression());
	};
}
void CalculatedInput::Load(const QString &s)// load the configuration
{
	#ifdef TRACE_ON
	cerr <<   "CalculatedInput::Load"  << endl;
	#endif
	if(GetConfigureDb()->GetNumberResults()) // it is a given that the correct properties record has been selected
	{ 
		QString s = GetConfigureDb()->GetString("DVAL");
		QTextIStream is (&s);
		Expr->setText(s);
	};
};
void CalculatedInput::Save(const QString &s)// save the configuration
{
	//
	#ifdef TRACE_ON
	cerr <<   "CalculatedInput::Save"  << endl;
	#endif
	// save from fields to properties
	// 
	QString cmd = "delete from PROPS where SKEY ='SAMPLEPROPS' and IKEY='"+s+"';";
	GetConfigureDb()->DoExec(0,cmd,0);
	//
	cmd = "insert into PROPS values('SAMPLEPROPS','"+s+"','"+Expr->text().stripWhiteSpace()+"');";
	GetConfigureDb()->DoExec(0,cmd,0);
	//
};

