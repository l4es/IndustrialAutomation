/**********************************************************************
--- Qt Architect generated file ---
File: ActionConfigure.h
Last generated: Tue May 16 10:23:37 2000
*********************************************************************/
#ifndef ActionConfigure_included
#define ActionConfigure_included
#include "ActionConfigureData.h"
class ActionConfigure : public ActionConfigureData
{
	Q_OBJECT
	QString SpName; // sample point name
	enum { tItem = 1};
	public:
	ActionConfigure(
	QWidget *parent = NULL,
	const char *name = NULL );
	virtual ~ActionConfigure();
	protected slots:
	virtual void Help();
	virtual void OkClicked();
	void QueryResponse(QObject *p, const QString &, int State, QObject*);
};
#endif // ActionConfigure_included

