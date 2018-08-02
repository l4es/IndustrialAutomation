/**********************************************************************
--- Qt Architect generated file ---
File: CalculatedInput.h
Last generated: Fri Sep 8 16:18:50 2000
*********************************************************************/
#ifndef CalculatedInput_included
#define CalculatedInput_included
#include "CalculatedInputData.h"
class CalculatedInput : public CalculatedInputData
{
	Q_OBJECT
	public:
	CalculatedInput(
	QWidget *parent = NULL,
	const char *name = NULL );
	virtual ~CalculatedInput();
	protected slots:
	virtual void Expression();
	void Load(const QString &s);// load the configuration
	void Save(const QString &s);// save the configuration
};
#endif // CalculatedInput_included

