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
*Header For:RFC1006
*
*Purpose:
*/

#ifndef include_rfc1006driver_h 
#define include_rfc1006driver_h

#include "driver.h"
#include "rfc1006driverConfiguration.h"
#include "rfc1006driverInput.h"
#include "rfc1006driverCommand.h"
#include "sptypes.h"
#include "smplstat.h"
#include "common.h"

#ifdef RFC1006DRIVER_EXPORTS
#define RFC1006DRIVERDRV __declspec(dllexport)
#else
#define RFC1006DRIVERDRV //__declspec(dllimport)
#endif

class Rfc1006driver_Instance;

struct InstanceCfg // friend to anyone
{
	QString RFC1006ServerIPAddress;  // RFC1006 server IP address (slave)
	QString RFC1006ServerIPPort;  // RFC1006 server TCP port (102 is the default)
	QString RFC1006ServerSlot;  // RFC1006 server slot
	QString PLCAddress;  // PLC address

	InstanceCfg() : 
	RFC1006ServerIPAddress(""),RFC1006ServerIPPort(""),RFC1006ServerSlot(""),PLCAddress("")
	{
	};

	InstanceCfg(const InstanceCfg &m) : 
	RFC1006ServerIPAddress(m.RFC1006ServerIPAddress), RFC1006ServerIPPort(m.RFC1006ServerIPPort),
	RFC1006ServerSlot(m.RFC1006ServerSlot), PLCAddress(m.PLCAddress)
	{
	};
};


class RFC1006DRIVERDRV Rfc1006driver : public Driver // all this is private to this one file
{
	Q_OBJECT
	public:
	typedef std::map<QString,Rfc1006driver_Instance *, std::less<QString> > IDict;
	IDict Instances;
	enum
	{
		tListUnits = 1, tcreateNewUnit
	};


	Rfc1006driver(QObject *parent,const QString &name);
	~Rfc1006driver();
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
	static Rfc1006driver *pDriver; // only one instance should be created

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

