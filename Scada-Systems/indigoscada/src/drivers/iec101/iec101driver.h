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
*Header For:IEC 101
*
*Purpose:
*/

#ifndef include_iec101driver_h 
#define include_iec101driver_h

#include "driver.h"
#include "iec101driverConfiguration.h"
#include "iec101driverInput.h"
#include "iec101driverCommand.h"
#include "sptypes.h"
#include "smplstat.h"
#include "common.h"

#ifdef IEC_101_DRIVER_EXPORTS
#define IEC_101_DRIVERDRV __declspec(dllexport)
#else
#define IEC_101_DRIVERDRV //__declspec(dllimport)
#endif

class Iec101driver_Instance;

struct InstanceCfg // friend to anyone
{
	QString IEC101LinkAddress;  // IEC 101 Link address of slave
	QString IEC101CASDU;  // IEC 101 CASDU of slave
	QString COMPortName;  //serial port name
	unsigned int SampleTime;  // sampleing time in ms
	unsigned int read_timeout_ms; //serial read timeout in ms
	unsigned int baud_rate; //serial speed in bit/s

	InstanceCfg() : 
	SampleTime(1000),IEC101LinkAddress(""),IEC101CASDU(""),COMPortName(""),
	read_timeout_ms(1000),baud_rate(9600) 
	{
	};

	InstanceCfg(const InstanceCfg &m) : 

	SampleTime(m.SampleTime),IEC101LinkAddress(m.IEC101LinkAddress),
	IEC101CASDU(m.IEC101CASDU),COMPortName(m.COMPortName),
	read_timeout_ms(m.read_timeout_ms),baud_rate(m.baud_rate)
	{
	};
};


class IEC_101_DRIVERDRV Iec101driver : public Driver // all this is private to this one file
{
	Q_OBJECT
	public:
	typedef std::map<QString,Iec101driver_Instance *, std::less<QString> > IDict;
	IDict Instances;
	enum
	{
		tListUnits = 1, tcreateNewUnit
	};


	Iec101driver(QObject *parent,const QString &name);
	~Iec101driver();
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
	static Iec101driver *pDriver; // only one instance should be created

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

