/**********************************************************************
--- Qt Architect generated file ---
File: Iec104driverCommand.h
Last generated: Mon May 22 17:14:04 2000
*********************************************************************/
#ifndef Iec104driverCommand_included
#define Iec104driverCommand_included
#include "iec104driverCommandData.h"

//NOTA BENE: questa dialog viene caricata
//dalla dll nel client di IndigoSCADA (ui.exe)

class Iec104driverCommand : public Iec104driverCommandData
{
	Q_OBJECT
	QString Unit_type; // receipe name
	QString samplePointName;
	enum {tUnit = 1
	//	, tSerial
	}; // transaction codes
	public:
	Iec104driverCommand(
	QWidget *parent = NULL,
	const char *name = NULL,
	const char *unit_type = NULL );
	virtual ~Iec104driverCommand();
	protected slots:
	virtual void Help();
	virtual void OkClicked();
	void QueryResponse (QObject *, const QString &, int, QObject*); // handles database responses
	void get_utc_host_time(struct cp56time2a* time);
};
#endif // Iec104driverCommand_included

