/**********************************************************************
--- Qt Architect generated file ---
File: Rfc1006driverConfiguration.h
Last generated: Mon May 22 17:14:04 2000
*********************************************************************/
#ifndef Rfc1006driverConfiguration_included
#define Rfc1006driverConfiguration_included
#include "rfc1006driverConfigurationData.h"

class Rfc1006driverConfiguration : public Rfc1006driverConfigurationData
{
	Q_OBJECT
	QString Receipe; // receipe name
	enum {tItem = 1, tSerial}; // transaction codes
	public:
	Rfc1006driverConfiguration(
	QWidget *parent = NULL,
	const char *name = NULL,
	const char *receipe = NULL );
	virtual ~Rfc1006driverConfiguration();
	protected slots:
	virtual void Help();
	virtual void OkClicked();
	void QueryResponse (QObject *, const QString &, int, QObject*); // handles database responses
};
#endif // Rfc1006driverConfiguration_included

