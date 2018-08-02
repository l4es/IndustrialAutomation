/**********************************************************************
--- Qt Architect generated file ---
File: Iec101driverConfiguration.h
Last generated: Mon May 22 17:14:04 2000
*********************************************************************/
#ifndef Iec101driverConfiguration_included
#define Iec101driverConfiguration_included
#include "iec101driverConfigurationData.h"

class Iec101driverConfiguration : public Iec101driverConfigurationData
{
	Q_OBJECT
	QString Receipe; // receipe name
	enum {tItem = 1, tSerial}; // transaction codes
	public:
	Iec101driverConfiguration(
	QWidget *parent = NULL,
	const char *name = NULL,
	const char *receipe = NULL );
	virtual ~Iec101driverConfiguration();
	protected slots:
	virtual void Help();
	virtual void OkClicked();
	void QueryResponse (QObject *, const QString &, int, QObject*); // handles database responses
};
#endif // Iec101driverConfiguration_included

