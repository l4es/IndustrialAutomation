/**********************************************************************
--- Qt Architect generated file ---
File: Iec103driverInput.h
Last generated: Mon May 22 17:08:25 2000
*********************************************************************/
#ifndef Iec103driverInput_included
#define Iec103driverInput_included
#include "iec103driverInputData.h"

class Iec103driverInput : public Iec103driverInputData
{
	Q_OBJECT
	public:
	enum  {  tConfigLoad = 1};
	Iec103driverInput(
	QWidget *parent = NULL,
	const char *name = NULL);
	virtual ~Iec103driverInput();

	QString Name;
	public slots:
	void Load(const QString &s);// load the configuration
	void Save(const QString &s);// save the configuration
};
#endif // Iec103driverInput_included

