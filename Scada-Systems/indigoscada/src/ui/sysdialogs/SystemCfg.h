/**********************************************************************
--- Qt Architect generated file ---
File: SystemCfg.h
Last generated: Thu Apr 13 15:02:26 2000
*********************************************************************/
#ifndef SystemCfg_included
#define SystemCfg_included
#include "SystemCfgData.h"
class SystemCfg:public SystemCfgData
{ 
	Q_OBJECT 
	
	enum
	{
		tGet = 1, tPut
	};
	public:
	SystemCfg (QWidget * parent = NULL, const char *name = NULL);
	virtual ~ SystemCfg ();
	void Help();
	protected slots:
	virtual void OkClicked ();
	void QueryResponse (QObject *, const QString &, int, QObject*);
	virtual void AutoLogoutToggled(bool);
};
#endif // SystemCfg_included

