/**********************************************************************
--- Qt Architect generated file ---
File: ReportCfg.h
Last generated: Thu Apr 13 17:01:39 2000
*********************************************************************/
#ifndef ReportCfg_included
#define ReportCfg_included
#include <qt.h>
#include "ReportCfgData.h"
#include <map>

class ReportGenerator;

class ReportCfg:public ReportCfgData
{ Q_OBJECT 
	typedef std::map< QString, bool, std::less<QString> >NameDict;
	NameDict SPD;		// sample poitn dictionary - cache the SP names on entry
	//
	QTimer *pTimer;
	enum
	{
		tList = 1, tItem, tDelete, tNew, tApply, tListSPs
	};
	//
	void Build ();		// build a record for insert / update 
	void ButtonState (bool f)
	{ ApplyButton->setEnabled (f);
		NewButton->setEnabled (f);
		DeleteButton->setEnabled (f);
		MakeButton->setEnabled (f);
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

	////////////support for ReportGeneretor////////////////
	ReportGenerator *repgen;
	//////////////////////////////////////////////////////////
	public:
	ReportCfg (QWidget * parent = NULL, const char *name = NULL);
	virtual ~ ReportCfg ();
	protected slots:virtual void Help ();
	virtual void Delete ();
	virtual void ToModeChanged (int);
	virtual void FromModeChanged (int);
	virtual void SelChanged (int);
	virtual void MakeReport ();
	virtual void Apply ();
	virtual void New ();
	virtual void RemoveSP ();
	virtual void AddSP ();
	void DoSelChange ();	// timer event to action change
	void QueryResponse (QObject *, const QString &, int, QObject*);
};
#endif // ReportCfg_included

