/**********************************************************************
--- Qt Architect generated file ---
File: SampleCfg.h
Last generated: Thu Apr 13 16:15:49 2000
*********************************************************************/
#ifndef SampleCfg_included
#define SampleCfg_included
#include <qt.h>
#include "SampleCfgData.h"

class Driver;
class SampleCfg:public SampleCfgData
{ 
	Q_OBJECT
	//********************************************************************
	QTimer * pTimer;
	QWidget *pConfig;		// specific configuration widget
	Driver *pCfgDriver;     // instance of driver for configuration
	//
	QString tmp_ip_index; //the last fetch input index
	QString tmp_type; // the last fetch type
	QString unit_type; // the current unit type
	QStringList CurrentTypeList; // currently unused types
	//
	enum
	{
		tList = 1,
		tItem,
		tDelete,
		tNew,
		tApply,
		tUnitList,
		tDeleteTags,
		tDeleteProps,
		tUnitType,
		tTagList,
		tResultsTable,
		tIpIndex,
		tConfigLoad
	};
	//
	void Build ();		// build a record for insert / update 
	void ButtonState (bool f)
	{ 
		ApplyButton->setEnabled (f);
		NewButton->setEnabled (f);
		DeleteButton->setEnabled (f);
	};
	void ShowConfig();
	void RemoveConfig();
	//
	public:
	SampleCfg ( QWidget * parent = NULL, const char *name = NULL);
	virtual ~ SampleCfg ();
	protected slots:
	//
	virtual void Help ();
	virtual void SelChanged (int);
	virtual void Limits ();
	virtual void TypeChanged (int);
	virtual void Delete ();
	virtual void UnitChanged (int);
	virtual void Actions ();
	virtual void Apply ();
	virtual void New ();
	virtual void Changed(QListViewItem*);
	virtual void ListClicked(int, QListViewItem*, const QPoint &, int);
	virtual void Rename();
	void IpIndexChanged(int);
	//
	void DoSelChange ();	// timer event to action change
	void QueryResponse (QObject *,const QString &, int, QObject*);
	signals: 
	void LoadConfig (const QString &);	// signal to load the configuration
	void SaveConfig (const QString &);	// signal to save configuration
};
#endif // SampleCfg_included

