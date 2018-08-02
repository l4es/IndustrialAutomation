/**********************************************************************
--- Qt Architect generated file ---
File: Opc_client_aeConfiguration.h
Last generated: Mon May 22 17:14:04 2000
*********************************************************************/
#ifndef Opc_client_aeConfiguration_included
#define Opc_client_aeConfiguration_included
#include "opc_client_aeConfigurationData.h"

class Opc_client_aeConfiguration : public Opc_client_aeConfigurationData
{
	Q_OBJECT
	QString Receipe; // receipe name
	enum {tItem = 1, tSerial}; // transaction codes
	public:
	Opc_client_aeConfiguration(
	QWidget *parent = NULL,
	const char *name = NULL,
	const char *receipe = NULL );
	virtual ~Opc_client_aeConfiguration();
	protected slots:
	virtual void Help();
	virtual void OkClicked();
	void QueryResponse (QObject *, const QString &, int, QObject*); // handles database responses
};
#endif // Opc_client_aeConfiguration_included

