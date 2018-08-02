/**********************************************************************
--- Qt Architect generated file ---
File: AlarmGroupCfg.h
Last generated: Fri Apr 14 08:57:03 2000
*********************************************************************/
#ifndef AlarmGroupCfg_included
#define AlarmGroupCfg_included
#include <qt.h>

#include "AlarmGroupCfgData.h"
#include <map>

class AlarmGroupCfg:public AlarmGroupCfgData
{ Q_OBJECT 
	typedef std::map< QString, bool, std::less < QString > >NameDict;
	NameDict SPD;		// sample poitn dictionary - cache the SP names on entry
	//
	QTimer *pTimer;
	enum
	{
		tList = 1, tItem, tDelete, tNew, tApply, tListSPs
	};
	//
	//
	void Build ();		// build a record for insert / update 
	void ButtonState (bool f)
	{ 
		ApplyButton->setEnabled (f);
		NewButton->setEnabled (f);
		DeleteButton->setEnabled (f);
	};
	void ClearSPD ()
	{
		// mark all entries as not selectedte  SP names on entry**************/
		NameDict::iterator i = SPD.begin ();
		for (; !(i == SPD.end()); i++)
		{
			(*i).second = false;
		};
	};
	//
	//
	public:
	AlarmGroupCfg ( QWidget * parent = NULL, const char *name = NULL);
	virtual ~ AlarmGroupCfg ();
	protected slots:virtual void Help ();
	virtual void Delete ();
	virtual void SelChanged (int);
	virtual void Apply ();
	virtual void New ();
	virtual void Add();
	virtual void Remove();
	void DoSelChange ();	// timer event to action change
	void QueryResponse (QObject *, const QString &, int, QObject*);
};
#endif // AlarmGroupCfg_included

