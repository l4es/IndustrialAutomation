/**********************************************************************
--- Qt Architect generated file ---
File: ScheduleCfg.h
Last generated: Thu Apr 20 09:35:35 2000
*********************************************************************/
#ifndef ScheduleCfg_included
#define ScheduleCfg_included
#include "ScheduleCfgData.h"
class ScheduleCfg:public ScheduleCfgData
{ Q_OBJECT QTimer * pTimer;
	enum
	{
		tList = 1, tItem, tDelete, tNew, tApply,
		tTagList, tReportList, tReceipeList, tSamplePointList
	};
	//
	int State;			// the state
	void Build ();		// build a record for insert / update 
	void ButtonState (bool f)
	{
		ApplyButton->setEnabled (f);
		NewButton->setEnabled (f);
		DeleteButton->setEnabled (f);
	};
	public:
	ScheduleCfg ( QWidget * parent = NULL, const char *name = NULL);
	virtual ~ ScheduleCfg ();
	//
	protected slots:
	//
	virtual void Help ();
	virtual void SpChanged (int);
	virtual void Delete ();
	virtual void SelChanged (int);
	virtual void Apply ();
	virtual void New ();
	virtual void ActionSetup ();
	virtual void FreqChange (int);
	virtual void ActionToggle (bool);
	virtual void EditReport ();
	virtual void EditReceipe ();
	//
	void QueryResponse (QObject *,const QString &, int, QObject*);
	void DoSelChange ();	// timer event to action change
	//
};
#endif // ScheduleCfg_included

