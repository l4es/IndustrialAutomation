/**********************************************************************
--- Qt Architect generated file ---
File: Iec101driverInput.h
Last generated: Mon May 22 17:08:25 2000
*********************************************************************/
#ifndef Iec101driverInput_included
#define Iec101driverInput_included
#include "iec101driverInputData.h"

class Iec101driverInput : public Iec101driverInputData
{
	Q_OBJECT
	public:
	enum  {  tConfigLoad = 1};
	Iec101driverInput(
	QWidget *parent = NULL,
	const char *name = NULL);
	virtual ~Iec101driverInput();

	QString Name;
	public slots:
	void Load(const QString &s);// load the configuration
	void Save(const QString &s);// save the configuration
};
#endif // Iec101driverInput_included

