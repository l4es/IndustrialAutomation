/**********************************************************************
--- Qt Architect generated file ---
File: SimulatorAnalog.h
Last generated: Sat Apr 22 11:04:57 2000
*********************************************************************/
#ifndef SimulatorAnalog_included
#define SimulatorAnalog_included
#include "SimulatorAnalogData.h"
#include "simulator.h"
class SimulatorAnalog : public SimulatorAnalogData
{
	Q_OBJECT
	public:
	SimulatorAnalog(
	QWidget *parent = NULL,
	const char *name = NULL );
	virtual ~SimulatorAnalog();
	public slots:
	void Load(const QString &s);// load the configuration
	void Save(const QString &s);// save the configuration
};
#endif // SimulatorAnalog_included

