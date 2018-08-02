/**********************************************************************
--- Qt Architect generated file ---
File: UnitCfg.h
Last generated: Thu Apr 13 16:01:56 2000
*********************************************************************/
#ifndef UnitCfg_included
#define UnitCfg_included
#include <qt.h>
#include "UnitCfgData.h"
class UnitCfg:public UnitCfgData
{
	Q_OBJECT
	// *******************************************************************
	QTimer * pTimer;
	bool fNew; // try and force configure on new unit
	enum
	{
		tList = 1, tItem, tDelete, tNew, tApply, tDrop
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
	UnitCfg ( QWidget * parent = NULL, const char *name = NULL);
	virtual ~UnitCfg ();
	protected slots:
	virtual void Help ();
	virtual void Configure();
	virtual void Delete ();
	virtual void SelChanged (int);
	virtual void Apply ();
	virtual void New ();
	void QueryResponse (QObject *, const QString &, int, QObject*);
	void DoSelChange ();	// timer event to action change
};
#endif // UnitCfg_included

