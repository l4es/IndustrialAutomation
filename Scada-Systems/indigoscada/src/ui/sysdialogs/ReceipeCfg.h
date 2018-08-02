/**********************************************************************
--- Qt Architect generated file ---
File: ReceipeCfg.h
Last generated: Thu Apr 20 13:36:32 2000
*********************************************************************/
#ifndef ReceipeCfg_included
#define ReceipeCfg_included
#include <qt.h>
#include "common.h"
#include "ReceipeCfgData.h"
class ReceipeCfg:public ReceipeCfgData
{ Q_OBJECT QTimer * pTimer;
	enum
	{
		tList = 1, tItem, tDelete, tNew,
		tApply, tUnitList, tSampleList,tTags,tCfgTag,tUnitsType,tTagList,
		tTagEnabled,tTagItem,tUnchekTag
	};

	typedef std::map <QString, QCheckListItem *, std::less<QString> > ItemDict; // index of checkboxes 
	ItemDict spIdx;
	typedef std::map <QString, ItemDict, std::less<QString> > SpDict; // sample point to tags
	SpDict Items;

	//QWidgetStack* widgetStack;
	//
	//
	// temps for building queries
	QString sp_type;
	QString unit_type;
	QString sp_unit;
	//
	bool ApplyPressed;
		 
	void Build ();		// build a record for insert / update 
	void ButtonState (bool f)
	{	ApplyButton->setEnabled (f);
		NewButton->setEnabled (f);
		DeleteButton->setEnabled (f);
		ExitButton->setEnabled (f);
	};
	public:
	ReceipeCfg ( QWidget * parent = NULL, const char *name = NULL);
	virtual ~ ReceipeCfg ();

	protected slots:
	virtual void Help ();
	virtual void Delete ();
	virtual void SelChanged (int);
	virtual void Apply ();
	virtual void New ();
	virtual void ConfigureUnit ();
	virtual void AlarmLimits ();
	virtual void Exit();
	//
	void QueryResponse (QObject *, const QString &, int, QObject*) ;
	void DoSelChange ();
	virtual void ClearAll();
	virtual void SetAll();
	void DoTags();
	void DoUnit();
	//
	public slots:


	void Clicked(QListViewItem *);
	void returnPressed ( QListViewItem * );
	
};
#endif // ReceipeCfg_included

