/**********************************************************************
--- Qt Architect generated file ---
File: Iec61850driverConfiguration.h
Last generated: Mon May 22 17:14:04 2000
*********************************************************************/
#ifndef Iec61850driverConfiguration_included
#define Iec61850driverConfiguration_included
#include "iec61850driverConfigurationData.h"

class Iec61850driverConfiguration : public Iec61850driverConfigurationData
{
	Q_OBJECT
	QString Receipe; // receipe name
	enum {tItem = 1, tSerial}; // transaction codes
	enum {TCP = 0}; // context codes
	int context;
	public:
	Iec61850driverConfiguration(
	QWidget *parent = NULL,
	const char *name = NULL,
	const char *receipe = NULL );
	virtual ~Iec61850driverConfiguration();
	protected slots:
	virtual void Help();
	virtual void OkClicked();
	void QueryResponse (QObject *, const QString &, int, QObject*); // handles database responses
};
#endif // Iec61850driverConfiguration_included

