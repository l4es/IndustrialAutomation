/**********************************************************************
--- Qt Architect generated file ---
File: Dnp3driverCommand.h
Last generated: Mon May 22 17:14:04 2000
*********************************************************************/
#ifndef Dnp3driverCommand_included
#define Dnp3driverCommand_included
#include "dnp3driverCommandData.h"

//NOTA BENE: questa dialog viene caricata
//dalla dll nel client di IndigoSCADA (ui.exe)

class Dnp3driverCommand : public Dnp3driverCommandData
{
	Q_OBJECT
	QString Unit_type; // receipe name
	QString samplePointName;
	enum {tUnit = 1
	//	, tSerial
	}; // transaction codes
	public:
	Dnp3driverCommand(
	QWidget *parent = NULL,
	const char *name = NULL,
	const char *unit_type = NULL );
	virtual ~Dnp3driverCommand();
	protected slots:
	virtual void Help();
	virtual void OkClicked();
	void QueryResponse (QObject *, const QString &, int, QObject*); // handles database responses
	void get_utc_host_time(struct cp56time2a* time);
};
#endif // Dnp3driverCommand_included

