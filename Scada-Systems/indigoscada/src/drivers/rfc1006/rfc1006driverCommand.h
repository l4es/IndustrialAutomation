/**********************************************************************
--- Qt Architect generated file ---
File: Rfc1006driverCommand.h
Last generated: Mon May 22 17:14:04 2000
*********************************************************************/
#ifndef Rfc1006driverCommand_included
#define Rfc1006driverCommand_included
#include "rfc1006driverCommandData.h"

//NOTA BENE: questa dialog viene caricata
//dalla dll nel client di IndigoSCADA (ui.exe)

class Rfc1006driverCommand : public Rfc1006driverCommandData
{
	Q_OBJECT
	QString Unit_type; // receipe name
	QString samplePointName;
	enum {tUnit = 1
	//	, tSerial
	}; // transaction codes
	public:
	Rfc1006driverCommand(
	QWidget *parent = NULL,
	const char *name = NULL,
	const char *unit_type = NULL );
	virtual ~Rfc1006driverCommand();
	protected slots:
	virtual void Help();
	virtual void OkClicked();
	void QueryResponse (QObject *, const QString &, int, QObject*); // handles database responses
	void get_utc_host_time(struct cp56time2a* time);
};
#endif // Rfc1006driverCommand_included

