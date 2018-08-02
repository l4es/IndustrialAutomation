/**********************************************************************
--- Qt Architect generated file ---
File: Modbus_driverConfiguration.h
Last generated: Mon May 22 17:14:04 2000
*********************************************************************/
#ifndef Modbus_driverConfiguration_included
#define Modbus_driverConfiguration_included
#include "modbus_driverConfigurationData.h"

class Modbus_driverConfiguration : public Modbus_driverConfigurationData
{
	Q_OBJECT
	QString Receipe; // receipe name
	enum {tItem = 1, tSerial}; // transaction codes
	enum {TCP = 0, RTU = 1}; // context codes
	int context;
	public:
	Modbus_driverConfiguration(
	QWidget *parent = NULL,
	const char *name = NULL,
	const char *receipe = NULL );
	virtual ~Modbus_driverConfiguration();
	protected slots:
	virtual void Help();
	virtual void OkClicked();
	virtual void RTUContextActive(bool);
    virtual void TCPContextActive(bool);
	void QueryResponse (QObject *, const QString &, int, QObject*); // handles database responses
};
#endif // Modbus_driverConfiguration_included

