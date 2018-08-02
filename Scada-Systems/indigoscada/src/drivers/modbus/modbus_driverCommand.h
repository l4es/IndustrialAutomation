/**********************************************************************
--- Qt Architect generated file ---
File: Modbus_driverCommand.h
Last generated: Mon May 22 17:14:04 2000
*********************************************************************/
#ifndef Modbus_driverCommand_included
#define Modbus_driverCommand_included
#include "modbus_driverCommandData.h"

//NOTA BENE: questa dialog viene caricata
//dalla dll nel client di IndigoSCADA (ui.exe)

class Modbus_driverCommand : public Modbus_driverCommandData
{
	Q_OBJECT
	QString Unit_type; // receipe name
	QString samplePointName;
	enum {tUnit = 1
	//	, tSerial
	}; // transaction codes
	public:
	Modbus_driverCommand(
	QWidget *parent = NULL,
	const char *name = NULL,
	const char *unit_type = NULL );
	virtual ~Modbus_driverCommand();
	protected slots:
	virtual void Help();
	virtual void OkClicked();
	void QueryResponse (QObject *, const QString &, int, QObject*); // handles database responses
	void get_utc_host_time(struct cp56time2a* time);
};
#endif // Modbus_driverCommand_included

