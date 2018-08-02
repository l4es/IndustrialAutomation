/**********************************************************************
--- Qt Architect generated file ---
File: BatchCfg.h
Last generated: Thu Apr 13 17:34:58 2000
*********************************************************************/
#ifndef BatchCfg_included
#define BatchCfg_included
#include <qt.h>

#include "BatchCfgData.h"
class ReportGenerator;

class BatchCfg:public BatchCfgData
{ 
	Q_OBJECT QTimer * pTimer;
	enum
	{
		tList = 1, tItem, tDelete, tNew, tApply
	};
	//
	void Build ();		// build a record for insert / update 
	//
	void ButtonState (bool f)
	{ 
		ApplyButton->setEnabled (f);
		NewButton->setEnabled (f);
		DeleteButton->setEnabled (f);
	};

	////////////support for ReportGeneretor////////////////
	ReportGenerator *repgen;
	//////////////////////////////////////////////////////////

	public:
	BatchCfg ( QWidget * parent = NULL, const char *name = NULL);
	virtual ~ BatchCfg ();
	protected slots:
	virtual void Help ();
	virtual void Delete ();
	virtual void SelChanged (int);
	virtual void Apply ();
	virtual void Report ();
	virtual void New ();
	void QueryResponse (QObject *p, const QString &c, int State, QObject*);
	void DoSelChange ();
};
#endif // BatchCfg_included

