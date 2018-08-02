/**********************************************************************
--- Qt Architect generated file ---
File: SimulatorCounts.h
Last generated: Sat Apr 22 11:10:31 2000
*********************************************************************/
#ifndef SimulatorCounts_included
#define SimulatorCounts_included
#include "SimulatorCountsData.h"
class SimulatorCounts : public SimulatorCountsData
{
	Q_OBJECT
	public:
	SimulatorCounts(
	QWidget *parent = NULL,
	const char *name = NULL );
	virtual ~SimulatorCounts();
	public slots:
	void Load(const QString &s);// load the configuration
	void Save(const QString &s);// save the configuration
};
#endif // SimulatorCounts_included

