/**********************************************************************
--- Qt Architect generated file ---
File: SerialCfg.h
Last generated: Fri Apr 14 12:49:20 2000
*********************************************************************/
#ifndef SerialCfg_included
#define SerialCfg_included
//
#include <qt.h>
#include "SerialCfgData.h"
//
class SerialCfg:public SerialCfgData
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
	};
	public:
	SerialCfg ( QWidget * parent = NULL, const char *name = NULL);
	virtual ~ SerialCfg ();
	protected slots:
	virtual void Help ();
	virtual void SelChanged (int);
	virtual void Apply ();
	void QueryResponse (QObject *, const QString &, int, QObject*);
	void DoSelChange ();
};
#endif // SerialCfg_included

