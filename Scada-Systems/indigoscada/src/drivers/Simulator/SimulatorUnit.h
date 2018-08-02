/**********************************************************************
--- Qt Architect generated file ---
File: SimulatorUnit.h
Last generated: Sat Apr 22 10:59:12 2000
*********************************************************************/
#ifndef SimulatorUnit_included
#define SimulatorUnit_included
#include "simulator.h"
#include "SimulatorUnitData.h"
class SimulatorUnit : public SimulatorUnitData
{
	Q_OBJECT
	QString Name; // unit name
	enum {tItem = 1}; // transaction codes
	public:
	SimulatorUnit(
	QWidget *parent = NULL,
	const char *name = NULL );
	virtual ~SimulatorUnit();
	public slots:
	void QueryResponse (QObject *, const QString &, int, QObject*); // handles database responses
};
#endif // SimulatorUnit_included

