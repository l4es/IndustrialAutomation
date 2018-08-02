/**********************************************************************
--- Qt Architect generated file ---
File: Iec104driverConfiguration.h
Last generated: Mon May 22 17:14:04 2000
*********************************************************************/
#ifndef Iec104driverConfiguration_included
#define Iec104driverConfiguration_included
#include "iec104driverConfigurationData.h"

class Iec104driverConfiguration : public Iec104driverConfigurationData
{
	Q_OBJECT
	QString Receipe; // receipe name
	enum {tItem = 1, tSerial}; // transaction codes
	public:
	Iec104driverConfiguration(
	QWidget *parent = NULL,
	const char *name = NULL,
	const char *receipe = NULL );
	virtual ~Iec104driverConfiguration();
	protected slots:
	virtual void Help();
	virtual void OkClicked();
	void QueryResponse (QObject *, const QString &, int, QObject*); // handles database responses
};
#endif // Iec104driverConfiguration_included

