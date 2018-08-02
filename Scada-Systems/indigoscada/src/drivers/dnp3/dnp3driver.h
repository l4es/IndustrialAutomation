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
*Header For:DNP 3.0
*
*Purpose:
*/

#ifndef include_dnp3driver_h 
#define include_dnp3driver_h

#include "driver.h"
#include "dnp3driverConfiguration.h"
#include "dnp3driverInput.h"
#include "dnp3driverCommand.h"
#include "sptypes.h"
#include "smplstat.h"
#include "common.h"

#ifdef DNP_3_DRIVER_EXPORTS
#define DNP_3_DRIVERDRV __declspec(dllexport)
#else
#define DNP_3_DRIVERDRV //__declspec(dllimport)
#endif

class Dnp3driver_Instance;

struct InstanceCfg // friend to anyone
{
	QString DNP3ServerIPAddress;  // DNP3 server IP address (slave)
	QString DNP3ServerIPPort;  // DNP3 server TCP port
	QString IOA_AO;
	QString IOA_BO;
	QString IOA_CI;
	QString IOA_BI;
	QString IOA_AI;
	QString serverID;

	unsigned SampleTime; // sampleing time 

	InstanceCfg() : 
	SampleTime(1000),DNP3ServerIPAddress(""),DNP3ServerIPPort(""),
		IOA_AO(""),IOA_BO(""), IOA_CI(""), IOA_BI(""), IOA_AI(""),serverID("")
	{
	};

	InstanceCfg(const InstanceCfg &m) : 

	SampleTime(m.SampleTime),DNP3ServerIPAddress(m.DNP3ServerIPAddress),
	DNP3ServerIPPort(m.DNP3ServerIPPort),
	IOA_AO(m.IOA_AO),IOA_BO(m.IOA_BO), IOA_CI(m.IOA_CI), IOA_BI(m.IOA_BI), 
	IOA_AI(m.IOA_AI),serverID(m.serverID)
	{
	};
};


class DNP_3_DRIVERDRV Dnp3driver : public Driver // all this is private to this one file
{
	Q_OBJECT
	public:
	typedef std::map<QString,Dnp3driver_Instance *, std::less<QString> > IDict;
	IDict Instances;
	enum
	{
		tListUnits = 1, tcreateNewUnit
	};


	Dnp3driver(QObject *parent,const QString &name);
	~Dnp3driver();
	//
	// user interface stuff
	// 
	void UnitConfigure(QWidget *parent, const QString &name, const QString &receipe="(default)"); // configure a unit
	void SetTypeList(QComboBox *pCombo, const QString &unitname); // set the type list for unit type
	void GetInputList(const QString &type, QStringList &,const QString &unit, const QString &name); // set the permitted input IDs
	QWidget * GetSpecificConfig(QWidget *, const QString &spname, const QString &sptype); //specific config for sample point of type
	void GetTagList(const QString &type, QStringList &,const QString &,const QString &); // returns the permitted tags for a given type for this unit
	void CreateNewUnit(QWidget *parent, const QString &, int); // create a new unit - quick configure
	void CommandDlg(QWidget *parent, const QString &name); // command dialog

	//
	// 
	static Dnp3driver *pDriver; // only one instance should be created

	int n_iec_items;
	QString iec_unit_name;
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
#endif

