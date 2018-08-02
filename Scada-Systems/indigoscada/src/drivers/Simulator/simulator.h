/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2009 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */
/*
*Header For:simualtor driver
*Purpose:
*/

#ifndef include_simulator_hpp 
#define include_simulator_hpp
#include "driver.h"
#include "SimulatorUnit.h"
#include "SimulatorDigital.h"
#include "SimulatorAnalog.h"
#include "SimulatorCounts.h"
#include "sptypes.h"

#ifdef SIMULATOR_EXPORTS
#define SIMULATOR_API __declspec(dllexport)
#else
#define SIMULATOR_API //__declspec(dllimport)
#endif

class SimulatorInstance;
struct SimulatorSP // sample point in the simulator
{
	int type; // type of point 0 = analogue / 1 = counts
	int input;  // input index
	double mean; // mean value to genrate - for counts this is the smallest size band
	double sd;   // the variation - fairly crude
	int   interval; // 0 = not running
	int countdown; // count downtrack
	bool fFail; // failure flag - track changes
	SimulatorSP() : type(0),input(0),mean(0),sd(0),interval(0),countdown(0),fFail(0) 
	{};
	SimulatorSP(const SimulatorSP &s) : type(s.type),input(s.input),mean(s.mean),sd(s.sd),
	interval(s.interval),countdown(s.countdown) 
	{};
};
class SIMULATOR_API Simulator : public Driver // all this is private to this one file
{
	Q_OBJECT
	typedef std::map<QString,SimulatorInstance *, std::less<QString> > IDict;
	IDict Instances;
	enum
	{
		tListUnits = 1
	};
	public:
	Simulator(QObject *parent,const QString &name);
	~Simulator();
	//
	// user interface stuff
	// 
	void UnitConfigure(QWidget *parent, const QString &name, const QString &receipe); // configure a unit
	void SetTypeList(QComboBox *pCombo, const QString &unitname); // set the type list for unit type
	void GetInputList(const QString &type, QStringList &,const QString &unit, const QString &name); // set the permitted input IDs
	QWidget * GetSpecificConfig(QWidget *, const QString &spname, const QString &sptype); //specific config for sample point of type
	void GetTagList(const QString &type, QStringList &,const QString &,const QString &); // returns the permitted tags for a given type for this unit
	void CreateNewUnit(QWidget *parent, const QString &, int); // create a new unit - quick configure
	//
	// 
	static Simulator *pDriver; // only one instance shoudl be created
	//
	// Actual driver stuff
	//
	// these are slots - the declarations are virtual in Driver
	//  
	public slots:
	void Start(); // start everything under this driver's control
	void Stop(); // stop everything under this driver's control
	void Command(const QString &, BYTE, LPVOID, DWORD, DWORD); // process a command for a named unit 
	void QueryResponse (QObject *, const QString &, int, QObject*);
};
//
//
// *****************************************************************
// 
// 
class SIMULATOR_API SimulatorInstance : public DriverInstance // the unit interface
{
	Q_OBJECT
	enum
	{
		tList = 1, tTags, tProperties
	};
	typedef std::map<QString, SimulatorSP, std::less<QString> > SPdict; // dictionary of sample points
	SPdict Sps; // the sample points
	bool fFail; // unit failed flag
	//
	public:
	SimulatorInstance(Driver *parent, const QString &name) : 
	DriverInstance(parent,name) , fFail(0)
	{
		connect (GetConfigureDb (),
		SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
		SLOT (QueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the database
	};

	~SimulatorInstance()
	{
	};

	public slots:
	virtual void Start(); // start everything under this driver's control
	virtual void Stop(); // stop everything under this driver's control
	virtual void Command(const QString &, BYTE, LPVOID, DWORD, DWORD); // process a command for a named unit 
	virtual void QueryResponse (QObject *, const QString &, int, QObject*); // handles database responses
	void Tick(); // one second timer tick
};
#endif

