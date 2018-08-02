/**********************************************************************
--- Qt Architect generated file ---
File: Iec104driverInput.h
Last generated: Mon May 22 17:08:25 2000
*********************************************************************/
#ifndef Iec104driverInput_included
#define Iec104driverInput_included
#include "iec104driverInputData.h"

class Iec104driverInput : public Iec104driverInputData
{
	Q_OBJECT
	public:
	enum  {  tConfigLoad = 1};
	Iec104driverInput(
	QWidget *parent = NULL,
	const char *name = NULL);
	virtual ~Iec104driverInput();

	QString Name;
	public slots:
	void Load(const QString &s);// load the configuration
	void Save(const QString &s);// save the configuration
};
#endif // Iec104driverInput_included

