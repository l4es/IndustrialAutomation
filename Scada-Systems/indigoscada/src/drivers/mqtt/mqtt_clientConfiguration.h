/**********************************************************************
--- Qt Architect generated file ---
File: MQTT_clientConfiguration.h
Last generated: Mon May 22 17:14:04 2000
*********************************************************************/
#ifndef MQTT_clientConfiguration_included
#define MQTT_clientConfiguration_included
#include "mqtt_clientConfigurationData.h"

class MQTT_clientConfiguration : public MQTT_clientConfigurationData
{
	Q_OBJECT
	QString Receipe; // receipe name
	enum {tItem = 1, tSerial}; // transaction codes
	public:
	MQTT_clientConfiguration(
	QWidget *parent = NULL,
	const char *name = NULL,
	const char *receipe = NULL );
	virtual ~MQTT_clientConfiguration();
	protected slots:
	virtual void Help();
	virtual void OkClicked();
	void QueryResponse (QObject *, const QString &, int, QObject*); // handles database responses
};
#endif // MQTT_clientConfiguration_included

