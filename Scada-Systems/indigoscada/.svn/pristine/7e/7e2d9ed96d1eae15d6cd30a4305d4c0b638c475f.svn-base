/**********************************************************************
--- Qt Architect generated file ---
File: MultiTraceDlg.h
Last generated: Wed Jun 28 10:28:28 2000
*********************************************************************/
#ifndef MultiTraceDlg_included
#define MultiTraceDlg_included
#include "MultiTraceDlgData.h"

#include <map>

class MultiTraceDlg : public MultiTraceDlgData
{
	Q_OBJECT
	enum { tList = 1};
	public:
	MultiTraceDlg(
	QWidget *parent = NULL,
	const char *name = NULL );
	virtual ~MultiTraceDlg();
	void Load(const QString &);
	void Save(const QString &);
	typedef std::map <QString, QCheckListItem *, std::less<QString> > ItemDict; // index to check items
	typedef std::map <QString, ItemDict, std::less<QString> > SpDict; // sample point to tags
	SpDict Items;
	public slots:
	void CurrentQueryResponse (QObject *,const QString &, int, QObject*); // current value responses
};
#endif // MultiTraceDlg_included

