/**********************************************************************
--- Qt Architect generated file ---
File: TagCfg.h
Last generated: Thu Apr 13 16:29:45 2000
*********************************************************************/
#ifndef TagCfg_included
#define TagCfg_included
#include "TagCfgData.h"
class TagCfg:public TagCfgData
{ 
	Q_OBJECT 
	QTimer * pTimer;
	QString SPName;		 // the sample point name - first part of key
	QString Type;        // the sample point type   
	QString UnitType;    // the dll with the config stuff
	QString ReceipeName; // the name of the receipe 
	enum
	{
		tList = 1, tItem, tApply
	};
	//
	int State;			// the state
	void Build ();		// build a record for insert / update 
	void ButtonState (bool f)
	{ 
		ApplyButton->setEnabled (f);
	};
	public:
	TagCfg ( QWidget * parent, 
	const QString &spname, 
	const QString &type, 
	const QString &unittype,
	const QString &receipename = "(default)", 
	const QString &unitname = "",
	const char *name = NULL);
	virtual ~ TagCfg ();
	protected slots:
	virtual void Help ();
	virtual void Apply ();
	void SelChanged (int);
	void DoSelChange ();	// timer event to action change
	void QueryResponse (QObject *, const QString &, int, QObject*);
};
#endif // TagCfg_included

