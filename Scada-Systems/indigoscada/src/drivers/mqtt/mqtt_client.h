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
*Header For:MQTT client
*
*Purpose:
*/

#ifndef include_mqtt_client_h 
#define include_mqtt_client_h

#include "driver.h"
#include "mqtt_clientConfiguration.h"
#include "mqtt_clientInput.h"
#include "mqtt_clientCommand.h"
#include "sptypes.h"
#include "smplstat.h"
#include "common.h"

#ifdef MQTT_CLIENT_EXPORTS
#define MQTT_CLIENT_DRV __declspec(dllexport)
#else
#define MQTT_CLIENT_DRV //__declspec(dllimport)
#endif

class MQTT_client_Instance;

struct InstanceCfg // friend to anyone
{
	QString MqttBrokerHostName;   
	QString SubscribeTopicName;
	QString UserName;
	QString Password;
	QString Port;

	InstanceCfg() : 
	MqttBrokerHostName(""),SubscribeTopicName(""),
	UserName(""), Password(""), Port("")
	{
	};

	InstanceCfg(const InstanceCfg &m) : 

	MqttBrokerHostName(m.MqttBrokerHostName),
	SubscribeTopicName(m.SubscribeTopicName),UserName(m.UserName),
	Password(m.Password),Port(m.Port)
	{
	};
};


class MQTT_CLIENT_DRV MQTT_client : public Driver // all this is private to this one file
{
	Q_OBJECT
	public:
	typedef std::map<QString,MQTT_client_Instance *, std::less<QString> > IDict;
	IDict Instances;
	enum
	{
		tListUnits = 1, tcreateNewUnit
	};


	MQTT_client(QObject *parent,const QString &name);
	~MQTT_client();
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
	static MQTT_client *pDriver; // only one instance should be created

	int n_mqtt_items;
	QString mqtt_unit_name;
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

