/**********************************************************************
--- Qt Architect generated file ---
File: Opc_client_daCommand.h
Last generated: Mon May 22 17:14:04 2000
*********************************************************************/
#ifndef Opc_client_daCommand_included
#define Opc_client_daCommand_included
#include "opc_client_daCommandData.h"

//NOTA BENE: questa dialog viene caricata
//dalla dll nel client di IndigoSCADA (ui.exe)

class Opc_client_daCommand : public Opc_client_daCommandData
{
	Q_OBJECT
	QString Unit_type; // receipe name
	QString samplePointName;
	enum {tUnit = 1
	//	, tSerial
	}; // transaction codes
	public:
	Opc_client_daCommand(
	QWidget *parent = NULL,
	const char *name = NULL,
	const char *unit_type = NULL );
	virtual ~Opc_client_daCommand();
	protected slots:
	virtual void Help();
	virtual void OkClicked();
	void QueryResponse (QObject *, const QString &, int, QObject*); // handles database responses
	void get_utc_host_time(struct cp56time2a* time);
};
#endif // Opc_client_daCommand_included

