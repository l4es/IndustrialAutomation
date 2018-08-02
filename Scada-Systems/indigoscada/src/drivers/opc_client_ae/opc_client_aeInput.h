/**********************************************************************
--- Qt Architect generated file ---
File: Opc_client_aeInput.h
Last generated: Mon May 22 17:08:25 2000
*********************************************************************/
#ifndef Opc_client_aeInput_included
#define Opc_client_aeInput_included
#include "opc_client_aeInputData.h"

class Opc_client_ae_DriverThread;

class Opc_client_aeInput : public Opc_client_aeInputData
{
	Q_OBJECT
	public:
	enum  {  tConfigLoad = 1};
	Opc_client_aeInput(
	QWidget *parent = NULL,
	const char *name = NULL);
	virtual ~Opc_client_aeInput();

	QString Name;
	public slots:
	void Load(const QString &s);// load the configuration
	void Save(const QString &s);// save the configuration
};
#endif // Opc_client_aeInput_included

