/**********************************************************************
--- Qt Architect generated file ---
File: UserCfgDlg.h
Last generated: Thu Apr 13 14:56:12 2000
*********************************************************************/
#ifndef UserCfgDlg_included
#define UserCfgDlg_included
#include <qt.h>
#include "UserCfgDlgData.h"


class UserCfgDlg:public UserCfgDlgData
{ 
	Q_OBJECT

	//********************************************************************/
	QTimer * pTimer;
	enum
	{
		tList = 1, tItem, tDelete, tNew, tApply
	};
	//
	void Build ();		// build a record for insert / update 
	void ButtonState (bool f)
	{ 
		ApplyButton->setEnabled (f);
		NewButton->setEnabled (f);
		DeleteButton->setEnabled (f);
	};
	//
	public:
	UserCfgDlg ( QWidget * parent = NULL, const char *name = NULL);
	virtual ~ UserCfgDlg ();
	protected slots:
	virtual void Help ();
	virtual void Delete ();
	virtual void SelChanged (int);
	virtual void Apply ();
	virtual void New ();
	virtual void AllChanged (bool);
	void QueryResponse (QObject *, const QString &, int, QObject*);
	void DoSelChange ();
};
#endif // UserCfgDlg_included

