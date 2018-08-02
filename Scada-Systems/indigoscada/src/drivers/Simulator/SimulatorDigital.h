/**********************************************************************
--- Qt Architect generated file ---
File: SimulatorDigital.h
Last generated: Sat Apr 22 11:04:57 2000
*********************************************************************/
#ifndef SimulatorDigital_included
#define SimulatorDigital_included
#include "SimulatorDigitalData.h"
#include "simulator.h"
class SimulatorDigital : public SimulatorDigitalData
{
	Q_OBJECT
	public:
	SimulatorDigital(
	QWidget *parent = NULL,
	const char *name = NULL );
	virtual ~SimulatorDigital();
	public slots:
	void Load(const QString &s);// load the configuration
	void Save(const QString &s);// save the configuration
};
#endif // SimulatorDigital_included

