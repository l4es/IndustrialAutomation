/**********************************************************************
--- Qt Architect generated file ---
File: MQTT_clientCommand.h
Last generated: Mon May 22 17:14:04 2000
*********************************************************************/
#ifndef MQTT_clientCommand_included
#define MQTT_clientCommand_included
#include "mqtt_clientCommandData.h"

//NOTA BENE: questa dialog viene caricata
//dalla dll nel client di IndigoSCADA (ui.exe)

class MQTT_clientCommand : public MQTT_clientCommandData
{
	Q_OBJECT
	QString Unit_type; // receipe name
	QString samplePointName;
	enum {tUnit = 1
	//	, tSerial
	}; // transaction codes
	public:
	MQTT_clientCommand(
	QWidget *parent = NULL,
	const char *name = NULL,
	const char *unit_type = NULL );
	virtual ~MQTT_clientCommand();
	protected slots:
	virtual void Help();
	virtual void OkClicked();
	void QueryResponse (QObject *, const QString &, int, QObject*); // handles database responses
	void get_utc_host_time(struct cp56time2a* time);
};
#endif // MQTT_clientCommand_included

